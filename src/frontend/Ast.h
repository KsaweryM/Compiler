#ifndef FRONTEND_AST_H
#define FRONTEND_AST_H

#include <optional>
#include <string>
#include <variant>
#include <vector>

struct Symbol;

// Abstract syntax tree built by the parser. The semantic analyzer later binds every
// identifier to its Symbol; the code generator reads the tree without modifying it.
namespace ast {

enum class Operator { PLUS, MINUS, TIMES, DIV, MOD };

// LE and GE are the strict relations (<, >), as in the source language.
enum class Relation { EQ, NEQ, LE, GE, LEQ, GEQ };

// A variable (`x`) or an array element (`t[5]`, `t[i]`).
struct Identifier {
    enum class Index { NONE, CONSTANT, VARIABLE };

    std::string name;
    int line = 0;
    Index index = Index::NONE;
    long long constantIndex = 0;
    std::string indexName;

    // Bound by the semantic analyzer.
    Symbol* symbol = nullptr;
    Symbol* indexSymbol = nullptr;

    static Identifier scalar(std::string name, int line) {
        Identifier identifier;
        identifier.name = std::move(name);
        identifier.line = line;
        return identifier;
    }

    static Identifier element(std::string name, long long index, int line) {
        Identifier identifier = scalar(std::move(name), line);
        identifier.index = Index::CONSTANT;
        identifier.constantIndex = index;
        return identifier;
    }

    static Identifier element(std::string name, std::string indexName, int line) {
        Identifier identifier = scalar(std::move(name), line);
        identifier.index = Index::VARIABLE;
        identifier.indexName = std::move(indexName);
        return identifier;
    }
};

// A number or an identifier.
struct Value {
    bool isConstant = false;
    long long constant = 0;
    Identifier identifier;

    static Value number(long long constant) {
        Value value;
        value.isConstant = true;
        value.constant = constant;
        return value;
    }

    static Value variable(Identifier identifier) {
        Value value;
        value.identifier = std::move(identifier);
        return value;
    }
};

// `left` alone, or `left op right`.
struct Expression {
    Value left;
    std::optional<Operator> op;
    Value right;
};

struct Condition {
    Value left;
    Relation relation = Relation::EQ;
    Value right;
};

struct Command;
using CommandList = std::vector<Command>;

struct Assign {
    Identifier target;
    Expression expression;
};

struct If {
    Condition condition;
    CommandList thenBranch;
    CommandList elseBranch;
};

struct While {
    Condition condition;
    CommandList body;
};

struct Repeat {
    CommandList body;
    Condition condition;
};

struct For {
    std::string iteratorName;
    Value from;
    Value to;
    bool descending = false;
    CommandList body;

    // Created by the semantic analyzer: the iterator and a hidden cell for the final value.
    Symbol* iterator = nullptr;
    Symbol* bound = nullptr;
};

struct Read {
    Identifier target;
};

struct Write {
    Value value;
};

struct Command {
    int line = 0;
    std::variant<Assign, If, While, Repeat, For, Read, Write> node;
};

struct Declaration {
    std::string name;
    int line = 0;
    bool isArray = false;
    long long firstIndex = 0;
    long long lastIndex = 0;

    static Declaration scalar(std::string name, int line) {
        return {std::move(name), line, false, 0, 0};
    }

    static Declaration array(std::string name, long long firstIndex, long long lastIndex, int line) {
        return {std::move(name), line, true, firstIndex, lastIndex};
    }
};

struct Program {
    std::vector<Declaration> declarations;
    CommandList commands;
};

}

#endif
