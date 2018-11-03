#ifndef SYMBOL_SELECTOR_H
#define SYMBOL_SELECTOR_H

#include <assert.h>
#include <string>
#include <vector>

#include "common.hpp"

namespace ope {

class SymbolSelector {
public:
    virtual bool selectSymbols (const std::vector<std::string>& key_list,
				const int64_t num_limit,
				std::vector<SymbolFreq>* symbol_freq_list) = 0;    
};

} // namespace ope

#endif // SYMBOL_SELECTOR_H
