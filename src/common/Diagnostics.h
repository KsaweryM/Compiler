#ifndef COMMON_DIAGNOSTICS_H
#define COMMON_DIAGNOSTICS_H

#include <stdexcept>
#include <string>

// An error in the compiled program, together with the source line it refers to.
class CompileError : public std::runtime_error {
public:
    CompileError(int line, const std::string& message) : std::runtime_error(message), line_(line) {}

    int line() const { return line_; }

private:
    int line_;
};

#endif
