#include <time.h>
#include <sys/time.h>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <map>

#include "encoder_factory.hpp"
#include "surf.hpp"

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

int main(int argc, char *argv[]) {
    if (argc != 8) {
	std::cout << "Usage:" << std::endl;
	std::cout << "1. filter type: SuRF, SuRFHash, SuRFReal" << std::endl;
	std::cout << "2. suffix length: 0 < len <= 64 (for SuRFHash and SuRFReal only)" << std::endl;
	std::cout << "3. key type: email, wiki, url" << std::endl;
	std::cout << "4. query type: point, range" << std::endl;
	std::cout << "5. compress?: 0, 1" << std::endl;
	std::cout << "6. encoder type: 1 --> single, 2 --> double, 3 --> 3gram" << std::endl;
	std::cout << "7. dictionary size limit" << std::endl;
	return -1;
    }

    std::string filter_type = argv[1];
    uint32_t suffix_len = (uint32_t)atoi(argv[2]);
    std::string key_type = argv[3];
    std::string query_type = argv[4];
    int is_compressed_int = (int)atoi(argv[5]);
    bool is_compressed = false;
    if (is_compressed_int > 0)
	is_compressed = true;
    int encoder_type = (int)atoi(argv[6]);
    int64_t dict_size_limit = (int64_t)atoi(argv[7]);

    // check args ====================================================
    if (filter_type.compare(std::string("SuRF")) != 0
	&& filter_type.compare(std::string("SuRFHash")) != 0
	&& filter_type.compare(std::string("SuRFReal")) != 0) {
	std::cout << kRed << "WRONG filter type\n" << kNoColor;
	return -1;
    }

    if (suffix_len == 0 || suffix_len > 64) {
	std::cout << kRed << "WRONG suffix length\n" << kNoColor;
	return -1;
    }

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
    
    if (encoder_type <= 0 || encoder_type > 3) {
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

    surf::SuRF* filter = nullptr;
    if (filter_type.compare(std::string("SuRF")) == 0) {
	filter = new surf::SuRF(enc_insert_keys);
    } else if (filter_type.compare(std::string("SuRFHash")) == 0) {
	filter = new surf::SuRF(enc_insert_keys, surf::kHash, suffix_len, 0);
    } else if (filter_type.compare(std::string("SuRFReal")) == 0) {
	filter = new surf::SuRF(enc_insert_keys, surf::kReal, 0, suffix_len);
    }
    double time2 = getNow();
    std::cout << "Build time = " << (time2 - time1) << std::endl;

    // execute transactions =======================================
    int64_t positives = 0;
    double start_time = getNow();

    if (query_type.compare(std::string("point")) == 0) {
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
    } else if (query_type.compare(std::string("range")) == 0) {
	// TODO
	// for (int i = 0; i < (int)txn_keys.size(); i++) {
	//     std::string ret_str = txn_keys[i];
	//     ret_str[ret_str.size() - 1] += (char)bench::kEmailRangeSize;
	//     positives += (int)filter->lookupRange(txn_keys[i], ret_str);
	// }
    }

    double end_time = getNow();

    // compute true positives ======================================
    std::map<std::string, bool> ht;
    for (int i = 0; i < (int)insert_keys.size(); i++)
	ht[insert_keys[i]] = true;

    int64_t true_positives = 0;
    std::map<std::string, bool>::iterator ht_iter;
    if (query_type.compare(std::string("point")) == 0) {
	for (int i = 0; i < (int)txn_keys.size(); i++) {
	    ht_iter = ht.find(txn_keys[i]);
	    true_positives += (ht_iter != ht.end());
	}
    } else if (query_type.compare(std::string("range")) == 0) {
	for (int i = 0; i < (int)txn_keys.size(); i++) {
	    ht_iter = ht.lower_bound(txn_keys[i]);
	    if (ht_iter != ht.end()) {
		std::string fetched_key = ht_iter->first;
		true_positives += (fetched_key.compare(upper_bound_keys[i]) < 0);
	    }
	}
    }
    
    int64_t false_positives = positives - true_positives;
    assert(false_positives >= 0);
    int64_t true_negatives = txn_keys.size() - positives;

    // print
    double tput = txn_keys.size() / (end_time - start_time) / 1000000; // Mops/sec
    std::cout << kGreen << "Throughput = " << kNoColor << tput << "\n";

    std::cout << "positives = " << positives << "\n";
    std::cout << "true positives = " << true_positives << "\n";
    std::cout << "false positives = " << false_positives << "\n";
    std::cout << "true negatives = " << true_negatives << "\n";

    double fp_rate = 0;
    if (false_positives > 0)
	fp_rate = false_positives / (true_negatives + false_positives + 0.0);
    std::cout << kGreen << "False Positive Rate = " << kNoColor << fp_rate << "\n";

    double mem = filter->getMemoryUsage() + 0.0;
    if (encoder != nullptr)
	mem += encoder->memoryUse();
    mem /= 1000000; // MB
    std::cout << kGreen << "Memory = " << kNoColor << mem << "MB\n\n";

    return 0;
}
