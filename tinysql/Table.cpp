#include "Table.h"
void Table::insert(Row row)
{
	rows.push_back(row);
}

void Table::deleteSTR(vector<sqlHeader> headers, string WHERE, string value)
{
	for (auto p_row = rows.begin(); p_row != rows.end();) {
		if (p_row->strings[WHERE] == value)	rows.erase(p_row);
		else p_row++;
	}
}

void Table::deleteINT(vector<sqlHeader> headers, string WHERE, int value)
{
	for (auto p_row = rows.begin(); p_row != rows.end();) {
		if (p_row->ints[WHERE] == value) rows.erase(p_row);
		else p_row++;
	}
}

static Row filter_row(vector<sqlHeader> headers, Row& row) {
	Row new_row;
	for (auto& p_col : headers) {
		if (p_col.type == SQL_STR) new_row.strings.insert(pair<string, string>(p_col.name, row.strings.at(p_col.name)));
		else new_row.ints.insert(pair<string, int>(p_col.name, row.ints.at(p_col.name)));
	}
	return new_row;
}

Table Table::selectSTR(vector<sqlHeader> headers, string WHERE, string value)
{
	vector<Row> new_rows;
	for (auto p_row = rows.begin(); p_row != rows.end(); p_row++) {
		if (p_row->strings[WHERE] == value) {
			new_rows.push_back(filter_row(headers, *p_row));
		}
		else p_row++;
	}
	Table new_table = Table(headers);
	new_table.rows = new_rows;
	return new_table;
}

Table Table::selectINT(vector<sqlHeader> headers, string WHERE, int value)
{
	vector<Row> new_rows;
	for (auto p_row = rows.begin(); p_row != rows.end(); p_row++) {
		if (p_row->ints[WHERE] == value) {
			new_rows.push_back(filter_row(headers, *p_row));
		}
		else p_row++;
	}
	Table new_table = Table(headers);
	new_table.rows = new_rows;
	return new_table;
}
