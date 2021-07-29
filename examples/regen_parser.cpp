#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include <iostream>
#include <fmt/format.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace Parsergen;

int main(){
    GrammarLexer l;
    std::ifstream file;
    file.open("src/parsergen/metagrammar.gram");
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); //read the file
        std::string pgram = strStream.str(); //str holds the content of the file
        
        l.setText(pgram);
        l.Lex();
        std::cout << "Start string: " << l.getText() << std::endl;
        for (auto const &tok : l.tokens){
            std::cout << "type : '" << tok.type << ", value: '" << tok.value << "'\n";
            //fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        }
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }
    
    return 0;
}