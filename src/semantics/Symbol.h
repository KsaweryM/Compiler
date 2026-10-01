#ifndef SEMANTICS_SYMBOL_H
#define SEMANTICS_SYMBOL_H

#include <string>

// A named (or hidden) memory location of the compiled program.
struct Symbol {
    enum class Kind {
        VARIABLE,     // declared scalar
        ARRAY,        // declared array
        ITERATOR,     // FOR loop iterator, read-only inside its loop
        LOOP_BOUND,   // hidden cell with the final value of a FOR loop
    };

    Kind kind = Kind::VARIABLE;
    std::string name;
    long long firstIndex = 0;   // arrays only
    long long lastIndex = 0;    // arrays only

    // Whether the scalar has been given a value earlier in the program text.
    bool initialized = false;

    // Estimated number of accesses at run time; frequently used symbols get the lowest
    // addresses, which are the cheapest to build.
    unsigned long long weight = 0;

    // Assigned by layoutMemory; for arrays, the address of the first element.
    long long address = -1;

    bool isArray() const { return kind == Kind::ARRAY; }
    bool isScalar() const { return kind != Kind::ARRAY; }
    long long size() const { return isArray() ? lastIndex - firstIndex + 1 : 1; }

    // Address of element `index`, or of the scalar itself.
    long long addressOf(long long index) const { return address + (index - firstIndex); }
};

#endif
