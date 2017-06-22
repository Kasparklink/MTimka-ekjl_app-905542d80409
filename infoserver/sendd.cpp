#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/regex.hpp>

#include "sendd.h"
#include "errors.h"
#include "globals.h"
#include "lprint.h"

std::string to_string(char *chrarr) {
	std::stringstream ss;
	ss << chrarr;
	return ss.str();
}

std::string to_string(int num) {
	std::stringstream ss;
	ss << num;
	return ss.str();
}

char *sendd::post(char *host, int port, char *path, char *SOAPAction, char *data, char *cookie) {
	std::string s_message;
	s_message += "POST " + to_string(path) + " HTTP/1.1\r\n";
    s_message += "User-Agent: Mozilla/4.0 (compatible; MSIE 6.0; MS Web Services Client Protocol 4.0.30319.42000)\r\n";
    s_message += "VsDebuggerCausalityData: uIDPo39pNr/2bhJHlHSYoSiSxwwAAAAASt05AL9p+E+ahTWfkkpofSscuus338VAhi4gAOEesrwACQAA\r\n";
    s_message += "Content-Type: text/xml; charset=utf-8\r\n";
    s_message += "SOAPAction: SOAPAction: \"" + to_string(SOAPAction) + "\"\r\n";
    s_message += "Host: " + to_string(host) + "\r\n";
	if (cookie != 0) s_message += "Cookie: " + to_string(cookie) + "\r\n";
    s_message += "Content-Length: " + to_string(strlen(data)) + "\r\n";
    s_message += "Connection: Keep-Alive\r\n\r\n";
	s_message += to_string(data) + "\r\n";

	struct hostent *server;
	struct sockaddr_in serv_addr;
	int sockfd, bytes, sent, received, total;
	char *message, response[16384];

	// fill message
	message = (char *)malloc(s_message.length());
	//sprintf(message, s_message.c_str());
	memcpy(message, s_message.c_str(), s_message.length());

	// creating socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) err::basic("ERROR opening socket");

	// lookup ip address
	server = gethostbyname(host);
	if (server == NULL) err::basic("ERROR failed to look up host");

	// fill in the structure
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(host);
	serv_addr.sin_port = htons(port);

	// connect socket
	if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		err::basic("sendd.cpp -- ERROR connecting");

	// send request
	total = strlen(message);
	sent = 0;

	while (sent < total) {
		bytes = write(sockfd, message + sent, total - sent);

		if (bytes < 0) err::basic("ERROR writing message to socket");
			if (bytes == 0) break;

		sent += bytes;
	}

	// receive response
    memset(response, 0, sizeof(response));
	std::string retme;
    received = 0;

	while (true) {
		bytes = read(sockfd, response, sizeof(response));

		if (bytes < 0) err::basic("ERROR reading response from socket");
		if (bytes == 0) break;

		retme += std::string(response);
	    received += bytes;
		
#if FAST_READ_EKJL == true
		if (bytes > 15) {
			if (strcmp((char *)(response + bytes -15), "</env:Envelope>") == 0) {
				lprint::notice("sendd.cpp -- using fast break");
				break;
			}
		}
		memset(response, 0, bytes);
#endif
	}

	// close socket
	close(sockfd);

	char *ret = (char *)malloc(retme.length() +1);
	memcpy(ret, retme.c_str(), retme.length() +1);
	return ret;
}

char *sendd::get_header_info(char *header, char *tag) {
	std::vector<std::string> lines;
	boost::algorithm::split_regex(lines, header, boost::regex("\r\n"));

	for (int i = 0; i < lines.size(); i++) {
		if (boost::starts_with(lines[i], tag)) {
			int size = lines[i].length() +1 - (strlen(tag) +2);
			char *ret = (char *)malloc(size);
			memcpy(ret, lines[i].c_str() + strlen(tag) +2, size);

			return ret;
		}
	}
	return NULL;
}
