#include "Table.h"
#include <iostream>
#include <exception>
void Table::assert_columns(vector<string> columns)
{
	for (auto& name : columns) {
		if (headers_map.find(name)== headers_map.end()) {
			cerr << "Table "<< table_name << " has no header named " << name << ".";
			throw new exception();
		}
	}
}
void Table::insert(Row row)
{
	for (auto& col : headers) {
		if (col.type == SQL_INT && row.ints.find(col.name) == row.ints.end()) {
			string errmsg = "Row missing column " + col.name + " of type INT.";
			throw new exception(errmsg.c_str());
		}
		else if (col.type == SQL_STR && row.strings.find(col.name) == row.strings.end()) {
			string errmsg = "Row missing column " + col.name + " of type STR.";
			throw new exception(errmsg.c_str());
		}
	}
	rows.push_back(row);
}

void Table::deleteSTR(string WHERE, string value)
{
	for (auto p_row = rows.begin(); p_row != rows.end();) {
		if (p_row->strings[WHERE] == value)	rows.erase(p_row);
		else p_row++;
	}
}

void Table::deleteINT(string WHERE, int value)
{
	for (auto p_row = rows.begin(); p_row != rows.end();) {
		if (p_row->ints[WHERE] == value) rows.erase(p_row);
		else p_row++;
	}
}

static Row filter_row(vector<sqlHeader>& hdrs, Row& row) {
	Row new_row = {{}, {}};
	for (auto& col : hdrs) {
		if (col.type == SQL_STR)
			new_row.strings.insert(pair<string,string>(col.name,row.strings.at(col.name)));
		else
			new_row.ints.insert(pair<string, int>(col.name, row.ints.at(col.name)));
	}
	return new_row;
}

Table Table::select_where(vector<string> columns, string WHERE, string svalue, int ivalue, column_type type)
{
	vector<Row> new_rows;
	vector<sqlHeader> hdrs;
	assert_columns(columns);
	for (string& s : columns) {
		hdrs.push_back({s, headers_map[s]});
	}
	for(Row& row : rows) {
		if (type == SQL_STR && row.strings[WHERE] == svalue ||
			type == SQL_INT && row.ints[WHERE] == ivalue) {
			new_rows.push_back(filter_row(hdrs, row));
		}
	}
	Table new_table = Table("", hdrs);
	new_table.rows = new_rows;
	return new_table;
}
Table Table::select(vector<string> columns)
{
	vector<Row> new_rows;
	vector<sqlHeader> hdrs;
	assert_columns(columns);
	for (string& s : columns) {
		hdrs.push_back({s, headers_map[s]});
	}
	for (Row& row : rows) {
		new_rows.push_back(filter_row(hdrs, row));
	}
	Table new_table = Table("", hdrs);
	new_table.rows = new_rows;
	return new_table;
}

void Table::print_table()
{
	cout << "------------------------------" << endl;
	cout << "Columns: ";
	for (auto& c : headers)
		cout << c.name << "|";
	cout << endl;
	for (auto& row : rows) {
		for (auto& c : headers) {
			if(c.type == SQL_INT) cout << row.ints[c.name] << "|";
			else cout << row.strings[c.name] << "|";
		}
		cout << endl;
	}
	cout << "------------------------------" << endl;
}
