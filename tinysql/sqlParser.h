#pragma once
#include "Table.h"
class sqlParser
{
private:
	map<string, Table> tables;
	Table* result;
	void insert(string str, Table* table);
	void select_delete(vector<string> columns, string str, Table* table, bool is_select);
	void create(string table_name, string str);
public:
	sqlParser() : tables(), result(NULL) {}
	void parse(string str);
	~sqlParser();
	Table* getresult() {
		return result;
	}
};