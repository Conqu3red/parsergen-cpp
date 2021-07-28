#include <string>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <regex>
#include <memory>
#include <stdexcept>
#include "fmt/core.h"

#pragma once

namespace Parsergen {

class Token {
public:
    std::string type;
    std::string value;
    int lineno;
    int column;

    Token(std::string type, std::string value, int lineno=0, int column=0);

    bool operator ==(Token &other) const;
};

class Lexer;

class LexRule {
public:
    std::string name;
    std::vector<std::regex> patterns;
    std::function<void (Lexer *lexer, Token *tok)> modifier;

    LexRule(
        std::string name,
        std::vector<std::regex> patterns
    );

    LexRule(
        std::string name,
        std::vector<std::regex> patterns,
        std::function<void (Lexer *lexer, Token *tok)> modifier
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

    const char *what() const throw();
private:
    std::string message;
    void make_message();
};

class Lexer {
public:
    std::string currentLine;
    std::vector<Token> tokens;
    std::vector<std::string> lines;
    std::vector<LexRule> rules;

    Lexer();

    void setText(std::string text);
    std::string getText();
    void Lex();
protected:
    std::string text;
    std::string source;
    int column = 0;
    int lineno = 0;
    void newline();
    Token GetNextToken();
    void StepSource(int amount);
};

};