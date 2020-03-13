#ifndef NGRAM_SS_H
#define NGRAM_SS_H

#include <algorithm>
#include <map>
#include "symbol_selector.hpp"

namespace ope {

class NGramSS : public SymbolSelector {
 public:
  NGramSS(int n) : n_(n){};
  ~NGramSS() { freq_map_.clear(); };

  bool selectSymbols(const std::vector<std::string> &key_list, const int64_t num_limit,
                     std::vector<SymbolFreq> *symbol_freq_list, int W = 10000);

 private:
  void countSymbolFreq(const std::vector<std::string> &key_list);
  void pickMostFreqSymbols(const int64_t num_limit, std::vector<std::string> *most_freq_symbols);
  void fillInGap(const std::vector<std::string> &most_freq_symbols);
  void fillInSingleChar(const int first, const int last);
  std::string commonPrefix(const std::string &str1, const std::string &str2);
  void countIntervalFreq(const std::vector<std::string> &key_list);
  int binarySearch(const std::string &key);

  int n_;
  std::map<std::string, int64_t> freq_map_;
  std::vector<std::string> interval_prefixes_;
  std::vector<std::string> interval_boundaries_;
  std::vector<int64_t> interval_freqs_;
};

bool NGramSS::selectSymbols(const std::vector<std::string> &key_list, const int64_t num_limit,
                            std::vector<SymbolFreq> *symbol_freq_list, int W) {
  if (key_list.empty()) return false;
  countSymbolFreq(key_list);
  std::vector<std::string> most_freq_symbols;
  int64_t adjust_num_limit = num_limit;
  if (num_limit > (int64_t)freq_map_.size() * 2) {
    adjust_num_limit = (int64_t)freq_map_.size() * 2 - 1;
    std::cout << "3 Gram: Input dictionary Size is too big, change to " << adjust_num_limit << std::endl;
  }
  pickMostFreqSymbols((adjust_num_limit / 2), &most_freq_symbols);
  fillInGap(most_freq_symbols);
  assert(interval_prefixes_.size() == interval_boundaries_.size());
  countIntervalFreq(key_list);
  assert(interval_prefixes_.size() == interval_freqs_.size());
  for (int i = 0; i < (int)interval_boundaries_.size(); i++) {
    symbol_freq_list->push_back(std::make_pair(interval_boundaries_[i], interval_freqs_[i]));
  }
  return true;
}

void NGramSS::countSymbolFreq(const std::vector<std::string> &key_list) {
  freq_map_.clear();
  //    std::unordered_map<std::string, int64_t>::iterator iter;
  std::map<std::string, int64_t>::iterator iter;
  for (int i = 0; i < (int)key_list.size(); i++) {
    for (int j = 0; j < (int)key_list[i].length() - n_ + 1; j++) {
      std::string ngram = key_list[i].substr(j, n_);
      iter = freq_map_.find(ngram);
      if (iter == freq_map_.end()) {
        freq_map_.insert(std::pair<std::string, int64_t>(ngram, 1));
      } else {
        iter->second += 1;
      }
    }
  }
}

void NGramSS::pickMostFreqSymbols(const int64_t num_limit, std::vector<std::string> *most_freq_symbols) {
  std::vector<SymbolFreq> symbol_freqs;
  std::map<std::string, int64_t>::iterator iter;
  for (iter = freq_map_.begin(); iter != freq_map_.end(); ++iter) {
    symbol_freqs.push_back(std::make_pair(iter->first, iter->second));
  }
  std::sort(symbol_freqs.begin(), symbol_freqs.end(), [](const SymbolFreq &x, const SymbolFreq &y) {
    if (x.second != y.second) return x.second > y.second;
    return (x.first.compare(y.first) > 0);
  });
  for (int i = 0; i < num_limit; i++) {
    most_freq_symbols->push_back(symbol_freqs[i].first);
  }
  std::sort(most_freq_symbols->begin(), most_freq_symbols->end());
}

void NGramSS::fillInGap(const std::vector<std::string> &most_freq_symbols) {
  fillInSingleChar(0, (int)most_freq_symbols[0][0]);

  int num_symbols = most_freq_symbols.size();
  for (int i = 0; i < num_symbols - 1; i++) {
    std::string str1 = most_freq_symbols[i];
    std::string str2 = most_freq_symbols[i + 1];
    interval_prefixes_.push_back(str1);
    interval_boundaries_.push_back(str1);

    std::string str1_right_bound = str1;
    for (int j = n_ - 1; j >= 0; j--) {
      if ((int)(uint8_t)str1_right_bound[j] < 255) {
        str1_right_bound[j] += 1;
        str1_right_bound = str1_right_bound.substr(0, j + 1);
        break;
      }
    }
    if (str1_right_bound.compare(str2) != 0) {
      interval_boundaries_.push_back(str1_right_bound);
      if (str1_right_bound[0] != str2[0]) {
        interval_prefixes_.push_back(std::string(1, str1[0]));
        fillInSingleChar((int)(uint8_t)(str1[0] + 1), (int)(uint8_t)str2[0]);
      } else {
        std::string common_str;
        if (str1[0] != str1_right_bound[0])
          common_str = str1_right_bound;
        else
          common_str = commonPrefix(str1, str2);
        assert(common_str.size() > 0);
        interval_prefixes_.push_back(common_str);
      }
    }
  }

  std::string last_str = most_freq_symbols[num_symbols - 1];
  interval_prefixes_.push_back(last_str);
  interval_boundaries_.push_back(last_str);
  std::string last_str_right_bound = last_str;
  for (int j = n_ - 1; j >= 0; j--) {
    if ((int)(uint8_t)last_str_right_bound[j] < 255) {
      last_str_right_bound[j] += 1;
      last_str_right_bound = last_str_right_bound.substr(0, j + 1);
      break;
    }
  }
  interval_boundaries_.push_back(last_str_right_bound);
  interval_prefixes_.push_back(std::string(1, last_str[0]));

  if ((int)(uint8_t)last_str[0] < 255) fillInSingleChar((int)(uint8_t)(last_str[0] + 1), 255);
}

void NGramSS::fillInSingleChar(const int first, const int last) {
  for (int c = first; c <= last; c++) {
    interval_prefixes_.push_back(std::string(1, (char)c));
    interval_boundaries_.push_back(std::string(1, (char)c));
  }
}

std::string NGramSS::commonPrefix(const std::string &str1, const std::string &str2) {
  if (str1[0] != str2[0]) return std::string();
  for (int i = 1; i < n_; i++) {
    if ((int)str1.size() < i) return str1.substr(0, i);
    if ((int)str2.size() < i) return str2.substr(0, i);
    if (str1[i] != str2[i]) return str1.substr(0, i);
  }
  assert(false);
  return std::string();
}

void NGramSS::countIntervalFreq(const std::vector<std::string> &key_list) {
  for (int i = 0; i < (int)interval_prefixes_.size(); i++) {
    interval_freqs_.push_back(1);
  }
  for (int i = 0; i < (int)key_list.size(); i++) {
    int pos = 0;
    while (pos < (int)key_list[i].length()) {
      std::string cur_str = key_list[i].substr(pos, n_ + 1);
      int idx = binarySearch(cur_str);
      interval_freqs_[idx]++;
      pos += (int)interval_prefixes_[idx].length();
    }
  }
}

int NGramSS::binarySearch(const std::string &key) {
  int l = 0;
  int r = interval_boundaries_.size();
  int m = 0;
  while (r - l > 1) {
    m = (l + r) >> 1;
    std::string cur_key = interval_boundaries_[m];
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

}  // namespace ope

#endif  // NGRAM_SS_H
