#include <string>
#include <vector>
#include <parsergen/lexer.hpp>
#pragma once

#define S_RULE(name, pattern) (\
    LexRule(\
        name,\
        std::vector<std::regex>{\
            std::regex(pattern, REGEX_SYNTAX_TYPE)\
        }\
    )\
)

#define F_RULE(name, pattern, func) (\
    LexRule(\
        name,\
        std::vector<std::regex>{\
            std::regex(pattern, REGEX_SYNTAX_TYPE)\
        },\
        func\
    )\
)

namespace Parsergen {

namespace AST {

class ASTNode {
public:
    virtual std::string class_name() { return "ASTNode"; }
    Position start() const {
        return m_start;
    }

    Position end() const {
        return m_end;
    }

    void set_start(const Position &start){
        m_start = start;
    }

    void set_end(const Position &end){
        m_end = end;
    }

protected:
    ASTNode(Position start, Position end)
        : m_start(start)
        , m_end(end)
    {}
private:
    Position m_start;
    Position m_end;
    // TODO: add filename property
};

class Terminator : public ASTNode {
public:
    using ASTNode::ASTNode;
};

class Pointer : public Terminator {
public:
    std::string target;
    Pointer(std::string target, Position start, Position end)
        : target(target)
        , Terminator(start, end)
    {}
};

class StatementPointer : Pointer {
    using Pointer::Pointer;
};

class TokenPointer : Pointer {
    using Pointer::Pointer;
};

class ConstantString : Terminator {
public:
    std::string value;
    ConstantString(std::string value, Position start, Position end)
        : value(value)
        , Terminator(start, end)
    {}
};

class Expr : ASTNode {
public:
    using ASTNode::ASTNode;
};

class ExprList : ASTNode {
public:
    std::vector<Expr> exprs;
    ExprList(std::vector<Expr> exprs, Position start, Position end)
        : exprs(exprs)
        , ASTNode(start, end)
    {}
};

class Quanitifier : Expr {
public:
    Expr expr;
    Quanitifier(Expr expr, Position start, Position end)
        : expr(expr)
        , Expr(start, end)
    {}
};

class ZeroOrMore : Quanitifier {
    using Quanitifier::Quanitifier;
};
class OneOrMore : Quanitifier {
    using Quanitifier::Quanitifier;
};
class ZeroOrOne : Quanitifier {
    using Quanitifier::Quanitifier;
};

class OrOp : Expr {
public:
    std::vector<Expr> exprs;
    OrOp(std::vector<Expr> exprs, Position start, Position end)
        : exprs(exprs)
        , Expr(start, end)
    {}
};

class NamedItem : Expr {
public:
    std::string name;
    Expr expr;
    NamedItem(std::string name, Expr expr, Position start, Position end)
        : name(name)
        , expr(expr)
        , Expr(start, end)
    {}
};

class Predicate : Expr {
public:
    Expr expr;
    Predicate(Expr expr, Position start, Position end)
        : expr(expr)
        , Expr(start, end)
    {}
};

class AndPredicate : Predicate {
    using Predicate::Predicate;
};
class NotPredicate : Predicate {
    using Predicate::Predicate;
};

class Section : ASTNode {
    using ASTNode::ASTNode;
};

class Statement : Section {
public:
    std::string name;
    std::vector<Expr> exprs;
    std::string action;
    Statement(std::string name, std::vector<Expr> exprs, std::string action, Position start, Position end)
        : name(name)
        , exprs(exprs)
        , action(action)
        , Section(start, end)
    {}

};

class ConfigurationCall : Section {
public:
    std::string name;
    std::string value;
    ConfigurationCall(std::string name, std::string value, Position start, Position end)
        : name(name)
        , value(value)
        , Section(start, end)
    {}
};

class ParserDefinition : ASTNode {
public:
    std::vector<Section> sections;
    ParserDefinition(std::vector<Section> sections, Position start, Position end)
        : sections(sections)
        , ASTNode(start, end)
    {}
};

}

class GrammarLexer : public Lexer {
public:
    GrammarLexer();
};

}