#ifndef NGRAM_SS_H
#define NGRAM_SS_H

#include <map>

#include "symbol_selector.hpp"

namespace ope {

class NGramSS : public SymbolSelector {
public:
    NGramSS(int n) : n_(n) {};

    bool selectSymbols (const std::vector<std::string>& key_list,
                        const int64_t num_limit,
                        std::vector<SymbolFreq>* symbol_freq_list);

private:
    void countSymbolFreq (const std::vector<std::string>& key_list);

    int n_;
    std::map<std::string, int64_t> freq_map_;
};

bool NGramSS::selectSymbols (const std::vector<std::string>& key_list,
			     const int64_t num_limit,
			     std::vector<SymbolFreq>* symbol_freq_list) {
    if (key_list.empty())
        return false;
    countSymbolFreq(key_list);
    std::map<std::string, int64_t>::iterator iter;
    for (iter = freq_map_.begin(); iter != freq_map_.end(); ++iter) {
	symbol_freq_list->push_back(std::make_pair(iter->first, iter->second));
    }
    return true;
}

void NGramSS::countSymbolFreq (const std::vector<std::string>& key_list) {
    std::map<std::string, int64_t>::iterator iter;
    for (int i = 0; i < (int)key_list.size(); i++) {
        for (int j = 0; j < (int)key_list[i].length(); j++) {
	    std::string ngram = key_list[i].substr(j, n_);
	    iter = freq_map_.find(ngram);
	    if (iter == freq_map_.end())
		freq_map_.insert(std::pair<std::string, int64_t>(ngram, 1));
	    else
		iter->second += 1;
        }
    }
}

} // namespace ope

#endif // NGRAM_SS_H
