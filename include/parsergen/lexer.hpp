#include <string>
#include <string_view>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <regex>
#include <memory>
#include <stdexcept>
#include "fmt/core.h"
#include "parsergen/utils.hpp"

#pragma once

#define REGEX_SYNTAX_TYPE std::regex_constants::ECMAScript

namespace Parsergen {

class Position {
public:
    int lineno { 0 };
    int column { 0 };
    Position(){}
    Position(int lineno, int column) : lineno(lineno), column(column) {}

    bool operator ==(Position &other) const {
        return lineno == other.lineno && column == other.column;
    }
};

class Token {
public:
    std::string type;
    std::string value;
    Position position;

    Token(std::string type, std::string value, Position position);
    std::string error_format();

    bool operator ==(Token &other) const;
};

class Lexer;

typedef std::function<void (Token &tok, utils::svmatch &sm)> TokenModifierFunc;

class LexRule {
public:
    std::string name;
    std::vector<std::regex> patterns;
    TokenModifierFunc modifier;

    LexRule(
        std::string name,
        std::vector<std::regex> patterns
    );

    LexRule(
        std::string name,
        std::vector<std::regex> patterns,
        TokenModifierFunc modifier
    );
};

struct NoToken : std::exception {
    const char *what() const throw() {
        return "No Token";
    }
};


struct LexError : std::runtime_error {
    std::string m_msg, m_lineText;
    int m_lineno, m_column;
    LexError(
        const std::string &msg,
        const int lineno,
        const int column,
        const std::string &lineText
    );

    const char *what() const noexcept;
private:
    std::string message;
    void make_message();
};

class Lexer {
public:
    std::string_view currentLine;
    std::vector<Token> tokens;
    std::vector<std::string> lines;
    std::vector<LexRule> rules;

    Lexer();

    void setText(std::string text);
    std::string getText();
    void Lex();
protected:
    std::string text;
    std::string_view source;
    constexpr bool source_left() { return source.length() > 0; }
    int column = 0;
    int lineno = 1;
    void newline();
    Token GetNextToken();
    void StepSource(int amount);
};

class TokenStream {
public:
    std::shared_ptr<Lexer> lexer;
    TokenStream(std::shared_ptr<Lexer> lexer) : lexer(lexer) {}
    int mark();
    void set_pos(int pos);
    Token &get_token();
    Token &peek_token();
    Token &peek_token(int pos);

private:
    int m_pos = 0;
    Token eof_tok { Token("EOF", "", Position(0, 0)) };
};

}