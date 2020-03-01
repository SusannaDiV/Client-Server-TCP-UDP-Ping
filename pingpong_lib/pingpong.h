#ifndef PINGPONG_H
#define PINGPONG_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <assert.h>

#define MINREPEATS 151		/* Min number of tests */
#define REPEATS 501		/* Default number of tests */
#define MAXREPEATS 1501		/* Max number of tests */
#define MINSIZE 16		/* Minimum message size */
#define LISTENBACKLOG 1
#define UDP_TIMEOUT ((double)1000.0)	/* 1.0 seconds */
#define MAXTCPSIZE (1024*1024)	/* 1 MiB */
#define MAXUDPSIZE 65500
#define MAXUDPRESEND 3
#define IANAMINEPHEM 49152
#define IANAMAXEPHEM 65535
#define PONGRECVTOUT 10
#define MAX_REQ 30
#define MAX_ANSW 10
#define DEBUG

extern void fail_errno(const char *const msg);
extern void fail(const char *const msg);
extern double timespec_delta2milliseconds(struct timespec *last, struct timespec *previous);
extern void print_statistics(FILE * outf, const char *name, int repeats,
			     double rtt[repeats], int msg_sz, double resolution);

#define CLOCK_TYPE CLOCK_MONOTONIC

#ifdef DEBUG
#define debug(...) printf(__VA_ARGS__)
#else
#define debug(...)
#endif

ssize_t read_all(int fd, void *ptr, size_t n);
ssize_t write_all(int fd, const void *ptr, size_t n);

#endif /* #ifdef PINGPONG_H */

