
#include "parsergen/lexer.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"

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
    std::optional< std::vector<std::shared_ptr<Statement>> > _loop_11(){
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
                auto _p3 = _loop_11();
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
    std::optional< std::optional<Token> > _maybe_13(){
        int pos = mark();
        auto rv = std::make_optional<std::optional<Token>>(std::nullopt);
        rv.value() = std::nullopt;
        auto _p0 = expect("RETURN_TYPE");
        if (!_p0.has_value()){
            set_pos(pos);
            return rv;
        }
        auto &p0 = _p0.value();
        rv.value() = _p0;
        return rv;
    }
    std::optional< std::vector<std::shared_ptr<Expr>> > _loop_16(){
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
    std::optional< std::optional<Token> > _maybe_18(){
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
                auto _p1 = _maybe_13();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("COLON");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = _loop_16();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                auto _p4 = _maybe_18();
                if (!_p4.has_value()){
                    fail();
                    break;
                }
                auto &p4 = _p4.value();
                auto _p5 = expect("TERMINATE");
                if (!_p5.has_value()){
                    fail();
                    break;
                }
                auto &p5 = _p5.value();
                end = current_pos();
                // match:
                auto &return_type = _p1.value();
                auto &es = _p3.value();
                auto &a = _p4.value();
                return std::make_shared<Statement>(getTokenValueOr(return_type, "<>"), es, getTokenValueOr(a, "<>"), POSITION);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<std::shared_ptr<Expr>> > _loop_21(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<ExprList>>, int>> __expr_list__memoization_cache;
    std::optional<std::shared_ptr<ExprList>> expr_list(){
        auto pos = mark();
        auto memoized = __expr_list__memoization_cache.find(pos);
        std::optional<std::shared_ptr<ExprList>> res;
        int endpos;
        if (memoized != __expr_list__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<ExprList>> lastres = std::nullopt;
            auto lastpos = pos;
            __expr_list__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _expr_list();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __expr_list__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<ExprList>> _expr_list(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = _loop_21();
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __expr__memoization_cache;
    std::optional<std::shared_ptr<Expr>> expr(){
        auto pos = mark();
        auto memoized = __expr__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __expr__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __expr__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _expr();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __expr__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _expr(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __or_op__memoization_cache;
    std::optional<std::shared_ptr<Expr>> or_op(){
        auto pos = mark();
        auto memoized = __or_op__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __or_op__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __or_op__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _or_op();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __or_op__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _or_op(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __star_op__memoization_cache;
    std::optional<std::shared_ptr<Expr>> star_op(){
        auto pos = mark();
        auto memoized = __star_op__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __star_op__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __star_op__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _star_op();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __star_op__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _star_op(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __plus_op__memoization_cache;
    std::optional<std::shared_ptr<Expr>> plus_op(){
        auto pos = mark();
        auto memoized = __plus_op__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __plus_op__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __plus_op__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _plus_op();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __plus_op__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _plus_op(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __qmark_op__memoization_cache;
    std::optional<std::shared_ptr<Expr>> qmark_op(){
        auto pos = mark();
        auto memoized = __qmark_op__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __qmark_op__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __qmark_op__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _qmark_op();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __qmark_op__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _qmark_op(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __term__memoization_cache;
    std::optional<std::shared_ptr<Expr>> term(){
        auto pos = mark();
        auto memoized = __term__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __term__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __term__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _term();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __term__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _term(){
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
    std::map<int, std::tuple<std::optional<std::shared_ptr<Expr>>, int>> __factor__memoization_cache;
    std::optional<std::shared_ptr<Expr>> factor(){
        auto pos = mark();
        auto memoized = __factor__memoization_cache.find(pos);
        std::optional<std::shared_ptr<Expr>> res;
        int endpos;
        if (memoized != __factor__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<std::shared_ptr<Expr>> lastres = std::nullopt;
            auto lastpos = pos;
            __factor__memoization_cache[pos] = std::make_tuple(lastres, lastpos);
            while (true) {
                set_pos(pos);
                res = _factor();
                endpos = mark();
                if (endpos <= lastpos) break;
                lastres = res;
                lastpos = endpos;
                __factor__memoization_cache[pos] = std::make_tuple(res, endpos);
            }
            res = lastres;
            set_pos(lastpos);
        }
        return res;
    }
    std::optional<std::shared_ptr<Expr>> _factor(){
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
};

}
