#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "ngram_ss.hpp"

namespace ope {

namespace ngramsstest {

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

class NGramSSTest : public ::testing::Test {
 public:
  virtual void SetUp() {}
  virtual void TearDown() {}
};

void print(const std::vector<SymbolFreq> &symbol_freq_list) {
  for (int i = 0; i < (int)symbol_freq_list.size(); i++) {
    std::cout << symbol_freq_list[i].first << "\t" << symbol_freq_list[i].second << std::endl;
  }
}

TEST_F(NGramSSTest, word3Test) {
  NGramSS *ss = new NGramSS(3);
  std::vector<SymbolFreq> symbol_freq_list;
  ss->selectSymbols(words, 200, &symbol_freq_list);
  print(symbol_freq_list);
}

TEST_F(NGramSSTest, word4Test) {
  NGramSS *ss = new NGramSS(4);
  std::vector<SymbolFreq> symbol_freq_list;
  ss->selectSymbols(words, 200, &symbol_freq_list);
  print(symbol_freq_list);
}
/*
TEST_F (NGramSSTest, email3Test) {
NGramSS* ss = new NGramSS(3);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(emails, 200, &symbol_freq_list);
print(symbol_freq_list);
}

TEST_F (NGramSSTest, email4Test) {
NGramSS* ss = new NGramSS(4);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(emails, 200, &symbol_freq_list);
print(symbol_freq_list);
}

TEST_F (NGramSSTest, wiki3Test) {
NGramSS* ss = new NGramSS(3);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(wikis, 200, &symbol_freq_list);
print(symbol_freq_list);
}

TEST_F (NGramSSTest, wiki4Test) {
NGramSS* ss = new NGramSS(4);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(wikis, 200, &symbol_freq_list);
print(symbol_freq_list);
}

TEST_F (NGramSSTest, url3Test) {
NGramSS* ss = new NGramSS(3);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(urls, 200, &symbol_freq_list);
print(symbol_freq_list);
}

TEST_F (NGramSSTest, url4Test) {
NGramSS* ss = new NGramSS(4);
std::vector<SymbolFreq> symbol_freq_list;
ss->selectSymbols(urls, 200, &symbol_freq_list);
print(symbol_freq_list);
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

}  // namespace ngramsstest

}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ope::ngramsstest::loadWords();
  // ope::ngramsstest::loadEmails();
  // ope::ngramsstest::loadWikis();
  // ope::ngramsstest::loadUrls();
  return RUN_ALL_TESTS();
}
