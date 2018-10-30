#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "symbol_selector_factory.hpp"
#include "code_generator_factory.hpp"
#include "single_char_encoder.hpp"

namespace ope {

namespace singlecharencodertest {

static const std::string kFilePath = "../../test/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
static const std::string kEmailFilePath = "../../test/emails.txt";
static const int kEmailTestSize = 25000000;
static std::vector<std::string> emails;
static const std::string kWikiFilePath = "../../test/wikis.txt";
static const int kWikiTestSize = 14000000;
static std::vector<std::string> wikis;
static const std::string kUrlFilePath = "../../test/urls.txt";
static const int kUrlTestSize = 25000000;
static std::vector<std::string> urls;
static const int kLongestCodeLen = 4096;

class SingleCharEncoderTest : public ::testing::Test {
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

TEST_F (SingleCharEncoderTest, wordTest) {
    SingleCharEncoder* encoder = new SingleCharEncoder();
    encoder->build(words, 1000);
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

TEST_F (SingleCharEncoderTest, emailTest) {
    SingleCharEncoder* encoder = new SingleCharEncoder();
    encoder->build(emails, 1000);
    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    for (int i = 0; i < (int)emails.size() - 1; i++) {
	int len = encoder->encode(emails[i], buffer);
	std::string str1 = std::string((const char*)buffer, getByteLen(len));
	len = encoder->encode(emails[i + 1], buffer);
	std::string str2 = std::string((const char*)buffer, getByteLen(len));
	int cmp = str1.compare(str2);

	if (cmp >= 0) {
	    std::cout << i << std::endl;
	    std::cout << emails[i] << std::endl;
	    print(str1);
	    std::cout << emails[i + 1] << std::endl;
	    print(str2);
	}
	
	ASSERT_TRUE(cmp < 0);
    }
}

TEST_F (SingleCharEncoderTest, wikiTest) {
    SingleCharEncoder* encoder = new SingleCharEncoder();
    encoder->build(wikis, 1000);
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

TEST_F (SingleCharEncoderTest, urlTest) {
    SingleCharEncoder* encoder = new SingleCharEncoder();
    encoder->build(urls, 1000);
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

} // namespace singlecharencodertest

} // namespace ope

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::singlecharencodertest::loadWords();
    ope::singlecharencodertest::loadEmails();
    ope::singlecharencodertest::loadWikis();
    ope::singlecharencodertest::loadUrls();
    return RUN_ALL_TESTS();
}
