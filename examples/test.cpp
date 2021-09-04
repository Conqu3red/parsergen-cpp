#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser.hpp"
#include "parsergen/parser_generator.hpp"

#include <list>
#include <string>
#include <iostream>
#include <fmt/format.h>

using namespace Parsergen;
using namespace AST;

class TestLexer : public Lexer {
public:
    TestLexer() : Lexer() {
        rules = {
            S_RULE("TEXT", "text"),
            S_RULE("ID", "[a-z]+"),
            F_RULE("NEWLINE", "\n",
                [this] (Token &tok, utils::svmatch &sm) {
                    newline();
                }
            ),
            F_RULE("COMMENT", "#.*",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
            F_RULE("SPACE", " ",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            ),
        };
    }
};

class TextNode : public Expr {
public:
    std::string class_name() override { return "TextNode - '" + text + "'"; }
    std::string text;
    TextNode(std::string text, Position start, Position end)
        : Expr(start, end), text(text)
    {}
};

class Top : public Expr {
public:
    std::string class_name() override { return "Top"; }
    std::list<std::shared_ptr<TextNode>> children;
    Top(std::list<std::shared_ptr<TextNode>> children, Position start, Position end)
        : Expr(start, end), children(children)
    {}
};


class TestParser : public Parser {
public:
    using Parser::Parser;

    /*
        start[Top] : node* { p1, POSITION };
        node[TextNode] : TEXT ID { p2.value, POSITION };


    PLAN: everything can return optionals?, that way we know if it succeeded/failed
    things like TOK? should return optional<optional<something>> maybe
    */
    
    std::optional<std::shared_ptr<Top>> start(){
        Position start, end;
        int pos = mark();
        for (;;){
            {            
                start = current_pos();
                auto _p1 = _loop_0<TextNode>();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match
                return std::make_shared<Top>(p1, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }

    template <typename T>
    std::optional<std::list< std::shared_ptr<T> >> _loop_0(){
        Position start, end;
        std::list< std::shared_ptr<T> > children;
        while (true){
            int pos = mark();
            
            auto _p1 = node();
            if (!_p1.has_value()){
                set_pos(pos);
                break;
            }
            auto &p1 = _p1.value();
    
            children.push_back(p1);
        }

        return children;
    }

    std::optional<std::shared_ptr<TextNode>> node(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("TEXT");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();

                auto _p2 = expect("ID");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();

                // match
                return std::make_shared<TextNode>(p2.value, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }

};

std::shared_ptr<Expr> something(){
    return std::make_shared<TokenPointer>("abc", Position(), Position());
}

int main(){
    TestLexer l;
    std::string input = "text abc text def";
    l.setText(input);
    l.Lex();
    std::cout << "Start string: " << l.getText() << std::endl;
    for (auto const& tok : l.tokens){
        fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
    }

    auto stream = std::make_shared<TokenStream>(TokenStream(std::make_shared<TestLexer>(l)));

    TestParser p(stream);

    if (auto top = p.start()){
        std::cout << top.value()->class_name() << "\n";
        for (auto &child : top.value()->children){
            std::cout << child->class_name() << "\n";
        }
    }

    auto x = std::make_optional<std::optional<int>>(std::nullopt);
    x.value() = std::nullopt;
    // x definitely has a value
    std::cout << "x has value? " << x.has_value() << "\n";
    auto p0 = x.value();
    std::cout << "p0 has value? " << p0.has_value() << "\n";

    std::shared_ptr<Token> t(new Token("x", "y", Position()));
    std::shared_ptr<Token> t2(t);
    //auto x1 = std::shared_ptr(new Token("x", "y", Position())); // invalid

    std::shared_ptr<TextNode> tn(new TextNode("abc", Position(), Position()));
    std::shared_ptr<Expr> e(tn);
    std::cout << e->class_name() << "\n";

    std::vector<std::shared_ptr<Expr>> exprs;
    std::cout << something()->class_name();
    //exprs.push_back(std::make_shared<Expr>(std::make_shared<TokenPointer>("abc", Position(), Position())));
    for (auto &p : exprs){
        std::cout << p->class_name();
    }

    return 0;
}