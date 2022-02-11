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

bool sortbysec(const std::pair<std::string, int>& a,
	const std::pair<std::string, int>& b)
{
	return (a.second < b.second);
}
int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	MPI_Comm c;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnamea[53];
	FILE* fp;
	int option, key, rand;
	char value[20];
	char content[1256] = { 0 };
	std::string line, word;
	std::vector<std::pair<std::string, int>>datas;
	char buf;

	MPI_Open_port(MPI_INFO_NULL, portname);
	int is_primary = 1;
	int again = 1;
	std::cout << portname << std::endl;
	fp = fopen("c.txt", "w");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file successfully..\n");
	}

	fprintf(fp, "%s\n", portname);
	fclose(fp);
	printf("Write file successfully..\n");

	while (fopen("server.txt", "r") != NULL) {
		fread(portnamea, 1, 53, fp);
		fclose(fp);
		break;
	}
	MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
	MPI_Recv(&content, 1255, MPI_CHAR, MPI_ANY_SOURCE, 2003, c, &status);

	line = content;
	// std::cout<<content;
	std::istringstream ss(content);

	while (ss >> word) {
		std::pair<std::string, int> pair1;
		pair1.first = word;
		pair1.second = word.length();
		datas.push_back(pair1);
	}


	std::sort(datas.begin(), datas.end(), sortbysec);
	sleep(1.0);
	MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
	char buff[100] = { 0 };
	strcpy(buff, datas[datas.size() - 1].first.c_str());
	int count = datas[datas.size() - 1].second;
	MPI_Send(&buff, 100, MPI_CHAR, 0, 2004, c);
	MPI_Send(&count, 1, MPI_INT, 0, 2004, c);
	MPI_Comm_disconnect(&c);
	MPI_Finalize();
	return 0;

}