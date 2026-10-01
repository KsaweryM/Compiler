#ifndef SEMANTICS_ANALYZER_H
#define SEMANTICS_ANALYZER_H

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "common/Diagnostics.h"
#include "frontend/Ast.h"
#include "semantics/Symbol.h"

// Checks the program and binds every identifier in the tree to its Symbol.
//
// Rules enforced:
//  - names are declared once, arrays have a non-empty range,
//  - arrays are always indexed and scalars never are,
//  - constant indices lie within the array range,
//  - a scalar is used only after an earlier (textually) assignment or READ,
//  - a FOR iterator is visible only inside its loop and cannot be modified there.
//
// A FOR iterator may reuse the name of a variable or of an outer iterator; it then
// hides that name until the end of the loop.
class Analyzer {
public:
    std::vector<CompileError> analyze(ast::Program& program);

    // Every symbol created during analysis, including iterators and loop bounds.
    const std::vector<std::unique_ptr<Symbol>>& symbols() const { return symbols_; }

private:
    void declare(const ast::Declaration& declaration);
    Symbol* newSymbol(Symbol::Kind kind, const std::string& name);
    Symbol* lookup(const std::string& name, int line);

    void analyze(ast::CommandList& commands);
    void analyze(ast::Assign& assign);
    void analyze(ast::If& statement);
    void analyze(ast::While& loop);
    void analyze(ast::Repeat& loop);
    void analyze(ast::For& loop);
    void analyze(ast::Read& read);
    void analyze(ast::Write& write);

    void analyze(ast::Expression& expression);
    void analyze(ast::Condition& condition);
    void analyze(ast::Value& value);

    bool resolve(ast::Identifier& identifier);
    void assignTo(ast::Identifier& target);
    void requireInitialized(Symbol& symbol, const std::string& name, int line);
    void markUsed(Symbol* symbol);
    void error(int line, const std::string& message);

    std::vector<std::unique_ptr<Symbol>> symbols_;
    std::vector<std::map<std::string, Symbol*>> scopes_;
    std::set<std::string> reportedUndeclared_;
    std::vector<CompileError> errors_;
    int loopDepth_ = 0;
};

#endif
