#include <assert.h>

#include <algorithm>
#include <bitset>
#include <cstdlib>  // std::rand, std::srand
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>
#include "art_dic_tree.hpp"
#include "gtest/gtest.h"

namespace ope {

namespace treetest {
static const char kEmailFilePath[] = "../../datasets/emails.txt";
static const char kWikiFilePath[] = "../../datasets/wikis.txt";
static const char kUrlFilePath[] = "../../datasets/urls.txt";
static const char kTsFilePath[] = "../../datasets/poisson_timestamps.csv";
static const int kEmailTestSize = 10000;
static const int kWikiTestSize = 100000;
static const int kUrlTestSize = 1000000;
static const int kTsTestSize = 10000;
static std::vector<std::string> emails;
static std::vector<std::string> wikis;
static std::vector<std::string> urls;
static std::vector<std::string> long_urls;
static std::vector<std::string> timestamps;

class ARTDICTest : public ::testing::Test {
 private:
  static int strCompare(std::string s1, std::string s2) {
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
  static void printTs(std::string ts) {
    for (int i = 0; i < static_cast<int>(ts.length()); i++) {
      std::cout << std::hex << unsigned(static_cast<uint8_t>(ts[i])) << " ";
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
  static std::string getNextString(std::string str) {
    for (int i = static_cast<int>(str.length()) - 1; i >= 0; i--) {
      if (static_cast<uint8_t>(str[i]) == 255) continue;
      char next_chr = str[i] + 1;
      return str.substr(0, i) + std::string(1, next_chr);
    }
    // All characters are shorter than 255 characters
    assert(str.length() < MAX_STR_LEN);
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
  static bool getNextInterval(std::vector<std::string> sorted_intervals, int cur_idx, const std::string &cur_str,
                              int *out_next_idx, std::string *out_next_str) {
    int i = cur_idx;
    for (; i < static_cast<int>(sorted_intervals.size()); i++) {
      if (strCompare(sorted_intervals[i], cur_str) > 0) {
        *out_next_idx = i - 1;
        *out_next_str = sorted_intervals[i - 1];
        return true;
      }
    }
    return false;
  }
};

TEST_F(ARTDICTest, emptyTest) {}

TEST_F(ARTDICTest, pointLookupEmailTest) {
  auto test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = emails[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    int prefix_len = -1;
    ope::Code result = test->lookup(emails[i].c_str(), emails[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

TEST_F(ARTDICTest, pointLookupWikiTest) {
  auto test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(wikis.size()) - 1; i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = wikis[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(wikis.size()) - 1; i++) {
    int prefix_len = -1;
    ope::Code result = test->lookup(wikis[i].c_str(), wikis[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

TEST_F(ARTDICTest, pointLookupUrlTest) {
  auto test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(urls.size()) - 1; i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = urls[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(urls.size()) - 1; i++) {
    int prefix_len = -1;
    ope::Code result = test->lookup(urls[i].c_str(), urls[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

TEST_F(ARTDICTest, pointLookupLongUrlTest) {
  auto test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;

  for (int i = 0; i < static_cast<int>(long_urls.size()) - 1; i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = long_urls[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(long_urls.size()) - 1; i++) {
    int prefix_len = -1;
    ope::Code result = test->lookup(long_urls[i].c_str(), long_urls[i].size(), prefix_len);
    if (result.code != i) std::cout << "lookup:" << result.code << " answer:" << i << std::endl;
    EXPECT_TRUE(result.code == i);
  }
  delete test;
}

TEST_F(ARTDICTest, pointTsLookupTest) {
  ArtDicTree *test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;
  std::cout << "number of test timestamps:" << timestamps.size() << std::endl;

  for (int i = 0; i < static_cast<int>(timestamps.size()) - 1; i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = timestamps[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(timestamps.size()) - 1; i++) {
    int prefix_len = -1;
    ope::Code result = test->lookup(timestamps[i].c_str(), timestamps[i].length(), prefix_len);
    if (result.code != i) {
      std::cout << std::dec << "lookup:" << result.code << " answer:" << i << std::endl;
    }
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
  std::vector<ope::SymbolCode> ls;
  std::sort(emails.begin(), emails.end());

  for (int i = 0; i < static_cast<int>(emails.size()); i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = emails[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    int prefix_len = -1;
    std::string cur_str = getNextString(emails[i]);
    std::string next_str = cur_str;
    int next_idx = i;
    bool find_next = getNextInterval(emails, i, cur_str, &next_idx, &next_str);
    // The given string does not belong to any interval
    if (!find_next) {
      break;
    }
    ope::Code result;
    result = test->lookup(cur_str.c_str(), cur_str.size(), prefix_len);

    if (result.code != next_idx) {
      std::cout << "*" << next_idx << " " << result.code << " " << i << std::endl;
      std::cout << emails[next_idx] << " " << emails[result.code] << " " << cur_str << std::endl;
      int l;
      auto org = test->lookup(emails[i].c_str(), emails[i].size(), l);
      std::cout << org.code << std::endl;
    }
    EXPECT_TRUE(result.code == next_idx);
  }
  delete test;
}

TEST_F(ARTDICTest, withinRangeTsLookupTest) {
  ArtDicTree *test = new ArtDicTree();
  std::vector<ope::SymbolCode> ls;
  std::cout << timestamps.size() << std::endl;
  std::sort(timestamps.begin(), timestamps.end());

  for (int i = 0; i < static_cast<int>(timestamps.size()); i++) {
    ope::SymbolCode symbol_code = ope::SymbolCode();
    symbol_code.first = timestamps[i];
    symbol_code.second = ope::Code();
    symbol_code.second.code = i;
    ls.push_back(symbol_code);
  }

  test->build(ls);

  for (int i = 0; i < static_cast<int>(timestamps.size()) - 1; i++) {
    int prefix_len = -1;
    std::string cur_str = getNextString(timestamps[i]);
    std::string next_str = cur_str;
    int next_idx = i;
    bool find_next = getNextInterval(timestamps, i, cur_str, &next_idx, &next_str);
    if (!find_next) {
      break;
    }
    ope::Code result;
    result = test->lookup(cur_str.c_str(), cur_str.size(), prefix_len);

    if (result.code != next_idx) {
      std::cout << "*" << next_idx << " " << result.code << " " << i << std::endl;
      std::cout << timestamps[next_idx] << " " << timestamps[result.code] << " " << cur_str << std::endl;
      int l;
      auto org = test->lookup(timestamps[i].c_str(), timestamps[i].size(), l);
      std::cout << org.code << std::endl;
    }
    EXPECT_TRUE(result.code == next_idx);
  }
  delete test;
}

int getCommonPrefixLen(const std::string &str1, const std::string &str2) {
  int min_len = static_cast<int>(std::min(str1.size(), str2.size()));
  int i = 0;
  for (; i < min_len; i++) {
    if (str1[i] != str2[i]) return i;
  }
  return i;
}

void loadEmails() {
  std::ifstream infile(kEmailFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kEmailTestSize) {
    infile >> key;
    emails.push_back(key);
    count++;
  }
  std::random_shuffle(emails.begin(), emails.end());
}

void loadWikis() {
  std::ifstream infile(kWikiFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWikiTestSize) {
    infile >> key;
    wikis.push_back(key);
    count++;
  }
  std::random_shuffle(wikis.begin(), wikis.end());
}

void loadUrls() {
  std::ifstream infile(kUrlFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kUrlTestSize) {
    infile >> key;
    urls.push_back(key);
    count++;
  }
  std::random_shuffle(urls.begin(), urls.end());
}

void loadLongUrls() {
  std::ifstream infile(kUrlFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kUrlTestSize) {
    infile >> key;
    if (key.size() < 200) {
      continue;
    }
    long_urls.push_back(key);
    count++;
  }
  std::random_shuffle(long_urls.begin(), long_urls.end());
}

std::string uint64ToString(uint64_t key) {
  uint64_t endian_swapped_key = __builtin_bswap64(key);
  return std::string(reinterpret_cast<const char *>(&endian_swapped_key), 8);
}

void loadTimestamp() {
  std::ifstream infile(kTsFilePath);
  uint64_t int_key;
  std::set<uint64_t> int_keys;
  std::string key;
  int count = 0;
  while (infile.good() && count < kTsTestSize) {
    infile >> int_key;
    // skip duplicate timestamp keys
    if (int_keys.find(int_key) != int_keys.end()) {
      continue;
    }
    int_keys.insert(int_key);
    key = uint64ToString(int_key);
    timestamps.push_back(key);
    count++;
  }
}
}  // namespace treetest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  ope::treetest::loadEmails();
  ope::treetest::loadWikis();
  ope::treetest::loadUrls();
  ope::treetest::loadLongUrls();
  ope::treetest::loadTimestamp();

  return RUN_ALL_TESTS();
}
