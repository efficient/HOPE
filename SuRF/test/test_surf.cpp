#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>

#include "config.hpp"
#include "surf.hpp"

#include "encoder_factory.hpp"

namespace surf {

namespace surftest {

static const std::string kFilePath = "../../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kNumSuffixType = 2;
static const SuffixType kSuffixTypeList[kNumSuffixType] = {kNone, kReal};
static const int kNumSuffixLen = 1;
static const level_t kSuffixLenList[kNumSuffixLen] = {8};
static const int kEncoderType = 3;
static const int kDictSizeLimit = 10000;
static std::vector<std::string> words;

class SuRFUnitTest : public ::testing::Test {
 public:
  virtual void SetUp(){};
  virtual void TearDown(){};

  void newSuRFWords(SuffixType suffix_type, level_t suffix_len);

  std::string encodeString(const std::string &key);

  hope::Encoder *encoder_;
  SuRF *surf_;
  std::vector<std::string> words_compressed_;
  uint8_t buffer_[256];
};

void SuRFUnitTest::newSuRFWords(SuffixType suffix_type, level_t suffix_len) {
  if (suffix_type == kNone)
    surf_ = new SuRF(words_compressed_);
  else if (suffix_type == kHash)
    surf_ = new SuRF(words_compressed_, kHash, suffix_len, 0);
  else if (suffix_type == kReal)
    surf_ = new SuRF(words_compressed_, kIncludeDense, kSparseDenseRatio, kReal, 0, suffix_len);
  else if (suffix_type == kMixed)
    surf_ = new SuRF(words_compressed_, kMixed, suffix_len, suffix_len);
  else
    surf_ = new SuRF(words_compressed_);
}

std::string SuRFUnitTest::encodeString(const std::string &key) {
  int enc_len = encoder_->encode(key, buffer_);
  int enc_len_round = (enc_len + 7) >> 3;
  return std::string((const char *)buffer_, enc_len_round);
}

TEST_F(SuRFUnitTest, lookupWordTest) {
  encoder_ = hope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words, kDictSizeLimit);
  for (int i = 0; i < (int)words.size(); i++) {
    words_compressed_.push_back(encodeString(words[i]));
  }
  for (int t = 0; t < kNumSuffixType; t++) {
    for (int k = 0; k < kNumSuffixLen; k++) {
      newSuRFWords(kSuffixTypeList[t], kSuffixLenList[k]);
      for (int i = 0; i < (int)words.size(); i++) {
        std::string enc_str = encodeString(words[i]);
        bool key_exist = surf_->lookupKey(enc_str);
        ASSERT_TRUE(key_exist);
      }
      surf_->destroy();
      delete surf_;
    }
  }
  delete encoder_;
}

TEST_F(SuRFUnitTest, lookupRangeWordTest) {
  words_compressed_.clear();
  encoder_ = hope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words, kDictSizeLimit);
  for (int i = 0; i < (int)words.size(); i++) {
    words_compressed_.push_back(encodeString(words[i]));
  }
  for (int t = 0; t < kNumSuffixType; t++) {
    for (int k = 0; k < kNumSuffixLen; k++) {
      newSuRFWords(kSuffixTypeList[t], kSuffixLenList[k]);
      bool exist = surf_->lookupRange(encodeString(std::string("\1")), true, words_compressed_[0], true);
      ASSERT_TRUE(exist);
      exist = surf_->lookupRange(encodeString(std::string("\1")), true, words_compressed_[0], false);
      ASSERT_TRUE(exist);

      for (unsigned i = 0; i < words.size() - 1; i++) {
        exist = surf_->lookupRange(words_compressed_[i], true, words_compressed_[i + 1], true);
        ASSERT_TRUE(exist);
        exist = surf_->lookupRange(words_compressed_[i], true, words_compressed_[i + 1], false);
        ASSERT_TRUE(exist);
        exist = surf_->lookupRange(words_compressed_[i], false, words_compressed_[i + 1], true);
        ASSERT_TRUE(exist);
        exist = surf_->lookupRange(words_compressed_[i], false, words_compressed_[i + 1], false);
        ASSERT_TRUE(exist);
      }

      exist = surf_->lookupRange(words_compressed_[words_compressed_.size() - 1], true, encodeString(std::string("zzzzzzzz")), false);
      ASSERT_TRUE(exist);
      exist = surf_->lookupRange(words_compressed_[words_compressed_.size() - 1], false, encodeString(std::string("zzzzzzzz")), false);
      ASSERT_TRUE(exist);
    }
  }
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

}  // namespace surftest

}  // namespace surf

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  surf::surftest::loadWordList();
  return RUN_ALL_TESTS();
}
