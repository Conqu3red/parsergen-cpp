#include "parsergen/lexer.hpp"
#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"
#include "fmt/core.h"

#include <string>
#include <vector>
#include <memory>
#include <tuple>
#include <iostream>

#pragma once

using namespace Parsergen::AST;

namespace Parsergen {

/*
std::string parser_format(std::shared_ptr<> item){
    std::string rv = "";
    return rv;
}
*/

std::string parser_format(std::shared_ptr<ASTNode> expr);

std::string parser_format_ParserDefinition(std::shared_ptr<ParserDefinition> item){
    std::string rv = "";
    for (auto &section : item->sections){
        if (section->is("StatementGroup")){
            auto a = std::dynamic_pointer_cast<StatementGroup>(section);
            rv += parser_format(a);
        }
        else {
            rv += parser_format(std::dynamic_pointer_cast<ConfigurationCall>(section));
        }
        rv += "\n";
    }
    return rv;
}

std::string parser_format_StatementGroup(std::shared_ptr<StatementGroup> item){
    std::string rv = fmt::format("{}[{}]\n", item->name, item->return_type);
    for (auto &statement : item->statements){
        if (statement->return_type == item->return_type || statement->return_type == "<>")
            rv += "    :  " + parser_format(statement);
        else
            rv += "  [" + statement->return_type + "]  :  " + parser_format(statement);
        rv += "\n";
    }
    return rv;
}

std::string parser_format_ConfigurationCall(std::shared_ptr<ConfigurationCall> item){
    return fmt::format("@{} = '{}'\n", item->name, item->value);
}

std::string parser_format_Statement(std::shared_ptr<Statement> item){
    std::string rv = "";
    for (auto &expr : item->exprs){
        rv += parser_format(expr) + " ";
    }
    rv += "{ " + item->action + " };";
    return rv;
}

std::string parser_format_TokenPointer(std::shared_ptr<TokenPointer> item){
    return fmt::format("{}", item->target);
}

std::string parser_format_StatementPointer(std::shared_ptr<StatementPointer> item){
    return fmt::format("{}", item->target);
}

std::string parser_format_ConstantString(std::shared_ptr<ConstantString> item){
    return fmt::format("'{}'", item->value);
}

std::string parser_format_AndPredicate(std::shared_ptr<AndPredicate> item){
    return fmt::format("&{}", parser_format(item->expr));
}

std::string parser_format_NotPredicate(std::shared_ptr<NotPredicate> item){
    return fmt::format("!{}", parser_format(item->expr));
}

std::string parser_format_ZeroOrMore(std::shared_ptr<ZeroOrMore> item){
    return fmt::format("{}*", parser_format(item->expr));
}

std::string parser_format_OneOrMore(std::shared_ptr<OneOrMore> item){
    return fmt::format("{}+", parser_format(item->expr));
}

std::string parser_format_ZeroOrOne(std::shared_ptr<ZeroOrOne> item){
    return fmt::format("{}?", parser_format(item->expr));
}

std::string parser_format_ExprList(std::shared_ptr<ExprList> item){
    std::string rv = "(";
    for (auto &expr : item->exprs){
        rv += parser_format(expr) + " ";
    }
    rv.pop_back();
    return rv + ")";
}

std::string parser_format_NamedItem(std::shared_ptr<NamedItem> item){
    return fmt::format("{}={}", item->name, parser_format(item->expr));
}

#define PFORMAT_DISPATCH(type) if (expr->is(#type)) return parser_format_ ##type(std::dynamic_pointer_cast<type>(expr))
std::string parser_format(std::shared_ptr<ASTNode> expr){
    PFORMAT_DISPATCH(ParserDefinition);
    else PFORMAT_DISPATCH(StatementGroup);
    else PFORMAT_DISPATCH(ConfigurationCall);
    else PFORMAT_DISPATCH(Statement);
    else PFORMAT_DISPATCH(TokenPointer);
    else PFORMAT_DISPATCH(StatementPointer);
    else PFORMAT_DISPATCH(ConstantString);
    else PFORMAT_DISPATCH(AndPredicate);
    else PFORMAT_DISPATCH(NotPredicate);
    else PFORMAT_DISPATCH(ZeroOrMore);
    else PFORMAT_DISPATCH(OneOrMore);
    else PFORMAT_DISPATCH(ZeroOrOne);
    else PFORMAT_DISPATCH(ExprList);
    else PFORMAT_DISPATCH(NamedItem);
    else
        throw std::runtime_error(fmt::format("Invalid type '{}'", expr->class_name()));
}


}