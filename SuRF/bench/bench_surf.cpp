#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>
#include <set>

#include "encoder_factory.hpp"
#include "surf.hpp"

//#define NOT_USE_ENCODE_PAIR 1
//#define RUN_TIMESTAMP

static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000000;
static const uint64_t kNumTsRecords = 25000000;
static const uint64_t kNumTxns = 10000000;
static const unsigned kPercent = 50;
static const int kSamplePercent = 1;
static const double kUrlSamplePercent = 1;

static const std::string file_load_email = "workloads/load_email";
static const std::string file_load_wiki = "workloads/load_wiki";
static const std::string file_load_url = "workloads/load_url";
static const std::string file_load_ts = "workloads/load_timestamp";

static const std::string file_txn_email = "workloads/txn_email_zipfian";
static const std::string file_txn_wiki = "workloads/txn_wiki_zipfian";
static const std::string file_txn_url = "workloads/txn_url_zipfian";
static const std::string file_txn_ts = "workloads/txn_timestamp_zipfian";

// for pretty print
static const char* kGreen ="\033[0;32m";
//static const char* kRed ="\033[0;31m";
static const char* kNoColor ="\033[0;0m";

//-------------------------------------------------------------
// Workload IDs
//-------------------------------------------------------------
static const int kEmail = 0;
static const int kWiki = 1;
static const int kUrl = 2;
static const int kTs = 3;

//-------------------------------------------------------------
// Expt ID = 0
//-------------------------------------------------------------
static const std::string output_dir_surf_point = "results/SuRF/point/";
static const std::string file_lat_email_surf = output_dir_surf_point + "lat_email_surf.csv";
std::ofstream output_lat_email_surf;
static const std::string file_mem_email_surf = output_dir_surf_point + "mem_email_surf.csv";
std::ofstream output_mem_email_surf;
static const std::string file_fpr_email_surf = output_dir_surf_point + "fpr_email_surf.csv";
std::ofstream output_fpr_email_surf;
static const std::string file_height_email_surf = output_dir_surf_point + "height_email_surf.csv";
std::ofstream output_height_email_surf;
static const std::string file_stats_email_surf = output_dir_surf_point + "stats_email_surf.csv";
std::ofstream output_stats_email_surf;


static const std::string file_lat_wiki_surf = output_dir_surf_point + "lat_wiki_surf.csv";
std::ofstream output_lat_wiki_surf;
static const std::string file_mem_wiki_surf = output_dir_surf_point + "mem_wiki_surf.csv";
std::ofstream output_mem_wiki_surf;
static const std::string file_fpr_wiki_surf = output_dir_surf_point + "fpr_wiki_surf.csv";
std::ofstream output_fpr_wiki_surf;
static const std::string file_height_wiki_surf = output_dir_surf_point + "height_wiki_surf.csv";
std::ofstream output_height_wiki_surf;
static const std::string file_stats_wiki_surf = output_dir_surf_point + "stats_wiki_surf.csv";
std::ofstream output_stats_wiki_surf;


static const std::string file_lat_url_surf = output_dir_surf_point + "lat_url_surf.csv";
std::ofstream output_lat_url_surf;
static const std::string file_mem_url_surf = output_dir_surf_point + "mem_url_surf.csv";
std::ofstream output_mem_url_surf;
static const std::string file_fpr_url_surf = output_dir_surf_point + "fpr_url_surf.csv";
std::ofstream output_fpr_url_surf;
static const std::string file_height_url_surf = output_dir_surf_point + "height_url_surf.csv";
std::ofstream output_height_url_surf;
static const std::string file_stats_url_surf = output_dir_surf_point + "stats_url_surf.csv";
std::ofstream output_stats_url_surf;


static const std::string file_lat_ts_surf = output_dir_surf_point + "lat_ts_surf.csv";
std::ofstream output_lat_ts_surf;
static const std::string file_mem_ts_surf = output_dir_surf_point + "mem_ts_surf.csv";
std::ofstream output_mem_ts_surf;
static const std::string file_fpr_ts_surf = output_dir_surf_point + "fpr_ts_surf.csv";
std::ofstream output_fpr_ts_surf;
static const std::string file_height_ts_surf = output_dir_surf_point + "height_ts_surf.csv";
std::ofstream output_height_ts_surf;
static const std::string file_stats_ts_surf = output_dir_surf_point + "stats_ts_surf.csv";
std::ofstream output_stats_ts_surf;

static const std::string output_dir_surfreal_point = "results/SuRF_real/point/";
static const std::string file_lat_email_surfreal = output_dir_surfreal_point + "lat_email_surfreal.csv";
std::ofstream output_lat_email_surfreal;
static const std::string file_mem_email_surfreal = output_dir_surfreal_point + "mem_email_surfreal.csv";
std::ofstream output_mem_email_surfreal;
static const std::string file_fpr_email_surfreal = output_dir_surfreal_point + "fpr_email_surfreal.csv";
std::ofstream output_fpr_email_surfreal;
static const std::string file_stats_email_surfreal = output_dir_surfreal_point + "stats_email_surfreal.csv";
std::ofstream output_stats_email_surfreal;


static const std::string file_lat_wiki_surfreal = output_dir_surfreal_point + "lat_wiki_surfreal.csv";
std::ofstream output_lat_wiki_surfreal;
static const std::string file_mem_wiki_surfreal = output_dir_surfreal_point + "mem_wiki_surfreal.csv";
std::ofstream output_mem_wiki_surfreal;
static const std::string file_fpr_wiki_surfreal = output_dir_surfreal_point + "fpr_wiki_surfreal.csv";
std::ofstream output_fpr_wiki_surfreal;
static const std::string file_stats_wiki_surfreal = output_dir_surfreal_point + "stats_wiki_surfreal.csv";
std::ofstream output_stats_wiki_surfreal;


static const std::string file_lat_url_surfreal = output_dir_surfreal_point + "lat_url_surfreal.csv";
std::ofstream output_lat_url_surfreal;
static const std::string file_mem_url_surfreal = output_dir_surfreal_point + "mem_url_surfreal.csv";
std::ofstream output_mem_url_surfreal;
static const std::string file_fpr_url_surfreal = output_dir_surfreal_point + "fpr_url_surfreal.csv";
std::ofstream output_fpr_url_surfreal;
static const std::string file_stats_url_surfreal = output_dir_surfreal_point + "stats_url_surfreal.csv";
std::ofstream output_stats_url_surfreal;


static const std::string file_lat_ts_surfreal = output_dir_surfreal_point + "lat_ts_surfreal.csv";
std::ofstream output_lat_ts_surfreal;
static const std::string file_mem_ts_surfreal = output_dir_surfreal_point + "mem_ts_surfreal.csv";
std::ofstream output_mem_ts_surfreal;
static const std::string file_fpr_ts_surfreal = output_dir_surfreal_point + "fpr_ts_surfreal.csv";
std::ofstream output_fpr_ts_surfreal;
static const std::string file_stats_ts_surfreal = output_dir_surfreal_point + "stats_ts_surfreal.csv";
std::ofstream output_stats_ts_surfreal;

//-------------------------------------------------------------
// Expt ID = 1
//-------------------------------------------------------------
static const std::string output_dir_surf_range = "results/SuRF/range/";
static const std::string file_lat_email_surf_range = output_dir_surf_range + "lat_email_surf_range.csv";
std::ofstream output_lat_email_surf_range;
static const std::string file_mem_email_surf_range = output_dir_surf_range + "mem_email_surf_range.csv";
std::ofstream output_mem_email_surf_range;
static const std::string file_fpr_email_surf_range = output_dir_surf_range + "fpr_email_surf_range.csv";
std::ofstream output_fpr_email_surf_range;
static const std::string file_stats_email_surf_range = output_dir_surf_range + "stats_email_surf_range.csv";
std::ofstream output_stats_email_surf_range;


static const std::string file_lat_wiki_surf_range = output_dir_surf_range + "lat_wiki_surf_range.csv";
std::ofstream output_lat_wiki_surf_range;
static const std::string file_mem_wiki_surf_range = output_dir_surf_range + "mem_wiki_surf_range.csv";
std::ofstream output_mem_wiki_surf_range;
static const std::string file_fpr_wiki_surf_range = output_dir_surf_range + "fpr_wiki_surf_range.csv";
std::ofstream output_fpr_wiki_surf_range;
static const std::string file_stats_wiki_surf_range = output_dir_surf_range + "stats_wiki_surf_range.csv";
std::ofstream output_stats_wiki_surf_range;


static const std::string file_lat_url_surf_range = output_dir_surf_range + "lat_url_surf_range.csv";
std::ofstream output_lat_url_surf_range;
static const std::string file_mem_url_surf_range = output_dir_surf_range + "mem_url_surf_range.csv";
std::ofstream output_mem_url_surf_range;
static const std::string file_fpr_url_surf_range = output_dir_surf_range + "fpr_url_surf_range.csv";
std::ofstream output_fpr_url_surf_range;
static const std::string file_stats_url_surf_range = output_dir_surf_range + "stats_url_surf_range.csv";
std::ofstream output_stats_url_surf_range;

static const std::string file_lat_ts_surf_range = output_dir_surf_range + "lat_ts_surf_range.csv";
std::ofstream output_lat_ts_surf_range;
static const std::string file_mem_ts_surf_range = output_dir_surf_range + "mem_ts_surf_range.csv";
std::ofstream output_mem_ts_surf_range;
static const std::string file_fpr_ts_surf_range = output_dir_surf_range + "fpr_ts_surf_range.csv";
std::ofstream output_fpr_ts_surf_range;
static const std::string file_stats_ts_surf_range = output_dir_surf_range + "stats_ts_surf_range.csv";
std::ofstream output_stats_ts_surf_range;


static const std::string output_dir_surfreal_range = "results/SuRF_real/range/";
static const std::string file_lat_email_surfreal_range = output_dir_surfreal_range + "lat_email_surfreal_range.csv";
std::ofstream output_lat_email_surfreal_range;
static const std::string file_mem_email_surfreal_range = output_dir_surfreal_range + "mem_email_surfreal_range.csv";
std::ofstream output_mem_email_surfreal_range;
static const std::string file_fpr_email_surfreal_range = output_dir_surfreal_range + "fpr_email_surfreal_range.csv";
std::ofstream output_fpr_email_surfreal_range;
static const std::string file_stats_email_surfreal_range = output_dir_surfreal_range + "stats_email_surfreal_range.csv";
std::ofstream output_stats_email_surfreal_range;


static const std::string file_lat_wiki_surfreal_range = output_dir_surfreal_range + "lat_wiki_surfreal_range.csv";
std::ofstream output_lat_wiki_surfreal_range;
static const std::string file_mem_wiki_surfreal_range = output_dir_surfreal_range + "mem_wiki_surfreal_range.csv";
std::ofstream output_mem_wiki_surfreal_range;
static const std::string file_fpr_wiki_surfreal_range = output_dir_surfreal_range + "fpr_wiki_surfreal_range.csv";
std::ofstream output_fpr_wiki_surfreal_range;
static const std::string file_stats_wiki_surfreal_range = output_dir_surfreal_range + "stats_wiki_surfreal_range.csv";
std::ofstream output_stats_wiki_surfreal_range;


static const std::string file_lat_url_surfreal_range = output_dir_surfreal_range + "lat_url_surfreal_range.csv";
std::ofstream output_lat_url_surfreal_range;
static const std::string file_mem_url_surfreal_range = output_dir_surfreal_range + "mem_url_surfreal_range.csv";
std::ofstream output_mem_url_surfreal_range;
static const std::string file_fpr_url_surfreal_range = output_dir_surfreal_range + "fpr_url_surfreal_range.csv";
std::ofstream output_fpr_url_surfreal_range;
static const std::string file_stats_url_surfreal_range = output_dir_surfreal_range + "stats_url_surfreal_range.csv";
std::ofstream output_stats_url_surfreal_range;

static const std::string file_lat_ts_surfreal_range = output_dir_surfreal_range + "lat_ts_surfreal_range.csv";
std::ofstream output_lat_ts_surfreal_range;
static const std::string file_mem_ts_surfreal_range = output_dir_surfreal_range + "mem_ts_surfreal_range.csv";
std::ofstream output_mem_ts_surfreal_range;
static const std::string file_fpr_ts_surfreal_range = output_dir_surfreal_range + "fpr_ts_surfreal_range.csv";
std::ofstream output_fpr_ts_surfreal_range;
static const std::string file_stats_ts_surfreal_range = output_dir_surfreal_range + "stats_ts_surfreal_range.csv";
std::ofstream output_stats_ts_surfreal_range;


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
    keys.push_back(key);
    count++;
    }
}

std::string uint64ToString(uint64_t key) {
    uint64_t endian_swapped_key = __builtin_bswap64(key);
    return std::string(reinterpret_cast<const char*>(&endian_swapped_key), 8);
}

void loadKeysInt(const std::string& file_name, const uint64_t num_records,
                 std::vector<std::string>& keys, bool remove_dup=false) {
    std::ifstream infile(file_name);
    uint64_t int_key;
    uint64_t count = 0;
    int continue_cnt = 0;
    std::set<uint64_t> int_keys;
    while (count < num_records && infile.good()) {
        infile >> int_key;
        if (remove_dup && int_keys.find(int_key) != int_keys.end()) {
            continue_cnt++;
            std::cout << "continue cnt:" << continue_cnt << std::endl;
            continue;
        }
        int_keys.insert(int_key);
        std::string key = uint64ToString(int_key);
        keys.push_back(key);
        count++;
    }
}

void selectKeysToInsert(const unsigned percent,
            std::vector<std::string> &insert_keys,
            std::vector<std::string> &keys) {
    random_shuffle(keys.begin(), keys.end());
    uint64_t num_insert_keys = keys.size() * percent / 100;
    for (uint64_t i = 0; i < num_insert_keys; i++)
    insert_keys.push_back(keys[i]);

    keys.clear();
    sort(insert_keys.begin(), insert_keys.end());
}

std::string getUpperBoundKey(const std::string& key) {
    std::string ret_str = key;
    ret_str[ret_str.size() - 1] = (char)'\255';
    return ret_str;
}

int64_t computePointTruePositives(const std::vector<std::string> insert_keys,
                  const std::vector<std::string> txn_keys) {
    std::map<std::string, bool> ht;
    for (int i = 0; i < (int)insert_keys.size(); i++)
    ht[insert_keys[i]] = true;

    int64_t true_positives = 0;
    std::map<std::string, bool>::iterator ht_iter;
    for (int i = 0; i < (int)txn_keys.size(); i++) {
    ht_iter = ht.find(txn_keys[i]);
    true_positives += (ht_iter != ht.end());
    }
    return true_positives;
}

int64_t computeRangeTruePositives(const std::vector<std::string> insert_keys,
                  const std::vector<std::string> txn_keys,
                  const std::vector<std::string> upper_bound_keys) {
    std::map<std::string, bool> ht;
    for (int i = 0; i < (int)insert_keys.size(); i++)
    ht[insert_keys[i]] = true;

    int64_t true_positives = 0;
    std::map<std::string, bool>::iterator ht_iter;
    for (int i = 0; i < (int)txn_keys.size(); i++) {
        ht_iter = ht.lower_bound(txn_keys[i]);
        if (ht_iter != ht.end()) {
            std::string fetched_key = ht_iter->first;
            true_positives += (fetched_key.compare(upper_bound_keys[i]) < 0);
        }
    }
    return true_positives;
}

void loadWorkload(int wkld_id,
          int64_t& point_tp, int64_t& range_tp,
          std::vector<std::string>& load_keys,
          std::vector<std::string>& insert_keys,
          std::vector<std::string>& insert_keys_shuffle,
          std::vector<std::string>& insert_keys_sample,
          std::vector<std::string>& txn_keys,
          std::vector<std::string>& upper_bound_keys) {
    if (wkld_id == kEmail)
        loadKeysFromFile(file_load_email, kNumEmailRecords, load_keys);
    else if (wkld_id == kWiki)
        loadKeysFromFile(file_load_wiki, kNumWikiRecords, load_keys);
    else if (wkld_id == kUrl)
        loadKeysFromFile(file_load_url, kNumEmailRecords, load_keys);
    else if (wkld_id == kTs)
        loadKeysInt(file_load_ts, kNumTsRecords, load_keys, true);
    else
        return;

    selectKeysToInsert(kPercent, insert_keys, load_keys);
    load_keys.clear();

    for (int i = 0; i < (int)insert_keys.size(); i++) {
        insert_keys_shuffle.push_back(insert_keys[i]);
    }
    std::random_shuffle(insert_keys_shuffle.begin(), insert_keys_shuffle.end());

    double current_percent = wkld_id == kUrl?kUrlSamplePercent : kSamplePercent;
    for (int i = 0; i < (int)insert_keys_shuffle.size(); i += int(100 / current_percent)) {
        insert_keys_sample.push_back(insert_keys_shuffle[i]);
    }
    insert_keys_shuffle.clear();

    if (wkld_id == kEmail)
        loadKeysFromFile(file_txn_email, kNumTxns, txn_keys);
    else if (wkld_id == kWiki)
        loadKeysFromFile(file_txn_wiki, kNumTxns, txn_keys);
    else if (wkld_id == kUrl)
        loadKeysFromFile(file_txn_url, kNumTxns, txn_keys);
    else if (wkld_id == kTs)
        loadKeysInt(file_txn_ts, kNumTxns, txn_keys);

    for (int i = 0; i < (int)txn_keys.size(); i++) {
        upper_bound_keys.push_back(getUpperBoundKey(txn_keys[i]));
    }
    std::cout << "insert_keys size = " << insert_keys.size() << std::endl;
    std::cout << "insert_keys_sample size = " << insert_keys_sample.size() << std::endl;
    std::cout << "txn_keys size = " << txn_keys.size() << std::endl;
    std::cout << "upper_bound_keys size = " << upper_bound_keys.size() << std::endl;

    point_tp = computePointTruePositives(insert_keys, txn_keys);
    range_tp = computeRangeTruePositives(insert_keys, txn_keys, upper_bound_keys);
}

void printKey(std::string key) {
    for (int i = 0; i < (int)key.size(); i++){
        int x = ( unsigned char )key[i];
        std::cout << x <<  " ";
    }
    std::cout << std::endl;
}

void exec(const int expt_id,
      const int wkld_id, const bool is_point,
      const int filter_type, const uint32_t suffix_len,
      const bool is_compressed,
      const int encoder_type, const int64_t dict_size_id,
      const int64_t point_true_positives,
      const int64_t range_true_positives,
      const std::vector<std::string>& insert_keys,
      const std::vector<std::string>& insert_keys_sample,
      const std::vector<std::string>& txn_keys,
      const std::vector<std::string>& upper_bound_keys) {
    ope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    uint8_t* buffer_r = new uint8_t[8192];
    std::vector<std::string> enc_insert_keys;

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
        encoder = ope::EncoderFactory::createEncoder(encoder_type, W);
        encoder->build(insert_keys_sample, input_dict_size);
        for (int i = 0; i < (int)insert_keys.size(); i++) {
            int enc_len = encoder->encode(insert_keys[i], buffer);
            int enc_len_round = (enc_len + 7) >> 3;
            enc_insert_keys.push_back(std::string((const char*)buffer, enc_len_round));
        }
    } else {
        for (int i = 0; i < (int)insert_keys.size(); i++) {
            enc_insert_keys.push_back(insert_keys[i]);
        }
    }

    surf::SuRF* filter = nullptr;
    if (filter_type == 0) { // SuRF
        filter = new surf::SuRF(enc_insert_keys);
    } else if (filter_type == 1) { // SuRFHash
        filter = new surf::SuRF(enc_insert_keys, surf::kHash, suffix_len, 0);
    } else if (filter_type == 2) { // SuRFReal
        filter = new surf::SuRF(enc_insert_keys, surf::kReal, 0, suffix_len);
    }
    double end_time = getNow();
    double bt = end_time - start_time;
    std::cout << "Build time = " << bt << std::endl;

    double height = filter->getAvgHeight();
    std::cout << "Avg Trie Height = " << height << std::endl;

    // execute transactions =======================================
    int64_t positives = 0;
    start_time = getNow();
    if (is_point) { // point query
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                int enc_len = encoder->encode(txn_keys[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                positives += (int)filter->lookupKey(std::string((const char*)buffer, enc_len_round));
            }
        } else {
            for (int i = 0; i < (int)txn_keys.size(); i++)
            positives += (int)filter->lookupKey(txn_keys[i]);
        }
    } else { // range query
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
            int enc_len = 0, enc_len_r = 0;
                if (encoder_type == 5) {
                    enc_len = encoder->encode(txn_keys[i], buffer);
                    enc_len_r = encoder->encode(upper_bound_keys[i], buffer_r);
                } else {
                    encoder->encodePair(txn_keys[i], upper_bound_keys[i], buffer, buffer_r, enc_len, enc_len_r);
                }
                int enc_len_round = (enc_len + 7) >> 3;
                int enc_len_r_round = (enc_len_r + 7) >> 3;
                std::string left_key = std::string((const char*)buffer, enc_len_round);
                std::string right_key = std::string((const char*)buffer_r, enc_len_r_round);
                positives += (int)filter->lookupRange(left_key, true, right_key, true);
            }
        } else {
            for (int i = 0; i < (int)txn_keys.size(); i++)
            positives += (int)filter->lookupRange(txn_keys[i], true, upper_bound_keys[i], true);
        }
    }
    end_time = getNow();
    double exec_time = end_time - start_time;
    double tput = txn_keys.size() / exec_time / 1000000; // Mops/sec
    std::cout << kGreen << "Throughput = " << kNoColor << tput << "\n";
    double lat = (exec_time * 1000000) / txn_keys.size(); // us
    std::cout << kGreen << "Latency = " << kNoColor << lat << "\n";

    int64_t false_positives = 0;
    if (is_point)
        false_positives = positives - point_true_positives;
    else
        false_positives = positives - range_true_positives;
    assert(false_positives >= 0);

    int64_t true_negatives = txn_keys.size() - positives;
    double fpr = (false_positives + 0.0) / (true_negatives + false_positives);
    std::cout << kGreen << "False Positive Rate = " << kNoColor << fpr << "\n";
    //double mem = (filter->getMemoryUsage() + 0.0) / 1000000; // MB
    double mem = filter->getMemoryUsage() + 0.0;
    double filter_mem = mem/1000000.0;
    double encoder_mem = 0;
    std::cout << "filter size:" << filter_mem << std::endl;
    if (encoder != nullptr) {
        std::cout << "encoder size:" << encoder->memoryUse() << std::endl;
        mem += encoder->memoryUse();
        encoder_mem = encoder->memoryUse() / 1000000.0;
    }
    mem /= 1000000; // MB
    std::cout << kGreen << "Memory = " << kNoColor << mem << "\n\n";

    delete encoder;
    delete[] buffer;
    delete[] buffer_r;
    if (expt_id == 0) {
        if (filter_type == 0) {
            if (wkld_id == kEmail) {
                output_lat_email_surf << lat << "\n";
                output_mem_email_surf << mem << "\n";
                output_fpr_email_surf << fpr << "\n";
                output_height_email_surf << height << "\n";
                output_stats_email_surf << mem << "," << filter_mem << "," << encoder_mem << "\n";
            } else if (wkld_id == kWiki) {
                output_lat_wiki_surf << lat << "\n";
                output_mem_wiki_surf << mem << "\n";
                output_fpr_wiki_surf << fpr << "\n";
                output_height_wiki_surf << height << "\n";
                output_stats_wiki_surf << mem << "," << filter_mem << "," << encoder_mem << "\n";
            } else if (wkld_id == kUrl) {
                output_lat_url_surf << lat << "\n";
                output_mem_url_surf << mem << "\n";
                output_fpr_url_surf << fpr << "\n";
                output_height_url_surf << height << "\n";
                output_stats_url_surf << mem << "," << filter_mem << "," << encoder_mem << "\n";
            } else if (wkld_id == kTs) {
                output_lat_ts_surf << lat << "\n";
                output_mem_ts_surf << mem << "\n";
                output_fpr_ts_surf << fpr << "\n";
                output_height_ts_surf << height << "\n";
                output_stats_ts_surf << mem << "," << filter_mem << "," << encoder_mem << "\n";
            }
        } else if (filter_type == 2) {
            if (wkld_id == kEmail) {
                output_lat_email_surfreal << lat << "\n";
                output_mem_email_surfreal << mem << "\n";
                output_fpr_email_surfreal << fpr << "\n";
                output_stats_email_surfreal << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kWiki) {
                output_lat_wiki_surfreal << lat << "\n";
                output_mem_wiki_surfreal << mem << "\n";
                output_fpr_wiki_surfreal << fpr << "\n";
                output_stats_wiki_surfreal << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kUrl) {
                output_lat_url_surfreal << lat << "\n";
                output_mem_url_surfreal << mem << "\n";
                output_fpr_url_surfreal << fpr << "\n";
                output_stats_url_surfreal << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kTs) {
                output_lat_ts_surfreal << lat << "\n";
                output_mem_ts_surfreal << mem << "\n";
                output_fpr_ts_surfreal << fpr << "\n";
                output_stats_ts_surfreal << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            }
        }
    } else if (expt_id == 1) {
        if (filter_type == 0) {
            if (wkld_id == kEmail) {
                output_lat_email_surf_range << lat << "\n";
                output_mem_email_surf_range << mem << "\n";
                output_fpr_email_surf_range << fpr << "\n";
                output_stats_email_surf_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kWiki) {
                output_lat_wiki_surf_range << lat << "\n";
                output_mem_wiki_surf_range << mem << "\n";
                output_fpr_wiki_surf_range << fpr << "\n";
                output_stats_wiki_surf_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kUrl) {
                output_lat_url_surf_range << lat << "\n";
                output_mem_url_surf_range << mem << "\n";
                output_fpr_url_surf_range << fpr << "\n";
                output_stats_url_surf_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            }  else if (wkld_id == kTs) {
                output_lat_ts_surf_range << lat << "\n";
                output_mem_ts_surf_range << mem << "\n";
                output_fpr_ts_surf_range << fpr << "\n";
                output_stats_ts_surf_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            }

        } else if (filter_type == 2) {
            if (wkld_id == kEmail) {
                output_lat_email_surfreal_range << lat << "\n";
                output_mem_email_surfreal_range << mem << "\n";
                output_fpr_email_surfreal_range << fpr << "\n";
                output_stats_email_surfreal_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kWiki) {
                output_lat_wiki_surfreal_range << lat << "\n";
                output_mem_wiki_surfreal_range << mem << "\n";
                output_fpr_wiki_surfreal_range << fpr << "\n";
                output_stats_wiki_surfreal_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            } else if (wkld_id == kUrl) {
                output_lat_url_surfreal_range << lat << "\n";
                output_mem_url_surfreal_range << mem << "\n";
                output_fpr_url_surfreal_range << fpr << "\n";
                output_stats_url_surfreal_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            }  else if (wkld_id == kTs) {
                output_lat_ts_surfreal_range << lat << "\n";
                output_mem_ts_surfreal_range << mem << "\n";
                output_fpr_ts_surfreal_range << fpr << "\n";
                output_stats_ts_surfreal_range << mem << "," << filter_mem <<  "," << encoder_mem <<"\n";
            }
        }
    }
}

void exec_group(const int expt_id,
        const int filter_type, const uint32_t suffix_len,
        const bool is_point,
        int& expt_num, const int total_num_expt,
        const int64_t email_point_tp, const int64_t email_range_tp,
        const int64_t wiki_point_tp, const int64_t wiki_range_tp,
        const int64_t url_point_tp, const int64_t url_range_tp,
        const int64_t ts_point_tp, const int64_t ts_range_tp,
        const std::vector<std::string>& insert_emails,
        const std::vector<std::string>& insert_emails_sample,
        const std::vector<std::string>& txn_emails,
        const std::vector<std::string>& upper_bound_emails,
        const std::vector<std::string>& insert_wikis,
        const std::vector<std::string>& insert_wikis_sample,
        const std::vector<std::string>& txn_wikis,
        const std::vector<std::string>& upper_bound_wikis,
        const std::vector<std::string>& insert_urls,
        const std::vector<std::string>& insert_urls_sample,
        const std::vector<std::string>& txn_urls,
        const std::vector<std::string>& upper_bound_urls,
        const std::vector<std::string>& insert_tss,
        const std::vector<std::string>& insert_tss_sample,
        const std::vector<std::string>& txn_tss,
        const std::vector<std::string>& upper_bound_tss) {
    int dict_size[2] = {3, 6};

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, filter_type, suffix_len,
     false, 0, 0, email_point_tp, email_range_tp,
     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, filter_type, suffix_len,
     false, 0, 0, wiki_point_tp, wiki_range_tp,
     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, filter_type, suffix_len,
     false, 0, 0, url_point_tp, url_range_tp,
     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, filter_type, suffix_len,
     false, 0, 0, ts_point_tp, ts_range_tp,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    //=================================================
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, filter_type, suffix_len,
     true, 1, 0, email_point_tp, email_range_tp,
     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, filter_type, suffix_len,
     true, 1, 0, wiki_point_tp, wiki_range_tp,
     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, filter_type, suffix_len,
     true, 1, 0, url_point_tp, url_range_tp,
     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, filter_type, suffix_len,
     true, 1, 0, ts_point_tp, ts_range_tp,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    //=================================================
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, filter_type, suffix_len,
     true, 2, 6, email_point_tp, email_range_tp,
     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, filter_type, suffix_len,
     true, 2, 6, wiki_point_tp, wiki_range_tp,
     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, filter_type, suffix_len,
     true, 2, 6, url_point_tp, url_range_tp,
     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, filter_type, suffix_len,
     true, 2, 6, ts_point_tp, ts_range_tp,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, filter_type, suffix_len,
        true, 3, 6, email_point_tp, email_range_tp,
        insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, filter_type, suffix_len,
        true, 3, 6, wiki_point_tp, wiki_range_tp,
        insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, filter_type, suffix_len,
        true, 3, 6, url_point_tp, url_range_tp,
        insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, filter_type, suffix_len,
         true, 3, 6, ts_point_tp, ts_range_tp,
         insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, filter_type, suffix_len,
        true, 4, 6, email_point_tp, email_range_tp,
        insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, filter_type, suffix_len,
        true, 4, 6, wiki_point_tp, wiki_range_tp,
        insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, filter_type, suffix_len,
        true, 4, 6, url_point_tp, url_range_tp,
        insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, filter_type, suffix_len,
         true, 4, 6, ts_point_tp, ts_range_tp,
         insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
        for (int j = 0; j < 2; j++) {
            std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
            exec(expt_id, kEmail, is_point, filter_type, suffix_len,
                 true, encoder_type, dict_size[j], email_point_tp, email_range_tp,
                 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
            expt_num++;

            std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
            exec(expt_id, kWiki, is_point, filter_type, suffix_len,
                 true, encoder_type, dict_size[j], wiki_point_tp, wiki_range_tp,
                 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
            expt_num++;

            std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
            exec(expt_id, kUrl, is_point, filter_type, suffix_len,
                 true, encoder_type, dict_size[j], url_point_tp, url_range_tp,
                 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
            expt_num++;
        }
    }
}

int main(int argc, char *argv[]) {
    int expt_id = (int)atoi(argv[1]);

    //-------------------------------------------------------------
    // Init Workloads
    //-------------------------------------------------------------
    std::vector<std::string> load_emails, insert_emails, insert_emails_shuffle,
    insert_emails_sample, txn_emails, upper_bound_emails;
    int64_t email_point_tp=0, email_range_tp=0;
    loadWorkload(kEmail, email_point_tp, email_range_tp,
         load_emails, insert_emails, insert_emails_shuffle,
         insert_emails_sample, txn_emails, upper_bound_emails);

    std::vector<std::string> load_wikis, insert_wikis, insert_wikis_shuffle,
    insert_wikis_sample, txn_wikis, upper_bound_wikis;
    int64_t wiki_point_tp=0, wiki_range_tp=0;
    loadWorkload(kWiki, wiki_point_tp, wiki_range_tp,
         load_wikis, insert_wikis, insert_wikis_shuffle,
         insert_wikis_sample, txn_wikis, upper_bound_wikis);

    std::vector<std::string> load_urls, insert_urls, insert_urls_shuffle,
    insert_urls_sample, txn_urls, upper_bound_urls;
    int64_t url_point_tp=0, url_range_tp=0;
    loadWorkload(kUrl, url_point_tp, url_range_tp,
         load_urls, insert_urls, insert_urls_shuffle,
         insert_urls_sample, txn_urls, upper_bound_urls);

    std::vector<std::string> load_tss, insert_tss, insert_tss_shuffle,
    insert_tss_sample, txn_tss, upper_bound_tss;
    int64_t ts_point_tp, ts_range_tp;
#ifdef RUN_TIMESTAMP
    loadWorkload(kTs, ts_point_tp, ts_range_tp,
         load_tss, insert_tss, insert_tss_shuffle,
         insert_tss_sample, txn_tss, upper_bound_tss);
#else
    ts_point_tp = 0;
    ts_range_tp = 0;
#endif

    if (expt_id == 0) {
        //-------------------------------------------------------------
        // Point Queries; Expt ID = 0
        //-------------------------------------------------------------
        std::cout << "====================================" << std::endl;
        std::cout << "Point Queries; Expt ID = 0" << std::endl;
        std::cout << "====================================" << std::endl;

        output_lat_email_surf.open(file_lat_email_surf,std::ofstream::app);
        output_mem_email_surf.open(file_mem_email_surf,std::ofstream::app);
        output_fpr_email_surf.open(file_fpr_email_surf,std::ofstream::app);
        output_height_email_surf.open(file_height_email_surf,std::ofstream::app);
        output_stats_email_surf.open(file_stats_email_surf,std::ofstream::app);

        output_lat_wiki_surf.open(file_lat_wiki_surf,std::ofstream::app);
        output_mem_wiki_surf.open(file_mem_wiki_surf,std::ofstream::app);
        output_fpr_wiki_surf.open(file_fpr_wiki_surf,std::ofstream::app);
        output_height_wiki_surf.open(file_height_wiki_surf,std::ofstream::app);
        output_stats_wiki_surf.open(file_stats_wiki_surf,std::ofstream::app);

        output_lat_url_surf.open(file_lat_url_surf,std::ofstream::app);
        output_mem_url_surf.open(file_mem_url_surf,std::ofstream::app);
        output_fpr_url_surf.open(file_fpr_url_surf,std::ofstream::app);
        output_height_url_surf.open(file_height_url_surf,std::ofstream::app);
        output_stats_url_surf.open(file_stats_url_surf,std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lat_ts_surf.open(file_lat_ts_surf,std::ofstream::app);
        output_mem_ts_surf.open(file_mem_ts_surf,std::ofstream::app);
        output_fpr_ts_surf.open(file_fpr_ts_surf,std::ofstream::app);
        output_height_ts_surf.open(file_height_ts_surf,std::ofstream::app);
        output_stats_ts_surf.open(file_stats_ts_surf,std::ofstream::app);
#endif
        output_lat_email_surfreal.open(file_lat_email_surfreal,std::ofstream::app);
        output_mem_email_surfreal.open(file_mem_email_surfreal,std::ofstream::app);
        output_fpr_email_surfreal.open(file_fpr_email_surfreal,std::ofstream::app);
        output_stats_email_surfreal.open(file_stats_email_surfreal,std::ofstream::app);

        output_lat_wiki_surfreal.open(file_lat_wiki_surfreal,std::ofstream::app);
        output_mem_wiki_surfreal.open(file_mem_wiki_surfreal,std::ofstream::app);
        output_fpr_wiki_surfreal.open(file_fpr_wiki_surfreal,std::ofstream::app);
        output_stats_wiki_surfreal.open(file_stats_wiki_surfreal,std::ofstream::app);

        output_lat_url_surfreal.open(file_lat_url_surfreal,std::ofstream::app);
        output_mem_url_surfreal.open(file_mem_url_surfreal,std::ofstream::app);
        output_fpr_url_surfreal.open(file_fpr_url_surfreal,std::ofstream::app);
        output_stats_url_surfreal.open(file_stats_url_surfreal,std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lat_ts_surfreal.open(file_lat_ts_surfreal,std::ofstream::app);
        output_mem_ts_surfreal.open(file_mem_ts_surfreal,std::ofstream::app);
        output_fpr_ts_surfreal.open(file_fpr_ts_surfreal,std::ofstream::app);
        output_stats_ts_surfreal.open(file_stats_ts_surfreal,std::ofstream::app);
#endif
        bool is_point = true;
        int expt_num = 1;
        int total_num_expt = 54;
        exec_group(expt_id, 0, 0, is_point, expt_num, total_num_expt,
               email_point_tp, email_range_tp,
               wiki_point_tp, wiki_range_tp,
               url_point_tp, url_range_tp,
               ts_point_tp, ts_range_tp,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        exec_group(expt_id, 2, 8, is_point, expt_num, total_num_expt,
               email_point_tp, email_range_tp,
               wiki_point_tp, wiki_range_tp,
               url_point_tp, url_range_tp,
               ts_point_tp, ts_range_tp,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        output_lat_email_surf << "-" << "\n";
        output_mem_email_surf << "-" << "\n";
        output_fpr_email_surf << "-" << "\n";
        output_height_email_surf << "-" << "\n";
        output_stats_email_surf << "-" << "\n";

        output_lat_email_surf.close();
        output_mem_email_surf.close();
        output_fpr_email_surf.close();
        output_height_email_surf.close();
        output_stats_email_surf.close();

        output_lat_wiki_surf << "-" << "\n";
        output_mem_wiki_surf << "-" << "\n";
        output_fpr_wiki_surf << "-" << "\n";
        output_height_wiki_surf << "-" << "\n";
        output_stats_wiki_surf << "-" << "\n";

        output_lat_wiki_surf.close();
        output_mem_wiki_surf.close();
        output_fpr_wiki_surf.close();
        output_height_wiki_surf.close();
        output_stats_wiki_surf.close();

        output_lat_url_surf << "-" << "\n";
        output_mem_url_surf << "-" << "\n";
        output_fpr_url_surf << "-" << "\n";
        output_height_url_surf << "-" << "\n";
        output_stats_url_surf << "-" << "\n";

        output_lat_url_surf.close();
        output_mem_url_surf.close();
        output_fpr_url_surf.close();
        output_height_url_surf.close();
        output_stats_url_surf.close();
#ifdef RUN_TIMESTAMP
        output_lat_ts_surf << "-" << "\n";
        output_mem_ts_surf << "-" << "\n";
        output_fpr_ts_surf << "-" << "\n";
        output_height_ts_surf << "-" << "\n";
        output_stats_ts_surf << "-" << "\n";

        output_lat_ts_surf.close();
        output_mem_ts_surf.close();
        output_fpr_ts_surf.close();
        output_height_ts_surf.close();
        output_stats_ts_surf.close();
#endif
        output_lat_email_surfreal << "-" << "\n";
        output_mem_email_surfreal << "-" << "\n";
        output_fpr_email_surfreal << "-" << "\n";
        output_stats_email_surfreal << "-" << "\n";

        output_lat_email_surfreal.close();
        output_mem_email_surfreal.close();
        output_fpr_email_surfreal.close();
        output_stats_email_surfreal.close();

        output_lat_wiki_surfreal << "-" << "\n";
        output_mem_wiki_surfreal << "-" << "\n";
        output_fpr_wiki_surfreal << "-" << "\n";
        output_stats_wiki_surfreal << "-" << "\n";

        output_lat_wiki_surfreal.close();
        output_mem_wiki_surfreal.close();
        output_fpr_wiki_surfreal.close();
        output_stats_wiki_surfreal.close();

        output_lat_url_surfreal << "-" << "\n";
        output_mem_url_surfreal << "-" << "\n";
        output_fpr_url_surfreal << "-" << "\n";
        output_stats_url_surfreal << "-" << "\n";

        output_lat_url_surfreal.close();
        output_mem_url_surfreal.close();
        output_fpr_url_surfreal.close();
        output_stats_url_surfreal.close();
#ifdef RUN_TIMESTAMP
        output_lat_ts_surfreal << "-" << "\n";
        output_mem_ts_surfreal << "-" << "\n";
        output_fpr_ts_surfreal << "-" << "\n";
        output_stats_ts_surfreal << "-" << "\n";

        output_lat_ts_surfreal.close();
        output_mem_ts_surfreal.close();
        output_fpr_ts_surfreal.close();
        output_stats_ts_surfreal.close();
#endif
    } else if (expt_id == 1) {
        //-------------------------------------------------------------
        // Range Queries; Expt ID = 1
        //-------------------------------------------------------------
        std::cout << "====================================" << std::endl;
        std::cout << "Range Queries; Expt ID = 1" << std::endl;
        std::cout << "====================================" << std::endl;

        output_lat_email_surf_range.open(file_lat_email_surf_range,std::ofstream::app);
        output_mem_email_surf_range.open(file_mem_email_surf_range,std::ofstream::app);
        output_fpr_email_surf_range.open(file_fpr_email_surf_range,std::ofstream::app);
        output_stats_email_surf_range.open(file_stats_email_surf_range,std::ofstream::app);

        output_lat_wiki_surf_range.open(file_lat_wiki_surf_range,std::ofstream::app);
        output_mem_wiki_surf_range.open(file_mem_wiki_surf_range,std::ofstream::app);
        output_fpr_wiki_surf_range.open(file_fpr_wiki_surf_range,std::ofstream::app);
        output_stats_wiki_surf_range.open(file_stats_wiki_surf_range,std::ofstream::app);

        output_lat_url_surf_range.open(file_lat_url_surf_range,std::ofstream::app);
        output_mem_url_surf_range.open(file_mem_url_surf_range,std::ofstream::app);
        output_fpr_url_surf_range.open(file_fpr_url_surf_range,std::ofstream::app);
        output_stats_url_surf_range.open(file_stats_url_surf_range,std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lat_ts_surf_range.open(file_lat_ts_surf_range,std::ofstream::app);
        output_mem_ts_surf_range.open(file_mem_ts_surf_range,std::ofstream::app);
        output_fpr_ts_surf_range.open(file_fpr_ts_surf_range,std::ofstream::app);
        output_stats_ts_surf_range.open(file_stats_ts_surf_range,std::ofstream::app);
#endif
        output_lat_email_surfreal_range.open(file_lat_email_surfreal_range,std::ofstream::app);
        output_mem_email_surfreal_range.open(file_mem_email_surfreal_range,std::ofstream::app);
        output_fpr_email_surfreal_range.open(file_fpr_email_surfreal_range,std::ofstream::app);
        output_stats_email_surfreal_range.open(file_stats_email_surfreal_range,std::ofstream::app);

        output_lat_wiki_surfreal_range.open(file_lat_wiki_surfreal_range,std::ofstream::app);
        output_mem_wiki_surfreal_range.open(file_mem_wiki_surfreal_range,std::ofstream::app);
        output_fpr_wiki_surfreal_range.open(file_fpr_wiki_surfreal_range,std::ofstream::app);
        output_stats_wiki_surfreal_range.open(file_stats_wiki_surfreal_range,std::ofstream::app);

        output_lat_url_surfreal_range.open(file_lat_url_surfreal_range,std::ofstream::app);
        output_mem_url_surfreal_range.open(file_mem_url_surfreal_range,std::ofstream::app);
        output_fpr_url_surfreal_range.open(file_fpr_url_surfreal_range,std::ofstream::app);
        output_stats_url_surfreal_range.open(file_stats_url_surfreal_range,std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lat_ts_surfreal_range.open(file_lat_ts_surfreal_range,std::ofstream::app);
        output_mem_ts_surfreal_range.open(file_mem_ts_surfreal_range,std::ofstream::app);
        output_fpr_ts_surfreal_range.open(file_fpr_ts_surfreal_range,std::ofstream::app);
        output_stats_ts_surfreal_range.open(file_stats_ts_surfreal_range,std::ofstream::app);
#endif
        bool is_point = false;
        int expt_num = 1;
        int total_num_expt = 54;
        exec_group(expt_id, 0, 0, is_point, expt_num, total_num_expt,
               email_point_tp, email_range_tp,
               wiki_point_tp, wiki_range_tp,
               url_point_tp, url_range_tp,
               ts_point_tp, ts_range_tp,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        exec_group(expt_id, 2, 8, is_point, expt_num, total_num_expt,
               email_point_tp, email_range_tp,
               wiki_point_tp, wiki_range_tp,
               url_point_tp, url_range_tp,
               ts_point_tp, ts_range_tp,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        output_lat_email_surf_range << "-" << "\n";
        output_mem_email_surf_range << "-" << "\n";
        output_fpr_email_surf_range << "-" << "\n";
        output_stats_email_surf_range << "-" << "\n";

        output_lat_email_surf_range.close();
        output_mem_email_surf_range.close();
        output_fpr_email_surf_range.close();
        output_stats_email_surf_range.close();

        output_lat_wiki_surf_range << "-" << "\n";
        output_mem_wiki_surf_range << "-" << "\n";
        output_fpr_wiki_surf_range << "-" << "\n";
        output_stats_wiki_surf_range << "-" << "\n";

        output_lat_wiki_surf_range.close();
        output_mem_wiki_surf_range.close();
        output_fpr_wiki_surf_range.close();
        output_stats_wiki_surf_range.close();

        output_lat_url_surf_range << "-" << "\n";
        output_mem_url_surf_range << "-" << "\n";
        output_fpr_url_surf_range << "-" << "\n";
        output_stats_url_surf_range << "-" << "\n";

        output_lat_url_surf_range.close();
        output_mem_url_surf_range.close();
        output_fpr_url_surf_range.close();
        output_stats_url_surf_range.close();
#ifdef RUN_TIMESTAMP
        output_lat_ts_surf_range << "-" << "\n";
        output_mem_ts_surf_range << "-" << "\n";
        output_fpr_ts_surf_range << "-" << "\n";
        output_stats_ts_surf_range << "-" << "\n";

        output_lat_ts_surf_range.close();
        output_mem_ts_surf_range.close();
        output_fpr_ts_surf_range.close();
        output_stats_ts_surf_range.close();
#endif
        output_lat_email_surfreal_range << "-" << "\n";
        output_mem_email_surfreal_range << "-" << "\n";
        output_fpr_email_surfreal_range << "-" << "\n";
        output_stats_email_surfreal_range << "-" << "\n";

        output_lat_email_surfreal_range.close();
        output_mem_email_surfreal_range.close();
        output_fpr_email_surfreal_range.close();
        output_stats_email_surfreal_range.close();

        output_lat_wiki_surfreal_range << "-" << "\n";
        output_mem_wiki_surfreal_range << "-" << "\n";
        output_fpr_wiki_surfreal_range << "-" << "\n";
        output_stats_wiki_surfreal_range << "-" << "\n";

        output_lat_wiki_surfreal_range.close();
        output_mem_wiki_surfreal_range.close();
        output_fpr_wiki_surfreal_range.close();
        output_stats_wiki_surfreal_range.close();

        output_lat_url_surfreal_range << "-" << "\n";
        output_mem_url_surfreal_range << "-" << "\n";
        output_fpr_url_surfreal_range << "-" << "\n";
        output_stats_url_surfreal_range << "-" << "\n";

        output_lat_url_surfreal_range.close();
        output_mem_url_surfreal_range.close();
        output_fpr_url_surfreal_range.close();
        output_stats_url_surfreal_range.close();
#ifdef RUN_TIMESTAMP
        output_lat_ts_surfreal_range << "-" << "\n";
        output_mem_ts_surfreal_range << "-" << "\n";
        output_fpr_ts_surfreal_range << "-" << "\n";
        output_stats_ts_surfreal_range << "-" << "\n";

        output_lat_ts_surfreal_range.close();
        output_mem_ts_surfreal_range.close();
        output_fpr_ts_surfreal_range.close();
        output_stats_ts_surfreal_range.close();
#endif
    }
    return 0;
}
