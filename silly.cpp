// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "silly.hpp"
#include "table.hpp"
#include <limits>

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
        join();
        break;
    }
    case 'G':
    {
        generate();        
        break;
    }
    default: { 
        string dummy;
        getline(cin,dummy);
        cout << "Error: unrecognized command\n";
        break;
        }    
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
    cin >> table_name >> table_name;

    if (tables.find(table_name) == tables.end()) 
    print_table_not_in("DELETE", table_name);

    tables[table_name]->delete_();
}

void Silly::join(){
    string dummy, table_name1, table_name2;
    cin >> table_name1 >> dummy >> table_name2 >> dummy;

    if (tables.find(table_name1) == tables.end()) 
        print_table_not_in("JOIN", table_name1);

    if (tables.find(table_name2) == tables.end()) 
        print_table_not_in("JOIN", table_name2);

    Table * table1 = tables[table_name1];
    Table * table2 = tables[table_name2];

    string col_1, col_2;
    cin >> col_1 >> dummy >> col_2 >> dummy >> dummy;

    size_t col_1_idx = table1->get_col_index(col_1);
    size_t col_2_idx = table2->get_col_index(col_2);

    if (col_1_idx == numeric_limits<size_t>::max()) {
        cout << "Error during JOIN: " << col_1 << " does not name a column in " << table_name1 << "\n";
        return;
    }
    if (col_2_idx == numeric_limits<size_t>::max()) {
        cout << "Error during JOIN: " << col_2 << " does not name a column in " << table_name2 << "\n";
        return;
    }

    size_t n;
    cin >> n;

    vector<pair<string, size_t>> cols_to_print; // {colname, table_number}
    cols_to_print.reserve(n);
    for (size_t i = 0; i < n; ++i){
        string col_name;
        size_t table_num;
        cin >> col_name >> table_num;
        size_t col_to_print_idx;
        if (table_num == 1) {
            col_to_print_idx = table1->get_col_index(col_name);
            if (col_to_print_idx == numeric_limits<size_t>::max()) {
                cout << "Error during JOIN: " << col_name << " does not name a column in " << table_name1 << "\n";
                return;
            }
        } // if col is in table1
        else{
            col_to_print_idx = table2->get_col_index(col_name);
            if (col_to_print_idx == numeric_limits<size_t>::max()) {
                cout << "Error during JOIN: " << col_name << " does not name a column in " << table_name2 << "\n";
                return;
            }
        } // if col is in table2
        cols_to_print.emplace_back(col_name,table_num);        
    } // for

    // print header if !quiet mode
    if (!options.quiet){
        for (size_t i = 0; i < cols_to_print.size(); ++i) {
            cout << cols_to_print[i].first << " ";
        }
        cout << "\n";
    }
    
    size_t printed_count = 0;
    
    // if we have an index on table2 and its on the right column
    if (table2->current_index_type != Table::IndexType::NONE && table2->index_col == static_cast<int>(col_2_idx)) {
        const auto& data1 = table1->get_data();
        for (const auto& row1 : data1) {
            const Field& key = row1[col_1_idx];
            const vector<size_t>* matches = nullptr;

            if (table2->current_index_type == Table::IndexType::HASH) {
                auto it = table2->hash_index.find(key);
                if (it != table2->hash_index.end()) {
                    matches = &it->second;
                }
            } else {
                auto it = table2->bst_index.find(key);
                if (it != table2->bst_index.end()) {
                    matches = &it->second;
                }
            }

            if (matches) {
                for (size_t idx : *matches) {
                        const auto& row2 = table2->get_data()[idx];
                        for (size_t i = 0; i < cols_to_print.size(); ++i) {
                            const auto& [colname, table_num] = cols_to_print[i];
                            if (!options.quiet) {
                                if (table_num == 1) {
                                    size_t idx1 = table1->get_col_index(colname);
                                    cout << row1[idx1];
                                } else {
                                    size_t idx2 = table2->get_col_index(colname);
                                    cout << row2[idx2];
                                }
                                cout << " ";
                            }
                        }
                        if (!options.quiet) {
                            cout << "\n";
                        }
                ++printed_count;
                }
            }
        }
    }
    // fallback version - no indices
    else {
        const auto& data1 = table1->get_data();
        const auto& data2 = table2->get_data();

        unordered_map<Field, vector<size_t>> temp_hash_index;
        for (size_t i = 0; i < data2.size(); ++i) {
            const Field& key = data2[i][col_2_idx];
            temp_hash_index[key].push_back(i);
        }

        // for each row in table 1, 
        for (const auto& row1 : data1) {
            // check that column for each row
            const Field& key = row1[col_1_idx];
            auto it = temp_hash_index.find(key);
            if (it != temp_hash_index.end()) {
                for (size_t idx : it->second) {
                    const auto& row2 = data2[idx];
                    if (!options.quiet) {
                        for (size_t i = 0; i < cols_to_print.size(); ++i) {
                            const auto& [colname, table_num] = cols_to_print[i];
                            if (table_num == 1) {
                                size_t idx1 = table1->get_col_index(colname);
                                cout << row1[idx1];
                            } else {
                                size_t idx2 = table2->get_col_index(colname);
                                cout << row2[idx2];
                            }
                            cout << " ";
                        }
                        cout << "\n";
                    }
                    ++printed_count;
                }
            }
        }
    }
    cout << "Printed " << printed_count << " rows from joining " << table_name1 << " to " << table_name2 << "\n";
}

void Silly::generate(){
    string table_name;
    cin >> table_name;
    cin >> table_name;

    if (tables.find(table_name) == tables.end()) 
    print_table_not_in("GENERATE", table_name);

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

// ================================== RANDOM ==========================================

ColumnType Silly::string_to_col_type(const string& str) {
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

// ================================== RANDOM ==========================================
