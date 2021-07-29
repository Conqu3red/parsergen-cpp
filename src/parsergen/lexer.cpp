#include "parsergen/lexer.hpp"
#include "iostream"

//#define DEBUG

namespace Parsergen {

Token::Token(std::string type, std::string value, Position position) {
    this->type = type;
    this->value = value;
    this->position = position;
}

bool Token::operator ==(Token &other) const {
    return type == other.type
        && value == other.value
        && position == other.position;
}

LexRule::LexRule(
    std::string name,
    std::vector<std::regex> patterns
){
    this->name = name;
    this->patterns = patterns;
}

LexRule::LexRule(
    std::string name,
    std::vector<std::regex> patterns,
    TokenModifierFunc modifier
){
    this->name = name;
    this->patterns = patterns;
    this->modifier = modifier;
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
    this->text = text;
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
}

Token Lexer::GetNextToken(){
    for (auto const &rule : rules){
        for (auto const &pattern : rule.patterns){
            utils::svmatch sm;
            // https://en.cppreference.com/w/cpp/regex/regex_search
            // The overload (3) is prohibited from accepting temporary strings,
            // otherwise this function populates match_results m with string 
            // iterators that become invalid immediately.
            bool matches = utils::regex_search(source.data(), sm, pattern);
            if (matches){
                if (sm.position() == 0){
                    // match found
                    Token tok = Token(rule.name, sm.str(), Position(lineno, column));
                    StepSource(sm.length());
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
    }
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


};