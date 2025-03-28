// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "silly.hpp"
#include "table.hpp"

using namespace std;

ColumnType Silly::string_to_col_type(const std::string& str) {
    if (str == "String") 
        return ColumnType::String;
    if (str == "Double") 
        return ColumnType::Double;
    if (str == "Int") 
        return ColumnType::Int;
    else 
        return ColumnType::Bool;
}


Silly::Silly(const Options &options) : options(options){

}

void Silly::read_command(){
    string command;
    cin >> command;

    char first_char = command[0];

    switch (first_char)
    {
    case 'C':
        create();        
        break;
    case 'Q':
        quit();
        break;
    case '#':
        /* code */
        break;
    case 'R':
        /* code */
        break;
    case 'I':
        /* code */
        break;
    case 'P':
        /* code */
        break;
    case 'D':
        /* code */
        break;
    case 'J':
        /* code */
        break;
    case 'G':
        /* code */
        break;


    default:
        break;
    }
}

// you need to grab table name, then make two vectors with the col types , names
void Silly::create(){ // const string &table_name, int num_cols, vector<ColumnType> &col_types, vector<string> &col_names
    string table_name;
    cin >> table_name;

    if (tables.find(table_name) != tables.end()) {
        std::cerr << "Table \"" << table_name << "\" already exists.\n";
        return;
    }

    int vec_size;
    cin >> vec_size;

    vector<ColumnType> col_types;
    vector<string> col_names;

    for(int i = 0; i < vec_size; ++i){
        string col_type;
        cin >> col_type;
        col_types.push_back(string_to_col_type(col_type));
    }

    for(int i = 0; i < vec_size; ++i){
        string col_name;
        cin >> col_name;
        col_names.push_back(col_name);
    }

    Table *table = new Table(table_name, col_types, col_names);
    tables[table_name] = table;
    table->print_created();

}

// clear memory, exit return 0, print "Thanks for being silly!\n"
void Silly::quit(){
    for (auto pair : tables){
        delete pair.second;
    }
    cout << "Thanks for being silly!\n";
    options.quit = true; 
}

void Silly::remove(){

}

void Silly::run(){
    while (!options.quit){
        read_command();
    }
}

int main(int argc, char **argv){
    ios_base::sync_with_stdio(false);

    Options options;
    parse_options(argc,argv,options);

    Silly silly(options);

    silly.run();
    return 0;
}