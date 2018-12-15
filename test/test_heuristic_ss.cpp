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
		static const int kEmailTestSize = 1;
		static std::vector<std::string> emails;

		class HeuristicSSTest : public ::testing::Test {
		};

		TEST_F(HeuristicSSTest, getFrequencyTable1) {
			HeuristicSS* ss = new HeuristicSS();
			ss->getFrequencyTable(emails);
			std::cout << emails.size() << std::endl;
			std::cout << ss->freq_map_.size() << std::endl;
			for (auto it = ss->freq_map_.begin(); it != ss->freq_map_.end(); it++) {
				std::cout << it->first << "\t" << it->second << std::endl;
			}
		}

		TEST_F(HeuristicSSTest, fillGap) {
			
		}

		TEST_F(HeuristicSSTest, getInterval) {

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
				emails.push_back(key);
				count++;
			}
		}
	
	} // namespace heuristictest

} // namespace ope

int main (int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	// ope::heuristicsstest::loadWords();
	ope::heuristicsstest::loadEmails();
	return RUN_ALL_TESTS();
}