#ifndef SINGLE_CHAR_ENCODER_H
#define SINGLE_CHAR_ENCODER_H

#include "encoder.hpp"

#include "hu_tucker_cg.hpp"
#include "single_char_ss.hpp"

namespace ope {

class SingleCharEncoder : public Encoder {
public:
    SingleCharEncoder() {};

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer);

private:
    bool buildDict(const std::vector<SymbolCode>& symbol_code_list);

    Code dict_[256];
};

bool SingleCharEncoder::build (const std::vector<std::string>& key_list,
                               const int64_t dict_size_limit) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selecotr = SymbolSelectorFactory::createSymbolSelector(0);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(0);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    return buildDict(symbol_code_list);
}

int SingleCharEncoder::encode (const std::string& key, uint8_t* buffer) {
    int64_t* int_buf = (int64_t*)code_buf;
    int idx = 0;
    int_buf[0] = 0;
    int int_buf_len = 0;
    for (int i = 0; i < (int)key.length(); i++) {
        
    }
}

bool SingleCharEncoder::buildDict(const std::vector<SymbolCode>& symbol_code_list) {
    if (symbol_code_list.size() < 256)
        return false;
    for (int i = 0; i < 256; i++) {
        dict_[i] = symbol_code_list[i].second;
    }
    return true;
}

} // namespace ope

#endif SINGLE_CHAR_ENCODER_H
