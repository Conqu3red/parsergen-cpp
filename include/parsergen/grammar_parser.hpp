#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"
#include "parsergen/parser_utils.hpp"

#pragma once

namespace Parsergen {
using namespace AST;

/*
@header = '
from .grammar_utils import *

'
@class_name = 'GrammarParser'

parser_definition[ParserDefinition]  : s=section* EOF { s, POSITION }; 

section
    :  s=statement { s };
    :  s=configuration { s };

configuration  :  AT name=ID EQ value=STRING { ConfigurationCall(name.value, value.value) };
statement  :  n=(ID? COLON)? es=expr* a=ACTION? TERMINATE { Statement(
    n[0].value if not isinstance(n, Filler) and not isinstance(n[0], Filler) else "<>", 
    es, 
    action=a.value if not isinstance(a, Filler) else None
) };

expr_list  :  es=expr* { ExprList(es) };
expr  :  name=(ID EQ)? v=or_op { NamedItem(name[0].value, v) if not isinstance(name, Filler) else v };
or_op  :  v=star_op others=(OR star_op)* { OrOp(exprs=[v]+[o[1] for o in others]) if len(others) > 0 else v };
star_op  :  v=plus_op s=STAR? { ZeroOrMore(v) if not isinstance(s, Filler) else v };
plus_op  :  v=qmark_op s=PLUS? { OneOrMore(v) if not isinstance(s, Filler) else v };
qmark_op  :  v=term s=QMARK? { ZeroOrOne(v) if not isinstance(s, Filler) else v };

term
    :  AND f=factor { AndPredicate(f) };
    :  NOT f=factor { NotPredicate(f) };
    :  f=factor { f };

factor
    :  i=item { i };
    :  LPAREN es=expr_list RPAREN { es };

item
    :  i=ID { StatementPointer(i.value) };
    :  i=TOKEN { TokenPointer(i.value) };
    :  i=STRING { ConstantString(i.value) };
*/

class GrammarParser : public Parser {
    using Parser::Parser;

    std::optional<std::shared_ptr<ParserDefinition>> start(){
        Position start;
        Position end;
        int pos = mark();
        int t1 = 0;
        while (t1 == 0){
            t1++; // 1 loop
            start = current_pos();
            auto _p1 = node();
            if (!_p1.has_value()){
                fail();
                break;
            }
            auto &p1 = _p1.value();
            
            end = current_pos();
            // match
            return std::make_shared<ParserDefinition>(p1, POSITION);
        }
        set_pos(pos);
        return {};
    }

};

}