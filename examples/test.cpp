#include "parsergen/lexer.hpp"
#include <iostream>
#include <fmt/format.h>

using namespace Parsergen;

class TestLexer : public Lexer {
public:
    TestLexer() : Lexer() {
        rules = {
            LexRule(
                "id",
                std::vector<std::regex>{
                    std::regex("[a-z]+")
                }
            ),
            LexRule(
                "NEWLINE",
                std::vector<std::regex>{
                    std::regex("\n")
                },
                [this] (Lexer *lexer, Token *tok) {
                    newline();
                }
            ),
            LexRule(
                "comment",
                std::vector<std::regex>{
                    std::regex("#.*")
                },
                [this] (Lexer *lexer, Token *tok) {
                    throw NoToken();
                }
            ),
            LexRule(
                "SPACE",
                std::vector<std::regex>{
                    std::regex(" ")
                },
                [this] (Lexer *lexer, Token *tok) {
                    throw NoToken();
                }
            )
        };
    }
};

int main(){
    TestLexer l;
    l.setText("abc # comment \n something");
    l.Lex();
    std::cout << "Start string: " << l.getText() << std::endl;
    for (auto const& tok : l.tokens){
        fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
    }
    return 0;
}