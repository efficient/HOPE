#ifndef FIXED_LEN_DICT_CG_H
#define FIXED_LEN_DICT_CG_H

#include "code_generator.hpp"

namespace ope {

class FixedLenDictCG : public CodeGenerator {
public:
    FixedLenDictCG() { code_len_ = 0; };
    ~FixedLenDictCG() {};
    bool genCodes(const std::vector<SymbolFreq>& symbol_freq_list,
                  std::vector<SymbolCode>* symbol_code_list);
    int getCodeLen() const;
    double getCompressionRate() const;

private:
    int8_t code_len_;
};

bool FixedLenDictCG::genCodes(const std::vector<SymbolFreq>& symbol_freq_list,
			      std::vector<SymbolCode>* symbol_code_list) {
    int num_intervals = (int)symbol_freq_list.size();
    code_len_ = 0;
    while (num_intervals > 0) {
	code_len_++;
	num_intervals >>= 1;
    }
    //int64_t counter = 0;
    int32_t counter = 0;
    for (int i = 0; i < (int)symbol_freq_list.size(); i++) {
	Code code = {counter, code_len_};
	symbol_code_list->push_back(std::make_pair(symbol_freq_list[i].first, code));
	counter++;
    }
    return true;
}

int FixedLenDictCG::getCodeLen() const {
    return code_len_;
}

double FixedLenDictCG::getCompressionRate() const {
    // TODO
    return 0;
}
    
} // namespace ope

#endif // FIXED_LEN_DICT_CG_H
