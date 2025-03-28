// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "silly.hpp"
#include "table.hpp"

using namespace std;

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
        comment();
        break;
    case 'R':
        remove();
        break;
    case 'I':
        insert();
        break;
    case 'P':
        print();
        break;
    case 'D':
        delete_();
        break;
    case 'J':
    {
        string dummy;
        getline(cin, dummy);
        break;
    }
    case 'G':
    {
        string dummy;
        getline(cin, dummy);
        break;
    }

    default:
        break;
    }
}

// you need to grab table name, then make two vectors with the col types , names
void Silly::create(){ // const string &table_name, int num_cols, vector<ColumnType> &col_types, vector<string> &col_names
    string table_name;
    cin >> table_name;

    if (tables.find(table_name) != tables.end()) {
        std::cout << "Error during CREATE: Cannot create already existing table " << table_name << "\n";
        getline(cin,table_name);
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

// get rid of stuff after #
void Silly::comment(){
    string dummy;
    getline(cin, dummy);
}

// delete tables[table_name]
void Silly::remove(){

    string table_name;
    cin >> table_name;

    // if there's no table_name in tables, error!!
    if (tables.find(table_name) == tables.end())
        print_table_not_in("REMOVE", table_name);

    delete tables[table_name];
    tables.erase(table_name);
    cout << "Table " << table_name << " removed\n";
}

void Silly::insert(){
    string table_name;
    cin >> table_name;
    cin >> table_name;

    // if there's no table_name in tables, error!!
    if (tables.find(table_name) == tables.end()) 
        print_table_not_in("INSERT", table_name);
    
    tables[table_name]->insert();
}

void Silly::print(){
    string table_name;
    cin >> table_name;
    cin >> table_name;

    // if there's no table_name in tables, error!!
    if (tables.find(table_name) == tables.end()) 
        print_table_not_in("PRINT", table_name);
    
    tables[table_name]->print(options.quiet);
}

void Silly::delete_(){
    string table_name;
    cin >> table_name;
    cin >> table_name;

    if (tables.find(table_name) == tables.end()) 
    print_table_not_in("DELETE", table_name);

    tables[table_name]->delete_();
}

void Silly::join(){
    return;
}

void Silly::generate(){
    string table_name;
    cin >> table_name;
    cin >> table_name;

    if (tables.find(table_name) == tables.end()) 
    print_table_not_in("PRINT", table_name);

    tables[table_name]->generate();

}

void Silly::run(){
    while (!options.quit){
        if (cin.fail()) {
            cerr << "Error: Reading from cin has failed" << endl;
            exit(1);
        }        
        cout << "% ";
        read_command();
    }
}

int main(int argc, char **argv){
    ios_base::sync_with_stdio(false);
    cin >> std::boolalpha;  // add these two lines
    cout << std::boolalpha; // add these two lines

    Options options;
    parse_options(argc,argv,options);

    Silly silly(options);

    silly.run();
    return 0;
}

// ==================================RANDOMSHIT==========================================

ColumnType Silly::string_to_col_type(const std::string& str) {
    if (str == "string") 
        return ColumnType::String;
    if (str == "double") 
        return ColumnType::Double;
    if (str == "int") 
        return ColumnType::Int;
    else 
        return ColumnType::Bool;
}

void Silly::print_table_not_in(const string& command, const string& table_name){
    string dummy;
    std::cout << "Error during "<< command << ": " << table_name << " does not name a table in the database\n";
    getline(cin,dummy);
    return;
}

// ==================================RANDOMSHIT==========================================
