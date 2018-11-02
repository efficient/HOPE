#ifndef ARRAY_DICT_H
#define ARRAY_DICT_H

#include "dictionary.hpp"

namespace ope {

class ArrayDict : public  Dictionary {
public:
    ArrayDict() {};
    bool build (const std::vector<SymbolCode>& symbol_code_list);
    Code lookup (const std::string& symbol) const;
    int numEntries () const;
    int64_t memoryUse () const;

private:
    int binarySearch(const std::string& key);
    
    std::vector<SymbolCode> dict_;
};

bool ArrayDict::build (const std::vector<SymbolCode>& symbol_code_list) {
    for (int i = 0; i < (int)symbol_code_list.size(); i++) {
	dict_.push_back(symbol_code_list[i]);
    }
    return true;
}
    
Code ArrayDict::lookup (const std::string& symbol) const {
    int idx = binarySearch(symbol);
    return dict_[idx].second;
}
    
int ArrayDict::numEntries () const {
    return (int)dict_.size();
}
    
int64_t ArrayDict::memoryUse () const {
    return (sizeof(SymbolCode) * numEntries());
}

int ArrayDict::binarySearch(const std::string& key) {
    int l = 0;
    int r = dict_.size();
    int m = 0;
    while (r - l > 1) {
	m = (l + r) >> 1;
	std::string cur_key = dict_[m];
	int cmp = key.compare(cur_key);
	if (cmp < 0) {
	    r = m;
	} else if (cmp == 0) {
	    return m;
	} else {
	    l = m;
	}
    }
    return l;
}

} // namespace ope

#endif // ARRAY_DICT_H
