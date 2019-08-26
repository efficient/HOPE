#ifndef HASHDICT_H
#define HASHDICT_H

#include "common.hpp"
#include <map>
#include <boost/dynamic_bitset.hpp>

namespace ope {

class HashDict {
public:
  HashDict(const std::vector<SymbolCode> &keys);
  bool lookup(const std::string &in_key, int &key_bit_pos,
              std::string &symbol);
  int memory();

private:
  std::unordered_map<Code, std::string, CodeHash>dict_;
};

HashDict::HashDict(const std::vector<SymbolCode> &pairs) {
  for (int i = 0; i < (int)pairs.size(); i++) {
    dict_.emplace(pairs[i].second, pairs[i].first);
  }
}

bool HashDict::lookup(const std::string &in_key, int &key_bit_pos,
                        std::string &symbol) {
  int key_bit_len = (int)in_key.size() * 8;
  int8_t code_len = 1;
  uint32_t cur_code = 0;
  while (key_bit_pos + code_len <= key_bit_len) {
    Code tmp_code;
    int offset = 7 - ((key_bit_pos + code_len - 1) & 7);
    cur_code = (cur_code << 1) | ((in_key[(key_bit_pos + code_len - 1) >> 3] & (1 << offset)) >> offset);
    tmp_code.code = cur_code;
    tmp_code.len = code_len;
    auto re = dict_.find(tmp_code);
    if (re != dict_.end()) {
      key_bit_pos = key_bit_pos + code_len;
      symbol = re->second;
      return true;
    }
    code_len++;
  }
  if (cur_code == 0) {
    key_bit_pos = key_bit_len;
    return true;
  }
  return false;
}

int HashDict::memory() {
  int total_size = 0;
  for (auto it = dict_.begin(); it != dict_.end(); it++) {
    total_size += it->second.length() + 5;
  }
  return total_size;
}


}
#endif