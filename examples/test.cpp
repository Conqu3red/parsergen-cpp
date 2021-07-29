#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include <iostream>
#include <fmt/format.h>

using namespace Parsergen;

class TestLexer : public Lexer {
public:
    TestLexer() : Lexer() {
        rules = {
            S_RULE("id", "[a-z]+"),
            F_RULE("NEWLINE", "\n",
                [this] (Token &tok, utils::svmatch &sm) {
                    newline();
                }
            ),
            F_RULE("COMMENT", "#.*",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
            F_RULE("SPACE", " ",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
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