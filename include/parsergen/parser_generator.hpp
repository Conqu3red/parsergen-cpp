#pragma once
#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
#include "parsergen/grammar_parser.hpp"
#include "parsergen/introspection.hpp"

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

namespace Parsergen {

class Generator;

class Generator {
public:
    static std::string header;
    std::stringstream result;
    tsl::ordered_map<std::string, StatementGroup> rules;
    
    Generator(std::shared_ptr<ParserDefinition> parser_definition)
        : parser_definition(std::move(parser_definition))
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

    void construct_mapping_graph();
    bool is_left_recursive(std::string name);
    void process_sections();
    void do_introspection();
    
    void generate(){
        process_sections();
        do_introspection();
        emit(header);
        emit(config["header"]);
        generate_parser_class();
        emit(config["footer"]);
    }

private:
    std::shared_ptr<ParserDefinition> parser_definition;
    int _indent = 0;
    int counter = 0;
    std::map<std::string, std::string> config;
    std::vector<std::string> memoization_caches;
   
    std::map<std::string, Introspection::RuleInfo> rule_info;

    void emit(std::string in);
    void warn(std::string text);
    
    void generate_clear_memoization_cache_function();
    void post_process(std::list<std::shared_ptr<StatementGroup>> rules_list);
    void generate_parser_class();
    void create_left_recursion_memoization(std::string return_type, std::string cache_name, std::string function_name);
    
    void generate_stmt_group(StatementGroup stmt_group);
    void generate_statement(std::shared_ptr<Statement> stmt);
    
    void gen(std::shared_ptr<Expr> expr, int &part);
    void gen(std::shared_ptr<TokenPointer> item, int &part);
    void gen(std::shared_ptr<StatementPointer> item, int &part);
    void gen(std::shared_ptr<ConstantString> item, int &part);
    void gen(std::shared_ptr<AndPredicate> item, int &part);
    void gen(std::shared_ptr<NotPredicate> item, int &part);
    void gen(std::shared_ptr<ZeroOrMore> item, int &part);
    void gen(std::shared_ptr<OneOrMore> item, int &part);
    void gen(std::shared_ptr<ZeroOrOne> item, int &part);
    void gen(std::shared_ptr<ExprList> item, int &part);

    std::string get_function_name(std::shared_ptr<Expr> expr);
    std::string get_return_type(std::shared_ptr<Expr> expr);

    void pre_process(std::vector<std::shared_ptr<Expr>> &exprs);
    void pre_process(std::shared_ptr<Expr> &expr);

    void resolve(std::shared_ptr<Expr> expr, int &c);
    void resolve(std::shared_ptr<ZeroOrMore> item, int &c);
    void resolve(std::shared_ptr<OneOrMore> item, int &c);
    void resolve(std::shared_ptr<ZeroOrOne> item, int &c);
    void resolve(std::shared_ptr<ExprList> item, int &c);

};

std::string generate_parser(std::string grammar);

}