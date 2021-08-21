#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
//#include "parsergen/grammar_parser.hpp"
#include "parsergen/parser_formatter.hpp"
#include "parsergen/parser_generator.hpp"

#include <iostream>
#include <fmt/format.h>
#include <chrono>
#include <fstream>
#include <sstream>

using namespace Parsergen;
using namespace AST;

std::string parser_header = R"(
#include "parsergen/grammar_ast.hpp"
using namespace Parsergen::AST;

namespace Parsergen {
)";

std::string parser_footer = "\n}";

int main(){
    GrammarLexer l;
    std::ifstream file;
    file.open("src/parsergen/metagrammar.gram");
    if (file.is_open()){
        std::stringstream strStream;
        strStream << file.rdbuf(); //read the file
        std::string pgram = strStream.str(); //str holds the content of the file
        
        l.setText(pgram);
        l.Lex();
        //std::cout << "Start string: " << l.getText() << std::endl;
        for (auto const &tok : l.tokens){
            std::cout << "type : '" << tok.type << ", value: '" << tok.value << "'\n";
            //fmt::print("type: '{}', value: '{}'\n", tok.type, tok.value);
        }
    }
    else {
        std::cout << "Unable to open file" << std::endl;
    }

    // Parser Generator Test
    #define p Position(), Position()
    std::cout << "alive\n";
    

    #define parser_definition(sections...) std::make_shared<ParserDefinition>(std::vector<std::shared_ptr<Section>>{sections}, p)
    #define config_call(name, value) std::make_shared<ConfigurationCall>(name, value, p)
    #define stmt_group(name, return_type, stmts...) std::make_shared<StatementGroup>(name, return_type, std::vector<std::shared_ptr<Statement>>{stmts}, p)
    #define stmt(return_type, action, exprs...) std::make_shared<Statement>(return_type, std::vector<std::shared_ptr<Expr>>{exprs}, action, p)
    #define tok(target) std::make_shared<TokenPointer>(target, p)
    #define sp(target) std::make_shared<StatementPointer>(target, p)
    #define zero_or_more(expr) std::make_shared<ZeroOrMore>(expr, p)
    #define one_or_more(expr) std::make_shared<OneOrMore>(expr, p)
    #define zero_or_one(expr) std::make_shared<ZeroOrOne>(expr, p)
    #define named(name, expr) std::make_shared<NamedItem>(name, expr, p)
    #define and_p(expr) std::make_shared<AndPredicate>(expr, p)
    #define not_p(expr) std::make_shared<NotPredicate>(expr, p)
    #define expr_list(exprs...) std::make_shared<ExprList>(std::vector<std::shared_ptr<Expr>>{exprs}, p)

    /*
        update definition to use "stmt_group"s
        update definition to include parsing for StatementGroupss
    */

   auto parser_def = parser_definition(
       config_call("header", parser_header),
        config_call("footer", parser_footer),
        config_call("class_name", "GrammarParser"),
        stmt_group("parser_definition", "ParserDefinition", stmt("<>", "new ParserDefinition(s, POSITION)", named("s", zero_or_more(sp("section"))))),
        
        stmt_group("section", "Section",
            stmt("<>", "s", named("s", sp("statement_group"))),
            stmt("<>", "s", named("s", sp("configuration")))),
        
        stmt_group("configuration", "ConfigurationCall", stmt("<>", "new ConfigurationCall(name.value, value.value, POSITION)", tok("AT"), named("name", tok("ID")), tok("EQ"), named("value", tok("STRING")))),
        
        stmt_group("statement_group", "StatementGroup", stmt("<>", "new StatementGroup(name.value, return_type.value, stmts, POSITION)", named("name", tok("ID")), named("return_type", tok("RETURN_TYPE")), named("stmts", zero_or_more(sp("statement"))))),
        
        stmt_group("statement", "Statement", stmt(
            "<>", "new Statement(getTokenValueOr(return_type, \"<>\"), es, getTokenValueOr(a, \"<>\"), POSITION)",
            named("return_type", zero_or_one(tok("RETURN_TYPE"))), tok("COLON"), named("es", zero_or_more(sp("expr"))), named("a", zero_or_one(tok("ACTION"))), tok("TERMINATE")
        )),
        
        stmt_group("expr_list", "ExprList", stmt("<>", "new ExprList(es, POSITION)", named("es", zero_or_more(sp("expr"))))),
        
        stmt_group("expr", "Expr",
            stmt("NamedItem", "new NamedItem(name.value, v, POSITION)", named("name", tok("ID")), tok("EQ"), named("v", sp("or_op"))),
            stmt("<>", "v", named("v", sp("or_op")))),
        
        stmt_group("or_op", "Expr", stmt("<>", "p1", sp("star_op"))),
        
        stmt_group("star_op", "Expr",
            stmt("ZeroOrMore", "new ZeroOrMore(v, POSITION)", named("v", sp("plus_op")), tok("STAR")),
            stmt("<>", "v", named("v", sp("plus_op")))),
        
        stmt_group("plus_op", "Expr",
            stmt("OneOrMore", "new OneOrMore(v, POSITION)", named("v", sp("qmark_op")), tok("PLUS")),
            stmt("<>", "v", named("v", sp("qmark_op")))),
        
        stmt_group("qmark_op", "Expr",
            stmt("ZeroOrOne", "new ZeroOrOne(v, POSITION)", named("v", sp("term")), tok("QMARK")),
            stmt("<>", "v", named("v", sp("term")))),

        stmt_group("term", "Expr",
            stmt("AndPredicate", "new AndPredicate(f, POSITION)", tok("AND"), named("f", sp("factor"))),
            stmt("NotPredicate", "new NotPredicate(f, POSITION)", tok("NOT"), named("f", sp("factor"))),
            stmt("<>", "f", named("f", sp("factor")))),

        stmt_group("factor", "Expr",
            stmt("<>", "i", named("i", sp("item"))),
            stmt("<>", "es", tok("LPAREN"), named("es", sp("expr_list")), tok("RPAREN"))),

        stmt_group("item", "Expr",
            stmt("StatementPointer", "new StatementPointer(i.value, POSITION)", named("i", tok("ID"))),
            stmt("TokenPointer", "new TokenPointer(i.value, POSITION)", named("i", tok("TOKEN"))),
            stmt("ConstantString", "new ConstantString(i.value, POSITION)", named("i", tok("STRING")))),

    );

    Generator g(
        parser_def
    );

    std::cout << parser_format(parser_def);
    
    auto start = std::chrono::steady_clock::now();
    g.generate();
    auto end = std::chrono::steady_clock::now();
    auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Generated Parser in " << diff.count() << "ms" << std::endl;
    
    std::ofstream f("include/parsergen/grammar_parser.hpp");
    f << g.result.str();
    f.close();
    
    return 0;
}