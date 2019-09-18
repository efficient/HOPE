#ifndef RANKBITVECTOR_H_
#define RANKBITVECTOR_H_

#include <assert.h>

#include <vector>

#include "popcount.h"

namespace ope {

class RankBitvector {
 public:
  RankBitvector() : num_bits_(0), bits_(nullptr), rank_lut_(nullptr){};

  RankBitvector(const int num_bits) {
    num_bits_ = num_bits;
    bits_ = new uint64_t[numWords()];
    memset(bits_, 0, bitsSize());
    int num_blocks = num_bits_ / 512 + 1;
    rank_lut_ = new int[num_blocks];
  }

  ~RankBitvector() {
    delete[] bits_;
    delete[] rank_lut_;
  }

  int numBits() const { return num_bits_; }

  int numWords() const {
    if (num_bits_ % 64 == 0)
      return (num_bits_ / 64);
    else
      return (num_bits_ / 64 + 1);
  }

  // in bytes
  int bitsSize() const { return (numWords() * 8); }

  int rankLutSize() const { return ((num_bits_ / 512 + 1) * sizeof(int)); }

  // in bytes
  int size() const { return (sizeof(RankBitvector) + bitsSize() + rankLutSize()); }

  bool readBit(const int pos) const;
  void setBit(const int pos);
  void initRankLut();
  int rank(const int pos) const;

 private:
  int num_bits_;
  uint64_t *bits_;
  int *rank_lut_;
};

bool RankBitvector::readBit(const int pos) const {
  assert(pos < num_bits_);
  int word_id = pos >> 6;
  int offset = pos & 63;
  return bits_[word_id] & (0x8000000000000000 >> offset);
}

void RankBitvector::setBit(const int pos) {
  assert(pos < num_bits_);
  int word_id = pos >> 6;
  int offset = pos & 63;
  bits_[word_id] |= (0x8000000000000000 >> offset);
}

void RankBitvector::initRankLut() {
  int num_blocks = num_bits_ / 512 + 1;
  int cumu_rank = 0;
  for (int i = 0; i < num_blocks - 1; i++) {
    rank_lut_[i] = cumu_rank;
    cumu_rank += surf::popcountLinear(bits_, i * 8, 512);
  }
  rank_lut_[num_blocks - 1] = cumu_rank;
}

int RankBitvector::rank(const int pos) const {
  assert(pos < num_bits_);
  int block_id = pos / 512;
  int offset = pos & 511;
  return (rank_lut_[block_id] + surf::popcountLinear(bits_, block_id * 8, offset + 1));
}

}  // namespace ope

#endif  // RANKBITVECTOR_H_
