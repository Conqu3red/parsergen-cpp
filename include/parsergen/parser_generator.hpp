#pragma once
#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
#include "parsergen/grammar_parser.hpp"

#include <assert.h>
#include <list>
#include <vector>
#include <tuple>
#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <fmt/format.h>
#include <sstream>
#include <algorithm>
#include <set>
#include "tsl/ordered_map.h"



using namespace Parsergen::AST;

typedef std::vector<std::tuple<int, std::shared_ptr<Expr>>> OtherFunctionsQueue;

namespace Parsergen {

class GenerationOutput {
public:
    std::stringstream &stream;
    int &indentation;
    bool add_newline;
    GenerationOutput(std::stringstream &stream, int &indentation, bool add_newline)
        : stream(stream)
        , indentation(indentation)
        , add_newline(add_newline)
    {}
    friend GenerationOutput &operator <<(GenerationOutput &out, std::string in);
};

GenerationOutput &operator <<(GenerationOutput &out, std::string in){
    std::string indent(out.indentation * 4, ' ');
    in = indent + utils::ReplaceAll(in, "\n", "\n" + indent) + (out.add_newline ? "\n" : ""); // align all lines for indentation
    out.stream << in;
    return out;
}

class Generator {
public:
    static const std::string _header_;
    
    GenerationOutput emit;
    GenerationOutput emitp;
    std::stringstream result;
    int counter = 0;
    int _indent;
    std::map<std::string, std::string> config;
    tsl::ordered_map<std::string, StatementGroup> rules;
    std::shared_ptr<ParserDefinition> parser_definition;
    tsl::ordered_map<std::string, std::set<std::string>> node_map;
    std::vector<std::string> memoization_caches;
    
    Generator(std::shared_ptr<ParserDefinition> parser_definition)
        : parser_definition(parser_definition)
        , _indent(0)
        , emit(GenerationOutput(result, _indent, true))
        , emitp(GenerationOutput(result, _indent, false))
    {
        config = {
            {"class_name", "CustomParser"},
            {"inherits_from", "Parser"},
            {"header", ""},
            {"footer", ""},
        };

    }

    void indent(){
        _indent += 1;
    }
    
    void unindent(){
        _indent -= 1;
    }

    auto construct_mapping_graph(){
        for (auto &[name, stmt_group] : rules){
            for (auto &stmt : stmt_group.statements){
                utils::set_union(node_map[name], map_node(stmt));
            }
        }
    }

    std::set<std::string> map_node(std::shared_ptr<ASTNode> node){
        std::set<std::string> result;
        if (node->is("Statement")){
            for (auto &part : std::dynamic_pointer_cast<Statement>(node)->exprs)
                utils::set_union(result, map_node(part));
        }
        if (node->is("ExprList")){
            for (auto &part : std::dynamic_pointer_cast<ExprList>(node)->exprs)
                utils::set_union(result, map_node(part));
        }
        if (node->is("ZeroOrMore") || node->is("OneOrMore") || node->is("ZeroOrOne"))
            utils::set_union(result, map_node(std::dynamic_pointer_cast<Quanitifier>(node)->expr));
        if (node->is("NamedItem"))
            utils::set_union(result, map_node(std::dynamic_pointer_cast<NamedItem>(node)->expr));
        if (node->is("StatementPointer"))
            result.insert(std::dynamic_pointer_cast<StatementPointer>(node)->target);
        return result;
    }

    bool is_left_recursive(std::string name){
        if (config["disable_left_recursion"] != "")
            return false;
        auto locations = node_located_in(name);
        while (true){
            std::set<std::string> prev = {};
            utils::set_union(prev, locations);
            for (auto &loc : locations){
                utils::set_union(locations, node_located_in(loc));
            }
            for (auto &loc : node_map[name]){
                if (locations.count(loc))
                    return true;
            }
            if (locations == prev) break;
        }
        return false;
    }

    std::set<std::string> node_located_in(std::string name){
        std::set<std::string> located_in;
        for (auto &[k, data] : node_map){
            if (data.count(name))
                located_in.insert(k);
        }
        return located_in;
    }

    // TODO: implement is_left_recursive(std::string name)
    // store statement_mapping_graph somewhere in the class so we don't recalculate on each call to is_left_rec...


    void process_sections(){
        std::list<std::shared_ptr<StatementGroup>> statement_group_list;
        for (auto &section : parser_definition->sections){
            if (section->is("StatementGroup")){
                statement_group_list.push_back(std::dynamic_pointer_cast<StatementGroup>(section));
            }
            else { // ConfigurationCall
                auto config_call = std::dynamic_pointer_cast<ConfigurationCall>(section);
                config[config_call->name] = config_call->value;
            }
        }

        post_process(statement_group_list);
    }

    void generate(){
        process_sections();
        construct_mapping_graph();
        emit << _header_;
        emit << config["header"];
        generate_parser_class();
        emit << config["footer"];
    }

    void generate_clear_memoization_cache_function(){
        emit << "void clear_memoization_cache(){";
        indent();
            for (auto &entry : memoization_caches){
                emit << fmt::format("{}.clear();", entry);
            }
        unindent();
        emit << "}";
    }

    void post_process(std::list<std::shared_ptr<StatementGroup>> rules_list){
        for (auto &r : rules_list){
            for (auto &stmt : r->statements)
                if (stmt->return_type == "<>")
                    stmt->return_type = r->return_type;
            
            rules[r->name] = *r;
        }
    }

    void generate_parser_class(){
        emit << fmt::format("class {} : public {} {{", config["class_name"], config["inherits_from"]);
        emit << "public:";
        indent();
        emit << fmt::format("using {0}::{0};", config["inherits_from"]);
        for (auto &[name, stmt_group] : rules){
            std::cout << name << "\n";
            generate_stmt_group(stmt_group);
            std::cout << "done\n";
        }
        generate_clear_memoization_cache_function();
        unindent();
        emit << "};";
    }

    void create_left_recursion_memoization(std::string return_type, std::string cache_name, std::string function_name){
        memoization_caches.push_back(cache_name);
        emit << fmt::format("std::map<int, std::tuple<{}, int>> {};", return_type, cache_name);
        emit << fmt::format("{} {}(){{", return_type, function_name);
        indent();
            emit << "auto pos = mark();";
            emit << fmt::format("auto memoized = {}.find(pos);", cache_name);
            emit << fmt::format("{} res;", return_type);
            emit << "int endpos;";
            emit << fmt::format("if (memoized != {}.end()) {{", cache_name);
            indent();
                emit << "res = std::get<0>(memoized->second);";
                emit << "endpos = std::get<1>(memoized->second);";
                emit << "set_pos(endpos);";
            unindent();
            emit << "}";
            emit << "else {";
            indent();
                // prime the cache with a failure.
                emit << fmt::format("{} lastres = std::nullopt;", return_type);
                emit << "auto lastpos = pos;";
                emit << fmt::format("{}[pos] = std::make_tuple(lastres, lastpos);", cache_name);
                // Loop until no longer parse is obtained.
                emit << "while (true) {";
                indent();
                    emit << "set_pos(pos);";
                    emit << fmt::format("res = _{}();", function_name);
                    emit << "endpos = mark();";
                    emit << "if (endpos <= lastpos) break;";
                    emit << "lastres = res;";
                    emit << "lastpos = endpos;";
                    emit << fmt::format("{}[pos] = std::make_tuple(res, endpos);", cache_name);
                unindent();
                emit << "}";
                emit << "res = lastres;";
                emit << "set_pos(lastpos);";
            unindent();
            emit << "}";
            emit << "return res;";
        unindent();
        emit << "}";
    }

    void generate_stmt_group(StatementGroup stmt_group){
        for (auto &stmt : stmt_group.statements){
            pre_process(stmt->exprs);
        }
        auto function_return_type = fmt::format("std::optional<{}>", stmt_group.return_type);
        
        if (is_left_recursive(stmt_group.name)){
            auto cache_name = fmt::format("__{}__memoization_cache", stmt_group.name);
            create_left_recursion_memoization(function_return_type, cache_name, stmt_group.name);
            
            emit << fmt::format("{} _{}(){{", function_return_type, stmt_group.name);
        }
        else
            emit << fmt::format("{} {}(){{", function_return_type, stmt_group.name);
        
        indent();
            emit << "Position start, end;";
            emit << "int pos = mark();";
            for (auto &stmt : stmt_group.statements){
                generate_statement(stmt);
            }
            emit << "return {};";
        unindent();
        emit << "}";
    }

    void generate_statement(std::shared_ptr<Statement> stmt){
        emit << "for (;;){";
        indent();
        emit << "{"; // new scope
        indent();
            emit << "start = current_pos();";
            int part_number = 0;
            std::vector<std::pair<int, std::string>> named_items;
            for (auto &expr : stmt->exprs){
                std::cout << "  " << expr->class_name() << "\n";
                part_number++;
                if (expr->is("NamedItem")){
                    auto item = std::dynamic_pointer_cast<NamedItem>(expr);
                    named_items.push_back(std::make_pair(part_number, item->name));
                    gen(item->expr, part_number);
                }
                else
                    gen(expr, part_number);
                
                if (expr->class_name() != "AndPredicate" && expr->class_name() != "NotPredicate"){
                    // Predicates generate this bit themselves, TODO: add a property so any node can specify this!
                    emit << fmt::format("if (!_p{:d}.has_value()){{", part_number);
                    indent();
                        emit << "fail();";
                        emit << "break;";
                    unindent();
                    emit << "}";
                }
                emit << fmt::format("auto &p{0:d} = _p{0:d}.value();", part_number);
            }
            emit << "end = current_pos();";
            emit << "// match:";
            for (auto &[part_num, name] : named_items){
                emit << fmt::format("auto &{} = _p{:d}.value();", name, part_num);
            }
            emit << fmt::format("return {};", stmt->action);
        unindent();
        emit << "}"; // end scope
        unindent();
        emit << "}";
        emit << "set_pos(pos);";
    }

    void gen(std::shared_ptr<Expr> expr, int &part){
        if (expr->is("TokenPointer")) gen(std::dynamic_pointer_cast<TokenPointer>(expr), part);
        else if (expr->is("StatementPointer")) gen(std::dynamic_pointer_cast<StatementPointer>(expr), part);
        else if (expr->is("ConstantString")) gen(std::dynamic_pointer_cast<ConstantString>(expr), part);
        else if (expr->is("AndPredicate")) gen(std::dynamic_pointer_cast<AndPredicate>(expr), part);
        else if (expr->is("NotPredicate")) gen(std::dynamic_pointer_cast<NotPredicate>(expr), part);
        else if (expr->is("ZeroOrMore")) gen(std::dynamic_pointer_cast<ZeroOrMore>(expr), part);
        else if (expr->is("OneOrMore")) gen(std::dynamic_pointer_cast<OneOrMore>(expr), part);
        else if (expr->is("ZeroOrOne")) gen(std::dynamic_pointer_cast<ZeroOrOne>(expr), part);
        else if (expr->is("ExprList")) gen(std::dynamic_pointer_cast<ExprList>(expr), part);
        else
            throw std::runtime_error(fmt::format("Invalid type '{}'", expr->class_name()));
    }

    void gen(std::shared_ptr<TokenPointer> item, int &part){
        emit << fmt::format("auto _p{:d} = expect(\"{}\");", part, item->target);
    }

    void gen(std::shared_ptr<StatementPointer> item, int &part){
        emit << fmt::format("auto _p{:d} = {}();", part, item->target);
    }

    void gen(std::shared_ptr<ConstantString> item, int &part){
        emit << fmt::format("auto _p{:d} = expect_constant(\"{}\");", part, item->value);
    }

    void gen(std::shared_ptr<AndPredicate> item, int &part){
        part--;
        emit << "{"; // scope
        indent();
            emit << "auto _predicate_pos = mark();";
            int _p = 0;
            gen(item->expr, _p); // will always be "_p0", this is fine because we are in a new scope
            emit << "set_pos(_predicate_pos);";
            emit << "if (!_p0.has_value()){";
            indent();
                emit << "fail();";
                emit << "break;";
            unindent();
            emit << "}";

        unindent();
        emit << "}";
    }

    void gen(std::shared_ptr<NotPredicate> item, int &part){
        part--;
        emit << "{"; // scope
        indent();
            emit << "auto _predicate_pos = mark();";
            int _p = 0;
            gen(item->expr, _p); // will always be "_p0", this is fine because we are in a new scope
            emit << "set_pos(_predicate_pos);";
            emit << "if (_p0.has_value()){";
            indent();
                emit << "fail();";
                emit << "break;";
            unindent();
            emit << "}";

        unindent();
        emit << "}";
    }

    void gen(std::shared_ptr<ZeroOrMore> item, int &part){
        emit << fmt::format("auto _p{:d} = {}();", part, item->handle_name);
    }

    void gen(std::shared_ptr<OneOrMore> item, int &part){
        emit << fmt::format("auto _p{:d} = {}();", part, item->handle_name);
    }

    void gen(std::shared_ptr<ZeroOrOne> item, int &part){
        emit << fmt::format("auto _p{:d} = {}();", part, item->handle_name);
    }

    void gen(std::shared_ptr<ExprList> item, int &part){
        // ExprList needs to pre pre-processed at the top of a block, so we can get it's return type
        emit << fmt::format("auto _p{:d} = {}();", part, item->handle_name);
    }

    std::string get_function_name(std::shared_ptr<Expr> expr){
        if (expr->is("ZeroOrMore")) return std::dynamic_pointer_cast<ZeroOrMore>(expr)->handle_name;
        if (expr->is("OneOrMore")) return std::dynamic_pointer_cast<OneOrMore>(expr)->handle_name;
        if (expr->is("ZeroOrOne")) return std::dynamic_pointer_cast<ZeroOrOne>(expr)->handle_name;
        if (expr->is("ExprList")) return std::dynamic_pointer_cast<ExprList>(expr)->handle_name;
        if (expr->is("StatementPointer")) return std::dynamic_pointer_cast<StatementPointer>(expr)->target;
        if (expr->is("TokenPointer")) return "expect";
        if (expr->is("ConstantString")) return "expect_constant";
        return "";
    }

    std::string get_return_type(std::shared_ptr<Expr> expr){
        if (expr->is("StatementPointer")){
            return rules[std::dynamic_pointer_cast<StatementPointer>(expr)->target].return_type;
        }
        std::string fname = get_function_name(expr);
        if (fname == "")
            return "";
        if (expr->is("TokenPointer") || expr->is("ConstantString"))
            return "Token"; // should maybe be std::optional<Token>
        return fmt::format("decltype(std::declval<{}>().{}().value_or(NULL))", config["class_name"], fname);
    }

    void pre_process(std::vector<std::shared_ptr<Expr>> &exprs){
        for (auto &expr : exprs){
            pre_process(expr);
        }
    }

    void pre_process(std::shared_ptr<Expr> &expr){
        resolve(expr, counter);
        counter++;
    }


    void resolve(std::shared_ptr<Expr> expr, int &c){
        if (expr->is("ZeroOrMore")) resolve(std::dynamic_pointer_cast<ZeroOrMore>(expr), c);
        else if (expr->is("OneOrMore")) resolve(std::dynamic_pointer_cast<OneOrMore>(expr), c);
        else if (expr->is("ZeroOrOne")) resolve(std::dynamic_pointer_cast<ZeroOrOne>(expr), c);
        else if (expr->is("ExprList")) resolve(std::dynamic_pointer_cast<ExprList>(expr), c);
        else if (expr->is("NamedItem")) resolve(std::dynamic_pointer_cast<NamedItem>(expr)->expr, c);
        //else
        //    throw std::runtime_error(fmt::format("Invalid type '{}'", expr->class_name()));
    }

    void resolve(std::shared_ptr<ZeroOrMore> item, int &c){
        pre_process(item->expr);
        auto return_type = get_return_type(item->expr);
        item->handle_name = fmt::format("_loop_{:d}", c);
        emit << fmt::format("std::optional< std::vector<{}> > {}(){{", return_type, item->handle_name);
        indent();
            emit << fmt::format("std::vector<{}> children;", return_type);
            emit << "for (;;){";
            indent();
            emit << "{";
            indent();
                emit << "int pos = mark();";
                int _p = 0;
                gen(item->expr, _p);
                emit << fmt::format("if (!_p0.has_value()){{");
                indent();
                    emit << "set_pos(pos);";
                    emit << "break;";
                unindent();
                emit << "}";
                emit << "auto &p0 = _p0.value();";
                emit << "children.push_back(p0);";
            unindent();
            emit << "}";
            unindent();
            emit << "}";
            emit << "return children;";
        unindent();
        emit << "}";
    }

    void resolve(std::shared_ptr<OneOrMore> item, int &c){
        pre_process(item->expr);
        auto return_type = get_return_type(item->expr);
        item->handle_name = fmt::format("_loop_{:d}", c);
        emit << fmt::format("std::optional< std::vector<{}> > {}{}(){{", return_type, item->handle_name);
        indent();
            emit << fmt::format("std::vector<{}> children;", return_type);
            emit << "for (;;){";
            indent();
            emit << "{";
            indent();
                emit << "int pos = mark();";
                int _p = 0;
                gen(item->expr, _p);
                emit << fmt::format("if (!_p0.has_value()){{");
                indent();
                    emit << "set_pos(pos);";
                    emit << "break;";
                unindent();
                emit << "}";
                emit << "auto &p0 = _p0.value();";
                emit << "children.push_back(p0);";
            unindent();
            emit << "}";
            unindent();
            emit << "}";
            emit << "if (children.size() == 0)";
            indent();
                emit << "return {};";
            unindent();
            emit << "return children;";
        unindent();
        emit << "}";
    }

    void resolve(std::shared_ptr<ZeroOrOne> item, int &c){
        pre_process(item->expr);
        auto return_type = get_return_type(item->expr);
        item->handle_name = fmt::format("_maybe_{:d}", c);
        emit << fmt::format("std::optional< std::optional<{}> > {}(){{", return_type, item->handle_name);
        indent();
            //emit << fmt::format("std::optional<{}> value;", return_type);
            emit << "int pos = mark();";
            emit << fmt::format("auto rv = std::make_optional<std::optional<{}>>(std::nullopt);", return_type);
            emit << "rv.value() = std::nullopt;";
            int _p = 0;
            gen(item->expr, _p);
            emit << fmt::format("if (!_p0.has_value()){{");
            indent();
                emit << "set_pos(pos);";
                emit << "return rv;";
            unindent();
            emit << "}";
            emit << "auto &p0 = _p0.value();";
            emit << "rv.value() = _p0;";
            emit << "return rv;";
        unindent();
        emit << "}";
    }

    void resolve(std::shared_ptr<ExprList> item, int &c){
        std::string tuple_types = "";
        for (auto &expr : item->exprs){
            pre_process(expr);
            std::string rt = get_return_type(expr);
            if (rt != "")
                tuple_types += rt + ", ";
        }
        if (utils::endsWith(tuple_types, ", "))
            tuple_types.erase(tuple_types.end()-2, tuple_types.end());
        std::string return_type = "std::optional<std::tuple<" + tuple_types + ">>";
        item->handle_name = fmt::format("_expr_list_{:d}", c);
        emit << fmt::format("{} {}(){{", return_type, item->handle_name);
        indent();
            emit << "Position start, end;";
            emit << "int pos = mark();";
            emit << "for (;;){";
            indent();
            // main generation
            emit << "start = current_pos();";
            int part_number = 0;
            std::string result_return = "";
            for (auto &expr : item->exprs){
                std::cout << "  " << expr->class_name() << "\n";
                part_number++;
                if (expr->is("NamedItem")){
                    auto item = std::dynamic_pointer_cast<NamedItem>(expr);
                    gen(item->expr, part_number);
                }
                else
                    gen(expr, part_number);
                
                if (expr->class_name() != "AndPredicate" && expr->class_name() != "NotPredicate"){
                    // Predicates generate this bit themselves, TODO: add a property so any node can specify this!
                    emit << fmt::format("if (!_p{:d}.has_value()){{", part_number);
                    indent();
                        emit << "fail();";
                        emit << "break;";
                    unindent();
                    emit << "}";
                }
                emit << fmt::format("auto &p{0:d} = _p{0:d}.value();", part_number);
                result_return += fmt::format("p{:d}, ", part_number);
            }
            emit << "end = current_pos();";
            emit << "// match:";
            if (utils::endsWith(result_return, ", "))
                result_return.erase(result_return.end()-2, result_return.end());
            emit << fmt::format("return std::make_tuple({});", result_return);
            unindent();
            emit << "}"; // end for loop
            emit << "set_pos(pos);";
            
            emit << "return {};";
        unindent();
        emit << "}";
    }

};

const std::string Generator::_header_ = R"(
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

)";

std::string generate_parser(std::string grammar){
    GrammarLexer l;
    l.setText(grammar);
    l.Lex();
    GrammarParser parser(std::make_shared<TokenStream>(std::make_shared<GrammarLexer>(l)));
    auto p = parser.parser_definition();
    if (p.has_value()){
        Generator g(p.value());
        g.generate();
        return g.result.str();
    }
    else {
        auto e = parser.error();
        if (e.has_value())
            throw e.value();
        throw ParseError("Parse Error", Position(), Position(), "");
    }
}

}