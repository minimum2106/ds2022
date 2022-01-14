#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
void error(const char* msg) {
	//print system error message
	perror(msg);
	exit(1);
}

void send_file(FILE* f, int sockfd) {
	char buffer[1024] = { 0 };
	while (fgets(buffer, sizeof(buffer), f) != NULL) {
		int i = send(sockfd, buffer, sizeof(buffer), 0);
		if (i == -1) {
			error("Error in sending data");
		}
		memset(&buffer, 0, sizeof(buffer));

	}
}


int main(int argc, char* argv[]) {
	int so;
	char s[100];
	struct sockaddr_in ad;


	socklen_t ad_length = sizeof(ad);
	struct hostent* hep;

	// create socket
	int serv = socket(AF_INET, SOCK_STREAM, 0);

	// init address
	hep = gethostbyname(argv[1]);
	memset(&ad, 0, sizeof(ad));
	ad.sin_family = AF_INET;
	ad.sin_addr = *(struct in_addr*)hep->h_addr_list[0];
	ad.sin_port = htons(12345);

	// connect to server
	connect(serv, (struct sockaddr*)&ad, ad_length);

	memset(&s, 0, 100);
	FILE* f;
	f = fopen("send.txt", "r");
	if (f == NULL) {
		error("Error in reading file.");
	}
	else {
		printf("Reading file successfully..\n");
	}
	rewind(f);
	send_file(f, serv);
	printf("The file has been transfered successfully...\n");
	printf("Close...\n");
	close(serv);
	return 0;


}