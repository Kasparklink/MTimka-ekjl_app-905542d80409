#ifndef SERVER_H
#define SERVER_H

class server {
	public:
		static bool set_up(char *ip, int port);
		static int get_connection();
		static char *get_data(int conn_desc);
		static bool send_data(int conn_desc, char *data);
		static int closee();
};

#endif
