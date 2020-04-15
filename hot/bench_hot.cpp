#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

#include "encoder_factory.hpp"
#include "hot/singlethreaded/HOTSingleThreaded.hpp"
#include "idx/contenthelpers/IdentityKeyExtractor.hpp"
#include "parameters.h"

#define WRITE_TO_FILE

static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000;
static const uint64_t kNumTxns = 5000;
static const int kSamplePercent = 20;
static const double kUrlSamplePercent = 20;

static const std::string file_load_email = "workloads/load_email";
static const std::string file_load_wiki = "workloads/load_wiki";
static const std::string file_load_url = "workloads/load_url";

static const std::string file_txn_email = "workloads/txn_email_zipfian";
static const std::string file_txn_email_len = "workloads/scan_len_email_zipfian";
static const std::string file_txn_wiki = "workloads/txn_wiki_zipfian";
static const std::string file_txn_wiki_len = "workloads/scan_len_wiki_zipfian";
static const std::string file_txn_url = "workloads/txn_url_zipfian";
static const std::string file_txn_url_len = "workloads/scan_len_url_zipfian";

// for pretty print
static const char* kGreen ="\033[0;32m";
static const char* kNoColor ="\033[0;0m";

static int runALM = 1;
//-------------------------------------------------------------
// Workload IDs
//-------------------------------------------------------------
static const int kEmail = 0;
static const int kWiki = 1;
static const int kUrl = 2;

//-------------------------------------------------------------
// Expt ID = 0
//-------------------------------------------------------------
static const std::string output_dir_hot_point = "results/hot/point/";
static const std::string file_lookuplat_email_hot = output_dir_hot_point + "lookuplat_email_hot.csv";
std::ofstream output_lookuplat_email_hot;
static const std::string file_insertlat_email_hot = output_dir_hot_point + "insertlat_email_hot.csv";
std::ofstream output_insertlat_email_hot;
static const std::string file_mem_email_hot = output_dir_hot_point + "mem_email_hot.csv";
std::ofstream output_mem_email_hot;
static const std::string file_height_email_hot = output_dir_hot_point + "height_email_hot.csv";
std::ofstream output_height_email_hot;
static const std::string file_time_email_hot = output_dir_hot_point + "time_email_hot.csv";
std::ofstream output_time_email_hot;
static const std::string file_stats_email_hot = output_dir_hot_point + "stats_email_hot.csv";
std::ofstream output_stats_email_hot;

static const std::string file_lookuplat_wiki_hot = output_dir_hot_point + "lookuplat_wiki_hot.csv";
std::ofstream output_lookuplat_wiki_hot;
static const std::string file_insertlat_wiki_hot = output_dir_hot_point + "insertlat_wiki_hot.csv";
std::ofstream output_insertlat_wiki_hot;
static const std::string file_mem_wiki_hot = output_dir_hot_point + "mem_wiki_hot.csv";
std::ofstream output_mem_wiki_hot;
static const std::string file_height_wiki_hot = output_dir_hot_point + "height_wiki_hot.csv";
std::ofstream output_height_wiki_hot;
static const std::string file_time_wiki_hot = output_dir_hot_point + "time_wiki_hot.csv";
std::ofstream output_time_wiki_hot;
static const std::string file_stats_wiki_hot = output_dir_hot_point + "stats_wiki_hot.csv";
std::ofstream output_stats_wiki_hot;

static const std::string file_lookuplat_url_hot = output_dir_hot_point + "lookuplat_url_hot.csv";
std::ofstream output_lookuplat_url_hot;
static const std::string file_insertlat_url_hot = output_dir_hot_point + "insertlat_url_hot.csv";
std::ofstream output_insertlat_url_hot;
static const std::string file_mem_url_hot = output_dir_hot_point + "mem_url_hot.csv";
std::ofstream output_mem_url_hot;
static const std::string file_height_url_hot = output_dir_hot_point + "height_url_hot.csv";
std::ofstream output_height_url_hot;
static const std::string file_time_url_hot = output_dir_hot_point + "time_url_hot.csv";
std::ofstream output_time_url_hot;
static const std::string file_stats_url_hot = output_dir_hot_point + "stats_url_hot.csv";
std::ofstream output_stats_url_hot;

static const std::string file_lookuplat_ts_hot = output_dir_hot_point + "lookuplat_ts_hot.csv";
std::ofstream output_lookuplat_ts_hot;
static const std::string file_insertlat_ts_hot = output_dir_hot_point + "insertlat_ts_hot.csv";
std::ofstream output_insertlat_ts_hot;
static const std::string file_mem_ts_hot = output_dir_hot_point + "mem_ts_hot.csv";
std::ofstream output_mem_ts_hot;
static const std::string file_height_ts_hot = output_dir_hot_point + "height_ts_hot.csv";
std::ofstream output_height_ts_hot;
static const std::string file_time_ts_hot = output_dir_hot_point + "time_ts_hot.csv";
std::ofstream output_time_ts_hot;
static const std::string file_stats_ts_hot = output_dir_hot_point + "stats_ts_hot.csv";
std::ofstream output_stats_ts_hot;
//-------------------------------------------------------------
// Expt ID = 1
//-------------------------------------------------------------
static const std::string output_dir_hot_range = "results/hot/range/";
static const std::string file_lookuplat_email_hot_range = output_dir_hot_range + "lookuplat_email_hot_range.csv";
std::ofstream output_lookuplat_email_hot_range;
static const std::string file_insertlat_email_hot_range = output_dir_hot_range + "insertlat_email_hot_range.csv";
std::ofstream output_insertlat_email_hot_range;
static const std::string file_mem_email_hot_range = output_dir_hot_range + "mem_email_hot_range.csv";
std::ofstream output_mem_email_hot_range;
static const std::string file_height_email_hot_range = output_dir_hot_range + "height_email_hot_range.csv";
std::ofstream output_height_email_hot_range;
static const std::string file_time_email_hot_range = output_dir_hot_range + "time_email_hot_range.csv";
std::ofstream output_time_email_hot_range;

static const std::string file_lookuplat_wiki_hot_range = output_dir_hot_range + "lookuplat_wiki_hot_range.csv";
std::ofstream output_lookuplat_wiki_hot_range;
static const std::string file_insertlat_wiki_hot_range = output_dir_hot_range + "insertlat_wiki_hot_range.csv";
std::ofstream output_insertlat_wiki_hot_range;
static const std::string file_mem_wiki_hot_range = output_dir_hot_range + "mem_wiki_hot_range.csv";
std::ofstream output_mem_wiki_hot_range;
static const std::string file_height_wiki_hot_range = output_dir_hot_range + "height_wiki_hot_range.csv";
std::ofstream output_height_wiki_hot_range;
static const std::string file_time_wiki_hot_range = output_dir_hot_range + "time_wiki_hot_range.csv";
std::ofstream output_time_wiki_hot_range;

static const std::string file_lookuplat_url_hot_range = output_dir_hot_range + "lookuplat_url_hot_range.csv";
std::ofstream output_lookuplat_url_hot_range;
static const std::string file_insertlat_url_hot_range = output_dir_hot_range + "insertlat_url_hot_range.csv";
std::ofstream output_insertlat_url_hot_range;
static const std::string file_mem_url_hot_range = output_dir_hot_range + "mem_url_hot_range.csv";
std::ofstream output_mem_url_hot_range;
static const std::string file_height_url_hot_range = output_dir_hot_range + "height_url_hot_range.csv";
std::ofstream output_height_url_hot_range;
static const std::string file_time_url_hot_range = output_dir_hot_range + "time_url_hot_range.csv";
std::ofstream output_time_url_hot_range;

static const std::string file_lookuplat_ts_hot_range = output_dir_hot_range + "lookuplat_ts_hot_range.csv";
std::ofstream output_lookuplat_ts_hot_range;
static const std::string file_insertlat_ts_hot_range = output_dir_hot_range + "insertlat_ts_hot_range.csv";
std::ofstream output_insertlat_ts_hot_range;
static const std::string file_mem_ts_hot_range = output_dir_hot_range + "mem_ts_hot_range.csv";
std::ofstream output_mem_ts_hot_range;
static const std::string file_height_ts_hot_range = output_dir_hot_range + "height_ts_hot_range.csv";
std::ofstream output_height_ts_hot_range;
static const std::string file_time_ts_hot_range = output_dir_hot_range + "time_ts_hot_range.csv";
std::ofstream output_time_ts_hot_range;


double getNow() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void loadKeysFromFile(const std::string& file_name, const uint64_t num_records,
		      std::vector<std::string> &keys) {
    std::ifstream infile(file_name);
    std::string key;
    uint64_t count = 0;
    while (count < num_records && infile.good()) {
	    infile >> key;
        if (key.length() > 255) {
            continue;
        }
	    keys.push_back(key);
	    count++;
    }
}

void loadLensInt(const std::string& file_name, const uint64_t num_records,              std::vector<int> &keys) {
    std::ifstream infile(file_name);
    int key;
    uint64_t count = 0;
    while (count < num_records && infile.good()) {
        infile >> key;
        keys.push_back(key);
        count++;
    }
}

std::string uint64ToString(uint64_t key) {
    uint64_t endian_swapped_key = __builtin_bswap64(key);
    return std::string(reinterpret_cast<const char*>(&endian_swapped_key), 8);
}

void loadKeysInt(const std::string& file_name, const uint64_t num_records,
                 std::vector<std::string>& keys) {
    std::ifstream infile(file_name);
    uint64_t int_key;
    uint64_t count = 0;
    while (count < num_records && infile.good()) {
        infile >> int_key;
        std::string key = uint64ToString(int_key);
        keys.push_back(key);
        count++;
    }
}

std::string getUpperBoundKey(const std::string& key) {
    std::string ret_str = key;
    ret_str[ret_str.size() - 1] = (char)'\255';
    return ret_str;
}

void checkTxnKeys(std::vector<std::string>& insert_keys, std::vector<std::string>& txn_keys) {
    for (int i = 0; i < (int)txn_keys.size(); i++) {
        std::string cur_key = txn_keys[i];
        for (int j = 0; j < (int)insert_keys.size();j++)
            if (cur_key.compare(insert_keys[j]) == 0) return;
        std::cout << "Key " << cur_key << " dose not exist" << std::endl;
    }
}

void loadWorkload(int wkld_id,
		  std::vector<std::string>& insert_keys,
		  std::vector<std::string>& insert_keys_sample,
		  std::vector<std::string>& txn_keys,
		  std::vector<int>& upper_bound_keys) {
    std::vector<std::string> load_keys;
    if (wkld_id == kEmail)
        loadKeysFromFile(file_load_email, kNumEmailRecords, load_keys);
    else if (wkld_id == kWiki)
        loadKeysFromFile(file_load_wiki, kNumWikiRecords, load_keys);
    else if (wkld_id == kUrl)
        loadKeysFromFile(file_load_url, kNumEmailRecords, load_keys);
    else
        return;

       for (int i = 0; i < (int)load_keys.size() - 1; i++) {
        int key_len = load_keys[i].length();
        int next_key_len = load_keys[i + 1].length();
        if (key_len < next_key_len) {
            std::string next_prefix = load_keys[i + 1].substr(0, key_len);
            if (load_keys[i].compare(next_prefix) != 0)
                insert_keys.push_back(load_keys[i]);
        } else {
            insert_keys.push_back(load_keys[i]);
        }
    }
    insert_keys.push_back(load_keys[load_keys.size() - 1]);

    load_keys.clear();
    std::random_shuffle(insert_keys.begin(), insert_keys.end());

    double percent = (wkld_id == kUrl) ? kUrlSamplePercent : kSamplePercent;
    for (int i = 0; i < (int)insert_keys.size(); i += int(100 / percent)) {
        insert_keys_sample.push_back(insert_keys[i]);
    }

    if (wkld_id == kEmail) {
        loadKeysFromFile(file_txn_email, kNumTxns, txn_keys);
        loadLensInt(file_txn_email_len, kNumTxns, upper_bound_keys);
    } else if (wkld_id == kWiki) {
        loadKeysFromFile(file_txn_wiki, kNumTxns, txn_keys);
        loadLensInt(file_txn_wiki_len, kNumTxns, upper_bound_keys);
    } else if (wkld_id == kUrl) {
        loadKeysFromFile(file_txn_url, kNumTxns, txn_keys);
        loadLensInt(file_txn_url_len, kNumTxns, upper_bound_keys);
    }

    std::cout << "insert_keys size = " << insert_keys.size() << std::endl;
    std::cout << "insert_keys_sample size = " << insert_keys_sample.size() << std::endl;
    std::cout << "txn_keys size = " << txn_keys.size() << std::endl;
    std::cout << "upper_bound_keys size = " << upper_bound_keys.size() << std::endl;
}

double getStatsDefault(std::map<std::string, double>& stats_map, std::string name) {
    std::map<std::string,double>::iterator it =  stats_map.find(name);
    if (it != stats_map.end()) {
        return it->second;
    }
    return 0;
}

bool unit8CompareBeq(const uint8_t* s1, const uint8_t* s2) {
    int len1 = sizeof(s1)/sizeof(uint8_t);
    int len2 = sizeof(s2)/sizeof(uint8_t);
    for (int i = 0; i < len1 && i < len2; i++) {
        if (s1[i] > s2[i])
            return true;
        if (s1[i] < s2[i])
            return false;
    }
    if (len1 >= len2)
        return true;
    return false;
}

void printStr(const char* str) {
    for (int i = 0; i < (int)strlen(str); i++) {
        std::cout << std::hex << unsigned((uint8_t)str[i]) << " ";
    }
    std::cout << std::endl;
}

void testEQ(char* str1, char* str2, int len) {
    if (memcmp(str1, str2, len - 1) != 0) {
        printStr(str1);
        printStr(str2);
    }
}

void exec(const int expt_id, const int wkld_id, const bool is_point,
	  const bool is_compressed,
	  const int encoder_type, const int64_t dict_size_id,
	  const std::vector<std::string>& insert_keys,
	  const std::vector<std::string>& insert_keys_sample,
	  const std::vector<std::string>& txn_keys,
	  const std::vector<int>& scan_key_lens) {
    hope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    uint8_t* buffer_r = new uint8_t[8192];
    std::vector<std::pair<std::string, char*> > cstr_enc_insert_keys;

    int64_t input_dict_size = dict_size_list[dict_size_id];
    if (encoder_type == 3) {
        input_dict_size = three_gram_input_dict_size[wkld_id][dict_size_id];
    } else if (encoder_type == 4) {
        input_dict_size = four_gram_input_dict_size[wkld_id][dict_size_id];
    }
    int W = 0;
    if (encoder_type == 5)
        W = ALM_W[wkld_id][dict_size_id];
    if (encoder_type == 6)
        W = ALM_W_improved[wkld_id][dict_size_id];

    double start_time = getNow();
    if (is_compressed) {
        encoder = hope::EncoderFactory::createEncoder(encoder_type, W);
        encoder->build(insert_keys_sample, input_dict_size);
    }

    std::string tmp_str;
    for (int i = 0; i < (int)insert_keys.size(); i++) {
        if (is_compressed) {
            int enc_len = encoder->encode(insert_keys[i], buffer);
            int enc_len_round = (enc_len + 7) >> 3;
            tmp_str = std::string((const char*)buffer, enc_len_round);
        } else {
            tmp_str = insert_keys[i];
        }
        char * cstr = new char [tmp_str.length()+1];
        memset(cstr, 0, tmp_str.length()+1);
        std::strcpy(cstr, tmp_str.c_str());
        cstr_enc_insert_keys.push_back(std::make_pair(insert_keys[i], cstr));
    }

    typedef hot::singlethreaded::HOTSingleThreaded<const char*, idx::contenthelpers::IdentityKeyExtractor> hot_type;
    hot_type* ht = new hot_type();
    double insert_start_time = getNow();
    for (int i = 0; i < (int)cstr_enc_insert_keys.size(); i++) {
        std::pair<std::string, char*>* tmp_pair = &cstr_enc_insert_keys[i];
        if (is_compressed) {
            encoder->encode(tmp_pair->first, buffer);
        }
        ht->insert(tmp_pair->second);
    }

    double end_time = getNow();
    double build_time = end_time - start_time;
    double insert_time = end_time - insert_start_time;
    std::cout << "Build time = " << build_time << std::endl;
    std::cout << "Insert time = " << insert_time << std::endl;

    int64_t htree_size = ht->getStatistics().first;
    std::map<std::string, double> stats_map =  ht->getStatistics().second;
    std::cout << "size = " << htree_size << std::endl;
    double height = stats_map["height"];
    std::vector<double>heights;
    for (int i = 1; i <= int(height); i++) {
        std::string key_name = "leafNodesOnDepth_0" + std::to_string(i);
        heights.push_back(int(stats_map[key_name.c_str()]));
    }

    std::vector<double> node_stats;
    node_stats.push_back(getStatsDefault(stats_map, "SINGLE_MASK_8_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "SINGLE_MASK_16_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "SINGLE_MASK_32_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "MULTI_MASK_8_BYTES_AND_8_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "MULTI_MASK_8_BYTES_AND_16_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "MULTI_MASK_8_BYTES_AND_32_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "MULTI_MASK_16_BYTES_AND_16_BIT_PARTIAL_KEYS "));
    node_stats.push_back(getStatsDefault(stats_map, "MULTI_MASK_32_BYTES_AND_32_BIT_PARTIAL_KEYS "));
    uint64_t node_total = 0;
    for (int i = 0; i < 8; i++) {
        node_total += node_stats[i];
    }
    node_stats.push_back(getStatsDefault(stats_map, "fanout") * 1.0 / node_total);

    double mem = htree_size;
    if (encoder != nullptr)
	    mem += encoder->memoryUse();
    mem = mem / 1000000.0;
    std::cout << kGreen << "Mem = " << kNoColor << mem << std::endl;

    // execute transactions =======================================
    //uint64_t sum = 0;
    start_time = getNow();
    uint64_t TIDs[120];
    uint64_t sum = 0;
    if (is_point) { // point query
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                int enc_len = encoder->encode(txn_keys[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                std::string enc_key = std::string((const char*)buffer, enc_len_round);
                sum += ht->lookup(reinterpret_cast<const char*>(enc_key.c_str())).mIsValid;
	        }
	    } else {
	        for (int i = 0; i < (int)txn_keys.size(); i++) {
                sum += ht->lookup(reinterpret_cast<const char*>(txn_keys[i].c_str())).mIsValid;
	        }
	    }
    } else { // range query
        std::string endStr = std::string(255, char(255));
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                int enc_len = 0;

	            enc_len = encoder->encode(txn_keys[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                std::string left_key = std::string((const char*)buffer, enc_len_round);
                hot_type::const_iterator iter = ht->lower_bound((const char*)(left_key.c_str()));
                int cnt = 0;
                while (iter != ht->end() && endStr.compare(*iter) > 0
                    && cnt < scan_key_lens[i]) {
		            TIDs[cnt] = uint64_t(*iter);
		            ++iter;
                    ++cnt;
		        }
	        }
	    } else {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                hot_type::const_iterator iter = ht->lower_bound(txn_keys[i].c_str());
                int cnt = 0;
                while (iter != ht->end() && (endStr).compare(*iter) > 0
                     && cnt < scan_key_lens[i]) {
                    TIDs[cnt] = uint64_t(*iter);
		            ++iter;
                    ++cnt;
                }

            }
  	    }
    }
    end_time = getNow();
    double exec_time = end_time - start_time;
    double tput = txn_keys.size() / exec_time / 1000000; // Mops/sec
    std::cout << kGreen << "Throughput = " << kNoColor << tput << "\n";
    double lookup_lat = (exec_time * 1000000) / txn_keys.size(); // us
    double insert_lat = (insert_time * 1000000) / cstr_enc_insert_keys.size(); // us
    std::cout << TIDs[0] << std::endl;
    std::cout << kGreen << "Lookup Latency = " << kNoColor << lookup_lat << "\n";
    std::cout << kGreen << "Insert Latency = " << kNoColor << insert_lat << "\n";

    delete ht;
    delete encoder;
    delete[] buffer;
    delete[] buffer_r;
    for (int i = 0; i < (int)cstr_enc_insert_keys.size(); i++)
        delete cstr_enc_insert_keys[i].second;
#ifdef WRITE_TO_FILE
    if (expt_id == 0) {
        if (wkld_id == kEmail) {
            output_lookuplat_email_hot << lookup_lat << "\n";
            output_insertlat_email_hot << insert_lat << "\n";
	        output_mem_email_hot << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_email_hot << std::fixed << heights[i] << ",";
            }
            output_height_email_hot << std::endl;
            for (int i = 0; i < int(node_stats.size()); i++) {
                output_stats_email_hot << node_stats[i] << ",";
            }
            output_stats_email_hot << std::endl;
	    } else if (wkld_id == kWiki) {

            output_lookuplat_wiki_hot << lookup_lat << "\n";
            output_insertlat_wiki_hot << insert_lat << "\n";
            output_mem_wiki_hot << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_wiki_hot << std::fixed << heights[i] << ",";
            }
            output_height_wiki_hot << std::endl;
            for (int i = 0; i < int(node_stats.size()); i++) {
                output_stats_wiki_hot << node_stats[i] << ",";
            }
            output_stats_wiki_hot << std::endl;
	    } else if (wkld_id == kUrl) {
            output_lookuplat_url_hot << lookup_lat << "\n";
            output_insertlat_url_hot << insert_lat << "\n";
	        output_mem_url_hot << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_url_hot << std::fixed << heights[i] << ",";
            }
            output_height_url_hot << std::endl;
            for (int i = 0; i < int(node_stats.size()); i++) {
                output_stats_url_hot << node_stats[i] << ",";
            }
            output_stats_url_hot << std::endl;
	    }
    } else if (expt_id == 1) {
        if (wkld_id == kEmail) {
            output_lookuplat_email_hot_range << lookup_lat << "\n";
            output_insertlat_email_hot_range << insert_lat << "\n";
            output_mem_email_hot_range << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_email_hot_range << std::fixed << heights[i] << ",";
            }
            output_height_email_hot_range << std::endl;
        } else if (wkld_id == kWiki) {
            output_lookuplat_wiki_hot_range << lookup_lat << "\n";
            output_insertlat_wiki_hot_range << insert_lat << "\n";
            output_mem_wiki_hot_range << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_wiki_hot_range << std::fixed << heights[i] << ",";
            }
            output_height_wiki_hot_range << std::endl;
    	} else if (wkld_id == kUrl) {

            output_lookuplat_url_hot_range << lookup_lat << "\n";
            output_insertlat_url_hot_range << insert_lat << "\n";
	        output_mem_url_hot_range << mem << "\n";
            for (int i = 0; i < int(height); i++) {
                output_height_url_hot_range << std::fixed << heights[i] << ",";
            }
            output_height_url_hot_range << std::endl;
	    }
    }
#endif
}

void exec_group(const int expt_id, const bool is_point,
		int& expt_num, const int total_num_expt,
		const std::vector<std::string>& insert_emails,
		const std::vector<std::string>& insert_emails_sample,
		const std::vector<std::string>& txn_emails,
		const std::vector<int>& upper_bound_emails,
		const std::vector<std::string>& insert_wikis,
		const std::vector<std::string>& insert_wikis_sample,
		const std::vector<std::string>& txn_wikis,
		const std::vector<int>& upper_bound_wikis,
		const std::vector<std::string>& insert_urls,
		const std::vector<std::string>& insert_urls_sample,
		const std::vector<std::string>& txn_urls,
		const std::vector<int>& upper_bound_urls,
        const std::vector<std::string>& insert_tss,
		const std::vector<std::string>& insert_tss_sample,
		const std::vector<std::string>& txn_tss,
		const std::vector<int>& upper_bound_tss){

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, false, 0, 0,
	 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, false, 0, 0,
	 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, false, 0, 0,
	 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

    //=================================================
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, true, 1, 0,
	 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 1, 0,
	 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 1, 0,
	 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

    //=================================================
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, true, 2, 6,
	 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 2, 6,
	 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 2, 6,
	 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	exec(expt_id, kEmail, is_point, true, 3, 6,
	     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
	expt_num++;
	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 3, 6,
	     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
	expt_num++;


	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	exec(expt_id, kUrl, is_point, true, 3, 6,
	     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
	expt_num++;


	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	exec(expt_id, kEmail, is_point, true, 4, 6,
	     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
	expt_num++;

	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	exec(expt_id, kWiki, is_point, true, 4, 6,
	     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
	expt_num++;

	std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	exec(expt_id, kUrl, is_point, true, 4, 6,
	     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
	expt_num++;

    if (runALM == 1) {
        for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
           int dict_size_5[2] = {2, 6};
           for (int j = 0; j < 2; j++) {
               std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
               exec(expt_id, kEmail, is_point, true, encoder_type, dict_size_5[j],
                       insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
               expt_num++;

               std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
               exec(expt_id, kWiki, is_point, true, encoder_type, dict_size_5[j],
                      insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
               expt_num++;

               std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
               exec(expt_id, kUrl, is_point, true, encoder_type, dict_size_5[j],
	                   insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
               expt_num++;
           }
        }
    }
}

int main(int argc, char *argv[]) {
    int expt_id = (int)atoi(argv[1]);
    runALM = (int)atoi(argv[2]);

    //-------------------------------------------------------------
    // Init Workloads
    //-------------------------------------------------------------
    std::vector<std::string> insert_emails, insert_emails_sample, txn_emails;
    std::vector<int> upper_bound_emails;
    loadWorkload(kEmail, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);

    std::vector<std::string> insert_wikis, insert_wikis_sample, txn_wikis;
    std::vector<int> upper_bound_wikis;
    loadWorkload(kWiki, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);

    std::vector<std::string> insert_urls, insert_urls_sample, txn_urls;
    std::vector<int> upper_bound_urls;
    loadWorkload(kUrl, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);

    std::vector<std::string> insert_tss, insert_tss_sample, txn_tss;
    std::vector<int> upper_bound_tss;
    if (expt_id == 0) {
	//-------------------------------------------------------------
	// Point Queries; Expt ID = 0
	//-------------------------------------------------------------
	std::cout << "====================================" << std::endl;
	std::cout << "Point Queries; Expt ID = 0" << std::endl;
	std::cout << "====================================" << std::endl;
#ifdef WRITE_TO_FILE
	output_lookuplat_email_hot.open(file_lookuplat_email_hot, std::ofstream::app);
	output_insertlat_email_hot.open(file_insertlat_email_hot, std::ofstream::app);
	output_mem_email_hot.open(file_mem_email_hot, std::ofstream::app);
	output_height_email_hot.open(file_height_email_hot, std::ofstream::app);
    output_stats_email_hot.open(file_stats_email_hot, std::ofstream::app);

	output_lookuplat_wiki_hot.open(file_lookuplat_wiki_hot, std::ofstream::app);
	output_insertlat_wiki_hot.open(file_insertlat_wiki_hot, std::ofstream::app);
	output_mem_wiki_hot.open(file_mem_wiki_hot, std::ofstream::app);
	output_height_wiki_hot.open(file_height_wiki_hot, std::ofstream::app);
    output_stats_wiki_hot.open(file_stats_wiki_hot, std::ofstream::app);

	output_lookuplat_url_hot.open(file_lookuplat_url_hot, std::ofstream::app);
	output_insertlat_url_hot.open(file_insertlat_url_hot, std::ofstream::app);
	output_mem_url_hot.open(file_mem_url_hot, std::ofstream::app);
	output_height_url_hot.open(file_height_url_hot, std::ofstream::app);
    output_stats_url_hot.open(file_stats_url_hot, std::ofstream::app);
#endif

	bool is_point = true;
	int expt_num = 1;
	int total_num_expt = 24;
	exec_group(expt_id, is_point, expt_num, total_num_expt,
		   insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
		   insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
		   insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
		   insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
#ifdef WRITE_TO_FILE
    output_lookuplat_email_hot << "-" << "\n";
    output_insertlat_email_hot << "-" << "\n";
	output_mem_email_hot << "-" << "\n";
    output_height_email_hot << "-" << "\n";
	output_stats_email_hot << "-" << "\n";

	output_lookuplat_email_hot.close();
	output_insertlat_email_hot.close();
	output_mem_email_hot.close();
    output_height_email_hot.close();
	output_stats_email_hot.close();

    output_lookuplat_wiki_hot << "-" << "\n";
    output_insertlat_wiki_hot << "-" << "\n";
	output_mem_wiki_hot << "-" << "\n";
	output_height_wiki_hot << "-" << "\n";
	output_stats_wiki_hot << "-" << "\n";

	output_lookuplat_wiki_hot.close();
	output_insertlat_wiki_hot.close();
	output_mem_wiki_hot.close();
	output_height_wiki_hot.close();
	output_stats_wiki_hot.close();

    output_lookuplat_url_hot << "-" << "\n";
    output_insertlat_url_hot << "-" << "\n";
	output_mem_url_hot << "-" << "\n";
	output_height_url_hot << "-" << "\n";
	output_stats_url_hot << "-" << "\n";

	output_lookuplat_url_hot.close();
	output_insertlat_url_hot.close();
	output_mem_url_hot.close();
	output_height_url_hot.close();
	output_stats_url_hot.close();

#endif

    }
    else if (expt_id == 1) {
	//-------------------------------------------------------------
	// Range Queries; Expt ID = 1
	//-------------------------------------------------------------
	std::cout << "====================================" << std::endl;
	std::cout << "Range Queries; Expt ID = 1" << std::endl;
	std::cout << "====================================" << std::endl;
#ifdef WRITE_TO_FILE
	output_lookuplat_email_hot_range.open(file_lookuplat_email_hot_range, std::ofstream::app);
	output_insertlat_email_hot_range.open(file_insertlat_email_hot_range, std::ofstream::app);
	output_mem_email_hot_range.open(file_mem_email_hot_range, std::ofstream::app);

	output_lookuplat_wiki_hot_range.open(file_lookuplat_wiki_hot_range, std::ofstream::app);
	output_insertlat_wiki_hot_range.open(file_insertlat_wiki_hot_range, std::ofstream::app);
	output_mem_wiki_hot_range.open(file_mem_wiki_hot_range, std::ofstream::app);

	output_lookuplat_url_hot_range.open(file_lookuplat_url_hot_range, std::ofstream::app);
	output_insertlat_url_hot_range.open(file_insertlat_url_hot_range, std::ofstream::app);
	output_mem_url_hot_range.open(file_mem_url_hot_range, std::ofstream::app);
#endif

	bool is_point = false;
	int expt_num = 1;
	int total_num_expt = 24;
	exec_group(expt_id, is_point, expt_num, total_num_expt,
		   insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
		   insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
		   insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
		   insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
#ifdef WRITE_TO_FILE
    output_lookuplat_email_hot_range << "-" << "\n";
    output_insertlat_email_hot_range << "-" << "\n";
	output_mem_email_hot_range << "-" << "\n";

	output_lookuplat_email_hot_range.close();
	output_insertlat_email_hot_range.close();
	output_mem_email_hot_range.close();

    output_lookuplat_wiki_hot_range << "-" << "\n";
    output_insertlat_wiki_hot_range << "-" << "\n";
	output_mem_wiki_hot_range << "-" << "\n";

	output_lookuplat_wiki_hot_range.close();
	output_insertlat_wiki_hot_range.close();
	output_mem_wiki_hot_range.close();

    output_lookuplat_url_hot_range << "-" << "\n";
    output_insertlat_wiki_hot_range << "-" << "\n";
	output_mem_url_hot_range << "-" << "\n";

	output_lookuplat_url_hot_range.close();
    output_insertlat_wiki_hot_range.close();
	output_mem_url_hot_range.close();
#endif

    }
    return 0;
}
