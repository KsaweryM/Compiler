// Compiler driver: kompilator <input file> <output file>
//
// Pipeline: parse -> semantic analysis -> memory layout -> code generation.

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "backend/CodeGenerator.h"
#include "common/Diagnostics.h"
#include "frontend/Frontend.h"
#include "semantics/Analyzer.h"
#include "semantics/MemoryLayout.h"

namespace {

void report(const std::string& file, const CompileError& error) {
    std::cerr << file << ":" << error.line() << ": error: " << error.what() << std::endl;
}

}

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "usage: " << argv[0] << " <input file> <output file>" << std::endl;
        return 2;
    }

    const std::string inputPath = argv[1];
    const std::string outputPath = argv[2];

    std::FILE* input = std::fopen(inputPath.c_str(), "r");
    if (input == nullptr) {
        std::cerr << inputPath << ": error: cannot open file" << std::endl;
        return 1;
    }

    ast::Program program;
    try {
        program = parseProgram(input);
    }
    catch (const CompileError& error) {
        std::fclose(input);
        report(inputPath, error);
        return 1;
    }
    std::fclose(input);

    Analyzer analyzer;
    const std::vector<CompileError> errors = analyzer.analyze(program);
    if (!errors.empty()) {
        for (const CompileError& error : errors) {
            report(inputPath, error);
        }
        return 1;
    }

    layoutMemory(analyzer.symbols());

    try {
        const Code code = CodeGenerator().generate(program);

        std::ofstream output(outputPath);
        output << code;

        if (!output) {
            std::cerr << outputPath << ": error: cannot write file" << std::endl;
            return 1;
        }
    }
    catch (const std::exception& error) {
        std::cerr << "internal compiler error: " << error.what() << std::endl;
        return 3;
    }

    return 0;
}
