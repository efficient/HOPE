#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>

#include "CPSBTreeOLC_Op.h"

#include "encoder_factory.hpp"

namespace prefixbtreetest {

static const std::string kWordFilePath = "../../../datasets/words.txt";
static const std::string kEmailFilePath = "../../../datasets/emails.txt";
static const int kWordTestSize = 234369;
static const int kEmailTestSize = 250000;
static const int kEncoderType = 1;
static const int kDictSizeLimit = 10000;
static std::vector<std::string> words;
static std::vector<std::string> emails;

class PrefixBtreeUnitTest : public ::testing::Test {
 public:
  virtual void SetUp() {}

  virtual void TearDown(){};

  std::string encodeString(const std::string &key);

  ope::Encoder *encoder_;
  cpsbtreeolc::BTree<int64_t> *bt_;
  std::vector<std::string> words_compressed_;
  uint8_t buffer_[256];
};

std::string PrefixBtreeUnitTest::encodeString(const std::string &key) {
  int enc_len = encoder_->encode(key, buffer_);
  int enc_len_round = (enc_len + 7) >> 3;
  return std::string((const char *)buffer_, enc_len_round);
}

TEST_F(PrefixBtreeUnitTest, lookupWordTest) {
  bt_ = new cpsbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)words.size(); i++) {
    cpsbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words[i]));
  }

  for (int i = 0; i < (int)words.size(); i++) {
    cpsbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(words[i])), re);
  }
  delete bt_;
}

TEST_F(PrefixBtreeUnitTest, lookupEmailTest) {
  bt_ = new cpsbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)emails.size(); i++) {
    cpsbtreeolc::Key key;
    key.setKeyStr(emails[i].c_str(), emails[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&emails[i]));
  }

  for (int i = 0; i < (int)emails.size(); i++) {
    cpsbtreeolc::Key key;
    key.setKeyStr(emails[i].c_str(), emails[i].length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(emails[i])), re);
  }
  delete bt_;
}

TEST_F(PrefixBtreeUnitTest, opcLookupWordTest) {
  encoder_ = ope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words, kDictSizeLimit);
  for (int i = 0; i < (int)words.size(); i++) {
    words_compressed_.push_back(encodeString(words[i]));
  }
  bt_ = new cpsbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)words_compressed_.size(); i++) {
    cpsbtreeolc::Key key;
    key.setKeyStr(words_compressed_[i].c_str(), words_compressed_[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words_compressed_[i]));
  }

  for (int i = 0; i < (int)words.size(); i++) {
    std::string enc_str = encodeString(words[i]);
    cpsbtreeolc::Key key;
    key.setKeyStr(enc_str.c_str(), enc_str.length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(words_compressed_[i])), re);
  }
  delete bt_;
  delete encoder_;
}

void loadWordList() {
  std::ifstream infile(kWordFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
}

void loadEmailList() {
  std::ifstream infile(kEmailFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kEmailTestSize) {
    infile >> key;
    emails.push_back(key);
    count++;
  }
}

}  // namespace prefixbtreetest

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  prefixbtreetest::loadWordList();
  prefixbtreetest::loadEmailList();
  return RUN_ALL_TESTS();
}
