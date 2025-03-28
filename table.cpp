#include "table.hpp"
using namespace std;

Table::Table(const string &name,
            vector<ColumnType> &col_types,
            vector<string> &col_names)
    : name(name), column_types(col_types), column_names(col_names) {}

void Table::print_created(){
    cout << "New table " << name << " with column(s) ";
    for (size_t i = 0; i < column_names.size() - 1; ++i){
        cout << column_names[i] << " ";
    }
    cout << column_names[column_names.size()-1] << "\n";
}