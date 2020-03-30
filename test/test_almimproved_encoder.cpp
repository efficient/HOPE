#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "ALMImproved_encoder.hpp"
#include "gtest/gtest.h"

namespace ope {

namespace almimprovedencodertest {

static const char kWordFilePath[] = "../../datasets/words.txt";
static const char kWikiFilePath[] = "../../datasets/wikis.txt";
static const char kUrlFilePath[] = "../../datasets/urls.txt";
static const int kWordTestSize = 23436;
static const int kWikiTestSize = 15000;
static const int kUrlTestSize = 4000;
static const int kInt64TestSize = 23436;
static std::vector<std::string> words;
static std::vector<std::string> wikis;
static std::vector<std::string> urls;
static std::vector<std::string> integers;
static const int kLongestCodeLen = 4096;

class ALMImprovedEncoderTest : public ::testing::Test {};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(const std::string &str) {
  for (int i = 0; i < static_cast<int>(str.size()); i++) {
    std::cout << std::bitset<8>(str[i]) << " ";
  }
  std::cout << std::endl;
}

std::string Uint64ToString(uint64_t key) {
  uint64_t endian_swapped_key = __builtin_bswap64(key);
  return std::string(reinterpret_cast<const char *>(&endian_swapped_key), 8);
}

std::string changeToBinary(int64_t num, int8_t len) {
  std::string result = std::string();
  int cnt = 0;
  while (num > 0) {
    result = std::string(1, num % 2 + '0') + result;
    num = num / 2;
    cnt += 1;
  }
  for (int i = cnt; i < len; i++) result = '0' + result;
  return result;
}

TEST_F(ALMImprovedEncoderTest, wordTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(words, 4096);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int len = encoder->encode(words[i], buffer);
    total_len += (words[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(words[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, wordPairTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(words, 1000);
  auto l_buffer = new uint8_t[kLongestCodeLen];
  auto r_buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int l_len = 0, r_len = 0;
    encoder->encodePair(words[i], words[i + 1], l_buffer, r_buffer, l_len, r_len);
    std::string str1 = std::string((const char *)l_buffer, GetByteLen(l_len));
    std::string str2 = std::string((const char *)r_buffer, GetByteLen(r_len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(ALMImprovedEncoderTest, wikiTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(wikis, 4096);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(wikis.size()) - 1; i++) {
    int len = encoder->encode(wikis[i], buffer);
    total_len += (wikis[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(words[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, urlTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(urls, 4096);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(urls.size()) - 1; i++) {
    int len = encoder->encode(urls[i], buffer);
    total_len += (wikis[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(words[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, intTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(integers, 4096);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(integers.size()) - 1; i++) {
    int len = encoder->encode(integers[i], buffer);
    total_len += (integers[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(integers[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
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

void LoadWikis() {
  std::ifstream infile(kWikiFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWikiTestSize) {
    infile >> key;
    wikis.push_back(key);
    count++;
  }
}

void LoadUrls() {
  std::ifstream infile(kUrlFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kUrlTestSize) {
    infile >> key;
    urls.push_back(key);
    count++;
  }
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

}  // namespace almimprovedencodertest
}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::almimprovedencodertest::LoadWords();
  ope::almimprovedencodertest::LoadWikis();
  ope::almimprovedencodertest::LoadUrls();
  ope::almimprovedencodertest::GenerateInt64();
  return RUN_ALL_TESTS();
}
