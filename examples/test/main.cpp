#include "examples/test/test_lexer.hpp"
#include "examples/test/test.hpp"

#include <iostream>
#include <string>

int main(){
    while (true){
        TestLexer l;
        std::string input = "";
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        
        l.setText(input);
        try {
            l.Lex();
        }
        catch (LexError &e){
            std::cout << e.what() << "\n";
            continue;
        }
        //std::cout << "Start string: " << l.getText() << std::endl;
        //for (auto const& tok : l.tokens){
        //    fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        //}
        auto stream = std::make_shared<TokenStream>(TokenStream(std::make_shared<TestLexer>(l)));
        TestParser p(stream);
        auto top = p.start();
        if (top.has_value()){
            std::cout << *top << "\n";
        }
        else {
            std::cout << p.error().value().what() << "\n";
        }
    }
    return 0;
}