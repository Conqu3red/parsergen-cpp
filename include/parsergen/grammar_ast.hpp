#pragma once
#include <string>
#include <list>
#include <vector>
#include <memory>
#include <initializer_list>
#include <parsergen/lexer.hpp>

namespace Parsergen {

namespace AST {

class ASTNode {
public:
    virtual std::string class_name() { return "ASTNode"; }
    bool is(std::string name) { return class_name() == name; }
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

    ASTNode(Position start, Position end)
        : m_start(start)
        , m_end(end)
    {}
protected:
    virtual ~ASTNode(){}
private:
    Position m_start;
    Position m_end;
    // TODO: add filename property
};

class Expr : public ASTNode {
public:
    std::string class_name() override { return "Expr"; }
    using ASTNode::ASTNode;
};

class Terminator : public Expr {
public:
    std::string class_name() override { return "Terminator"; }
    using Expr::Expr;
};

class Pointer : public Terminator {
public:
    virtual std::string class_name() { return "Pointer"; }
    std::string target;
    Pointer(std::string target, Position start, Position end)
        : target(target)
        , Terminator(start, end)
    {}
};

class StatementPointer : public Pointer {
public:
    virtual std::string class_name() { return "StatementPointer"; }
    using Pointer::Pointer;
};

class TokenPointer : public Pointer {
public:
    virtual std::string class_name() { return "TokenPointer"; }
    using Pointer::Pointer;
};

class ConstantString : public Terminator {
public:
    virtual std::string class_name() { return "ConstantString"; }
    std::string value;
    ConstantString(std::string value, Position start, Position end)
        : value(value)
        , Terminator(start, end)
    {}
};

class ExprList : public Expr {
public:
    virtual std::string class_name() { return "ExprList"; }
    std::string handle_name = "";
    std::vector<std::shared_ptr<Expr>> exprs;
    
    ExprList(std::vector<std::shared_ptr<Expr>> exprs, Position start, Position end)
        : exprs(exprs)
        , Expr(start, end)
    {}
};

class Quanitifier : public Expr {
public:
    virtual std::string class_name() { return "Quantifier"; }
    std::string handle_name = "";
    std::shared_ptr<Expr> expr;
    Quanitifier(std::shared_ptr<Expr> expr, Position start, Position end)
        : expr(expr)
        , Expr(start, end)
    {}
};

class ZeroOrMore : public Quanitifier {
public:
    virtual std::string class_name() { return "ZeroOrMore"; }
    using Quanitifier::Quanitifier;
};
class OneOrMore : public Quanitifier {
public:
    virtual std::string class_name() { return "OneOrMore"; }
    using Quanitifier::Quanitifier;
};
class ZeroOrOne : public Quanitifier {
public:
    virtual std::string class_name() { return "ZeroOrOne"; }
    using Quanitifier::Quanitifier;
};

class OrOp : public Expr {
public:
    virtual std::string class_name() { return "OrOp"; }
    std::vector<std::shared_ptr<Expr>> exprs;
    OrOp(std::vector<std::shared_ptr<Expr>> exprs, Position start, Position end)
        : exprs(exprs)
        , Expr(start, end)
    {}
};

class NamedItem : public Expr {
public:
    virtual std::string class_name() { return "NamedItem"; }
    std::string name;
    std::shared_ptr<Expr> expr;
    NamedItem(std::string name, std::shared_ptr<Expr> expr, Position start, Position end)
        : name(name)
        , expr(expr)
        , Expr(start, end)
    {}
};

class Predicate : public Expr {
public:
    virtual std::string class_name() { return "Predicate"; }
    std::shared_ptr<Expr> expr;
    Predicate(std::shared_ptr<Expr> expr, Position start, Position end)
        : expr(expr)
        , Expr(start, end)
    {}
};

class AndPredicate : public Predicate {
public:
    virtual std::string class_name() { return "AndPredicate"; }
    using Predicate::Predicate;
};
class NotPredicate : public Predicate {
public:
    virtual std::string class_name() { return "NotPredicate"; }
    using Predicate::Predicate;
};

class Section : public ASTNode {
public:
    virtual std::string class_name() { return "Section"; }
    using ASTNode::ASTNode;
};

class Statement : public ASTNode {
public:
    virtual std::string class_name() { return "Statement"; }
    std::vector<std::shared_ptr<Expr>> exprs;
    std::string action;
    Statement(std::vector<std::shared_ptr<Expr>> exprs, std::string action, Position start, Position end)
        : exprs(exprs)
        , action(action)
        , ASTNode(start, end)
    {}

};

class StatementGroup : public Section {
public:
    std::string class_name() { return "StatementGroup"; }
    std::string name = "";
    std::string return_type = "";
    std::vector<std::shared_ptr<Statement>> statements;
    StatementGroup() : Section(Position(), Position()) {}
    StatementGroup(std::string name, std::string return_type, std::vector<std::shared_ptr<Statement>> statements, Position start, Position end)
        : name(name)
        , return_type(return_type)
        , statements(statements)
        , Section(start, end)
    {}

};

class ConfigurationCall : public Section {
public:
    virtual std::string class_name() { return "ConfigurationCall"; }
    std::string name;
    std::string value;
    ConfigurationCall(std::string name, std::string value, Position start, Position end)
        : name(name)
        , value(value)
        , Section(start, end)
    {}
};

class ParserDefinition : public ASTNode {
public:
    virtual std::string class_name() { return "ParserDefinition"; }
    std::vector<std::shared_ptr<Section>> sections;
    ParserDefinition(std::vector<std::shared_ptr<Section>> sections, Position start, Position end)
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