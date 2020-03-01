#include <stdlib.h>
#include <stdio.h>
#include "pingpong.h"

void fail_errno(const char * const msg)
{
	perror(msg);
	exit(EXIT_FAILURE);
}

void fail(const char *const msg)
{
	fprintf(stderr, "%s\n", msg);
	exit(EXIT_FAILURE);
}

