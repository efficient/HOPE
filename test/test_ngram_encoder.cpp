#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "ngram_encoder.hpp"

namespace ope {

namespace ngramencodertest {

static const char kWordFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const int kLongestCodeLen = 4096;

class NGramEncoderTest : public ::testing::Test {
 public:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(const std::string &str) {
  for (auto c : str) {
    std::cout << std::bitset<8>(c) << " ";
  }
  std::cout << std::endl;
}

TEST_F(NGramEncoderTest, word3Test) {
  NGramEncoder *encoder = new NGramEncoder(3);
  encoder->build(words, 10000);
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
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(NGramEncoderTest, word3PairTest) {
  NGramEncoder *encoder = new NGramEncoder(3);
  encoder->build(words, 10000);
  auto l_buffer = new uint8_t[kLongestCodeLen];
  auto r_buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    total_len += (words[i].length() * 8);
    int l_len = 0, r_len = 0;
    encoder->encodePair(words[i], words[i + 1], l_buffer, r_buffer, l_len, r_len);
    total_enc_len += l_len;
    std::string str1 = std::string((const char *)l_buffer, GetByteLen(l_len));
    std::string str2 = std::string((const char *)r_buffer, GetByteLen(r_len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(NGramEncoderTest, word4Test) {
  NGramEncoder *encoder = new NGramEncoder(4);
  encoder->build(words, 10000);
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

    if (cmp >= 0) {
      std::cout << i << std::endl;
      std::cout << words[i] << std::endl;
      std::cout << words[i + 1] << std::endl;
      Print(str1);
      Print(str2);
    }
    EXPECT_LT(cmp, 0);
  }
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(NGramEncoderTest, word4PairTest) {
  NGramEncoder *encoder = new NGramEncoder(4);
  encoder->build(words, 10000);
  auto l_buffer = new uint8_t[kLongestCodeLen];
  auto r_buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    total_len += (words[i].length() * 8);
    int l_len = 0, r_len = 0;
    encoder->encodePair(words[i], words[i + 1], l_buffer, r_buffer, l_len, r_len);
    total_enc_len += l_len;
    std::string str1 = std::string((const char *)l_buffer, GetByteLen(l_len));
    std::string str2 = std::string((const char *)r_buffer, GetByteLen(r_len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
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
}

}  // namespace ngramencodertest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::ngramencodertest::loadWords();
  return RUN_ALL_TESTS();
}
