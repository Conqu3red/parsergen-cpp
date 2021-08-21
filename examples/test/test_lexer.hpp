#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"

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

class TestLexer : public Lexer {
public:
    TestLexer() : Lexer() {
        rules = {
            S_RULE("INT",  "[0-9]+"),
            S_RULE("ADD",  "\\+"),
            S_RULE("SUB",  "\\-"),
            S_RULE("POW",  "\\*\\*"),
            S_RULE("MUL",  "\\*"),
            S_RULE("DIV",  "\\/"),
            S_RULE("LPAREN",  "\\("),
            S_RULE("RPAREN",  "\\)"),
            F_RULE("COMMENT", "#.*",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
            F_RULE("SPACE", "[ \t]",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
        };
    }
};

class TextNode : public Expr {
public:
    std::string class_name() override { return "TextNode - \'" + text + "\'"; }
    std::string text;
    TextNode(std::string text, Position start, Position end)
        : Expr(start, end), text(text)
    {}
};

class Top : public Expr {
public:
    std::string class_name() override { return "Top"; }
    std::vector<std::shared_ptr<TextNode>> children;
    Top(std::vector<std::shared_ptr<TextNode>> children, Position start, Position end)
        : Expr(start, end), children(children)
    {}
};