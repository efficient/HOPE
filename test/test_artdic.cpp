#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include "art_dic_tree.hpp"


namespace ope {

    namespace treetest {
        static const std::string kEmailFilePath = "../../datasets/emails.txt";
        static const int kEmailTestSize = 10000;
        static std::vector<std::string> emails;


        class ARTDICTest : public ::testing::Test {
        public:
            static std::string getNextString(std::string str) {
                for (int i = int(str.size() - 1); i >= 0; i--) {
                    if (uint8_t (str[i]) != 255) {
                        char next_chr = str[i] + 1;
                        return str.substr(0, i) + std::string(1, next_chr);
                    }
                }
                assert(false);
                __builtin_unreachable();
            }

            static void getNextInterval(std::vector<std::string> sorted_intervals,
                                        int cur_idx, std::string cur_str,
                                        int& next_idx, std::string& next_str) {
                for (int i = cur_idx; i < (int)sorted_intervals.size(); i++) {
                    if (sorted_intervals[i].compare(cur_str) > 0) {
                        next_idx = i - 1;
                        next_str = sorted_intervals[i-1];
                        return;
                    }
                }
                assert(false);
                __builtin_unreachable();
            }

            static int getCommonPrefixLen(std::string &str1, std::string &str2) {
                int min_len = (int) std::min(str1.size(), str2.size());
                int i = 0;
                for (; i < min_len; i++) {
                    if (str1[i] != str2[i])
                        return i;
                }
                return i;
            }
        };

        TEST_F (ARTDICTest, emptyTest) {

        }

        TEST_F(ARTDICTest, pointLookupTest) {
            ArtDicTree *test = new ArtDicTree();
            std::vector<ope::SymbolCode> ls;
            std::cout << "number of test emails:" << emails.size() << std::endl;
            std::sort(emails.begin(), emails.end());

            for (int i = 0; i < (int)emails.size(); i++) {
                ope::SymbolCode symbol_code = ope::SymbolCode();
                symbol_code.first = emails[i];
                symbol_code.second = ope::Code();
                symbol_code.second.code = i;
                ls.push_back(symbol_code);
            }

            test->build(ls);

            for (int i = 0; i < (int)emails.size() - 1; i++) {
                int prefix_len = -1;
                ope::Code result = test->lookup(emails[i].c_str(), emails[i].size(), prefix_len);
                //ASSERT_TRUE(prefix_len == getCommonPrefixLen(emails[i], emails[i+1]));
                ASSERT_TRUE(result.code == i);
            }
            delete test;
        }

        TEST_F(ARTDICTest, withinRangeLookupTest) {
            ArtDicTree *test = new ArtDicTree();
            std::vector<ope::SymbolCode> ls;
            std::cout << emails.size() << std::endl;
            std::sort(emails.begin(), emails.end());

            for (int i = 0; i < (int)emails.size(); i++) {
                ope::SymbolCode symbol_code = ope::SymbolCode();
                symbol_code.first = emails[i];
                symbol_code.second = ope::Code();
                symbol_code.second.code = i;
                ls.push_back(symbol_code);
            }

            test->build(ls);

            for (int i = 0; i < (int)emails.size() - 1; i++) {
                int prefix_len = -1;
                std::string cur_str = getNextString(emails[i]);
                std::string next_str;
                int next_idx = -1;
                getNextInterval(emails, i, cur_str, next_idx, next_str);
                ope::Code result;
                result = test->lookup(cur_str.c_str(), cur_str.size(), prefix_len);
                if (cur_str.compare(next_str) < 0) {
                    //ASSERT_TRUE(prefix_len == getCommonPrefixLen(emails[i], emails[i+1]));
                    ASSERT_TRUE(result.code == i);
                }
                else {
//                    std::cout << cur_str << "\t" << next_str << std::endl;
//                    std::cout << result.code << "\t" << next_idx << std::endl;
//                    std::cout << emails[next_idx+1] << std::endl;
//                    ASSERT_TRUE(prefix_len == getCommonPrefixLen(emails[next_idx], emails[next_idx+1]));
//                    ASSERT_TRUE(result.code == next_idx);
                }
            }
            delete test;
        }

        TEST_F(ARTDICTest, emailTest) {

        }

        TEST_F (ARTDICTest, insertTest) {

        }

        TEST_F (ARTDICTest, lookupTest) {

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

    } // namespace treetest

} // namespace ART_DIC

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);

    ope::treetest::loadEmails();

    return RUN_ALL_TESTS();
}
