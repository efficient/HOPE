#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "code_generator_factory.hpp"
#include "double_char_encoder.hpp"
#include "gtest/gtest.h"
#include "symbol_selector_factory.hpp"

namespace ope {

namespace doublecharencodertest {

static const char kWordFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const int kLongestCodeLen = 4096;

class DoubleCharEncoderTest : public ::testing::Test {
};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(const std::string &str) {
  for (auto c : str) {
    std::cout << std::bitset<8>(c) << " ";
  }
  std::cout << std::endl;
}

TEST_F(DoubleCharEncoderTest, wordTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(words, 65536);
  auto buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int len = encoder->encode(words[i], buffer);
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(words[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);

#ifdef INCLUDE_DECODE
    len = encoder->decode(str1, buffer);
    std::string dec_str1 = std::string((const char *)buffer, len);
    cmp = dec_str1.compare(words[i]);

    EXPECT_EQ(cmp, 0);

    len = encoder->decode(str2, buffer);
    std::string dec_str2 = std::string((const char *)buffer, len);
    cmp = dec_str2.compare(words[i + 1]);
    EXPECT_EQ(cmp, 0);
#endif
  }
}

TEST_F(DoubleCharEncoderTest, wordPairTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(words, 65536);
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

}  // namespace doublecharencodertest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::doublecharencodertest::LoadWords();
  return RUN_ALL_TESTS();
}
