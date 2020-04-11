#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "array_3gram_dict.hpp"
#include "gtest/gtest.h"

namespace hope {

namespace treetest {
static const char kWordFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kInt64TestSize = 10000;
static std::vector<std::string> words;
static std::vector<std::string> integers;

class Array3GramDicTest : public ::testing::Test {
 private:
  static int StrCompare(std::string s1, std::string s2) {
    int len1 = static_cast<int>(s1.size());
    int len2 = static_cast<int>(s2.size());
    int len = std::min(len1, len2);
    for (int i = 0; i < len; i++) {
      auto c1 = static_cast<uint8_t>(s1[i]);
      auto c2 = static_cast<uint8_t>(s2[i]);
      if (c1 < c2) return -1;
      if (c1 > c2) return 1;
    }
    if (len1 < len2)
      return -1;
    else if (len1 == len2)
      return 0;
    else
      return 1;
  }

  void SetUp() override {
    for (int i = 0; i < (int)integers.size(); i++) {
      if (i == 0 || (integers[i].substr(0, 3)).compare(int_3_[int_3_.size() - 1]) != 0)
        int_3_.push_back(integers[i].substr(0, 3));
    }
    std::sort(int_3_.begin(), int_3_.end());
    for (int i = 0; i < (int)int_3_.size(); i++) {
      hope::SymbolCode symbol_code = hope::SymbolCode();
      symbol_code.first = int_3_[i];
      symbol_code.second = hope::Code();
      symbol_code.second.code = i;
      int_symbol_code_list_.push_back(symbol_code);
    }

    for (int i = 0; i < (int)words.size(); i++) {
      if (i == 0 || (words[i].substr(0, 3)).compare(word_3_[word_3_.size() - 1]) != 0)
        word_3_.push_back(words[i].substr(0, 3));
    }
    std::sort(word_3_.begin(), word_3_.end());
    for (int i = 0; i < (int)word_3_.size(); i++) {
      hope::SymbolCode symbol_code = hope::SymbolCode();
      symbol_code.first = word_3_[i];
      symbol_code.second = hope::Code();
      symbol_code.second.code = i;
      word_symbol_code_list_.push_back(symbol_code);
    }
  };

 public:
  std::vector<std::string> word_3_;
  std::vector<std::string> int_3_;
  std::vector<SymbolCode> int_symbol_code_list_;
  std::vector<SymbolCode> word_symbol_code_list_;
};

TEST_F(Array3GramDicTest, pointLookupInt64Test) {
  auto dict = new hope::Array3GramDict();
  dict->build(int_symbol_code_list_);
  int prefix_len = -1;
  for (int i = 0; i < (int)int_3_.size(); i++) {
    hope::Code code = dict->lookup(int_3_[i].c_str(), (int)int_3_[i].size(), prefix_len);
    EXPECT_EQ(code.code, i);
  }
  delete dict;
}

TEST_F(Array3GramDicTest, pointLookupWordTest) {
  auto dict = new hope::Array3GramDict();
  dict->build(word_symbol_code_list_);
  int prefix_len = -1;
  for (int i = 0; i < (int)word_3_.size(); i++) {
    hope::Code code = dict->lookup(word_3_[i].c_str(), word_3_.size(), prefix_len);
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
