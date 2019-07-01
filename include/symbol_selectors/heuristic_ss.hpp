#ifndef HEURISTIC_SS_H
#define HEURISTIC_SS_H

#include <algorithm>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cmath>
#include "symbol_selector.hpp"
#include "blending_trie.hpp"

#define BLEND_FILE_NAME  "./blend_result_"
//#define WRITE_BLEND_RESULT 1 
#define PRINT_TIME_BREAKDOWN
#define MAX_KEY_LEN 50

namespace ope {

    class HeuristicSS : public SymbolSelector {
    public:
        HeuristicSS();

        ~HeuristicSS() {};

        bool selectSymbols(const std::vector<std::string> &key_list,
                           const int64_t num_limit,
                           std::vector<SymbolFreq> *symbol_freq_list);

        std::string getPrevString(const std::string &str);

        void setW(int64_t new_w);

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
        std::unordered_map<std::string, int64_t> freq_map_;
        std::vector<std::pair<std::string, std::string>> intervals_;

    };

    HeuristicSS::HeuristicSS(){
        W = 20000;
    }

    void HeuristicSS::setW(int64_t new_w) {
        W = new_w;
    }

    int strCompare(std::string s1, std::string s2) {
        int len1 = (int) s1.size();
        int len2 = (int) s2.size();
        int len = len1 < len2 ? len1 : len2;
        for (int i = 0; i < len; i++) {
            uint8_t c1 = static_cast<uint8_t >(s1[i]);
            uint8_t c2 = static_cast<uint8_t >(s2[i]);
            if (c1 < c2)
                return -1;
            if (c1 > c2)
                return 1;
        }
        if (len1 < len2)
            return -1;
        else if (len1 == len2)
            return 0;
        else
            return 1;
    }

    bool readIntervals(std::vector<SymbolFreq>& blend_freq_table, std::string key) {
#ifdef WRITE_BLEND_RESULT
        std::ifstream myfile(BLEND_FILE_NAME + key.substr(0,4));
        if (myfile.is_open()) {
            std::string line;
            while (getline(myfile, line)) {
                errno = 0;
                unsigned long idx = line.find('\t');
                std::string key = line.substr(0, idx);
                std::string freq_str = line.substr(idx+1, line.length());
                int64_t freq = std::atoi(freq_str.c_str());
                if (errno != 0) {
                    std::cout << "Fail to convert interval: " << freq_str << std::endl;
                } else {
                    blend_freq_table.emplace_back(key, freq);
                }
            }
            myfile.close();
            return true;
        }
#endif
        return false;
    }

    bool writeIntervals(std::vector<SymbolFreq>& blend_freq_table, std::string key){
        std::ofstream myfile(BLEND_FILE_NAME + key.substr(0,4));
        if (myfile.is_open()){
            for (int i = 0; i < (int)blend_freq_table.size(); i++) {
                myfile << blend_freq_table[i].first << "\t";
                myfile << blend_freq_table[i].second << std::endl;
            }
            myfile.close();
            return true;
        }
        std::cout << "Fail to write to Blend file***" << std::endl;
        return false;
    }

    bool HeuristicSS::selectSymbols(const std::vector<std::string> &key_list,
                                    const int64_t num_limit,
                                    std::vector<SymbolFreq> *symbol_freq_list) {
        if (key_list.empty())
            return false;

        double curtime = getNow();
        std::vector<SymbolFreq> blend_freq_table;
        if (readIntervals(blend_freq_table, key_list[0])) {
            std::cout << "Read blending results from file" << std::endl;
        } else {
            curtime = getNow();
            // Build Trie
            BlendTrie tree;
            tree.build(key_list);
#ifdef PRINT_TIME_BREAKDOWN
            std::cout << "Build trie = " << getNow() - curtime << std::endl;
#endif
            // Blending
            tree.blendingAndGetLeaves(blend_freq_table);
            // Write blending results  to file
#ifdef WRITE_BLEND_RESULT
            writeIntervals(blend_freq_table, key_list[0]);
#endif
        }
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Blending = " << getNow() - curtime << std::endl;
#endif
        // Search for best W
        int64_t last_w = 0;
        while (abs(num_limit - (int)intervals_.size()) > (int)(0.1 * num_limit)) {
            intervals_.clear();
            getInterval(blend_freq_table);
            // sort intervals
            std::sort(intervals_.begin(), intervals_.end(),
                      [](std::pair<std::string, std::string>& x, std::pair<std::string, std::string> y) {
                          return strCompare(x.first, y.first) < 0;
                      });
            // Merge adjacent intervals with same prefix
            mergeAdjacentComPrefixIntervals();
            // too many intervals
            if ((int) intervals_.size() > num_limit) {
                std::cout << "Interval number exceeds limit " << intervals_.size();
                std::cout << " Change new W to " << W * 2 << std::endl;
                last_w = W;
                setW(W * 2);
            } else  { // size < num_limit, W is too big
                std::cout << "Not enough intervals " << intervals_.size();
                int64_t new_w = W - 1 - (int64_t)(abs((W - last_w)/2.0 + 1));
                std::cout << " Change new W to " << new_w << std::endl;
                last_w = W;
                setW(new_w);
            }
        }
        std::string start = std::string(1, char(0));
        std::string end = std::string(MAX_KEY_LEN, char(255));
        checkIntervals(start, end);
        curtime = getNow();
        // simulate encode process to get Frequency
        getIntervalFreq(symbol_freq_list, key_list);
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Simulate encode process = " << getNow() - curtime << std::endl;
#endif
        curtime = getNow();
        return true;
    }


    int HeuristicSS::BinarySearch(const std::string& str, unsigned int pos, int &prefix_len) {
        int l = 0;
        int r = static_cast<int>(intervals_.size()) - 1;
        assert(pos <= str.size());
        std::string compare_str = str.substr(pos, str.size() - pos);
        while (l <= r) {
            int mid = (l+r)/2;
            std::string mid_left_bound = intervals_[mid].first;
            int cmp = strCompare(mid_left_bound, compare_str);
            if(cmp < 0) {
                l = mid + 1;
            } else if (cmp > 0){
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
            int key_len = (int)(key.size()<128?key.size():128);
            // Get substring frequency
            for (int j = 0; j < key_len; j++) {
                for (int k = 1; k <= key_len - j; k++) {
                    std::string substring = key.substr(j, k);

                    auto result = freq_map_.find(substring);
                    if (result == freq_map_.end()) {
                        freq_map_.insert(std::pair<std::string, int64_t>(substring, 1));
                    } else {
                        result->second++;
                    }
                }
            }
        }
    }

    void HeuristicSS::getInterval(std::vector<SymbolFreq> &blend_freq_table) {
        std::sort(blend_freq_table.begin(), blend_freq_table.end(),
                  [](const SymbolFreq &x, const SymbolFreq &y) {
                      return strCompare(x.first, y.first) < 0;
                  });
        auto next_start = blend_freq_table.begin();
        bool is_first_peak = (next_start->second * (int)(next_start->first.size()) > W);
        for (auto iter = blend_freq_table.begin(); iter != blend_freq_table.end(); iter++) {
            if (iter->second * (int)(iter->first.length()) > W) {
                intervals_.emplace_back(std::make_pair(iter->first, getNextString(iter->first)));
                mergeIntervals(next_start, iter);
                next_start = iter;
            }
        }

        auto blend_freq_table_end = blend_freq_table.end() - 1;
        mergeIntervals(next_start, blend_freq_table_end);
        std::string start_string = is_first_peak ? blend_freq_table.begin()->first
                                                          : getNextString(blend_freq_table.begin()->first);
        fillGap(std::string(1, char(0)), start_string);
        std::string end_string = next_start == blend_freq_table_end ? getNextString(blend_freq_table_end->first)
                                                                    : blend_freq_table_end->first;
        fillGap(end_string, std::string(MAX_KEY_LEN, char(255)));
    }

    void HeuristicSS::fillGap(std::string start_include, std::string end_exclude) {
        if (strCompare(start_include, end_exclude) >= 0)
            return;
        std::string com_prefix = commonPrefix(start_include, getPrevString(end_exclude));
        if (!com_prefix.empty()) {
            intervals_.emplace_back(std::make_pair(start_include, end_exclude));
            return;
        }
        uint8_t start_chr = static_cast<uint8_t >(start_include[0]);
        uint8_t end_chr = static_cast<uint8_t >(end_exclude[0]);
        for (int i = 0; i <= int(end_chr - start_chr); i++) {
            std::string cur_start = std::string(1, start_chr + i);
            std::string cur_end;
            if (int(start_chr) + i >= 255)
                cur_end = std::string(MAX_KEY_LEN, char(255));
            else
                cur_end = std::string(1, start_chr + i + 1);
            if (i == 0 && strCompare(cur_start, start_include) < 0)
                cur_start = start_include;
            if (i == end_chr - start_chr && strCompare(cur_end, end_exclude) > 0)
                cur_end = end_exclude;
            if (strCompare(cur_start, cur_end) < 0) {
                intervals_.emplace_back(std::make_pair(cur_start, cur_end));
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

        for (auto iter = start_exclude_iter + 1; iter != end_exclude_iter; iter++) {
            std::string cur_key = iter->first;
            cnt += iter->second;
            if (!has_prefix) {
                prefix = cur_key;
                has_prefix = true;
            } else {
                prefix = commonPrefix(prefix, cur_key);
            }
            if ((int)prefix.length() * cnt > W) {
                std::string cur_start = max(prefix, next_start);
                std::string cur_end = min(getNextString(prefix), (iter + 1)->first);
                intervals_.emplace_back(cur_start, cur_end);
                fillGap(next_start, cur_start);
                next_start = cur_end;
                prefix.clear();
                cnt = 0;
                has_prefix = false;
            }
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
                uint8_t prev_chr = static_cast<uint8_t>(str[i] - 1);
                addchr = std::string(1, char(prev_chr));
            } else
                addchr = std::string(1, str[i]);
            return str.substr(0, i) + addchr;
        }
        assert(false);
        return std::string();
    }

    std::string HeuristicSS::getNextString(const std::string &str) {
        assert(str.length() > 0);
        for (int i = int(str.length() - 1); i >= 0; i--) {
            if (uint8_t (str[i]) != 255) {
                char next_chr = str[i] + 1;
                return str.substr(0, i) + std::string(1, next_chr);
            }
        }
        return std::string(MAX_KEY_LEN, 255);
    }

void printString(std::string str) {
    for (int i = 0; i < (int)str.length();i++)
        std::cout << std::hex << (int)str[i] << " ";
}

    void HeuristicSS::checkIntervals(std::string& start_str, std::string& end_str) {
        if (strCompare(start_str, end_str) >= 0)
            assert(false);
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const std::pair<std::string, std::string> &x, const std::pair<std::string, std::string> &y) {
                      return strCompare(x.first, y.first) < 0;
                  });
        std::string end = std::string();
        int64_t cnt = 0;
        for (auto iter = intervals_.begin(); iter != intervals_.end(); iter++) {
            if (strCompare(iter->first, start_str) < 0)
                continue;
            if (strCompare(iter->first, end_str) > 0) {
                std::cout << "Check " << cnt << " intervals" << std::endl;
                return;
            }
            cnt++;
            if (strCompare(iter->first, iter->second) >= 0) {
                std::cout << "[Error] Start boundary : ";
                printString(iter->first);
                std::cout << std::endl << " End boundary : ";
                printString(iter->second);
                std::cout << std::endl;
                assert(false);
            }
            if ((int)end.size() > 0 && strCompare(end, iter->first) != 0) {
                std::cout << "[Error] intervals not connected," << std::endl
                          << " Current interval : ";
                printString(iter->first);
                std::cout << " ";
                printString(iter->second);
                std::cout << std::endl
                          << " Correct start :  ";
                printString(end);
                std::cout << std::endl;
                auto nit = iter - 5;
                for (; nit != iter + 5 && nit != intervals_.end(); nit++) {
                    printString(nit->first);
                    std::cout << "|";
                    printString(nit->second);
                    std::cout << std::endl;
                }
		        assert(false);
            }
            end = iter->second;
        }
        std::cout << "Check " << cnt << " intervals" << std::endl;
    }

    void HeuristicSS::mergeAdjacentComPrefixIntervals() {
        std::vector<std::pair<std::string, std::string>>merged_intervals;
        merged_intervals.push_back(std::make_pair(intervals_.begin()->first, intervals_.begin()->second));
        std::string last_prefix = std::string();
        for (auto iter = intervals_.begin() + 1; iter != intervals_.end(); iter++) {
            auto last_interval = merged_intervals.rbegin();
	        std::string last_key = last_interval->first;
            last_prefix = commonPrefix(last_interval->first, getPrevString(last_interval->second));
            std::string cur_prefix = commonPrefix(iter->first, getPrevString(iter->second));
            if (last_prefix == cur_prefix) {
                merged_intervals.pop_back();
                merged_intervals.emplace_back(last_key, iter->second);
            } else {
                merged_intervals.emplace_back(iter->first, iter->second);
            }
        }
        intervals_.clear();
        intervals_.insert(intervals_.begin(), merged_intervals.begin(), merged_intervals.end());
    }

} // namespace ope

#endif // HEURISTIC_SS_H
