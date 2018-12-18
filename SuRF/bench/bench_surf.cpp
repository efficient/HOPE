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
static const int kSamplePercent = 10;

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
static const std::string output_dir_surf_point = "results/SuRF/point/";
static const std::string file_lat_email_surf = output_dir_surf_point + "lat_email_surf.csv";
std::ofstream output_lat_email_surf;
static const std::string file_mem_email_surf = output_dir_surf_point + "mem_email_surf.csv";
std::ofstream output_mem_email_surf;
static const std::string file_fpr_email_surf = output_dir_surf_point + "fpr_email_surf.csv";
std::ofstream output_fpr_email_surf;

static const std::string file_lat_wiki_surf = output_dir_surf_point + "lat_wiki_surf.csv";
std::ofstream output_lat_wiki_surf;
static const std::string file_mem_wiki_surf = output_dir_surf_point + "mem_wiki_surf.csv";
std::ofstream output_mem_wiki_surf;
static const std::string file_fpr_wiki_surf = output_dir_surf_point + "fpr_wiki_surf.csv";
std::ofstream output_fpr_wiki_surf;

static const std::string file_lat_url_surf = output_dir_surf_point + "lat_url_surf.csv";
std::ofstream output_lat_url_surf;
static const std::string file_mem_url_surf = output_dir_surf_point + "mem_url_surf.csv";
std::ofstream output_mem_url_surf;
static const std::string file_fpr_url_surf = output_dir_surf_point + "fpr_url_surf.csv";
std::ofstream output_fpr_url_surf;

static const std::string output_dir_surfreal_point = "results/SuRF_real/point/";
static const std::string file_lat_email_surfreal = output_dir_surfreal_point + "lat_email_surfreal.csv";
std::ofstream output_lat_email_surfreal;
static const std::string file_mem_email_surfreal = output_dir_surfreal_point + "mem_email_surfreal.csv";
std::ofstream output_mem_email_surfreal;
static const std::string file_fpr_email_surfreal = output_dir_surfreal_point + "fpr_email_surfreal.csv";
std::ofstream output_fpr_email_surfreal;

static const std::string file_lat_wiki_surfreal = output_dir_surfreal_point + "lat_wiki_surfreal.csv";
std::ofstream output_lat_wiki_surfreal;
static const std::string file_mem_wiki_surfreal = output_dir_surfreal_point + "mem_wiki_surfreal.csv";
std::ofstream output_mem_wiki_surfreal;
static const std::string file_fpr_wiki_surfreal = output_dir_surfreal_point + "fpr_wiki_surfreal.csv";
std::ofstream output_fpr_wiki_surfreal;

static const std::string file_lat_url_surfreal = output_dir_surfreal_point + "lat_url_surfreal.csv";
std::ofstream output_lat_url_surfreal;
static const std::string file_mem_url_surfreal = output_dir_surfreal_point + "mem_url_surfreal.csv";
std::ofstream output_mem_url_surfreal;
static const std::string file_fpr_url_surfreal = output_dir_surfreal_point + "fpr_url_surfreal.csv";
std::ofstream output_fpr_url_surfreal;


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

void exec(const int expt_id,
	  const int wkld_id, const bool is_point,
	  const int filter_type, const uint32_t suffix_len,
	  const bool is_compressed,
	  const int encoder_type, const int64_t dict_size_limit,
	  const int64_t point_true_positives,
	  const int64_t range_true_positives,
	  const std::vector<std::string> insert_keys,
	  const std::vector<std::string> insert_keys_sample,
	  const std::vector<std::string> txn_keys,
	  const std::vector<std::string> upper_bound_keys) {
    ope::Encoder* encoder = nullptr;
    uint8_t* buffer = new uint8_t[8192];
    std::vector<std::string> enc_insert_keys;

    double start_time = getNow();
    if (is_compressed) {
	encoder = ope::EncoderFactory::createEncoder(encoder_type);
	encoder->build(insert_keys_sample, dict_size_limit);
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
	// TODO
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
    double mem = (filter->getMemoryUsage() + 0.0) / 1000000; // MB
    std::cout << kGreen << "Memory = " << kNoColor << mem << "\n\n";

    if (expt_id == 0) {
	if (filter_type == 0) {
	    if (wkld_id == kEmail) {
		output_lat_email_surf << lat << "\n";
		output_mem_email_surf << mem << "\n";
		output_fpr_email_surf << fpr << "\n";
	    } else if (wkld_id == kWiki) {
		output_lat_wiki_surf << lat << "\n";
		output_mem_wiki_surf << mem << "\n";
		output_fpr_wiki_surf << fpr << "\n";
	    } else if (wkld_id == kUrl) {
		output_lat_url_surf << lat << "\n";
		output_mem_url_surf << mem << "\n";
		output_fpr_url_surf << fpr << "\n";
	    }
	} else if (filter_type == 2) {
	    if (wkld_id == kEmail) {
		output_lat_email_surfreal << lat << "\n";
		output_mem_email_surfreal << mem << "\n";
		output_fpr_email_surfreal << fpr << "\n";
	    } else if (wkld_id == kWiki) {
		output_lat_wiki_surfreal << lat << "\n";
		output_mem_wiki_surfreal << mem << "\n";
		output_fpr_wiki_surfreal << fpr << "\n";
	    } else if (wkld_id == kUrl) {
		output_lat_url_surfreal << lat << "\n";
		output_mem_url_surfreal << mem << "\n";
		output_fpr_url_surfreal << fpr << "\n";
	    }
	}
    }
}

int main(int argc, char *argv[]) {
    int expt_id = (int)atoi(argv[1]);

    //-------------------------------------------------------------
    // Init Email Workload
    //-------------------------------------------------------------
    std::vector<std::string> load_emails;
    std::vector<std::string> insert_emails;
    std::vector<std::string> insert_emails_shuffle;
    std::vector<std::string> insert_emails_sample;
    std::vector<std::string> txn_emails;
    std::vector<std::string> upper_bound_emails;
    
    loadKeysFromFile(file_load_email, kNumEmailRecords, load_emails);
    selectKeysToInsert(kPercent, insert_emails, load_emails);
    load_emails.clear();
    
    for (int i = 0; i < (int)insert_emails.size(); i++) {
	insert_emails_shuffle.push_back(insert_emails[i]);
    }
    std::random_shuffle(insert_emails_shuffle.begin(), insert_emails_shuffle.end());
    
    for (int i = 0; i < (int)insert_emails_shuffle.size(); i += (100 / kSamplePercent)) {
	insert_emails_sample.push_back(insert_emails_shuffle[i]);
    }
    insert_emails_shuffle.clear();
    
    loadKeysFromFile(file_txn_email, kNumTxns, txn_emails);
    for (int i = 0; i < (int)txn_emails.size(); i++) {
	upper_bound_emails.push_back(getUpperBoundKey(txn_emails[i]));
    }
    std::cout << "insert_emails size = " << insert_emails.size() << std::endl;
    std::cout << "insert_emails_sample size = " << insert_emails_sample.size() << std::endl;
    std::cout << "txn_emails size = " << txn_emails.size() << std::endl;
    std::cout << "upper_bound_emails size = " << upper_bound_emails.size() << std::endl;

    int64_t email_point_tp
	= computePointTruePositives(insert_emails, txn_emails);
    int64_t email_range_tp
	= computeRangeTruePositives(insert_emails, txn_emails, upper_bound_emails);

    //-------------------------------------------------------------
    // Init Wiki Workload
    //-------------------------------------------------------------
    std::vector<std::string> load_wikis;
    std::vector<std::string> insert_wikis;
    std::vector<std::string> insert_wikis_shuffle;
    std::vector<std::string> insert_wikis_sample;
    std::vector<std::string> txn_wikis;
    std::vector<std::string> upper_bound_wikis;
    
    loadKeysFromFile(file_load_wiki, kNumWikiRecords, load_wikis);
    selectKeysToInsert(kPercent, insert_wikis, load_wikis);
    load_wikis.clear();
    
    for (int i = 0; i < (int)insert_wikis.size(); i++) {
	insert_wikis_shuffle.push_back(insert_wikis[i]);
    }
    std::random_shuffle(insert_wikis_shuffle.begin(), insert_wikis_shuffle.end());
    
    for (int i = 0; i < (int)insert_wikis_shuffle.size(); i += (100 / kSamplePercent)) {
	insert_wikis_sample.push_back(insert_wikis_shuffle[i]);
    }
    insert_wikis_shuffle.clear();
    
    loadKeysFromFile(file_txn_wiki, kNumTxns, txn_wikis);
    for (int i = 0; i < (int)txn_wikis.size(); i++) {
	upper_bound_wikis.push_back(getUpperBoundKey(txn_wikis[i]));
    }
    std::cout << "insert_wikis size = " << insert_wikis.size() << std::endl;
    std::cout << "insert_wikis_sample size = " << insert_wikis_sample.size() << std::endl;
    std::cout << "txn_wikis size = " << txn_wikis.size() << std::endl;
    std::cout << "upper_bound_wikis size = " << upper_bound_wikis.size() << std::endl;

    int64_t wiki_point_tp
	= computePointTruePositives(insert_wikis, txn_wikis);
    int64_t wiki_range_tp
	= computeRangeTruePositives(insert_wikis, txn_wikis, upper_bound_wikis);

    //-------------------------------------------------------------
    // Init URL Workload
    //-------------------------------------------------------------
    std::vector<std::string> load_urls;
    std::vector<std::string> insert_urls;
    std::vector<std::string> insert_urls_shuffle;
    std::vector<std::string> insert_urls_sample;
    std::vector<std::string> txn_urls;
    std::vector<std::string> upper_bound_urls;
    
    loadKeysFromFile(file_load_url, kNumEmailRecords, load_urls);
    selectKeysToInsert(kPercent, insert_urls, load_urls);
    load_urls.clear();
    
    for (int i = 0; i < (int)insert_urls.size(); i++) {
	insert_urls_shuffle.push_back(insert_urls[i]);
    }
    std::random_shuffle(insert_urls_shuffle.begin(), insert_urls_shuffle.end());
    
    for (int i = 0; i < (int)insert_urls_shuffle.size(); i += (100 / kSamplePercent)) {
	insert_urls_sample.push_back(insert_urls_shuffle[i]);
    }
    insert_urls_shuffle.clear();
    
    loadKeysFromFile(file_txn_url, kNumTxns, txn_urls);
    for (int i = 0; i < (int)txn_urls.size(); i++) {
	upper_bound_urls.push_back(getUpperBoundKey(txn_urls[i]));
    }
    std::cout << "insert_urls size = " << insert_urls.size() << std::endl;
    std::cout << "insert_urls_sample size = " << insert_urls_sample.size() << std::endl;
    std::cout << "txn_urls size = " << txn_urls.size() << std::endl;
    std::cout << "upper_bound_urls size = " << upper_bound_urls.size() << std::endl;

    int64_t url_point_tp
	= computePointTruePositives(insert_urls, txn_urls);
    int64_t url_range_tp
	= computeRangeTruePositives(insert_urls, txn_urls, upper_bound_urls);

    if (expt_id == 0) {
	//-------------------------------------------------------------
	// Point Queries; Expt ID = 0
	//-------------------------------------------------------------
	std::cout << "====================================" << std::endl;
	std::cout << "Point Queries; Expt ID = 0" << std::endl;
	std::cout << "====================================" << std::endl;

	//expt_id, kEmail, is_point, filter_type, suffix_len,
	//is_compressed, encoder_type, dict_size_limit,
	//email_point_true_positives, email_range_true_positives,
	//insert_keys, insert_keys_sample, txn_keys, upper_bound_keys

	output_lat_email_surf.open(file_lat_email_surf);
	output_mem_email_surf.open(file_mem_email_surf);
	output_fpr_email_surf.open(file_fpr_email_surf);

	output_lat_wiki_surf.open(file_lat_wiki_surf);
	output_mem_wiki_surf.open(file_mem_wiki_surf);
	output_fpr_wiki_surf.open(file_fpr_wiki_surf);

	output_lat_url_surf.open(file_lat_url_surf);
	output_mem_url_surf.open(file_mem_url_surf);
	output_fpr_url_surf.open(file_fpr_url_surf);

	output_lat_email_surfreal.open(file_lat_email_surfreal);
	output_mem_email_surfreal.open(file_mem_email_surfreal);
	output_fpr_email_surfreal.open(file_fpr_email_surfreal);

	output_lat_wiki_surfreal.open(file_lat_wiki_surfreal);
	output_mem_wiki_surfreal.open(file_mem_wiki_surfreal);
	output_fpr_wiki_surfreal.open(file_fpr_wiki_surfreal);

	output_lat_url_surfreal.open(file_lat_url_surfreal);
	output_mem_url_surfreal.open(file_mem_url_surfreal);
	output_fpr_url_surfreal.open(file_fpr_url_surfreal);

	int filter_type[2] = {0, 2};
	uint32_t suffix_len[2] = {0, 8};
	int dict_size[2] = {10000, 65536};

	int expt_num = 1;
	int total_num_expt = 42;

	for (int i = 0; i < 2; i++) {
	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kEmail, true, filter_type[i], suffix_len[i],
		 false, 0, 0, email_point_tp, email_range_tp,	     
		 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kWiki, true, filter_type[i], suffix_len[i],
		 false, 0, 0, wiki_point_tp, wiki_range_tp,
		 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kUrl, true, filter_type[i], suffix_len[i],
		 false, 0, 0, url_point_tp, url_range_tp,
		 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
	    expt_num++;
	    //=================================================
	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kEmail, true, filter_type[i], suffix_len[i],
		 true, 1, 1000, email_point_tp, email_range_tp,	     
		 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kWiki, true, filter_type[i], suffix_len[i],
		 true, 1, 1000, wiki_point_tp, wiki_range_tp,
		 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kUrl, true, filter_type[i], suffix_len[i],
		 true, 1, 1000, url_point_tp, url_range_tp,
		 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
	    expt_num++;
	    //=================================================
	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kEmail, true, filter_type[i], suffix_len[i],
		 true, 2, 65536, email_point_tp, email_range_tp,	     
		 insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kWiki, true, filter_type[i], suffix_len[i],
		 true, 2, 65536, wiki_point_tp, wiki_range_tp,
		 insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
	    expt_num++;

	    std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
	    exec(expt_id, kUrl, true, filter_type[i], suffix_len[i],
		 true, 2, 65536, url_point_tp, url_range_tp,
		 insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
	    expt_num++;
	    
	    for (int j = 0; j < 2; j++) {
		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kEmail, true, filter_type[i], suffix_len[i],
		     true, 3, dict_size[j], email_point_tp, email_range_tp,	     
		     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
		expt_num++;

		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kWiki, true, filter_type[i], suffix_len[i],
		     true, 3, dict_size[j], wiki_point_tp, wiki_range_tp,
		     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
		expt_num++;

		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kUrl, true, filter_type[i], suffix_len[i],
		     true, 3, dict_size[j], url_point_tp, url_range_tp,
		     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
		expt_num++;
	    }

	    for (int j = 0; j < 2; j++) {
		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kEmail, true, filter_type[i], suffix_len[i],
		     true, 4, dict_size[j], email_point_tp, email_range_tp,	     
		     insert_emails, insert_emails_sample, txn_emails, upper_bound_emails);
		expt_num++;

		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kWiki, true, filter_type[i], suffix_len[i],
		     true, 4, dict_size[j], wiki_point_tp, wiki_range_tp,
		     insert_wikis, insert_wikis_sample, txn_wikis, upper_bound_wikis);
		expt_num++;

		std::cout << "-------------" << expt_num << "/" << total_num_expt << "--------------" << std::endl;
		exec(expt_id, kUrl, true, filter_type[i], suffix_len[i],
		     true, 4, dict_size[j], url_point_tp, url_range_tp,
		     insert_urls, insert_urls_sample, txn_urls, upper_bound_urls);
		expt_num++;
	    }
	}

	output_lat_email_surf.close();
	output_mem_email_surf.close();
	output_fpr_email_surf.close();

	output_lat_wiki_surf.close();
	output_mem_wiki_surf.close();
	output_fpr_wiki_surf.close();

	output_lat_url_surf.close();
	output_mem_url_surf.close();
	output_fpr_url_surf.close();

	output_lat_email_surfreal.close();
	output_mem_email_surfreal.close();
	output_fpr_email_surfreal.close();

	output_lat_wiki_surfreal.close();
	output_mem_wiki_surfreal.close();
	output_fpr_wiki_surfreal.close();

	output_lat_url_surfreal.close();
	output_mem_url_surfreal.close();
	output_fpr_url_surfreal.close();
    }
    else if (expt_id == 1) {
	//-------------------------------------------------------------
	// Range Queries; Expt ID = 1
	//-------------------------------------------------------------
	std::cout << "====================================" << std::endl;
	std::cout << "Range Queries; Expt ID = 1" << std::endl;
	std::cout << "====================================" << std::endl;
    }

    return 0;
}
