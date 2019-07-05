#ifndef SINGLE_CHAR_ENCODER_H
#define SINGLE_CHAR_ENCODER_H

#include <string.h>

#include "encoder.hpp"

#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"
#include "sbt.hpp"

//#define PRINT_BUILD_TIME_BREAKDOWN

namespace ope {

class SingleCharEncoder : public Encoder {
public:
    SingleCharEncoder() {};
    ~SingleCharEncoder() {
	if (decode_dict_)
	    delete decode_dict_;
    }

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer) const;

    void encodePair (const std::string& l_key, const std::string& r_key,
		     uint8_t* l_buffer, uint8_t* r_buffer,
		     int& l_enc_len, int& r_enc_len) const;

    int decode (const std::string& enc_key, uint8_t* buffer) const;

    int numEntries () const;
    int64_t memoryUse () const;

private:
    bool buildDict(const std::vector<SymbolCode>& symbol_code_list);

    Code dict_[256];
    SBT* decode_dict_;
};

bool SingleCharEncoder::build (const std::vector<std::string>& key_list,
                               const int64_t dict_size_limit) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    std::cout << "------------------------Build single character Encoder-----------------------" << std::endl;
    double cur_time = getNow();
#endif
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    std::cout << "Symbol Select time = " << getNow() - cur_time << std::endl;
    cur_time = getNow();
#endif

    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(1);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    std::cout << "Code Assign(Hu-Tucker) time = " << getNow() - cur_time << std::endl;
    cur_time = getNow();
#endif
    bool br = buildDict(symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    std::cout << "Build Dictionary time = " << getNow() - cur_time << std::endl;
#endif
    return br;
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

void SingleCharEncoder::encodePair(const std::string& l_key, const std::string& r_key,
				   uint8_t* l_buffer, uint8_t* r_buffer,
				   int& l_enc_len, int& r_enc_len) const {
    int64_t* int_buf_l = (int64_t*)l_buffer;
    int64_t* int_buf_r = (int64_t*)r_buffer;
    int idx_l = 0, idx_r = 0;
    int_buf_l[0] = 0;
    int int_buf_len_l = 0, int_buf_len_r = 0;
    bool found_mismatch = false;
    int r_start_pos = 0;
    for (int i = 0; i < (int)l_key.length(); i++) {
	if (!found_mismatch) {
	    if ((uint8_t)l_key[i] < (uint8_t)r_key[i]) {
		r_start_pos = i;
		memcpy((void*)r_buffer, (const void*)l_buffer, 8 * (idx_l + 1));
		idx_r = idx_l;
		int_buf_len_r = int_buf_len_l;
	    }
	    found_mismatch = true;
	}
	uint8_t s = (uint8_t)l_key[i];
	int64_t s_buf = dict_[s].code;
	int s_len = dict_[s].len;
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
    for (int i = r_start_pos; i < (int)r_key.length(); i++) {
	uint8_t s = (uint8_t)r_key[i];
	int64_t s_buf = dict_[s].code;
	int s_len = dict_[s].len;
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

int SingleCharEncoder::decode (const std::string& enc_key, uint8_t* buffer) const {
#ifdef INCLUDE_DECODE
    int buf_pos = 0;
    int key_bit_pos = 0;
    int idx = 0;
    while (key_bit_pos < (int)enc_key.size() * 8) {
	if (!decode_dict_->lookup(enc_key, key_bit_pos, &idx)) {
	    if (key_bit_pos < (int)enc_key.size() * 8)
		return 0;
	    else
		return buf_pos;
	}
	if (idx == 0) return buf_pos;
	buffer[buf_pos] = (uint8_t)(unsigned)idx;
	buf_pos++;
    }
    return buf_pos;
#else
    return 0;
#endif
}

int SingleCharEncoder::numEntries () const {
    return 256;
}

int64_t SingleCharEncoder::memoryUse () const {
#ifdef INCLUDE_DECODE
    return sizeof(Code) * 256 + decode_dict_->memory();
#else
    return sizeof(Code) * 256;
#endif
}

bool SingleCharEncoder::buildDict(const std::vector<SymbolCode>& symbol_code_list) {
    if (symbol_code_list.size() < 256)
        return false;
    for (int i = 0; i < 256; i++) {
        dict_[i] = symbol_code_list[i].second;
    }

#ifdef INCLUDE_DECODE
    std::vector<Code> codes;
    for (int i = 0; i < 256; i++) {
	codes.push_back(dict_[i]);
    }
    decode_dict_ = new SBT(codes);
#else
    decode_dict_ = nullptr;
#endif
    
    return true;
}

} // namespace ope

#endif // SINGLE_CHAR_ENCODER_H
