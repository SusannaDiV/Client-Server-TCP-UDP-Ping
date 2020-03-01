#include <stdlib.h>
#include "pingpong.h"

/* read_all and write_all, inspired by readn and writen of the 
   book "Advanced Programming in the UNIX Environment" */

ssize_t read_all(int fd, void *ptr, size_t n)
{
	size_t n_left = n;
	while (n_left > 0) {
		ssize_t n_read = read(fd, ptr, n_left);
		if (n_read < 0) {
			if (n_left == n)
				return -1; /* nothing has been read */
			else
				break; /* we have read something */
		} else if (n_read == 0) {
			break; /* EOF */
		}
		n_left -= n_read;
		ptr += n_read;
	}
	assert(n - n_left >= 0);
	return n - n_left;
}

ssize_t write_all(int fd, const void *ptr, size_t n)
{
	size_t n_left = n;
	while (n_left > 0) {
		ssize_t n_written = write(fd, ptr, n_left);
		if (n_written < 0) {
			if (n_left == n)
				return -1; /* nothing has been written */
			else
				break; /* we have written something */
		} else if (n_written == 0) {
			break;
		}
		n_left -= n_written;
		ptr += n_written;
	}
	assert(n - n_left >= 0);
	return n - n_left;
}

