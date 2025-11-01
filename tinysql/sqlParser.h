#pragma once
#include "Table.h"
class sqlParser
{
private:
	enum opcode_type {DELETE, SELECT, CREATE, INSERT};
	map<string, Table>* tables;
	opcode_type type;
	Table* table;
	vector<string> columns;
	string where;
	string sval;
	Row row;
	int ival;
public:
	sqlParser(map<string, Table>* tables) :
		type(SELECT), tables(tables), table(NULL), columns(),
		where(""), sval(""), ival(0), row()
	{}
	void parse(string str);
};