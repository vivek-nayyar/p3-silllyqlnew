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

        void remove(); // remove a table - make sure there's a table with that name

        void run(); // read input while options.quit == false

        void read_command(); // read command - switch case based on first word.

        void quit(); // clear memory, exit return 0, print "Thanks for being silly!\n"
        


        ColumnType string_to_col_type(const string& str);

    private:
        Options options;



};

#endif // SILLY_HPP