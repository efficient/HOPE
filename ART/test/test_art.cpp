#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>

#include "Tree.h"

#include "encoder_factory.hpp"

namespace arttest {

static const std::string kFilePath = "../../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kEncoderType = 1;
static const int kDictSizeLimit = 10000;
static std::vector<std::string> words;

void loadKey(TID tid, Key &key) {
  std::string *key_str = (std::string *)tid;
  key.set(key_str->c_str(), key_str->length());
}

class ARTUnitTest : public ::testing::Test {
 public:
  virtual void SetUp() { eliminatePrefixKeys(); }

  virtual void TearDown(){};

  void eliminatePrefixKeys();
  std::string encodeString(const std::string &key);

  ope::Encoder *encoder_;
  ART_ROWEX::Tree *art_;
  std::vector<std::string> words_;
  std::vector<std::string> words_compressed_;
  uint8_t buffer_[256];
};

void ARTUnitTest::eliminatePrefixKeys() {
  for (int i = 0; i < (int)words.size() - 1; i++) {
    int key_len = words[i].size();
    int next_key_len = words[i + 1].size();
    if (key_len < next_key_len) {
      std::string next_prefix = words[i + 1].substr(0, key_len);
      if (words[i].compare(next_prefix) != 0) words_.push_back(words[i]);
    } else {
      words_.push_back(words[i]);
    }
  }
}

std::string ARTUnitTest::encodeString(const std::string &key) {
  int enc_len = encoder_->encode(key, buffer_);
  int enc_len_round = (enc_len + 7) >> 3;
  return std::string((const char *)buffer_, enc_len_round);
}

TEST_F(ARTUnitTest, lookupWordTest) {
  art_ = new ART_ROWEX::Tree(loadKey);
  auto t = art_->getThreadInfo();
  for (int i = 0; i < (int)words_.size(); i++) {
    Key key;
    loadKey((TID) & (words_[i]), key);
    art_->insert(key, (TID) & (words_[i]), t);
  }

  auto t2 = art_->getThreadInfo();
  for (int i = 0; i < (int)words_.size(); i++) {
    Key key;
    loadKey((TID) & (words_[i]), key);
    TID result_tid = art_->lookup(key, t2);
    EXPECT_EQ((TID) & (words_[i]), result_tid);
  }
  delete art_;
}

TEST_F(ARTUnitTest, opcLookupWordTest) {
  encoder_ = ope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words_, kDictSizeLimit);
  for (int i = 0; i < (int)words_.size(); i++) {
    words_compressed_.push_back(encodeString(words_[i]));
  }
  art_ = new ART_ROWEX::Tree(loadKey);
  auto t = art_->getThreadInfo();
  for (int i = 0; i < (int)words_compressed_.size(); i++) {
    Key key;
    loadKey((TID) & (words_compressed_[i]), key);
    art_->insert(key, (TID) & (words_compressed_[i]), t);
  }

  auto t2 = art_->getThreadInfo();
  for (int i = 0; i < (int)words_.size(); i++) {
    std::string enc_str = encodeString(words_[i]);
    Key key;
    loadKey((TID)&enc_str, key);
    TID result_tid = art_->lookup(key, t2);
    EXPECT_EQ((TID) & (words_compressed_[i]), result_tid);
  }
  delete art_;
  delete encoder_;
}

void loadWordList() {
  std::ifstream infile(kFilePath);
  std::string key;
  int count = 0;
  while (infile.good() && count < kWordTestSize) {
    infile >> key;
    words.push_back(key);
    count++;
  }
}

}  // namespace arttest

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  arttest::loadWordList();
  return RUN_ALL_TESTS();
}
