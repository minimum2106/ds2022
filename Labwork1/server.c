#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

void error(const char* msg) {
	//print system error message
	perror(msg);
	exit(1);
}

void write_file(FILE* fp, int sockfd) {
	char buffer[1024];

	while (1) {
		int n = recv(sockfd, buffer, sizeof(buffer), 0);
		if (n <= 0) {
			break;
		}
		fprintf(fp, "%s", buffer);
		memset(&buffer, 0, sizeof(buffer));
	}
	return;

}

int main() {
	int ss, cli, pid;
	struct sockaddr_in ad;
	char s[100];
	socklen_t ad_length = sizeof(ad);
	FILE* fp;
	// create the socket
	ss = socket(AF_INET, SOCK_STREAM, 0);

	// bind the socket to port 12345
	memset(&ad, 0, sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_addr.s_addr = INADDR_ANY;
	ad.sin_port = htons(12345);
	bind(ss, (struct sockaddr*)&ad, ad_length);

	// then listen
	listen(ss, 0);

	while (1) {
		// an incoming connection
		cli = accept(ss, (struct sockaddr*)&ad, &ad_length);

		pid = fork();
		if (pid == 0) {
			printf("client connected\n");

			fp = fopen("received.txt", "w");
			if (fp == NULL) {
				error("Error in reading file.");
			}
			else {
				printf("Reading file successfully..\n");
			}
			write_file(fp, cli);
			printf("Received file.");

			return 0;
		}
		else {
			// continue the loop to accept more clients
			continue;
		}
	}

	// disconnect
	close(cli);
	close(ss);

}