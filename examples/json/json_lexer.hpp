#pragma once
#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
#include "parsergen/grammar_ast.hpp"
#include "examples/json/utils.hpp"
#include "examples/json/json_nodes.hpp"

#include <list>
#include <optional>
#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <utility>
#include <variant>
#include <map>

using namespace Parsergen;

#include <list>
#include <string>
#include <iostream>
#include <fmt/format.h>
using namespace AST;

char hex_char_to_num(char c){
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

void ignore(Token &tok){
    throw NoToken();
}

class JsonLexer : public Lexer {
public:
    static std::map<char, char> escape_loopup_table;
    
    JsonLexer() : Lexer() {
        
        auto process_STRING = [this](Token &tok){
            char end = '\"';
            bool escape = false;
            std::string result = "";

            while (true){
                if (source_left()){
                    char cur = source[0];
                    StepSource(1);
                    if (!escape){
                        if (cur == '\\'){
                            if (!source_left()) continue;
                            cur = source[0]; // move to char that is escaped
                            StepSource(1);
                            if (escape_loopup_table.count(cur)){
                                // normal escape characters
                                cur = escape_loopup_table[cur];
                            }
                            
                            if (cur == 'u'){
                                // unicode character
                                int codepoint;
                                char parts[4] = {0, 0, 0, 0};
                                for (int i = 0; i < 4; i++){
                                    if (!source_left()) continue;
                                    char v = hex_char_to_num(source[0]);
                                    StepSource(1);
                                    if (v == -1)
                                        throw std::runtime_error("Cannot have non hex char in unicode escaped character");
                                    parts[i] = v;
                                }
                                // TODO: neaten generation of unicode characters
                                codepoint = parts[3] + (parts[2] << 4) + (parts[1] << 8) + (parts[0] << 12);
                                std::cout << codepoint;
                                result += utf8chr(codepoint);
                                continue;
                            }
                            escape = true;
                        }
                        else if (cur == end){
                            tok.value = result;
                            tok.end = cur_pos();
                            return;
                        }
                    }
                    if (cur == '\n')
                        newline();
                    result += cur;
                    escape = false;
                }
                else throw std::runtime_error("No End of string");
            }
        };
        
        rules = {
            token_match_fast("STRING", "\"", process_STRING),
            token_match_fast("NULL", "null"),
            token_match_fast("LCURLY", "{"),
            token_match_fast("RCURLY", "}"),
            token_match_fast("LSQUARE", "["),
            token_match_fast("RSQUARE", "]"),
            token_match_fast("COLON", ":"),
            token_match_fast("COMMA", ","),
            token_match("BOOL", "true|false"),
            token_match_fast("SPACE", {" ", "\t"}, ignore),
            token_match_fast("NEWLINE", "\n", 
                [this](Token &tok){
                    newline();
                    throw NoToken();
                }
            ),
            token_match("NUMBER", "-?(?:0|[1-9]\\d*)(?:\\.\\d+)?(?:[eE][+-]?\\d+)?")
        };
    }
};

std::map<char, char> JsonLexer::escape_loopup_table = {
    {'a', '\a'},
    {'b', '\b'},
    {'f', '\f'},
    {'n', '\n'},
    {'r', '\r'},
    {'t', '\t'},
    {'v', '\v'},
    {'\\', '\\'},
    {'\"', '\"'},
    {'\'', '\''},
    {'?', '\?'},
};

std::shared_ptr<JSON::JsonObject> construct_object(std::optional<std::tuple<JSON::Member, std::vector<std::tuple<Parsergen::Token, JSON::Member>>>> members){
    std::vector<JSON::Member> values;
    if (members.has_value()){
        values.push_back(std::get<0>(*members));
        for (auto &member : std::get<1>(*members)){
            values.push_back(std::get<1>(member));
        }
    }
    return std::make_shared<JSON::JsonObject>(values);
}

std::shared_ptr<JSON::JsonArray> construct_array(std::optional<std::tuple<JSON::Value, std::vector<std::tuple<Parsergen::Token, JSON::Value>>>> parameters){
    std::vector<JSON::Value> values;
    if (parameters.has_value()){
        values.push_back(std::get<0>(*parameters));
        for (auto &value : std::get<1>(*parameters)){
            values.push_back(std::get<1>(value));
        }
    }
    return std::make_shared<JSON::JsonArray>(values);
}