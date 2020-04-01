#ifndef ALMIMPROVED_ENCODER_H
#define ALMIMPROVED_ENCODER_H

#include <string.h>
#include <vector>
#include "encoder.hpp"

#include "code_assigner_factory.hpp"
#include "dictionary_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {
class ALMImprovedEncoder : public Encoder {
 public:
  static const int kCgType = 0;
  ALMImprovedEncoder(int _W = 10000) { W = _W; };

  ~ALMImprovedEncoder() { delete dict_; };

  bool build(const std::vector<std::string> &key_list, const int64_t dict_size_limit);

  int encode(const std::string &key, uint8_t *buffer) const;

  void encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer, uint8_t *r_buffer,
                  int &l_enc_len, int &r_enc_len) const;

  int64_t encodeBatch(const std::vector<std::string> &ori_keys, int start_id, int batch_size,
                      std::vector<std::string> &enc_keys);

  int decode(const std::string &enc_key, uint8_t *buffer) const;

  int numEntries() const;

  int64_t memoryUse() const;

  std::vector<SymbolCode> getSymbolCodeList();

 private:
  int W;
  Dictionary *dict_;
  std::vector<SymbolCode> symbol_code_list;
  std::string changeToBinary(int64_t num, int8_t len);
};

std::vector<SymbolCode> ALMImprovedEncoder::getSymbolCodeList() { return symbol_code_list; }

bool ALMImprovedEncoder::build(const std::vector<std::string> &key_list, const int64_t dict_size_limit) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "---------------------ALM Improved-------------------------" << std::endl;
  double curtime = getNow();
  double symbol_select_time = 0;
  double code_assign_time = 0;
  double build_dict_time = 0;
#endif
  std::vector<SymbolFreq> symbol_freq_list;
  SymbolSelector *symbol_selector = SymbolSelectorFactory::createSymbolSelector(6);
  reinterpret_cast<ALMImprovedSS *>(symbol_selector)->setW(W);
  symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  double new_time = getNow();
  symbol_select_time = new_time - curtime;
  curtime = new_time;
#endif
  CodeAssigner *code_assigner = CodeAssignerFactory::createCodeAssigner(kCgType);
  code_assigner->assignCodes(symbol_freq_list, &symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  new_time = getNow();
  code_assign_time = new_time - curtime;
  curtime = new_time;
#endif
  dict_ = DictionaryFactory::createDictionary(5);
  bool dic = dict_->build(symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  new_time = getNow();
  build_dict_time = new_time - curtime;

  std::cout << "Symbol Select time = " << symbol_select_time << std::endl;
  std::cout << "Code Assign time = " << code_assign_time << std::endl;
  std::cout << "Build Dictionary time = " << build_dict_time << std::endl;
#endif
  delete symbol_selector;
  delete code_assigner;
  return dic;
}

int ALMImprovedEncoder::encode(const std::string &key, uint8_t *buffer) const {
  int64_t *int_buf = (int64_t *)buffer;
  int idx = 0;
  int_buf[0] = 0;
  int int_buf_len = 0;
  const char *key_str = key.c_str();
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

void ALMImprovedEncoder::encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer,
                                    uint8_t *r_buffer, int &l_enc_len, int &r_enc_len) const {
  l_enc_len = encode(l_key, l_buffer);
  r_enc_len = encode(r_key, r_buffer);
  return;
}

int64_t ALMImprovedEncoder::encodeBatch(const std::vector<std::string> &ori_keys, int start_id, int batch_size,
                                        std::vector<std::string> &enc_keys) {
  return 0;
}

int ALMImprovedEncoder::decode(const std::string &enc_key, uint8_t *buffer) const { return 0; }

int ALMImprovedEncoder::numEntries() const { return dict_->numEntries(); }

int64_t ALMImprovedEncoder::memoryUse() const { return dict_->memoryUse(); }
}  // namespace ope

#endif
