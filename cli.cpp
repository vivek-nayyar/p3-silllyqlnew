// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include "cli.hpp"
#include <getopt.h>
#include <iostream>
using namespace std;

//help message
void print_help(){
    cout << "Usage: silly [options]\n"
         << "Options:\n"
         << "  --help               Show this help message\n"
         << "  --quiet              Causes the program to run in quiet mode\n";
}

// option flag parsing
void parse_options(int argc, char **argv, Options &options){
    struct option longOpts[] = {{"help", no_argument, nullptr, 'h'},
                                {"quiet", no_argument, nullptr, 'q'},
                                {nullptr, 0, nullptr, '\0'}};

    int option_index = 0, option = 0;

    while ((option = getopt_long(argc, argv, "hq", longOpts, &option_index)) != -1){

        switch (option)
        {
            case 'h':
                print_help();
                exit(0);
                break;
            case 'q':
                options.quiet = true;
                break;
            default:
                cerr << "Error: Unknown command line option\n";
                exit(1);
        }
    }
}