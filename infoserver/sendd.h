#ifndef SENDD_H
#define SENDD_H

class sendd {
	public:
		static char *post(char *host, int port, char *path, char *SOAPAction, char *data, char *cookie = 0);
		static char *get_header_info(char *header, char *tag);
};

#endif
