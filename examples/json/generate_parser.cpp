#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
#include "parsergen/grammar_parser.hpp"
#include "parsergen/parser_generator.hpp"

#include <iostream>
#include <fmt/format.h>
#include <fstream>
#include <sstream>

using namespace Parsergen;
using namespace AST;

int main(){
    std::ifstream file;
    file.open("examples/json/json.gram");
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); //read the file
        std::string pgram = strStream.str(); //str holds the content of the file
        
        auto result = generate_parser(pgram);
        std::ofstream f("examples/json/json_parser.hpp");
        f << result;
        f.close();
    }
    else {
        std::cout << "Unable to open file" << std::endl;
        return 1;
    }
}