#ifndef SBT_H
#define SBT_H

#include <queue>
#include <string>
#include <vector>

#include "common.hpp"
#include "rank_bitvector.hpp"

namespace ope {

class Btnode {
 public:
  Btnode() : left(nullptr), right(nullptr), leaf_order(-1){};
  ~Btnode(){};

  Btnode *left;
  Btnode *right;
  int leaf_order;
};

class SBT {
 public:
  SBT() : num_nodes_(0), trie_(nullptr), leaf_orders_(nullptr){};
  SBT(const std::vector<Code> &keys);
  ~SBT() {
    delete trie_;
    delete[] leaf_orders_;
  }

  bool lookup(const std::string &in_key, int &key_bit_pos, int *out_idx);
  int memory();

 private:
  bool isLeaf(const int pos);
  void moveToLeftChild(int &pos);

  Btnode *buildBinaryTrie(const std::vector<Code> &keys);
  void deleteBinaryTrie(Btnode *root);

 private:
  int num_nodes_;
  RankBitvector *trie_;
  int *leaf_orders_;
};

SBT::SBT(const std::vector<Code> &keys) {
  Btnode *root = buildBinaryTrie(keys);

  trie_ = new RankBitvector(2 * num_nodes_ + 1);
  leaf_orders_ = new int[num_nodes_];

  int pos = 0;
  std::queue<Btnode *> node_q;
  Btnode *n = root;
  node_q.push(n);
  while (!node_q.empty()) {
    n = node_q.front();
    if (n->left) {
      trie_->setBit(pos);
      node_q.push(n->left);
    }
    if (n->right) {
      trie_->setBit(pos + 1);
      node_q.push(n->right);
    }
    leaf_orders_[pos / 2] = n->leaf_order;
    node_q.pop();
    pos += 2;
  }

  trie_->initRankLut();

  deleteBinaryTrie(root);
}

Btnode *SBT::buildBinaryTrie(const std::vector<Code> &keys) {
  num_nodes_ = 0;
  Btnode *root = new Btnode();
  num_nodes_++;
  for (int i = 0; i < (int)keys.size(); i++) {
    int32_t code = keys[i].code;
    int code_len = keys[i].len;
    code <<= (32 - code_len);
    Btnode *n = root;
    for (int j = 0; j < code_len; j++) {
      if (code & 0x80000000) {
        if (n->right) {
          n = n->right;
        } else {
          n->right = new Btnode();
          num_nodes_++;
          n = n->right;
        }
      } else {
        if (n->left) {
          n = n->left;
        } else {
          n->left = new Btnode();
          num_nodes_++;
          n = n->left;
        }
      }
      code <<= 1;
    }
    n->leaf_order = i;
  }

  return root;
}

void SBT::deleteBinaryTrie(Btnode *root) {
  std::queue<Btnode *> node_q;
  Btnode *n = root;
  node_q.push(n);
  while (!node_q.empty()) {
    n = node_q.front();
    if (n->left) node_q.push(n->left);
    if (n->right) node_q.push(n->right);
    delete n;
    node_q.pop();
  }
}

bool SBT::lookup(const std::string &in_key, int &key_bit_pos, int *out_idx) {
  char cur_char = in_key[key_bit_pos >> 3];
  int offset = key_bit_pos & 7;
  int pos = 0;
  while (key_bit_pos < (int)in_key.size() * 8) {
    if (cur_char & (1 << (7 - offset))) pos += 1;
    moveToLeftChild(pos);
    if (pos >= trie_->numBits()) return false;
    if (isLeaf(pos)) {
      key_bit_pos++;
      *out_idx = leaf_orders_[pos / 2];
      return true;
    }

    key_bit_pos++;
    offset++;
    if (offset == 8) {
      cur_char = in_key[key_bit_pos >> 3];
      offset = 0;
    }
  }
  return false;
}

bool SBT::isLeaf(const int pos) { return !trie_->readBit(pos) && !trie_->readBit(pos + 1); }

void SBT::moveToLeftChild(int &pos) {
  pos = trie_->rank(pos);
  pos <<= 1;
}

int SBT::memory() { return (sizeof(SBT) + trie_->size() + num_nodes_ * sizeof(int)); }

}  // namespace ope

#endif  // SBT_H
