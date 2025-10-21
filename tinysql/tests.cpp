#include "tests.h"
#include <iostream>
void test_table1() {
	vector<sqlHeader> columns1 = {{"Name", SQL_STR}, {"Age", SQL_INT}};
	Table t1 = Table("People", columns1);
	Row row1 = {{{"Name","Joe"}}, {{"Age", 40}}};
	Row row2 = {{{"Name","Joe"}}, {{"Age", 40}}};
	Row row3 = {{{"Name","Dennis"}}, {{"Age", -1}}};
	Row row4 = {{{"Name","ErrorMan"}}, {{"Age", -1}}};
	t1.insert(row1);
	t1.insert(row2);
	t1.insert(row3);
	t1.insert(row4);
	cout << "First table:" << endl;
	t1.print_table();
	cout << "SELECT Name FROM People" << endl;
	Table t2 = t1.select({"Name"});
	t2.print_table();
	cout << "SELECT Name FROM People WHERE age=-1" << endl;
	Table t3 = t1.select_where({"Name"}, "Age", "", -1, SQL_INT);
	t3.print_table();
}