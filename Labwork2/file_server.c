#include "file.h"

int* transfer_file_1_svc(file* argp, struct svc_req* rqstp)
{
	static int  result;
	FILE* fp;
	fp = fopen("received.txt", "w+");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file successfully..\n");
	}

	fwrite(argp->buffer, 1, argp->buffer_size, fp);
	printf("%s \n", argp->buffer);
	printf("Received file..\n");
	fclose(fp);

	return &result;
}