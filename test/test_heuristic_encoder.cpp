#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "heuristic_encoder.hpp"

namespace ope {

    namespace heuristicencodertest {

        static const std::string kFilePath = "../../datasets/words.txt";
        static const int kWordTestSize = 100;
        static std::vector<std::string> words;
        static const int kLongestCodeLen = 4096;

        class HeuristicEnCoderTest : public ::testing::Test {

        };

        int getByteLen(const int bitlen) {
            return ((bitlen + 7) & ~7) / 8;
        }

        TEST_F(HeuristicEnCoderTest, word3Test) {
                HeuristicEncoder* encoder = new HeuristicEncoder();
                encoder->build(words, 100000000);
                uint8_t* buffer = new uint8_t[kLongestCodeLen];
                int64_t total_len = 0;
                int64_t total_enc_len = 0;
                for (int i = 0; i < (int)words.size() - 1; i++) {
                    int len = encoder->encode(words[i], buffer);
                    total_len += (words[i].length() * 8);
                    total_enc_len += len;
                    std::string str1 = std::string((const char*)buffer, getByteLen(len));
                    len = encoder->encode(words[i + 1], buffer);
                    std::string str2 = std::string((const char*)buffer, getByteLen(len));
                    int cmp = str1.compare(str2);
                    ASSERT_TRUE(cmp < 0);
                }
                std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;

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

    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::heuristicencodertest::loadWords();
    //ope::ngramencodertest::loadEmails();
    //ope::ngramencodertest::loadWikis();
    //ope::ngramencodertest::loadUrls();
    return RUN_ALL_TESTS();
}
