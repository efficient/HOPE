#include "art_dic_N.hpp"
#include "gtest/gtest.h"

namespace ope {
namespace nodetest {

class ARTNodeTest : public ::testing::Test {
 public:
  bool uint8Cmp(uint8_t *str1, uint8_t *str2, uint32_t len) {
    for (int i = 0; i < static_cast<int>(len); i++) {
      if (str1[i] != str2[i]) {
        return false;
      }
    }
    return true;
  }
};

TEST_F(ARTNodeTest, prefixLengthTest) {
  uint32_t short_len = 5;
  uint32_t long_len = 100;
  NTypes node_type = NTypes::N4;
  uint8_t *short_prefix = new uint8_t[short_len];
  uint8_t *long_prefix = new uint8_t[long_len];
  for (int i = 0; i < static_cast<int>(short_len); i++) {
    short_prefix[i] = 0x1;
  }
  for (int i = 0; i < static_cast<int>(long_len); i++) {
    long_prefix[i] = 0x2;
  }
  N *n1 = new N(node_type, short_prefix, short_len);
  N *n2 = new N(node_type, long_prefix, long_len);
  EXPECT_TRUE(uint8Cmp(n1->getPrefix(), short_prefix, short_len));
  EXPECT_TRUE(uint8Cmp(n2->getPrefix(), long_prefix, long_len));
  delete n1;
  delete n2;
  delete[] short_prefix;
  delete[] long_prefix;
}

TEST_F(ARTNodeTest, prefixSetGetTest) {
  uint32_t short_len = 5;
  uint32_t long_len = 100;
  NTypes node_type = NTypes::N4;
  uint8_t *short_prefix = new uint8_t[short_len];
  uint8_t *long_prefix = new uint8_t[long_len];
  for (int i = 0; i < static_cast<int>(short_len); i++) {
    short_prefix[i] = 0x1;
  }
  for (int i = 0; i < static_cast<int>(long_len); i++) {
    long_prefix[i] = 0x2;
  }
  N *node = new N(node_type, short_prefix, short_len);
  EXPECT_EQ(node->prefix_len, short_len);
  EXPECT_TRUE(uint8Cmp(node->getPrefix(), short_prefix, short_len));
  node->setPrefix(long_prefix, long_len);
  EXPECT_EQ(node->prefix_len, long_len);
  EXPECT_TRUE(uint8Cmp(node->getPrefix(), long_prefix, long_len));

  delete node;
  delete[] short_prefix;
  delete[] long_prefix;
}

}  // namespace nodetest
}  // namespace ope

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}


