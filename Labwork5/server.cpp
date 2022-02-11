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
	MPI_Comm a;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnamea[53];
	char portnameb[53];
	char buff2[100] = { 0 };
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
	std::cout << content.length() << std::endl;
	std::string half = content.substr(0, 959);
	std::string otherhalf = content.substr(960, content.length() - 960);
	std::cout << half << otherhalf << std::endl;
	f1.close();
	while (1) {
		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		std::cout << "connect to slave 1" << std::endl;

		char buff[959] = { 0 };
		memset(&buff, 0, sizeof(buff));
		strcpy(buff, half.c_str());
		MPI_Send(&buff, 959, MPI_CHAR, 0, 2002, a);

		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		std::cout << "connect to slave 2" << std::endl;

		char buff1[1255] = { 0 };
		memset(&buff1, 0, sizeof(buff1));
		strcpy(buff1, otherhalf.c_str());
		MPI_Send(&buff1, 1255, MPI_CHAR, 0, 2003, a);
		memset(&buff1, 0, sizeof(buff1));
		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		MPI_Recv(&buff2, 100, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);
		MPI_Recv(&count1, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
		word = buff2;
		datas.push_back(std::make_pair(word, count1));

		MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &a);
		memset(&buff2, 0, sizeof(buff2));
		MPI_Recv(&buff2, 100, MPI_CHAR, MPI_ANY_SOURCE, 2004, a, &status);
		MPI_Recv(&count2, 1, MPI_INT, MPI_ANY_SOURCE, 2004, a, &status);
		memset(&buff2, 0, sizeof(buff2));
		word = buff2;
		datas.push_back(std::make_pair(word, count2));

		break;

	}
	std::sort(datas.begin(), datas.end(), sortbysec);
	for (int i = 0; i < datas.size(); i++) {
		if (datas[i].second == datas[datas.size() - 1].second) {
			std::cout << "Longest file path " << datas[i].first << " with length " << datas[i].second << std::endl;
		}
	}
	MPI_Comm_disconnect(&a);
	// MPI_Close_port(portname);
	MPI_Finalize();
	return 0;
}