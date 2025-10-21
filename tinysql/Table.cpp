#include "Table.h"
#include <iostream>
#include <exception>
void Table::assert_headers(vector<sqlHeader> hdrs)
{
	for (auto& col : hdrs) {
		if (headers_set.find(col) == headers_set.end()) {
			string stype = "INT.";
			if (col.type == SQL_STR) stype = "STR.";
			string errmsg = "Table"+table_name+" has no header named" + col.name + " of type " + stype;
			throw new exception(errmsg.c_str());
		}
	}
}
void Table::insert(Row row)
{
	for (auto& col : headers) {
		assert_column_exists(col, row);
	}
	rows.push_back(row);
}
static inline void assert_column_exists(const sqlHeader& col, Row& row) {
	if (col.type == SQL_INT && row.ints.find(col.name) == row.ints.end()) {
		string errmsg = "Row missing column " + col.name + " of type INT.";
		throw new exception(errmsg.c_str());
	}
	else if (row.strings.find(col.name) == row.strings.end()) {
		string errmsg = "Row missing column " + col.name + " of type STR.";
		throw new exception(errmsg.c_str());
	}
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

static Row filter_row(vector<sqlHeader> hdrs, Row& row) {
	Row new_row;
	for (auto& col : hdrs) {
		assert_column_exists(col, row);
		if (col.type == SQL_STR)
			new_row.strings.insert(pair<string,string>(col.name,row.strings.at(col.name)));
		else
			new_row.ints.insert(pair<string, int>(col.name, row.ints.at(col.name)));
	}
	return new_row;
}

Table Table::selectSTR(vector<sqlHeader> hdrs, string WHERE, string value)
{
	vector<Row> new_rows;
	for (auto p_row = rows.begin(); p_row != rows.end(); p_row++) {
		if (p_row->strings[WHERE] == value) {
			new_rows.push_back(filter_row(hdrs, *p_row));
		}
		else p_row++;
	}
	Table new_table = Table("", hdrs);
	new_table.rows = new_rows;
	return new_table;
}

Table Table::selectINT(vector<sqlHeader> hdrs, string WHERE, int value)
{
	vector<Row> new_rows;
	for (auto p_row = rows.begin(); p_row != rows.end(); p_row++) {
		if (p_row->ints[WHERE] == value) {
			new_rows.push_back(filter_row(hdrs, *p_row));
		}
		else p_row++;
	}
	Table new_table = Table(hdrs);
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
