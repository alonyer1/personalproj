#pragma once
#include<vector>
#include<string>
#include<map>
#include<unordered_set>
using namespace std;

enum column_type {
	SQL_INT, SQL_STR
};
typedef struct sqlHeader {
	string name;
	enum column_type type;
} sqlHeader;
typedef struct Row {
	map<string, string> strings;
	map<string, int> ints;
} Row;
class Table
{
private:
	const string table_name;
	map<string, column_type> headers_map;
	const vector<sqlHeader> headers;
	vector<Row> rows;
	void assert_columns(vector<string> columns);
public:
	Table(const string name, vector<sqlHeader> headers) : table_name(name),headers(headers),headers_map() {
		for (auto& c : headers) {
			headers_map.insert({c.name, c.type});
		}
	}
	Table(const Table& other) = default;
	void insert(Row row);
	void deleteSTR(string WHERE, string value);
	void deleteINT(string WHERE, int value);
	Table select(vector<string> columns);
	Table select_where(vector<string> columns, string WHERE, string svalue, int ivalue, column_type type);
	void print_table();
};