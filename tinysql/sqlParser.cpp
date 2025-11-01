#include "sqlParser.h"
#include<regex>
#include<queue>
#include <iostream>

//string trim_whitespace(string str)
//{
//
//    string result = "";
//    if (iswspace(str[0])) str = str.substr(1);
//    for (size_t i = 0; i < str.length(); i++) {
//        if (iswspace(str[i])) result.push_back(' ');
//        else result += str[i];
//        while (str[i + 1] && iswspace(str[i + 1])) i++;
//    }
//    if (iswspace(result.back())) {
//        result.pop_back();
//    }
//    return result;
//}

//static string str_to_upper(string s) {
//    string result = "";
//    for (char c : s) {
//        result += toupper(c);
//    }
//    return result;
//}

// Check if a string contains only digits.
//static bool is_num(string s) {
//    if (s.empty()) return false;
//    for (char c : s) {
//        if (c < 0x30 || c>0x39) return false;
//    }
//}

#define MATCHVAL "('[^']+'|\\d+)" // STR has double quotes, INT has only digits.
#define PARAN(ST) "\\(" ST "\\)" // Literal parantheses.
#define LISTOF(ST) "(?:" ST ", ?)*" ST // item, item,item,..., item
#define MATCHVALUES PARAN("(" LISTOF(MATCHVAL) ")") // Matches values in "('a', 5, 'abb')".
#define LISTCOLS LISTOF("\\w+") // Matches columns in "col1,col2, col3".
#define MATCHCOLS "(" LISTOF("\\w+") ")"
#define MATCHCREATE PARAN("(" LISTOF("\\w+ (?:STR|INT)") ")") // Matches (col1 STR, col2 INT, col3 INT,...)
enum opcode_type {
    DELETE, SELECT, CREATE, INSERT
};
vector<string> split(string list, string delimiters) {
    vector<string> vector;
    smatch matcher;
    string s = "[^" + delimiters + "]+";
    regex finder(s);
    while (!list.empty()) {
        regex_search(list, matcher, finder);
        vector.push_back(matcher[0].str());
        list = matcher.suffix().str();
    }
    return vector;
}

void sqlParser::insert(string str, Table* table)
{
    Row row;
    smatch columns_match;
    regex columns_reg(PARAN(MATCHCOLS) " VALUES");
    if (!regex_search(str, columns_match, columns_reg)) {
        cerr << "Illegal INSERT command! Syntax: INSERT INTO [table] [column1,column2,...] VALUES [val1,val2,...]" << endl;
        return;
    }
    vector<string> columns = split(columns_match[1], ", ");
    str = columns_match.suffix().str();
    smatch values_match;
    regex values_reg(MATCHVALUES "$");
    if (!regex_search(str, values_match, values_reg)) {
        cerr << "Illegal INSERT command! Syntax: INSERT INTO [table] [column1,column2,...] VALUES [val1,val2,...]" << endl;
        return;
    }
    vector<string> values = split(values_match[1], ", ");
    //Check legallity of values.
    if (values.size() != columns.size()) {
        cerr << "Number of values does not match the number of columns!" << endl;
        return;
    }
    for (int i = 0; i < values.size(); i++) {
        string val = values[i];
        string column = columns[i];
        auto tmptype = table->get_col_type(column);
        if (val[0] == '\'' && val.back() == '\'') {
            if (tmptype == SQL_INT) {
                cerr << "Value " << val << " cannot be used for a column of type INT!" << endl;
                return;
            }
            row.strings.insert({column, val});
        }
        else {
            if (tmptype == SQL_STR) {
                cerr << "Value " << val << " cannot be used for a column of type STR!" << endl;
                return;
            }
            row.ints.insert({column, atoi(val.c_str())});
        }
    }
    // Handle default values.
    for (auto column : table->get_headers()) {
        if (column.type == SQL_STR) {
            if (row.strings.find(column.name) == row.strings.end()) {
                row.strings.insert({column.name, ""});
            }
        }
        if (column.type == SQL_INT) {
            if (row.ints.find(column.name) == row.ints.end()) {
                row.ints.insert({column.name, 0});
            }
        }
    }
    table->insert(row);
}

void sqlParser::select_delete(vector<string> columns, string str, Table* table, bool is_select)
{
    string sval = "";
    int ival = 0;
    string where = "";
    column_type tmptype = SQL_INT;
    if (!str.empty()) {
        smatch where_match;
        string where_reg_str = " WHERE (\\w+) ? = ?" MATCHVAL "$"; // We allow spaces around the '='.
        regex where_reg(where_reg_str);
        if (!regex_search(str, where_match, where_reg)) {
            cerr << "Illegal WHERE clause! Syntax: WHERE [column] = [val]" << endl;
            return;
        }
        where = where_match[1];
        string val = where_match[2];
        tmptype = table->get_col_type(where);
        if (val[0] == '\'' && val.back() == '\'') {
            sval = val;
            if (tmptype == SQL_INT) {
                cerr << "Cannot compare a column of type INT to an STR!" << endl;
                return;
            }
        }
        else {
            ival = atoi(val.c_str());
            if (tmptype == SQL_STR) {
                cerr << "Cannot compare a column of type STR to an INT!" << endl;
                return;
            }
        }
    }
    if (!is_select) {
        table->delete_where(where, sval, ival, tmptype);
    }
    else {
        if (result != NULL) delete result;
        result = new Table(table->select_where(columns, where, sval, ival, tmptype));
    }
}

void sqlParser::create(string table_name, string str)
{
    smatch create_match;
    regex create_reg(MATCHCREATE);
    if (!regex_search(str, create_match, create_reg)) {
        cerr << "Illegal CREATE syntax! Syntax: CREATE TABLE table_name (col1 type1, col2 type2,...)" << endl;
        return;
    }
    vector<string> matches = split(create_match[1], ", ");
    vector<sqlHeader> new_columns;
    for (int i = 0; i < matches.size(); i += 2) {
        column_type tmptype = matches[i + 1] == "STR" ? SQL_STR : SQL_INT;
        new_columns.push_back({matches[i],tmptype});
    }
    tables.insert({table_name,Table(table_name, new_columns)});
    //result = &tables.find(table_name)->second;
}

void sqlParser::parse(string str)
{
    if (result != NULL) {
        delete result;
        result = NULL;
    }
    opcode_type type;
    string table_name;
    Table* table = NULL;
    //Get the type of command.
    smatch opcode_match;
    regex opcode_reg("^(SELECT|DELETE FROM|CREATE TABLE|INSERT INTO)", regex_constants::ECMAScript);
    vector<string> columns;
    if (!regex_search(str, opcode_match, opcode_reg)) {
        cerr << "Illegal command!" << endl;
        return;
    }
    string opcode = *opcode_match.begin();
    if (opcode == "SELECT") type = SELECT;
    if (opcode == "DELETE FROM") type = DELETE;
    if (opcode == "CREATE TABLE") type = CREATE;
    if (opcode == "INSERT INTO") type = INSERT;
    //Advance the string.
    str = opcode_match.suffix().str();
    //If it was SELECT, get column names.
    if (type == SELECT) {
        smatch columns_match;
        regex columns_reg(MATCHCOLS " FROM");
        if (!regex_search(str, columns_match, columns_reg)) {
            cerr << "Illegal SELECT command! Syntax: SELECT [column1,column2,...] FROM [table] WHERE [column] = [val]" << endl;
            return;
        }
        columns = split(columns_match[1], ", ");
        str = str.substr(columns_match[0].length());
    }
    //Get table name.
    smatch table_match;
    regex table_reg(" (\\w+)");
    if (!regex_search(str, table_match, table_reg)) {
        cerr << "Illegal table name!" << endl;
        return;
    }
    table_name = table_match[1].str();
    if (type != CREATE) {
        auto p_table = tables.find(table_name);
        if (p_table == tables.end()) {
            cerr << "Table " << table_name << " does not exist!" << endl;
            return;
        }
        table = &(p_table->second);
    }
    else {
        if (tables.find(table_name) != tables.end()) {
            cerr << "Table " << table_name << " already exists!" << endl;
            return;
        }
    }
    str = table_match.suffix().str(); //Advance the string.
    if (type == INSERT) {
        insert(str, table);
    }
    // Handle WHERE clause.
    if (type == DELETE || type == SELECT) {
        select_delete(columns, str, table, (type == SELECT));
    }
    if (type == CREATE) {
        create(table_name, str);
    }
}

sqlParser::~sqlParser()
{
    if (result != NULL) {
        delete result;
    }
}
