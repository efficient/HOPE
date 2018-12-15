#ifndef HEURISTIC_SS_H
#define HEURISTIC_SS_H

#include <map>
#include "symbol_selector.hpp"
#include "blending_trie.hpp"

namespace ope {

class HeuristicSS : public SymbolSelector {
public:
	HeuristicSS();
	~HeuristicSS() {};

	bool selectSymbols (const std::vector<std::string>& key_list,
		const int64_t num_limit,
		std::vector<SymbolFreq>* symbol_freq_list);


// private:
	void checkIntervals(std::string start_str, std::string end_str);
	void getFrequencyTable(const std::vector<std::string>& key_list);
	void getInterval(std::vector<SymbolFreq>& blend_freq_table);
	void mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter, 
					std::vector<SymbolFreq>::iterator end_exclude_iter);
	void fillGap(std::string start_exclude, std::string end_exclude);
	std::string commonPrefix (const std::string& str1, const std::string& str2);
	std::string getNextString(const std::string& str);

	int64_t W;
	std::map<std::string, int64_t> freq_map_;
	std::vector<std::pair<std::string, std::string>> intervals_;

};

HeuristicSS::HeuristicSS() {
	W = 100;
}

bool HeuristicSS::selectSymbols (const std::vector<std::string>& key_list,
                        const int64_t num_limit,
                        std::vector<SymbolFreq>* symbol_freq_list) {
	if (key_list.empty())
		return false;
	getFrequencyTable(key_list);
	// Build Trie
	BlendTrie *tree = new BlendTrie();
	tree->build(freq_map_);
	// Blending
	std::vector<SymbolFreq> blend_freq_table;
	tree->blendingAndGetLeaves(blend_freq_table);
	getInterval(blend_freq_table);
	delete tree;
	return true;
}

void HeuristicSS::getFrequencyTable(const std::vector<std::string>& key_list) {
	std::map<std::string, int64_t>::iterator iter;
	for (int i = 0; i < (int)key_list.size(); i++) {
		const std::string key = key_list[i];
		int key_len = key.size();
		// Get substring frequency
		for (int j = 0; j < key_len; j++) {
			for (int k = 1; k <= key_len - j; k++) {
				std::string substring = key.substr(j, k);
				iter = freq_map_.find(substring);
				if (iter == freq_map_.end())
					freq_map_.insert(std::pair<std::string, int64_t>(substring, 1));
				else
					iter->second += 1;
			}
		}
	}
}

void HeuristicSS::getInterval(std::vector<SymbolFreq>& blend_freq_table) {
	std::vector<SymbolFreq>::iterator next_start;
	std::sort(blend_freq_table.begin(), blend_freq_table.end(),
		[](const SymbolFreq& x, const SymbolFreq& y) {
			return (x.first.compare(y.first) > 0);
		});
	for (auto iter = blend_freq_table.begin(); iter != blend_freq_table.end(); iter++) {
		if (iter->second > W) {
			intervals_.push_back(std::make_pair(iter->first, getNextString(iter->first)));
			mergeIntervals(next_start, iter);
			next_start = iter;
		}
	}
	mergeIntervals(next_start, --blend_freq_table.end());
	fillGap(next_start->first, std::string(50, char(255)));
}

void HeuristicSS::fillGap(std::string start_include, std::string end_exclude) {
	if (start_include.compare(end_exclude) >= 0)
		return;
	std::string com_prefix = commonPrefix (start_include, end_exclude);
	if (com_prefix.size() > 0)
		intervals_.push_back(std::make_pair(start_include, end_exclude));
	char start_chr = start_include[0];
	char end_chr = end_exclude[0];
	for (int i = 0; i <= end_chr - start_chr; i++) {
		std::string cur_start = std::string(1, start_chr + i);
		std::string cur_end = std::string(1, start_chr + i + 1);
		if (i == 0 && cur_start.compare(start_include) < 0 )
			cur_start = start_include;
		if (i == end_chr - start_chr && cur_end.compare(end_exclude) > 0)
			cur_end = end_exclude;
		intervals_.push_back(std::make_pair(cur_start, cur_end));
	}
}

void HeuristicSS::mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter, 
					std::vector<SymbolFreq>::iterator end_exclude_iter){
	std::string prefix = std::string();
	std::string next_start;
	int64_t cnt = 0;
	for (auto iter = start_exclude_iter; iter != end_exclude_iter; iter++) {
		std::string cur_key = iter->first;
		if(iter == start_exclude_iter) {
			next_start = getNextString(cur_key);
			continue;
		}
		cnt += iter->second;
		if (prefix.size() == 0)
			prefix = cur_key;
		else
			prefix = commonPrefix(prefix, cur_key);
		if (prefix.size() * cnt >= W) {
			std::string cur_start = max(prefix, next_start);
			std::string cur_end = min(getNextString(prefix), (iter+1)->first);
			fillGap(next_start, cur_start);
			next_start = cur_start;
			prefix.clear();
			cnt = 0;
		}
	}
	fillGap(next_start, end_exclude_iter->first);
} 

std::string HeuristicSS::commonPrefix(const std::string& str1,
				  const std::string& str2) {
    if (str1[0] != str2[0])
    	return std::string();
    int min_len = std::min(str1.size(), str2.size());
    for (int i = 1; i < min_len; i++) {
		if (str1[i] != str2[i])
	    	return str1.substr(0, i);
    }
    assert(false);
    return std::string();
}

std::string HeuristicSS::getNextString(const std::string& str) {
	bool end_with_startchr = false;
	for (int i = str.size(); i >= 0; i--) {
		if (int(str[i]) == 0)
			end_with_startchr = true;
		else {
			std::string addchr;
			if (!end_with_startchr) {
				char prev_chr = str[i] - 1;
				addchr = std::string(1, prev_chr);
			}
			else
				addchr = std::string(1, str[i]);
			return str.substr(0, i) + addchr;
		}
	}
	assert(false);
	return std::string();
}

void HeuristicSS::checkIntervals(std::string start_str, std::string end_str) {
	if (start_str.compare(end_str) >= 0)
		assert(false);
	std::sort(intervals_.begin(), intervals_.end(),
		[](const std::pair<std::string, std::string>& x, const std::pair<std::string, std::string>& y) {
			return x.first.compare(y.first);
		});
	// int64_t cnt = 0;
	// for (auto iter = intervals_.begin(); iter != end; iter++) {

	// }
}

} // namespace ope

#endif // HEURISTIC_SS_H