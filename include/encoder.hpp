#ifndef ENCODER_H
#define ENCODER_H

#include <assert.h>
#include <string>
#include <vector>

namespace ope {

class Encoder {
 public:
  virtual ~Encoder(){};

  virtual bool build(const std::vector<std::string> &key_list,
		     const int64_t dict_size_limit) = 0;

  virtual int encode(const std::string &key, uint8_t *buffer) const = 0;

  // Encode a pair of keys at the same time
  virtual void encodePair(const std::string &l_key, const std::string &r_key,
			  uint8_t *l_buffer, uint8_t *r_buffer,
                          int &l_enc_len, int &r_enc_len) const = 0;

  // Encode a batch of keys
  // The algorithm is faster than encoding the keys individually
  // because the common prefixes of the keys are only encoded once
  virtual int64_t encodeBatch(const std::vector<std::string> &ori_keys,
			      int start_id, int batch_size,
                              std::vector<std::string> &enc_keys) = 0;

  virtual int decode(const std::string &enc_key, uint8_t *buffer) const = 0;

  virtual int numEntries() const = 0;

  virtual int64_t memoryUse() const = 0;
};

}  // namespace ope

#endif  // ENCODER_H
