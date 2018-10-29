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

    virtual Code lookup (const std::string& symbol) = 0;

    virtual int numEntries () = 0;

    virtual int64_t memoryUse () = 0;
};

} // namespace ope

#endif // DICTIONARY_H
