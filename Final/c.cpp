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
int randomRange(int min, int max) //range : [min, max]
{
	static bool first = true;
	if (first)
	{
		srand(time(NULL)); //seeding for the first time only!
		first = false;
	}
	return min + rand() % ((max + 1) - min);
}

void dumpFile(std::string filename, std::unordered_map<std::string, std::string>& db) {
	std::ofstream MyFile(filename);
	for (auto& it : db) {
		MyFile << it.first << "   " << it.second << "\n";
	}
	MyFile.close();
}

int main(int argc, char* argv[]) {
	MPI_Init(&argc, &argv);
	MPI_Comm c;
	MPI_Status status;
	char portname[MPI_MAX_PORT_NAME];
	char portnameb[53];
	char portnamea[53];
	std::string value;
	std::string filename = "db.txt";
	char buf, key_db[100], value_db[100];
	int option, random_num = 0;
	int isprimary = 0;
	int again = 1;
	int cont;
	FILE* fp;
	std::unordered_map<std::string, std::string>db;

	MPI_Open_port(MPI_INFO_NULL, portname);
	std::cout << portname << std::endl;
	fp = fopen("c.txt", "w");
	if (fp == NULL) {
		perror("Error in reading file..\n");
		exit(1);
	}

	fprintf(fp, "%s\n", portname);
	fclose(fp);
	// printf("Write file successfully..\n");

	std::ifstream input(filename);
	for (std::string eachLine; getline(input, eachLine); )
	{
		std::istringstream strm(eachLine);
		std::string a, b;
		// loop for each string and add to the vector
		while (strm >> a >> b) {
			db[a] = b;
		}
	}

	std::cout << "Database size is: " << db.size() << std::endl;

	while (1) {
		if (isprimary) {
			MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
			std::cout << "Primary server connects to client" << std::endl;
			MPI_Recv(&option, 1, MPI_INT, MPI_ANY_SOURCE, 2001, c, &status);
			std::cout << "Client choose option: " << option << std::endl;
			MPI_Recv(&key_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2001, c, &status);
			switch (option) {
				case 1: {
					value = db[key_db];
					std::cout << "Value of key_db:" << key_db << " is " << value << std::endl;
					strcpy(value_db, value.c_str());
					MPI_Comm_connect(portnameb, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
					MPI_Send(&option, 1, MPI_INT, 0, 2002, c);
					MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
					MPI_Send(&value_db, 100, MPI_CHAR, 0, 2004, c);
					break;
				}

				case 2: {
					MPI_Recv(&value_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2001, c, &status);
					// std::cout << "Key: " << key_db << " Value: " << value_db << std::endl;
					random_num = randomRange(1, 10);
					fp = fopen("b.txt", "r");
					if (fp == NULL) {
						perror("Error in reading file..\n");
						exit(1);
					}

					fread(portnameb, 1, 53, fp);
					fclose(fp);
					MPI_Comm_connect(portnameb, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
					std::cout << "Primary Server connects to back up server" << std::endl;
					if (random_num > 8) {
						std::cout << "Start recovery process..." << std::endl;
						MPI_Send(&option, 1, MPI_INT, 0, 2002, c);
						MPI_Send(&random_num, 1, MPI_INT, 0, 2002, c);

						// MPI_Send(&key_db, 100, MPI_CHAR, 0, 2002, c);
						// MPI_Send(&value_db, 100, MPI_CHAR, 0, 2002, c);

						// db[key_db] = value_db;
						std::cout << "Finished switching server..." << std::endl;

						break;

					}
					else if (random_num <= 8) {
						fp = fopen("primarynow.txt", "w");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						fprintf(fp, "%s\n", portname);
						fclose(fp);

						db[key_db] = value_db;
						std::cout << "Successfully add to database " << value_db << std::endl;

						MPI_Send(&option, 1, MPI_INT, 0, 2002, c);
						MPI_Send(&random_num, 1, MPI_INT, 0, 2002, c);

						MPI_Send(&key_db, 100, MPI_CHAR, 0, 2002, c);
						MPI_Send(&value_db, 100, MPI_CHAR, 0, 2002, c);
						MPI_Recv(&buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 2003, c, &status);
						// std::cout<<HASH_COUNT(primary_server.db)<<std::endl;
						fp = fopen("a.txt", "r");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}

						MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
						// MPI_Send(&portname, 53, MPI_CHAR, 0, 2004, b);
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2004, c);
						break;
					}


				}

				case 3: {
					random_num = randomRange(1, 10);
					fp = fopen("b.txt", "r");
					if (fp == NULL) {
						perror("Error in reading file..\n");
						exit(1);
					}

					fread(portnameb, 1, 53, fp);
					fclose(fp);
					MPI_Comm_connect(portnameb, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
					std::cout << "Primary Server connects to back up server" << std::endl;
					if (random_num > 8) {
						std::cout << "Start recovery process..." << std::endl;
						MPI_Send(&option, 1, MPI_INT, 0, 2002, c);
						MPI_Send(&random_num, 1, MPI_INT, 0, 2002, c);

						// MPI_Send(&key_db, 100, MPI_CHAR, 0, 2002, c);

						// db.erase(key_db);
						std::cout << "Finished switching server..." << std::endl;

						break;

					}
					else if (random_num <= 8) {
						fp = fopen("primarynow.txt", "w");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						fprintf(fp, "%s\n", portname);
						fclose(fp);

						db.erase(key_db);
						std::cout << "Delete from database "<< std::endl;
						std::cout << "Database size is: " << db.size() << std::endl;
						MPI_Send(&option, 1, MPI_INT, 0, 2002, c);
						MPI_Send(&random_num, 1, MPI_INT, 0, 2002, c);

						MPI_Send(&key_db, 100, MPI_CHAR, 0, 2002, c);
						MPI_Recv(&buf, 1, MPI_CHAR, MPI_ANY_SOURCE, 2003, c, &status);
						// std::cout<<HASH_COUNT(primary_server.db)<<std::endl;
						fp = fopen("a.txt", "r");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}

						MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
						// MPI_Send(&portname, 53, MPI_CHAR, 0, 2004, b);
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2004, c);
						break;
					}


				}


			}
		}
		else if (isprimary == 0) {
			MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
			std::cout << "Back up server connects to primary server" << std::endl;
			MPI_Recv(&option, 1, MPI_INT, MPI_ANY_SOURCE, 2002, c, &status);

			switch (option) {
				case 1: {
					break;
				}
				case 2: {
					MPI_Recv(&random_num, 1, MPI_INT, MPI_ANY_SOURCE, 2002, c, &status);


					if (random_num <= 8) {
						MPI_Recv(&key_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2002, c, &status);
						MPI_Recv(&value_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2002, c, &status);
						db[key_db] = value_db;
						char buf = 'A';
						// std::cout << "Nothing here! " << std::endl;
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2003, c);
						break;
					}
					else if (random_num > 8) {
						std::cout << "Start switching server..." << std::endl;
						fp = fopen("primarynow.txt", "w");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						// else {
						// 	printf("Reading file successfully..\n");
						// }
						fprintf(fp, "%s\n", portname);
						fclose(fp);

						// db[key_db] = value_db;
						char buf = 'A';
						fp = fopen("a.txt", "r");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						// else {
						// 	printf("Reading file successfully..\n");
						// }

						fread(portnamea, 1, 53, fp);
						fclose(fp);


						MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
						// MPI_Send(&portname, 53, MPI_CHAR, 0, 2004, c);
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2004, c);
						break;

					}
				}

				case 3: {
					MPI_Recv(&random_num, 1, MPI_INT, MPI_ANY_SOURCE, 2002, c, &status);


					if (random_num <= 8) {
						MPI_Recv(&key_db, 100, MPI_CHAR, MPI_ANY_SOURCE, 2002, c, &status);
						db.erase(key_db);
						char buf = 'A';
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2003, c);
						break;
					}
					else if (random_num > 8) {
						std::cout << "Start switching server..." << std::endl;
						fp = fopen("primarynow.txt", "w");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						// else {
						// 	printf("Reading file successfully..\n");
						// }
						fprintf(fp, "%s\n", portname);
						fclose(fp);

						// db.erase(key_db);
						char buf = 'A';
						fp = fopen("a.txt", "r");
						if (fp == NULL) {
							perror("Error in reading file..\n");
							exit(1);
						}
						// else {
						// 	printf("Reading file successfully..\n");
						// }

						fread(portnamea, 1, 53, fp);
						fclose(fp);


						MPI_Comm_connect(portnamea, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
						// MPI_Send(&portname, 53, MPI_CHAR, 0, 2004, c);
						MPI_Send(&buf, 1, MPI_CHAR, 0, 2004, c);
						break;

					}
				}
			}


		}

		// std::cout << "Randon num: " << random_num << std::endl;

		if (random_num > 8 && isprimary == 0) {
			isprimary = 1;
		}
		else if (random_num > 8 && isprimary == 1) {
			isprimary = 0;
		}

		std::cout << "Database size is: " << db.size() << std::endl;
		// std::cout << "Isprimary " << isprimary << std::endl;

		if (isprimary) {
			fp = fopen("b.txt", "r");
			if (fp == NULL) {
				perror("Error in reading file..\n");
				exit(1);
			}
			// else {
			// 	printf("Reading file successfully..\n");
			// }

			fread(portnameb, 1, 53, fp);
			fclose(fp);
			MPI_Recv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, 2010, c, &status);
			MPI_Comm_connect(portnameb, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
			MPI_Send(&cont, 1, MPI_INT, 0, 2011, c);
			if (cont == 0) {
				// std::cout << "here pccont0" << std::endl;
				dumpFile(filename, db);
				break;
			}
			else {
				// std::cout << "here pccont1" << std::endl;
				continue;
			}
		}
		else if (isprimary == 0) {
			MPI_Comm_accept(portname, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &c);
			MPI_Recv(&cont, 1, MPI_INT, MPI_ANY_SOURCE, 2011, c, &status);
			if (cont == 0) {
				// std::cout << "here ccont0" << std::endl;
				break;
			}
			else {
				// std::cout << "here ccont1" << std::endl;
				continue;
			}
		}

	}
	std::cout << "Disconnect" << std::endl;
	MPI_Close_port(portname);
	MPI_Comm_disconnect(&c);
	MPI_Finalize();
	return 0;
}