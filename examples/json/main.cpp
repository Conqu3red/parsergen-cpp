#include "examples/json/json_lexer.hpp"
#include "examples/json/json_parser.hpp"
#include "fmt/core.h"

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>

namespace JSON {
    std::string escape_special(std::string string){
        std::string result;
        for (char c : string){
            for (auto &[k, v] : JsonLexer::escape_loopup_table){
                if (v == c){
                    result += "\\";
                    c = k;
                }
            }
            result += c;
        }
        return result;
    }

    std::string format(Value &node){
        std::string buf = "";
        switch (node.GetType()){
            case Object:
                {
                    auto obj = node.as_object();
                    buf += "{";
                    for (auto &member : obj->members){
                        buf += fmt::format("\"{}\": {}, ", escape_special(member.first), format(member.second));
                    }
                    if (obj->members.size() > 0){
                        buf.pop_back();
                        buf.pop_back();
                    }
                    buf += "}";
                }
                break;
            case Array:
                {
                    buf += "[";
                    auto arr = node.as_array();
                    for (int i = 0; i < arr->size(); i++){
                        auto v = arr->operator[](i);
                        buf += format(v) + ", ";
                    }
                    if (arr->size() > 0){
                        buf.pop_back();
                        buf.pop_back();
                    }
                    buf += "]";
                }
                break;
            case String:
                buf = fmt::format("\"{}\"", escape_special(node.as_string()));
                break;
            case Number:
                buf = fmt::format("{}", node.as_number());
                break;
            case Bool:
                buf = node.as_boolean() ? "true" : "false";
                break;
            case Null:
                buf = "null";
                break;
            default: buf = "<INVALID>";
        }
        return buf;
    }

    Value parse(std::string data){
        std::unique_ptr<Lexer> l = std::make_unique<JsonLexer>();
        l->setText(data);
        l->Lex();
        JsonParser p(std::make_unique<TokenStream>(std::move(l)));
        auto result = p.json();
        if (result.has_value()){
            return result.value();
        }
        throw p.error().value();
    }
}

int main(){
    std::unique_ptr<Lexer> l = std::make_unique<JsonLexer>();
    std::ifstream file;
    auto start = std::chrono::steady_clock::now();
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    file.open("examples/json/data.json");
    if (!file.is_open()){
        std::cout << "unable to open json file";
        return EXIT_FAILURE;
    }
    std::stringstream strStream;
    strStream << file.rdbuf(); // read the file
    std::string input = strStream.str(); // str holds the content of the file
    
    std::cout << "Begin Tokenizing...\n";
    start = std::chrono::steady_clock::now();
    
    l->setText(input);
    try {
        l->Lex();
    }
    catch (LexError &e){
        std::cout << e.what() << "\n";
        return EXIT_FAILURE;
    }
    
    end = std::chrono::steady_clock::now();
    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    fmt::print("Tokenized JSON in {:d} ms ({:d} tokens)\n", diff.count(), l->tokens.size());
    
    //std::cout << "Start string: " << l.getText() << std::endl;
    //for (auto const& tok : l.tokens){
    //    fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
    //}
    JsonParser p(std::make_unique<TokenStream>(std::move(l)));
    
    std::cout << "Begin Parsing...\n";
    start = std::chrono::steady_clock::now();
    
    auto top = p.json();
    
    end = std::chrono::steady_clock::now();
    diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Parsed JSON in " << diff.count() << "ms" << std::endl;
   
    if (top.has_value()){
        std::cout << "parsed" << "\n";
        //std::cout << JSON::format(top.value());
    }
    else {
        std::cout << p.error().value().what() << "\n";
    }


    // sample.json
    std::ifstream f2;
    f2.open("examples/json/sample.json");
    if (!f2.is_open()){
        std::cout << "unable to open json file";
        return EXIT_FAILURE;
    }
    std::stringstream ss2;
    ss2 << f2.rdbuf(); // read the file
    std::string data = ss2.str(); // str holds the content of the file

    auto value = JSON::parse(data);

    std::cout << "formatted: " << JSON::format(value) << "\n";
    std::cout << "key1: " << value.as_object()->get("key1").as_string() << "\n";
    std::cout << "key2[0]: " << value.as_object()->get("key2").as_array()->operator[](0).as_number() << "\n";

    return 0;
}