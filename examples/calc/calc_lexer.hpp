#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser.hpp"

#include <list>
#include <optional>
#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <utility>
#pragma once

using namespace Parsergen;

#include <list>
#include <string>
#include <iostream>
#include <fmt/format.h>
#include "parsergen/grammar_ast.hpp"
using namespace AST;

class CalcLexer : public Lexer {
public:
    CalcLexer() : Lexer() {
        rules = {
            token_match("INT",  "[0-9]+"),
            token_match_fast("ADD",  "+"),
            token_match_fast("SUB",  "-"),
            token_match_fast("POW",  "**"),
            token_match_fast("MUL",  "*"),
            token_match_fast("DIV",  "/"),
            token_match_fast("LPAREN",  "("),
            token_match_fast("RPAREN",  ")"),
            token_match("COMMENT", "#.*",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
            token_match("SPACE", "[ \t]",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
        };
    }
};