#ifndef SINGLE_CHAR_SS_H
#define SINGLE_CHAR_SS_H

#include "symbol_selector.hpp"

namespace ope {

class SingleCharSS : public SymbolSelector {
public:
    SingleCharSS();

    bool selectSymbols (const std::vector<std::string>& key_list,
                        const int64_t num_limit,
                        std::vector<SymbolFreq>* symbol_freq_list);

private:
    void countSymbolFreq (const std::vector<std::string>& key_list);

    int64_t freq_list_[256];
};

SingleCharSS::SingleCharSS() {
    for (int i = 0; i < 256; i++) {
        freq_list_[i] = 1;
    }
}

bool SingleCharSS::selectSymbols (const std::vector<std::string>& key_list,
                                  const int64_t num_limit,
                                  std::vector<SymbolFreq>* symbol_freq_list) {
    if (key_list.empty() || num_limit < 256)
        return false;
    countSymbolFreq(key_list);
    for (int i = 0; i < 256; i++) {
        symbol_freq_list->push_back(std::make_pair(std::string(1, (char)i), freq_list_[i]));
    }
    return true;
}

void SingleCharSS::countSymbolFreq (const std::vector<std::string>& key_list) {
    for (int i = 0; i < (int)key_list.size(); i++) {
        for (int j = 0; j < (int)key_list[i].length(); j++) {
            freq_list_[(uint8_t)key_list[i][j]]++;
        }
    }
}

} // namespace ope

#endif // SINGLE_CHAR_SS_H
