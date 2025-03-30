// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "table.hpp"
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <limits>
using namespace std;

Table::Table(const string &name, vector<ColumnType> &col_types, vector<string> &col_names)
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

void Table::insert() {
    size_t num_rows_inserted;
    string dummy;
    cin >> num_rows_inserted >> dummy;

    size_t start = data.size();
    size_t num_cols = column_names.size();

    data.resize(start + num_rows_inserted);

    for (size_t i = start; i < start + num_rows_inserted; ++i) {
        data[i].reserve(num_cols);

        for (size_t j = 0; j < num_cols; ++j) {
            string raw;
            cin >> raw;

            Field field = make_field(raw, column_types[j]);
            data[i].emplace_back(std::move(field));

            // update index if needed
            if (current_index_type != IndexType::NONE && j == static_cast<size_t>(index_col)) {
                const Field& key = data[i][j];
                if (current_index_type == IndexType::HASH) {
                    hash_index[key].push_back(i);
                } else {
                    bst_index[key].push_back(i);
                }
            }
        }
    }

    print_inserted(start, num_rows_inserted);
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

void Table::print_condition(vector<string> &cols_to_print, bool quiet) {
    unordered_map<string, size_t> colname_to_index;
    for (size_t i = 0; i < column_names.size(); ++i) {
        colname_to_index[column_names[i]] = i;
    }

    // Validate all print columns early
    for (const string& col : cols_to_print) {
        if (!colname_to_index.count(col)) {
            cout << "Error during PRINT: " << col << " does not name a column in " << name << "\n";
            string dummy;
            getline(cin, dummy);
            return;
        }
    }

    string col_name;
    cin >> col_name;

    if (!colname_to_index.count(col_name)) {
        cout << "Error during PRINT: " << col_name << " does not name a column in " << name << "\n";
        string dummy;
        getline(cin, dummy);
        return;
    }

    string op, raw_value;
    cin >> op >> raw_value;

    size_t cond_index = colname_to_index[col_name];
    ColumnType cond_type = column_types[cond_index];
    Field target = make_field(raw_value, cond_type);

    vector<size_t> matched_rows;
    size_t matched_count = 0;

    // try index
    if (current_index_type == IndexType::BST && index_col == static_cast<int>(cond_index)) {
        auto it = bst_index.begin();
        if (op == "=") {
            auto found = bst_index.find(target);
            if (found != bst_index.end()) {
                for (size_t idx : found->second) {
                    matched_rows.push_back(idx);
                }
            }
        } else if (op == "<") {
            for (it = bst_index.begin(); it != bst_index.lower_bound(target); ++it) {
                for (size_t idx : it->second) {
                    matched_rows.push_back(idx);
                }
            }
        } else if (op == ">") {
            for (it = bst_index.upper_bound(target); it != bst_index.end(); ++it) {
                for (size_t idx : it->second) {
                    matched_rows.push_back(idx);
                }
            }
        }
    }
    else if (current_index_type == IndexType::HASH && index_col == static_cast<int>(cond_index) && op == "=") {
        auto found = hash_index.find(target);
        if (found != hash_index.end()) {
            for (size_t idx : found->second) {
                matched_rows.push_back(idx);
            }
        }
    }
    else {
        // fallback to full scan
        for (size_t i = 0; i < data.size(); ++i) {
            if (matches_condition(data[i][cond_index], op, raw_value, cond_type)) {
                matched_rows.push_back(i);
            }
        }
    }

    matched_count = matched_rows.size();

    // header
    if (!quiet) {
        for (size_t i = 0; i < cols_to_print.size(); ++i) {
            cout << cols_to_print[i] << " ";
        }
        cout << "\n";

        // data rows
        for (size_t i : matched_rows) {
            const auto& row = data[i];
            for (size_t j = 0; j < cols_to_print.size(); ++j) {
                size_t col_index = colname_to_index[cols_to_print[j]];
                cout << row[col_index] << " ";
            }
            cout << "\n";
        }
    }

    cout << "Printed " << matched_count << " matching rows from " << name << "\n";
}

void Table::delete_(){
    string col_name;
    cin >> col_name >> col_name;

    size_t col_index = numeric_limits<size_t>::max();
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (column_names[i] == col_name) {
            col_index = i;
            break;
        }
    }

    if (col_index == numeric_limits<size_t>::max()) {
        cout << "Error during DELETE: " << col_name << " does not name a column in " << name << "\n";
        return;
    }

    string op, raw_value;
    cin >> op >> raw_value;

    ColumnType type = column_types[col_index];

    // use std::remove_if to filter rows
    auto new_end = std::remove_if(data.begin(), data.end(),
        [&](const vector<Field>& row) {
            const Field& field = row[col_index];
            return matches_condition(field, op, raw_value, type);
        }
    );

    size_t deleted_count = static_cast<size_t>(distance(new_end, data.end()));
    data.erase(new_end, data.end());

    // rebuild index if needed
    if (deleted_count > 0 && current_index_type != IndexType::NONE) {
        hash_index.clear();
        bst_index.clear();
        for (size_t i = 0; i < data.size(); ++i) {
            const Field& key = data[i][static_cast<size_t>(index_col)];
            if (current_index_type == IndexType::HASH) {
                hash_index[key].push_back(i);
            } else {
                bst_index[key].push_back(i);
            }
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

    // find the column index
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
        

    // clear any previous index (only one allowed at a time)
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

// ================================== HELPERS ==========================================

Field Table::make_field(const std::string& raw, ColumnType type) const {
    switch (type) {
        case ColumnType::Int: return Field(std::stoi(raw));
        case ColumnType::Double: return Field(std::stod(raw));
        case ColumnType::Bool: return Field(raw == "true" || raw == "1");
        case ColumnType::String: return Field(raw);
    }
    return Field(""); // fallback
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

size_t Table::get_col_index(const string& col_name) const {
    for (size_t i = 0; i < column_names.size(); ++i) {
        if (column_names[i] == col_name) {
            return i;
        }
    }
    return std::numeric_limits<size_t>::max();
}

const vector<vector<Field>> & Table::get_data() const {
    return data;
}

// ================================== HELPERS ==========================================
