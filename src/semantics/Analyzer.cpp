#include "semantics/Analyzer.h"

#include <algorithm>
#include <climits>

namespace {

std::string quoted(const std::string& name) {
    return "'" + name + "'";
}

}

std::vector<CompileError> Analyzer::analyze(ast::Program& program) {
    scopes_.assign(1, {});

    for (const ast::Declaration& declaration : program.declarations) {
        declare(declaration);
    }

    analyze(program.commands);

    return std::move(errors_);
}

void Analyzer::declare(const ast::Declaration& declaration) {
    auto& globals = scopes_.front();

    if (globals.count(declaration.name) != 0) {
        error(declaration.line, quoted(declaration.name) + " is already declared");
        return;
    }

    if (!declaration.isArray) {
        globals[declaration.name] = newSymbol(Symbol::Kind::VARIABLE, declaration.name);
        return;
    }

    long long size = 0;
    if (declaration.firstIndex > declaration.lastIndex) {
        error(declaration.line, "array " + quoted(declaration.name) + " has an invalid range [" +
              std::to_string(declaration.firstIndex) + ":" + std::to_string(declaration.lastIndex) + "]");
    }
    else if (__builtin_sub_overflow(declaration.lastIndex, declaration.firstIndex, &size) || size == LLONG_MAX) {
        error(declaration.line, "array " + quoted(declaration.name) + " is too large");
    }

    Symbol* array = newSymbol(Symbol::Kind::ARRAY, declaration.name);
    array->firstIndex = declaration.firstIndex;
    array->lastIndex = declaration.lastIndex;
    globals[declaration.name] = array;
}

Symbol* Analyzer::newSymbol(Symbol::Kind kind, const std::string& name) {
    symbols_.push_back(std::make_unique<Symbol>());
    Symbol* symbol = symbols_.back().get();
    symbol->kind = kind;
    symbol->name = name;
    return symbol;
}

// Finds the innermost declaration of `name`. Each undeclared name is reported once.
Symbol* Analyzer::lookup(const std::string& name, int line) {
    for (auto scope = scopes_.rbegin(); scope != scopes_.rend(); ++scope) {
        auto it = scope->find(name);
        if (it != scope->end()) {
            return it->second;
        }
    }

    if (reportedUndeclared_.insert(name).second) {
        error(line, quoted(name) + " is not declared");
    }

    return nullptr;
}

void Analyzer::analyze(ast::CommandList& commands) {
    for (ast::Command& command : commands) {
        std::visit([this](auto& node) { analyze(node); }, command.node);
    }
}

void Analyzer::analyze(ast::Assign& assign) {
    bool resolved = resolve(assign.target);
    analyze(assign.expression);

    if (resolved) {
        assignTo(assign.target);
    }
}

void Analyzer::analyze(ast::If& statement) {
    analyze(statement.condition);
    analyze(statement.thenBranch);
    analyze(statement.elseBranch);
}

void Analyzer::analyze(ast::While& loop) {
    loopDepth_++;
    analyze(loop.condition);
    analyze(loop.body);
    loopDepth_--;
}

void Analyzer::analyze(ast::Repeat& loop) {
    loopDepth_++;
    analyze(loop.body);
    analyze(loop.condition);
    loopDepth_--;
}

void Analyzer::analyze(ast::For& loop) {
    // The range is evaluated before the iterator comes into scope.
    analyze(loop.from);
    analyze(loop.to);

    loop.iterator = newSymbol(Symbol::Kind::ITERATOR, loop.iteratorName);
    loop.iterator->initialized = true;
    loop.bound = newSymbol(Symbol::Kind::LOOP_BOUND, loop.iteratorName + "'bound");
    loop.bound->initialized = true;

    loopDepth_++;

    // Every iteration increments the iterator and compares it with the bound. A constant
    // bound is usually compiled into the code instead of being kept in memory.
    markUsed(loop.iterator);
    markUsed(loop.iterator);
    if (!loop.to.isConstant) {
        markUsed(loop.bound);
    }

    scopes_.push_back({{loop.iteratorName, loop.iterator}});
    analyze(loop.body);
    scopes_.pop_back();

    loopDepth_--;
}

void Analyzer::analyze(ast::Read& read) {
    if (resolve(read.target)) {
        assignTo(read.target);
    }
}

void Analyzer::analyze(ast::Write& write) {
    analyze(write.value);
}

void Analyzer::analyze(ast::Expression& expression) {
    analyze(expression.left);

    if (expression.op) {
        analyze(expression.right);
    }
}

void Analyzer::analyze(ast::Condition& condition) {
    analyze(condition.left);
    analyze(condition.right);
}

void Analyzer::analyze(ast::Value& value) {
    if (value.isConstant) {
        return;
    }

    ast::Identifier& identifier = value.identifier;

    if (resolve(identifier) && identifier.symbol->isScalar()) {
        requireInitialized(*identifier.symbol, identifier.name, identifier.line);
    }
}

// Binds the identifier (and its index variable) to symbols. Returns false after reporting an error.
bool Analyzer::resolve(ast::Identifier& identifier) {
    using Index = ast::Identifier::Index;

    Symbol* symbol = lookup(identifier.name, identifier.line);
    if (symbol == nullptr) {
        return false;
    }

    if (identifier.index == Index::NONE) {
        if (symbol->isArray()) {
            error(identifier.line, quoted(identifier.name) + " is an array and must be indexed");
            return false;
        }
    }
    else if (!symbol->isArray()) {
        error(identifier.line, quoted(identifier.name) + " is not an array");
        return false;
    }
    else if (identifier.index == Index::CONSTANT) {
        long long index = identifier.constantIndex;
        bool validRange = symbol->firstIndex <= symbol->lastIndex;

        if (validRange && (index < symbol->firstIndex || index > symbol->lastIndex)) {
            error(identifier.line, "index " + std::to_string(index) + " is out of bounds for array " +
                  quoted(identifier.name) + " [" + std::to_string(symbol->firstIndex) + ":" +
                  std::to_string(symbol->lastIndex) + "]");
            return false;
        }
    }
    else {
        Symbol* index = lookup(identifier.indexName, identifier.line);
        if (index == nullptr) {
            return false;
        }

        if (index->isArray()) {
            error(identifier.line, quoted(identifier.indexName) + " is an array and must be indexed");
            return false;
        }

        requireInitialized(*index, identifier.indexName, identifier.line);
        identifier.indexSymbol = index;
        markUsed(index);
    }

    identifier.symbol = symbol;
    markUsed(symbol);
    return true;
}

void Analyzer::assignTo(ast::Identifier& target) {
    Symbol& symbol = *target.symbol;

    if (symbol.kind == Symbol::Kind::ITERATOR) {
        error(target.line, "cannot modify loop iterator " + quoted(target.name));
    }

    if (symbol.isScalar()) {
        symbol.initialized = true;
    }
}

// Reports a use before initialization once per symbol.
void Analyzer::requireInitialized(Symbol& symbol, const std::string& name, int line) {
    if (!symbol.initialized) {
        error(line, quoted(name) + " is used before being initialized");
        symbol.initialized = true;
    }
}

// Accesses inside loops count more, as they are likely to run many times.
void Analyzer::markUsed(Symbol* symbol) {
    const int maxWeightedDepth = 15;
    symbol->weight += 1ULL << (3 * std::min(loopDepth_, maxWeightedDepth));
}

void Analyzer::error(int line, const std::string& message) {
    errors_.emplace_back(line, message);
}
