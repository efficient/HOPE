#ifndef ARRAY_4GRAM_DICT_H
#define ARRAY_4GRAM_DICT_H

#include "dictionary.hpp"

namespace ope {

class Array4GramDict : public  Dictionary {
public:
    Array4GramDict() {};
    ~Array4GramDict() { delete[] dict_; };
    bool build (const std::vector<SymbolCode>& symbol_code_list);
    Code lookup (const char* symbol, const int symbol_len, int& prefix_len) const;
    int numEntries () const;
    int64_t memoryUse () const;

private:
    int binarySearch(const char* symbol, const int symbol_len) const;
    int compareDictEntry(const char* s1, const int s1_len, const int dict_idx) const;
    
    int dict_size_;
    Interval4Gram* dict_;
};

bool Array4GramDict::build (const std::vector<SymbolCode>& symbol_code_list) {
    dict_size_ = (int)symbol_code_list.size();
    dict_ = new Interval4Gram[dict_size_];
    
    for (int i = 0; i < dict_size_; i++) {
	std::string symbol = symbol_code_list[i].first;
	int symbol_len = symbol.length();
	assert(symbol_len <= 4);
	for (int j = 0; j < 4; j++) {
	    if (j < symbol_len)
		(dict_[i].start_key)[j] = symbol[j];
	    else
		(dict_[i].start_key)[j] = 0;
	}
	if (i < dict_size_ - 1) {
	    dict_[i].common_prefix_len = 0;
	    std::string next_symbol = symbol_code_list[i + 1].first;
	    int next_symbol_len = next_symbol.length();
	    next_symbol[next_symbol_len - 1] -= 1;
	    int j = 0;
	    while (j < symbol_len && j < next_symbol_len
		   && symbol[j] == next_symbol[j]) {
		dict_[i].common_prefix_len++;
		j++;
	    }
	} else {
	    dict_[i].common_prefix_len = (uint8_t)symbol.length();
	}

	assert(dict_[i].common_prefix_len > 0);
	
	dict_[i].code.code = symbol_code_list[i].second.code;
	dict_[i].code.len = symbol_code_list[i].second.len;
    }
    return true;
}
    
Code Array4GramDict::lookup (const char* symbol, const int symbol_len,
			     int& prefix_len) const {
    int idx = binarySearch(symbol, symbol_len);
    prefix_len = dict_[idx].common_prefix_len;
    return dict_[idx].code;
}
    
int Array4GramDict::numEntries () const {
    return dict_size_;
}
    
int64_t Array4GramDict::memoryUse () const {
    return (sizeof(Interval4Gram) * dict_size_);
}

int Array4GramDict::binarySearch(const char* symbol, const int symbol_len) const {
    int l = 0;
    int r = dict_size_;
    int m = 0;
    while (r - l > 1) {
	m = (l + r) >> 1;
	int cmp = compareDictEntry(symbol, symbol_len, m);
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

int Array4GramDict::compareDictEntry(const char* s1, const int s1_len,
				     const int dict_idx) const {
    const char* dict_str = dict_[dict_idx].start_key;
    for (int i = 0; i < 4; i++) {
	if (i >= s1_len) {
	    if (dict_str[i] == 0)
		return 0;
	    else
		return -1;
	}
	if ((uint8_t)s1[i] < (uint8_t)dict_str[i])
	    return -1;
	if ((uint8_t)s1[i] > (uint8_t)dict_str[i])
	    return 1;
    }
    if (s1_len > 4) {
	return 1;
    } else {
	return 0;
    }
}

} // namespace ope

#endif // ARRAY_3GRAM_DICT_H
