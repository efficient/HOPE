#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "art_dic_tree.hpp"
#include "gtest/gtest.h"

namespace hope {

namespace treetest {
static const char kWordFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kInt64TestSize = 10000;
static std::vector<std::string> words;
static std::vector<std::string> integers;

class ARTDICTest : public ::testing::Test {
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

 public:
  static void PrintTs(const std::string &ts) {
    for (auto t : ts) {
      std::cout << std::hex << unsigned(static_cast<uint8_t>(t)) << " ";
    }
    std::cout << std::endl;
  }

  /**
   * Get next string based on alphabetical order
   * If all letters in the given string is 255, the next string is the string
   * with one more letter
   * @param str
   * @return The next string based on alphabetical order
   */
  static std::string GetNextString(const std::string &str) {
    for (int i = static_cast<int>(str.length()) - 1; i >= 0; i--) {
      if (static_cast<uint8_t>(str[i]) == 255) continue;
      char next_chr = str[i] + 1;
      return str.substr(0, i) + std::string(1, next_chr);
    }
    return str + std::string(1, 1);
  }

  /**
   * Find the interval the given str belongs to. interval_start <= str
   * @param sorted_intervals  Starting points of all intervals
   * @param cur_idx           search from the cur_idx, this param is used to
   * speed up search
   * @param cur_str           the string that we want to find its interval
   * @param out_next_idx      the index of the interval the string belongs to
   * @param out_next_str      the start string of that interval
   * @return true : find the next interval
             false : the given string does not belong to any interval
   */
  static bool GetNextInterval(const std::vector<std::string> &sorted_intervals, int cur_idx, const std::string &cur_str,
                              int *out_next_idx, std::string *out_next_str) {
    int i = cur_idx;
    for (; i < static_cast<int>(sorted_intervals.size()); i++) {
      if (StrCompare(sorted_intervals[i], cur_str) > 0) {
        *out_next_idx = i - 1;
        *out_next_str = sorted_intervals[i - 1];
        return true;
      }
    }
    return false;
  }
};

TEST_F(ARTDICTest, pointLookupInt64Test) {
  auto test = new ArtDicTree();
  std::vector<hope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(integers.size()) - 1; i++) {
    hope::SymbolCode symbol_code = hope::SymbolCode();
    symbol_code.first = integers[i];
    symbol_code.second = hope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(integers.size()) - 1; i++) {
    int prefix_len = -1;
    hope::Code result = test->lookup(integers[i].c_str(), integers[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

TEST_F(ARTDICTest, pointLookupWordTest) {
  auto test = new ArtDicTree();
  std::vector<hope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    hope::SymbolCode symbol_code = hope::SymbolCode();
    symbol_code.first = words[i];
    symbol_code.second = hope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int prefix_len = -1;
    hope::Code result = test->lookup(words[i].c_str(), words[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

/*
 * ART should find the first key greater than or equal to the given key
 * the test simulates the condition when the provided key does not exist
 * in ART
 */
TEST_F(ARTDICTest, withinRangeLookupTest) {
  auto test = new ArtDicTree();
  std::vector<hope::SymbolCode> ls;
  std::sort(words.begin(), words.end());

  for (int i = 0; i < static_cast<int>(words.size()); i++) {
    hope::SymbolCode symbol_code = hope::SymbolCode();
    symbol_code.first = words[i];
    symbol_code.second = hope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int prefix_len = -1;
    std::string cur_str = GetNextString(words[i]);
    std::string next_str = cur_str;
    int next_idx = i;
    bool find_next = GetNextInterval(words, i, cur_str, &next_idx, &next_str);
    // The given string does not belong to any interval
    if (!find_next) {
      break;
    }
    hope::Code result;
    result = test->lookup(cur_str.c_str(), cur_str.size(), prefix_len);

    if (result.code != next_idx) {
      std::cout << "*" << next_idx << " " << result.code << " " << i << std::endl;
      std::cout << words[next_idx] << " " << words[result.code] << " " << cur_str << std::endl;
      int l;
      auto org = test->lookup(words[i].c_str(), words[i].size(), l);
      std::cout << org.code << std::endl;
    }
    EXPECT_TRUE(result.code == next_idx);
  }
  delete test;
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
  std::random_shuffle(words.begin(), words.end());
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
