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
	MPI_Comm a;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnamea[53];
	char portnameb[53];
	char buff2[20] = { 0 };
	FILE* fp;
	int filea, fileb, total1, total2, count1, count2;
	std::vector<std::pair<std::string, int>>datas, finaldatas;
	std::string word;
	MPI_Open_port(MPI_INFO_NULL, portname);
	fp = fopen("server.txt", "w");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}
	else {
		printf("Reading file successfully..\n");
	}
	fprintf(fp, "%s\n", portname);
	fclose(fp);

	std::ifstream f1("test.txt");
	std::string content((std::istreambuf_iterator<char>(f1)),
		(std::istreambuf_iterator<char>()));
	std::string half = content.substr(0, 329);
	std::string otherhalf = content.substr(331, 627);
	while (1) {
		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		printf("Connect to slave 1\n");

		char buff[329] = { 0 };
		memset(&buff, 0, sizeof(buff));
		strcpy(buff, half.c_str());
		MPI_Send(&buff, 329, MPI_CHAR, 0, 2002, a);

		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		printf("Connect to slave 2\n");

		char buff1[298] = { 0 };
		memset(&buff1, 0, sizeof(buff1));
		strcpy(buff1, otherhalf.c_str());
		MPI_Send(&buff1, 298, MPI_CHAR, 0, 2003, a);

		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		MPI_Recv(&total1, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
		std::cout << "Number of words in 1: " << total1 << std::endl;
		for (int i = 0; i < total1; i++) {

			memset(&buff2, 0, sizeof(buff2));
			MPI_Recv(&buff2, 20, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);
			MPI_Recv(&count1, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
			word = buff2;
			datas.push_back(std::make_pair(word, count1));
		}

		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		MPI_Recv(&total2, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
		std::cout << "Number of words in 2: " << total2 << std::endl;
		for (int i = 0; i < total2; i++) {
			memset(&buff2, 0, sizeof(buff2));
			MPI_Recv(&buff2, 20, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);
			MPI_Recv(&count2, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
			word = buff2;
			datas.push_back(std::make_pair(word, count2));
		}

		std::cout << "Datas size: " << datas.size() << std::endl;

		break;

	}
	// for (int i = 0; i < datas.size(); i++) {
	// 	std::cout << datas[i].first << " "<<datas[i].second << std::endl;
	// }

	std::map<std::string, int> counts;

	for (const auto& p : datas) {
		if (!counts[p.first]) {
			counts[p.first] = p.second;
		}
		else {
			counts[p.first] += p.second;
		}
	}
	for (const auto& p : counts) {
		std::string p1 = p.first;
		int count = p.second;
		// std::cout<<"key: "<<p1<<" value "<<count<<std::endl;
		finaldatas.push_back(std::make_pair(p1, count));
	}
	for (int i = 0; i < finaldatas.size(); i++) {
		std::cout << finaldatas[i].first << " occurs " << finaldatas[i].second << " times" << std::endl;
	}
	MPI_Comm_disconnect(&a);
	MPI_Finalize();
	return 0;
}
