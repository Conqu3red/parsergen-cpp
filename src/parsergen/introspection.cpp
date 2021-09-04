#include "parsergen/introspection.hpp"
#include "parsergen/parser_generator.hpp"
#include <algorithm>

namespace Parsergen::Introspection {

NullMatch Introspector::nullMatching(std::shared_ptr<ASTNode> node){
    // NullMatching expressions are defined as expressions that are able to return without consuming
    // any tokens
    
    NullMatch null_matching;
    auto name = node->class_name();
    if (name == "Statement"){ // can null match if all children null match
        for (auto &expr : std::dynamic_pointer_cast<Statement>(node)->exprs)
            if (nullMatching(expr) == NullMatch::no)
                return NullMatch::no;
        for (auto &expr : std::dynamic_pointer_cast<Statement>(node)->exprs)
            if (nullMatching(expr) == NullMatch::indeterminate)
                return NullMatch::indeterminate;
        
        return NullMatch::yes;
    }
    if (name == "StatementPointer")
        return rule_info[std::dynamic_pointer_cast<StatementPointer>(node)->target].null_match;
    if (name == "TokenPointer" || name == "ConstantString") // TokenPointer always consumes a token
        return NullMatch::no;
    if (name == "ExprList"){ // can null match if all children null match
        for (auto &expr : std::dynamic_pointer_cast<ExprList>(node)->exprs)
            if (nullMatching(expr) == NullMatch::no)
                return NullMatch::no;
        for (auto &expr : std::dynamic_pointer_cast<ExprList>(node)->exprs)
            if (nullMatching(expr) == NullMatch::indeterminate)
                return NullMatch::indeterminate;
        
        return NullMatch::yes;
    }
    if (name == "ZeroOrMore" || name == "ZeroOrOne" || name == "AndPredicate" || name == "NotPredicate")
        return NullMatch::yes;
    if (name == "OneOrMore")
        return nullMatching(std::dynamic_pointer_cast<OneOrMore>(node)->expr);
    if (name == "NamedItem")
        return nullMatching(std::dynamic_pointer_cast<NamedItem>(node)->expr);
    
    throw std::runtime_error(fmt::format("Unexpected type '{}'", name));
    //return NullMatch::indeterminate;
}

NullMatch Introspector::nullMatching(std::string rule_name){
    // can null match if *any* of the statements can
    NullMatch null_matching;
    bool found_indeterminate;
    auto &group = generator->rules[rule_name];
    for (auto &stmt : group.statements){
        null_matching = nullMatching(stmt);
        if (null_matching == NullMatch::yes) return NullMatch::yes;
        if (null_matching == NullMatch::indeterminate) found_indeterminate = true;
    }
    if (found_indeterminate) return NullMatch::indeterminate;
    return NullMatch::no;
}

void Introspector::leftRecursions(std::shared_ptr<ASTNode> node, std::set<std::string> &elements){
    auto name = node->class_name();
    NullMatch null_matching;
    if (name == "Statement"){
        for (auto &expr : std::dynamic_pointer_cast<Statement>(node)->exprs){
            null_matching = nullMatching(expr);
            // no idea if this works???
            leftRecursions(expr, elements);
            if (null_matching == NullMatch::no)
                break;
        }
    }
    if (name == "StatementPointer"){
        elements.insert(std::dynamic_pointer_cast<StatementPointer>(node)->target);
    }
    if (name == "ExprList"){
        for (auto &expr : std::dynamic_pointer_cast<ExprList>(node)->exprs){
            null_matching = nullMatching(expr);
            // no idea if this works???
            leftRecursions(expr, elements);
            if (null_matching == NullMatch::no)
                break;
        }
    }
    if (name == "ZeroOrMore" || name == "OneOrMore" || name == "ZeroOrOne")
        leftRecursions(std::dynamic_pointer_cast<Quanitifier>(node)->expr, elements);
    if (name == "AndPredicate" || name == "NotPredicate")
        leftRecursions(std::dynamic_pointer_cast<Predicate>(node)->expr, elements);
    if (name == "NamedItem")
        leftRecursions(std::dynamic_pointer_cast<NamedItem>(node)->expr, elements);
}

std::set<std::string> Introspector::leftRecursions(std::string name){
    std::set<std::string> elements;
    auto &group = generator->rules[name];
    for (auto &stmt : group.statements){
        leftRecursions(stmt, elements);
    }
    return elements;
}

std::vector<std::vector<std::string>> Introspector::get_cycles(std::string searching_for){
    std::vector<std::string> cycle;
    std::vector<std::vector<std::string>> all_cycles;

    std::vector<std::string> scan_stack = {searching_for};

    while (scan_stack.size() > 0){
        auto loc = scan_stack.back();
        //fmt::print("check -> {}\n", loc);
        if (std::find(cycle.begin(), cycle.end(), loc) != cycle.end()){
            scan_stack.pop_back();
            if (cycle.back() == loc) // unwind
                cycle.pop_back();
            continue;
        }
        
        cycle.push_back(loc);

        //fmt::print("cycle: {}\n", fmt::join(cycle, ", "));

        for (auto &e : shallow_references[loc]){
            if (e == searching_for){ // cycle found
                all_cycles.push_back(cycle);
            }
            if (!utils::contains(scan_stack, e)) // e not in scan stack
                scan_stack.push_back(e);
        }
    }

    return all_cycles;
}

void Introspector::introspect(){
    bool changed = true;
    while (changed){
        changed = false;
        for (auto &[name, group] : generator->rules){
            if (rule_info[name].null_match == Introspection::NullMatch::indeterminate){
                rule_info[name].null_match = nullMatching(name);
                
                if (rule_info[name].null_match != Introspection::NullMatch::indeterminate)
                    changed = true;
            }
        }
    }
    
    // display null match values
    //for (auto &[name, group] : generator->rules)
        //std::cout << fmt::format("{} : null_match? {}\n", name, rule_info[name].null_match);
    
    // find all possibly left recursive references
    for (auto &[name, group] : generator->rules){
        shallow_references[name] = leftRecursions(name);
    }

    // print 
    for (auto &[name, group] : generator->rules){
        //std::cout << name << " : ";
        for (auto &refers_to : shallow_references[name]){
            //std::cout << refers_to << ", ";
        }
        //std::cout << "\n";
    }

    // find all cycles
    // all_cycles_sets;
    //std::cout << fmt::format("\n\nCYCLES: \n");
    for (auto &[name, refs] : shallow_references){
        auto cycles = get_cycles(name);
        if (cycles.size() > 0){
            for (auto &cycle : cycles){
                std::set<std::string> s(cycle.begin(), cycle.end());
                if (!utils::contains(completed_cycle_permutations, s)){
                    all_cycles.push_back(cycle);
                    completed_cycle_permutations.push_back(s);

                    // tell all rules they are left recursive
                    for (auto &r : cycle)
                        rule_info[r].left_recursive = LeftRecursive::yes;
                    
                    //std::cout << fmt::format("{}\n", fmt::join(cycle, " -> "));
                }
            }
        }
    }

    std::set<std::string> apply_recursion_correction;
    // TODO: based on the sets of cycles, determine which nodes need left recursion memoization
    for (size_t i = 0; i < completed_cycle_permutations.size(); i++){
        auto cycle = completed_cycle_permutations[i];
        if (utils::set_intersection(apply_recursion_correction, cycle).empty()){
            auto r = all_cycles[i][0];
            apply_recursion_correction.insert(r);
            rule_info[r].flagged = true;
        }
    }
}

}