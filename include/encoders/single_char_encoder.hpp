#ifndef SINGLE_CHAR_ENCODER_H
#define SINGLE_CHAR_ENCODER_H

#include "encoder.hpp"

#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

class SingleCharEncoder : public Encoder {
public:
    SingleCharEncoder() {};

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer) const;

    int numEntries () const;
    int64_t memoryUse () const;

private:
    bool buildDict(const std::vector<SymbolCode>& symbol_code_list);

    Code dict_[256];
};

bool SingleCharEncoder::build (const std::vector<std::string>& key_list,
                               const int64_t dict_size_limit) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(0);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    return buildDict(symbol_code_list);
}

int SingleCharEncoder::encode (const std::string& key, uint8_t* buffer) const {
    int64_t* int_buf = (int64_t*)buffer;
    int idx = 0;
    int_buf[0] = 0;
    int int_buf_len = 0;
    for (int i = 0; i < (int)key.length(); i++) {
	uint8_t s = (uint8_t)key[i];
	int64_t s_buf = dict_[s].code;
	int s_len = dict_[s].len;
	if (int_buf_len + s_len > 63) {
	    int num_bits_left = 64 - int_buf_len;
	    int_buf_len = s_len - num_bits_left;
	    int_buf[idx] <<= num_bits_left;
	    int_buf[idx] |= (s_buf >> int_buf_len);
	    int_buf[idx] = __builtin_bswap64(int_buf[idx]);
	    int_buf[idx + 1] = s_buf;
	    idx++;
	} else {
	    int_buf[idx] <<= s_len;
	    int_buf[idx] |= s_buf;
	    int_buf_len += s_len;
	}
    }
    int_buf[idx] <<= (64 - int_buf_len);
    int_buf[idx] = __builtin_bswap64(int_buf[idx]);
    return ((idx << 6) + int_buf_len);
}

int SingleCharEncoder::numEntries () const {
    return 256;
}

int64_t SingleCharEncoder::memoryUse () const {
    return sizeof(Code) * 256;
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

#endif // SINGLE_CHAR_ENCODER_H
