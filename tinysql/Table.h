#pragma once
#include<vector>
#include<string>
#include<unordered_map>
#include<set>
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
	const string table_name;
	const set<sqlHeader> headers_set;
	const vector<sqlHeader> headers;
	vector<Row> rows;
	void assert_headers(vector<sqlHeader> hdrs);
public:
	Table(const string name, vector<sqlHeader> headers) :
		table_name(name),
		headers(headers),
		headers_set(set<sqlHeader>(headers.begin(), headers.end())) {}
	Table(const Table& other) = default;
	void insert(Row row);
	void deleteSTR(string WHERE, string value);
	void deleteINT(string WHERE, int value);
	Table selectSTR(vector<sqlHeader> headers, string WHERE, string value);
	Table selectINT(vector<sqlHeader> headers, string WHERE, int value);
	void print_table();
};

