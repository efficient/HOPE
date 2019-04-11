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
        static const int kEmailTestSize = 20000;
        static std::vector<std::string> emails;


        class ARTDICTest : public ::testing::Test {
        private:
            static int strCompare(std::string s1, std::string s2) {
                int len1 = (int)s1.size();
                int len2 = (int)s2.size();
                int len = min(len1, len2);
                for(int i = 0; i < len ; i++) {
                    uint8_t c1 = static_cast<uint8_t >(s1[i]);
                    uint8_t c2 = static_cast<uint8_t >(s2[i]);
                    if (c1 < c2)
                        return -1;
                    if (c1 > c2)
                        return 1;
                }
                if (len1< len2)
                    return -1;
                else if (len1 == len2)
                    return 0;
                else
                    return 1;
            }
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
                                        int cur_idx, std::string& cur_str,
                                        int& next_idx, std::string& next_str) {
                for (int i = cur_idx; i < (int)sorted_intervals.size(); i++) {
                    if (strCompare(sorted_intervals[i], cur_str) > 0) {
                        next_idx = i - 1;
                        next_str = sorted_intervals[i-1];
                        return;
                    }
                }
                assert(false);
                __builtin_unreachable();
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
                std::string next_str = cur_str;
                int next_idx = i;
                getNextInterval(emails, i, cur_str, next_idx, next_str);
                ope::Code result;
                // next string <= cur_star
                result = test->lookup(cur_str.c_str(), cur_str.size(), prefix_len);


                if (result.code != next_idx) {
                    std::cout << "*" << next_idx << " " << result.code << " " << i << std::endl;
                    std::cout << emails[next_idx] << " " << emails[result.code] << " " << cur_str << std::endl;
                    int l;
                    auto org = test->lookup(emails[i].c_str(), emails[i].size(), l);
                    std::cout << org.code <<std::endl;
                }
                ASSERT_TRUE(result.code == next_idx);
//                if (cur_str.compare(next_str) < 0) {
//                    //ASSERT_TRUE(prefix_len == getCommonPrefixLen(emails[i], emails[i+1]));
//                    ASSERT_TRUE(result.code == i);
//                }
//                else {
//                    std::cout << cur_str << "\t" << next_str << std::endl;
//                    std::cout << result.code << "\t" << next_idx << std::endl;
//                    std::cout << emails[next_idx+1] << std::endl;
//                    ASSERT_TRUE(prefix_len == getCommonPrefixLen(emails[next_idx], emails[next_idx+1]));
//                    ASSERT_TRUE(result.code == next_idx);
//                }
            }
            delete test;
        }

        TEST_F(ARTDICTest, emailTest) {

        }

        TEST_F (ARTDICTest, insertTest) {

        }

        TEST_F (ARTDICTest, lookupTest) {

        }


        int getCommonPrefixLen(std::string &str1, std::string &str2) {
            int min_len = (int) std::min(str1.size(), str2.size());
            int i = 0;
            for (; i < min_len; i++) {
                if (str1[i] != str2[i])
                    return i;
            }
            return i;
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
