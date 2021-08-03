#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include <vector>
#include <string>
#include <memory>

#pragma once

#define POSITION start, end

namespace Parsergen {

struct ParseError : std::runtime_error {
    std::string m_msg, m_lineText;
    Position m_start, m_end;
    ParseError(
        const std::string &msg,
        const Position start,
        const Position end,
        const std::string &lineText
    );

    const char *what() const throw();
private:
    std::string message;
    void make_message();
};

class Parser {
public:
    Parser(std::shared_ptr<TokenStream> token_stream) : token_stream(token_stream) {}
    void fail();
    std::optional<ParseError> error();
    int mark();
    void set_pos(int pos);
    std::optional<Token> expect(std::string type);
    std::optional<Token> expect_contant(std::string value);
    Token &peek_token();
    Token &peek_token(int pos);
    Position current_pos();
protected:
    std::shared_ptr<TokenStream> token_stream;
    int error_pos = -1;
};

}