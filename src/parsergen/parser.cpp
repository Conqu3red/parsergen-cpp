#include "fmt/core.h"
#include <algorithm>
#include "parsergen/parser.hpp"
#include <iostream>

namespace Parsergen {

ParseError::ParseError(const std::string &msg, const Position start, const Position end, const std::string &lineText)
    : m_msg(msg)
    , m_start(start)
    , m_end(end)
    , m_lineText(lineText)
    , std::runtime_error(msg)
{
    make_message();
}

void ParseError::make_message(){
    std::string message = fmt::format("\n  Line {:d}:\n", m_start.lineno);
    if (m_lineText.length() > 0)
        message += fmt::format(
            "  {1}\n  {0: ^{2}}^{0:~^{3}}\n", 
            "", m_lineText, m_start.column, std::max(m_end.column - m_start.column - 1, 0)
        );
    message += m_msg;
    //printf("message: %s", message.c_str());
    this->message = message;
}

const char *ParseError::what() const noexcept {
    return message.c_str();
}

void Parser::fail(){
    int pos = mark();
    if (pos > error_pos)
        error_pos = pos;
}

std::optional<ParseError> Parser::error(){
    if (error_pos == -1)
        return std::nullopt;
    auto tok = peek_token(error_pos);
    auto num_lines = token_stream->get_lines().size();
    //auto end = tok.end;
    return std::optional<ParseError>(
        ParseError(
            fmt::format("Unexpected token {}", tok.error_format()), 
            tok.start, tok.end,
            tok.start.lineno - 1 < num_lines && num_lines > 0 ? token_stream->get_lines()[tok.start.lineno - 1] : ""
        )
    );
}

int Parser::mark(){
    return token_stream->mark();
}

void Parser::set_pos(int pos){
    token_stream->set_pos(pos);
}

std::optional<Token> Parser::expect(std::string type){
    auto &tok = peek_token();
    if (tok.type == type){
        token_stream->set_pos(token_stream->mark() + 1);
        return std::optional<Token>(tok);
    }
    return std::nullopt;
}

std::optional<Token> Parser::expect_contant(std::string value){
    auto &tok = peek_token();
    if (tok.value == value){
        token_stream->set_pos(token_stream->mark() + 1);
        return std::optional<Token>(tok);
    }
    return std::nullopt;
}

Token &Parser::peek_token(){
    return token_stream->peek_token();
}

Token &Parser::peek_token(int pos){
    return token_stream->peek_token(pos);
}

Position Parser::current_pos(){
    return peek_token().start;
}

}