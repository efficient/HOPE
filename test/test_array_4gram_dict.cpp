#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "array_4gram_dict.hpp"
#include "gtest/gtest.h"

namespace hope {

namespace treetest {
static const char kWordFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kInt64TestSize = 10000;
static std::vector<std::string> words;
static std::vector<std::string> integers;

class Array3GramDicTest : public ::testing::Test {
  void SetUp() override {
    for (int i = 0; i < (int)integers.size(); i++) {
      if (i == 0 || (integers[i].substr(0, 4)).compare(int_4_[int_4_.size() - 1]) != 0)
        int_4_.push_back(integers[i].substr(0, 4));
    }
    std::sort(int_4_.begin(), int_4_.end());
    for (int i = 0; i < (int)int_4_.size(); i++) {
      hope::SymbolCode symbol_code = hope::SymbolCode();
      symbol_code.first = int_4_[i];
      symbol_code.second = hope::Code();
      symbol_code.second.code = i;
      int_symbol_code_list_.push_back(symbol_code);
    }

    for (int i = 0; i < (int)words.size(); i++) {
      if (i == 0 || (words[i].substr(0, 4)).compare(word_4_[word_4_.size() - 1]) != 0)
        word_4_.push_back(words[i].substr(0, 4));
    }
    std::sort(word_4_.begin(), word_4_.end());
    for (int i = 0; i < (int)word_4_.size(); i++) {
      hope::SymbolCode symbol_code = hope::SymbolCode();
      symbol_code.first = word_4_[i];
      symbol_code.second = hope::Code();
      symbol_code.second.code = i;
      word_symbol_code_list_.push_back(symbol_code);
    }
  };

 public:
  std::vector<std::string> word_4_;
  std::vector<std::string> int_4_;
  std::vector<SymbolCode> int_symbol_code_list_;
  std::vector<SymbolCode> word_symbol_code_list_;
};

TEST_F(Array3GramDicTest, pointLookupInt64Test) {
  auto dict = new hope::Array4GramDict();
  dict->build(int_symbol_code_list_);
  int prefix_len = -1;
  for (int i = 0; i < (int)int_4_.size(); i++) {
    hope::Code code = dict->lookup(int_4_[i].c_str(), (int)int_4_[i].size(), prefix_len);
    EXPECT_EQ(code.code, i);
  }
  delete dict;
}

TEST_F(Array3GramDicTest, pointLookupWordTest) {
  auto dict = new hope::Array4GramDict();
  dict->build(word_symbol_code_list_);
  int prefix_len = -1;
  for (int i = 0; i < (int)word_4_.size(); i++) {
    hope::Code code = dict->lookup(word_4_[i].c_str(), word_4_.size(), prefix_len);
    EXPECT_EQ(code.code, i);
  }
  delete dict;
}

int GetCommonPrefixLen(const std::string &str1, const std::string &str2) {
  int min_len = static_cast<int>(std::min(str1.size(), str2.size()));
  int i = 0;
  for (; i < min_len; i++) {
    if (str1[i] != str2[i]) return i;
  }
  return i;
}

void LoadWords() {
  std::ifstream infile(kWordFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
  std::sort(words.begin(), words.end());
}

std::string Uint64ToString(uint64_t key) {
  uint64_t endian_swapped_key = __builtin_bswap64(key);
  return std::string(reinterpret_cast<const char *>(&endian_swapped_key), 8);
}

void GenerateInt64() {
  std::random_device rd;   // Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd());  // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> dis(1, 2000000);
  uint64_t data = 1;
  for (int i = 0; i < kInt64TestSize; i++) {
    data += dis(gen);
    integers.push_back(Uint64ToString(data));
  }
}

}  // namespace treetest

}  // namespace hope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  hope::treetest::LoadWords();
  hope::treetest::GenerateInt64();
  return RUN_ALL_TESTS();
}
