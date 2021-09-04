#pragma once
#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include <functional>
#include <optional>
#include <vector>
#include <string>
#include <memory>


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
    Parser(std::unique_ptr<TokenStream> token_stream) : token_stream(std::move(token_stream)) {}
    void fail();
    std::optional<ParseError> error();
    int mark();
    void set_pos(int pos);
    std::optional<Token> expect(std::string type);
    std::optional<Token> expect_contant(std::string value);
    Token &peek_token();
    Token &peek_token(int pos);
    Position current_pos();
    void clear_memoization_caches();
protected:
    std::unique_ptr<TokenStream> token_stream;
    int error_pos = -1;
};

static const std::string parsergen_version = "1.0.0";

inline std::string getTokenValueOr(std::optional<Token> &tok, std::string other){
    return tok.has_value() ? tok->value : other;
}

}