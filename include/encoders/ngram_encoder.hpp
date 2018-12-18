#ifndef NGRAM_ENCODER_H
#define NGRAM_ENCODER_H

#include "encoder.hpp"

#include "dictionary_factory.hpp"
#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

class NGramEncoder : public Encoder {
public:
    static const int kCgType = 0;    
    
    NGramEncoder(int n) : n_(n) {};
    ~NGramEncoder() { delete dict_; };

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer) const;

    void encodePair (const std::string& l_key, const std::string& r_key,
		     uint8_t* l_buffer, uint8_t* r_buffer,
		     int& l_enc_len, int& r_enc_len) const;

    int numEntries () const;
    int64_t memoryUse () const;

private:
    int n_;
    int code_len_; // -1 means variable length
    Dictionary* dict_;
};

bool NGramEncoder::build (const std::vector<std::string>& key_list,
			  const int64_t dict_size_limit) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    double time_start = getNow();
#endif
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(n_);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    double time_end = getNow();
    double time_diff = time_end - time_start;
    std::cout << "symbol select time = " << time_diff << std::endl;
#endif

#ifdef PRINT_BUILD_TIME_BREAKDOWN
    time_start = getNow();
#endif
    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(kCgType);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);
    code_len_ = code_generator->getCodeLen();
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    time_end = getNow();
    time_diff = time_end - time_start;
    std::cout << "hu-tucker time = " << time_diff << std::endl;
#endif

#ifdef PRINT_BUILD_TIME_BREAKDOWN
    time_start = getNow();
#endif
    dict_ = DictionaryFactory::createDictionary(n_);
    //return dict_->build(symbol_code_list);
    bool ret_val = dict_->build(symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
    time_end = getNow();
    time_diff = time_end - time_start;
    std::cout << "dict build time = " << time_diff << std::endl;
    std::cout << "num entries = " << dict_->numEntries() << std::endl;
#endif
    return ret_val;
}

#ifdef USE_FIXED_LEN_DICT_CODE
int NGramEncoder::encode (const std::string& key, uint8_t* buffer) const {
    int64_t* int_buf = (int64_t*)buffer;
    int int_buf_len = 0;
    int idx = 0;
    int_buf[0] = 0;
    const char* key_str = key.c_str();
    int pos = 0;
    if (code_len_ == 16 || code_len_ == 8) {
	while (pos < (int)key.length()) {
	    int prefix_len = 0;
	    Code code = dict_->lookup(key_str + pos, n_ + 1, prefix_len);
	    int_buf[idx] <<= code_len_;
	    int_buf[idx] += code.code;
	    int_buf_len += code_len_;
	    if (int_buf_len >= 64) {
		int_buf[idx] = __builtin_bswap64(int_buf[idx]);
		idx++;
		int_buf[idx] = 0;
		int_buf_len = 0;
	    }
	    pos += prefix_len;
	}
    } else {
	while (pos < (int)key.length()) {
	    int prefix_len = 0;
	    Code code = dict_->lookup(key_str + pos, n_ + 1, prefix_len);
	    int64_t s_buf = code.code;
	    int s_len = code.len;
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
	    pos += prefix_len;
	}
    }
    int_buf[idx] <<= (64 - int_buf_len);
    int_buf[idx] = __builtin_bswap64(int_buf[idx]);
    return ((idx << 6) + int_buf_len);
}
#else
int NGramEncoder::encode (const std::string& key, uint8_t* buffer) const {
    int64_t* int_buf = (int64_t*)buffer;
    int idx = 0;
    int_buf[0] = 0;
    int int_buf_len = 0;
    const char* key_str = key.c_str();
    int pos = 0;
    while (pos < (int)key.length()) {
	int prefix_len = 0;
	Code code = dict_->lookup(key_str + pos, n_ + 1, prefix_len);
	int64_t s_buf = code.code;
	int s_len = code.len;
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
	pos += prefix_len;
    }
    int_buf[idx] <<= (64 - int_buf_len);
    int_buf[idx] = __builtin_bswap64(int_buf[idx]);
    return ((idx << 6) + int_buf_len);
}
#endif

void NGramEncoder::encodePair (const std::string& l_key, const std::string& r_key,
			       uint8_t* l_buffer, uint8_t* r_buffer,
			       int& l_enc_len, int& r_enc_len) const {
    int64_t* int_buf_l = (int64_t*)l_buffer;
    int64_t* int_buf_r = (int64_t*)r_buffer;
    int idx_l = 0, idx_r = 0;
    int_buf_l[0] = 0;
    int int_buf_len_l = 0, int_buf_len_r = 0;
    int key_len_l = (int)l_key.length();
    int key_len_r = (int)r_key.length();

    // compute common prefix len
    int cp_len = 0;
    while ((cp_len < key_len_l) && (l_key[cp_len] == r_key[cp_len])) {
	cp_len++;
    }

    const char* l_key_str = l_key.c_str();
    const char* r_key_str = r_key.c_str();
    int pos = 0;

    bool found_mismatch = false;
    int r_start_pos = 0;
    while (pos < key_len_l) {
	if (!found_mismatch) {
	    if (pos + n_ >= cp_len) {
		r_start_pos = pos;
		memcpy((void*)r_buffer, (const void*)l_buffer, 8 * (idx_l + 1));
		idx_r = idx_l;
		int_buf_len_r = int_buf_len_l;
	    }
	    found_mismatch = true;
	}
	
	int prefix_len = 0;
	Code code = dict_->lookup(l_key_str + pos, n_ + 1, prefix_len);
	int64_t s_buf = code.code;
	int s_len = code.len;
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
	pos += prefix_len;
    }
    int_buf_l[idx_l] <<= (64 - int_buf_len_l);
    int_buf_l[idx_l] = __builtin_bswap64(int_buf_l[idx_l]);
    l_enc_len = (idx_l << 6) + int_buf_len_l;

    // continue encoding right key
    pos = r_start_pos;
    while (pos < key_len_r) {
	int prefix_len = 0;
	Code code = dict_->lookup(r_key_str + pos, n_ + 1, prefix_len);
	int64_t s_buf = code.code;
	int s_len = code.len;
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
	pos += prefix_len;
    }
    int_buf_r[idx_r] <<= (64 - int_buf_len_r);
    int_buf_r[idx_r] = __builtin_bswap64(int_buf_r[idx_r]);
    r_enc_len = (idx_r << 6) + int_buf_len_r;
}

int NGramEncoder::numEntries () const {
    return dict_->numEntries();
}

int64_t NGramEncoder::memoryUse () const {
    return dict_->memoryUse();
}

} // namespace ope

#endif // NGRAM_ENCODER_H