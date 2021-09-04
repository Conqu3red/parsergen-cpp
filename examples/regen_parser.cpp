#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser.hpp"
#include "parsergen/grammar_parser.hpp"
#include "parsergen/parser_formatter.hpp"
#include "parsergen/parser_generator.hpp"

#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace Parsergen;
using namespace AST;

std::string parser_header = R"(
#include "parsergen/grammar_ast.hpp"
using namespace Parsergen::AST;

namespace Parsergen {
)";

std::string parser_footer = "\n}";

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
        //std::cout << "Start string: " << l.getText() << std::endl;
        for (auto const &tok : l.tokens){
            std::cout << "type : '" << tok.type << ", value: '" << tok.value << "'\n";
            //fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        }
    }
    else {
        std::cout << "Unable to open file" << std::endl;
        return 0;
    }

    GrammarParser parser(std::make_shared<TokenStream>(std::make_shared<GrammarLexer>(l)));
    auto p = parser.parser_definition();
    if (auto parser_def = p.value()){
        std::cout << parser_format(parser_def);
        Generator g(parser_def);
        
        auto start = std::chrono::steady_clock::now();
        g.generate();
        auto end = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        std::cout << "Generated Parser in " << diff.count() << "ms" << std::endl;

        std::ofstream f("include/parsergen/grammar_parser.hpp");
        f << g.result.str();
        f.close();
    }
    else {
        throw parser.error().value();
    }
    
    return 0;
}