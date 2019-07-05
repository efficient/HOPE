#ifndef ALMIMPROVED_ENCODER_H
#define ALMIMPROVED_ENCODER_H

#include <string.h>

#include "encoder.hpp"

#include "dictionary_factory.hpp"
#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {
    class ALMImprovedEncoder: public Encoder {
    public:
        ALMImprovedEncoder(int _W = 10000) {
            W = _W;
        };

        ~ALMImprovedEncoder() {
            delete dict_;
        };

        bool build(const std::vector<std::string> &key_list,
                   const int64_t dict_size_limit);

        int encode(const std::string &key, uint8_t *buffer) const;

        void encodePair(const std::string &l_key, const std::string &r_key,
                        uint8_t *l_buffer, uint8_t *r_buffer,
                        int &l_enc_len, int &r_enc_len) const;

	int decode (const std::string& enc_key, uint8_t* buffer) const;

        int numEntries() const;

        int64_t memoryUse() const;

        void checkOrder(std::vector<SymbolCode> &symbol_code_list);

    private:
        int W;
        Dictionary *dict_;
        std::vector<SymbolCode> symbol_code_list;
        std::string changeToBinary(int64_t num, int8_t len);

    };

    std::string ALMImprovedEncoder::changeToBinary(int64_t num, int8_t len) {
        std::string result = std::string();
        int cnt = 0;
        while (num > 0) {
            result = std::string(1, num%2 + '0') + result;
            num = num / 2;
            cnt += 1;
        }
        for (int i = cnt; i < len; i++)
            result = '0' + result;
        return result;
    }

    void ALMImprovedEncoder::checkOrder(vector<SymbolCode> &symbol_code_list) {
        std::sort(symbol_code_list.begin(), symbol_code_list.end(),
                  [](SymbolCode& x, SymbolCode& y){
                      return x.first.compare(y.first) < 0;
                  });
        for(auto iter = symbol_code_list.begin()+1; iter != symbol_code_list.end(); iter++){
            std::string str1 = changeToBinary((iter-1)->second.code, (iter-1)->second.len);
            std::string str2 = changeToBinary(iter->second.code, iter->second.len);
            int cmp = str1.compare(str2);
            if (cmp >= 0) {
                std::cout << (iter-1)->first << "\t" << (iter-1)->second.code << "\t";
                std::cout << iter->first << "\t" << iter->second.code << "\t";
                std::cout << int((iter-1)->second.len) << "\t" << int(iter->second.len)<< "\t";
                std::cout << str1 << "\t" << str2 << "\t"<<std::endl;
            }
            assert(cmp < 0);
        }
    }

    bool ALMImprovedEncoder::build(const std::vector<std::string> &key_list,
                                 const int64_t dict_size_limit) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
        std::cout << "---------------------ALM Improved-------------------------" << std::endl;
        double curtime = getNow();
#endif
        std::vector<SymbolFreq> symbol_freq_list;
        SymbolSelector *symbol_selector = SymbolSelectorFactory::createSymbolSelector(6);
        symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list, W);
        delete symbol_selector;
#ifdef PRINT_BUILD_TIME_BREAKDOWN
        std::cout << "Symbol Select time = " << getNow() - curtime << std::endl;
        curtime = getNow();
#endif
        CodeGenerator *code_generator = CodeGeneratorFactory::createCodeGenerator(1);
//        CodeGenerator *code_generator = CodeGeneratorFactory::createCodeGenerator(0);
        code_generator->genCodes(symbol_freq_list, &symbol_code_list);
        delete code_generator;
#ifdef PRINT_BUILD_TIME_BREAKDOWN
        std::cout << "Code Assign(Hu-Tucker) time = " << getNow() - curtime << std::endl;
        curtime = getNow();
#endif
        dict_ = DictionaryFactory::createDictionary(5);
        bool dic = dict_->build(symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
        std::cout << "Build Dictionary time = " << getNow() - curtime << std::endl;
#endif
        return dic;
    }

    int ALMImprovedEncoder::encode(const std::string &key, uint8_t *buffer) const {
        int64_t *int_buf = (int64_t *) buffer;
        int idx = 0;
        int_buf[0] = 0;
        int int_buf_len = 0;
        const char *key_str = key.c_str();
        int pos = 0;
        while (pos < (int) key.length()) {
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

    void ALMImprovedEncoder::encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer,
                                      uint8_t *r_buffer, int &l_enc_len, int &r_enc_len) const {
        l_enc_len = encode(l_key,l_buffer);
        r_enc_len = encode(r_key,r_buffer);
        return;
    }

    int ALMImprovedEncoder::decode(const std::string& enc_key, uint8_t* buffer) const {
	return 0;
    }

    int ALMImprovedEncoder::numEntries() const {
        return dict_->numEntries();
    }

    int64_t ALMImprovedEncoder::memoryUse() const {
        return dict_->memoryUse();
    }
}

#endif
