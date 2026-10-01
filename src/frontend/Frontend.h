#ifndef FRONTEND_FRONTEND_H
#define FRONTEND_FRONTEND_H

#include <cstdio>

#include "frontend/Ast.h"

// Reads and parses a whole program. Throws CompileError on a lexical or syntax error.
ast::Program parseProgram(std::FILE* input);

#endif
