#ifndef ALMImprovedSS_H
#define ALMImprovedSS_H

#include <algorithm>
#include <map>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <cmath>
#include <math.h>
#include "symbol_selector.hpp"
#include "blending_trie.hpp"

#define BLEND_FILE_NAME  "./blend_result_"
//#define WRITE_BLEND_RESULT 1
#define PRINT_TIME_BREAKDOWN
#define MAX_KEY_LEN 50

typedef struct SymbolFreqValid {
    std::string symbol;
    int64_t freq;
    bool valid;
} SymbolFreqValid;

typedef std::vector<SymbolFreqValid>::iterator freq_iter;

namespace ope {

    class ALMImprovedSS : public SymbolSelector {
    public:
        ALMImprovedSS();

        bool selectSymbols(const std::vector<std::string> &key_list,
                           const int64_t num_limit,
                           std::vector<SymbolFreq> *symbol_freq_list, int W = 10000);

        std::string getPrevString(const std::string &str);

        void setW(int64_t new_w);

        void checkIntervals(std::string& start_str, std::string& end_str);

    //private:
        void getSkewInterval(std::vector<SymbolFreq> &blend_freq_table);

        void getEqualInterval(std::vector<SymbolFreq> &blend_freq_table);

        void mergeAdjacentComPrefixIntervals();

        void mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter,
                            std::vector<SymbolFreq>::iterator end_exclude_iter);


        void fillGap(std::string start_exclude, std::string end_exclude);

        void getIntervalFreqEntropy(std::vector<SymbolFreq> *symbol_freq_list,
                                const std::vector<std::string> &key_list);

        std::string commonPrefix(const std::string &str1, const std::string &str2);

        std::string getNextString(const std::string &str);


        void encode(const std::string& str, std::vector<int>& cnt);

        int BinarySearch(const std::string& str, unsigned int pos, int &prefix_len);

        void growInterval(std::vector<SymbolFreqValid>::iterator iter, std::vector<SymbolFreqValid>& blend_freq_table);

        void generateConnectedIntervals();

        int64_t threeMAX(int64_t x, int64_t y, int64_t z);

        int64_t W;
    public:
        std::vector<std::pair<std::string, std::string>> intervals_;

    };

    ALMImprovedSS::ALMImprovedSS(){
        W = 20000;
    }

    void ALMImprovedSS::setW(int64_t new_w) {
        W = new_w;
    }


/*
    bool ALMImprovedSS::selectSymbols(const std::vector<std::string> &key_list,
                                    const int64_t num_limit,
                                    std::vector<SymbolFreq> *symbol_freq_list, int _W) {
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
        int64_t l = 0;
        int64_t r = _W * 2;
        while (l <= r && abs(num_limit - (int)intervals_.size()) > (int)(0.02 * num_limit)) {
            setW((l+r)/2);
            intervals_.clear();
            //std::cout << "W = " << _W << std::endl;
            getSkewInterval(blend_freq_table);
            // sort intervals
            std::sort(intervals_.begin(), intervals_.end(),
                      [](std::pair<std::string, std::string>& x, std::pair<std::string, std::string> y) {
                          return strCompare(x.first, y.first) < 0;
                      });
            // Merge adjacent intervals with same prefix
            mergeAdjacentComPrefixIntervals();

            if (abs(num_limit - (int)intervals_.size()) <= (int)(0.02 * num_limit)) {
        //        std::cout << "W = " << _W << "\tNumber of intervals = "<< intervals_.size() << "\tTarget = " << num_limit << std::endl;
                break;
            }
            // too many intervals
            if ((int) intervals_.size() > num_limit) {
                r = W - 1;
            } else  { // size < num_limit, W is too big
                l = W + 1;
            }
        }
        if (l > r)
            std::cout << "Best approoach W = " << W <<", target = " << num_limit << ", current size = " << intervals_.size() << std::endl;

        std::string start = std::string(1, char(0));
        std::string end = std::string(MAX_KEY_LEN, char(255));
        checkIntervals(start, end);
        curtime = getNow();
        // simulate encode process to get Frequency
        getIntervalFreqEntropy(symbol_freq_list, key_list);
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Simulate encode process = " << getNow() - curtime << std::endl;
#endif
        curtime = getNow();
        return true;
    }
*/

    bool ALMImprovedSS::selectSymbols(const std::vector<std::string> &key_list,
                                    const int64_t num_limit,
                                    std::vector<SymbolFreq> *symbol_freq_list, int _W) {
        if (key_list.empty())
            return false;
#ifdef PRINT_TIME_BREAKDOWN
        double curtime = getNow();
#endif

#ifdef PRINT_TIME_BREAKDOWN
        curtime = getNow();
#endif
        // Build Trie
        BlendTrie tree(1);
        tree.build(key_list);
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Build trie = " << getNow() - curtime << std::endl;
#endif
        std::vector<SymbolFreq> blend_freq_table;
        // Blending
        tree.blendingAndGetLeaves(blend_freq_table);
        // Write blending results  to file
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Blending = " << getNow() - curtime << std::endl;
#endif
        // Search for best W
        int64_t l = 0;
        int64_t r = _W * 2;
        while (l <= r && abs(num_limit - (int)intervals_.size()) > (int)(0.02 * num_limit)) {
            setW((l+r)/2);
            intervals_.clear();
            //std::cout << "W = " << _W << std::endl;
            getEqualInterval(blend_freq_table);
            // sort intervals
            std::sort(intervals_.begin(), intervals_.end(),
                      [](std::pair<std::string, std::string>& x, std::pair<std::string, std::string> y) {
                          return strCompare(x.first, y.first) < 0;
                      });
            // Merge adjacent intervals with same prefix
            mergeAdjacentComPrefixIntervals();

            if (abs(num_limit - (int)intervals_.size()) <= (int)(0.02 * num_limit)) {
        //        std::cout << "W = " << _W << "\tNumber of intervals = "<< intervals_.size() << "\tTarget = " << num_limit << std::endl;
                break;
            }
            // too many intervals
            if ((int) intervals_.size() > num_limit) {
                l = W + 1;
            } else  { // size < num_limit, W is too big
                r = W - 1;
            }
        }
        if (l > r)
            std::cout << "Best approoach W = " << _W <<", target = " << num_limit << ", current size = " << intervals_.size() << std::endl;
        std::string start = std::string(1, char(0));
        std::string end = std::string(MAX_KEY_LEN, char(255));
        checkIntervals(start, end);
#ifdef PRINT_TIME_BREAKDOWN
        curtime = getNow();
#endif
        // simulate encode process to get Frequency
        getIntervalFreqEntropy(symbol_freq_list, key_list);
#ifdef PRINT_TIME_BREAKDOWN
        std::cout << "Simulate encode process = " << getNow() - curtime << std::endl;
        curtime = getNow();
#endif
        return true;
    }

    int ALMImprovedSS::BinarySearch(const std::string& str, unsigned int pos, int &prefix_len) {
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

    void ALMImprovedSS::encode(const std::string &str, std::vector<int>& cnt) {
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

    void ALMImprovedSS::getIntervalFreqEntropy(std::vector<SymbolFreq> *symbol_freq_list,
                                        const std::vector<std::string> &key_list) {
        std::vector<int> cnt(intervals_.size(), 0);
        for (const auto &iter : key_list) {
            encode(iter, cnt);
        }

        std::vector<double> freq_len;
        double sum_fl = 0;

        for (int i = 0; i < (int)intervals_.size(); i++) {
            std::string interval_start = intervals_[i].first;
            std::string interval_end = intervals_[i].second;
            std::string common_prefix = commonPrefix(interval_start, getPrevString(interval_end));
            freq_len.push_back(common_prefix.length() * (cnt[i] + 1));
            sum_fl += common_prefix.length() * (cnt[i] + 1);
            // plus one for each interval to avoid 0 frequency
            // Pass Frequencty to Hu-Tucker
            symbol_freq_list->push_back(std::make_pair(interval_start, cnt[i] + 1));
            // Pass Frequencty * length to Hu-Tucker
            //symbol_freq_list->push_back(std::make_pair(interval_start, (cnt[i] + 1) * common_prefix.length()));
        }

        double entropy = 0;
        for (int i = 0; i < (int)freq_len.size(); i++) {
            double p = freq_len[i] / sum_fl;
            entropy += p * log2(p);
        }
        std::cout << "Entropy = " << -entropy << std::endl;
    }

    int64_t ALMImprovedSS::threeMAX(int64_t x, int64_t y, int64_t z) {
        return std::max(std::max(x,y),z);
    }

    void ALMImprovedSS::growInterval(std::vector<SymbolFreqValid>::iterator iter, std::vector<SymbolFreqValid>& blend_freq_table) {
        if (iter->valid == false)
            return;

        iter->valid = false;
        std::string interval_start = iter->symbol;
        std::string interval_end = getNextString(interval_start);

        while (true) {
            int64_t score =iter->symbol.length() * iter->freq;
            int64_t left_score = 0;
            int64_t right_score = 0;
            auto left_iter = iter - 1;
            auto right_iter = iter + 1;

            if (iter == blend_freq_table.begin() || left_iter->valid == false) {
                left_score = 0;
            } else {
                left_score = commonPrefix(left_iter->symbol, iter->symbol).length() * (iter->freq + left_iter->freq);
            }
            if (iter == blend_freq_table.end() - 1 || right_iter->valid == false) {
                right_score = 0;
            } else {
                right_score = commonPrefix(right_iter->symbol, iter->symbol).length() * (iter->freq + right_iter->freq);
            }

            int side = 0;
            int64_t max_score = threeMAX(score, left_score, right_score);
            if (max_score == score) {
                break;
            }
            side = max_score == left_score ? -1 : 1;

            // grow interval
            if (side == -1) {
                left_iter->valid = false;
                interval_start = left_iter->symbol;
                iter = left_iter;
            } else {
                right_iter->valid = false;
                interval_end = getNextString(right_iter->symbol);
                iter = right_iter;
            }
        }
        intervals_.push_back(std::make_pair(interval_start, interval_end));
    }

    void  ALMImprovedSS::generateConnectedIntervals() {
        std::sort(intervals_.begin(), intervals_.end(),
                  [](const std::pair<std::string, std::string> &x, const std::pair<std::string, std::string> &y) {
                      return strCompare(x.first, y.first) < 0;
                  });
        std::vector<std::pair<std::string, std::string> > tmp_intervals(intervals_);
        intervals_.clear();
        std::string start_string = std::string(1, char(0));
        for (int i = 0; i < (int)tmp_intervals.size(); i++) {
            std::pair<std::string, std::string>* cur_interval = &tmp_intervals[i];
            fillGap(start_string, cur_interval->first);
            intervals_.push_back(std::make_pair(cur_interval->first, cur_interval->second));
            start_string = cur_interval->second;
        }
        fillGap(start_string, std::string(MAX_KEY_LEN, char(255)));
    }

    void ALMImprovedSS::getSkewInterval(std::vector<SymbolFreq> &blend_freq_table) {
       // Add isvalid field
        std::vector<SymbolFreqValid>tmp_freq_table;
        // Add ranking vector    frequency, iter of the original vector
        std::vector<std::pair<int64_t, freq_iter> > rank;

        for (int i = 0; i < (int)blend_freq_table.size(); i++) {
            auto cur_freq = blend_freq_table[i];
            SymbolFreqValid tmp;
            tmp.symbol = cur_freq.first;
            tmp.freq = cur_freq.second;
            tmp.valid = true;
            tmp_freq_table.push_back(tmp);
        }

        std::sort(tmp_freq_table.begin(), tmp_freq_table.end(),
                  [](const SymbolFreqValid &x, const SymbolFreqValid &y) {
                      return strCompare(x.symbol, y.symbol) < 0;
                  });

        for (auto it = tmp_freq_table.begin();it != tmp_freq_table.end(); it++) {
            rank.push_back(std::make_pair(it->freq, it));
        }

        // Sort the frequency table ordered by the frequency in descending order
         std::sort(rank.begin(), rank.end(),
                  [](auto &x, const auto &y) {
                      return (x.first - y.first) > 0;
                  });

        int seed_cnt = std::min((int)W, (int)tmp_freq_table.size());
        std::cout << "seed cnt:" << seed_cnt << std::endl;
        auto stop_iter = rank.begin() + seed_cnt;
        for (auto iter = rank.begin(); iter != rank.end() && iter != stop_iter ; iter++) {
            growInterval(iter->second, tmp_freq_table);
         //   std::cout << intervals_.size() << std::endl;
        }

        // Go through all itervals to fill Gaps
        generateConnectedIntervals();
    }

    void ALMImprovedSS::fillGap(std::string start_include, std::string end_exclude) {
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


   void ALMImprovedSS::getEqualInterval(std::vector<SymbolFreq> &blend_freq_table) {
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

    // Wrong way of merging intervals
    void ALMImprovedSS::mergeIntervals(std::vector<SymbolFreq>::iterator start_exclude_iter,
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
            if ((int)prefix.size() * cnt > W) {
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
    std::string ALMImprovedSS::commonPrefix(const std::string &str1,
                                          const std::string &str2) {
        int min_len = (int)std::min(str1.size(), str2.size());
        int i = 0;
        for (; i < min_len; i++) {
            if (str1[i] != str2[i])
                return str1.substr(0, i);
        }
        return str1.substr(0, i);
    }

    std::string ALMImprovedSS::getPrevString(const std::string &str) {
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

    std::string ALMImprovedSS::getNextString(const std::string &str) {
        assert(str.length() > 0);
        for (int i = int(str.length() - 1); i >= 0; i--) {
            if (uint8_t (str[i]) != 255) {
                char next_chr = str[i] + 1;
                return str.substr(0, i) + std::string(1, next_chr);
            }
        }
        return std::string(MAX_KEY_LEN, 255);
    }


    void ALMImprovedSS::checkIntervals(std::string& start_str, std::string& end_str) {
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
//                auto nit = iter - 5;
//                for (; nit != iter + 5 && nit != intervals_.end(); nit++) {
//                    printString(nit->first);
//                    std::cout << "|";
//                    printString(nit->second);
//                    std::cout << std::endl;
//                }
//		        assert(false);
            }
            end = iter->second;
        }
        std::cout << "Check " << cnt << " intervals" << std::endl;
    }

    void ALMImprovedSS::mergeAdjacentComPrefixIntervals() {
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
