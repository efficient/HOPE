#ifndef DOUBLE_CHAR_ENCODER_H
#define DOUBLE_CHAR_ENCODER_H

// for memset
#include <stdio.h>
#include <string.h>

#include "code_generator_factory.hpp"
#include "encoder.hpp"
#include "sbt.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

class DoubleCharEncoder : public Encoder {
 public:
  static const int kCgType = 0;
  DoubleCharEncoder() = default;
  ~DoubleCharEncoder() = default;

  bool build(const std::vector<std::string> &key_list, const int64_t dict_size_limit);

  int encode(const std::string &key, uint8_t *buffer) const;

  void encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer, uint8_t *r_buffer,
                  int &l_enc_len, int &r_enc_len) const;

  int64_t encodeBatch(const std::vector<std::string> &org_keys, int start_id, int batch_size,
                      std::vector<std::string> &enc_keys);

  int decode(const std::string &enc_key, uint8_t *buffer) const;

  int numEntries() const;
  int64_t memoryUse() const;

 private:
  bool buildDict(const std::vector<SymbolCode> &symbol_code_list);

  Code dict_[65536];
  SBT *decode_dict_;
};

bool DoubleCharEncoder::build(const std::vector<std::string> &key_list, const int64_t dict_size_limit) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "-----------------------------Double Character "
               "Encoder------------------------------"
            << std::endl;
  double cur_time = getNow();
#endif
  std::vector<SymbolFreq> symbol_freq_list;
  SymbolSelector *symbol_selector = SymbolSelectorFactory::createSymbolSelector(2);
  symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "Symbol Select time = " << getNow() - cur_time << std::endl;
  cur_time = getNow();
#endif

  std::vector<SymbolCode> symbol_code_list;
  CodeGenerator *code_generator = CodeGeneratorFactory::createCodeGenerator(kCgType);
  code_generator->genCodes(symbol_freq_list, &symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "Code Assign(Hu-Tucker) time = " << getNow() - cur_time << std::endl;
  cur_time = getNow();
#endif
  bool br = buildDict(symbol_code_list);
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "Build Dictionary time = " << getNow() - cur_time << std::endl;
#endif
  return br;
}

int DoubleCharEncoder::encode(const std::string &key, uint8_t *buffer) const {
  int64_t *int_buf = (int64_t *)buffer;
  int idx = 0;
  int_buf[0] = 0;
  int int_buf_len = 0;
  int key_len = (int)key.length();
  for (int i = 0; i < key_len; i += 2) {
    unsigned s_idx = 256 * (uint8_t)key[i];
    if (i + 1 < key_len) s_idx += (uint8_t)key[i + 1];
    int64_t s_buf = dict_[s_idx].code;
    int s_len = dict_[s_idx].len;
    if (int_buf_len + s_len > 63) {
      int num_bits_left = 64 - int_buf_len;
      int_buf_len = s_len - num_bits_left;
      int_buf[idx] <<= num_bits_left;
      int_buf[idx] |= (s_buf >> int_buf_len);
      int_buf[idx] = __builtin_bswap64(int_buf[idx]);
      int_buf[idx + 1] = s_buf;
      idx++;
    } else {
      int_buf[idx] <<= s_len;
      int_buf[idx] |= s_buf;
      int_buf_len += s_len;
    }
  }
  int_buf[idx] <<= (64 - int_buf_len);
  int_buf[idx] = __builtin_bswap64(int_buf[idx]);
  return ((idx << 6) + int_buf_len);
}

void DoubleCharEncoder::encodePair(const std::string &l_key, const std::string &r_key, uint8_t *l_buffer,
                                   uint8_t *r_buffer, int &l_enc_len, int &r_enc_len) const {
  int64_t *int_buf_l = (int64_t *)l_buffer;
  int64_t *int_buf_r = (int64_t *)r_buffer;
  int idx_l = 0, idx_r = 0;
  int_buf_l[0] = 0;
  int int_buf_len_l = 0, int_buf_len_r = 0;
  int key_len_l = (int)l_key.length();
  int key_len_r = (int)r_key.length();
  bool found_mismatch = false;
  int r_start_pos = 0;
  for (int i = 0; i < key_len_l; i += 2) {
    unsigned s_idx = 256 * (uint8_t)l_key[i];
    if (i + 1 < key_len_l) s_idx += (uint8_t)l_key[i + 1];

    if (!found_mismatch) {
      unsigned s_idx_r = 256 * (uint8_t)r_key[i];
      if (i + 1 < key_len_r) s_idx_r += (uint8_t)r_key[i + 1];

      if (s_idx < s_idx_r) {
        r_start_pos = i;
        memcpy((void *)r_buffer, (const void *)l_buffer, 8 * (idx_l + 1));
        idx_r = idx_l;
        int_buf_len_r = int_buf_len_l;
        found_mismatch = true;
      }
    }
    int64_t s_buf = dict_[s_idx].code;
    int s_len = dict_[s_idx].len;
    if (int_buf_len_l + s_len > 63) {
      int num_bits_left = 64 - int_buf_len_l;
      int_buf_len_l = s_len - num_bits_left;
      int_buf_l[idx_l] <<= num_bits_left;
      int_buf_l[idx_l] |= (s_buf >> int_buf_len_l);
      int_buf_l[idx_l] = __builtin_bswap64(int_buf_l[idx_l]);
      int_buf_l[idx_l + 1] = s_buf;
      idx_l++;
    } else {
      int_buf_l[idx_l] <<= s_len;
      int_buf_l[idx_l] |= s_buf;
      int_buf_len_l += s_len;
    }
  }
  int_buf_l[idx_l] <<= (64 - int_buf_len_l);
  int_buf_l[idx_l] = __builtin_bswap64(int_buf_l[idx_l]);
  l_enc_len = (idx_l << 6) + int_buf_len_l;

  // continue encoding right key
  for (int i = r_start_pos; i < key_len_r; i += 2) {
    unsigned s_idx = 256 * (uint8_t)r_key[i];
    if (i + 1 < key_len_r) s_idx += (uint8_t)r_key[i + 1];

    int64_t s_buf = dict_[s_idx].code;
    int s_len = dict_[s_idx].len;
    if (int_buf_len_r + s_len > 63) {
      int num_bits_left = 64 - int_buf_len_r;
      int_buf_len_r = s_len - num_bits_left;
      int_buf_r[idx_r] <<= num_bits_left;
      int_buf_r[idx_r] |= (s_buf >> int_buf_len_r);
      int_buf_r[idx_r] = __builtin_bswap64(int_buf_r[idx_r]);
      int_buf_r[idx_r + 1] = s_buf;
      idx_r++;
    } else {
      int_buf_r[idx_r] <<= s_len;
      int_buf_r[idx_r] |= s_buf;
      int_buf_len_r += s_len;
    }
  }
  int_buf_r[idx_r] <<= (64 - int_buf_len_r);
  int_buf_r[idx_r] = __builtin_bswap64(int_buf_r[idx_r]);
  r_enc_len = (idx_r << 6) + int_buf_len_r;
}

int64_t DoubleCharEncoder::encodeBatch(const std::vector<std::string> &org_keys, int start_id, int batch_size,
                                       std::vector<std::string> &enc_keys) {
  int64_t batch_code_size = 0;
  int end_id = start_id + batch_size;

  // Get batch common prefix
  int cp_len = 0;
  std::string start_string = org_keys[start_id];
  int last_len = start_string.length();
  for (int i = start_id + 1; i < end_id; i++) {
    const auto &cur_key = org_keys[i];
    cp_len = 0;
    while ((cp_len < last_len) && *(int *)(cur_key.c_str() + cp_len) == *(int *)(start_string.c_str() + cp_len)) {
      cp_len += 4;
    }
    while (cp_len < last_len && cur_key[cp_len] == start_string[cp_len]) cp_len++;
    last_len = cp_len;
  }

  uint8_t buffer[8192];
  int64_t *int_buf = (int64_t *)buffer;
  int idx = 0;
  int int_buf_len = 0;
  // Encode common prefix
  int cp_pos = 0;
  int64_t s_buf = 0;
  int s_len = 0;
  int num_bits_left = 0;
  while (cp_pos + 2 <= cp_len) {
    unsigned s_idx = 256 * (uint8_t)start_string[cp_pos];
    s_idx += (uint8_t)start_string[cp_pos + 1];
    s_buf = dict_[s_idx].code;
    s_len = dict_[s_idx].len;
    if (int_buf_len + s_len > 63) {
      num_bits_left = 64 - int_buf_len;
      int_buf_len = s_len - num_bits_left;
      int_buf[idx] <<= num_bits_left;
      int_buf[idx] |= (s_buf >> int_buf_len);
      int_buf[idx] = __builtin_bswap64(int_buf[idx]);
      int_buf[idx + 1] = s_buf;
      idx++;
    } else {
      int_buf[idx] <<= s_len;
      int_buf[idx] |= s_buf;
      int_buf_len += s_len;
    }
    cp_pos += 2;
  }

  // Encode left part
  uint8_t key_buffer[8192];
  int64_t *int_key_buf = (int64_t *)key_buffer;
  for (int i = start_id; i < end_id; i++) {
    int int_key_len = int_buf_len;
    int key_idx = idx;
    memcpy(key_buffer, buffer, 8 * (idx + 1));
    const auto &cur_key = org_keys[i];
    int pos = cp_pos;
    while (pos < (int)cur_key.length()) {
      unsigned s_idx = 256 * (uint8_t)cur_key[pos] + (uint8_t)cur_key[pos + 1];
      int64_t s_buf = dict_[s_idx].code;
      int s_len = dict_[s_idx].len;
      if (int_key_len + s_len > 63) {
        int num_bits_left = 64 - int_key_len;
        int_key_len = s_len - num_bits_left;
        int_key_buf[key_idx] <<= num_bits_left;
        int_key_buf[key_idx] |= (s_buf >> int_key_len);
        int_key_buf[key_idx] = __builtin_bswap64(int_key_buf[key_idx]);
        int_key_buf[key_idx + 1] = s_buf;
        key_idx++;
      } else {
        int_key_buf[key_idx] <<= s_len;
        int_key_buf[key_idx] |= s_buf;
        int_key_len += s_len;
      }
      pos += 2;
    }
    int_key_buf[key_idx] <<= (64 - int_key_len);
    int_key_buf[key_idx] = __builtin_bswap64(int_key_buf[key_idx]);
    int64_t cur_size = (key_idx << 6) + int_key_len;
#ifndef BATCH_DRY_ENCODE
    int enc_len = (cur_size + 7) >> 3;
    enc_keys.push_back(std::string((const char *)key_buffer, enc_len));
#endif
    batch_code_size += cur_size;
  }
  return batch_code_size;
}

int DoubleCharEncoder::decode(const std::string &enc_key, uint8_t *buffer) const {
#ifdef INCLUDE_DECODE
  int buf_pos = 0;
  int key_bit_pos = 0;
  int idx = 0;
  while (key_bit_pos < (int)enc_key.size() * 8) {
    if (!decode_dict_->lookup(enc_key, key_bit_pos, &idx)) {
      if (key_bit_pos < (int)enc_key.size() * 8) {
        return 0;
      } else {
        if (buffer[buf_pos - 1] == 0) buf_pos--;
        return buf_pos;
      }
    }
    buffer[buf_pos] = (uint8_t)(unsigned)((idx >> 8) & 0xFF);
    buffer[buf_pos + 1] = (uint8_t)(unsigned)(idx & 0xFF);
    buf_pos += 2;
  }
  if (buffer[buf_pos - 1] == 0) buf_pos--;
  return buf_pos;
#else
  return 0;
#endif
}

int DoubleCharEncoder::numEntries() const { return 65536; }

int64_t DoubleCharEncoder::memoryUse() const {
#ifdef INCLUDE_DECODE
  return sizeof(Code) * 65536 + decode_dict_->memory();
#else
  return sizeof(Code) * 65536;
#endif
}

bool DoubleCharEncoder::buildDict(const std::vector<SymbolCode> &symbol_code_list) {
  if (symbol_code_list.size() < 65536) return false;
  for (int i = 0; i < 65536; i++) {
    dict_[i] = symbol_code_list[i].second;
  }

#ifdef INCLUDE_DECODE
  std::vector<Code> codes;
  for (int i = 0; i < 65536; i++) {
    codes.push_back(dict_[i]);
  }
  decode_dict_ = new SBT(codes);
#else
  decode_dict_ = nullptr;
#endif

  return true;
}

}  // namespace ope

#endif  // DOUBLE_CHAR_ENCODER_H
