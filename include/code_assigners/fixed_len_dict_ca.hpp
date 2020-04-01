#ifndef FIXED_LEN_DICT_CA_H
#define FIXED_LEN_DICT_CA_H

#include "code_assigner.hpp"

namespace ope {

class FixedLenDictCA : public CodeAssigner {
 public:
  FixedLenDictCA() { code_len_ = 0; };
  ~FixedLenDictCA(){};
  bool assignCodes(const std::vector<SymbolFreq> &symbol_freq_list, std::vector<SymbolCode> *symbol_code_list);
  int getCodeLen() const;
  double getCompressionRate() const;

 private:
  int8_t code_len_;
  std::vector<SymbolFreq> symbol_freq_list_;
};

bool FixedLenDictCA::assignCodes(const std::vector<SymbolFreq> &symbol_freq_list,
                              std::vector<SymbolCode> *symbol_code_list) {
  symbol_freq_list_ = symbol_freq_list;
  int num_intervals = (int)symbol_freq_list.size();
  code_len_ = 0;
  while (num_intervals > 0) {
    code_len_++;
    num_intervals >>= 1;
  }

  int32_t counter = 0;
  for (int i = 0; i < (int)symbol_freq_list.size(); i++) {
    Code code = {counter, code_len_};
    symbol_code_list->push_back(std::make_pair(symbol_freq_list[i].first, code));
    counter++;
  }
  return true;
}

int FixedLenDictCA::getCodeLen() const { return code_len_; }

double FixedLenDictCA::getCompressionRate() const {
  double len = 0, enc_len = 0;
  for (int i = 0; i < (int)symbol_freq_list_.size(); i++) {
    len += symbol_freq_list_[i].first.length() * symbol_freq_list_[i].second * 8.0;
    enc_len += symbol_freq_list_[i].second * code_len_;
  }

  double cpr = len / enc_len;
  return cpr;
}

}  // namespace ope

#endif  // FIXED_LEN_DICT_CA_H
