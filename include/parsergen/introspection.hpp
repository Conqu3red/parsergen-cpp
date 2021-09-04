#pragma once
#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser.hpp"
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
namespace Parsergen {

class Generator;

}

namespace Parsergen::Introspection {

enum struct NullMatch {
    no, // a rulle will *always* consume some tokens
    yes, // a rule can succeed without consuming any tokens
    indeterminate // algorithm couldn't converge
};

enum struct InfiniteLoop {
    no,
    yes,
    indeterminate
};

enum struct LeftRecursive {
    no,
    yes
};

struct RuleInfo {
public:
    NullMatch null_match { NullMatch::indeterminate };
    InfiniteLoop infinite_loop { InfiniteLoop::indeterminate };
    LeftRecursive left_recursive { LeftRecursive::no };
    bool flagged { false };
};

class Introspector {
public:
    Generator *generator;
    std::map<std::string, RuleInfo> rule_info;
    
    std::map<std::string, std::set<std::string>> shallow_references;

    
    std::vector<std::set<std::string>> completed_cycle_permutations;
    std::vector<std::vector<std::string>> all_cycles;

    
    Introspector(){

    }

    Introspector(Generator *generator) : generator(generator) {}

    NullMatch nullMatching(std::shared_ptr<ASTNode> node);

    NullMatch nullMatching(std::string rule_name);

    // TODO: implement infiniteLooping

    //InfiniteLoop infiniteLooping(std::shared_ptr<ASTNode> node){
    //    InfiniteLoop infinite_looping;
    //    auto name = node->class_name();
    //    // should just return indeterminate for StatementPointer like on NullMatch
    //    if (name == "StatementPointer"){ // loops if one of it's children loops
    //        auto &group = generator->rules[std::dynamic_pointer_cast<StatementPointer>(node)->target];
    //        for (auto &s : group.statements)
    //            for (auto &expr : s->exprs){
    //                infinite_looping = infiniteLooping(expr);
    //            }
    //    }
    //}

    void leftRecursions(std::shared_ptr<ASTNode> node, std::set<std::string> &elements);
    // find all StatementPointers that *might* be able to be reached without consuming tokens/input
    std::set<std::string> leftRecursions(std::string name);

    std::vector<std::vector<std::string>> get_cycles(std::string searching_for);

    void introspect();
};

}