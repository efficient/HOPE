#include <sys/time.h>
#include <set>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include "encoder_factory.hpp"
#include "common.hpp"

#define RUN_BATCH

namespace microbench {

static const std::string file_email = "datasets/emails.txt";
static const std::string file_wiki = "datasets/wikis.txt";
static const std::string file_url = "datasets/urls.txt";
static const std::string file_ts = "datasets/poisson_timestamps.csv";
static const std::string file_email1 = "datasets/email_1.txt";
static const std::string file_email2 = "datasets/email_2.txt";
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

//-------------------------------------------------------------
// Batch Encode vs Non-batch
//-------------------------------------------------------------
static const std::string batch_subdir = "batch/";
static const std::string file_batch_lat
= output_dir + batch_subdir + "batch_lat.csv";
std::ofstream output_batch_lat;

//------------------------------------------------------------
// Insert Percentage
//-----------------------------------------------------------
static const std::string per_subdir = "percentage/";
static const std::string file_per_email = output_dir + per_subdir + "per_cpr_lat.csv";
std::ofstream output_per_cpr_lat;

double getNow() {
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + tv.tv_usec / 1000000.0;
}

std::string uint64ToString(uint64_t key) {
    uint64_t endian_swapped_key = __builtin_bswap64(key);
    return std::string(reinterpret_cast<const char*>(&endian_swapped_key), 8);
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

/* choose sample_percent keys*/
void getSampleKeys(const double sample_percent,
                    std::vector<std::string> &sample_keys,
                    const std::vector<std::string> &all_keys,
                    int64_t &enc_src_len) {
    int sample_size = (int)(all_keys.size() * sample_percent / 100);
    enc_src_len = 0;
    for (int i = 0; i < sample_size; i++) {
        sample_keys.push_back(all_keys[i]);
        enc_src_len += all_keys[i].length();
    }
}

void exec_helper(const int encoder_type, const int W, const int input_dict_size,
                 const std::vector<std::string> sample_keys,
                 const std::vector<std::string> enc_src_keys,
                 const int64_t enc_src_len, int encode_method, const int batch_size,
                 double &bt, double &tput, double &lat, double &cpr, double &dict_size, double &mem) {
    ope::Encoder* encoder = ope::EncoderFactory::createEncoder(encoder_type, W);
    double time_start = getNow();
    encoder->build(sample_keys, input_dict_size);
    double time_end = getNow();
    bt = time_end - time_start;

    dict_size = encoder->numEntries();
    mem = encoder->memoryUse();

    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    uint8_t* lb = new uint8_t[kLongestCodeLen];
    uint8_t* rb = new uint8_t[kLongestCodeLen];
    int64_t total_enc_len = 0;

    time_start = getNow();
    std::vector<std::string> enc_keys;

    // encode one string each time
    if (encode_method == 0) {
        for (int i = 0; i < (int)enc_src_keys.size(); i++) {
            total_enc_len += encoder->encode(enc_src_keys[i], buffer);
        }
    // encode a pair of strings
    } else if (encode_method == 1) {
        int l_len;
        int r_len;
        for (int i = 0; i < (int)enc_src_keys.size() - 1; i+=2) {
            encoder->encodePair(enc_src_keys[i], enc_src_keys[i+1], lb, rb, l_len, r_len);
            total_enc_len += l_len + r_len;
        }
    // encode a batch of strings
    } else if (encode_method == 2) {
        for (int i = 0; i <= (int)enc_src_keys.size() - batch_size; i += batch_size) {
            total_enc_len += encoder->encodeBatch(enc_src_keys, i, batch_size, enc_keys);
        }
    }
    time_end = getNow();
    double time_diff = time_end - time_start;
    tput = enc_src_keys.size() / time_diff / 1000000; // in Mops/s
    lat = time_diff * 1000000000 / enc_src_len; // in ns
    cpr = (enc_src_len * 8.0) / total_enc_len;
    delete[] buffer;
    delete lb;
    delete rb;
    delete encoder;

    std::cout << "Throughput = " << tput << " Mops/s" << std::endl;
    std::cout << "Latency = " << lat << " ns/char" << std::endl;
    std::cout << "CPR = " << cpr << std::endl;
    std::cout << "Dict Size = " << dict_size << std::endl;
    std::cout << "Memory = " << mem << std::endl;
}

void exec(const int expt_id, const int wkld_id,
      const int encoder_type, const int64_t dict_size_id,
      const double sample_percent, const double enc_percent,
      std::vector<std::string> &keys_shuffle,
      const int64_t total_len, int encode_method = 0, int batch_size = 3) {
#ifdef RUN_BATCH
    std::sort(keys_shuffle.begin(), keys_shuffle.end());
#endif
    std::vector<std::string> sample_keys;
    int64_t sample_enc_src_len = 0;
    getSampleKeys(sample_percent, sample_keys, keys_shuffle, sample_enc_src_len);

    // Get parameters from file to speed up building the encoder
    int64_t input_dict_size = 0;
    if (encoder_type == 3) {
        input_dict_size = three_gram_input_dict_size[wkld_id][dict_size_id];
    } else if (encoder_type == 4) {
        input_dict_size = four_gram_input_dict_size[wkld_id][dict_size_id];
    } else {
        input_dict_size = dict_size_list[dict_size_id];
    }

    int  W = 0;
    if (encoder_type == 5) {
        W = ALM_W[wkld_id][dict_size_id];
    }
    if (encoder_type == 6) {
        W = ALM_W_improved[wkld_id][dict_size_id];
    }

    std::vector<std::string> enc_src_keys;
    int64_t enc_src_len = 0;
    getSampleKeys(enc_percent, enc_src_keys, keys_shuffle, enc_src_len);
    double tput = 0;
    double lat = 0;
    double cpr = 0;
    double bt = 0;
    double dict_size = 0;
    double mem = 0;
    exec_helper(encoder_type, W, input_dict_size,
                 sample_keys, enc_src_keys,
                 enc_src_len, encode_method,
                 batch_size, bt, tput, lat, cpr, dict_size, mem);

    if (expt_id < 0 || expt_id > 9)
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
    } else if (expt_id == 7) {
        output_batch_lat << lat << "\n";
    } else if (expt_id == 8) {
        output_per_cpr_lat << cpr << "\n";
        output_per_cpr_lat << lat << "\n";
    }

}
} // namespace microbench

using namespace microbench;

int main(int argc, char *argv[]) {
    int expt_id = (int)atoi(argv[1]);
    runALM = (int)atoi(argv[2]);

    std::vector<std::string> emails;
    std::vector<std::string> emails1;
    std::vector<std::string> emails2;
    std::vector<std::string> emails_shuffle;
    std::vector<std::string> emails1_shuffle;
    std::vector<std::string> emails2_shuffle;
    int64_t total_len_email = loadKeys(file_email, emails, emails_shuffle);
    int64_t total_len_email1 = loadKeys(file_email1, emails1, emails1_shuffle);
    int64_t total_len_email2 = loadKeys(file_email2, emails2, emails2_shuffle);

    std::vector<std::string> wikis;
    std::vector<std::string> wikis_shuffle;
    int64_t total_len_wiki = loadKeys(file_wiki, wikis, wikis_shuffle);

    std::vector<std::string> urls;
    std::vector<std::string> urls_shuffle;
    int64_t total_len_url = loadKeys(file_url, urls, urls_shuffle);

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

        int ds = 6; // 65536
        int percent_list[3] = {100, 10, 1};
        int enc_src_percent = 100;
        int expt_num = 0;
        int total_num_expts = 36;
        for (int p = 0; p < 3; p++) {
            int percent = percent_list[p];
            for (int et = 1; et < 5; et++) {
            std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, et, ds, percent, enc_src_percent, emails_shuffle, total_len_email);
            std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kWiki, et, ds, percent, enc_src_percent, wikis_shuffle, total_len_wiki);
            std::cout << "Sample Size Sweep " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kUrl, et, ds, percent, enc_src_percent, urls_shuffle, total_len_url);
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

        int sample_percent = 1;
        int enc_src_percent = 100;
        int expt_num = 1;
        int total_num_expts = 0;

        int stop_method = 0;
        if (runALM == 1) {
            stop_method = 7;
            total_num_expts = 108;
        } else {
            stop_method = 5;
            total_num_expts = 57;
        }

        // Single-Char
        std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kEmail, 1, 0, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
        std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kWiki, 1, 0, sample_percent, enc_src_percent, wikis_shuffle, total_len_wiki);

        std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kUrl, 1, 0, sample_percent, enc_src_percent, urls_shuffle, total_len_url);
        expt_num++;

        // Double-Char
        std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kEmail, 2, 6, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
        std::cout << "CPR and Latency (" << (expt_num++) << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kWiki, 2, 6, sample_percent, enc_src_percent, wikis_shuffle, total_len_wiki);

        std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
        exec(expt_id, kUrl, 2, 6, sample_percent, enc_src_percent, urls_shuffle, total_len_url);
        expt_num++;

        for (int ds = 0; ds < 7; ds++) {
            for (int et = 3; et < stop_method; et++) {
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kEmail, et, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
                expt_num++;
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kWiki, et, ds, sample_percent, enc_src_percent, wikis_shuffle, total_len_wiki);
                expt_num++;
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kUrl, et, ds, sample_percent, enc_src_percent, urls_shuffle, total_len_url);
                expt_num++;
            }
        }

        for (int ds = 7; ds < 9; ds++) {
            for  (int et = 4; et < stop_method;et++) {
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kEmail, et, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
                expt_num++;
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kWiki, et, ds, sample_percent, enc_src_percent, wikis_shuffle, total_len_wiki);
                expt_num++;
                std::cout << "CPR and Latency (" << expt_num << "/" << total_num_expts << ")" << std::endl;
                exec(expt_id, kUrl, et, ds, sample_percent, enc_src_percent, urls_shuffle, total_len_url);
                expt_num++;
            }
        }
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
    }
    else if (expt_id == 2) {
        //-------------------------------------------------------------
        // Trie vs Array; Expt ID = 2
        //-------------------------------------------------------------
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Trie vs Array; Expt ID = 2" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        int sample_percent = 1;
        int enc_src_percent = 100;
        //int expt_num = 1;
        //int total_num_expts = 16;
        /*for (int ds = 0; ds < 7; ds++) {
            std::cout << "Trie vs Array " << (expt_num++) << "/" << total_num_expts << std::endl;
        }

        for (int ds = 0; ds < 9; ds++) {
            //int dict_size_limit = dict_size_list[ds];
            std::cout << "Trie vs Array " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, 4, ds, percent, emails_shuffle, total_len_email);
        }*/
        int ds = 4;
        exec(expt_id, kEmail, 4, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
    }
    else if (expt_id == 3) {
        //-------------------------------------------------------------
        // Hu-Tucker Build Time; Expt ID = 3
        //-------------------------------------------------------------
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Hu-Tucker Build Time; Expt ID = 3" << std::endl;
        std::cout << "------------------------------------------------" << std::endl;

        int sample_percent = 1;
        int enc_src_percent = 100;
        int encoder_type = 4;
        int expt_num = 0;
        int total_num_expts = 9;
        for (int ds = 0; ds < 9; ds++) {
            std::cout << "Hu-Tucker Build Time " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
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

        int sample_percent = 1;
        int enc_src_percent = 100;
        int encoder_type = 4;
        int expt_num = 0;
        int total_num_expts = 9;
        for (int ds = 0; ds < 9; ds++) {
            //int dict_size_limit = dict_size_list[ds];
            std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 1: Hu-Tucker) "
                  << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
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

        int sample_percent = 1;
        int enc_src_percent = 100;
        int encoder_type = 4;
        int expt_num = 0;
        int total_num_expts = 9;
        for (int ds = 0; ds < 9; ds++) {
            //int dict_size_limit = dict_size_list[ds];
            std::cout << "Hu-Tucker vs. Fixed Length Dict Codes (Part 2: Dict Codes) "
                  << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
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

        int sample_percent = 1;
        int enc_src_percent = 100;
        int ds = 6;
        int expt_num = 0;
        int total_num_expts = 6;
        for (int encoder_type = 5; encoder_type < 7; encoder_type++) {
            std::cout << "Build Time Breakdown " << (expt_num++) << "/" << total_num_expts << std::endl;
            exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
        }
    }
    else if (expt_id == 7) {
        //-------------------------------------------------------------
        // Batch Encoder vs Non-batch
        //-------------------------------------------------------------
        std::cout << "------------------------------------------------" << std::endl;
        std::cout << "Batch Encoder vs Non-batch; Expt ID = 7" << std::endl;
        std::cout << "------------------------------------------------" <<std::endl;
        output_batch_lat.open(file_batch_lat);
        int ds = 6;
        int sample_percent = 1;
        int enc_src_percent = 100;
        int batch_sizes[10] = {1, 2, 4, 8, 16, 32, 64};
        for (int bs = 0; bs < 7; bs++) {
            int batch_size = batch_sizes[bs];
            for (int encoder_type = 1; encoder_type < 5; encoder_type++) {
                std::cout << "-------Batch size--------" << batch_size << "-----Encoder Type----" << encoder_type << std::endl;
                exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email, 2, batch_size);
            }
        }
        output_batch_lat.close();
    } else if (expt_id == 8) {
        //---------------------------------------------------------------
        // Compression Rate vs Insert percentage
        //---------------------------------------------------------------
        output_per_cpr_lat.open(file_per_email);
        int enc_src_percents[10] = { 10, 20, 30, 40, 50, 60, 70, 80, 90, 100};
        int sample_percent = 1;
        int ds = 6;
        for (int encoder_type = 1; encoder_type < 5; encoder_type++) {
            std::cout << "Encode Method " << encoder_type << std::endl;
            for (int i = 0; i < 10; i++) {
                int enc_src_percent = enc_src_percents[i];
                std::cout << "----------Insert percentage " << enc_src_percent << "%------------" << std::endl;
                exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails_shuffle, total_len_email);
            }
        }
        output_per_cpr_lat.close();
    } else if (expt_id == 9) {
        int encoder_type = 2;
        int ds = 6;
        int sample_percent = 1;
        int enc_src_percent = 100;
        int W = 0;
        int input_dict_size = 0;
        int encode_method = 0;
        int batch_size = 0;
        double bt = 0;
        double tput = 0;
        double lat = 0;
        double cpr = 0;
        double mem = 0;
        double dict_size = 0;
        std::cout << "-----------------Dataset 1, Dictionary 1---------------" << std::endl;
        exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails1_shuffle, total_len_email1);
        std::cout << "-----------------Dataset 2, Dictionary 2---------------" << std::endl;
        exec(expt_id, kEmail, encoder_type, ds, sample_percent, enc_src_percent, emails2_shuffle, total_len_email2);
        std::cout << "----------------Dataset 1, Dictionary 2----------------" << std::endl;
        std::vector<std::string> sample1_keys;
        std::vector<std::string> sample2_keys;
        int64_t sample_src_len1 = 0;
        int64_t sample_src_len2 = 0;
        getSampleKeys(sample_percent, sample1_keys, emails1_shuffle, sample_src_len1);
        getSampleKeys(sample_percent, sample2_keys, emails2_shuffle, sample_src_len2);
        exec_helper(encoder_type, W, input_dict_size,
                    sample2_keys, emails1_shuffle,
                    total_len_email1, encode_method,
                    batch_size, bt, tput, lat, cpr, dict_size, mem);
        std::cout << "---------------Dataset 2, Dictionary 1-----------------" << std::endl;
        exec_helper(encoder_type, W, input_dict_size,
                    sample1_keys, emails2_shuffle,
                    total_len_email2, encode_method,
                    batch_size, bt, tput, lat, cpr, dict_size, mem);
    }
    return 0;
}
