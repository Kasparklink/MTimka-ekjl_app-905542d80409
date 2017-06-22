#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <pthread.h>
#include <termios.h>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>

#include "sendd.h"
#include "errors.h"
#include "globals.h"
#include "server.h"
#include "lprint.h"
#include "functions.h"

#include "../0include/rapidxml-1.13/rapidxml.hpp"
#include "../0include/rapidxml-1.13/rapidxml_print.hpp"
#include "../0include/rapidjson-1.1.0/document.h"
#include "../0include/rapidjson-1.1.0/stringbuffer.h"
#include "../0include/rapidjson-1.1.0/writer.h"

#define NUM_THREADS 512
pthread_t threads[NUM_THREADS];

std::string api_cookie;

int get_free_thread() {
	int ret = -1;
	for (int i = 0; i < NUM_THREADS; i++) {
		if (threads[i] == 0) {
			ret = i;
			break;
		}
	}

	return ret;
}

bool login() {
	// setting string where to load login.xml
	char *loginxmlpath = get_xml((char *)"login.xml");
	//printf("der: %s\n", loginxmlpath.c_str());

	// reading that login.xml into string
	std::ifstream t(loginxmlpath);
	std::stringstream buf;
	buf << t.rdbuf();
	std::string xml = buf.str();

	// posting that login.xml with few other params
	char *c = sendd::post(
			(char *)globals::api_host, globals::api_port, 
			(char *)"/webservice_login/api", (char *)"/webservice_login/api/Login", 
			(char *)xml.c_str()
	);
#if DEBUGD == true
	printf("%s\n", c);
#endif

	// splitting header and post data 2 difference pieces
        std::vector<std::string> split;
        boost::algorithm::split_regex(split, c, boost::regex("\r\n\r\n"));

	// getting something that we have to set cookie
        char *cookie = sendd::get_header_info((char *)split[0].c_str(), (char *)"Set-Cookie");
	if (cookie == NULL) return false;

        std::vector<std::string> cookies;
        boost::split(cookies, cookie, boost::is_any_of(";"));
	char *_athletics_session = (char *)cookies[0].c_str();
        //printf("%s\n", cookies[0].c_str());
	
	// parsing xml to get auth token
	rapidxml::xml_document<> doc;
	doc.parse<0>((char *)split[1].c_str());	
	
	rapidxml::xml_node<> *root_node = doc.first_node("env:Envelope");
	char *auth_token = root_node->first_node("env:Body")->first_node("n1:LoginResponse")->first_node("return")->value();
	//printf("%s\n", auth_token);

	// setting new api cookie to get future info
	api_cookie = "";
	api_cookie += std::string(_athletics_session) + "; ";
	api_cookie += "auth_token=" + std::string(auth_token) + "; ";
	api_cookie += "locale=et";

	return true;
}

char *get_club(rapidxml::xml_node<> *root_node, char *id) {
	for (rapidxml::xml_node<> *child = root_node->first_node("env:Body")->first_node("club"); child; child = child->next_sibling("club")) {
		if (strcmp((const char *)child->first_attribute("id")->value(), (const char *)id) == 0) {
			char *ret = (char *)malloc(strlen(child->value()) +1);
			memcpy(ret, child->value(), strlen(child->value()) +1);
			return ret;
		}
	}

	return (char *)"";
}

struct thread_data {
	int conn_desc;
	int i;
};

void *deal_with_client(void *v_td) {
	char *read;
	int conn_desc, i;

	conn_desc = ((thread_data *)v_td)->conn_desc;
	i = ((thread_data *)v_td)->i;
	free(v_td);

	for (;;) {
		read = server::get_data(conn_desc);
		if (read == 0) break;

		// removing new lines from read
		for (int j = strlen(read) -1; j > 0; j--) {
			if (read[j] == '\n' || read[j] == '\r')
				read[j] = '\x00';
			else break;
		}


		if (lprint::notice("got data:") == true)
			printf("%s\n", read);

		if (strcmp(read, "SZ_GET_COMPS") == 0) {
			// getting competitions	
			char *xmlpath = get_xml((char *)"getcompetitions.xml");
			
			std::ifstream t(xmlpath);
			std::stringstream buf;
			buf << t.rdbuf();
			std::string xml = buf.str();
			t.close();

			char *c = sendd::post(
					(char *)globals::api_host, globals::api_port, 
					(char *)"/webservice/api", (char *)"/webservice/api/GetCompetitions", 
					(char *)xml.c_str(), (char *)api_cookie.c_str()
			);
#if DEBUGD == true
			printf("%s\n", c);
#endif

			// splitting header and post data 2 difference pieces
			std::vector<std::string> split;
			boost::algorithm::split_regex(split, c, boost::regex("\r\n\r\n"));


			// creating json document
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value arr(rapidjson::kArrayType);
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
				
			// parsing xml to get info and put it into json doc
			rapidxml::xml_document<> xml_doc;
			xml_doc.parse<0>((char *)split[1].c_str());
			
			// getting GetCompetitioinsResponse node
			rapidxml::xml_node<> *root_node = xml_doc.first_node("env:Envelope");
			rapidxml::xml_node<> *compsres_node = root_node->first_node("env:Body")->first_node("n1:GetCompetitionsResponse")->first_node("return");

			// parsing items
			for (rapidxml::xml_node<> *child = compsres_node->first_node("item"); child; child = child->next_sibling("item")) {
				char *id = child->first_node("id")->value();
				char *name = child->first_node("name")->value();


				rapidjson::Value item;
				item.SetObject();
				item.AddMember("id", rapidjson::StringRef(id, strlen(id)), allocator);
				item.AddMember("name", rapidjson::StringRef(name, strlen(name)), allocator);
				arr.PushBack(item, allocator); 
			}

			doc.AddMember("items", arr, allocator);
			rapidjson::StringBuffer strbuf;
			rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
			doc.Accept(writer);

			const char *jsonString = strbuf.GetString();
#if DEBUGD == true
			if (lprint::notice("sending competitions:") == true)
				printf("%s\n", jsonString);
#endif

			server::send_data(conn_desc, (char *)jsonString);
			free(c);
		} else if (starts_with(read, "SZ_GET_TIMETABLE") == true) {
			std::vector<std::string> read_split;
			boost::split(read_split, read, boost::is_any_of("="));

			// getting competitions	
			std::string xmlpath = get_xml((char *)"getallcompetitionsheats.xml");
			printf("path: %s\n", xmlpath.c_str());

			std::ifstream t(xmlpath.c_str());
			std::stringstream buf;
			buf << t.rdbuf();
			std::string xml = buf.str();
			t.close();

			boost::replace_all(xml, "competitionId", read_split[1].c_str());

			char *c = sendd::post(
					(char *)globals::api_host, globals::api_port, 
					(char *)"/webservice/api", (char *)"/webservice/api/GetAllCompetitionHeats", 
					(char *)xml.c_str(), (char *)api_cookie.c_str()
			);
#if DEBUGD == true
			printf("%s\n", c);
#endif

			// splitting header and post data 2 difference pieces
			std::vector<std::string> split;
			boost::algorithm::split_regex(split, c, boost::regex("\r\n\r\n"));


			// creating json document
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value arr(rapidjson::kArrayType);
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
				
			// parsing xml to get info and put it into json doc
			rapidxml::xml_document<> xml_doc;
			xml_doc.parse<0>((char *)split[1].c_str());

			// getting GetAllCompetitioinsHeatsResponse node
			rapidxml::xml_node<> *root_node = xml_doc.first_node("env:Envelope");
			rapidxml::xml_node<> *compsres_node = root_node->first_node("env:Body")->first_node("n1:GetAllCompetitionHeatsResponse")->first_node("return");

			// parsing itemsi
			for (rapidxml::xml_node<> *child = compsres_node->first_node("item"); child; child = child->next_sibling("item")) {
				char *id = child->first_node("id")->value();
				char *long_name = child->first_node("long_name")->value();
				char *starting_time = child->first_node("starting_time")->value();


				rapidjson::Value item;
				item.SetObject();
				item.AddMember("id", rapidjson::StringRef(id, strlen(id)), allocator);
				item.AddMember("long_name", rapidjson::StringRef(long_name, strlen(long_name)), allocator);
				item.AddMember("starting_time", rapidjson::StringRef(starting_time, strlen(starting_time)), allocator);
				arr.PushBack(item, allocator); 
			}

			doc.AddMember("items", arr, allocator);
			rapidjson::StringBuffer strbuf;
			rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
			doc.Accept(writer);

			const char *jsonString = strbuf.GetString();
			
#if DEBUGD == true
			if (lprint::notice("sending all competition heats:") == true)
				printf("%s\n", jsonString);
#endif

			server::send_data(conn_desc, (char *)jsonString);
			free(c);
		} else if (starts_with(read, "SZ_GET_RESULTS=") == true) {
			std::vector<std::string> read_split;
			boost::split(read_split, read, boost::is_any_of("="));

			// getting competitions	
			std::string xmlpath = get_xml((char *)"getheatstate.xml");
			printf("path: %s\n", xmlpath.c_str());

			std::ifstream t(xmlpath.c_str());
			std::stringstream buf;
			buf << t.rdbuf();
			std::string xml = buf.str();
			t.close();

			boost::replace_all(xml, "heatId", read_split[1].c_str());

			char *c = sendd::post(
					(char *)globals::api_host, globals::api_port, 
					(char *)"/webservice/api", (char *)"/webservice/api/GetHeatState", 
					(char *)xml.c_str(), (char *)api_cookie.c_str()
			);
#if DEBUGD == true
			printf("%s\n", c);
#endif

			// splitting header and post data 2 difference pieces
			std::vector<std::string> split;
			boost::algorithm::split_regex(split, c, boost::regex("\r\n\r\n"));


			// creating json document
			rapidjson::Document doc;
			doc.SetObject();

			rapidjson::Value arr(rapidjson::kArrayType);
			rapidjson::Value attmtarr(rapidjson::kArrayType);
			rapidjson::Document::AllocatorType& allocator = doc.GetAllocator();
				
			// parsing xml to get info and put it into json doc
			rapidxml::xml_document<> xml_doc;
			xml_doc.parse<0>((char *)split[1].c_str());

			// getting GetAllCompetitioinsHeatsResponse node
			rapidxml::xml_node<> *root_node = xml_doc.first_node("env:Envelope");
			rapidxml::xml_node<> *compsres_node = root_node->first_node("env:Body")->first_node("n1:GetHeatStateResponse")->first_node("return");
			
			// parsing attempt names
			for (rapidxml::xml_node<> *child = compsres_node->first_node("attempt_names")->first_node("item"); child; child = child->next_sibling("item")) {
				char *item = child->value();

				rapidjson::Value itm;
				itm.SetObject();
				itm.AddMember("item", rapidjson::StringRef(item, strlen(item)), allocator);
				attmtarr.PushBack(itm, allocator);
			}

			// parsing itemsi
			for (rapidxml::xml_node<> *child = compsres_node->first_node("athletes")->first_node("item"); child; child = child->next_sibling("item")) {
				char *rank = child->first_node("rank")->value();
				char *first_name = child->first_node("first_name")->value();
				char *last_name = child->first_node("last_name")->value();
				char *club;
				if (!child->first_node("club")->first_attribute("href"))
					club = (char *)"";
				else club = get_club(root_node, (char *)(child->first_node("club")->first_attribute("href")->value() +1));
				char *best_result = child->first_node("best_result")->value();

				rapidjson::Value resultsarr(rapidjson::kArrayType);

				for (rapidxml::xml_node<> *chld = child->first_node("results")->first_node("item"); chld; chld = chld->next_sibling("item")) {
					if (!chld->first_attribute("xsi:nil")) {
						char *result_string = chld->first_node("result_string")->value();
						char *attempt_number = chld->first_node("attempt_number")->value();

						rapidjson::Value itm;
						itm.SetObject();
						itm.AddMember("result_string", rapidjson::StringRef(result_string, strlen(result_string)), allocator);
						itm.AddMember("attempt_number", rapidjson::StringRef(attempt_number, strlen(attempt_number)), allocator);
						resultsarr.PushBack(itm, allocator);
					}
				}

				rapidjson::Value item;
				item.SetObject();
				item.AddMember("rank", rapidjson::StringRef(rank, strlen(rank)), allocator);
				item.AddMember("first_name", rapidjson::StringRef(first_name, strlen(first_name)), allocator);
				item.AddMember("last_name", rapidjson::StringRef(last_name, strlen(last_name)), allocator);
				item.AddMember("club", rapidjson::StringRef(club, strlen(club)), allocator);
				item.AddMember("best_result", rapidjson::StringRef(best_result, strlen(best_result)), allocator);
				item.AddMember("results", resultsarr, allocator);
				arr.PushBack(item, allocator);
			}

			doc.AddMember("attempt_names", attmtarr, allocator);
			doc.AddMember("items", arr, allocator);
			rapidjson::StringBuffer strbuf;
			rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
			doc.Accept(writer);

			const char *jsonString = strbuf.GetString();
			
#if DEBUGD == true
			if (lprint::notice("sending heat states:") == true)
				printf("%s\n", jsonString);
#endif

			server::send_data(conn_desc, (char *)jsonString);
			free(c);
		}

		free(read);
	}

	lprint::notice("closing client");

	close(conn_desc);
	threads[i] = 0;
	pthread_exit(NULL);
}

void *get_clients(void *v_td) {
	int conn_desc, i, free_spot;

	i = ((thread_data *)v_td)->i;
	free(v_td);

	for (;;) {
		conn_desc = server::get_connection();
		free_spot = get_free_thread();

		thread_data *v_td = new thread_data();
		v_td->conn_desc = conn_desc;
		v_td->i = free_spot;

		pthread_create(&threads[free_spot], NULL, deal_with_client, (void*)v_td);
		lprint::notice("got connection");
	}

	threads[i] = 0;
	pthread_exit(NULL);
}

int main(int argc, char **argv) {
	if (argv[1]) {
		printf("using ip: %s\n", argv[1]);
		globals::ip = argv[1];
	}

	for (int i = 0; i < NUM_THREADS; i++)
		threads[i] = 0;

	if (login() == true) {
		lprint::good("login success");


		if (server::set_up((char *)globals::ip, globals::port)) {
			lprint::good("server setup completed");
			
			thread_data *v_td = new thread_data();
			int i = get_free_thread();
			v_td->i = i;

			pthread_create(&threads[v_td->i], NULL, get_clients, (void *)v_td);
			
			for (;;) {
				if (getch() == ':') {
					putchar(':');
					
					char cmd[256];
					scanf("%s", (char *)&cmd);

					if (strcmp(cmd, "exit") == 0 || strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0)
						break;
				}
			}

			server::closee();
		} else err::basic("failed to set up server");
	} else lprint::bad("login failed");

	return 0;
}
