#include "semantics/MemoryLayout.h"

#include <algorithm>

void layoutMemory(const std::vector<std::unique_ptr<Symbol>>& symbols) {
    std::vector<Symbol*> scalars;
    std::vector<Symbol*> arrays;

    for (const auto& symbol : symbols) {
        (symbol->isArray() ? arrays : scalars).push_back(symbol.get());
    }

    std::stable_sort(scalars.begin(), scalars.end(), [](const Symbol* x, const Symbol* y) {
        return x->weight > y->weight;
    });
    std::stable_sort(arrays.begin(), arrays.end(), [](const Symbol* x, const Symbol* y) {
        return x->size() < y->size();
    });

    long long nextAddress = 0;

    for (Symbol* scalar : scalars) {
        scalar->address = nextAddress++;
    }

    for (Symbol* array : arrays) {
        array->address = nextAddress;
        nextAddress += array->size();
    }
}
