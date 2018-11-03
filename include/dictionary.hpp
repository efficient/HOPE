#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <assert.h>
#include <string>
#include <vector>

#include "common.hpp"

namespace ope {

class Dictionary {
public:
    virtual bool build (const std::vector<SymbolCode>& symbol_code_list) = 0;

    virtual Code lookup (const char* symbol, const int symbol_len, int& prefix_len) const = 0;

    virtual int numEntries () const = 0;

    virtual int64_t memoryUse () const = 0;
};

} // namespace ope

#endif // DICTIONARY_H
