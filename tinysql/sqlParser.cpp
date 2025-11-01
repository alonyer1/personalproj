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
#define MATCHVALUES PARAN(LISTOF(MATCHVAL)) // Matches values in "('a', 5, 'abb')".
#define MATCHCOLS LISTOF("(\\w+)") // Matches columns in "col1,col2, col3".

void sqlParser::parse(string str)
{
    smatch opcode_match;
    regex opcode_reg("(SELECT|DELETE FROM|CREATE TABLE|INSERT INTO)");
    if (!regex_match(str, opcode_match, opcode_reg)) {
        cerr << "Illegal command!" << endl;
        throw;
    }
    string opcode = *opcode_match.begin();
    if (opcode == "SELECT") type = SELECT;
    if (opcode == "DELETE FROM") type = DELETE;
    if (opcode == "CREATE TABLE") type = CREATE;
    if (opcode == "INSERT INTO") type = INSERT;
    //Advance the string.
    str = str.substr(opcode.length());
    //If it was SELECT, get column names.
    if (type == SELECT) {
        smatch columns_match;
        regex columns_reg("( " MATCHCOLS " FROM)");
        if (!regex_match(str, columns_match, columns_reg)) {
            cerr << "Illegal SELECT command! Syntax: SELECT [column1,column2,...] FROM [table] WHERE [column] = [val]" << endl;
            throw;
        }
        columns = vector<string>(columns_match.begin()+1, columns_match.end());
        str = str.substr(columns_match[0].length());
    }
    //Get table name.
    smatch table_match;
    regex table_reg("( (\\w+))");
    if (!regex_match(str, table_match, table_reg)) {
        cerr << "Illegal table name!" << endl;
        throw;
    }
    auto p_table = tables->find(table_match[1]);
    if (p_table == tables->end()) {
        cerr << "Table " << table_match[1] << " doesn't exist!" << endl;
        throw;
    }
    table = &(p_table->second);
    str = str.substr(table_match[0].length()); //Advance the string.

    if (type == INSERT) {
        Row row;
        smatch columns_match;
        regex columns_reg("( " PARAN(MATCHCOLS) " VALUES)");
        if (!regex_match(str, columns_match, columns_reg)) {
            cerr << "Illegal INSERT command! Syntax: INSERT INTO [table] [column1,column2,...] VALUES [val1,val2,...]" << endl;
            throw;
        }
        columns = vector<string>(columns_match.begin() + 1, columns_match.end());
        str = str.substr(columns_match[0].length());
        smatch values_match;
        regex values_reg(MATCHVALUES "$");
        if (!regex_match(str, values_match, values_reg)) {
            cerr << "Illegal INSERT command! Syntax: INSERT INTO [table] [column1,column2,...] VALUES [val1,val2,...]" << endl;
            throw;
        }
        //Check legallity of values.
        for (int i = 0; i < values_match.length(); i++) {
            string val = values_match[i];
            string column = columns[i];
            auto tmptype = table->get_col_type(column);
            if (val[0] == '\'' && val.back() == '\'') {
                if (tmptype == SQL_INT) {
                    cerr << "Value " << val << " cannot be used for a column of type INT!" << endl;
                    throw;
                }
                row.strings.insert({column, val});
            }
            else {
                if (tmptype == SQL_STR) {
                    cerr << "Cannot compare a column of type STR to an INT!" << endl;
                    throw;
                }
                row.ints.insert({column, atoi(val.c_str())});
            }
        }
    }
    // Handle WHERE clause.
    if (type == DELETE || type == SELECT) {
        if (!str.empty()) {
            smatch where_match;
            string where_reg_str = " WHERE (\\w+) ? = ?" MATCHVAL "$"; // We allow spaces around the '='.
            regex where_reg(where_reg_str);
            if (!regex_match(str, where_match, where_reg)) {
                cerr << "Illegal WHERE clause! Syntax: WHERE [column] = [val]" << endl;
                throw;
            }
            where = where_match[0];
            string val = where_match[1];
            auto tmptype = table->get_col_type(where);
            if (val[0] == '\'' && val.back() == '\'') {
                sval = val;
                if (tmptype == SQL_INT) {
                    cerr << "Cannot compare a column of type INT to an STR!" << endl;
                    throw;
                }
            }
            else {
                ival = atoi(val.c_str());
                if (tmptype == SQL_STR) {
                    cerr << "Cannot compare a column of type STR to an INT!" << endl;
                    throw;
                }
            }
        }
    }

}
