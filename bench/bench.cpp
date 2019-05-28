#include <sys/time.h>
#include <time.h>

#include <algorithm>
#include <fstream>
#include <iostream>

#include "encoder_factory.hpp"

static const std::string file_email = "../../datasets/emails.txt";
static const std::string file_wiki = "../../datasets/wikis.txt";
static const std::string file_url = "../../datasets/urls.txt";

static const int kLongestCodeLen = 4096;

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int64_t loadKeys(const std::string& file_name,
		 std::vector<std::string> &keys,
		 std::vector<std::string> &keys_shuffle) {
    int cnt = 0;
    std::ifstream infile(file_name);
    std::string key;
    int64_t total_len = 0;
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

void exec(const int encoder_type, const int64_t dict_size_limit,
	  const std::vector<std::string>& keys,
	  const std::vector<std::string>& keys_shuffle,
	  const int64_t total_len) {
    ope::Encoder* encoder = ope::EncoderFactory::createEncoder(encoder_type);
    encoder->build(keys, dict_size_limit);

    uint8_t* buffer = new uint8_t[kLongestCodeLen];
    uint64_t total_enc_len = 0;
    double time_start = getNow();
    int64_t mem = encoder->memoryUse();
    for (int i = 0; i < (int)keys_shuffle.size(); i++) {
	total_enc_len += encoder->encode(keys_shuffle[i], buffer);
    }
    double time_end = getNow();
    double time_diff = time_end - time_start;
    double tput = keys_shuffle.size() / time_diff / 1000000; // in Mops/s
    double lat = time_diff * 1000000000 / total_len; // in ns
    double cpr = (total_len * 8.0) / total_enc_len;
    
    std::cout << "Throughput = " << tput << " Mops/s" << std::endl;
    std::cout << "Latency = " << lat << " ns/char" << std::endl;
    std::cout << "CPR = " << cpr << std::endl;
    std::cout << "Memory = " << mem << std::endl;
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
    //std::vector<std::string> emails;
    //std::vector<std::string> emails_shuffle;
    //std::vector<std::string> emails_insert;
    //int64_t total_len_email = loadKeys(file_email, emails, emails_shuffle);
    //int64_t total_len_insert = getPercentKey(emails_shuffle, emails_insert, 10);
    //exec(5, 65536, emails, emails_insert, total_len_insert);

    std::vector<std::string> wikis;
    std::vector<std::string> wikis_shuffle;
    loadKeys(file_wiki, wikis, wikis_shuffle);
    std::vector<std::string> wikis_insert;
    //exec(1, 1000, wikis, wikis_shuffle, total_len_wiki);
    int64_t total_len_insert = getPercentKey(wikis_shuffle, wikis_insert, 10);
    wikis_shuffle.clear();
    exec(5, 65536, wikis_insert, wikis_insert, total_len_insert);


    //std::vector<std::string> urls;
    //std::vector<std::string> urls_shuffle;
    //int64_t total_len_url = loadKeys(file_url, urls, urls_shuffle);
    /*
    exec(1, 1000, urls, urls_shuffle, total_len_url);

    exec(2, 65536, emails, emails_shuffle, total_len_email);
    exec(2, 65536, wikis, wikis_shuffle, total_len_wiki);
    exec(2, 65536, urls, urls_shuffle, total_len_url);

    exec(3, 65536, emails, emails_shuffle, total_len_email);
    exec(3, 65536, wikis, wikis_shuffle, total_len_wiki);
    exec(3, 65536, urls, urls_shuffle, total_len_url);

    exec(4, 65536, emails, emails_shuffle, total_len_email);
    exec(4, 65536, wikis, wikis_shuffle, total_len_wiki);
    exec(4, 65536, urls, urls_shuffle, total_len_url);
    */

    //exec(4, 65536, emails, emails_shuffle, total_len_email);
    //exec(5, 65536, urls, urls_shuffle, total_len_url);
    
    return 0;
}
