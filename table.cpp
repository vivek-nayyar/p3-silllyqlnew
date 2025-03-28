// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "table.hpp"
#include <unordered_map>
#include <limits>
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
    cout << column_names[column_names.size()-1] << " created\n";
}

void Table::print_inserted(size_t num_rows_before, size_t num_rows_added){
    cout << "Added " << num_rows_added << " rows to " << name << " from position " 
    << num_rows_before << " to " << num_rows_before + num_rows_added - 1 << "\n";
}

bool Table::matches_condition(const Field& field, const string& op, const string& raw, ColumnType type){
    switch (type) {
        case ColumnType::Int: {
            int target = std::stoi(raw);
            if (op == "=") return field == target;
            if (op == "<") return field < target;
            if (op == ">") return field > target;
            break;
        }
        case ColumnType::Double: {
            double target = std::stod(raw);
            if (op == "=") return field == target;
            if (op == "<") return field < target;
            if (op == ">") return field > target;
            break;
        }
        case ColumnType::Bool: {
            bool target = (raw == "true" || raw == "1");
            if (op == "=") return field == target;
            break;
        }
        case ColumnType::String: {
            if (op == "=") return field == raw;
            break;
        }
    }
    return false; // fallback if op/type combo is invalid

}

void Table::insert(){
    size_t num_rows_inserted;
    string dummy;
    cin >> num_rows_inserted;
    cin >> dummy;

    size_t num_rows_before = data.size();
    for (size_t i = 0; i < num_rows_inserted; ++i){
        vector<Field>row;
        row.reserve(num_rows_inserted);
        for (size_t col = 0; col < column_names.size(); ++col)
        {
            string raw;
            cin >> raw;

            ColumnType type = column_types[col];

            switch (type) {
                case ColumnType::Int:
                    row.emplace_back(Field(std::stoi(raw)));
                    break;
                case ColumnType::Double:
                    row.emplace_back(Field(std::stod(raw)));
                    break;
                case ColumnType::Bool:
                    row.emplace_back(Field(raw == "true" || raw == "1"));
                    break;
                case ColumnType::String:
                    row.emplace_back(Field(raw));
                    break;
            }
        }
        data.emplace_back(row);
        if (current_index_type != IndexType::NONE) {
            const Field& key = row[static_cast<size_t>(index_col)];
            if (current_index_type == IndexType::HASH) {
                hash_index[key].push_back(data.size() - 1);
            } else {
                bst_index[key].push_back(data.size() - 1);
            }
        }
        
    }
    print_inserted(num_rows_before, num_rows_inserted);
}

void Table::print(bool quiet){
    size_t num_cols;
    cin >> num_cols;

    vector<string>cols_to_print;
    cols_to_print.reserve(num_cols);

    for (size_t i = 0; i < num_cols; i++)
    {
        string col;
        cin >> col;
        cols_to_print.push_back(col);
    }

    string condition;
    cin >> condition;

    if (condition == "WHERE")
    {
        print_condition(cols_to_print, quiet);
    }
    else{
        print_all(cols_to_print, quiet);
    }
}

void Table::print_all(vector<string> &cols_to_print, bool quiet){
    unordered_map <string, size_t> colname_to_index;
    for (size_t i = 0; i < column_names.size(); ++i) {
        colname_to_index[column_names[i]] = i;
    }

    for (const std::string& col : cols_to_print) {
        if (colname_to_index.find(col) == colname_to_index.end()) {
            cout << "Error during PRINT: " << col << " does not name a column in " << name << "\n";
            return;
        }
    }
    
    if (!quiet) {

        // header at the top
        for (size_t i = 0; i < cols_to_print.size(); ++i) {
            cout << cols_to_print[i];
            if (i < cols_to_print.size() - 1)
                cout << " ";
        }
        cout << "\n";
    
        // rows 
        for (const auto& row : data) {
            for (size_t i = 0; i < cols_to_print.size(); ++i) {
                const string& col = cols_to_print[i];
                size_t col_index = colname_to_index[col];
                cout << row[col_index];
                if (i < cols_to_print.size() - 1) 
                    cout << " ";
            }
            cout << "\n";
        }
    }
    cout << "Printed " << data.size() <<  " matching rows from " << name << "\n";
}

void Table::print_condition(vector<string> &cols_to_print, bool quiet){
    unordered_map <string, size_t> colname_to_index;
    for (size_t i = 0; i < column_names.size(); ++i) {
        colname_to_index[column_names[i]] = i;
    }

    for (const std::string& col : cols_to_print) {
        if (colname_to_index.find(col) == colname_to_index.end()) {
            cout << "Error during PRINT: " << col << " does not name a column in " << name << "\n";
            string dummy;
            getline(cin,dummy);
            return;
        }
    }

    string col_name; // condition col name
    cin >> col_name;

    auto it = colname_to_index.find(col_name);
    if (it == colname_to_index.end()){
        cout << "Error during PRINT: " << col_name << " does not name a column in " << name << "\n";
        string dummy;
        getline(cin,dummy);
        return;
    }

    string op;
    cin >> op;

    string raw_value;
    cin >> raw_value;
    
    size_t cond_index = it->second;
    ColumnType cond_type = column_types[cond_index];


    // header at the top
    if (!quiet) {
        for (size_t i = 0; i < cols_to_print.size(); ++i) {
            cout << cols_to_print[i];
            if (i < cols_to_print.size() - 1)
                cout << " ";
        }
        cout << "\n";
    }
    
    size_t rows_matched = 0;
    for (const auto& row : data) {
        const Field& cond_field = row[cond_index];
    
        if (!matches_condition(cond_field, op, raw_value, cond_type)) {
            continue; // skip this row if condition doesn't match
        }
    
        // matched — print the row
        ++rows_matched;
        if (!quiet){ 
            for (size_t i = 0; i < cols_to_print.size(); ++i) {
                const std::string& col = cols_to_print[i];
                size_t col_index = colname_to_index[col];
                std::cout << row[col_index];
        
                if (i < cols_to_print.size() - 1)
                    std::cout << " ";
            }
            std::cout << "\n";
        }
    }

    cout << "Printed " << rows_matched <<  " matching rows from " << name << "\n";
}

void Table::delete_(){
    string col_name;
    cin >> col_name;
    cin >> col_name;

    size_t col_index = std::numeric_limits<size_t>::max();
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (column_names[i] == col_name) {
            col_index = i;
            break;
        }
    }

    if (col_index == std::numeric_limits<size_t>::max()) {
        cout << "Error during DELETE: " << col_name << " does not name a column in " << name << "\n";
        return;
    }

    string op;
    cin >> op;

    string raw_value;
    cin >> raw_value;

    ColumnType type = column_types[col_index];
    int deleted_count = 0;

    auto it = data.begin();
    while (it != data.end()) {
        const Field& field = (*it)[col_index];
        if (matches_condition(field, op, raw_value, type)) {
            it = data.erase(it);
            ++deleted_count;
        } else {
            ++it;
        }
    }
    cout << "Deleted " << deleted_count << " rows from " << name << "\n";
}

void Table::generate(){
    string index_type;
    cin >> index_type;

    string col_name;
    cin >> col_name; 
    cin >> col_name; 
    cin >> col_name;

    // Step 1: Find the column index
    int col_index = -1;
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (column_names[i] == col_name) {
            col_index = static_cast<int>(i);
            break;
        }
    }

    if (col_index == -1) {
        cout << "Error during GENERATE: " << col_name << " does not name a column in " << name << "\n";
        return;
    }
        

    // Clear any previous index (only one allowed at a time)
    hash_index.clear();
    bst_index.clear();
    index_col = col_index;
    current_index_type = (index_type == "hash") ? IndexType::HASH : IndexType::BST;
    
    // build the new index
    for (size_t row_idx = 0; row_idx < data.size(); ++row_idx) {
        const Field& key = data[row_idx][static_cast<size_t>(col_index)];

        if (current_index_type == IndexType::HASH) {
            hash_index[key].push_back(row_idx);
        } else {
            bst_index[key].push_back(row_idx);
        }
    }
    
    size_t distinct_keys = (current_index_type == IndexType::HASH)
                            ? hash_index.size()
                            : bst_index.size();
    
    std::cout << "Generated " << index_type
                << " index for table " << name
                << " on column " << col_name
                << ", with " << distinct_keys << " distinct keys\n";
}