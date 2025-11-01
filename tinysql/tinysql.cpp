// tinysql.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "sqlParser.h"
#include <iostream>
#include <fstream>
#include <regex>
#define TEST "test3"

int main()
{
	FILE* file;
	if (fopen_s(&file, "tests/" TEST ".txt", "r") != 0) {
		cerr << "Error opening file." << endl;
	}
	fstream sfile = fstream(file);
	sqlParser server = sqlParser();
	string line;
	Table* result = NULL;
	while (getline(sfile, line)) {
		cout << ">>>>" << line << endl;
		server.parse(line);
		result = server.getresult();
		if (result != NULL) result->print_table();
	}
	fclose(file);
	return 0;
	
	//smatch matches;
	//regex reg("hello (world|banana) yo");
	//string s = "hello world yo hello banana yo what";
	//regex_search(s, matches, reg);
	//cout << matches.suffix().str() << endl;
	//return 0;
	
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
