// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#ifndef CLI_HPP
#define CLI_HPP

// options for the game
struct Options
{
    bool quiet = false;
    bool quit = false;
};


void print_help();
void parse_options(int argc, char **argv, Options &options);

#endif // CLI_HPP