#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

#include "encoder_factory.hpp"
#include "Tree.h"

static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000000;
static const uint64_t kNumTxns = 10000000;
static const unsigned kPercent = 50;

// for pretty print
static const char* kGreen ="\033[0;32m";
static const char* kRed ="\033[0;31m";
static const char* kNoColor ="\033[0;0m";

double getNow() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

void loadKey(TID tid, Key &key) {
    std::string* key_str = (std::string*)tid;
    key.set(key_str->c_str(), key_str->length());
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

void selectKeysToInsert(const unsigned percent,
			std::vector<std::string> &insert_keys,
			std::vector<std::string> &keys) {
    random_shuffle(keys.begin(), keys.end());
    uint64_t num_insert_keys = keys.size() * percent / 100;
    std::vector<std::string> selected_keys;
    for (uint64_t i = 0; i < num_insert_keys; i++)
	selected_keys.push_back(keys[i]);

    keys.clear();
    sort(selected_keys.begin(), selected_keys.end());

    for (int i = 0; i < (int)selected_keys.size() - 1; i++) {
	int key_len = selected_keys[i].length();
	int next_key_len = selected_keys[i + 1].length();
	if (key_len < next_key_len) {
	    std::string next_prefix = selected_keys[i + 1].substr(0, key_len);
	    if (selected_keys[i].compare(next_prefix) != 0)
		insert_keys.push_back(selected_keys[i]);
	} else {
	    insert_keys.push_back(selected_keys[i]);
	}
    }
    insert_keys.push_back(selected_keys[selected_keys.size() - 1]);
    std::cout << "Number of insert keys = " << insert_keys.size() << std::endl;
}

std::string getUpperBoundKey(const std::string& key) {
    std::string ret_str = key;
    ret_str[ret_str.size() - 1] = (char)'\255';
    return ret_str;
}

int main(int argc, char *argv[]) {
    if (argc != 6) {
	std::cout << "Usage:" << std::endl;
	std::cout << "1. key type: email, wiki, url" << std::endl;
	std::cout << "2. query type: point, range" << std::endl;
	std::cout << "3. compress?: 0, 1" << std::endl;
	std::cout << "4. encoder type: 1 --> single, 2 --> double, 3 --> 3gram, 4 --> 4gram" << std::endl;
	std::cout << "5. dictionary size limit" << std::endl;
	return -1;
    }

    std::string key_type = argv[1];
    std::string query_type = argv[2];
    int is_compressed_int = (int)atoi(argv[3]);
    bool is_compressed = false;
    if (is_compressed_int > 0)
	is_compressed = true;
    int encoder_type = (int)atoi(argv[4]);
    int64_t dict_size_limit = (int64_t)atoi(argv[5]);

    // check args ====================================================
    if (key_type.compare(std::string("email")) != 0
	&& key_type.compare(std::string("wiki")) != 0
	&& key_type.compare(std::string("url")) != 0) {
	std::cout << kRed << "WRONG key type\n" << kNoColor;
	return -1;
    }

    if (query_type.compare(std::string("point")) != 0
	&& query_type.compare(std::string("range")) != 0) {
	std::cout << kRed << "WRONG query type\n" << kNoColor;
	return -1;
    }
    
    if (encoder_type <= 0 || encoder_type > 4) {
	std::cout << kRed << "WRONG encoder type\n" << kNoColor;
	return -1;
    }

    if (dict_size_limit <= 0) {
	std::cout << kRed << "WRONG dictionary size limit\n" << kNoColor;
	return -1;
    }

    // load keys from files =======================================
    std::string load_file = "../../workloads/load_";
    load_file += key_type;
    std::vector<std::string> load_keys;
    if (key_type.compare(std::string("wiki")) == 0)
	loadKeysFromFile(load_file, kNumWikiRecords, load_keys);
    else
	loadKeysFromFile(load_file, kNumEmailRecords, load_keys);

    std::string txn_file = "../../workloads/txn_";
    txn_file += key_type;
    txn_file += "_zipfian";
    std::vector<std::string> txn_keys;
    loadKeysFromFile(txn_file, kNumTxns, txn_keys);

    std::vector<std::string> insert_keys;
    selectKeysToInsert(kPercent, insert_keys, load_keys);

    // compute upperbound keys for range queries =================
    std::vector<std::string> upper_bound_keys;
    if (query_type.compare(std::string("range")) == 0) {
	for (int i = 0; i < (int)txn_keys.size(); i++) {
	    upper_bound_keys.push_back(getUpperBoundKey(txn_keys[i]));
	}
    }

    // create filter ==============================================
    ope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    uint8_t* buffer_r = new uint8_t[8192];
    std::vector<std::string> enc_insert_keys;
    
    double time1 = getNow();
    if (is_compressed) {
	encoder = ope::EncoderFactory::createEncoder(encoder_type);
	encoder->build(insert_keys, dict_size_limit);
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

    ART_ROWEX::Tree* art = new ART_ROWEX::Tree(loadKey);
    auto t = art->getThreadInfo();
    for (int i = 0; i < (int)enc_insert_keys.size(); i++) {
	Key key;
	loadKey((TID)&(enc_insert_keys[i]), key);
	art->insert(key, (TID)&(enc_insert_keys[i]), t);
    }

    double time2 = getNow();
    std::cout << "Build time = " << (time2 - time1) << std::endl;

    double mem = 0;
    double avg_height = 0;
    int cnt_N4 = 0;
    int cnt_N16 = 0;
    int cnt_N48 = 0;
    int cnt_N256 = 0;
    uint64_t waste_child_mem = 0;
    uint64_t skip_prefix_mem = 0;
    uint64_t waste_prefix_mem = 0;
    // traverse ART to get stats ==================================
    art->traverse(mem, avg_height, cnt_N4, cnt_N16, cnt_N48, cnt_N256,
                  waste_child_mem, skip_prefix_mem, waste_prefix_mem);
    if (encoder != nullptr)
	mem += (encoder->memoryUse() / 1000000.0);
    std::cout << "Mem = " << mem << std::endl;
    std::cout << "Avg Tree Height = " << avg_height << std::endl;

    // execute transactions =======================================
    double start_time = getNow();

    uint64_t sum = 0;
    auto t2 = art->getThreadInfo();
    if (query_type.compare(std::string("point")) == 0) {
	if (is_compressed) {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		int enc_len = encoder->encode(txn_keys[i], buffer);
		int enc_len_round = (enc_len + 7) >> 3;
		std::string enc_key = std::string((const char*)buffer, enc_len_round);
		Key key;
		loadKey((TID)&(enc_key), key);
		sum += art->lookup(key, t2);
	    }
	} else {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		Key key;
		loadKey((TID)&(txn_keys[i]), key);
		sum += art->lookup(key, t2);
	    }
	}
    } else if (query_type.compare(std::string("range")) == 0) {
	TID result[100];
	std::size_t result_size = 100;
	std::size_t results_found = 0;
	if (is_compressed) {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		int enc_len = 0, enc_len_r = 0;

		encoder->encodePair(txn_keys[i], upper_bound_keys[i], buffer, buffer_r, enc_len, enc_len_r);

		int enc_len_round = (enc_len + 7) >> 3;
		int enc_len_r_round = (enc_len_r + 7) >> 3;
		std::string left_key = std::string((const char*)buffer, enc_len_round);
		std::string right_key = std::string((const char*)buffer_r, enc_len_r_round);

		Key start_key;
		loadKey((TID)&(left_key), start_key);
		Key end_key;
		loadKey((TID)&(right_key), end_key);
		Key continue_key;
		
		sum += (int)art->lookupRange(start_key, end_key, continue_key, result, result_size, results_found, t2);
	    }
	} else {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		Key start_key;
		loadKey((TID)&(txn_keys[i]), start_key);
		Key end_key;
		loadKey((TID)&(upper_bound_keys[i]), end_key);
		Key continue_key;
		
		sum += (int)art->lookupRange(start_key, end_key, continue_key, result, result_size, results_found, t2);
	    }
	}
    }

    double end_time = getNow();

    // print
    std::cout << "sum = " << sum << std::endl;
    double tput = txn_keys.size() / (end_time - start_time) / 1000000; // Mops/sec
    std::cout << kGreen << "Throughput = " << kNoColor << tput << "\n\n";

    return 0;
}
