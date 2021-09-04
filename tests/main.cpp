#include "parsergen/parser_generator.hpp"
#include "parsergen/introspection.hpp"
#include "parsergen/utils.hpp"
#include "fmt/core.h"
#include <iostream>
#include <vector>
#include <stdexcept>

#include "lest.hpp"

using namespace Parsergen;
using namespace Parsergen::Introspection;

Generator make_generator(std::string grammar){
    std::unique_ptr<Lexer> l = std::make_unique<GrammarLexer>();
    l->setText(grammar);
    l->Lex();
    GrammarParser parser(std::make_unique<TokenStream>(std::move(l)));
    auto p = parser.parser_definition();
    if (p.has_value()){
        Generator g(p.value());
        return g;
    }
    else {
        auto e = parser.error();
        if (e.has_value())
            throw e.value();
        throw ParseError("Parse Error", Position(), Position(), "");
    }
}

#define INTROSPECT(grammar)             \
    auto g = make_generator(grammar);   \
    g.process_sections();               \
    Introspector i = Introspector(&g);  \
    i.introspect()

const lest::test specification[] =
{
    CASE( "Simple Left Recursion" ){
        INTROSPECT("e[n] : e X;");
        EXPECT(i.rule_info["e"].left_recursive == LeftRecursive::yes);
        EXPECT(utils::contains(i.all_cycles, {"e"}));
    },

    CASE( "Hidden Left Recursion" ){
        INTROSPECT("e[n] : X? X* e X;");
        EXPECT(i.rule_info["e"].left_recursive == LeftRecursive::yes);
        EXPECT(utils::contains(i.all_cycles, {"e"}));
    },

    CASE( "Indirect Left Recursion" ){
        INTROSPECT(R"(
            top[n] : o1 Y;
            o1[n] : o2;
            o2[n] : o3;
            o3[n] : top;
                  : X;
        )");
        EXPECT(i.rule_info["top"].left_recursive == LeftRecursive::yes);
        EXPECT(i.rule_info["o1"].left_recursive == LeftRecursive::yes);
        EXPECT(i.rule_info["o2"].left_recursive == LeftRecursive::yes);
        EXPECT(i.rule_info["o3"].left_recursive == LeftRecursive::yes);
        EXPECT(utils::contains(i.completed_cycle_permutations, {"top", "o1", "o2", "o3"}));
    },

    CASE( "Indirect Left Recursion" ){
        INTROSPECT(R"(
            e[n] : f e '+n';
                 : 'n';
            f[n] : A B C;
                 : D*;
        )");
        EXPECT(i.rule_info["e"].null_match == NullMatch::indeterminate);
        EXPECT(i.rule_info["e"].left_recursive == LeftRecursive::yes);
        
        EXPECT(i.rule_info["f"].null_match == NullMatch::yes);
        EXPECT(i.rule_info["f"].left_recursive == LeftRecursive::no);
        
        EXPECT(utils::contains(i.completed_cycle_permutations, {"e"}));
    },

    CASE( "Mutual left recursion" ){
        INTROSPECT(R"(
            m[n] : l EOF;
            l[n] : p '.x';
                : 'x';
            p[n] : p '(n)';
                : l;
        )");
        EXPECT(i.completed_cycle_permutations.size() == 2);
        EXPECT(utils::contains(i.completed_cycle_permutations, {"p"}));
        EXPECT(utils::contains(i.completed_cycle_permutations, {"l", "p"}));
    },

    CASE( "Interlocking cycles" ){
        INTROSPECT(R"(
            s[n] : e EOF;

            e[n] : f 'n';
                : 'n';

            f[n] : e '+' i*;
                : g '-';

            g[n] : h 'm';
                : e;

            h[n] : g 'l';
            i[n] : '(' a+ ')';
            a[n] : 'a';
        )");
        EXPECT(i.completed_cycle_permutations.size() == 3);
        EXPECT(utils::contains(i.completed_cycle_permutations, {"e", "f"}));
        EXPECT(utils::contains(i.completed_cycle_permutations, {"g", "h"}));
        EXPECT(utils::contains(i.completed_cycle_permutations, {"e", "f", "g"}));
        
    },
};

int main( int argc, char * argv[] ){
    return lest::run( specification, argc, argv );
}