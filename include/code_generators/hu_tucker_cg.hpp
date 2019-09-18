#ifndef HU_TUCKER_CG_H
#define HU_TUCKER_CG_H

#include <queue>

#include "code_generator.hpp"

namespace ope {

class HuTuckerCG : public CodeGenerator {
 public:
  class Node {
   public:
    Node() : left_idx(0), right_idx(0), left_child(nullptr), right_child(nullptr){};
    Node(int idx) : left_idx(idx), right_idx(idx), left_child(nullptr), right_child(nullptr){};
    Node(int l_idx, int r_idx, Node *l_child, Node *r_child)
        : left_idx(l_idx), right_idx(r_idx), left_child(l_child), right_child(r_child){};

    bool isLeaf() { return (left_child == nullptr); }

    int left_idx;
    int right_idx;
    Node *left_child;
    Node *right_child;
  };

  HuTuckerCG(){};
  ~HuTuckerCG();
  bool genCodes(const std::vector<SymbolFreq> &symbol_freq_list, std::vector<SymbolCode> *symbol_code_list);
  int getCodeLen() const;
  double getCompressionRate() const;

 private:
  void clear();
  void loadInput(const std::vector<SymbolFreq> &symbol_freq_list);
  void genOptimalCodeLen();

  void buildBinaryTree();
  void initLeafs();
  int getMaxCodeLen() const;
  void mergeNodes(const int idx1, const int idx2);

  Code lookup(const int idx) const;

  void destroyBinaryTree();

  std::vector<std::string> symbol_list_;
  std::vector<int64_t> freq_list_;
  std::vector<int> code_len_list_;
  std::vector<Node *> node_list_;
  Node *root_;
};

HuTuckerCG::~HuTuckerCG() { destroyBinaryTree(); }

bool HuTuckerCG::genCodes(const std::vector<SymbolFreq> &symbol_freq_list, std::vector<SymbolCode> *symbol_code_list) {
  clear();
  loadInput(symbol_freq_list);
  genOptimalCodeLen();
  buildBinaryTree();

  // int max_code_len = 0;
  for (int i = 0; i < (int)symbol_list_.size(); i++) {
    Code code = lookup(i);
    // if (code.len > max_code_len)
    // max_code_len = code.len;
    symbol_code_list->push_back(std::make_pair(symbol_list_[i], code));
  }
  // std::cout << "max code len = " << max_code_len << std::endl;
  return true;
}

int HuTuckerCG::getCodeLen() const { return -1; }

double HuTuckerCG::getCompressionRate() const {
  int64_t freq_sum = 0;
  for (int i = 0; i < (int)freq_list_.size(); i++) {
    freq_sum += freq_list_[i];
  }
  double len = 0, enc_len = 0;
  for (int i = 0; i < (int)freq_list_.size(); i++) {
    if (freq_list_[i] > 1) {
      double prob = (freq_list_[i] + 0.0) / freq_sum;
      len += (prob * symbol_list_[i].length() * 8.0);
      enc_len += (prob * code_len_list_[i]);
    }
  }
  double cpr = len / enc_len;
  return cpr;
}

void HuTuckerCG::clear() {
  symbol_list_.clear();
  freq_list_.clear();
  code_len_list_.clear();
}

void HuTuckerCG::loadInput(const std::vector<SymbolFreq> &symbol_freq_list) {
  for (int i = 0; i < (int)symbol_freq_list.size(); i++) {
    symbol_list_.push_back(symbol_freq_list[i].first);
    freq_list_.push_back(symbol_freq_list[i].second);
  }
}

void HuTuckerCG::genOptimalCodeLen() {
  int n = (int)freq_list_.size();
  int64_t maxp = 1;
  std::vector<int> L, s, d;
  std::vector<int64_t> P;
  for (int k = 0; k < n; k++) {
    L.push_back(0);
    P.push_back(freq_list_[k]);
    maxp += freq_list_[k];
  }

  for (int m = 0; m < n - 1; m++) {
    int i = 0, i1 = 0, i2 = 0;
    int64_t pmin = maxp;
    int sumL = -1;
    while (i < n - 1) {
      if (P[i] == 0) {
        i++;
        continue;
      }
      int j1 = i, j2 = -1;
      int64_t min1 = P[i], min2 = maxp;
      int minL1 = L[i], minL2 = -1;

      int j = 0;
      for (j = i + 1; j < n; j++) {
        if (P[j] == 0) continue;
        if (P[j] < min1 || (P[j] == min1 && L[j] < minL1)) {
          min2 = min1;
          j2 = j1;
          minL2 = minL1;
          min1 = P[j];
          j1 = j;
          minL1 = L[j];
        } else if (P[j] < min2 || (P[j] == min2 && L[j] < minL2)) {
          min2 = P[j];
          j2 = j;
          minL2 = L[j];
        }
        if (L[j] == 0) break;
      }

      int64_t pt = P[j1] + P[j2];
      int sumLt = L[j1] + L[j2];
      if (pt < pmin || (pt == pmin && sumLt < sumL)) {
        pmin = pt;
        sumL = sumLt;
        i1 = j1;
        i2 = j2;
      }
      i = j;
    }

    if (i1 > i2) {
      int tmp = i1;
      i1 = i2;
      i2 = tmp;
    }
    s.push_back(i1);
    d.push_back(i2);
    P[i1] = pmin;
    P[i2] = 0;
    L[i1] = sumL + 1;
  }

  L[s[n - 2]] = 0;
  for (int m = n - 2; m >= 0; m--) {
    L[s[m]] += 1;
    L[d[m]] = L[s[m]];
  }

  for (int k = 0; k < n; k++) {
    code_len_list_.push_back(L[k]);
  }
}

void HuTuckerCG::buildBinaryTree() {
  initLeafs();
  int max_code_len = getMaxCodeLen();
  std::vector<int> tmp_code_lens;
  for (int i = 0; i < (int)code_len_list_.size(); i++) {
    tmp_code_lens.push_back(code_len_list_[i]);
  }
  std::vector<int> node_idx_list;
  for (int len = max_code_len; len > 0; len--) {
    node_idx_list.clear();
    for (int i = 0; i < (int)tmp_code_lens.size(); i++) {
      if (tmp_code_lens[i] == len) node_idx_list.push_back(i);
    }
    for (int i = 0; i < (int)node_idx_list.size(); i += 2) {
      int idx1 = node_idx_list[i];
      int idx2 = node_idx_list[i + 1];
      mergeNodes(idx1, idx2);
      tmp_code_lens[idx1] = len - 1;
      tmp_code_lens[idx2] = 0;
    }
  }
  root_ = node_list_[0];
}

void HuTuckerCG::initLeafs() {
  for (int i = 0; i < (int)code_len_list_.size(); i++) {
    Node *n = new Node(i);
    node_list_.push_back(n);
  }
}

int HuTuckerCG::getMaxCodeLen() const {
  int max_len = 0;
  for (int i = 0; i < (int)code_len_list_.size(); i++) {
    if (code_len_list_[i] > max_len) max_len = code_len_list_[i];
  }
  return max_len;
}

void HuTuckerCG::mergeNodes(const int idx1, const int idx2) {
  assert(idx1 < idx2);
  Node *left_node = node_list_[idx1];
  Node *right_node = node_list_[idx2];
  Node *new_node = new Node(left_node->left_idx, right_node->right_idx, left_node, right_node);
  node_list_[idx1] = new_node;
  node_list_[idx2] = nullptr;
}

Code HuTuckerCG::lookup(const int idx) const {
  Code code = {0, 0};
  Node *n = root_;
  while (!n->isLeaf()) {
    code.code <<= 1;
    if (idx > n->left_child->right_idx) {
      code.code += 1;
      n = n->right_child;
    } else {
      n = n->left_child;
    }
    code.len++;
  }
  return code;
}

void HuTuckerCG::destroyBinaryTree() {
  std::queue<Node *> node_q;
  node_q.push(root_);
  while (!node_q.empty()) {
    Node *n = node_q.front();
    if (!n->isLeaf()) {
      node_q.push(n->left_child);
      node_q.push(n->right_child);
    }
    node_q.pop();
    delete n;
  }
}

}  // namespace ope

#endif  // HU_TUCKER_CG_H
