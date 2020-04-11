#ifndef SYMBOL_SELECTOR_H
#define SYMBOL_SELECTOR_H

#include <assert.h>
#include <string>
#include <vector>

#include "common.hpp"

namespace hope {

class SymbolSelector {
 public:
  virtual ~SymbolSelector(){};

  // key_list: sampled keys to extract common patterns from
  // num_limit: maximum number of symbol selected
  // symbol_freq_list: outputs that will be passed to the CodeAssigner
  virtual bool selectSymbols(const std::vector<std::string> &key_list,
			     const int64_t num_limit,
                             std::vector<SymbolFreq> *symbol_freq_list) = 0;
};

}  // namespace hope

#endif  // SYMBOL_SELECTOR_H
