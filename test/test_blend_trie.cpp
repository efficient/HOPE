#include "gtest/gtest.h"

#include <assert.h>

#include <bitset>
#include <fstream>
#include <iostream>

#include "blending_trie.hpp"

namespace ope {

	namespace blendtrietest {

		class BlendTrieTest : public ::testing::Test {
		public:

		};

		TEST_F (BlendTrieTest, empty) {

		}

	} // namespace blendtrie

} // namespace ope

int main (int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}