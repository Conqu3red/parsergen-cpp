
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



#include "examples/json/json_lexer.hpp"
#include "examples/json/json_nodes.hpp"

class JsonParser : public Parser {
public:
    using Parser::Parser;
    std::optional<JSON::Value> json(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = value();
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
    std::optional<JSON::Value> value(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = object();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &e = _p1.value();
                return JSON::Value(e);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = array();
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &e = _p1.value();
                return JSON::Value(e);
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
                auto &e = _p1.value();
                return JSON::Value(e.value);
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("NUMBER");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &e = _p1.value();
                return JSON::Value(std::stod(e.value));
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("BOOL");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &e = _p1.value();
                return JSON::Value(e.value == "true");
            }
        }
        set_pos(pos);
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("NULL");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                end = current_pos();
                // match:
                auto &e = _p1.value();
                return JSON::Value();
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::tuple<Token, std::pair<std::string, JSON::Value>>> _expr_list_0(){
        Position start, end;
        int pos = mark();
        for (;;){
            start = current_pos();
            auto _p1 = expect("COMMA");
            if (!_p1.has_value()){
                fail();
                break;
            }
            auto &p1 = _p1.value();
            auto _p2 = member();
            if (!_p2.has_value()){
                fail();
                break;
            }
            auto &p2 = _p2.value();
            end = current_pos();
            // match:
            return std::make_tuple(p1, p2);
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<decltype(std::declval<JsonParser>()._expr_list_0().value_or(NULL))> > _loop_1(){
        std::vector<decltype(std::declval<JsonParser>()._expr_list_0().value_or(NULL))> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = _expr_list_0();
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
    std::optional<std::tuple<std::pair<std::string, JSON::Value>, decltype(std::declval<JsonParser>()._loop_1().value_or(NULL))>> _expr_list_2(){
        Position start, end;
        int pos = mark();
        for (;;){
            start = current_pos();
            auto _p1 = member();
            if (!_p1.has_value()){
                fail();
                break;
            }
            auto &p1 = _p1.value();
            auto _p2 = _loop_1();
            if (!_p2.has_value()){
                fail();
                break;
            }
            auto &p2 = _p2.value();
            end = current_pos();
            // match:
            return std::make_tuple(p1, p2);
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::optional<decltype(std::declval<JsonParser>()._expr_list_2().value_or(NULL))> > _maybe_3(){
        int pos = mark();
        auto rv = std::make_optional<std::optional<decltype(std::declval<JsonParser>()._expr_list_2().value_or(NULL))>>(std::nullopt);
        rv.value() = std::nullopt;
        auto _p0 = _expr_list_2();
        if (!_p0.has_value()){
            set_pos(pos);
            return rv;
        }
        auto &p0 = _p0.value();
        rv.value() = _p0;
        return rv;
    }
    std::optional<std::shared_ptr<JSON::JsonObject>> object(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("LCURLY");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = _maybe_3();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = expect("RCURLY");
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &members = _p2.value();
                return construct_object(members);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::pair<std::string, JSON::Value>> member(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("STRING");
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
                auto _p3 = value();
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &key = _p1.value();
                auto &value = _p3.value();
                return std::make_pair(key.value, value);
            }
        }
        set_pos(pos);
        return {};
    }
    std::optional<std::tuple<Token, JSON::Value>> _expr_list_4(){
        Position start, end;
        int pos = mark();
        for (;;){
            start = current_pos();
            auto _p1 = expect("COMMA");
            if (!_p1.has_value()){
                fail();
                break;
            }
            auto &p1 = _p1.value();
            auto _p2 = value();
            if (!_p2.has_value()){
                fail();
                break;
            }
            auto &p2 = _p2.value();
            end = current_pos();
            // match:
            return std::make_tuple(p1, p2);
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::vector<decltype(std::declval<JsonParser>()._expr_list_4().value_or(NULL))> > _loop_5(){
        std::vector<decltype(std::declval<JsonParser>()._expr_list_4().value_or(NULL))> children;
        for (;;){
            {
                int pos = mark();
                auto _p0 = _expr_list_4();
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
    std::optional<std::tuple<JSON::Value, decltype(std::declval<JsonParser>()._loop_5().value_or(NULL))>> _expr_list_6(){
        Position start, end;
        int pos = mark();
        for (;;){
            start = current_pos();
            auto _p1 = value();
            if (!_p1.has_value()){
                fail();
                break;
            }
            auto &p1 = _p1.value();
            auto _p2 = _loop_5();
            if (!_p2.has_value()){
                fail();
                break;
            }
            auto &p2 = _p2.value();
            end = current_pos();
            // match:
            return std::make_tuple(p1, p2);
        }
        set_pos(pos);
        return {};
    }
    std::optional< std::optional<decltype(std::declval<JsonParser>()._expr_list_6().value_or(NULL))> > _maybe_7(){
        int pos = mark();
        auto rv = std::make_optional<std::optional<decltype(std::declval<JsonParser>()._expr_list_6().value_or(NULL))>>(std::nullopt);
        rv.value() = std::nullopt;
        auto _p0 = _expr_list_6();
        if (!_p0.has_value()){
            set_pos(pos);
            return rv;
        }
        auto &p0 = _p0.value();
        rv.value() = _p0;
        return rv;
    }
    std::optional<std::shared_ptr<JSON::JsonArray>> array(){
        Position start, end;
        int pos = mark();
        for (;;){
            {
                start = current_pos();
                auto _p1 = expect("LSQUARE");
                if (!_p1.has_value()){
                    fail();
                    break;
                }
                auto &p1 = _p1.value();
                auto _p2 = _maybe_7();
                if (!_p2.has_value()){
                    fail();
                    break;
                }
                auto &p2 = _p2.value();
                auto _p3 = expect("RSQUARE");
                if (!_p3.has_value()){
                    fail();
                    break;
                }
                auto &p3 = _p3.value();
                end = current_pos();
                // match:
                auto &values = _p2.value();
                return construct_array(values);
            }
        }
        set_pos(pos);
        return {};
    }
    void clear_memoization_caches(){
    }
};

