#include <stdio.h>

#include "lprint.h"

bool bo_print = true;

bool lprint::good(const char *str) {
	if (bo_print == true)
		printf("--[+] %s\n", str);
	return bo_print;
}

bool lprint::bad(const char *str) {
	if (bo_print == true)
		printf("--[-] %s\n", str);
	return bo_print;
}

bool lprint::notice(const char *str) {
	if (bo_print == true)
		printf("--[*] %s\n", str);
	return bo_print;
}


void lprint::disable_print() {
	bo_print = false;
}

void lprint::enable_print() {
	bo_print = true;
}
