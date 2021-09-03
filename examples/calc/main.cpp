#include "examples/calc/calc_lexer.hpp"
#include "examples/calc/calc.hpp"

#include <iostream>
#include <string>

int main(){
    while (true){
        std::unique_ptr<Lexer> l = std::make_unique<CalcLexer>();
        std::string input = "";
        std::cout << "> ";
        std::getline(std::cin, input);
        if (input == "exit") break;
        
        l->setText(input);
        try {
            l->Lex();
        }
        catch (LexError &e){
            std::cout << e.what() << "\n";
            continue;
        }
        //std::cout << "Start string: " << l.getText() << std::endl;
        //for (auto const& tok : l.tokens){
        //    fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        //}
        CalcParser p(std::make_unique<TokenStream>(std::move(l)));
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