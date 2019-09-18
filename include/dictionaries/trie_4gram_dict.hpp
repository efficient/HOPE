#ifndef TRIE_4GRAM_DICT_H
#define TRIE_4GRAM_DICT_H

#include "dictionary.hpp"

namespace ope {

class Trie4GramDict : public Dictionary {
 public:
  class TrieNode {
   public:
    TrieNode() {
      ones_before_ = 0;
      for (int i = 0; i < 4; i++) {
        bits_[i] = 0;
      }
    }

    bool readBit(const int pos) const {
      assert(pos >= 0 && pos < 256);
      int word_id = pos >> 6;
      int offset = pos & 63;
      uint64_t mask = 0x8000000000000000 >> offset;
      return (bits_[word_id] & mask);
    }

    void setBit(const int pos) {
      assert(pos >= 0 && pos < 256);
      int word_id = pos >> 6;
      int offset = pos & 63;
      uint64_t mask = 0x8000000000000000 >> offset;
      bits_[word_id] |= mask;
    }

    const uint64_t *getBits() const { return bits_; }

    int32_t getOnesBefore() const { return (ones_before_ & 0x7FFFFFFF); }

    void setOnesBefore(int32_t num) {
      ones_before_ &= 0x80000000;
      ones_before_ += num;
    }

    bool hasPrefixKey() const { return (ones_before_ < 0); }

    void setPrefixKey() { ones_before_ |= 0x80000000; }

    void copyNode(const TrieNode *node) {
      ones_before_ = node->getOnesBefore();
      if (node->hasPrefixKey()) setPrefixKey();
      for (int i = 0; i < 4; i++) {
        bits_[i] = node->getBits()[i];
      }
    }

    int countBits(int pos) {
      assert(pos < 256);
      int count = 0;
      if (hasPrefixKey()) count = 1;
      if (pos < 0) return count;
      int last_word_id = pos >> 6;
      int offset = pos & 63;
      for (int i = 0; i < last_word_id; i++) {
        count += __builtin_popcountll(bits_[i]);
      }
      uint64_t last_word = bits_[last_word_id];
      last_word >>= (63 - offset);
      count += __builtin_popcountll(last_word);
      return count;
    }

   private:
    int32_t ones_before_;
    uint64_t bits_[4];
  };

 public:
  Trie4GramDict();
  ~Trie4GramDict();
  bool build(const std::vector<SymbolCode> &symbol_code_list);
  Code lookup(const char *symbol, const int symbol_len, int &prefix_len) const;
  int numEntries() const;
  int64_t memoryUse() const;

 private:
  void buildTrie(const std::vector<SymbolCode> &symbol_code_list, std::vector<TrieNode> &level_1,
                 std::vector<TrieNode> &level_2, std::vector<TrieNode> &level_3, std::vector<IntervalCode> &leafs);
  void copyNodesToArray(const std::vector<TrieNode> &level_1, const std::vector<TrieNode> &level_2,
                        const std::vector<TrieNode> &level_3, const std::vector<IntervalCode> &leafs);
  void fillinOnesBefore();

  int dict_size_;
  int level_1_num_nodes_;
  int level_2_num_nodes_;
  int level_3_num_nodes_;
  int num_leafs_;
  TrieNode *root_;
  TrieNode *level_1_;
  TrieNode *level_2_;
  TrieNode *level_3_;
  IntervalCode *leafs_;
};

Trie4GramDict::Trie4GramDict() {
  root_ = nullptr;
  level_1_num_nodes_ = 0;
  level_2_num_nodes_ = 0;
  level_3_num_nodes_ = 0;
  num_leafs_ = 0;
}

Trie4GramDict::~Trie4GramDict() {
  delete root_;
  delete[] level_1_;
  delete[] level_2_;
  delete[] level_3_;
  delete[] leafs_;
}

bool Trie4GramDict::build(const std::vector<SymbolCode> &symbol_code_list) {
  dict_size_ = (int)symbol_code_list.size();
  root_ = new TrieNode();
  std::vector<TrieNode> level_1, level_2, level_3;
  std::vector<IntervalCode> leafs;
  buildTrie(symbol_code_list, level_1, level_2, level_3, leafs);
  copyNodesToArray(level_1, level_2, level_3, leafs);
  fillinOnesBefore();
  return true;
}

void Trie4GramDict::buildTrie(const std::vector<SymbolCode> &symbol_code_list, std::vector<TrieNode> &level_1,
                              std::vector<TrieNode> &level_2, std::vector<TrieNode> &level_3,
                              std::vector<IntervalCode> &leafs) {
  int prev_char_idx_0 = -1, prev_char_idx_1 = -1, prev_char_idx_2 = -1, prev_char_idx_3 = -1;
  for (int i = 0; i < dict_size_; i++) {
    std::string symbol = symbol_code_list[i].first;
    int symbol_len = symbol.length();
    int char_idx_0 = (uint8_t)symbol[0];
    int char_idx_1 = -1;
    if (symbol_len > 1) char_idx_1 = (uint8_t)symbol[1];
    int char_idx_2 = -1;
    if (symbol_len > 2) char_idx_2 = (uint8_t)symbol[2];
    int char_idx_3 = -1;
    if (symbol_len > 3) char_idx_3 = (uint8_t)symbol[3];

    if (char_idx_0 > prev_char_idx_0) {
      level_1.push_back(TrieNode());
      level_2.push_back(TrieNode());
      level_3.push_back(TrieNode());
    } else if (char_idx_1 > prev_char_idx_1) {
      assert(char_idx_0 == prev_char_idx_0);
      level_2.push_back(TrieNode());
      level_3.push_back(TrieNode());
    } else if (char_idx_2 > prev_char_idx_2) {
      assert(char_idx_0 == prev_char_idx_0);
      assert(char_idx_1 == prev_char_idx_1);
      level_3.push_back(TrieNode());
    } else {
      assert(char_idx_0 == prev_char_idx_0);
      assert(char_idx_1 == prev_char_idx_1);
      assert(char_idx_2 == prev_char_idx_2);
      assert(char_idx_3 > prev_char_idx_3);
    }

    root_->setBit(char_idx_0);
    if (char_idx_1 < 0)
      level_1[level_1.size() - 1].setPrefixKey();
    else
      level_1[level_1.size() - 1].setBit(char_idx_1);
    if (char_idx_2 < 0)
      level_2[level_2.size() - 1].setPrefixKey();
    else
      level_2[level_2.size() - 1].setBit(char_idx_2);
    if (char_idx_3 < 0)
      level_3[level_3.size() - 1].setPrefixKey();
    else
      level_3[level_3.size() - 1].setBit(char_idx_3);

    IntervalCode leaf;
    leaf.common_prefix_len = 0;
    leaf.code.code = symbol_code_list[i].second.code;
    leaf.code.len = symbol_code_list[i].second.len;

    if (i < dict_size_ - 1) {
      std::string next_symbol = symbol_code_list[i + 1].first;
      int next_symbol_len = next_symbol.length();
      next_symbol[next_symbol_len - 1] -= 1;
      int j = 0;
      while (j < symbol_len && j < next_symbol_len && symbol[j] == next_symbol[j]) {
        leaf.common_prefix_len++;
        j++;
      }
    } else {
      leaf.common_prefix_len = (uint8_t)symbol.length();
    }
    leafs.push_back(leaf);

    prev_char_idx_0 = char_idx_0;
    prev_char_idx_1 = char_idx_1;
    prev_char_idx_2 = char_idx_2;
    prev_char_idx_3 = char_idx_3;
  }
}

void Trie4GramDict::copyNodesToArray(const std::vector<TrieNode> &level_1, const std::vector<TrieNode> &level_2,
                                     const std::vector<TrieNode> &level_3, const std::vector<IntervalCode> &leafs) {
  level_1_num_nodes_ = level_1.size();
  level_1_ = new TrieNode[level_1_num_nodes_];
  for (int i = 0; i < level_1_num_nodes_; i++) {
    level_1_[i].copyNode(&level_1[i]);
  }

  level_2_num_nodes_ = level_2.size();
  level_2_ = new TrieNode[level_2_num_nodes_];
  for (int i = 0; i < level_2_num_nodes_; i++) {
    level_2_[i].copyNode(&level_2[i]);
  }

  level_3_num_nodes_ = level_3.size();
  level_3_ = new TrieNode[level_3_num_nodes_];
  for (int i = 0; i < level_3_num_nodes_; i++) {
    level_3_[i].copyNode(&level_3[i]);
  }

  num_leafs_ = leafs.size();
  leafs_ = new IntervalCode[num_leafs_];
  for (int i = 0; i < num_leafs_; i++) {
    leafs_[i].common_prefix_len = leafs[i].common_prefix_len;
    leafs_[i].code.code = leafs[i].code.code;
    leafs_[i].code.len = leafs[i].code.len;
  }
}

void Trie4GramDict::fillinOnesBefore() {
  int level_1_ones = level_1_[0].countBits(255);
  for (int i = 1; i < level_1_num_nodes_; i++) {
    level_1_[i].setOnesBefore(level_1_ones);
    level_1_ones += level_1_[i].countBits(255);
  }
  int level_2_ones = level_2_[0].countBits(255);
  for (int i = 1; i < level_2_num_nodes_; i++) {
    level_2_[i].setOnesBefore(level_2_ones);
    level_2_ones += level_2_[i].countBits(255);
  }
  int level_3_ones = level_3_[0].countBits(255);
  for (int i = 1; i < level_3_num_nodes_; i++) {
    level_3_[i].setOnesBefore(level_3_ones);
    level_3_ones += level_3_[i].countBits(255);
  }
}

Code Trie4GramDict::lookup(const char *symbol, const int symbol_len, int &prefix_len) const {
  int char_idx_0 = (uint8_t)symbol[0];
  int char_idx_1 = 0;
  if (symbol_len > 1) char_idx_1 = (uint8_t)symbol[1];
  int char_idx_2 = 0;
  if (symbol_len > 2) char_idx_2 = (uint8_t)symbol[2];
  int char_idx_3 = 0;
  if (symbol_len > 3) char_idx_3 = (uint8_t)symbol[3];

  int level_1_node_num = root_->countBits(char_idx_0) - 1;
  TrieNode level_1_node = level_1_[level_1_node_num];
  int level_2_node_num = level_1_node.getOnesBefore() + level_1_node.countBits(char_idx_1) - 1;
  TrieNode level_2_node = level_2_[level_2_node_num];

  int level_3_node_num = level_2_node.getOnesBefore() - 1;
  bool level_1_mismatch = false;
  if (level_1_node.readBit(char_idx_1)) {
    level_3_node_num += level_2_node.countBits(char_idx_2);
  } else {
    level_3_node_num += level_2_node.countBits(255);
    level_1_mismatch = true;
  }
  TrieNode level_3_node = level_3_[level_3_node_num];

  int leaf_num = level_3_node.getOnesBefore() - 1;
  if (level_1_mismatch) {
    leaf_num += level_3_node.countBits(255);
  } else {
    if (level_2_node.readBit(char_idx_2))
      leaf_num += level_3_node.countBits(char_idx_3);
    else
      leaf_num += level_3_node.countBits(255);
  }

  prefix_len = leafs_[leaf_num].common_prefix_len;
  return leafs_[leaf_num].code;
}

int Trie4GramDict::numEntries() const { return dict_size_; }

int64_t Trie4GramDict::memoryUse() const {
  return (sizeof(Trie4GramDict) +
          sizeof(TrieNode) * (1 + level_1_num_nodes_ + level_2_num_nodes_ + level_3_num_nodes_) +
          sizeof(Code) * num_leafs_);
}

}  // namespace ope

#endif  // TRIE_4GRAM_DICT_H
