
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



#include <math.h>

class CalcParser : public Parser {
public:
    using Parser::Parser;
    std::optional<int> start(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expr();
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
                auto &e = _p1.value();
                return e;
            }
        }
        set_pos(pos);
        return {};
    }
    std::map<int, std::tuple<std::optional<int>, int>> __expr__memoization_cache;
    std::optional<int> expr(){
        auto pos = mark();
        auto memoized = __expr__memoization_cache.find(pos);
        std::optional<int> res;
        int endpos;
        if (memoized != __expr__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<int> lastres = std::nullopt;
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
    std::optional<int> _expr(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expr();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("ADD");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = term();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &left = _p1.value();
                auto &right = _p3.value();
                return left + right;
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expr();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = expect("SUB");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = term();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &left = _p1.value();
                auto &right = _p3.value();
                return left - right;
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
                auto &e = _p1.value();
                return e;
            }
        }
        set_pos(pos);
        return {};
    }
    std::map<int, std::tuple<std::optional<int>, int>> __term__memoization_cache;
    std::optional<int> term(){
        auto pos = mark();
        auto memoized = __term__memoization_cache.find(pos);
        std::optional<int> res;
        int endpos;
        if (memoized != __term__memoization_cache.end()) {
            res = std::get<0>(memoized->second);
            endpos = std::get<1>(memoized->second);
            set_pos(endpos);
        }
        else {
            std::optional<int> lastres = std::nullopt;
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
    std::optional<int> _term(){
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
                auto _p2 = expect("MUL");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = factor();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &left = _p1.value();
                auto &right = _p3.value();
                return left * right;
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
                auto _p2 = expect("DIV");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = factor();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &left = _p1.value();
                auto &right = _p3.value();
                return left / right;
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
                auto &e = _p1.value();
                return e;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<int> factor(){
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
                auto _p2 = expect("POW");
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = factor();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &left = _p1.value();
                auto &right = _p3.value();
                return pow(left, right);
            }
        }
        set_pos(pos);
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
                auto &e = _p1.value();
                return e;
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<int> item(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("INT");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &n = _p1.value();
                return std::stoi(n.value);
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
                auto _p2 = expr();
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
                auto &e = _p2.value();
                return e;
            }
        }
        set_pos(pos);
        return {};
    }
    void clear_memoization_caches(){
        __expr__memoization_cache.clear();
        __term__memoization_cache.clear();
    }
};

