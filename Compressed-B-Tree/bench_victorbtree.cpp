#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <set>

#include "encoder_factory.hpp"
#include "BTreeOLC.h"

//#define NOT_USE_ENCODE_PAIR 1
//#define RUN_TIMESTAMP

static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000000;
static const uint64_t kNumTsRecords = 14000000;
static const uint64_t kNumTxns = 10000000;

static const int kSamplePercent = 1;
static const double kUrlSamplePercent = 1;

static const std::string file_load_email = "workloads/load_email";
static const std::string file_load_wiki = "workloads/load_wiki";
static const std::string file_load_url = "workloads/load_url";
static const std::string file_load_ts = "workloads/load_timestamp";

static const std::string file_txn_email = "workloads/txn_email_zipfian";
static const std::string file_txn_email_len = "workloads/scan_len_email_zipfian";
static const std::string file_txn_wiki = "workloads/txn_wiki_zipfian";
static const std::string file_txn_wiki_len = "workloads/scan_len_wiki_zipfian";
static const std::string file_txn_url = "workloads/txn_url_zipfian";
static const std::string file_txn_url_len = "workloads/scan_len_url_zipfian";
static const std::string file_txn_ts  = "workloads/txn_timestamp_zipfian";

// for pretty print
static const char* kGreen ="\033[0;32m";
//static const char* kRed ="\033[0;31m";
static const char* kNoColor ="\033[0;0m";

static int runALM = 1;
static std::string endStr = std::string(255, char(255));
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
static const std::string output_dir_btree_point = "results/victorbtree/point/";
static const std::string file_lookuplat_email_btree = output_dir_btree_point + "lookuplat_email_btree.csv";
std::ofstream output_lookuplat_email_btree;
static const std::string file_insertlat_email_btree = output_dir_btree_point + "insertlat_email_btree.csv";
std::ofstream output_insertlat_email_btree;
static const std::string file_mem_email_btree = output_dir_btree_point + "mem_email_btree.csv";
std::ofstream output_mem_email_btree;

static const std::string file_lookuplat_wiki_btree = output_dir_btree_point + "lookuplat_wiki_btree.csv";
std::ofstream output_lookuplat_wiki_btree;
static const std::string file_insertlat_wiki_btree = output_dir_btree_point + "insertlat_wiki_btree.csv";
std::ofstream output_insertlat_wiki_btree;
static const std::string file_mem_wiki_btree = output_dir_btree_point + "mem_wiki_btree.csv";
std::ofstream output_mem_wiki_btree;

static const std::string file_lookuplat_url_btree = output_dir_btree_point + "lookuplat_url_btree.csv";
std::ofstream output_lookuplat_url_btree;
static const std::string file_insertlat_url_btree = output_dir_btree_point + "insertlat_url_btree.csv";
std::ofstream output_insertlat_url_btree;
static const std::string file_mem_url_btree = output_dir_btree_point + "mem_url_btree.csv";
std::ofstream output_mem_url_btree;

static const std::string file_lookuplat_ts_btree = output_dir_btree_point + "lookuplat_ts_btree.csv";
std::ofstream output_lookuplat_ts_btree;
static const std::string file_insertlat_ts_btree = output_dir_btree_point + "insertlat_ts_btree.csv";
std::ofstream output_insertlat_ts_btree;
static const std::string file_mem_ts_btree = output_dir_btree_point + "mem_ts_btree.csv";
std::ofstream output_mem_ts_btree;

//-------------------------------------------------------------
// Expt ID = 1
//-------------------------------------------------------------
static const std::string output_dir_btree_range = "results/victorbtree/range/";
static const std::string file_lookuplat_email_btree_range = output_dir_btree_range + "lookuplat_email_btree_range.csv";
std::ofstream output_lookuplat_email_btree_range;
static const std::string file_insertlat_email_btree_range = output_dir_btree_range + "insertlat_email_btree_range.csv";
std::ofstream output_insertlat_email_btree_range;
static const std::string file_mem_email_btree_range = output_dir_btree_range + "mem_email_btree_range.csv";
std::ofstream output_mem_email_btree_range;

static const std::string file_lookuplat_wiki_btree_range = output_dir_btree_range + "lookuplat_wiki_btree_range.csv";
std::ofstream output_lookuplat_wiki_btree_range;
static const std::string file_insertlat_wiki_btree_range = output_dir_btree_range + "insertlat_wiki_btree_range.csv";
std::ofstream output_insertlat_wiki_btree_range;
static const std::string file_mem_wiki_btree_range = output_dir_btree_range + "mem_wiki_btree_range.csv";
std::ofstream output_mem_wiki_btree_range;

static const std::string file_lookuplat_url_btree_range = output_dir_btree_range + "lookuplat_url_btree_range.csv";
std::ofstream output_lookuplat_url_btree_range;
static const std::string file_insertlat_url_btree_range = output_dir_btree_range + "insertlat_url_btree_range.csv";
std::ofstream output_insertlat_url_btree_range;
static const std::string file_mem_url_btree_range = output_dir_btree_range + "mem_url_btree_range.csv";
std::ofstream output_mem_url_btree_range;

static const std::string file_lookuplat_ts_btree_range = output_dir_btree_range + "lookuplat_ts_btree_range.csv";
std::ofstream output_lookuplat_ts_btree_range;
static const std::string file_insertlat_ts_btree_range = output_dir_btree_range + "insertlat_ts_btree_range.csv";
std::ofstream output_insertlat_ts_btree_range;
static const std::string file_mem_ts_btree_range = output_dir_btree_range + "mem_ts_btree_range.csv";
std::ofstream output_mem_ts_btree_range;

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

void loadLensInt(const std::string& file_name, const uint64_t num_records,
              std::vector<int> &keys) {
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

std::string getUpperBoundKey(const std::string& key) {
    std::string ret_str = key;
    ret_str[ret_str.size() - 1] = (char)'\255';
    return ret_str;
}

void loadWorkload(int wkld_id,
          std::vector<std::string>& insert_keys,
          std::vector<std::string>& insert_keys_sample,
          std::vector<std::string>& txn_keys,
          std::vector<int>& scan_key_lens) {
    std::vector<std::string> load_keys;
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

    std::sort(load_keys.begin(), load_keys.end());
    for (int i = 0; i < (int)load_keys.size() - 1; i++) {
        //int key_len = load_keys[i].length();
        //int next_key_len = load_keys[i + 1].length();
        //if (key_len < next_key_len) {
        //    std::string next_prefix = load_keys[i + 1].substr(0, key_len);
        //    if (load_keys[i].compare(next_prefix) != 0)
        //    insert_keys.push_back(load_keys[i]);
        //} else {
            insert_keys.push_back(load_keys[i]);
        //}
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
    } else if (wkld_id == kWiki) {
        loadKeysFromFile(file_txn_wiki, kNumTxns, txn_keys);
    } else if (wkld_id == kUrl) {
        loadKeysFromFile(file_txn_url, kNumTxns, txn_keys);
    } else if (wkld_id == kTs)
        loadKeysInt(file_txn_ts, kNumTxns, txn_keys);

    loadLensInt(file_txn_email_len, kNumTxns, scan_key_lens);
    std::cout << "insert_keys size = " << insert_keys.size() << std::endl;
    std::cout << "insert_keys_sample size = " << insert_keys_sample.size() << std::endl;
    std::cout << "txn_keys size = " << txn_keys.size() << std::endl;
    std::cout << "scan_key_lens size = " << scan_key_lens.size() << std::endl;
}

void exec(const int expt_id, const int wkld_id, const bool is_point,
      const bool is_compressed,
      const int encoder_type, const int64_t dict_size_id,
      const std::vector<std::string>& insert_keys,
      const std::vector<std::string>& insert_keys_sample,
      const std::vector<std::string>& txn_keys,
      const std::vector<int>& scan_key_lens) {
    ope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    std::vector<std::pair<std::string, std::string> >enc_insert_keys;

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

    int64_t total_key_size = 0;
    double start_time = getNow();
    if (is_compressed) {
        encoder = ope::EncoderFactory::createEncoder(encoder_type, W);
        encoder->build(insert_keys_sample, input_dict_size);
    }

    for (int i = 0; i < (int)insert_keys.size(); i++) {
        std::string encode_str;
        if (is_compressed) {
            int enc_len = encoder->encode(insert_keys[i], buffer);
            int enc_len_round = (enc_len + 7) >> 3;
            encode_str = std::string((const char*)buffer, enc_len_round);
            total_key_size += enc_len_round;
        } else {
            encode_str = insert_keys[i];
            total_key_size += insert_keys[i].size();
        }
        enc_insert_keys.push_back(std::make_pair(insert_keys[i], encode_str));
    }

    //auto bt = new btreeolc::BTree<std::string, int64_t>();
    auto bt = new btreeolc::BTree<std::string, std::string>();
    std::cout << enc_insert_keys.size() << "*" << std::endl;
    double insert_start_time = getNow();
    for (int i = 0; i < (int)enc_insert_keys.size(); i++) {
        if (i%1000000 == 0)
            std::cout << i << "/" << enc_insert_keys.size() << std::endl;
        std::pair<std::string, std::string>* tmp_pair = &enc_insert_keys[i];
        if (is_compressed) {
            encoder->encode(tmp_pair->first, buffer);
        }
//        bt->insert(tmp_pair->second, reinterpret_cast<int64_t>(&(tmp_pair->second)));
          bt->insert(tmp_pair->second, tmp_pair->second);
    }

    double end_time = getNow();
    double insert_time = end_time - insert_start_time;
    double build_time = end_time - start_time;
    std::cout << "Insert time = " << insert_time << std::endl;
    std::cout << "Build time = " << build_time << std::endl;

    std::cout << "size = " << bt->getSize() << std::endl;
    std::cout << "total key size = " << total_key_size << std::endl;

    int64_t btree_size = bt->getSize();
    double encoder_mem = 0;
    if (encoder != nullptr)
        encoder_mem = encoder->memoryUse();
    double mem = (btree_size + encoder_mem + total_key_size) / 1000000.0;
    std::cout << kGreen << "Mem = " << kNoColor << mem << std::endl;

    // execute transactions =======================================
    uint64_t sum = 0;
    int64_t TIDs[1000];
    std::string results[1000];

    start_time = getNow();
    if (is_point) { // point query
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                int enc_len = encoder->encode(txn_keys[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                std::string enc_key = std::string((const char*)buffer, enc_len_round);
                //int64_t re;
                std::string re;
                bool find = bt->lookup(enc_key, re);
                sum += (int)find;
            }
        } else {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                //int64_t re;
                std::string re;
                bool find = bt->lookup(txn_keys[i], re);
                sum += (int)find;
            }
        }
    } else { // range query
        if (is_compressed) {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
                int enc_len = 0;
                enc_len = encoder->encode(txn_keys[i], buffer);
                int enc_len_round = (enc_len + 7) >> 3;
                std::string left_key = std::string((const char*)buffer, enc_len_round);
//                int cnt = bt->scan(left_key, scan_key_lens[i], TIDs);
//                if (cnt != scan_key_lens[i]) {
//                    std::cout << "Input Size: " << scan_key_lens[i] << "\t" << "Result Size: " << cnt << std::endl;
//                }
                int cnt = bt->scan(left_key, scan_key_lens[i], results);
                if (cnt != scan_key_lens[i]) {
                    std::cout << "Input Size: " << scan_key_lens[i] << "\t" << "Result Size: " << cnt << std::endl;
                    for (int j = 0; j < cnt; j++) {
                        std::cout << results[j] << std::endl;
                    }
                }

            }
        } else {
            for (int i = 0; i < (int)txn_keys.size(); i++) {
//                int cnt = bt->scan(txn_keys[i], scan_key_lens[i], TIDs);
//                if (cnt != scan_key_lens[i]) {
//                    std::cout << "Input Size: " << scan_key_lens[i] << "\t" << "Result Size: " << cnt << std::endl;
//                }
                int cnt = bt->scan(txn_keys[i], scan_key_lens[i], results);
                if (cnt != scan_key_lens[i]) {
                    std::cout << "Input Size: " << scan_key_lens[i] << "\t" << "Result Size: " << cnt << std::endl;
                    for (int j = 0; j < cnt; j++) {
                        std::cout << results[j] << std::endl;
                    }
                }

            }
        }
    }
    end_time = getNow();
    double exec_time = end_time - start_time;
    double iput = enc_insert_keys.size() / insert_time / 1000000; // Mops/sec
    double tput = txn_keys.size() / exec_time / 1000000; // Mops/sec
    std::cout << kGreen << "Insert Througput = " << kNoColor << iput << "\n";
    std::cout << kGreen << "Lookup Throughput = " << kNoColor << tput << "\n";
    double lookup_lat = (exec_time * 1000000) / txn_keys.size(); // us
    double insert_lat = (insert_time * 1000000) / enc_insert_keys.size();
    std::cout << kGreen << "Insert Latency = " << kNoColor << insert_lat << "\n";
    std::cout << kGreen << "Lookup Latency = " << kNoColor << lookup_lat << "\n";

    delete bt;

    if (expt_id == 0) {
        if (wkld_id == kEmail) {
            output_lookuplat_email_btree << lookup_lat << "\n";
            output_insertlat_email_btree << insert_lat << "\n";
            output_mem_email_btree << mem << "\n";
        } else if (wkld_id == kWiki) {
            output_lookuplat_wiki_btree << lookup_lat << "\n";
            output_insertlat_wiki_btree << insert_lat << "\n";
            output_mem_wiki_btree << mem << "\n";
        } else if (wkld_id == kUrl) {
            output_lookuplat_url_btree << lookup_lat << "\n";
            output_insertlat_url_btree << insert_lat << "\n";
            output_mem_url_btree << mem << "\n";
        } else if (wkld_id == kTs) {
            output_lookuplat_ts_btree << lookup_lat << "\n";
            output_insertlat_ts_btree << insert_lat << "\n";
            output_mem_ts_btree << mem << "\n";
        }
    } else if (expt_id == 1) {
        if (wkld_id == kEmail) {
            output_lookuplat_email_btree_range << lookup_lat << "\n";
            output_insertlat_email_btree_range << insert_lat <<"\n";
            output_mem_email_btree_range << mem << "\n";
        } else if (wkld_id == kWiki) {
            output_lookuplat_wiki_btree_range << lookup_lat << "\n";
            output_insertlat_wiki_btree_range << insert_lat <<"\n";
            output_mem_wiki_btree_range << mem << "\n";
        } else if (wkld_id == kUrl) {
            output_lookuplat_url_btree_range << lookup_lat << "\n";
            output_insertlat_url_btree_range << insert_lat << "\n";
            output_mem_url_btree_range << mem << "\n";
        }  else if (wkld_id == kTs) {
            output_lookuplat_ts_btree_range << lookup_lat << "\n";
            output_insertlat_ts_btree_range  << insert_lat << "\n";
            output_mem_ts_btree_range << mem << "\n";
        }
    }
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
        const std::vector<int>& upper_bound_tss) {
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

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, false, 0, 0,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif
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

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, true, 1, 0,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif
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

#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, true, 2, 6,
     insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif
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
#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, true, 3, 6,
         insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif
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
#ifdef RUN_TIMESTAMP
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kTs, is_point, true, 4, 6,
         insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
    expt_num++;
#endif

    int dict_size_5[2] = {2, 6};
    if (runALM == 1) {
        for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
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
#ifdef RUN_TIMESTAMP
    loadWorkload(kTs, insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);
#endif

    if (expt_id == 0) {
        //-------------------------------------------------------------
        // Point Queries; Expt ID = 0
        //-------------------------------------------------------------
        std::cout << "====================================" << std::endl;
        std::cout << "Point Queries; Expt ID = 0" << std::endl;
        std::cout << "====================================" << std::endl;

        output_lookuplat_email_btree.open(file_lookuplat_email_btree, std::ofstream::app);
        output_insertlat_email_btree.open(file_insertlat_email_btree, std::ofstream::app);
        output_mem_email_btree.open(file_mem_email_btree, std::ofstream::app);

        output_lookuplat_wiki_btree.open(file_lookuplat_wiki_btree, std::ofstream::app);
        output_insertlat_wiki_btree.open(file_insertlat_wiki_btree, std::ofstream::app);
        output_mem_wiki_btree.open(file_mem_wiki_btree, std::ofstream::app);

        output_lookuplat_url_btree.open(file_lookuplat_url_btree, std::ofstream::app);
        output_insertlat_url_btree.open(file_insertlat_url_btree, std::ofstream::app);
        output_mem_url_btree.open(file_mem_url_btree, std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lookuplat_ts_btree.open(file_lookuplat_ts_btree, std::ofstream::app);
        output_insertlat_ts_btree.open(file_insertlat_ts_btree, std::ofstream::app);
        output_mem_ts_btree.open(file_mem_ts_btree, std::ofstream::app);
#endif
        bool is_point = true;
        int expt_num = 1;
        int total_num_expt = 24;
        exec_group(expt_id, is_point, expt_num, total_num_expt,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        output_lookuplat_email_btree << "-" << "\n";
        output_insertlat_email_btree << "-" << "\n";
        output_mem_email_btree << "-" << "\n";

        output_lookuplat_email_btree.close();
        output_insertlat_email_btree.close();
        output_mem_email_btree.close();

        output_lookuplat_wiki_btree << "-" << "\n";
        output_insertlat_wiki_btree << "-" << "\n";
        output_mem_wiki_btree << "-" << "\n";

        output_lookuplat_wiki_btree.close();
        output_insertlat_wiki_btree.close();
        output_mem_wiki_btree.close();

        output_lookuplat_url_btree << "-" << "\n";
        output_insertlat_url_btree << "-" << "\n";
        output_mem_url_btree << "-" << "\n";

        output_lookuplat_url_btree.close();
        output_insertlat_url_btree.close();
        output_mem_url_btree.close();
#ifdef RUN_TIMESTAM
        output_lookuplat_ts_btree << "-" << "\n";
        output_insertlat_ts_btree << "-" << "\n";
        output_mem_ts_btree << "-" << "\n";

        output_lookuplat_ts_btree.close();
        output_insertlat_ts_btree.close();
        output_mem_ts_btree.close();
#endif
    } else if (expt_id == 1) {
        //-------------------------------------------------------------
        // Range Queries; Expt ID = 1
        //-------------------------------------------------------------
        std::cout << "====================================" << std::endl;
        std::cout << "Range Queries; Expt ID = 1" << std::endl;
        std::cout << "====================================" << std::endl;

        output_lookuplat_email_btree_range.open(file_lookuplat_email_btree_range, std::ofstream::app);
        output_insertlat_email_btree_range.open(file_insertlat_email_btree_range, std::ofstream::app);
        output_mem_email_btree_range.open(file_mem_email_btree_range, std::ofstream::app);

        output_lookuplat_wiki_btree_range.open(file_lookuplat_wiki_btree_range, std::ofstream::app);
        output_insertlat_wiki_btree_range.open(file_insertlat_wiki_btree_range, std::ofstream::app);
        output_mem_wiki_btree_range.open(file_mem_wiki_btree_range, std::ofstream::app);

        output_lookuplat_url_btree_range.open(file_lookuplat_url_btree_range, std::ofstream::app);
        output_insertlat_url_btree_range.open(file_insertlat_url_btree_range, std::ofstream::app);
        output_mem_url_btree_range.open(file_mem_url_btree_range, std::ofstream::app);
#ifdef RUN_TIMESTAMP
        output_lookuplat_ts_btree_range.open(file_lookuplat_ts_btree_range, std::ofstream::app);
        output_insertlat_ts_btree_range.open(file_insertlat_ts_btree_range, std::ofstream::app);
        output_mem_ts_btree_range.open(file_mem_ts_btree_range, std::ofstream::app);
#endif
        bool is_point = false;
        int expt_num = 1;
        int total_num_expt = 24;
        exec_group(expt_id, is_point, expt_num, total_num_expt,
               insert_emails, insert_emails_sample, txn_emails, upper_bound_emails,
               insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis,
               insert_urls, insert_urls_sample, txn_urls, upper_bound_urls,
               insert_tss, insert_tss_sample, txn_tss, upper_bound_tss);

        output_lookuplat_email_btree_range << "-" << "\n";
        output_insertlat_email_btree_range << "-" << "\n";
        output_mem_email_btree_range << "-" << "\n";

        output_lookuplat_email_btree_range.close();
        output_insertlat_email_btree_range.close();
        output_mem_email_btree_range.close();

        output_lookuplat_wiki_btree_range << "-" << "\n";
        output_insertlat_wiki_btree_range << "-" << "\n";
        output_mem_wiki_btree_range << "-" << "\n";

        output_lookuplat_wiki_btree_range.close();
        output_insertlat_wiki_btree_range.close();
        output_mem_wiki_btree_range.close();

        output_lookuplat_url_btree_range << "-" << "\n";
        output_insertlat_url_btree_range << "-" << "\n";
        output_mem_url_btree_range << "-" << "\n";

        output_lookuplat_url_btree_range.close();
        output_insertlat_url_btree_range.close();
        output_mem_url_btree_range.close();
#ifdef RUN_TIMESTAMP
        output_lookuplat_ts_btree_range << "-" << "\n";
        output_insertlat_ts_btree_range << "-" << "\n";
        output_mem_ts_btree_range << "-" << "\n";

        output_lookuplat_ts_btree_range.close();
        output_insertlat_ts_btree_range.close();
        output_mem_ts_btree_range.close();
#endif
    }
    return 0;
}
