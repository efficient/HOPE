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

    int numEntries () const;
    int64_t memoryUse () const;

private:
    int n_;
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

int NGramEncoder::numEntries () const {
    return dict_->numEntries();
}

int64_t NGramEncoder::memoryUse () const {
    return dict_->memoryUse();
}

} // namespace ope

#endif // NGRAM_ENCODER_H
