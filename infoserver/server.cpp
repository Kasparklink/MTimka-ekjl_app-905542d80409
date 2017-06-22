#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "server.h"
#include "functions.h"

int sock_desc;
struct sockaddr_in serv_addr, client_addr;
char buff[1024];

int size;

bool server::set_up(char *ip, int port) {
	sock_desc = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_desc < 0) return false;

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(ip);
	serv_addr.sin_port = htons(port);

	if (bind(sock_desc, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
		return false;

	listen(sock_desc, 5);
	size = sizeof(client_addr);

	return true;
}


int server::get_connection() {
	return accept(sock_desc, (struct sockaddr *)&client_addr, (socklen_t *)&size);
}

char *server::get_data(int conn_desc) {
	double read_so_far = 0;
	int i_read = 0;
	std::string *all_msg = new std::string("");
	
	memset(buff, 0, sizeof(buff));

	for (;;) {
		i_read = read(conn_desc, buff, sizeof(buff));

		if (i_read != 0 && ends_with(buff, (int)i_read, ";end")) {
			all_msg->append(std::string(buff));

			char *ret = (char *)malloc(all_msg->length());
			memcpy(ret, all_msg->c_str(), all_msg->length() -4);
			ret[all_msg->length() -4] = '\x00';

			delete all_msg;
			return ret;
		}
		
		read_so_far += i_read;
		all_msg->append(std::string(buff));
		if (read_so_far == 0) break;
	}
	
	delete all_msg;
	return NULL;
}

bool server::send_data(int conn_desc, char *data) {
	int total = strlen(data);
	int sent = 0;
	int bytes;
	
	char *data_send = (char *)malloc(total + 4);
	memcpy(data_send, data, total);
	memcpy(data_send + total, ";end", 4);
	total += 4;
	
	while (sent < total) {
		bytes = write(conn_desc, data_send + sent, total - sent);
		
		if (bytes < 0) return false;
		if (bytes == 0) break;

		sent += bytes;
	}
	//close(conn_desc);
	//printf("\nbytes wrote: %d\n", sent);

	free(data_send);
	return true;
}

int server::closee() {
	close(sock_desc);
	return 0;
}
