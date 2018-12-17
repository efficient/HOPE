#ifndef CODE_GENERATOR_H
#define CODE_GENERATOR_H

#include <assert.h>
#include <vector>

#include "common.hpp"

namespace ope {

class CodeGenerator {
public:
    virtual ~CodeGenerator () {};
    
    virtual bool genCodes (const std::vector<SymbolFreq>& symbol_freq_list,
                           std::vector<SymbolCode>* symbol_code_list) = 0;

    // -1 means variable length
    virtual int getCodeLen () const = 0;

    virtual double getCompressionRate () const = 0;
};

} // namespace ope

#endif // CODE_GENERATOR_H
