
#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser.hpp"

#include <list>
#include <optional>
#include <vector>
#include <string>
#include <tuple>
#include <memory>
#include <utility>
#pragma once

using namespace Parsergen;



#include "parsergen/grammar_ast.hpp"
using namespace Parsergen::AST;

namespace Parsergen {

class GrammarParser : public Parser {
public:
    using Parser::Parser;
    std::optional< std::vector<std::shared_ptr<Section>> > _loop_1(){
        std::vector<std::shared_ptr<Section>> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = section();
                if (!_p0.has_value()){
                    set_pos(pos);
                    break;
                }
                auto &p0 = _p0.value();
                children.push_back(p0);
            }
        }
        return children;
    }
    std::optional<std::shared_ptr<ParserDefinition>> parser_definition(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = _loop_1();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("EOF");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &s = _p1.value();
                return std::make_shared<ParserDefinition>(s, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Section>> section(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = statement_group();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &s = _p1.value();
                return s;
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = configuration();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &s = _p1.value();
                return s;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<ConfigurationCall>> configuration(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("AT");
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
                auto _p3 = expect("EQ");
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                auto _p4 = expect("STRING");
                if (!_p4.has_value()){
                    fail();
                    break;
                }
                auto &p4 = _p4.value();
                end = current_pos();
                // match:
                auto &name = _p2.value();
                auto &value = _p4.value();
                return std::make_shared<ConfigurationCall>(name.value, value.value, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<std::shared_ptr<Statement>> > _loop_12(){
        std::vector<std::shared_ptr<Statement>> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = statement();
                if (!_p0.has_value()){
                    set_pos(pos);
                    break;
                }
                auto &p0 = _p0.value();
                children.push_back(p0);
            }
        }
        return children;
    }
    std::optional<std::shared_ptr<StatementGroup>> statement_group(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("ID");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("RETURN_TYPE");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = _loop_12();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &name = _p1.value();
                auto &return_type = _p2.value();
                auto &stmts = _p3.value();
                return std::make_shared<StatementGroup>(name.value, return_type.value, stmts, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<std::shared_ptr<Expr>> > _loop_15(){
        std::vector<std::shared_ptr<Expr>> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = expr();
                if (!_p0.has_value()){
                    set_pos(pos);
                    break;
                }
                auto &p0 = _p0.value();
                children.push_back(p0);
            }
        }
        return children;
    }
    std::optional< std::optional<Token> > _maybe_17(){
        int pos = mark();
        auto rv = std::make_optional<std::optional<Token>>(std::nullopt);
        rv.value() = std::nullopt;
        auto _p0 = expect("ACTION");
        if (!_p0.has_value()){
            set_pos(pos);
            return rv;
        }
        auto &p0 = _p0.value();
        rv.value() = _p0;
        return rv;
    }
    std::optional<std::shared_ptr<Statement>> statement(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("COLON");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = _loop_15();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = _maybe_17();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                auto _p4 = expect("TERMINATE");
                if (!_p4.has_value()){
                    fail();
                    break;
                }
                auto &p4 = _p4.value();
                end = current_pos();
                // match:
                auto &es = _p2.value();
                auto &a = _p3.value();
                return std::make_shared<Statement>(es, getTokenValueOr(a, "<>"), POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<std::shared_ptr<Expr>> > _loop_20(){
        std::vector<std::shared_ptr<Expr>> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = expr();
                if (!_p0.has_value()){
                    set_pos(pos);
                    break;
                }
                auto &p0 = _p0.value();
                children.push_back(p0);
            }
        }
        return children;
    }
    std::optional<std::shared_ptr<ExprList>> expr_list(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = _loop_20();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &es = _p1.value();
                return std::make_shared<ExprList>(es, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> expr(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("ID");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("EQ");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = or_op();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &name = _p1.value();
                auto &v = _p3.value();
                return std::make_shared<NamedItem>(name.value, v, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = or_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return v;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> or_op(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = star_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                return p1;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> star_op(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = plus_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("STAR");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return std::make_shared<ZeroOrMore>(v, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = plus_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return v;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> plus_op(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = qmark_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("PLUS");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return std::make_shared<OneOrMore>(v, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = qmark_op();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return v;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> qmark_op(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = term();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("QMARK");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return std::make_shared<ZeroOrOne>(v, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = term();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &v = _p1.value();
                return v;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> term(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("AND");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = factor();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &f = _p2.value();
                return std::make_shared<AndPredicate>(f, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("NOT");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = factor();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                end = current_pos();
                // match:
                auto &f = _p2.value();
                return std::make_shared<NotPredicate>(f, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = factor();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &f = _p1.value();
                return f;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> factor(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = item();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &i = _p1.value();
                return i;
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("LPAREN");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expr_list();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = expect("RPAREN");
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &es = _p2.value();
                return es;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::shared_ptr<Expr>> item(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("ID");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &i = _p1.value();
                return std::make_shared<StatementPointer>(i.value, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("TOKEN");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &i = _p1.value();
                return std::make_shared<TokenPointer>(i.value, POSITION);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("STRING");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &i = _p1.value();
                return std::make_shared<ConstantString>(i.value, POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    void clear_memoization_caches(){
    }
};

}
