#ifndef HEURISTIC_SS_H
#define HEURISTIC_SS_H

#include <algorithm>
#include <map>
#include <fstream>
#include <iostream>
#include "symbol_selector.hpp"
#include "blending_trie.hpp"

namespace ope {

    class HeuristicSS : public SymbolSelector {
    public:
        HeuristicSS();

        ~HeuristicSS() {};

        bool selectSymbols(const std::vector<std::string> &key_list,
                           const int64_t num_limit,
                           std::vector<SymbolFreq> *symbol_freq_list);

        std::string getPrevString(const std::string &str);

    private:
        void checkIntervals(std::string& start_str, std::string& end_str);

        void getFrequencyTable(const std::vector<std::string> &key_list);

        void getInterval(std::vector<SymbolFreq> &blend_freq_table);

        void mergeAdjacentComPrefixIntervals();

        void mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter,
                            std::vector<SymbolFreq>::iterator end_exclude_iter);


        void fillGap(std::string start_exclude, std::string end_exclude);

        void getIntervalFreq(std::vector<SymbolFreq> *symbol_freq_list,
                                const std::vector<std::string> &key_list);

        std::string commonPrefix(const std::string &str1, const std::string &str2);

        std::string getNextString(const std::string &str);


        void encode(const std::string& str, std::vector<int>& cnt);

        int BinarySearch(const std::string& str, unsigned int pos, int &prefix_len);

        int64_t W;
        std::map<std::string, int64_t> freq_map_;
        std::vector<std::pair<std::string, std::string>> intervals_;

    };

    HeuristicSS::HeuristicSS() {
        W = 200;
    }

    bool HeuristicSS::selectSymbols(const std::vector<std::string> &key_list,
                                    const int64_t num_limit,
                                    std::vector<SymbolFreq> *symbol_freq_list) {
        if (key_list.empty())
            return false;
        getFrequencyTable(key_list);
        // Build Trie
        BlendTrie tree;
        tree.build(freq_map_);
        // Blending
        std::vector<SymbolFreq> blend_freq_table;
        tree.blendingAndGetLeaves(blend_freq_table);
        getInterval(blend_freq_table);
        // too many intervals
        if ((int)intervals_.size() > num_limit) {
            std::cout << "Interval number exceeds limit" << std::endl;
            assert(false);
        }
        // sort intervals
        std::sort(intervals_.begin(), intervals_.end(),
                [](std::pair<std::string, std::string>& x, std::pair<std::string, std::string> y) {
                    return x.first.compare(y.first) < 0;
                });
        // Merge adjacent intervals with same prefix
        mergeAdjacentComPrefixIntervals();
        std::string start = std::string(1, char(0));
        std::string end = std::string(50, char(127));
        checkIntervals(start, end);
        // simulate encode process to get Frequency
        getIntervalFreq(symbol_freq_list, key_list);
        return true;
    }

    int HeuristicSS::BinarySearch(const std::string& str, unsigned int pos, int &prefix_len) {
        int l = 0;
        int r = static_cast<int>(intervals_.size());
        assert(pos <= str.size());
        std::string compare_str = str.substr(pos, str.size() - pos);
        while (l <= r) {
            int mid = (l+r)/2;
            std::string mid_left_bound = intervals_[mid].first;
            if(mid_left_bound.compare(compare_str) < 0) {
                l = mid + 1;
            } else if (mid_left_bound.compare(compare_str) > 0){
                r = mid - 1;
            } else {
                r = mid;
                break;
            }
        }
        assert(r < (int)intervals_.size());
        prefix_len = static_cast<int>(commonPrefix(intervals_[r].first, getPrevString(intervals_[r].second)).size());
        return r;
    }

    void HeuristicSS::encode(const std::string &str, std::vector<int>& cnt) {
        unsigned int pos = 0;
        int prefix_len = 0;
        int interval_idx;
        while (pos < str.size()) {
            interval_idx = BinarySearch(str, pos, prefix_len);
            cnt[interval_idx]++;
            assert(prefix_len > 0);
            pos += prefix_len;
        }
    }

    void HeuristicSS::getIntervalFreq(std::vector<SymbolFreq> *symbol_freq_list,
                                        const std::vector<std::string> &key_list) {
        std::vector<int> cnt(intervals_.size(), 0);
        for (const auto &iter : key_list) {
            encode(iter, cnt);
        }

        for (int i = 0; i < (int)intervals_.size(); i++) {
            // plus one for each interval to avoid 0 frequency
            symbol_freq_list->push_back(std::make_pair(intervals_[i].first, cnt[i] + 1));
        }
    }

    void HeuristicSS::getFrequencyTable(const std::vector<std::string> &key_list) {

        for (int i = 0; i < (int) key_list.size(); i++) {
            const std::string key = key_list[i];
            int key_len = (int)key.size();
            // Get substring frequency
            for (int j = 0; j < key_len; j++) {
                for (int k = 1; k <= key_len - j; k++) {
                    std::string substring = key.substr(j, k);
                    if (freq_map_.find(substring) == freq_map_.end())
                        freq_map_.insert(std::pair<std::string, int64_t>(substring, 1));
                    else
                        // TODO
                        // clean code
                        freq_map_.find(substring)->second++;
                }
            }
        }
    }

    void HeuristicSS::getInterval(std::vector<SymbolFreq> &blend_freq_table) {
        std::sort(blend_freq_table.begin(), blend_freq_table.end(),
                  [](const SymbolFreq &x, const SymbolFreq &y) {
                      return (x.first.compare(y.first) < 0);
                  });
        auto next_start = blend_freq_table.begin();
        bool not_first_peak = true;
        for (auto iter = blend_freq_table.begin(); iter != blend_freq_table.end(); iter++) {
            if (iter->second * (int)(iter->first.size()) > W) {
                if (not_first_peak) {
                    not_first_peak = (iter == blend_freq_table.begin());
                }
                intervals_.emplace_back(iter->first, getNextString(iter->first));
                mergeIntervals(next_start, iter);
                next_start = iter;
            }
        }
        auto blend_freq_table_end = blend_freq_table.end() - 1;
        mergeIntervals(next_start, blend_freq_table_end);
        std::string start_string = not_first_peak ? blend_freq_table.begin()->first
                                                          : getNextString(blend_freq_table.begin()->first);
        fillGap(std::string(1, char(0)), start_string);
        std::string end_string = next_start == blend_freq_table_end ? getNextString(blend_freq_table_end->first)
                                                                    : blend_freq_table_end->first;
        fillGap(end_string, std::string(50, char(127)));
    }

    void HeuristicSS::fillGap(std::string start_include, std::string end_exclude) {
        if (start_include.compare(end_exclude) >= 0)
            return;
        std::string com_prefix = commonPrefix(start_include, getPrevString(end_exclude));
        if (!com_prefix.empty()) {
            intervals_.push_back(std::make_pair(start_include, end_exclude));
            return;
        }
        char start_chr = start_include[0];
        char end_chr = end_exclude[0];
        for (int i = 0; i <= int(end_chr - start_chr); i++) {
            std::string cur_start = std::string(1, start_chr + i);
            std::string cur_end = std::string(1, start_chr + i + 1);
            if (i == 0 && cur_start.compare(start_include) < 0)
                cur_start = start_include;
            if (i == end_chr - start_chr && cur_end.compare(end_exclude) > 0)
                cur_end = end_exclude;
            if (cur_start.compare(cur_end) < 0) {
                intervals_.push_back(std::make_pair(cur_start, cur_end));
            }
        }
    }

    void HeuristicSS::mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter,
                                     std::vector<SymbolFreq>::iterator end_exclude_iter) {
        if (end_exclude_iter - start_exclude_iter <= 0)
            return;
        bool has_prefix = false;
        std::string prefix = std::string();
        std::string next_start = getNextString(start_exclude_iter->first);
        int64_t cnt = 0;
        int interval_num = 0;
        for (auto iter = start_exclude_iter + 1; iter != end_exclude_iter; iter++) {
            std::string cur_key = iter->first;
            cnt += iter->second;
            if (!has_prefix) {
                prefix = cur_key;
                has_prefix = true;
            } else
                prefix = commonPrefix(prefix, cur_key);
            if ((int)prefix.size() * cnt > W) {
                std::string cur_start = max(prefix, next_start);
                std::string cur_end = min(getNextString(prefix), (iter + 1)->first);
                intervals_.push_back(std::make_pair(cur_start, cur_end));
                fillGap(next_start, cur_start);
                next_start = cur_end;
                prefix.clear();
                cnt = 0;
                has_prefix = false;
            }
            interval_num += 1;
        }
        fillGap(next_start, end_exclude_iter->first);
    }

    std::string HeuristicSS::commonPrefix(const std::string &str1,
                                          const std::string &str2) {
        int min_len = (int)std::min(str1.size(), str2.size());
        int i = 0;
        for (; i < min_len; i++) {
            if (str1[i] != str2[i])
                return str1.substr(0, i);
        }
        return str1.substr(0, i);
    }

    std::string HeuristicSS::getPrevString(const std::string &str) {
        bool end_with_startchr = false;
        for (int i = (int)str.size() - 1; i >= 0; i--) {
            if (uint8_t (str[i]) == 0) {
                end_with_startchr = true;
                continue;
            }
            std::string addchr;
            if (!end_with_startchr) {
                char prev_chr = static_cast<char>(str[i] - 1);
                addchr = std::string(1, prev_chr);
            } else
                addchr = std::string(1, str[i]);
            return str.substr(0, i) + addchr;
        }
        assert(false);
        return std::string();
    }

    std::string HeuristicSS::getNextString(const std::string &str) {
        for (int i = int(str.size() - 1); i >= 0; i--) {
            if (uint8_t (str[i]) != 127) {
                char next_chr = str[i] + 1;
                return str.substr(0, i) + std::string(1, next_chr);
            }
        }
        assert(false);
        return std::string();
    }

    void HeuristicSS::checkIntervals(std::string& start_str, std::string& end_str) {
        if (start_str.compare(end_str) >= 0)
            assert(false);
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const std::pair<std::string, std::string> &x, const std::pair<std::string, std::string> &y) {
                      return x.first.compare(y.first) < 0;
                  });
        std::string end = std::string();
        int64_t cnt = 0;
        for (auto iter = intervals_.begin(); iter != intervals_.end(); iter++) {
            if (iter->first.compare(start_str) < 0)
                continue;
            if (iter->first.compare(end_str) > 0) {
                std::cout << "Check " << cnt << " intervals" << std::endl;
                return;
            }
            cnt++;
            if (iter->first.compare(iter->second) >= 0) {
                std::cout << "[Error] Start boundary : " << iter->first
                          << " End boundary : " << iter->second << std::endl;
                assert(false);
            }
            if (end.size() > 0 && end.compare(iter->first) != 0) {
                std::cout << "[Error] intervals not connected," << std::endl
                          << " Current interval : " << iter->first << " " << iter->second << std::endl
                          << " Correct start :  " << end << std::endl;
               // assert(false);
            }
            end = iter->second;

            if (cnt >= 15000 && cnt <= 15010) {
                std::cout << iter->first << " " << iter->second << std::endl;
            }
        }
        std::cout << "Check " << cnt << " intervals" << std::endl;
    }


    void HeuristicSS::mergeAdjacentComPrefixIntervals() {
        std::vector<std::pair<std::string, std::string>>merged_intervals;
        merged_intervals.push_back(std::make_pair(intervals_.begin()->first, intervals_.begin()->second));
        std::string last_prefix = std::string();
        for (auto iter = intervals_.begin() + 1; iter != intervals_.end(); iter++) {
            auto last_interval = merged_intervals.rbegin();
            last_prefix = commonPrefix(last_interval->first, getPrevString(last_interval->second));
            std::string cur_prefix = commonPrefix(iter->first, getPrevString(iter->second));
            if (last_prefix == cur_prefix) {
                merged_intervals.pop_back();
                merged_intervals.push_back(std::make_pair(last_interval->first, iter->second));
            } else {
                merged_intervals.push_back(std::make_pair(iter->first, iter->second));
            }
        }
        intervals_.clear();
        intervals_.insert(intervals_.begin(), merged_intervals.begin(), merged_intervals.end());
    }

} // namespace ope

#endif // HEURISTIC_SS_H
