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

    void encodePair (const std::string& l_key, const std::string& r_key,
		     uint8_t* l_buffer, uint8_t* r_buffer,
		     int& l_enc_len, int& r_enc_len) const;

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

void DoubleCharEncoder::encodePair (const std::string& l_key, const std::string& r_key,
				    uint8_t* l_buffer, uint8_t* r_buffer,
				    int& l_enc_len, int& r_enc_len) const {
    int64_t* int_buf_l = (int64_t*)l_buffer;
    int64_t* int_buf_r = (int64_t*)r_buffer;
    int idx_l = 0, idx_r = 0;
    int_buf_l[0] = 0;
    int int_buf_len_l = 0, int_buf_len_r = 0;
    int key_len_l = (int)l_key.length();
    int key_len_r = (int)r_key.length();
    bool found_mismatch = false;
    int r_start_pos = 0;
    for (int i = 0; i < key_len_l; i += 2) {
	unsigned s_idx = 256 * (uint8_t)l_key[i];
	if (i + 1 < key_len_l)
	    s_idx += (uint8_t)l_key[i + 1];
	
	if (!found_mismatch) {
	    unsigned s_idx_r = 256 * (uint8_t)r_key[i];
	    if (i + 1 < key_len_r)
		s_idx_r += (uint8_t)r_key[i + 1];
	    
	    if (s_idx < s_idx_r) {
		r_start_pos = i;
		memcpy((void*)r_buffer, (const void*)l_buffer, 8 * (idx_l + 1));
		idx_r = idx_l;
		int_buf_len_r = int_buf_len_l;
	    }
	    found_mismatch = true;
	}
	int64_t s_buf = dict_[s_idx].code;
	int s_len = dict_[s_idx].len;
	if (int_buf_len_l + s_len > 63) {
	    int num_bits_left = 64 - int_buf_len_l;
	    int_buf_len_l = s_len - num_bits_left;
	    int_buf_l[idx_l] <<= num_bits_left;
	    int_buf_l[idx_l] |= (s_buf >> int_buf_len_l);
	    int_buf_l[idx_l] = __builtin_bswap64(int_buf_l[idx_l]);
	    int_buf_l[idx_l + 1] = s_buf;
	    idx_l++;
	} else {
	    int_buf_l[idx_l] <<= s_len;
	    int_buf_l[idx_l] |= s_buf;
	    int_buf_len_l += s_len;
	}
    }
    int_buf_l[idx_l] <<= (64 - int_buf_len_l);
    int_buf_l[idx_l] = __builtin_bswap64(int_buf_l[idx_l]);
    l_enc_len = (idx_l << 6) + int_buf_len_l;

    // continue encoding right key
    for (int i = r_start_pos; i < key_len_r; i += 2) {
	unsigned s_idx = 256 * (uint8_t)r_key[i];
	if (i + 1 < key_len_r)
	    s_idx += (uint8_t)r_key[i + 1];
	
	int64_t s_buf = dict_[s_idx].code;
	int s_len = dict_[s_idx].len;
	if (int_buf_len_r + s_len > 63) {
	    int num_bits_left = 64 - int_buf_len_r;
	    int_buf_len_r = s_len - num_bits_left;
	    int_buf_r[idx_r] <<= num_bits_left;
	    int_buf_r[idx_r] |= (s_buf >> int_buf_len_r);
	    int_buf_r[idx_r] = __builtin_bswap64(int_buf_r[idx_r]);
	    int_buf_r[idx_r + 1] = s_buf;
	    idx_r++;
	} else {
	    int_buf_r[idx_r] <<= s_len;
	    int_buf_r[idx_r] |= s_buf;
	    int_buf_len_r += s_len;
	}
    }
    int_buf_r[idx_r] <<= (64 - int_buf_len_r);
    int_buf_r[idx_r] = __builtin_bswap64(int_buf_r[idx_r]);
    r_enc_len = (idx_r << 6) + int_buf_len_r;
}

int DoubleCharEncoder::numEntries () const {
    return 65536;
}

int64_t DoubleCharEncoder::memoryUse () const {
    //return (4 + 1) * 65536;
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
