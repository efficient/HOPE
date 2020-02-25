#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "code_generator_factory.hpp"
#include "gtest/gtest.h"
#include "single_char_encoder.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

namespace singlecharencodertest {

static const char kFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kInt64TestSize = 10000;
static std::vector<std::string> words;
static std::vector<std::string> integers;
static const int kLongestCodeLen = 4096;

class SingleCharEncoderTest : public ::testing::Test {
 public:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(std::string str) {
  for (int i = 0; i < static_cast<int>(str.size()); i++) {
    std::cout << std::bitset<8>(str[i]) << " ";
  }
  std::cout << std::endl;
}

TEST_F(SingleCharEncoderTest, wordTest) {
  SingleCharEncoder *encoder = new SingleCharEncoder();
  encoder->build(words, 1000);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
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

TEST_F(SingleCharEncoderTest, wordPairTest) {
  SingleCharEncoder *encoder = new SingleCharEncoder();
  encoder->build(words, 1000);
  uint8_t *l_buffer = new uint8_t[kLongestCodeLen];
  uint8_t *r_buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int l_len = 0, r_len = 0;
    encoder->encodePair(words[i], words[i + 1], l_buffer, r_buffer, l_len, r_len);
    std::string str1 = std::string((const char *)l_buffer, GetByteLen(l_len));
    std::string str2 = std::string((const char *)r_buffer, GetByteLen(r_len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(SingleCharEncoderTest, wordBatchTest) {
  SingleCharEncoder *encoder = new SingleCharEncoder();
  std::vector<std::string> enc_keys;
  encoder->build(words, 1000);
  int batch_size = 10;
  int ls = static_cast<int>(words.size());
  for (int i = 0; i < ls - batch_size; i += batch_size) {
    encoder->encodeBatch(words, i, batch_size, enc_keys);
  }
  for (int i = 0; i < static_cast<int>(enc_keys.size()) - 1; i += 2) {
    std::string str1 = enc_keys[i];
    std::string str2 = enc_keys[i + 1];
    int cmp = strCompare(str1, str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(SingleCharEncoderTest, intTest) {
  SingleCharEncoder *encoder = new SingleCharEncoder();
  encoder->build(integers, 1000);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(integers.size()) - 1; i++) {
    int len = encoder->encode(integers[i], buffer);
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(integers[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);

#ifdef INCLUDE_DECODE
    len = encoder->decode(str1, buffer);
    std::string dec_str1 = std::string((const char *)buffer, len);
    cmp = dec_str1.compare(integers[i]);
    EXPECT_EQ(cmp, 0);

    len = encoder->decode(str2, buffer);
    std::string dec_str2 = std::string((const char *)buffer, len);
    cmp = dec_str2.compare(integers[i + 1]);
    EXPECT_EQ(cmp, 0);
#endif
  }
}

void LoadWords() {
  std::ifstream infile(kFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
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

}  // namespace singlecharencodertest
}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::singlecharencodertest::LoadWords();
  ope::singlecharencodertest::GenerateInt64();
  return RUN_ALL_TESTS();
}
