#include "pingpong.h"

/*
 * This function sends and wait for a reply on a socket.
 * int msg_size: message length
 * int msg_no: message sequence number (written into the message)
 * char message[msg_size]: buffer to send
 * int tcp_socket: socket file descriptor
 */
double do_ping(size_t msg_size, int msg_no, char message[msg_size], int tcp_socket)
{
	char rec_buffer[msg_size];
	ssize_t recv_bytes, sent_bytes;
	size_t offset;
	struct timespec send_time, recv_time;

    /*** write msg_no at the beginning of the message buffer ***/

	sprintf(message, "%d\n", msg_no);

    /*** Store the current time in send_time ***/

       if (clock_gettime(CLOCK_TYPE, &send_time) != 0)
		fail_errno("Can't store the current time in send_time");

    /*** Send the message through the socket ***/
       if ((sent_bytes = write_all(tcp_socket, message, msg_size)) == -1)
                fail_errno("Error sending the message");

    /*** Receive answer through the socket (blocking) ***/
	for (offset = 0; (offset + (recv_bytes = recv(tcp_socket, rec_buffer + offset, sent_bytes - offset, MSG_WAITALL))) < msg_size; offset += recv_bytes) {
		debug(" ... received %zd bytes back\n", recv_bytes);
		if (recv_bytes < 0)
			fail_errno("Error receiving data");
	}

    /*** Store the current time in recv_time ***/
	if (clock_gettime(CLOCK_TYPE, &recv_time) != 0)
		fail_errno("Can't store the current time in recv_time");

	printf("tcp_ping received %zd bytes back\n", recv_bytes);
	return timespec_delta2milliseconds(&recv_time, &send_time);
}

int main(int argc, char **argv)
{
	struct addrinfo gai_hints, *server_addrinfo;
	int msgsz, norep;
	int gai_rv;
	char ipstr[INET_ADDRSTRLEN];
	struct sockaddr_in *ipv4;
	int tcp_socket;
	char request[MAX_REQ], answer[MAX_ANSW];
	ssize_t nr;

	if (argc < 4)
		fail("Incorrect parameters provided. Use: tcp_ping PONG_ADDR PONG_PORT SIZE [NO_REP]\n");
	for (nr = 4, norep = REPEATS; nr < argc; nr++)
		if (*argv[nr] >= '1' && *argv[nr] <= '9')
			sscanf(argv[nr], "%d", &norep);
	if (norep < MINREPEATS)
		norep = MINREPEATS;
	else if (norep > MAXREPEATS)
		norep = MAXREPEATS;

    /*** Initialize hints in order to specify socket options ***/
	memset(&gai_hints, 0, sizeof gai_hints);

	gai_hints.ai_family = AF_INET;
	gai_hints.ai_socktype = SOCK_STREAM;
	gai_hints.ai_flags = AI_PASSIVE;
	gai_hints.ai_protocol = IPPROTO_TCP;


    /*** call getaddrinfo() in order to get Pong Server address in binary form ***/

    gai_rv = getaddrinfo(argv[1], argv[2], &gai_hints, &server_addrinfo);
    if (gai_rv != 0)
        fail_errno("Error getaddrinfo");


    /*** Print address of the Pong server before trying to connect ***/
	ipv4 = (struct sockaddr_in *)server_addrinfo->ai_addr;
	printf("TCP Ping trying to connect to server %s (%s) on port %s\n", argv[1], inet_ntop(AF_INET, &ipv4->sin_addr, ipstr, INET_ADDRSTRLEN), argv[2]);

    /*** create a new TCPsocket and connect it with the server ***/

	 if ((tcp_socket = socket(server_addrinfo->ai_family, server_addrinfo->ai_socktype, server_addrinfo->ai_protocol)) == -1)
            fail_errno("Error creating a new socket");

        if (connect(tcp_socket, server_addrinfo->ai_addr, server_addrinfo->ai_addrlen) == -1) {
            close(tcp_socket); 
            fail_errno("Error connecting to the server");
        }


	freeaddrinfo(server_addrinfo);
	if (sscanf(argv[3], "%d", &msgsz) != 1)
		fail("Incorrect format of size parameter");
	if (msgsz < MINSIZE)
		msgsz = MINSIZE;
	else if (msgsz > MAXTCPSIZE)
		msgsz = MAXTCPSIZE;
	printf(" ... connected to Pong server: asking for %d repetitions of %d bytes TCP messages\n", norep, msgsz);
	sprintf(request, "TCP %d %d\n", msgsz, norep);

    /*** Write the request on socket ***/

	 nr = write_all(tcp_socket, request, strlen(request));
	 if (nr < 0)
		 fail_errno("TCP Ping could not send request to Pong server");

	nr = read(tcp_socket, answer, sizeof(answer));
	if (nr < 0)
		fail_errno("TCP Ping could not receive answer from Pong server");
		

    /*** Check if the answer is OK, and fail if it is not ***/

	if (strncmp(answer, "OK", 2) != 0)
		fail_errno("Pong server hasn't agreed");


    /*** else ***/
	printf(" ... Pong server agreed :-)\n");

	{
		double ping_times[norep];
		struct timespec zero, resolution;
		char message[msgsz];
		int rep;
		memset(message, 0, (size_t)msgsz);
		for(rep = 1; rep <= norep; ++rep) {
			double current_time = do_ping((size_t)msgsz, rep, message, tcp_socket);
			ping_times[rep - 1] = current_time;
			printf("Round trip time was %lg milliseconds in repetition %d\n", current_time, rep);
		}
		memset((void *)(&zero), 0, sizeof(struct timespec));
		if (clock_getres(CLOCK_TYPE, &resolution))
			fail_errno("TCP Ping could not get timer resolution");
		print_statistics(stdout, "TCP Ping: ", norep, ping_times, msgsz, timespec_delta2milliseconds(&resolution, &zero));
	}

	shutdown(tcp_socket, SHUT_RDWR);
	close(tcp_socket);
	exit(EXIT_SUCCESS);
}

