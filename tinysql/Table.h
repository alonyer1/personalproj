#pragma once
#include<vector>
#include<string>
#include<unordered_map>
using namespace std;

enum column_type {
	SQL_INT, SQL_STR
};
typedef struct sqlHeader {
	string name;
	enum column_type type;
} sqlHeader;
typedef struct Row {
	unordered_map<string, string> strings;
	unordered_map<string, int> ints;
} Row;
class Table
{
private:
	vector<sqlHeader> headers;
	vector<Row> rows;
public:
	Table(vector<sqlHeader> headers) : headers(headers) {}
	Table(const Table& other) = default;
	void insert(Row row);
	void deleteSTR(vector<sqlHeader> headers, string WHERE, string value);
	void deleteINT(vector<sqlHeader> headers, string WHERE, int value);
	Table selectSTR(vector<sqlHeader> headers, string WHERE, string value);
	Table selectINT(vector<sqlHeader> headers, string WHERE, int value);
	void print_table();
};

