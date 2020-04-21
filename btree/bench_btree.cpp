#include <sys/time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>

#include "btree_map.hpp"
#include "encoder_factory.hpp"
#include "parameters.h"

static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000;
static const uint64_t kNumUrlRecords = 5000;
static const uint64_t kNumTxns = 10000000;

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
static const char *kGreen = "\033[0;32m";
static const char *kNoColor = "\033[0;0m";

static int kRunALM = 1;
static int kRunEmail = 0;
static int kRunWiki = 0;
static bool kRunUrl = 0;
static const double kSamplePercent = 1;
static std::string endStr = std::string(255, char(255));

//-------------------------------------------------------------
// Workload IDs
//-------------------------------------------------------------
static const int kEmail = 0;
static const int kWiki = 1;
static const int kUrl = 2;

//-------------------------------------------------------------
// Expt ID = 0
//-------------------------------------------------------------
static const std::string output_dir_btree_point = "results/btree/point/";
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

//-------------------------------------------------------------
// Expt ID = 1
//-------------------------------------------------------------
static const std::string output_dir_btree_range = "results/btree/range/";
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

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void loadKeysFromFile(const std::string &file_name, const uint64_t num_records, std::vector<std::string> &keys) {
  std::ifstream infile(file_name);
  std::string key;
  uint64_t count = 0;
  while (count < num_records && infile.good()) {
    infile >> key;
    keys.push_back(key);
    count++;
  }
}

void loadLensInt(const std::string &file_name, const uint64_t num_records, std::vector<int> &keys) {
  std::ifstream infile(file_name);
  int key;
  uint64_t count = 0;
  while (count < num_records && infile.good()) {
    infile >> key;
    keys.push_back(key);
    count++;
  }
}

void loadWorkload(int wkld_id, std::vector<std::string> &insert_keys, std::vector<std::string> &insert_keys_sample,
                  std::vector<std::string> &txn_keys, std::vector<int> &scan_key_lens) {
  std::vector<std::string> load_keys;
  if (wkld_id == kEmail)
    loadKeysFromFile(file_load_email, kNumEmailRecords, load_keys);
  else if (wkld_id == kWiki)
    loadKeysFromFile(file_load_wiki, kNumWikiRecords, load_keys);
  else if (wkld_id == kUrl)
    loadKeysFromFile(file_load_url, kNumUrlRecords, load_keys);
  else
    return;

  std::sort(load_keys.begin(), load_keys.end());
  for (int i = 0; i < (int)load_keys.size() - 1; i++) {
    int key_len = load_keys[i].length();
    int next_key_len = load_keys[i + 1].length();
    if (key_len < next_key_len) {
      std::string next_prefix = load_keys[i + 1].substr(0, key_len);
      if (load_keys[i].compare(next_prefix) != 0) insert_keys.push_back(load_keys[i]);
    } else {
      insert_keys.push_back(load_keys[i]);
    }
  }
  insert_keys.push_back(load_keys[load_keys.size() - 1]);

  load_keys.clear();
  std::random_shuffle(insert_keys.begin(), insert_keys.end());

  for (int i = 0; i < (int)insert_keys.size(); i += int(100 / kSamplePercent)) {
    insert_keys_sample.push_back(insert_keys[i]);
  }

  if (wkld_id == kEmail) {
    loadKeysFromFile(file_txn_email, kNumTxns, txn_keys);
    loadLensInt(file_txn_email_len, kNumTxns, scan_key_lens);
  } else if (wkld_id == kWiki) {
    loadKeysFromFile(file_txn_wiki, kNumTxns, txn_keys);
    loadLensInt(file_txn_wiki_len, kNumTxns, scan_key_lens);
  } else if (wkld_id == kUrl) {
    loadKeysFromFile(file_txn_url, kNumTxns, txn_keys);
    loadLensInt(file_txn_url_len, kNumTxns, scan_key_lens);
  }

  std::cout << "insert_keys size = " << insert_keys.size() << std::endl;
  std::cout << "insert_keys_sample size = " << insert_keys_sample.size() << std::endl;
  std::cout << "txn_keys size = " << txn_keys.size() << std::endl;
  std::cout << "scan_key_lens size = " << scan_key_lens.size() << std::endl;
}

void exec(const int expt_id, const int wkld_id, const bool is_point, const bool is_compressed, const int encoder_type,
          const int64_t dict_size_id, const std::vector<std::string> &insert_keys,
          const std::vector<std::string> &insert_keys_sample, const std::vector<std::string> &txn_keys,
          const std::vector<int> &scan_key_lens) {
  hope::Encoder *encoder = nullptr;
  uint8_t *buffer = new uint8_t[8192];
  std::vector<std::pair<std::string, std::string> > enc_insert_keys;

  int64_t input_dict_size = dict_size_list[dict_size_id];
  if (encoder_type == 3) {
    input_dict_size = three_gram_input_dict_size[wkld_id][dict_size_id];
  } else if (encoder_type == 4) {
    input_dict_size = four_gram_input_dict_size[wkld_id][dict_size_id];
  }

  int W = 0;
  if (encoder_type == 5) W = ALM_W[wkld_id][dict_size_id];
  if (encoder_type == 6) W = ALM_W_improved[wkld_id][dict_size_id];

  int64_t total_key_size = 0;
  double start_time = getNow();
  if (is_compressed) {
    encoder = hope::EncoderFactory::createEncoder(encoder_type, W);
    encoder->build(insert_keys_sample, input_dict_size);
  }

  for (int i = 0; i < (int)insert_keys.size(); i++) {
    std::string encode_str;
    if (is_compressed) {
      int enc_len = encoder->encode(insert_keys[i], buffer);
      int enc_len_round = (enc_len + 7) >> 3;
      encode_str = std::string((const char *)buffer, enc_len_round);
      total_key_size += enc_len_round;
    } else {
      encode_str = insert_keys[i];
      total_key_size += insert_keys[i].size();
    }
    enc_insert_keys.push_back(std::make_pair(insert_keys[i], encode_str));
  }

  typedef tlx::btree_map<std::string, uint64_t, std::less<std::string> > btree_type;
  btree_type *bt = new btree_type();
  double insert_start_time = getNow();
  for (int i = 0; i < (int)enc_insert_keys.size(); i++) {
    std::pair<std::string, std::string> *tmp_pair = &enc_insert_keys[i];
    if (is_compressed) {
      encoder->encode(tmp_pair->first, buffer);
    }
    bt->insert2(tmp_pair->second, (uint64_t) & (tmp_pair->second));
  }

  double end_time = getNow();
  double insert_time = end_time - insert_start_time;
  double build_time = end_time - start_time;
  std::cout << "Insert time = " << insert_time << std::endl;
  std::cout << "Build time = " << build_time << std::endl;

  std::cout << "size = " << bt->get_stats().size << std::endl;
  std::cout << "leaves = " << bt->get_stats().leaves << std::endl;
  std::cout << "inner_nodes = " << bt->get_stats().inner_nodes << std::endl;
  std::cout << "avgfill = " << bt->get_stats().avgfill_leaves() << std::endl;
  std::cout << "btree size = " << (256 * bt->get_stats().nodes()) << std::endl;
  std::cout << "total key size = " << total_key_size << std::endl;

  int64_t btree_size = 256 * bt->get_stats().nodes();
  double encoder_mem = 0;
  if (encoder != nullptr) encoder_mem = encoder->memoryUse();
  double mem = (btree_size + total_key_size + encoder_mem) / 1000000.0;
  std::cout << kGreen << "Mem = " << kNoColor << mem << std::endl;

  // execute transactions =======================================
  uint64_t sum = 0;
  uint64_t TIDs[120];
  start_time = getNow();
  if (is_point) {  // point query
    if (is_compressed) {
      for (int i = 0; i < (int)txn_keys.size(); i++) {
        int enc_len = encoder->encode(txn_keys[i], buffer);
        int enc_len_round = (enc_len + 7) >> 3;
        std::string enc_key = std::string((const char *)buffer, enc_len_round);
        btree_type::const_iterator iter = bt->find(enc_key);
        sum += (iter->second);
      }
    } else {
      for (int i = 0; i < (int)txn_keys.size(); i++) {
        btree_type::const_iterator iter = bt->find(txn_keys[i]);
        sum += (iter->second);
      }
    }
  } else {  // range query
    if (is_compressed) {
      for (int i = 0; i < (int)txn_keys.size(); i++) {
        int enc_len = 0;
        enc_len = encoder->encode(txn_keys[i], buffer);
        int enc_len_round = (enc_len + 7) >> 3;
        std::string left_key = std::string((const char *)buffer, enc_len_round);
        btree_type::const_iterator iter = bt->lower_bound(left_key);
        int cnt = 0;
        while (iter != bt->end() && iter.key().compare(endStr) < 0 && cnt < scan_key_lens[i]) {
          TIDs[cnt] = iter->second;
          ++iter;
          ++cnt;
        }
      }
      std::cout << "Finish Uncompressed Range Query" << std::endl;
    } else {
      for (int i = 0; i < (int)txn_keys.size(); i++) {
        btree_type::const_iterator iter = bt->lower_bound(txn_keys[i]);
        int cnt = 0;
        while (iter != bt->end() && iter.key().compare(endStr) < 0 && cnt < scan_key_lens[i]) {
          TIDs[cnt] = iter->second;
          ++iter;
          ++cnt;
        }
      }
      std::cout << "Finish Uncompressed Range Query" << std::endl;
    }
  }
  end_time = getNow();
  double exec_time = end_time - start_time;
  double iput = enc_insert_keys.size() / insert_time / 1000000;  // Mops/sec
  double tput = txn_keys.size() / exec_time / 1000000;           // Mops/sec
  std::cout << kGreen << "Insert Througput = " << kNoColor << iput << "\n";
  std::cout << kGreen << "Lookup Throughput = " << kNoColor << tput << "\n";
  double lookup_lat = (exec_time * 1000000) / txn_keys.size();  // us
  double insert_lat = (insert_time * 1000000) / enc_insert_keys.size();
  std::cout << TIDs[0] << std::endl;
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
    }
  } else if (expt_id == 1) {
    if (wkld_id == kEmail) {
      output_lookuplat_email_btree_range << lookup_lat << "\n";
      output_insertlat_email_btree_range << insert_lat << "\n";
      output_mem_email_btree_range << mem << "\n";
    } else if (wkld_id == kWiki) {
      output_lookuplat_wiki_btree_range << lookup_lat << "\n";
      output_insertlat_wiki_btree_range << insert_lat << "\n";
      output_mem_wiki_btree_range << mem << "\n";
    } else if (wkld_id == kUrl) {
      output_lookuplat_url_btree_range << lookup_lat << "\n";
      output_insertlat_url_btree_range << insert_lat << "\n";
      output_mem_url_btree_range << mem << "\n";
    }
  }
}

void exec_group(const int expt_id, const bool is_point, int &expt_num, const int total_num_expt,
                const std::vector<std::string> &insert_emails, const std::vector<std::string> &insert_emails_sample,
                const std::vector<std::string> &txn_emails, const std::vector<int> &upper_bound_emails,
                const std::vector<std::string> &insert_wikis, const std::vector<std::string> &insert_wikis_sample,
                const std::vector<std::string> &txn_wikis, const std::vector<int> &upper_bound_wikis,
                const std::vector<std::string> &insert_urls, const std::vector<std::string> &insert_urls_sample,
                const std::vector<std::string> &txn_urls, const std::vector<int> &upper_bound_urls) {

  if (kRunEmail) {
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, false, 0, 0, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
  expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, true, 2, 6, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, true, 3, 6, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kEmail, is_point, true, 4, 6, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
    expt_num++;
  
  }
  //=================================================

  if (kRunWiki) {
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, false, 0, 0, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 1, 0, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 2, 6, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 3, 6, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;

    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kWiki, is_point, true, 4, 6, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
    expt_num++;
  }
  //=================================================
  if (kRunUrl) {
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, false, 0, 0, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 1, 0, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 2, 6, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 3, 6, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
  
    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
    exec(expt_id, kUrl, is_point, true, 4, 6, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
    expt_num++;
  }

  int dict_size_5[2] = {2, 6};
  if (kRunALM && kRunEmail) {
    for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
      for (int j = 0; j < 2; j++) {
        std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
        exec(expt_id, kEmail, is_point, true, encoder_type, dict_size_5[j], insert_emails, insert_emails_sample,
             txn_emails, upper_bound_emails);
        expt_num++;
      }
    }
  }

  if (kRunALM && kRunWiki) {
    for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
      for (int j = 0; j < 2; j++) {
        std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
        exec(expt_id, kWiki, is_point, true, encoder_type, dict_size_5[j], insert_wikis, insert_wikis_sample, txn_wikis,
             upper_bound_wikis);
        expt_num++;
      }
    }
  }

  if (kRunALM && kRunUrl) {
    for (int encoder_type = 6; encoder_type < 7; encoder_type++) {
      for (int j = 0; j < 2; j++) {
        std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
        exec(expt_id, kUrl, is_point, true, encoder_type, dict_size_5[j], insert_urls, insert_urls_sample, txn_urls,
             upper_bound_urls);
        expt_num++;
      }
    }
  }
}

int main(int argc, char *argv[]) {
  int expt_id = (int)atoi(argv[1]);
  kRunALM = (int)atoi(argv[2]);
  kRunEmail = (int)atoi(argv[3]);
  kRunWiki = (int)atoi(argv[4]);
  kRunUrl = (int)atoi(argv[5]);

  //-------------------------------------------------------------
  // Init Workloads
  //-------------------------------------------------------------
  std::vector<std::string> insert_emails, insert_emails_sample, txn_emails;
  std::vector<int> upper_bound_emails;
  if (kRunEmail) {
    loadWorkload(kEmail, insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
  }

  std::vector<std::string> insert_wikis, insert_wikis_sample, txn_wikis;
  std::vector<int> upper_bound_wikis;
  if (kRunWiki) {
    loadWorkload(kWiki, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
  }

  std::vector<std::string> insert_urls, insert_urls_sample, txn_urls;
  std::vector<int> upper_bound_urls;
  if (kRunUrl) {
    loadWorkload(kUrl, insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
  }

  if (expt_id == 0) {
    //-------------------------------------------------------------
    // Point Queries; Expt ID = 0
    //-------------------------------------------------------------
    std::cout << "====================================" << std::endl;
    std::cout << "Point Queries; Expt ID = 0" << std::endl;
    std::cout << "====================================" << std::endl;

    if (kRunEmail) {
      output_lookuplat_email_btree.open(file_lookuplat_email_btree, std::ofstream::app);
      output_insertlat_email_btree.open(file_insertlat_email_btree, std::ofstream::app);
      output_mem_email_btree.open(file_mem_email_btree, std::ofstream::app);
    }

    if (kRunWiki) {
      output_lookuplat_wiki_btree.open(file_lookuplat_wiki_btree, std::ofstream::app);
      output_insertlat_wiki_btree.open(file_insertlat_wiki_btree, std::ofstream::app);
      output_mem_wiki_btree.open(file_mem_wiki_btree, std::ofstream::app);
    }

    if (kRunUrl) {
      output_lookuplat_url_btree.open(file_lookuplat_url_btree, std::ofstream::app);
      output_insertlat_url_btree.open(file_insertlat_url_btree, std::ofstream::app);
      output_mem_url_btree.open(file_mem_url_btree, std::ofstream::app);
    }

    bool is_point = true;
    int expt_num = 1;
    int total_num_expt = 24;
    exec_group(expt_id, is_point, expt_num, total_num_expt, insert_emails, insert_emails_sample, txn_emails,
               upper_bound_emails, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis, insert_urls,
               insert_urls_sample, txn_urls, upper_bound_urls);

    if (kRunEmail) {
      output_lookuplat_email_btree << "-"
                                 << "\n";
      output_insertlat_email_btree << "-"
                                 << "\n";
      output_mem_email_btree << "-"
                           << "\n";
    
      output_lookuplat_email_btree.close();
      output_insertlat_email_btree.close();
      output_mem_email_btree.close();
    }

    if (kRunWiki) {
      output_lookuplat_wiki_btree << "-"
                                << "\n";
      output_insertlat_wiki_btree << "-"
                                << "\n";
      output_mem_wiki_btree << "-"
                          << "\n";

      output_lookuplat_wiki_btree.close();
      output_insertlat_wiki_btree.close();
      output_mem_wiki_btree.close();
    }

    if (kRunUrl) {
      output_lookuplat_url_btree << "-"
                               << "\n";
      output_insertlat_url_btree << "-"
                               << "\n";
      output_mem_url_btree << "-"
                         << "\n";

      output_lookuplat_url_btree.close();
      output_insertlat_url_btree.close();
      output_mem_url_btree.close();
    }
  } else if (expt_id == 1) {
    //-------------------------------------------------------------
    // Range Queries; Expt ID = 1
    //-------------------------------------------------------------
    std::cout << "====================================" << std::endl;
    std::cout << "Range Queries; Expt ID = 1" << std::endl;
    std::cout << "====================================" << std::endl;

    if (kRunEmail) {
      output_lookuplat_email_btree_range.open(file_lookuplat_email_btree_range, std::ofstream::app);
      output_insertlat_email_btree_range.open(file_insertlat_email_btree_range, std::ofstream::app);
      output_mem_email_btree_range.open(file_mem_email_btree_range, std::ofstream::app);
    }

    if (kRunWiki) {
      output_lookuplat_wiki_btree_range.open(file_lookuplat_wiki_btree_range, std::ofstream::app);
      output_insertlat_wiki_btree_range.open(file_insertlat_wiki_btree_range, std::ofstream::app);
      output_mem_wiki_btree_range.open(file_mem_wiki_btree_range, std::ofstream::app);
    }

    if (kRunUrl) {
      output_lookuplat_url_btree_range.open(file_lookuplat_url_btree_range, std::ofstream::app);
      output_insertlat_url_btree_range.open(file_insertlat_url_btree_range, std::ofstream::app);
      output_mem_url_btree_range.open(file_mem_url_btree_range, std::ofstream::app);
    }

    bool is_point = false;
    int expt_num = 1;
    int total_num_expt = 24;
    exec_group(expt_id, is_point, expt_num, total_num_expt, insert_emails, insert_emails_sample, txn_emails,
               upper_bound_emails, insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis, insert_urls,
               insert_urls_sample, txn_urls, upper_bound_urls);

    if (kRunEmail) {
      output_lookuplat_email_btree_range << "-"
                                       << "\n";
      output_insertlat_email_btree_range << "-"
                                       << "\n";
      output_mem_email_btree_range << "-"
                                 << "\n";

      output_lookuplat_email_btree_range.close();
      output_insertlat_email_btree_range.close();
      output_mem_email_btree_range.close();
    }

    if (kRunWiki) {
      output_lookuplat_wiki_btree_range << "-"
                                      << "\n";
      output_insertlat_wiki_btree_range << "-"
                                      << "\n";
      output_mem_wiki_btree_range << "-"
                                << "\n";

      output_lookuplat_wiki_btree_range.close();
      output_insertlat_wiki_btree_range.close();
      output_mem_wiki_btree_range.close();
    }

    if (kRunUrl) {
      output_lookuplat_url_btree_range << "-"
                                     << "\n";
      output_insertlat_url_btree_range << "-"
                                     << "\n";
      output_mem_url_btree_range << "-"
                               << "\n";

      output_lookuplat_url_btree_range.close();
      output_insertlat_url_btree_range.close();
      output_mem_url_btree_range.close();
    }
  }
  return 0;
}
