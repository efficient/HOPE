#ifndef HEURISTIC_ENCODER_H
#define HEURISTIC_ENCODER_H

#include <string.h>

#include "encoder.hpp"

#include "dictionary_factory.hpp"
#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {
    class HeuristicEncoder : public Encoder {
    public:
        HeuristicEncoder(){};
        ~HeuristicEncoder(){};

        bool build (const std::vector<std::string>& key_list,
                    const int64_t dict_size_limit);

        int encode (const std::string& key, uint8_t* buffer) const;

        void encodePair (const std::string& l_key, const std::string& r_key,
                         uint8_t* l_buffer, uint8_t* r_buffer,
                         int& l_enc_len, int& r_enc_len) const;

        int numEntries () const;
        int64_t memoryUse () const;

    private:
        Dictionary* dict_;

    };

    bool HeuristicEncoder::build(const std::vector<std::string> &key_list,
            const int64_t dict_size_limit) {
        std::vector<SymbolFreq> symbol_freq_list;
        SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(5);
        symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);

        std::vector<SymbolCode> symbol_code_list;
        CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(0);
        code_generator->genCodes(symbol_freq_list, &symbol_code_list);

        dict_ = DictionaryFactory::createDictionary(5);

        return dict_->build(symbol_code_list);
    }

    int HeuristicEncoder::encode (const std::string& key, uint8_t* buffer) const {
        int64_t* int_buf = (int64_t*)buffer;
        int idx = 0;
        int_buf[0] = 0;
        int int_buf_len = 0;
        const char* key_str = key.c_str();
        int pos = 0;
        while (pos < (int)key.length()) {
            int prefix_len = 0;
            Code code = dict_->lookup(key_str + pos, key.size() - pos, prefix_len);
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

    // TODO
    void HeuristicEncoder::encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer,
                                     uint8_t *r_buffer, int &l_enc_len, int &r_enc_len) const {
        return;
    }

    // TODO
    int HeuristicEncoder::numEntries() const {
        return 0;
    }

    // TODO
    int64_t HeuristicEncoder::memoryUse() const {
        return 0;
    }

}

#endif