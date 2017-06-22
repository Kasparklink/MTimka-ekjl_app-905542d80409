#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <string>

#include "functions.h"

const int self_exe_length = 10;

char *program_path() {
	char *path = (char *)malloc(PATH_MAX);
	memset(path, 0, PATH_MAX);

	if (path != NULL) {
		if (readlink("/proc/self/exe", path, PATH_MAX) == -1) {
			free(path);
			path = NULL;
		}
	}
	return path;
}

int getch() {
	struct termios t;
	int ch;
	
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	
	ch = getchar();
	
	tcgetattr(STDIN_FILENO, &t);
	t.c_lflag |= (ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &t);
	
	return ch;
}

bool starts_with(char *str, const char *needle) {
	for (int i = 0; i < strlen(needle); i++)
		if (str[i] != needle[i])
			return false;
	return true;
}

bool ends_with(char *str, int size, const char *needle) {
	int needle_size = strlen(needle);

	for (int i = 0; i < needle_size; i++)
		if (str[size - needle_size + i] != needle[i])
			return false;
	return true;
}

char *get_xml(char *name) {
	std::string str;
	char *path = program_path();

	str += path;//std::string(path);
	str = str.substr(0, str.length() - self_exe_length);
	str += "xmls/" + std::string(name);
	
	free(path);

	char *ret = (char *)malloc(str.length() +1);
	memcpy(ret, str.c_str(), str.length() +1);
	return ret;
} 
