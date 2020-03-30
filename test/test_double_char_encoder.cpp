#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <vector>

#include "code_generator_factory.hpp"
#include "double_char_encoder.hpp"
#include "gtest/gtest.h"
#include "symbol_selector_factory.hpp"

namespace ope {

namespace doublecharencodertest {

static const char kWordFilePath[] = "../../datasets/words.txt";
static const char kEmailFilePath[] = "../../datasets/emails.txt";
// static const int kWordTestSize = 234369;
static const int kWordTestSize = 234369;
static const int kEmailTestSize = 100000;
static const int kInt64TestSize = 100000;
static std::vector<std::string> words;
static std::vector<std::string> emails;
static std::vector<std::string> integers;
static const int kLongestCodeLen = 4096;

class DoubleCharEncoderTest : public ::testing::Test {};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(const std::string &str) {
  for (auto c : str) {
    std::cout << std::bitset<8>(c) << " ";
  }
  std::cout << std::endl;
}

std::string Uint64ToString(uint64_t key) {
  uint64_t endian_swapped_key = __builtin_bswap64(key);
  return std::string(reinterpret_cast<const char *>(&endian_swapped_key), 8);
}

TEST_F(DoubleCharEncoderTest, wordTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(words, 65536);
  auto buffer = new uint8_t[kLongestCodeLen];
  // encode single key each time
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
  // encode pair
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
  delete[] l_buffer;
  delete[] r_buffer;

  // encode batch
  std::vector<std::string> enc_keys;
  int batch_size = 10;
  int ls = (int)words.size();
  for (int i = 0; i < ls - batch_size; i += batch_size) {
    encoder->encodeBatch(words, i, batch_size, enc_keys);
  }
  std::cout << enc_keys.size() << std::endl;
  for (int i = 0; i < (int)enc_keys.size() - 1; i += 2) {
    std::string str1 = enc_keys[i];
    std::string str2 = enc_keys[i + 1];
    int cmp = str1.compare(str2);
    ASSERT_TRUE(cmp < 0);
  }

  delete encoder;
}

TEST_F(DoubleCharEncoderTest, emailTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(words, 65536);
  auto buffer = new uint8_t[kLongestCodeLen];
  // encode single key each time
  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    int len = encoder->encode(emails[i], buffer);
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(emails[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

/*TEST_F(DoubleCharEncoderTest, intTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(integers, 1024);
  auto buffer = new uint8_t[kLongestCodeLen];
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
}*/

TEST_F(DoubleCharEncoderTest, intTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(integers, 1024);
  auto buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(integers.size()) - 1; i++) {
    int len = encoder->encode(integers[i], buffer);
    std::string str1 = std::string((const char *)buffer, GetByteLen(len));
    len = encoder->encode(integers[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, GetByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);

/*#ifdef INCLUDE_DECODE
    len = encoder->decode(str1, buffer);
    std::string dec_str1 = std::string((const char *)buffer, len);
    cmp = dec_str1.compare(integers[i]);

    EXPECT_EQ(cmp, 0);

    len = encoder->decode(str2, buffer);
    std::string dec_str2 = std::string((const char *)buffer, len);
    cmp = dec_str2.compare(integers[i + 1]);
    EXPECT_EQ(cmp, 0);
#endif
*/
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

void LoadEmails() {
  std::ifstream infile(kEmailFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kEmailTestSize) {
    infile >> key;
    emails.push_back(key);
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
}  // namespace doublecharencodertest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::doublecharencodertest::LoadWords();
  ope::doublecharencodertest::LoadEmails();
  ope::doublecharencodertest::GenerateInt64();
  return RUN_ALL_TESTS();
}
