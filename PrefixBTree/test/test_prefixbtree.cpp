#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <string>
#include <vector>
#include <random>

#include "PrefixBtree.h"

#include "encoder_factory.hpp"

namespace prefixbtreetest {

static const std::string kWordFilePath = "../../../datasets/words.txt";
static const int kWordTestSize = 234369;
static const int kInt64TestSize = 250000;
static const int kEncoderType = 1;
static const int kDictSizeLimit = 10000;
static std::vector<std::string> words;
static std::vector<std::string> integers;

class PrefixBtreeUnitTest : public ::testing::Test {
 public:
  virtual void SetUp() {}

  virtual void TearDown(){};

  std::string encodeString(const std::string &key);

  hope::Encoder *encoder_;
  prefixbtreeolc::BTree<int64_t> *bt_;
  std::vector<std::string> words_compressed_;
  std::vector<std::string> integers_compressed_;
  uint8_t buffer_[256];
};

std::string PrefixBtreeUnitTest::encodeString(const std::string &key) {
  int enc_len = encoder_->encode(key, buffer_);
  int enc_len_round = (enc_len + 7) >> 3;
  return std::string((const char *)buffer_, enc_len_round);
}

TEST_F(PrefixBtreeUnitTest, lookupWordTest) {
  bt_ = new prefixbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)words.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words[i]));
  }

  for (int i = 0; i < (int)words.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(words[i])), re);
  }
  delete bt_;
}

TEST_F(PrefixBtreeUnitTest, opcLookupWordTest) {
  encoder_ = hope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words, kDictSizeLimit);
  for (int i = 0; i < (int)words.size(); i++) {
    words_compressed_.push_back(encodeString(words[i]));
  }
  bt_ = new prefixbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)words_compressed_.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words_compressed_[i].c_str(), words_compressed_[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words_compressed_[i]));
  }

  for (int i = 0; i < (int)words.size(); i++) {
    std::string enc_str = encodeString(words[i]);
    prefixbtreeolc::Key key;
    key.setKeyStr(enc_str.c_str(), enc_str.length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(words_compressed_[i])), re);
  }
  delete bt_;
  delete encoder_;
}

TEST_F(PrefixBtreeUnitTest, rangeScanWordTest) {
  bt_ = new prefixbtreeolc::BTree<int64_t>();
  std::vector<std::string> sorted_words;
  for (int i = 0; i < (int)words.size(); i++) {
    sorted_words.push_back(words[i]);
  }
  std::sort(sorted_words.begin(), sorted_words.end());
  std::hash<std::string> str_hash;
  // key: hash(string), value: index
  std::unordered_map<size_t, int> index_map;
  for (int i = 0; i < (int)sorted_words.size(); i++) {
    index_map[str_hash(sorted_words[i])] = i;
  }

  for (int i = 0; i < (int)words.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words[i]));
  }

  int scanlen = 100;
  int64_t re[200];
  for (int i = 0; i + scanlen < (int)words.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words[i].c_str(), words[i].length());
    int cnt = bt_->rangeScan(key, scanlen, re);
    int start_idx = index_map[str_hash(*reinterpret_cast<std::string *>(re[0]))];
    for (int j = 0; j < cnt; j++) {
      EXPECT_EQ(sorted_words[start_idx + j], *reinterpret_cast<std::string *>(re[j]));
    }
  }
  delete bt_;
}

TEST_F(PrefixBtreeUnitTest, rangeScanEncodeWordTest) {
  encoder_ = hope::EncoderFactory::createEncoder(kEncoderType);
  encoder_->build(words, kDictSizeLimit);
  for (int i = 0; i < (int)words.size(); i++) {
    words_compressed_.push_back(encodeString(words[i]));
  }

  bt_ = new prefixbtreeolc::BTree<int64_t>();
  std::vector<std::string> sorted_words;
  for (int i = 0; i < (int)words_compressed_.size(); i++) {
    sorted_words.push_back(words_compressed_[i]);
  }
  std::sort(sorted_words.begin(), sorted_words.end());
  std::hash<std::string> str_hash;
  // key: hash(string), value: index
  std::unordered_map<size_t, int> index_map;
  for (int i = 0; i < (int)sorted_words.size(); i++) {
    index_map[str_hash(sorted_words[i])] = i;
  }

  for (int i = 0; i < (int)words_compressed_.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words_compressed_[i].c_str(), words_compressed_[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&words_compressed_[i]));
  }

  int scanlen = 100;
  int64_t re[200];
  for (int i = 0; i + scanlen < (int)words_compressed_.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(words_compressed_[i].c_str(), words_compressed_[i].length());
    int cnt = bt_->rangeScan(key, scanlen, re);
    int start_idx = index_map[str_hash(*reinterpret_cast<std::string *>(re[0]))];
    for (int j = 0; j < cnt; j++) {
      EXPECT_EQ(sorted_words[start_idx + j], *reinterpret_cast<std::string *>(re[j]));
    }
  }
  delete bt_;
  delete encoder_;
}

TEST_F(PrefixBtreeUnitTest, lookIntTest) {
  bt_ = new prefixbtreeolc::BTree<int64_t>();
  for (int i = 0; i < (int)integers.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(integers[i].c_str(), integers[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&integers[i]));
  }

  for (int i = 0; i < (int)integers.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(integers[i].c_str(), integers[i].length());
    int64_t re;
    bool find = bt_->lookup(key, re);
    EXPECT_TRUE(find);
    EXPECT_EQ(reinterpret_cast<int64_t>(&(integers[i])), re);
  }
  delete bt_;
}

TEST_F(PrefixBtreeUnitTest, rangeScanIntTest) {
  bt_ = new prefixbtreeolc::BTree<int64_t>();
  std::vector<std::string> sorted_integers;
  for (int i = 0; i < (int)integers.size(); i++) {
    sorted_integers.push_back(integers[i]);
  }
  std::sort(sorted_integers.begin(), sorted_integers.end());
  std::hash<std::string> str_hash;
  // key: hash(string), value: index
  std::unordered_map<size_t, int> index_map;
  for (int i = 0; i < (int)sorted_integers.size(); i++) {
    index_map[str_hash(sorted_integers[i])] = i;
  }

  for (int i = 0; i < (int)integers.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(integers[i].c_str(), integers[i].length());
    bt_->insert(key, reinterpret_cast<int64_t>(&integers[i]));
  }

  int scanlen = 100;
  int64_t re[200];
  for (int i = 0; i + scanlen < (int)integers.size(); i++) {
    prefixbtreeolc::Key key;
    key.setKeyStr(integers[i].c_str(), integers[i].length());
    int cnt = bt_->rangeScan(key, scanlen, re);
    int start_idx = index_map[str_hash(*reinterpret_cast<std::string *>(re[0]))];
    for (int j = 0; j < cnt; j++) {
      EXPECT_EQ(sorted_integers[start_idx + j], *reinterpret_cast<std::string *>(re[j]));
    }
  }
  delete bt_;
}

std::string Uint64ToString(uint64_t key) {
  uint64_t endian_swapped_key = __builtin_bswap64(key);
  return std::string(reinterpret_cast<const char *>(&endian_swapped_key), 8);
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

void GenerateInt64() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(1, 2000000);
  uint64_t data = 1;
  for (int i = 0; i < kInt64TestSize; i++) {
    data += dis(gen);
    integers.push_back(Uint64ToString(data));
  }
}

}  // namespace prefixbtreetest

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  prefixbtreetest::loadWordList();
  prefixbtreetest::GenerateInt64();
  return RUN_ALL_TESTS();
}
