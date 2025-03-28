// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef TABLE_HPP
#define TABLE_HPP

#include <vector>
#include <iostream>
#include "Field.h"

using namespace std;

class Table{

    private:
        string name;
        vector<ColumnType> column_types;
        vector<string> column_names;
        vector<vector<Field>> data;
    public:
        Table(const string &name, vector<ColumnType> &col_types, vector<string> &col_names);
        void print_created();

};

#endif // TABLE_HPP