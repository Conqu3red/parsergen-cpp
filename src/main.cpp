// TODO: parsergen.exe
// Usage: parsergen somefile.gram -o somefile.hpp

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
//#include "fmt/core.h"
#include "args/args.hpp"
#include <fstream>
#include <sstream>

#include "parsergen/parser_generator.hpp"
using namespace Parsergen;

static std::string help_string = R"(
Usage: parsergen file [options]

Options:
    -o <arg>   target location for generated code
    --version  display current version
    --help     display help message
)";

int main(int argc, char **argv){
    args::ArgumentParser parser("Generates C++ files from grammar definitions.");
    args::HelpFlag help(parser, "help", "Display this help menu", {'h', "help"});
    args::Flag version(parser, "version", "Show the version of this program", {"version"});
    args::Positional<std::string> input(parser, "file", "Input file");
    args::ValueFlag<std::string> output(parser, "output", "target location for generated code", {'o'});
    try
    {
        parser.ParseCLI(argc, argv);
    }
    catch (args::Help)
    {
        std::cout << parser;
        return 0;
    }
    catch (args::ParseError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }
    catch (args::ValidationError e)
    {
        std::cerr << e.what() << std::endl;
        std::cerr << parser;
        return 1;
    }

    if (version){
        std::cout << "version: " << "1.0.0" << "\n";
        return 0;
    }

    std::string input_loc, output_loc;
    if (input) input_loc = args::get(input);
    else {
        std::cerr << parser;
        return 1;
    }
    if (output) output_loc = args::get(output);

    std::ifstream file;
    file.open(input_loc);
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); // read the file
        std::string pgram = strStream.str(); // str holds the content of the file
        
        auto result = generate_parser(pgram);
        std::cout << "Generation complete.\n";
        std::ofstream f(output_loc);
        if (!f.is_open()){
            std::cout << "Unable to open output file!\n";
            return 1;
        }
        f << result;
        f.close();
    }
    else {
        std::cout << "Unable to open file '" << input_loc << "'"  << "\n";
        return 1;
    }

    return 0;
}