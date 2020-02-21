#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <random>

#include "ALMImproved_encoder.hpp"
#include "gtest/gtest.h"

namespace ope {

namespace almimprovedencodertest {

static const char kFilePath[] = "../../datasets/words.txt";
//static const int kWordTestSize = 234369;
static const int kWordTestSize = 23436;
static const int kInt64TestSize = 23436;
static std::vector<std::string> words;
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

TEST_F(ALMImprovedEncoderTest, wordTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(words, 65535);
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

TEST_F(ALMImprovedEncoderTest, intTest) {
   ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(integers, 65535);
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
  std::ifstream infile(kFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
  std::sort(words.begin(), words.end());
}

void GenerateInt64() {
  std::random_device rd;  //Will be used to obtain a seed for the random number engine
  std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
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
  ope::almimprovedencodertest::GenerateInt64();
  return RUN_ALL_TESTS();
}
