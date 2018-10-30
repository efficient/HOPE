#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "hu_tucker_cg.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

namespace opetest {

static const std::string kFilePath = "../../test/words.txt";
static const int kWordTestSize = 234369;
static std::vector<std::string> words;
    //static const int kLongestCodeLen = 4096;

class HuTuckerCGTest : public ::testing::Test {
public:
    virtual void SetUp () {}
    virtual void TearDown () {}
};

TEST_F (HuTuckerCGTest, printSingleCharTest) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(0);
    symbol_selector->selectSymbols(words, 1000, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    HuTuckerCG* code_generator = new HuTuckerCG();
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    for (int i = 0; i < 256; i++) {
	std::cout << i << ", " << "\t" << (int)symbol_code_list[i].second.len << "\t";
	std::cout << std::bitset<16>(symbol_code_list[i].second.code)
		  << std::endl;
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

} // namespace opetest

} // namespace ope

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::opetest::loadWords();
    return RUN_ALL_TESTS();
}
