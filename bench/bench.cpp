#include <sys/time.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <algorithm>

#include "encoder_factory.hpp"

static const int64_t MAX_THREE = 80000;
static const int64_t MAX_FOUR = 450000;
static const std::string file_email = "../../datasets/emails.txt";
static const std::string file_wiki = "../../datasets/wikis.txt";
static const std::string file_url = "../../datasets/urls.txt";
static const std::string file_ts = "../../datasets/poisson_timestamps.csv";

static const int kLongestCodeLen = 4096;

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int getByteLen(const int bitlen) {
    return ((bitlen + 7) & ~7) / 8;
}

int64_t loadKeys(const std::string& file_name,
         const int sample_percent,
         std::vector<std::string> &keys,
         std::vector<std::string> &keys_shuffle) {
    std::ifstream infile(file_name);
    int step_size = 100 / sample_percent;
    std::string key;
    int64_t total_len = 0;
    int count = 0;
    while (infile.good()) {
        infile >> key;
        keys.push_back(key);
        total_len += key.length();
        count++;
    }
    std::random_shuffle(keys.begin(), keys.end());
    for (int i = 0; i < (int)keys.size(); i += step_size) {
        keys_shuffle.push_back(keys[i]);
    }
    return total_len;
}

std::string uint64ToString(uint64_t key) {
    uint64_t endian_swapped_key = __builtin_bswap64(key);
    return std::string(reinterpret_cast<const char*>(&endian_swapped_key), 8);
}

int64_t loadKeysInt(const std::string& file_name,
            const int sample_percent,
            std::vector<std::string> &keys,
            std::vector<std::string> &keys_shuffle) {
    std::ifstream infile(file_name);
    int step_size = 100 / sample_percent;
    std::string key;
    int64_t total_len = 0;
    int count = 0;
    while (infile.good()) {
    uint64_t int_key;
    infile >> int_key;
    key = uint64ToString(int_key);
    if (count % step_size == 0) {
        keys.push_back(key);
        keys_shuffle.push_back(key);
        total_len += key.length();
    }
    count++;
    }
    std::random_shuffle(keys_shuffle.begin(), keys_shuffle.end());
    return total_len;
}

int64_t getInputSize(const int encoder_type,
    const std::vector<std::string>& keys_shuffle,
    const int64_t dict_size_limit, int64_t& cur_dict_size) {
    ope::Encoder* encoder = nullptr;
    int64_t input_size = 0;
    int64_t l = 0;
    int64_t r;
    if (encoder_type != 3 && encoder_type != 4)
        return dict_size_limit;
    if (encoder_type == 3)
        r = std::min(dict_size_limit * 2, MAX_THREE);
    else
        r = std::min(dict_size_limit * 3, MAX_FOUR);
    cur_dict_size = 0;
    while(abs(cur_dict_size - dict_size_limit) > (int)(0.01 * dict_size_limit) && l <= r) {
            if (encoder != nullptr)
                delete encoder;
            input_size = (l + r)/2;
            std::cout << cur_dict_size << " " << input_size << " " << l << " " << r << std::endl;
            encoder = ope::EncoderFactory::createEncoder(encoder_type);
            encoder->build(keys_shuffle, input_size);
            cur_dict_size = encoder->numEntries();
            if (cur_dict_size < dict_size_limit) {
                l = input_size + 1;
            } else {
                r = input_size - 1;
            }
        }
    if (abs(cur_dict_size - dict_size_limit) > (int)(0.01 * dict_size_limit))
        std::cout << "Fail to approach " << dict_size_limit << ", best approach " <<  cur_dict_size << ", input " << input_size << std::endl;
    if (encoder != nullptr)
        delete encoder;
    return input_size;
}

void findAllInputSize(const int encoder_type, const int wkld, const std::vector<std::string>& keys_shuffle) {
    int64_t dic[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};
    int64_t cur_dict_size = 0;
    int stop_id = 0;
    if (encoder_type == 3)
        stop_id = 7;
    else if (encoder_type == 4)
        stop_id = 9;
    if (encoder_type == 3 || encoder_type == 4) {
        for (int i = 8; i < stop_id; i++) {
            std::cout << encoder_type << "\t" << dic[i] << "\t" << getInputSize(encoder_type, keys_shuffle, 254547, cur_dict_size) << "\t" << cur_dict_size << std::endl;
        }
    }
    if (encoder_type == 5) {
        for (int i = 0; i < 9; i++) {
            ope::Encoder* encoder = ope::EncoderFactory::createEncoder(encoder_type, 200000);
            encoder->build(keys_shuffle, dic[i]);
        }
    }
}

void exec(const int encoder_type, const int64_t dict_size_limit,
      const std::vector<std::string>& keys,
      const std::vector<std::string>& keys_shuffle,
      const int64_t total_len) {

    int64_t cur_dict_size = 0;
    int64_t input_size = getInputSize(encoder_type, keys_shuffle, dict_size_limit, cur_dict_size);
    int W = 0;
    if (encoder_type == 5 || encoder_type == 6)
        W = 50000;
    ope::Encoder* encoder = ope::EncoderFactory::createEncoder(encoder_type, W);
    encoder->build(keys_shuffle, input_size);

    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    uint64_t total_enc_len = 0;
    double time_start = getNow();
    int64_t mem = encoder->memoryUse();
    for (int i = 0; i < (int)keys.size(); i++) {
        total_enc_len += encoder->encode(keys[i], buffer);
    }
    double time_end = getNow();
    double time_diff = time_end - time_start;
    double tput = keys_shuffle.size() / time_diff / 1000000; // in Mops/s
    double lat = time_diff * 1000000000 / total_len; // in ns
    double cpr = (total_len * 8.0) / total_enc_len;

    std::cout << "Dictionary size = " << encoder->numEntries() << std::endl;
    std::cout << "Input size = " << input_size << std::endl;
    std::cout << "Throughput = " << tput << " Mops/s" << std::endl;
    std::cout << "Latency = " << lat << " ns/char" << std::endl;
    std::cout << "CPR = " << cpr << std::endl;
    std::cout << "Memory = " << mem << std::endl;

#ifdef INCLUDE_DECODE
    int64_t total_key_len = 0;
    int64_t total_dec_len = 0;
    std::vector<std::string> enc_keys;
    for (int i = 0; i < (int)keys_shuffle.size(); i++) {
        total_key_len += (int)keys_shuffle[i].size();
        int len = encoder->encode(keys_shuffle[i], buffer);
        std::string enc_str = std::string((const char*)buffer, getByteLen(len));
        enc_keys.push_back(enc_str);
    }

    time_start = getNow();
    for (int i = 0; i < (int)enc_keys.size(); i++) {
        total_dec_len += encoder->decode(enc_keys[i], buffer);
    }
    time_end = getNow();
    time_diff = time_end - time_start;
    tput = keys_shuffle.size() / time_diff / 1000000; // in Mops/s
    lat = time_diff * 1000000000 / total_len; // in ns

    std::cout << "total_key_len = " << total_key_len << std::endl;
    std::cout << "total_dec_len = " << total_dec_len << std::endl;

    std::cout << "Decode Throughput = " << tput << " Mops/s" << std::endl;
    std::cout << "Decode Latency = " << lat << " ns/char" << std::endl;
#endif

    delete[] buffer;
    delete encoder;

}

int64_t getPercentKey(std::vector<std::string>& all_keys,
                   std::vector<std::string>& insert_keys,
                   double percent) {
    int64_t insert_len = 0;
    int step = int(100/percent);
    for (int i = 0; i < (int)all_keys.size(); i+= step) {
        insert_keys.push_back(all_keys[i]);
        insert_len += all_keys[i].length();
    }
    std::cout << "Get " << all_keys.size() << " " << insert_keys.size() << " keys" << std::endl;
    return insert_len;
}

int main(int argc, char *argv[]) {
    int wkld = atoi(argv[1]);
    int dict_type = atoi(argv[2]);

    int percent = 1;
    if (wkld == 0) {
        std::vector<std::string> emails;
        std::vector<std::string> emails_shuffle;
        int64_t total_len_email = loadKeys(file_email, percent, emails, emails_shuffle);

        if (dict_type == 1)
            exec(1, 1000, emails, emails_shuffle, total_len_email);
        else if (dict_type == 2)
            exec(2, 65536, emails, emails_shuffle, total_len_email);
        else if (dict_type == 3)
            exec(3, 8192, emails, emails_shuffle, total_len_email);
            //exec(3, 90000, emails, emails_shuffle, total_len_email);
        else if (dict_type == 4)
            exec(4, 65536, emails, emails_shuffle, total_len_email);
        else if (dict_type == 5)
            exec(5, 65536, emails, emails_shuffle, total_len_email);
        else if (dict_type == 6)
            exec(6, 65536, emails, emails_shuffle, total_len_email);
        else if (dict_type == 7) {
                findAllInputSize(3, wkld, emails_shuffle);
                std::cout << "------------------------------------" << std::endl;
                findAllInputSize(4, wkld, emails_shuffle);
        } else {
            std::cout << "------------------------------------" << std::endl;
            findAllInputSize(5, wkld, emails_shuffle);
        }
    } else if (wkld == 1) {
         std::vector<std::string> wikis;
         std::vector<std::string> wikis_shuffle;
         int64_t total_len_wiki = loadKeys(file_wiki, percent, wikis, wikis_shuffle);
         if (dict_type == 1)
             exec(1, 1000, wikis, wikis_shuffle, total_len_wiki);
         else if (dict_type == 2)
             exec(2, 65536, wikis, wikis_shuffle, total_len_wiki);
         else if (dict_type == 3)
             exec(3, 65536, wikis, wikis_shuffle, total_len_wiki);
         else if (dict_type == 4)
             exec(4, 65536, wikis, wikis_shuffle, total_len_wiki);
         else if (dict_type == 5)
             exec(5, 65536, wikis, wikis_shuffle, total_len_wiki);
         else if (dict_type == 6)
             exec(6, 65536, wikis, wikis_shuffle, total_len_wiki);
        else if (dict_type == 7) {
                 findAllInputSize(3, wkld, wikis_shuffle);
                 std::cout << "------------------------------------" << std::endl;
                 findAllInputSize(4, wkld, wikis_shuffle);
         } else {
             std::cout << "------------------------------------" << std::endl;
             findAllInputSize(5, wkld, wikis_shuffle);
         }
    } else if (wkld == 2) {
        std::vector<std::string> urls;
        std::vector<std::string> urls_shuffle;
        int64_t total_len_url = loadKeys(file_url, percent, urls, urls_shuffle);
        if (dict_type == 1)
            exec(1, 1000, urls, urls_shuffle, total_len_url);
        else if (dict_type == 2)
            exec(2, 65536, urls, urls_shuffle, total_len_url);
        else if (dict_type == 3)
            exec(3, 65536, urls, urls_shuffle, total_len_url);
        else if (dict_type == 4)
            exec(4, 65536, urls, urls_shuffle, total_len_url);
        else if (dict_type == 5)
            exec(5, 65536, urls, urls_shuffle, total_len_url);
        else if (dict_type == 6)
            exec(6, 65536, urls, urls_shuffle, total_len_url);
        else if (dict_type == 7)  {
                findAllInputSize(3, wkld, urls_shuffle);
                std::cout << "------------------------------------" << std::endl;
                findAllInputSize(4, wkld, urls_shuffle);
        } else {
            std::cout << "------------------------------------" << std::endl;
            findAllInputSize(5, wkld, urls_shuffle);
        }
    } else if (wkld == 3) {
        std::vector<std::string> tss;
        std::vector<std::string> tss_shuffle;
        int64_t total_len_ts = loadKeysInt(file_ts, percent, tss, tss_shuffle);
        if (dict_type == 1)
            exec(1, 1000, tss, tss_shuffle, total_len_ts);
        else if (dict_type == 2)
            exec(2, 65536, tss, tss_shuffle, total_len_ts);
        else if (dict_type == 3)
            exec(3, 8192, tss, tss_shuffle, total_len_ts);
        else if (dict_type == 4)
            exec(4, 65536, tss, tss_shuffle, total_len_ts);
        else if (dict_type == 5)
            exec(5, 65536, tss, tss_shuffle, total_len_ts);
        else
            exec(2, 65536, tss, tss_shuffle, total_len_ts);
    }

    // std::vector<std::string> emails;
    // std::vector<std::string> emails_shuffle;
    // int64_t total_len_email = loadKeys(file_email, emails, emails_shuffle);
    // exec(1, 1000, emails, emails_shuffle, total_len_email);

    // std::vector<std::string> wikis;
    // std::vector<std::string> wikis_shuffle;
    // int64_t total_len_wiki = loadKeys(file_wiki, wikis, wikis_shuffle);
    // exec(1, 1000, wikis, wikis_shuffle, total_len_wiki);

    // std::vector<std::string> urls;
    // std::vector<std::string> urls_shuffle;
    // int64_t total_len_url = loadKeys(file_url, urls, urls_shuffle);
    // exec(1, 1000, urls, urls_shuffle, total_len_url);

    // exec(2, 65536, emails, emails_shuffle, total_len_email);
    // exec(2, 65536, wikis, wikis_shuffle, total_len_wiki);
    // exec(2, 65536, urls, urls_shuffle, total_len_url);

    // exec(3, 65536, emails, emails_shuffle, total_len_email);
    // exec(3, 65536, wikis, wikis_shuffle, total_len_wiki);
    // exec(3, 65536, urls, urls_shuffle, total_len_url);

    // exec(4, 65536, emails, emails_shuffle, total_len_email);
    // exec(4, 65536, wikis, wikis_shuffle, total_len_wiki);
    // exec(4, 65536, urls, urls_shuffle, total_len_url);

    return 0;
}
