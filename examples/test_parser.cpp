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
    GrammarLexer l;
    std::ifstream file;
    file.open("examples/test/test.gram");
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); //read the file
        std::string pgram = strStream.str(); //str holds the content of the file
        
        l.setText(pgram);
        l.Lex();
        //std::cout << "Start string: " << l.getText() << std::endl;
        for (auto const &tok : l.tokens){
            std::cout << "type : '" << tok.type << "', value: '" << tok.value << "'\n";
            //fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        }
    }
    else {
        std::cout << "Unable to open file" << std::endl;
        return 1;
    }
    GrammarParser parser(std::make_shared<TokenStream>(std::make_shared<GrammarLexer>(l)));
    auto p = parser.parser_definition();
    if (p.has_value()){
        Generator g(p.value());
        g.generate();
        std::ofstream f("examples/test/test.hpp");
        f << g.result.str();
        f.close();
    }
    else {
        throw parser.error().value();
    }
}