#include "parsergen/parser_generator.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace Parsergen;

int main(){
    std::ifstream file;
    file.open("examples/calc/calc.gram");
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); //read the file
        std::string pgram = strStream.str(); //str holds the content of the file
        
        auto result = generate_parser(pgram);
        std::ofstream f("examples/calc/calc.hpp");
        f << result;
        f.close();
        std::cout << "Generation complete.\n";
    }
    else {
        std::cout << "Unable to open file" << std::endl;
        return 1;
    }
    return 0;
}