// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef TABLE_HPP
#define TABLE_HPP

#include <vector>
#include <iostream>
#include "Field.h"
#include <unordered_map>
#include <map>
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
        void print_inserted(size_t num_rows_before, size_t num_rows_added);
        void insert();
        void print(bool quiet);
        void print_all(vector<string> &cols_to_print, bool quiet);
        void print_condition(vector<string> &cols_to_print, bool quiet);
        void delete_();
        void generate();

        // ============================== HELPERS ====================================
        size_t get_col_index(const string& col_name) const; 
        const vector<vector<Field>> &get_data() const;
        bool matches_condition(const Field& field, const string& op, const string& raw, ColumnType type);
        Field make_field(const std::string& raw, ColumnType type) const;
        // ============================== HELPERS ====================================

        // ============================== INDICES STUFF ==============================
        unordered_map<Field, vector<size_t>> hash_index; //unsorted map
        map<Field, vector<size_t>> bst_index; // sorted map
        int index_col = -1; // which column is currently indexed
        enum class IndexType { NONE, HASH, BST };
        IndexType current_index_type = IndexType::NONE;
        // ============================== INDICES STUFF ==============================
        

};

#endif // TABLE_HPP