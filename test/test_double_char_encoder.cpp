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

static const char kFilePath[] = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const char kEmailFilePath[] = "../../datasets/emails.txt";
static const int kEmailTestSize = 25000000;
static std::vector<std::string> emails;
static const char kWikiFilePath[] = "../../datasets/wikis.txt";
static const int kWikiTestSize = 14000000;
static std::vector<std::string> wikis;
static const char kUrlFilePath[] = "../../datasets/urls.txt";
static const int kUrlTestSize = 25000000;
static std::vector<std::string> urls;
static const int kLongestCodeLen = 4096;

class DoubleCharEncoderTest : public ::testing::Test {
 public:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

int getByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void print(std::string str) {
  for (int i = 0; i < static_cast<int>(str.size()); i++) {
    std::cout << std::bitset<8>(str[i]) << " ";
  }
  std::cout << std::endl;
}

TEST_F(DoubleCharEncoderTest, wordTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(words, 65536);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int len = encoder->encode(words[i], buffer);
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(words[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
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
  uint8_t *l_buffer = new uint8_t[kLongestCodeLen];
  uint8_t *r_buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(words.size()) - 1; i++) {
    int l_len = 0, r_len = 0;
    encoder->encodePair(words[i], words[i + 1], l_buffer, r_buffer, l_len, r_len);
    std::string str1 = std::string((const char *)l_buffer, getByteLen(l_len));
    std::string str2 = std::string((const char *)r_buffer, getByteLen(r_len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(DoubleCharEncoderTest, emailBatchTest) {
  std::vector<std::string> enc_keys;
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(emails, 65536);
  int batch_size = 10;
  for (int i = 0; i < static_cast<int>(emails.size()) - batch_size; i += batch_size) {
    encoder->encodeBatch(emails, i, batch_size, enc_keys);
  }
  std::cout << enc_keys.size() << std::endl;
  for (int i = 0; i < static_cast<int>(enc_keys.size()) - 1; i += 2) {
    std::string str1 = enc_keys[i];
    std::string str2 = enc_keys[i + 1];
    int cmp = strCompare(str1, str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(DoubleCharEncoderTest, emailTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(emails, 65536);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(emails.size()) - 1; i++) {
    int len = encoder->encode(emails[i], buffer);
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(emails[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);

    if (cmp >= 0) {
      std::cout << i << std::endl;
      std::cout << emails[i] << std::endl;
      print(str1);
      std::cout << emails[i + 1] << std::endl;
      print(str2);
    }

    EXPECT_LT(cmp, 0);
  }
}

TEST_F(DoubleCharEncoderTest, wikiTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(wikis, 65536);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(wikis.size()) - 1; i++) {
    int len = encoder->encode(wikis[i], buffer);
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(wikis[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

TEST_F(DoubleCharEncoderTest, urlTest) {
  DoubleCharEncoder *encoder = new DoubleCharEncoder();
  encoder->build(urls, 65536);
  uint8_t *buffer = new uint8_t[kLongestCodeLen];
  for (int i = 0; i < static_cast<int>(urls.size()) - 1; i++) {
    int len = encoder->encode(urls[i], buffer);
    std::string str1 = std::string((const char *)buffer, getByteLen(len));
    len = encoder->encode(urls[i + 1], buffer);
    std::string str2 = std::string((const char *)buffer, getByteLen(len));
    int cmp = str1.compare(str2);
    EXPECT_LT(cmp, 0);
  }
}

void loadWords() {
  std::ifstream infile(kFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
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
}

}  // namespace doublecharencodertest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::doublecharencodertest::loadWords();
  ope::doublecharencodertest::loadEmails();
  ope::doublecharencodertest::loadWikis();
  ope::doublecharencodertest::loadUrls();
  return RUN_ALL_TESTS();
}
