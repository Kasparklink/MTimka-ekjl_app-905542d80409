#include <stdio.h>
#include <stdlib.h>

#include "errors.h"

void err::basic(const char *msg) {
	perror(msg);
	exit(0);
}
