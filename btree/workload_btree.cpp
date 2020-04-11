#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

#include <valgrind/callgrind.h>

#include "encoder_factory.hpp"
#include "btree_map.hpp"

static const uint64_t kNumIntRecords = 100000000;
static const uint64_t kNumEmailRecords = 25000000;
static const uint64_t kNumWikiRecords = 14000000;
static const uint64_t kNumTxns = 10000000;

// for pretty print
static const char* kGreen ="\033[0;32m";
static const char* kRed ="\033[0;31m";
static const char* kNoColor ="\033[0;0m";

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
    std::vector<std::string> insert_keys;
    if (key_type.compare(std::string("wiki")) == 0)
	loadKeysFromFile(load_file, kNumWikiRecords, insert_keys);
    else
	loadKeysFromFile(load_file, kNumEmailRecords, insert_keys);
    random_shuffle(insert_keys.begin(), insert_keys.end());

    std::string txn_file = "../../workloads/txn_";
    txn_file += key_type;
    txn_file += "_zipfian";
    std::vector<std::string> txn_keys;
    loadKeysFromFile(txn_file, kNumTxns, txn_keys);

    // compute upperbound keys for range queries =================
    std::vector<std::string> upper_bound_keys;
    if (query_type.compare(std::string("range")) == 0) {
	for (int i = 0; i < (int)txn_keys.size(); i++) {
	    upper_bound_keys.push_back(getUpperBoundKey(txn_keys[i]));
	}
    }

    // create index ==============================================
    hope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    uint8_t* buffer_r = new uint8_t[8192];
    std::vector<std::string> enc_insert_keys;

    int64_t total_key_size = 0;
    double time1 = getNow();
    if (is_compressed) {
	encoder = hope::EncoderFactory::createEncoder(encoder_type);
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

    typedef tlx::btree_map<std::string, uint64_t, std::less<std::string> > btree_type;
    btree_type* bt = new btree_type();
    for (int i = 0; i < (int)enc_insert_keys.size(); i++) {
	bt->insert2(enc_insert_keys[i], (uint64_t)&(enc_insert_keys[i]));
    }

    double time2 = getNow();
    std::cout << "Build time = " << (time2 - time1) << std::endl;

    std::cout << "size = " << bt->get_stats().size << std::endl;
    std::cout << "leaves = " << bt->get_stats().leaves << std::endl;
    std::cout << "inner_nodes = " << bt->get_stats().inner_nodes << std::endl;
    std::cout << "avgfill = " << bt->get_stats().avgfill_leaves() << std::endl;
    std::cout << "mem = " << (256 * bt->get_stats().nodes()) << std::endl;
    //std::cout << "mem = " << (512 * bt->get_stats().nodes()) << std::endl;
    std::cout << "total key size = " << total_key_size << std::endl;

    // execute transactions =======================================
    double start_time = getNow();

    CALLGRIND_START_INSTRUMENTATION;
    CALLGRIND_TOGGLE_COLLECT;

    uint64_t sum = 0;
    if (query_type.compare(std::string("point")) == 0) {
	if (is_compressed) {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		int enc_len = encoder->encode(txn_keys[i], buffer);
		int enc_len_round = (enc_len + 7) >> 3;
		std::string enc_key = std::string((const char*)buffer, enc_len_round);
		//sum += bt->exists(enc_key);
		btree_type::const_iterator iter = bt->find(enc_key);
		sum += (iter->second);
	    }
	} else {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		//sum += bt->exists(enc_key);
		btree_type::const_iterator iter = bt->find(txn_keys[i]);
		sum += (iter->second);
	    }
	}
    } else if (query_type.compare(std::string("range")) == 0) {
	if (is_compressed) {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		int enc_len = 0, enc_len_r = 0;

		encoder->encodePair(txn_keys[i], upper_bound_keys[i], buffer, buffer_r, enc_len, enc_len_r);

		int enc_len_round = (enc_len + 7) >> 3;
		int enc_len_r_round = (enc_len_r + 7) >> 3;
		std::string left_key = std::string((const char*)buffer, enc_len_round);
		std::string right_key = std::string((const char*)buffer_r, enc_len_r_round);
		btree_type::const_iterator iter = bt->lower_bound(left_key);
		while (iter.key().compare(right_key) < 0) {
		    ++iter;
		}
		sum += (iter->second);
	    }
	} else {
	    for (int i = 0; i < (int)txn_keys.size(); i++) {
		btree_type::const_iterator iter = bt->lower_bound(txn_keys[i]);
		while (iter != bt->end()
		       && iter.key().compare(upper_bound_keys[i]) < 0) {
		    ++iter;
		}
		sum += (iter->second);
	    }
	}
    }

    CALLGRIND_TOGGLE_COLLECT;
    CALLGRIND_STOP_INSTRUMENTATION;

    double end_time = getNow();

    // print
    std::cout << "sum = " << sum << std::endl;
    double tput = txn_keys.size() / (end_time - start_time) / 1000000; // Mops/sec
    std::cout << kGreen << "Throughput = " << kNoColor << tput << "\n\n";

    return 0;
}
