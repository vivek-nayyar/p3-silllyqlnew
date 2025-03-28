// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef SILLY_HPP
#define SILLY_HPP

#include <deque>
#include <unordered_map>
#include <iostream>
#include <vector>
#include "table.hpp"
#include "Field.h"
#include "cli.hpp"


using namespace std;

class Silly{
    public:
        unordered_map<string, Table*> tables;
        // constructor
        Silly(const Options &options); 

        
        void create(); // create a table - check if there's already a table with that name

        void quit(); // clear memory, exit return 0, print "Thanks for being silly!\n"

        void comment(); // get rid of the line

        void remove(); // remove a table - make sure there's a table with that name

        void insert(); // inserts N new rows at the bottom of given table

        void print(); // inserts N new rows at the bottom of given table

        void delete_(); // deletes all rows that satisfy condition

        void join(); // prints data which two tables share? 

        void generate();

        void read_command(); // read command - switch case based on first word.

        void run(); // read input while options.quit == false


        

        void print_table_not_in(const string& command, const string& table_name);
        ColumnType string_to_col_type(const string& str);

    private:
        Options options;



};

#endif // SILLY_HPP