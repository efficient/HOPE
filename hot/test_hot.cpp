#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>

#include "encoder_factory.hpp"
#include <hot/singlethreaded/HOTSingleThreaded.hpp>
#include <idx/contenthelpers/IdentityKeyExtractor.hpp>

//#define NOT_USE_ENCODE_PAIR 1
#define WRITE_TO_FILE
#define BREAKDOWN_TIME

// static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumEmailRecords = 250000;
uint64_t kNumWikiRecords;
//static const uint64_t kNumWikiRecords = 140000;
static const uint64_t kNumTxns = 10000000;
//static const uint64_t kNumTxns = 10000;
//static const int kSamplePercent = 10;
//static const int kUrlSamplePercent = 1;

static const std::string file_load_email = "workloads/load_email";
static const std::string file_load_wiki = "workloads/load_wiki";
static const std::string file_load_url = "workloads/load_url";

static const std::string file_txn_email = "workloads/txn_email_zipfian";
static const std::string file_txn_wiki = "workloads/txn_wiki_zipfian";
static const std::string file_txn_url = "workloads/txn_url_zipfian";

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

//-------------------------------------------------------------
// Expt ID = 0
//-------------------------------------------------------------
static const std::string output_dir_hot_point = "results/hot/test/";
static const std::string file_lat_wiki_hot = output_dir_hot_point + "path_point_wiki_hot.csv";
std::ofstream output_lat_wiki_hot;

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

std::string getUpperBoundKey(const std::string& key) {
    std::string ret_str = key;
    ret_str[ret_str.size() - 1] = (char)'\255';
    return ret_str;
}

void loadWorkload(int wkld_id,
		  std::vector<std::string>& insert_keys) {
    std::vector<std::string> load_keys;
    if (wkld_id == kEmail)
        loadKeysFromFile(file_load_email, kNumEmailRecords, load_keys);
    else if (wkld_id == kWiki)
        loadKeysFromFile(file_load_wiki, kNumWikiRecords, load_keys);
    else if (wkld_id == kUrl)
        loadKeysFromFile(file_load_url, kNumEmailRecords, load_keys);
    else
	    return;

    std::sort(load_keys.begin(), load_keys.end());
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

    std::cout << "insert_keys size = " << insert_keys.size() << std::endl;
}

void exec(const int expt_id, const int wkld_id, const bool is_point,
	  const bool is_compressed,
	  const int encoder_type, const int64_t dict_size_limit,
	  const std::vector<std::string>& insert_keys,
	  std::vector<int>& key_heights,
      std::vector<std::string>& enc_insert_keys) {

    ope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    uint8_t* buffer_r = new uint8_t[8192];

    int64_t total_key_size = 0;
    if (is_compressed) {
	    encoder = ope::EncoderFactory::createEncoder(encoder_type);
	    encoder->build(insert_keys, dict_size_limit);
	    for (int i = 0; i < (int)insert_keys.size(); i++) {
	        int enc_len = encoder->encode(insert_keys[i], buffer);
	        int enc_len_round = (enc_len + 7) >> 3;
	        enc_insert_keys.push_back(std::string((const char*)buffer, enc_len_round));
	        total_key_size += enc_len_round;
	    }
    } else {
	    for (int i = 0; i < (int)insert_keys.size(); i++) {
	        enc_insert_keys.push_back(insert_keys[i]);
	        total_key_size += insert_keys[i].size();
	    }
    }

    typedef hot::singlethreaded::HOTSingleThreaded<const char*, idx::contenthelpers::IdentityKeyExtractor> hot_type;
    hot_type* ht = new hot_type();
    for (int i = 0; i < (int)enc_insert_keys.size(); i++) {
        ht->insert(enc_insert_keys[i].c_str());
    }

    int64_t htree_size = ht->getStatistics().first;
    std::map<std::string, double> stats_map =  ht->getStatistics().second;
    std::cout << "size = " << htree_size << std::endl;
    double height = stats_map["height"];
    std::vector<double>heights;
    for (int i = 1; i <= int(height); i++) {
        std::string key_name = "leafNodesOnDepth_0" + std::to_string(i);
        heights.push_back(int(stats_map[key_name.c_str()]));
    }

    for  (auto i : stats_map) {
        std::cout << i.first << " = " << i.second << std::endl;
    } 
    double mem = htree_size;

    if (encoder != nullptr)
	    mem += encoder->memoryUse();
    mem = mem / 1000000.0;
    std::cout << kGreen << "Mem = " << kNoColor << mem << std::endl;

    // execute transactions =======================================
    //uint64_t sum = 0;
    if (is_compressed) {
        for (int i = 0; i < (int)insert_keys.size(); i++) {
            int enc_len = encoder->encode(insert_keys[i], buffer);
            int enc_len_round = (enc_len + 7) >> 3;
            std::string enc_key = std::string((const char*)buffer, enc_len_round);
            int key_height = 0;
            idx::contenthelpers::OptionalValue<const char*> result = ht->lookup(enc_key.c_str(),key_height);
            key_heights.push_back(key_height);
            //assert(strcmp(result.mValue, enc_key.c_str()) == 0);
	    }
    } else {
	    for (int i = 0; i < (int)insert_keys.size(); i++) {
            int key_height = 0;
            idx::contenthelpers::OptionalValue<const char*> result = ht->lookup(insert_keys[i].c_str(), key_height);
            key_heights.push_back(key_height);
		    //sum += (iter->second);
            
	    }
	}

    delete ht;
    delete encoder;
    delete[] buffer;
    delete[] buffer_r;
}

int main(int argc, char *argv[]) {
    kNumWikiRecords = (uint64_t)atoi(argv[1]);
    //-------------------------------------------------------------
    // Init Workloads
    //-------------------------------------------------------------
//    std::vector<std::string> insert_emails, insert_emails_sample;
//    loadWorkload(kEmail, insert_emails, insert_emails_sample);

    std::vector<std::string> insert_wikis;
    loadWorkload(kWiki, insert_wikis);
/*
    std::vector<std::string> insert_urls, insert_urls_sample;
    loadWorkload(kUrl, insert_urls, insert_urls_sample);
*/
    std::vector<int> uncompressed_heights;
    std::vector<int> compressed_heights;
    std::vector<std::string> unc_enc_keys;
    std::vector<std::string> com_enc_keys;
    bool is_point = true;
    int expt_id = 0;
    exec(expt_id, kWiki, is_point, false, 0, 0,
	     insert_wikis, uncompressed_heights, unc_enc_keys);
    exec(expt_id, kWiki, is_point, true, 1, 1000,
	     insert_wikis, compressed_heights, com_enc_keys);
    std::cout << "------------------------------------Compare Heights-------------------------"<<std::endl;
    for (int i = 0; i < (int)insert_wikis.size(); i++) {
        int comp_size = int(com_enc_keys[i].size());
        if (compressed_heights[i] > uncompressed_heights[i] && comp_size < int(insert_wikis[i].size())) {
            std::cout << insert_wikis[i] << "\t";
            for (int j = 0; j < int(com_enc_keys[i].size()); j++) {
                std::bitset<8> y(com_enc_keys[i][j]);
                std::cout << y << " ";
            }
            std::cout << "\tuncompressed:" << uncompressed_heights[i] << "\tcompressed:"
                      << compressed_heights[i] << std::endl;
        }
    }
    return 0;
}
