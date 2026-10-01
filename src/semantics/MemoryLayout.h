#ifndef SEMANTICS_MEMORY_LAYOUT_H
#define SEMANTICS_MEMORY_LAYOUT_H

#include <memory>
#include <vector>

#include "semantics/Symbol.h"

// Assigns memory addresses, starting from 0. Scalars come first, the most frequently
// used ones at the lowest addresses; arrays follow, smallest first.
void layoutMemory(const std::vector<std::unique_ptr<Symbol>>& symbols);

#endif
