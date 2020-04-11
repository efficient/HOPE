#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <assert.h>
#include <string>
#include <vector>

#include "common.hpp"

namespace hope {

class Dictionary {
 public:
  virtual ~Dictionary(){};

  virtual bool build(const std::vector<SymbolCode> &symbol_code_list) = 0;

  virtual Code lookup(const char *symbol, const int symbol_len, int &prefix_len) const = 0;

  virtual int numEntries() const = 0;

  virtual int64_t memoryUse() const = 0;
};

}  // namespace hope

#endif  // DICTIONARY_H
