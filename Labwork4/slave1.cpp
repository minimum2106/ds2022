#include<iostream>
#include<vector>
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
#include<algorithm>

int main(int argc, char* argv[]) {

	MPI_Init(&argc, &argv);
	MPI_Comm b;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnamea[53];
	FILE* fp;
	int option, key, rand;
	char value[20];
	char content[330] = { 0 };
	std::string line, word;
	std::vector<std::pair<std::string, int>>datas;
	MPI_Open_port(MPI_INFO_NULL, portname);
	std::cout << portname << std::endl;
	fp = fopen("b.txt", "w");
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
	MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &b);
	MPI_Recv(&content, 329, MPI_CHAR, MPI_ANY_SOURCE, 2002, b, &status);
	std::cout << "ALl content" << content;
	line = content;
	for (int i = 0, len = line.size(); i < len; i++) {
		if (ispunct(line[i])) {
			line.erase(i--, 1);
			len = line.size();
		}
	}
	std::istringstream ss(line);

	while (ss >> word) {
		std::pair<std::string, int> pair1;
		pair1.first = word;
		pair1.second = 1;
		datas.push_back(pair1);
	}

	std::sort(datas.begin(), datas.end());
	for (int i = 0; i < datas.size(); i++) {
		std::cout << datas[i].first << datas[i].second << std::endl;
	}
	std::map<std::string, int> counts;

	for (const auto& p : datas) {
		if (!counts[p.first]) {
			counts[p.first] = p.second;
		}
		else {
			counts[p.first] += p.second;
		}
	}
	// std::cout<<"In count"<<counts.size()<<std::endl;
	sleep(1.0);
	MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &b);
	int total = counts.size();
	MPI_Send(&(total), 1, MPI_INT, 0, 2004, b);
	for (const auto& p : counts) {
		std::string p1 = p.first;
		int count = p.second;
		char buff[20] = { 0 };
		memset(&buff, 0, sizeof(buff));
		strcpy(buff, p1.c_str());
		MPI_Send(&buff, 20, MPI_CHAR, 0, 2004, b);
		MPI_Send(&count, 1, MPI_INT, 0, 2004, b);
	}

	MPI_Comm_disconnect(&b);
	MPI_Finalize();
	return 0;

}