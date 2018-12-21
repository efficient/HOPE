#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "hu_tucker_cg.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

namespace hutuckertest {

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

class HuTuckerCGTest : public ::testing::Test {
public:
    virtual void SetUp () {}
    virtual void TearDown () {}
};

void print(const std::vector<SymbolCode> &symbol_code_list,
	   const HuTuckerCG* code_generator) {
    for (int i = 0; i < (int)symbol_code_list.size(); i++) {
	int64_t code = symbol_code_list[i].second.code;
	unsigned len = symbol_code_list[i].second.len;
	std::cout << i << ", " << "\t" << len << "\t";
	code <<= (32 - len);
	std::cout << std::bitset<32>(code) << std::endl;
    }
    std::cout << "Compression Rate = "
	      << code_generator->getCompressionRate() << std::endl;
}

void printCPR(const HuTuckerCG* code_generator) {
    std::cout << "Compression Rate = "
	      << code_generator->getCompressionRate() << std::endl;
}

std::string changeToBinary(int64_t num) {
    std::string result = std::string();
    while (num > 0) {
        result = std::string(1, num%2 + '0') + result;
        num = num / 2;
    }
    return result;
}

TEST_F(HuTuckerCGTest, testCodeOrder) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(words, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    std::sort(symbol_code_list.begin(), symbol_code_list.end(),
            [](SymbolCode& x, SymbolCode& y){
        return x.first.compare(y.first) < 0;
    });
    for(auto iter = symbol_code_list.begin()+1; iter != symbol_code_list.end(); iter++){
        std::string str1 = changeToBinary(iter->second.code);
        std::string str2 = changeToBinary((iter-1)->second.code);
        int cmp = str1.compare(str2);
        assert(cmp > 0);
    }
}
//======================= Word Tests ==============================

TEST_F (HuTuckerCGTest, printSingleCharWordTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(words, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    print(symbol_code_list, code_generator);
}

TEST_F (HuTuckerCGTest, printDoubleCharWordTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
    symbol_selector->selectSymbols(words, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print3GramWordTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(3);
    symbol_selector->selectSymbols(words, 10000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print4GramWordTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(4);
    symbol_selector->selectSymbols(words, 50000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

//======================= Email Tests ==============================
/*
TEST_F (HuTuckerCGTest, printSingleCharEmailTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(emails, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    print(symbol_code_list, code_generator);
}

TEST_F (HuTuckerCGTest, printDoubleCharEmailTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
    symbol_selector->selectSymbols(emails, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print3GramEmailTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(3);
    symbol_selector->selectSymbols(emails, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print4GramEmailTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(4);
    symbol_selector->selectSymbols(emails, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}
*/
//======================= Wiki Tests ==============================
/*
TEST_F (HuTuckerCGTest, printSingleCharWikiTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(wikis, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    print(symbol_code_list, code_generator);
}

TEST_F (HuTuckerCGTest, printDoubleCharWikiTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
    symbol_selector->selectSymbols(wikis, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print3GramWikiTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(3);
    symbol_selector->selectSymbols(wikis, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print4GramWikiTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(4);
    symbol_selector->selectSymbols(wikis, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}
*/
//======================= Url Tests ==============================
/*
TEST_F (HuTuckerCGTest, printSingleCharUrlTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(1);
    symbol_selector->selectSymbols(urls, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    print(symbol_code_list, code_generator);
}

TEST_F (HuTuckerCGTest, printDoubleCharUrlTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
    symbol_selector->selectSymbols(urls, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print3GramUrlTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(3);
    symbol_selector->selectSymbols(urls, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
}

TEST_F (HuTuckerCGTest, print4GramUrlTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(4);
    symbol_selector->selectSymbols(urls, 65536, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    printCPR(code_generator);
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

} // namespace hutuckertest

} // namespace ope

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::hutuckertest::loadWords();
    //ope::hutuckertest::loadEmails();
    //ope::hutuckertest::loadWikis();
    //ope::hutuckertest::loadUrls();
    return RUN_ALL_TESTS();
}
