#include "encoder_factory.hpp"

int main() {
    static const int kEncoderType = 3; // 3-grams Encoder
    static const char kKeyFilePath[] = "../datasets/words.txt";
    static const int kNumKeys = 234369;

    // load input key list
    // this input key list is already sorted
    std::vector<std::string> keys;
    int64_t total_key_len = 0;
    std::ifstream infile(kKeyFilePath);
    std::string key;
    int count = 0;
    while (infile.good() && count < kNumKeys) {
	infile >> key;
	keys.push_back(key);
	total_key_len += (key.length() * 8);
	count++;
    }

    // sample x% of the keys for building HOPE
    int skip = 100; // 1%
    // int skip = 10; // 10%
    // int skip = 1; // 100%
    std::vector<std::string> key_samples;
    for (int i = skip / 2; i < (int)keys.size(); i += skip) {
	key_samples.push_back(keys[i]);
    }

    // build the encoder
    hope::Encoder *encoder = hope::EncoderFactory::createEncoder(kEncoderType);
    encoder->build(key_samples, 5000); // 2nd para is dictionary size limit

    // compress all keys
    std::vector<std::string> enc_keys;
    int64_t total_enc_len = 0;
    uint8_t *buffer = new uint8_t[1024];
    for (int i = 0; i < (int)keys.size(); i++) {
	int bit_len = encoder->encode(keys[i], buffer);
	total_enc_len += bit_len;
	enc_keys.push_back(std::string((const char *)buffer, (bit_len + 7) / 8));
    }

    double cpr_rate =  total_key_len / (total_enc_len + 0.0);
    std::cout << "Compression Rate = " << cpr_rate << std::endl;

    // verify the order-preserving property of HOPE
    for (int i = 0; i < (int)keys.size() - 1; i++) {
	int cmp = enc_keys[i].compare(enc_keys[i + 1]);
	if (cmp >= 0) {
	    std::cout << "Order-Preserving property violated!" << std::endl;
	    return -1;
	}
    }
    
    return 0;
}
