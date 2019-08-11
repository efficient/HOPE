#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "heuristic_encoder.hpp"

namespace ope {

    namespace heuristicencodertest {

        static const std::string kFilePath = "../../datasets/words.txt";
        static const int kWordTestSize = 234369;
        static std::vector<std::string> words;
        static const int kLongestCodeLen = 4096;

        static const std::string kEmailFilePath = "../../datasets/emails.txt";
        static const int kEmailTestSize = 1000;
        static std::vector<std::string> emails;

        class HeuristicEnCoderTest : public ::testing::Test {

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
/*
        TEST_F(HeuristicEnCoderTest, wordTest) {
                HeuristicEncoder* encoder = new HeuristicEncoder();
                encoder->build(words, 65535);
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

                    if (cmp >= 0) {
                        int len1 = encoder->encode(words[i], buffer);
                        std::cout << words[i] << "\t" << len1 << std::endl;
                        print(str1);
                        int len2 = encoder->encode(words[i+1], buffer);
                        std::cout << words[i+1] << "\t" << len2 << std::endl;
                        print(str2);
                    }

                    ASSERT_TRUE(cmp < 0);
                }
                delete[] buffer;
                delete encoder;
                std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
        }


        TEST_F (HeuristicEnCoderTest, emailTest) {
            HeuristicEncoder* encoder = new HeuristicEncoder();
            encoder->build(emails, 65535);
            uint8_t* buffer = new uint8_t[kLongestCodeLen];
            int64_t total_len = 0;
            int64_t total_enc_len = 0;
            for (int i = 0; i < (int)emails.size() - 1; i++) {
                int len = encoder->encode(emails[i], buffer);
                total_len += (emails[i].length() * 8);
                total_enc_len += len;
                std::string str1 = std::string((const char*)buffer, getByteLen(len));
                len = encoder->encode(emails[i + 1], buffer);
                std::string str2 = std::string((const char*)buffer, getByteLen(len));
                int cmp = str1.compare(str2);
                if (cmp >= 0) {
                    int len1 = encoder->encode(emails[i], buffer);
                    std::cout << emails[i] << "\t" << len1 << std::endl;
                    print(str1);
                    int len2 = encoder->encode(emails[i+1], buffer);
                    std::cout << emails[i+1] << "\t" << len2 << std::endl;
                    print(str2);
                }
                ASSERT_TRUE(cmp < 0);
            }
            delete[] buffer;
            delete encoder;
            std::cout << "cpr = " << ((total_len + 0.0) / total_enc_len) << std::endl;
        }
*/
        TEST_F (HeuristicEnCoderTest, emailBatchTest) {
            HeuristicEncoder* encoder = new HeuristicEncoder();
            std::vector<std::string> org_enc_keys;
            std::vector<std::string> enc_keys;
            encoder->build(emails, 65536);
            int batch_size = 10;
            int ls = (int)emails.size();
            for (int i = 0; i <= ls - batch_size; i+=batch_size) {
                encoder->encodeBatch(emails, i, batch_size, enc_keys);
            }
            uint8_t* buffer = new uint8_t[8192];
            for (int i = 0; i < ls; i++) {
                int enc_len = encoder->encode(emails[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                org_enc_keys.push_back(std::string((const char*)buffer, enc_len_round));
            }
            ASSERT_TRUE(org_enc_keys.size() == enc_keys.size());
            std::cout << "Encode Size :" << enc_keys.size() << std::endl;
/*            for (int i = 0; i < (int)enc_keys.size() - 1; i+= 1) {
                std::string str1 = enc_keys[i];
                std::string str2 = enc_keys[i+1];
                int cmp = str1.compare(str2);
                ASSERT_TRUE(cmp < 0);
            }*/
            for (int i = 0; i < (int)enc_keys.size(); i++) {
                std::string str1 = enc_keys[i];
                std::string str2 = org_enc_keys[i];
                int cmp = str1.compare(str2);
                if (cmp != 0) {
                    std::cout << i << "\t" << emails[i] << std::endl;
                    printString(str1);
                    std::cout << std::endl;
                    printString(str2);
                    std::cout << std::endl;
                }
                ASSERT_TRUE(cmp == 0);
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
            std::sort(words.begin(), words.end());
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
            std::cout << "Load " << count << " emails" << std::endl;
            std::sort(emails.begin(), emails.end());
            emails.erase( unique( emails.begin(), emails.end() ), emails.end() );
        }
    }
}

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ope::heuristicencodertest::loadWords();
    ope::heuristicencodertest::loadEmails();
    //ope::ngramencodertest::loadWikis();
    //ope::ngramencodertest::loadUrls();
    return RUN_ALL_TESTS();
}
