#include <iostream>
#include <vector>
#include <stdio.h>
#include <mpi.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <map>
#include <iterator>
#include <cctype>
#include <algorithm>
#include <unordered_map>

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	MPI_Comm a;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnameb[53];
	char portnamec[53];
	char portnamenow[53];
	int option, cont;
	std::string key, value;
	char value_db[100], key_db[100];
	char buf;
	FILE* fp;
	MPI_Open_port(MPI_INFO_NULL, portname);
	std::cout << portname << std::endl;
	fp = fopen("a.txt", "w");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}

	fprintf(fp, "%s\n", portname);
	fclose(fp);

	fp = fopen("b.txt", "r");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}

	fread(portnameb, 1, 53, fp);
	fclose(fp);

	fp = fopen("c.txt", "r");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	fread(portnamec, 1, 53, fp);
	fclose(fp);

	MPI_Comm_connect(portnameb, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
	std::cout << "Client connected to primary server" << std::endl;
	int again = 1;
	strcpy(portnamenow, portnameb);
	while (again != 0) {
		std::cout << portnamenow << std::endl;
		MPI_Comm_connect(portnamenow, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		std::cout << "Your option is: 1 to get, 2 to add/update, 3 to delete" << std::endl;
		std::cin >> option;
		std::cout << "Your option: " << option << std::endl;

		switch (option) {
			case 1: {
				printf("Client requests GET\n");
				std::cin >> key;
				MPI_Send(&option, 1, MPI_INT, 0, 2001, a);
				strcpy(key_db, key.c_str());
				MPI_Send(&key_db, 100, MPI_CHAR, 0, 2001, a);
				MPI_Comm_accept(portnamenow, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
				MPI_Recv(&value_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);
				std::cout << "Value of " << key << " is " << value_db << std::endl;
				std::cout << "Finish get" << std::endl;
				break;
			}
			case 2: {
				printf("Client requests ADD/UPDATE\n");
				std::cin >> key >> value;
				MPI_Send(&option, 1, MPI_INT, 0, 2001, a);
				strcpy(key_db, key.c_str());
				strcpy(value_db, value.c_str());
				MPI_Send(&key_db, 100, MPI_CHAR, 0, 2001, a);
				MPI_Send(&value_db, 100, MPI_CHAR, 0, 2001, a);
				sleep(1.0);
				while (fopen("primarynow.txt", "r") != NULL) {
					fread(portnamenow, 1, 53, fp);
					fclose(fp);
					break;
				}
				MPI_Comm_accept(portnamenow, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
				std::cout << portnamenow << std::endl;
				// MPI_Recv(&portnamerecv, 53, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);

				std::cout << "Finish add/update" << std::endl;
				break;
			}
			case 3: {
				printf("Client requests Delete\n");
				std::cin >> key;
				MPI_Send(&option, 1, MPI_INT, 0, 2001, a);
				strcpy(key_db, key.c_str());
				MPI_Send(&key_db, 100, MPI_CHAR, 0, 2001, a);
				sleep(1.0);
				while (fopen("primarynow.txt", "r") != NULL) {
					fread(portnamenow, 1, 53, fp);
					fclose(fp);
					break;
				}
				MPI_Comm_accept(portnamenow, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
				std::cout << portnamenow << std::endl;
				// MPI_Recv(&portnamerecv, 53, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);

				std::cout << "Finish delete" << std::endl;
				break;
			}
		}
		// std::cout << "Out switch" << std::endl;
		std::cout << "Do you want to continue?" << std::endl;
		scanf("%d", &cont);
		if (cont == 0) {
			MPI_Send(&cont, 1, MPI_INT, 0, 2010, a);
			// again = 0;
			break;
		}
		else {
			MPI_Send(&cont, 1, MPI_INT, 0, 2010, a);
			continue;
		}

	}
	std::cout << "Disconnect" << std::endl;
	MPI_Close_port(portname);
	MPI_Comm_disconnect(&a);
	MPI_Finalize();
	return 0;
}