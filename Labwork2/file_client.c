#include "file.h"
#include <time.h>

void filestransfer_1(char* host, char* filename)
{
	CLIENT* clnt;
	FILE* fp;
	int* result_1;
	file  transfer_file_1_arg;
	char ch;

#ifndef	DEBUG
	clnt = clnt_create(host, FILESTRANSFER, FILESTRANSFER_1, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror(host);
		exit(1);
	}
#endif	/* DEBUG */
	clock_t begin = clock();
	strcpy(transfer_file_1_arg.filename, filename);
	fp = fopen("send.txt", "r");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file successfully..\n");
	}
	memset(transfer_file_1_arg.buffer, 0, sizeof(transfer_file_1_arg.buffer));
	transfer_file_1_arg.buffer_size = 0;
	int num_read = fread(transfer_file_1_arg.buffer, 1, 1024, fp);
	transfer_file_1_arg.buffer_size += num_read;

	result_1 = transfer_file_1(&transfer_file_1_arg, clnt);
	if (result_1 == (int*)NULL) {
		clnt_perror(clnt, "call failed");
	}
	clock_t end = clock();
	double upload_time = (double)(end - begin) / CLOCKS_PER_SEC;

	printf("Upload time: %lf\n", upload_time);
	printf("The file has been successfully sent..\n");



#ifndef	DEBUG
	clnt_destroy(clnt);
#endif	 /* DEBUG */
	fclose(fp);
}


int main(int argc, char* argv[])
{
	char* host;
	char* filename;

	if (argc < 3) {
		printf("usage: %s server_host file_name\n", argv[0]);
		exit(1);
	}
	host = argv[1];
	filename = argv[2];
	filestransfer_1(host, filename);
	exit(0);
}