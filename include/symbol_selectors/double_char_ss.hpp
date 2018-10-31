#ifndef DOUBLE_CHAR_SS_H
#define DOUBLE_CHAR_SS_H

#include "symbol_selector.hpp"

namespace ope {

class DoubleCharSS : public SymbolSelector {
public:
    DoubleCharSS();

    bool selectSymbols (const std::vector<std::string>& key_list,
                        const int64_t num_limit,
                        std::vector<SymbolFreq>* symbol_freq_list);

private:
    void countSymbolFreq (const std::vector<std::string>& key_list);

    int64_t freq_list_[65536];
};

DoubleCharSS::DoubleCharSS() {
    for (int i = 0; i < 65536; i++) {
        freq_list_[i] = 1;
    }
}

bool DoubleCharSS::selectSymbols (const std::vector<std::string>& key_list,
                                  const int64_t num_limit,
                                  std::vector<SymbolFreq>* symbol_freq_list) {
    if (key_list.empty() || num_limit < 65536)
        return false;
    countSymbolFreq(key_list);
    for (int i = 0; i < 65536; i++) {
	std::string symbol;
	symbol += (char)(i / 256);
	symbol += (char)(i % 256);
        symbol_freq_list->push_back(std::make_pair(symbol, freq_list_[i]));
    }
    return true;
}

void DoubleCharSS::countSymbolFreq (const std::vector<std::string>& key_list) {
    for (int i = 0; i < (int)key_list.size(); i++) {
	int key_len = (int)key_list[i].length();
        for (int j = 0; j < key_len; j++) {
	    unsigned idx = 256 * (uint8_t)key_list[i][j];
	    if (j + 1 < key_len)
		idx += (uint8_t)key_list[i][j + 1];
            freq_list_[idx]++;
        }
    }
}

} // namespace ope

#endif // DOUBLE_CHAR_SS_H
