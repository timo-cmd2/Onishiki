#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>

#define STATUS_LINE "HTTP/1.1 200 OK\r\n"
#define CONTENT_LENGTH "Content-Length: %d\r\n"
#define CONTENT_TYPE "Content-Type: text/html; charset=UTF-8\r\n"

#define ONISHIKI_VERSION_MAJOR 0
#define ONISHIKI_VERSION_MINOR 1
#define ONISHIKI_VERSION_PATCH 1
#define ONISHIKI_VERSION_EXTRA "-dev"
#define ONISHIKI_VERSION_FULL  "0.1.1-dev"

struct sigaction sigint;

void handle_sigint( int sig ) {
	printf( "[http] ^C pressed! Exiting!\n" );
	exit( 0 );
}

void init_signal_handling( ) {
	sigint.sa_handler = handle_sigint;
	sigaction( SIGINT, &sigint, NULL );
}

int main() {
    int sock0;
	struct sockaddr_in addr;
	struct sockaddr_in client;
	int len;
	int sock;
	int yes = 1;

	char buf[1024];
	char inbuf[1024];
	char urn[128];
	char *request;
	
	int doc;
    char *body;

	printf("[http] Onishiki version %s starting\n", ONISHIKI_VERSION_FULL);
	init_signal_handling();

	if ((sock0 = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket");
		return 1;
	}

    addr.sin_family = AF_INET;
	addr.sin_port = htons(9999);
	addr.sin_addr.s_addr = INADDR_ANY;

	setsockopt(sock0, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes, sizeof(yes));

	if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) {
		perror("bind");
		return 1;
	}
	if (listen(sock0, 5) != 0) {
		perror("listen");
		return 1;
	}

    listen(sock0, 5);
	while (1) {
		len = sizeof(client);
		sock = accept(sock0, (struct sockaddr *)&client, &len);
		if (sock < 0) {
			perror("accept");
			break;
		}
		memset(inbuf, 0, sizeof(inbuf));
		recv(sock, inbuf, sizeof(inbuf), 0);
		printf("%s", inbuf);

		strtok(inbuf, " ");
		request = strtok(NULL, " ");

		if (strcmp(request, "/") == 0) {
			request = "index.html";
		}

        snprintf(urn, sizeof(urn), "./docroot/%s", request);
		if (access(urn, F_OK) < 0) {
			printf("404 Not Found\n");
			snprintf(buf, sizeof(buf),
			    "HTTP/1.1 404 OK\r\n"
			    CONTENT_LENGTH
			    CONTENT_TYPE
			    "\r\n"
			    "<h1>404 Not Found</h1>", 22);
			send(sock, buf, (int)strlen(buf), 0);
			goto exit_loop;
		}
		doc = open(urn, O_RDONLY);
		body = mmap(NULL, 100, PROT_READ, MAP_PRIVATE, doc, 0);

        memset(buf, 0, sizeof(buf));
		snprintf(buf, sizeof(buf),
		    STATUS_LINE
		    CONTENT_LENGTH
		    CONTENT_TYPE
		    "\r\n"
		    "%s", (int)strlen(body), body);
		send(sock, buf, (int)strlen(buf), 0);
		munmap(body, 100);

        exit_loop:
		close(sock);
	}
    return 0;
}
