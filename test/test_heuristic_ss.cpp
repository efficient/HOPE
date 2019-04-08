#include "gtest/gtest.h"

#include <assert.h>

#include <fstream>
#include <iostream>

#include "heuristic_ss.hpp"
namespace ope {

    namespace heuristicsstest {
        static const std::string kFilePath = "../../datasets/words.txt";
        static const int kWordTestSize = 234369;
        static std::vector<std::string> words;
        static const std::string kEmailFilePath = "../../datasets/emails.txt";
        static const int kEmailTestSize = 100000;
        static std::vector<std::string> emails;

        class HeuristicSSTest : public ::testing::Test {
        };

        TEST_F(HeuristicSSTest, getFrequencyTable1) {
            //HeuristicSS ss;
            //ss.getFrequencyTable(emails);
            //std::cout << emails.size() << std::endl;
            //std::cout << ss.freq_map_.size() << std::endl;
            //for (auto it = ss.freq_map_.begin(); it != ss.freq_map_.end(); it++) {
            // 	std::cout << it->first << "\t" << it->second << std::endl;
            //}
        }

        TEST_F(HeuristicSSTest, fillGapSmall) {
            //HeuristicSS ss;
            //ss.fillGap("abc", "dc");
            //for (auto iter = ss.intervals_.begin(); iter != ss.intervals_.end(); iter++) {
            //     std::cout << "[" << iter->first << ", " << iter->second << ")" << std::endl;
            //}
            //std::cout << "------------------------" << std::endl;
            //HeuristicSS ss1;
            //ss1.fillGap("beq", "bev");
            //for (auto iter = ss1.intervals_.begin(); iter != ss1.intervals_.end(); iter++) {
            //	std::cout << "[" << iter->first << ", " << iter->second << ")" << std::endl;
            //}
            //std::cout << "------------------------" << std::endl;
            //HeuristicSS ss2;
            //ss2.fillGap("beq", "ber");
            //for (auto iter = ss2.intervals_.begin(); iter != ss2.intervals_.end(); iter++) {
            // 	std::cout << "[" << iter->first << ", " << iter->second << ")" << std::endl;
            //}
        }

        TEST_F(HeuristicSSTest, getPrevString) {
            //HeuristicSS ss;
            //ASSERT_TRUE(ss.getPrevString("abc").compare("abb")==0);
            //ASSERT_TRUE(ss.getPrevString("abcc").compare("abcb")==0);
            //ASSERT_TRUE(ss.getPrevString("abc" + std::string(1,char(0))).compare("abc") == 0);
        }


        TEST_F(HeuristicSSTest, getInterval) {
            HeuristicSS ss;
            std::vector<SymbolFreq> symbol_freq_list;
            ss.selectSymbols(emails, 1000000, &symbol_freq_list);
            //std::sort(ss.intervals_.begin(), ss.intervals_.end());
            //for(auto iter = ss.intervals_.begin(); iter != ss.intervals_.end(); iter++) {
            //    std::cout << iter->first << "\t" << iter->second << std::endl;
            //}
            //std::string start_str = std::string(1, char(0));
            //std::string end_str = std::string(50, char(127));
            //ss.checkIntervals(start_str, end_str);
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
                int i = 0;
                for(; i < (int)key.size(); i++) {
                    if(key[i] <= 0) {
                        std::cout << key << std::endl;
                        break;
                    }
                }
                if (i != (int)key.size()) {
                    std::cout << "Continue here" << std::endl;
                    continue;
                }
                emails.push_back(key);
                count++;
            }
            std::sort(emails.begin(), emails.end());
        }

    } // namespace heuristictest

} // namespace ope

int main (int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    // ope::heuristicsstest::loadWords();
    ope::heuristicsstest::loadEmails();
    return RUN_ALL_TESTS();
}
