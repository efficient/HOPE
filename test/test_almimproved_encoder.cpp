#include <assert.h>

#include <algorithm>
#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "ALMImproved_encoder.hpp"
#include "gtest/gtest.h"

namespace ope {

namespace almimprovedencodertest {

static const char kFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const int kLongestCodeLen = 4096;

class ALMImprovedEncoderTest : public ::testing::Test {};

int GetByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void Print(const std::string &str) {
  for (int i = 0; i < static_cast<int>(str.size()); i++) {
    std::cout << std::bitset<8>(str[i]) << " ";
  }
  std::cout << std::endl;
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

    if (cmp >= 0) {
      int len1 = encoder->encode(words[i], buffer);
      std::cout << words[i] << "\t" << len1 << std::endl;
      Print(str1);
      int len2 = encoder->encode(words[i + 1], buffer);
      std::cout << words[i + 1] << "\t" << len2 << std::endl;
      Print(str2);
    }

    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, emailTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(emails, 65535);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    int len = encoder->encode(emails[i], buffer);
    total_len += (emails[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(emails[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);
    if (cmp >= 0) {
      int len1 = encoder->encode(emails[i], buffer);
      std::cout << emails[i] << "\t" << len1 << std::endl;
      Print(str1);
      int len2 = encoder->encode(emails[i + 1], buffer);
      std::cout << emails[i + 1] << "\t" << len2 << std::endl;
      Print(str2);
    }
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, wikiTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(wikis, 65535);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(wikis.size()) - 1; i++) {
    int len = encoder->encode(wikis[i], buffer);
    total_len += (wikis[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(wikis[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);
    if (cmp >= 0) {
      int len1 = encoder->encode(wikis[i], buffer);
      std::cout << emails[i] << "\t" << len1 << std::endl;
      Print(str1);
      int len2 = encoder->encode(wikis[i + 1], buffer);
      std::cout << emails[i + 1] << "\t" << len2 << std::endl;
      Print(str2);
    }
    EXPECT_LT(cmp, 0);
  }
  delete[] buffer;
  delete encoder;
  std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
}

TEST_F(ALMImprovedEncoderTest, urlTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  encoder->build(urls, 65535);
  auto buffer = new uint8_t[kLongestCodeLen];
  int64_t total_len = 0;
  int64_t total_enc_len = 0;
  for (int i = 0; i < static_cast<int>(urls.size()) - 1; i++) {
    int len = encoder->encode(urls[i], buffer);
    total_len += (urls[i].length() * 8);
    total_enc_len += len;
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(urls[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);
    if (cmp >= 0) {
      int len1 = encoder->encode(urls[i], buffer);
      std::cout << emails[i] << "\t" << len1 << std::endl;
      Print(str1);
      int len2 = encoder->encode(urls[i + 1], buffer);
      std::cout << emails[i + 1] << "\t" << len2 << std::endl;
      Print(str2);
    }
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

}  // namespace almimprovedencodertest
}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::almimprovedencodertest::loadWords();
  return RUN_ALL_TESTS();
}
