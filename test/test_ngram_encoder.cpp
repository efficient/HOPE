#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "ngram_encoder.hpp"

namespace ope {

namespace ngramencodertest {

static const std::string kFilePath = "../../datasets/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const std::string kEmailFilePath = "../../datasets/emails.txt";
static const int kEmailTestSize = 25000000;
static std::vector<std::string> emails;
static const std::string kWikiFilePath = "../../datasets/wikis.txt";
static const int kWikiTestSize = 14000000;
static std::vector<std::string> wikis;
static const std::string kUrlFilePath = "../../datasets/urls.txt";
static const int kUrlTestSize = 25000000;
static std::vector<std::string> urls;
static const int kLongestCodeLen = 4096;

class NGramEncoderTest : public ::testing::Test {
public:
    virtual void SetUp () {}
    virtual void TearDown () {}
};

int getByteLen(const int bitlen) {
    return ((bitlen + 7) & ~7) / 8;
}

void print(std::string str) {
    for (int i = 0; i < (int)str.size(); i++) {
	std::cout << std::bitset<8>(str[i]) << " ";
    }
    std::cout << std::endl;
}

TEST_F (NGramEncoderTest, word3Test) {
    NGramEncoder* encoder = new NGramEncoder(3);
    encoder->build(words, 10000);
    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    for (int i = 0; i < (int)words.size() - 1; i++) {
	int len = encoder->encode(words[i], buffer);
	std::string str1 = std::string((const char*)buffer, getByteLen(len));
	len = encoder->encode(words[i + 1], buffer);
	std::string str2 = std::string((const char*)buffer, getByteLen(len));
	int cmp = str1.compare(str2);
	ASSERT_TRUE(cmp < 0);
    }
}
    /*
TEST_F (NGramEncoderTest, email3Test) {
    NGramEncoder* encoder = new NGramEncoder(3);
    encoder->build(emails, 65536);
    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    for (int i = 0; i < (int)emails.size() - 1; i++) {
	int len = encoder->encode(emails[i], buffer);
	std::string str1 = std::string((const char*)buffer, getByteLen(len));
	len = encoder->encode(emails[i + 1], buffer);
	std::string str2 = std::string((const char*)buffer, getByteLen(len));
	int cmp = str1.compare(str2);
	ASSERT_TRUE(cmp < 0);
    }
}

TEST_F (NGramEncoderTest, wiki3Test) {
    NGramEncoder* encoder = new NGramEncoder(3);
    encoder->build(wikis, 65536);
    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    for (int i = 0; i < (int)wikis.size() - 1; i++) {
	int len = encoder->encode(wikis[i], buffer);
	std::string str1 = std::string((const char*)buffer, getByteLen(len));
	len = encoder->encode(wikis[i + 1], buffer);
	std::string str2 = std::string((const char*)buffer, getByteLen(len));
	int cmp = str1.compare(str2);
	ASSERT_TRUE(cmp < 0);
    }
}

TEST_F (NGramEncoderTest, url3Test) {
    NGramEncoder* encoder = new NGramEncoder(3);
    encoder->build(urls, 65536);
    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    for (int i = 0; i < (int)urls.size() - 1; i++) {
	int len = encoder->encode(urls[i], buffer);
	std::string str1 = std::string((const char*)buffer, getByteLen(len));
	len = encoder->encode(urls[i + 1], buffer);
	std::string str2 = std::string((const char*)buffer, getByteLen(len));
	int cmp = str1.compare(str2);
	ASSERT_TRUE(cmp < 0);
    }
}
    */
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

} // namespace ngramencodertest

} // namespace ope

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::ngramencodertest::loadWords();
    //ope::ngramencodertest::loadEmails();
    //ope::ngramencodertest::loadWikis();
    //ope::ngramencodertest::loadUrls();
    return RUN_ALL_TESTS();
}
