#ifndef DOUBLE_CHAR_ENCODER_H
#define DOUBLE_CHAR_ENCODER_H

#include "encoder.hpp"

#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

class DoubleCharEncoder : public Encoder {
public:
    DoubleCharEncoder() {};
    ~DoubleCharEncoder() {};

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer) const;

    int numEntries () const;
    int64_t memoryUse () const;

private:
    bool buildDict(const std::vector<SymbolCode>& symbol_code_list);

    Code dict_[65536];
};

bool DoubleCharEncoder::build (const std::vector<std::string>& key_list,
                               const int64_t dict_size_limit) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(0);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    return buildDict(symbol_code_list);
}

int DoubleCharEncoder::encode (const std::string& key, uint8_t* buffer) const {
    int64_t* int_buf = (int64_t*)buffer;
    int idx = 0;
    int_buf[0] = 0;
    int int_buf_len = 0;
    int key_len = (int)key.length();
    for (int i = 0; i < key_len; i += 2) {
	unsigned s_idx = 256 * (uint8_t)key[i];
	if (i + 1 < key_len)
	    s_idx += (uint8_t)key[i + 1];
	int64_t s_buf = dict_[s_idx].code;
	int s_len = dict_[s_idx].len;
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

int DoubleCharEncoder::numEntries () const {
    return 65536;
}

int64_t DoubleCharEncoder::memoryUse () const {
    return sizeof(Code) * 65536;
}

bool DoubleCharEncoder::buildDict(const std::vector<SymbolCode>& symbol_code_list) {
    if (symbol_code_list.size() < 65536)
        return false;
    for (int i = 0; i < 65536; i++) {
        dict_[i] = symbol_code_list[i].second;
    }
    return true;
}

} // namespace ope

#endif // DOUBLE_CHAR_ENCODER_H
