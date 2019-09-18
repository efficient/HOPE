#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "ALMImproved_encoder.hpp"

namespace ope {

namespace almimprovedencodertest {

static const std::string kFilePath = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const int kLongestCodeLen = 4096;

static const std::string kEmailFilePath = "../../datasets/urls.txt";
static const int kEmailTestSize = 1000000;
static std::vector<std::string> emails;

class ALMImprovedEnCoderTest : public ::testing::Test {};

int getByteLen(const int bitlen) { return ((bitlen + 7) & ~7) / 8; }

void print(std::string str) {
  for (int i = 0; i < (int)str.size(); i++) {
    std::cout << std::bitset<8>(str[i]) << " ";
  }
  std::cout << std::endl;
}

TEST_F(ALMImprovedEnCoderTest, emailBatchTest) {
  ALMImprovedEncoder *encoder = new ALMImprovedEncoder();
  std::vector<std::string> enc_keys;
  encoder->build(emails, 65536);
  int batch_size = 10;
  int ls = (int)emails.size();
  for (int i = 0; i < ls - batch_size; i += batch_size) {
    encoder->encodeBatch(emails, i, batch_size, enc_keys);
  }
  for (int i = 0; i < (int)enc_keys.size() - 1; i += 2) {
    std::string str1 = enc_keys[i];
    std::string str2 = enc_keys[i + 1];
    int cmp = strCompare(str1, str2);
    ASSERT_TRUE(cmp < 0);
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
  std::sort(words.begin(), words.end());
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
  std::cout << "Load " << count << " emails" << std::endl;
  std::sort(emails.begin(), emails.end());
  emails.erase(unique(emails.begin(), emails.end()), emails.end());
}
}  // namespace almimprovedencodertest
}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::almimprovedencodertest::loadWords();
  ope::almimprovedencodertest::loadEmails();
  // ope::ngramencodertest::loadWikis();
  // ope::ngramencodertest::loadUrls();
  return RUN_ALL_TESTS();
}
