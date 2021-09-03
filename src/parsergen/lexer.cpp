#include "parsergen/lexer.hpp"
#include "iostream"

//#define DEBUG

namespace Parsergen {

Token::Token(std::string type, std::string value, Position start, Position end)
    : type(type)
    , value(value)
    , start(start)
    , end(end)    
{}

bool Token::operator ==(Token &other) const {
    return type == other.type
        && value == other.value
        && start == other.start
        && end == other.end;
}

std::string Token::error_format(){
    return fmt::format("'{}' ({})", value, type);
}

LexRule::LexRule(
    std::string name,
    std::vector<std::regex> patterns
)
    : name(name)
    , patterns(patterns)
{}

LexRule::LexRule(
    std::string name,
    std::vector<std::regex> patterns,
    TokenModifierFunc modifier
)
    : name(name)
    , patterns(patterns)
    , modifier(modifier)
{}

LexRule::LexRule(
    std::string name,
    std::vector<std::string> patterns
)
    : name(name)
    , string_patterns(patterns)
{}

LexRule::LexRule(
    std::string name,
    std::vector<std::string> patterns,
    TokenFastModifierFunc fast_modifier
)
    : name(name)
    , string_patterns(patterns)
    , fast_modifier(fast_modifier)
{}


LexRule token_match(std::string name, std::initializer_list<std::string> patterns){
    std::vector<std::regex> regexes;
    for (auto & pattern : patterns){
        regexes.push_back(std::regex(pattern, REGEX_SYNTAX_TYPE));
    }
    return LexRule(name, regexes);
}

LexRule token_match(std::string name, std::initializer_list<std::string> patterns, TokenModifierFunc modifier){
    auto rule = token_match(name, patterns);
    rule.modifier = modifier;
    return rule;
}

LexRule token_match(std::string name, std::string pattern){
    return token_match(name, {pattern});
}

LexRule token_match(std::string name, std::string pattern, TokenModifierFunc modifier){
    return token_match(name, {pattern}, modifier);
}

LexRule token_match_fast(std::string name, std::initializer_list<std::string> patterns){
    return LexRule(name, patterns);
}

LexRule token_match_fast(std::string name, std::initializer_list<std::string> patterns, TokenFastModifierFunc modifier){
    auto rule = token_match_fast(name, patterns);
    rule.fast_modifier = modifier;
    return rule;
}

LexRule token_match_fast(std::string name, std::string pattern){
    return token_match_fast(name, {pattern});
}

LexRule token_match_fast(std::string name, std::string pattern, TokenFastModifierFunc modifier){
    return token_match_fast(name, {pattern}, modifier);
}


LexError::LexError(const std::string &msg, const int lineno, const int column, const std::string &lineText)
    : m_msg(msg),
    m_lineno(lineno),
    m_column(column),
    m_lineText(lineText),
    std::runtime_error(msg)
{
    make_message();
}

void LexError::make_message(){
    std::string message = fmt::format("\n  Line {:d}:\n", m_lineno);
    if (m_lineText.length() > 0)
        message += fmt::format("  {}\n  {}^\n", m_lineText, std::string(m_column, ' '));
    message += m_msg;
    //printf("message: %s", message.c_str());
    this->message = message;
}

const char *LexError::what() const noexcept {
    return message.c_str();
}

Lexer::Lexer(){
    
}

void Lexer::setText(std::string text){
    this->text = utils::normalizeNewlines(text);
    this->source = std::string_view(this->text);
    this->currentLine = std::string_view(this->source.data(), 0);
}

std::string Lexer::getText(){
    return text;
}

void Lexer::newline(){
    lines.push_back(std::string(currentLine));
    currentLine = std::string_view(source.data(), 0);
    lineno += 1;
    column = 0;
}

void Lexer::StepSource(int amount){
    column += amount;
    // throws std::out_of_range if string is too short
    currentLine = std::string_view(currentLine.data(), currentLine.length() + amount);
    //currentLine += source.substr(0, amount);
    
    source = source.substr(amount);
    //source = std::string_view(source.data() + amount, source.length() - amount);
}

Token Lexer::GetNextToken(){
    utils::svmatch sm;
    for (auto const &rule : rules){
        for (auto const &string_pattern : rule.string_patterns){
            if (source.rfind(string_pattern, 0) == 0){ // matches start of string
                auto length = string_pattern.length();
                
                Token tok = Token(rule.name, string_pattern, Position(lineno, column), Position(lineno, column + length));
                StepSource(length);
                //std::cout << rule.name << ": " << sm.str() << std::endl;
                if (rule.fast_modifier)
                    rule.fast_modifier(tok);
                #ifdef DEBUG
                std::cout << rule.name << ": " << sm.str() << std::endl;
                #endif
                return tok;
            }
        }
        for (auto const &pattern : rule.patterns){
            // https://en.cppreference.com/w/cpp/regex/regex_search
            // The overload (3) is prohibited from accepting temporary strings,
            // otherwise this function populates match_results m with string 
            // iterators that become invalid immediately.
            // match_continous is *critical* to only match at start of string
            //      this makes speed bearable
            bool matches = utils::regex_search(source, sm, pattern, std::regex_constants::match_continuous);
            if (matches){
                // match found
                auto length = sm.length();
                Token tok = Token(rule.name, sm.str(), Position(lineno, column), Position(lineno, column + length));
                StepSource(length);
                //std::cout << rule.name << ": " << sm.str() << std::endl;
                if (rule.modifier)
                    rule.modifier(tok, sm);
                #ifdef DEBUG
                std::cout << rule.name << ": " << sm.str() << std::endl;
                #endif
                return tok;
            }
        }
    }
    std::cout << (int)(source[0]) << "\n";
    throw LexError(
        fmt::format("Found Unexpected character '{}' while tokenizing!", source[0]),
        lineno,
        column,
        std::string(currentLine) + source[0]
    );
}

void Lexer::Lex(){
    while (source.length() > 0){
        #ifdef DEBUG
        printf("source length: %d\n", source.length());
        #endif
        
        try {   
            Token token = GetNextToken();
            tokens.push_back(token);
        }
        catch (NoToken &e){
            // There is no token, ignore silently
        }
    }
    lines.push_back(std::string(currentLine)); // push final line
}

Position Lexer::cur_pos(){
    return Position(lineno, column);
}

int TokenStream::mark(){
    return m_pos;
}

void TokenStream::set_pos(int pos){
    m_pos = pos;
}

Token &TokenStream::get_token(){
    Token &tok = peek_token();
    m_pos += 1;
    return tok;
}

Token &TokenStream::peek_token(){
    if (m_pos >= lexer->tokens.size()){
        Position eof_pos(0,0);
        if (lexer->tokens.size() > 0){
            eof_pos = lexer->tokens[lexer->tokens.size() - 1].start;
            eof_pos.column += 1;
        }
        eof_tok = Token("EOF", "<EOF>", eof_pos, eof_pos);
        return eof_tok;
    }
    return lexer->tokens[m_pos];
}

Token &TokenStream::peek_token(int pos){
    int old = m_pos;
    m_pos = pos;
    Token &tok = peek_token();
    m_pos = old;
    return tok;
}

std::vector<std::string> &TokenStream::get_lines(){
    return lexer->lines;
}


}