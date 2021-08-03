#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
//#include "parsergen/grammar_parser.hpp"
#include "parsergen/parser_generator.hpp"

#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace Parsergen;
using namespace AST;

std::string header = R"(
#include "parsergen/grammar_ast.hpp"
using namespace Parsergen::AST;

class TextNode : public Expr {
public:
    std::string class_name() override { return "TextNode - '" + text + "'"; }
    std::string text;
    TextNode(std::string text, Position start, Position end)
        : Expr(start, end), text(text)
    {}
};

class Top : public Expr {
public:
    std::string class_name() override { return "Top"; }
    std::list<std::shared_ptr<TextNode>> children;
    Top(std::list<std::shared_ptr<TextNode>> children, Position start, Position end)
        : Expr(start, end), children(children)
    {}
};
)";

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
    }

    // Parser Generator Test
    #define p Position(), Position()
    std::cout << "alive\n";
    Generator g(
        std::make_shared<ParserDefinition>(
            std::vector<std::shared_ptr<Section>>{
                std::make_shared<ConfigurationCall>(
                    "header", header, p
                ),
                std::make_shared<Statement>(
                    "start", "Top", std::vector<std::shared_ptr<Expr>>{
                        std::make_shared<ExprList>(
                            std::vector<std::shared_ptr<Expr>>{
                                std::make_shared<ZeroOrMore>(
                                std::make_shared<StatementPointer>("node", p),
                                p)
                            },
                        p)
                    }, "p1, POSITION", p
                ),
                std::make_shared<Statement>(
                    "node", "TextNode", std::vector<std::shared_ptr<Expr>>{
                        std::make_shared<TokenPointer>("TEXT", p),
                        std::make_shared<TokenPointer>("ID", p)
                    }, "p2.value, POSITION", p
                )
            },
            p
        )
    );
    
    auto start = std::chrono::steady_clock::now();
    g.generate();
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Generated Parser in " << diff.count() << "ms" << std::endl;
    
    std::ofstream f("examples/out.cpp");
    f << g.result.str();
    f.close();
    
    return 0;
}