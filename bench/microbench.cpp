#include <sys/time.h>
#include <set>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "encoder_factory.hpp"

namespace microbench {

static const std::string file_email = "datasets/emails.txt";
static const std::string file_wiki = "datasets/wikis.txt";
static const std::string file_url = "datasets/urls.txt";
static const std::string file_ts = "datasets/poisson_timestamps.csv";
static const int kLongestCodeLen = 4096;

static int runALM = 1;

//-------------------------------------------------------------
// Workload IDs
//-------------------------------------------------------------
static const int kEmail = 0;
static const int kWiki = 1;
static const int kUrl = 2;
static const int kTs = 3;

static const std::string output_dir = "results/microbench/";

//-------------------------------------------------------------
// Sample Size Sweep
//-------------------------------------------------------------
static const std::string sample_size_sweep_subdir = "sample_size_sweep/";
static const std::string file_cpr_email_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "cpr_email_sample_size_sweep.csv";
std::ofstream output_cpr_email_sample_size_sweep;
static const std::string file_bt_email_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "bt_email_sample_size_sweep.csv";
std::ofstream output_bt_email_sample_size_sweep;
static const std::string file_cpr_wiki_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "cpr_wiki_sample_size_sweep.csv";
std::ofstream output_cpr_wiki_sample_size_sweep;
static const std::string file_bt_wiki_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "bt_wiki_sample_size_sweep.csv";
std::ofstream output_bt_wiki_sample_size_sweep;
static const std::string file_cpr_url_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "cpr_url_sample_size_sweep.csv";
std::ofstream output_cpr_url_sample_size_sweep;
static const std::string file_bt_url_sample_size_sweep
= output_dir + sample_size_sweep_subdir + "bt_url_sample_size_sweep.csv";
std::ofstream output_bt_url_sample_size_sweep;

//-------------------------------------------------------------
// CPR and Latency
//-------------------------------------------------------------
static const std::string cpr_latency_subdir = "cpr_latency/";
static const std::string file_x_email_dict_size
= output_dir + cpr_latency_subdir + "x_email_dict_size.csv";
std::ofstream output_x_email_dict_size;
static const std::string file_cpr_email_dict_size
= output_dir + cpr_latency_subdir + "cpr_email_dict_size.csv";
std::ofstream output_cpr_email_dict_size;
static const std::string file_lat_email_dict_size
= output_dir + cpr_latency_subdir + "lat_email_dict_size.csv";
std::ofstream output_lat_email_dict_size;
static const std::string file_mem_email_dict_size
= output_dir + cpr_latency_subdir + "mem_email_dict_size.csv";
std::ofstream output_mem_email_dict_size;

static const std::string file_x_wiki_dict_size
= output_dir + cpr_latency_subdir + "x_wiki_dict_size.csv";
std::ofstream output_x_wiki_dict_size;
static const std::string file_cpr_wiki_dict_size
= output_dir + cpr_latency_subdir + "cpr_wiki_dict_size.csv";
std::ofstream output_cpr_wiki_dict_size;
static const std::string file_lat_wiki_dict_size
= output_dir + cpr_latency_subdir + "lat_wiki_dict_size.csv";
std::ofstream output_lat_wiki_dict_size;
static const std::string file_mem_wiki_dict_size
= output_dir + cpr_latency_subdir + "mem_wiki_dict_size.csv";
std::ofstream output_mem_wiki_dict_size;

static const std::string file_x_url_dict_size
= output_dir + cpr_latency_subdir + "x_url_dict_size.csv";
std::ofstream output_x_url_dict_size;
static const std::string file_cpr_url_dict_size
= output_dir + cpr_latency_subdir + "cpr_url_dict_size.csv";
std::ofstream output_cpr_url_dict_size;
static const std::string file_lat_url_dict_size
= output_dir + cpr_latency_subdir + "lat_url_dict_size.csv";
std::ofstream output_lat_url_dict_size;
static const std::string file_mem_url_dict_size
= output_dir + cpr_latency_subdir + "mem_url_dict_size.csv";
std::ofstream output_mem_url_dict_size;

static const std::string file_x_ts_dict_size
= output_dir + cpr_latency_subdir + "x_ts_dict_size.csv";
std::ofstream output_x_ts_dict_size;
static const std::string file_cpr_ts_dict_size
= output_dir + cpr_latency_subdir + "cpr_ts_dict_size.csv";
std::ofstream output_cpr_ts_dict_size;
static const std::string file_lat_ts_dict_size
= output_dir + cpr_latency_subdir + "lat_ts_dict_size.csv";
std::ofstream output_lat_ts_dict_size;
static const std::string file_mem_ts_dict_size
= output_dir + cpr_latency_subdir + "mem_ts_dict_size.csv";
std::ofstream output_mem_ts_dict_size;

//-------------------------------------------------------------
// Hu-Tucker vs. Fixed Length Dict Codes
//-------------------------------------------------------------
static const std::string ht_vs_dc_subdir = "ht_vs_dc/";
static const std::string file_x_email_ht
= output_dir + ht_vs_dc_subdir + "x_email_ht.csv";
std::ofstream output_x_email_ht;
static const std::string file_cpr_email_ht
= output_dir + ht_vs_dc_subdir + "cpr_email_ht.csv";
std::ofstream output_cpr_email_ht;
static const std::string file_lat_email_ht
= output_dir + ht_vs_dc_subdir + "lat_email_ht.csv";
std::ofstream output_lat_email_ht;
static const std::string file_bt_email_ht
= output_dir + ht_vs_dc_subdir + "bt_email_ht.csv";
std::ofstream output_bt_email_ht;

static const std::string file_x_email_dc
= output_dir + ht_vs_dc_subdir + "x_email_dc.csv";
std::ofstream output_x_email_dc;
static const std::string file_cpr_email_dc
= output_dir + ht_vs_dc_subdir + "cpr_email_dc.csv";
std::ofstream output_cpr_email_dc;
static const std::string file_lat_email_dc
= output_dir + ht_vs_dc_subdir + "lat_email_dc.csv";
std::ofstream output_lat_email_dc;
static const std::string file_bt_email_dc
= output_dir + ht_vs_dc_subdir + "bt_email_dc.csv";
std::ofstream output_bt_email_dc;

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

std::string uint64ToString(uint64_t key) {
    uint64_t endian_swapped_key = __builtin_bswap64(key);
    return std::string(reinterpret_cast<const char*>(&endian_swapped_key), 8);
}

int64_t loadKeysInt(const std::string& file_name,
                 std::vector<std::string>& keys,
                 std::vector<std::string>& keys_shuffle) {
    std::ifstream infile(file_name);
    uint64_t int_key;
    int64_t total_len = 0;
    uint64_t count = 0;
    std::set<uint64_t> int_keys;
    int continue_cnt = 0;
    uint64_t cnt = 0;
    while (infile.good() && cnt < 100000000) {
        infile >> int_key;
	if (int_keys.find(int_key) != int_keys.end()) {
            continue_cnt++;
            std::cout << "continue cnt:" << continue_cnt << std::endl;
	    continue;
        }
        cnt += 1;
        int_keys.insert(int_key);
        std::string key = uint64ToString(int_key);
        keys.push_back(key);
        keys_shuffle.push_back(key);
        count++;
        total_len += key.length();
        assert(key.length() == 8);
    }
    std::random_shuffle(keys_shuffle.begin(), keys_shuffle.end());
    return total_len;
}

int64_t loadKeys(const std::string& file_name,
		 std::vector<std::string> &keys,
		 std::vector<std::string> &keys_shuffle) {
    std::ifstream infile(file_name);
    std::string key;
    int64_t total_len = 0;
    int cnt = 0;
    while (infile.good()) {
        cnt++;
	infile >> key;
	keys.push_back(key);
	keys_shuffle.push_back(key);
	total_len += key.length();
    }
    std::random_shuffle(keys_shuffle.begin(), keys_shuffle.end());
    return total_len;
}

void printStr(std::string str) {
    for (int i = 0; i < (int)str.length(); i++) {
        std::cout << std::hex << (int)str[i] << " ";
    }
    std::cout << std::endl;
}

void exec(const int expt_id, const int wkld_id,
	  const int encoder_type, const int64_t dict_size_limit,
	  const double sample_percent,
	  const std::vector<std::string>& keys_shuffle,
	  const int64_t total_len) {
    std::vector<std::string> sample_keys;
    int step_size = 100 / sample_percent;
    std::cout << "Step size:" << step_size << ", total size:" << keys_shuffle.size() << std::endl;
    for (int i = 0; i < (int)keys_shuffle.size(); i += step_size) {
	sample_keys.push_back(keys_shuffle[i]);
    }
    
    ope::Encoder* encoder = ope::EncoderFactory::createEncoder(encoder_type);
    double time_start = getNow();
    encoder->build(sample_keys, dict_size_limit);
    double time_end = getNow();
    double bt = time_end - time_start;

    int dict_size = encoder->numEntries();
    int64_t mem = encoder->memoryUse();

    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    uint64_t total_enc_len = 0;
    time_start = getNow();

    for (int i = 0; i < (int)keys_shuffle.size(); i++) {
	    total_enc_len += encoder->encode(keys_shuffle[i], buffer);
    }
    delete[] buffer;
    delete encoder;
    time_end = getNow();
    double time_diff = time_end - time_start;
    double tput = keys_shuffle.size() / time_diff / 1000000; // in Mops/s
    double lat = time_diff * 1000000000 / total_len; // in ns
    double cpr = (total_len * 8.0) / total_enc_len;

    if (expt_id < 0 || expt_id > 6)
	std::cout << "ERROR: INVALID EXPT ID!" << std::endl;
    
    if (wkld_id < 0 || wkld_id > 3)
	std::cout << "ERROR: INVALID WKLD ID!" << std::endl;

    if (expt_id == 0) {
	if (wkld_id == kEmail) {
	    output_cpr_email_sample_size_sweep << cpr << "\n";
	    output_bt_email_sample_size_sweep << bt << "\n";
	} else if (wkld_id == kWiki) {
	    output_cpr_wiki_sample_size_sweep << cpr << "\n";
	    output_bt_wiki_sample_size_sweep << bt << "\n";
	} else if (wkld_id == kUrl) {
	    output_cpr_url_sample_size_sweep << cpr << "\n";
	    output_bt_url_sample_size_sweep << bt << "\n";
	}
    } else if (expt_id == 1) {
	if (wkld_id == kEmail) {
	    output_x_email_dict_size << dict_size << "\n";
	    output_cpr_email_dict_size << cpr << "\n";
	    output_lat_email_dict_size << lat << "\n";
	    output_mem_email_dict_size << mem << "\n";
	} else if (wkld_id == kWiki) {
	    output_x_wiki_dict_size << dict_size << "\n";
	    output_cpr_wiki_dict_size << cpr << "\n";
	    output_lat_wiki_dict_size << lat << "\n";
	    output_mem_wiki_dict_size << mem << "\n";
	} else if (wkld_id == kUrl) {
	    output_x_url_dict_size << dict_size << "\n";
	    output_cpr_url_dict_size << cpr << "\n";
	    output_lat_url_dict_size << lat << "\n";
	    output_mem_url_dict_size << mem << "\n";
	}  else if (wkld_id == kTs) {
	    output_x_ts_dict_size << dict_size << "\n";
	    output_cpr_ts_dict_size << cpr << "\n";
	    output_lat_ts_dict_size << lat << "\n";
	    output_mem_ts_dict_size << mem << "\n";
	}
    } else if (expt_id == 4) {
	if (wkld_id == kEmail) {
	    output_x_email_ht << dict_size << "\n";
	    output_cpr_email_ht << cpr << "\n";
	    output_lat_email_ht << lat << "\n";
	    output_bt_email_ht << bt << "\n";
	}
    } else if (expt_id == 5) {
	if (wkld_id == kEmail) {
	    output_x_email_dc << dict_size << "\n";
	    output_cpr_email_dc << cpr << "\n";
	    output_lat_email_dc << lat << "\n";
	    output_bt_email_dc << bt << "\n";
	}
    }
    
    std::cout << "Throughput = " << tput << " Mops/s" << std::endl;
    std::cout << "Latency = " << lat << " ns/char" << std::endl;
    std::cout << "CPR = " << cpr << std::endl;
    std::cout << "Dict Size = " << dict_size << std::endl;
    std::cout << "Memory = " << mem << std::endl;
}
} // namespace microbench

using namespace microbench;

int main(int argc, char *argv[]) {
    int expt_id = (int)atoi(argv[1]);
    runALM = (int)atoi(argv[2]);

    std::vector<std::string> emails;
    std::vector<std::string> emails_shuffle;
    int64_t total_len_email =  0;
//    int64_t total_len_email = loadKeys(file_email, emails, emails_shuffle);

    std::vector<std::string> wikis;
    std::vector<std::string> wikis_shuffle;
    int64_t total_len_wiki = 0;
//  int64_t total_len_wiki = loadKeys(file_wiki, wikis, wikis_shuffle);

    std::vector<std::string> urls;
    std::vector<std::string> urls_shuffle;
    int64_t total_len_url = 0;
//    int64_t total_len_url = loadKeys(file_url, urls, urls_shuffle);

    std::vector<std::string> tss;
    std::vector<std::string> tss_shuffle;
//    int64_t total_len_ts = 0;
    int64_t total_len_ts = loadKeysInt(file_ts, tss, tss_shuffle);

    if (expt_id == 0) {
	//-------------------------------------------------------------
	// Sample Size Sweep; Expt ID = 0
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Sample Size Sweep; Expt ID = 0" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	output_cpr_email_sample_size_sweep.open(file_cpr_email_sample_size_sweep);
	output_bt_email_sample_size_sweep.open(file_bt_email_sample_size_sweep);
	output_cpr_wiki_sample_size_sweep.open(file_cpr_wiki_sample_size_sweep);
	output_bt_wiki_sample_size_sweep.open(file_bt_wiki_sample_size_sweep);
	output_cpr_url_sample_size_sweep.open(file_cpr_url_sample_size_sweep);
	output_bt_url_sample_size_sweep.open(file_bt_url_sample_size_sweep);

	int dict_size_limit = 65536;
	int percent_list[3] = {100, 10, 1};
	int expt_num = 0;
	int total_num_expts = 36;
	for (int p = 0; p < 3; p++) {
	    int percent = percent_list[p];
	    for (int et = 1; et < 5; et++) {
		std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
		exec(expt_id, kEmail, et, dict_size_limit, percent, emails_shuffle, total_len_email);
		std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
		exec(expt_id, kWiki, et, dict_size_limit, percent, wikis_shuffle, total_len_wiki);
		std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
		exec(expt_id, kUrl, et, dict_size_limit, percent, urls_shuffle, total_len_url);
	    }
	}

	output_cpr_email_sample_size_sweep.close();
	output_bt_email_sample_size_sweep.close();
	output_cpr_wiki_sample_size_sweep.close();
	output_bt_wiki_sample_size_sweep.close();
	output_cpr_url_sample_size_sweep.close();
	output_bt_url_sample_size_sweep.close();
    }
    else if (expt_id == 1) {
	//-------------------------------------------------------------
	// CPR and Latency; Expt ID = 1
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "CPR and Latency; Expt ID = 1" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;
	output_x_email_dict_size.open(file_x_email_dict_size, std::fstream::app);
	output_cpr_email_dict_size.open(file_cpr_email_dict_size, std::fstream::app);
	output_lat_email_dict_size.open(file_lat_email_dict_size, std::fstream::app);
	output_mem_email_dict_size.open(file_mem_email_dict_size, std::fstream::app);

	output_x_wiki_dict_size.open(file_x_wiki_dict_size, std::fstream::app);
	output_cpr_wiki_dict_size.open(file_cpr_wiki_dict_size, std::fstream::app);
	output_lat_wiki_dict_size.open(file_lat_wiki_dict_size, std::fstream::app);
	output_mem_wiki_dict_size.open(file_mem_wiki_dict_size, std::fstream::app);

	output_x_url_dict_size.open(file_x_url_dict_size, std::fstream::app);
	output_cpr_url_dict_size.open(file_cpr_url_dict_size, std::fstream::app);
	output_lat_url_dict_size.open(file_lat_url_dict_size, std::fstream::app);
	output_mem_url_dict_size.open(file_mem_url_dict_size, std::fstream::app);

    output_x_ts_dict_size.open(file_x_ts_dict_size, std::fstream::app);
	output_cpr_ts_dict_size.open(file_cpr_ts_dict_size, std::fstream::app);
	output_lat_ts_dict_size.open(file_lat_ts_dict_size, std::fstream::app);
	output_mem_ts_dict_size.open(file_mem_ts_dict_size, std::fstream::app);

	int percent = 1;
    double url_percent = 0.1;
    int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
	//int dict_size_list[5] = {16384, 32768, 65536, 131072, 262144};
	int expt_num = 1;
	int total_num_expts = 81;
/*
	// Single-Char
	std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kEmail, 1, 1000, percent, emails_shuffle, total_len_email);
	std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kWiki, 1, 1000, percent, wikis_shuffle, total_len_wiki);
	
    std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kUrl, 1, 1000, url_percent, urls_shuffle, total_len_url);
	expt_num++;
	std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kTs, 1, 1000, percent, tss_shuffle, total_len_ts);
	expt_num++;

	// Double-Char
	std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kEmail, 2, 65536, percent, emails_shuffle, total_len_email);
	std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kWiki, 2, 65536, percent, wikis_shuffle, total_len_wiki);

	std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kUrl, 2, 65536, url_percent, urls_shuffle, total_len_url);
	expt_num++;
    std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
	exec(expt_id, kTs, 2, 65536, percent, tss_shuffle, total_len_ts);
	expt_num++;
*/

    int stop_method = 0;
    if (runALM == 1) {
        stop_method = 6;
    } else {
        stop_method = 5;
    }

	for (int ds = 0; ds < 7; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    for (int et = 5; et < stop_method; et++) {
	/*	std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
		exec(expt_id, kEmail, et, dict_size_limit, percent, emails_shuffle, total_len_email);
		expt_num++;
		std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
		exec(expt_id, kWiki, et, dict_size_limit, percent, wikis_shuffle, total_len_wiki);
		expt_num++;
		std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
		exec(expt_id, kUrl, et, dict_size_limit, url_percent, urls_shuffle, total_len_url);
		expt_num++;
      */
        if (et == 5)
            continue;
        std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
		exec(expt_id, kTs, et, dict_size_limit, percent, tss_shuffle, total_len_ts);
		expt_num++;
	    }
	}

/*   
    for (int ds = 7; ds < 9; ds++) {
        int dict_size_limit = dict_size_list[ds];
        for  (int et = 4; et < stop_method;et++) {
             std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
            exec(expt_id, kEmail, et, dict_size_limit, percent, emails_shuffle, total_len_email);
            expt_num++;
            std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
            exec(expt_id, kWiki, et, dict_size_limit, percent, wikis_shuffle, total_len_wiki);
            expt_num++;
            std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
            exec(expt_id, kUrl, et, dict_size_limit, url_percent, urls_shuffle, total_len_url);
            expt_num++;

            if (et == 5)
                continue;
            std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
            exec(expt_id, kTs, et, dict_size_limit, percent, tss_shuffle, total_len_ts);
            expt_num++;

        }
    }
    
*/
   	output_x_email_dict_size << "-" << "\n";
	output_cpr_email_dict_size << "-" << "\n";
	output_lat_email_dict_size << "-" << "\n";
	output_mem_email_dict_size << "-" << "\n";

	output_x_email_dict_size.close();
	output_cpr_email_dict_size.close();
	output_lat_email_dict_size.close();
	output_mem_email_dict_size.close();

    output_x_wiki_dict_size << "-" << "\n";
	output_cpr_wiki_dict_size << "-" << "\n";
	output_lat_wiki_dict_size << "-" << "\n";
	output_mem_wiki_dict_size << "-" << "\n";

	output_x_wiki_dict_size.close();
	output_cpr_wiki_dict_size.close();
	output_lat_wiki_dict_size.close();
	output_mem_wiki_dict_size.close();

    output_x_url_dict_size << "-" << "\n";
	output_cpr_url_dict_size << "-" << "\n";
	output_lat_url_dict_size << "-" << "\n";
	output_mem_url_dict_size << "-" << "\n";

	output_x_url_dict_size.close();
	output_cpr_url_dict_size.close();
	output_lat_url_dict_size.close();
	output_mem_url_dict_size.close();

    output_x_ts_dict_size << "-" << "\n";
	output_cpr_ts_dict_size << "-" << "\n";
	output_lat_ts_dict_size << "-" << "\n";
	output_mem_ts_dict_size << "-" << "\n";

    output_x_ts_dict_size.close();
	output_cpr_ts_dict_size.close();
	output_lat_ts_dict_size.close();
	output_mem_ts_dict_size.close();

    }
    else if (expt_id == 2) {
	//-------------------------------------------------------------
	// Trie vs Array; Expt ID = 2
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Trie vs Array; Expt ID = 2" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	int percent = 10;
	int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
	int expt_num = 1;
	int total_num_expts = 16;
	for (int ds = 0; ds < 7; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    std::cout << "Trie vs Array " << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, 3, dict_size_limit, percent, emails_shuffle, total_len_email);
	}

	for (int ds = 0; ds < 9; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    std::cout << "Trie vs Array " << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, 4, dict_size_limit, percent, emails_shuffle, total_len_email);
	}
    }
    else if (expt_id == 3) {
	//-------------------------------------------------------------
	// Hu-Tucker Build Time; Expt ID = 3
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Hu-Tucker Build Time; Expt ID = 3" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	int percent = 10;
	int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
	int encoder_type = 4;
	int expt_num = 0;
	int total_num_expts = 9;
	for (int ds = 0; ds < 9; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    std::cout << "Hu-Tucker Build Time " << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, encoder_type, dict_size_limit, percent, emails_shuffle, total_len_email);
	}
    }
    else if (expt_id == 4) {
	//-------------------------------------------------------------
	// Hu-Tucker vs. Fixed Length Dict Codes (Part 1: Hu-Tucker)
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 1: Hu-Tucker); Expt ID = 4" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	output_x_email_ht.open(file_x_email_ht);
	output_cpr_email_ht.open(file_cpr_email_ht);
	output_lat_email_ht.open(file_lat_email_ht);
	output_bt_email_ht.open(file_bt_email_ht);

	int percent = 10;
	int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
	int encoder_type = 4;
	int expt_num = 0;
	int total_num_expts = 9;
	for (int ds = 0; ds < 9; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 1: Hu-Tucker) "
		      << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, encoder_type, dict_size_limit, percent, emails_shuffle, total_len_email);
	}

	output_x_email_ht.close();
	output_cpr_email_ht.close();
	output_lat_email_ht.close();
	output_bt_email_ht.close();
    }
    else if (expt_id == 5) {
	//-------------------------------------------------------------
	// Hu-Tucker vs. Fixed Length Dict Codes (Part 2: Dict Codes)
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 2: Dict Codes); Expt ID = 5" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	output_x_email_dc.open(file_x_email_dc);
	output_cpr_email_dc.open(file_cpr_email_dc);
	output_lat_email_dc.open(file_lat_email_dc);
	output_bt_email_dc.open(file_bt_email_dc);

	int percent = 10;
	int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
	int encoder_type = 4;
	int expt_num = 0;
	int total_num_expts = 9;
	for (int ds = 0; ds < 9; ds++) {
	    int dict_size_limit = dict_size_list[ds];
	    std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 2: Dict Codes) "
		      << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, encoder_type, dict_size_limit, percent, emails_shuffle, total_len_email);
	}

	output_x_email_dc.close();
	output_cpr_email_dc.close();
	output_lat_email_dc.close();
	output_bt_email_dc.close();
    }
    else if (expt_id == 6) {
	//-------------------------------------------------------------
	// Build Time Breakdown; Expt ID = 6
	//-------------------------------------------------------------
	std::cout << "------------------------------------------------" << std::endl;
	std::cout << "Build Time Breakdown; Expt ID = 6" << std::endl;
	std::cout << "------------------------------------------------" << std::endl;

	int percent_list[3] = {100, 10, 1};
	int dict_size_limit = 65536;
	int encoder_type = 4;
	int expt_num = 0;
	int total_num_expts = 3;
	for (int p = 0; p < 3; p++) {
	    std::cout << "Build Time Breakdown " << (expt_num++) << "/" << total_num_expts << std::endl;
	    exec(expt_id, kEmail, encoder_type, dict_size_limit, percent_list[p], emails_shuffle, total_len_email);
	}
    }
    
    return 0;
}
