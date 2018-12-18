#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>

#include "Tree_ArtDic.h"
#include "symbol_selector_factory.hpp"
#include "code_generator_factory.hpp"
#include "double_char_encoder.hpp"

namespace ARTDIC {

    namespace treetest {
        static const std::string kEmailFilePath = "../../../datasets/emails.txt";
        static const int kEmailTestSize = 100;
        static std::vector<std::string> emails;


        class ARTDICTest : public ::testing::Test {

        };

        TEST_F (ARTDICTest, emptyTest) {

        }

        TEST_F (ARTDICTest, insertTest) {
            Tree *test = new Tree();
            std::vector<ope::SymbolCode*> ls = {};
            std::cout << emails.size() << std::endl;


            for (int i = 0; i < emails.size(); i++) {
                ope::SymbolCode* l = new ope::SymbolCode();
                l->first = emails[i];
                l->second = ope::Code();
                l->second.code = i;
                test->insert(*l);
                ls.push_back(l);
            }
            for (int i = 0; i < emails.size(); i++) {
                int prefix_len = -1;
                ope::Code result = test->lookup(emails[i].c_str(), emails[i].size(), prefix_len);
                ASSERT_TRUE(result.code == i);
            }
            for (int i = 0; i <ls.size(); i++) {
                delete ls[i];
            }
//            ope::SymbolCode l;
//            l.first = "1";
//            l.second = ope::Code();
//            l.second.code = 123;
//
//            test->insert(l);
//            int prefix_len = -1;
//            ope::Code result = test->lookup("1",1,prefix_len);
//            std::cout << result.code << std::endl;
//            ASSERT_TRUE(result.code == 123);
            delete test;
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

    ARTDIC::treetest::loadEmails();

    return RUN_ALL_TESTS();
}
