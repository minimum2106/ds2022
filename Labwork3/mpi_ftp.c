#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <stdlib.h>
#define MAX_SIZE 1024
#define size_tag 2001
#define char_tag 2002

void send_file(FILE* fp, char* filename, char* buffer, int rank_des) {
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file successfully..\n");
	}
	int buffer_size;
	while (1) {
		int buffer_size = fread(buffer, 1, MAX_SIZE, fp);
		// printf("Buffer size %d \n", buffer_size);
		MPI_Send(&buffer_size, 1, MPI_INT, rank_des, size_tag, MPI_COMM_WORLD);
		MPI_Send(buffer, buffer_size, MPI_CHAR, rank_des, char_tag, MPI_COMM_WORLD);
		// printf("Buffer send: %s \n", buffer);                        
		if (buffer_size < MAX_SIZE) {
			printf("\nUpload finished.\n");
			break;
		}
	}


	fclose(fp);

}

void receive_file(FILE* fp, char* filename, char* buffer, int rank_from) {

	fp = fopen(filename, "a");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file received successfully..\n");
	}
	int buffer_size;
	while (1) {
		MPI_Recv(&buffer_size, 1, MPI_INT, rank_from, size_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(buffer, buffer_size, MPI_CHAR, rank_from, char_tag, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		fwrite(buffer, 1, buffer_size, fp);
		if (buffer_size < MAX_SIZE) {
			printf("\nWrite finished.\n");
			break;
		}


	}
	fclose(fp);
}

int main(int argc, char* argv[]) {
	FILE* fp;
	int my_id, numprocs, len;
	char buffer[MAX_SIZE];
	char name[MPI_MAX_PROCESSOR_NAME];
	int client_to_server = 0;
	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &my_id);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);

	if (numprocs < 2) {
		printf("Need at least 2 processes");
	}
	MPI_Get_processor_name(name, &len);
	char* root_send = "send.txt";
	char* slave_received = "received.txt";
	memset(&buffer, 0, sizeof(buffer));
	if (client_to_server) {
		if (my_id == 0) {
			send_file(fp, root_send, buffer, 1);

			// printf("Processor %s has sent the messages\n",name);

		}
		else if (my_id == 1) {
			receive_file(fp, slave_received, buffer, 0);

			// printf("Processor %s has received the message:\n",name);

		}
	}
	else {
		if (my_id == 0) {
			receive_file(fp, root_send, buffer, 1);

		}
		else if (my_id == 1) {
			send_file(fp, slave_received, buffer, 0);

		}
	}


	MPI_Finalize();

}