#ifndef ART_DIC_N_H
#define ART_DIC_N_H

#include <emmintrin.h>  // x86 SSE intrinsics
#include <stdio.h>
#include <algorithm>
#include <cstdint>

using namespace std;

namespace ope {

struct LeafInfo {
  const SymbolCode *symbol_code;
  LeafInfo *prev_leaf;
  uint32_t prefix_len;
  int visit_cnt = 0;
};

static const unsigned maxPrefixLen = 16;
static int cnt_N4 = 0;
static int cnt_N16 = 0;
static int cnt_N48 = 0;
static int cnt_N256 = 0;
static int64_t extra_size = 0;

enum class NTypes : uint8_t { N4 = 0, N16 = 1, N48 = 2, N256 = 3 };

class N {
 public:
  // node type
  NTypes type;

  // number of children
  uint8_t count = 0;

  // length of compressed path
  uint32_t prefix_len = 0;

  // compressed path
  uint8_t prefix[maxPrefixLen];

  // store prefix key, value pairs
  N *prefix_leaf = nullptr;

  uint8_t *long_prefix;

  N(NTypes _type, const uint8_t *_prefix, uint32_t _prefix_len) : type(_type), prefix_len(_prefix_len) {
    if (_prefix_len > maxPrefixLen) {
      long_prefix = new uint8_t[_prefix_len];
      extra_size += _prefix_len;
      for (int i = 0; i < (int)_prefix_len; i++) long_prefix[i] = _prefix[i];
    } else {
      for (int i = 0; i < (int)_prefix_len; i++) prefix[i] = _prefix[i];
    }
  };

  void setPrefix(const uint8_t *prefix, int length);

  uint8_t *getPrefix();

  N *duplicate();

  void setPrefixLeaf(N *leaf);

  N *getPrefixLeaf();

  void deletePrefixLeaf();

  bool insert(uint8_t key, N *node);

  bool remove(uint8_t key);

  template <class curN, class biggerN>
  static void insertGrow(curN *n, uint8_t k, N *node, uint8_t key_par, N *parent);

  template <class curN, class smallerN>
  static void removeAndShrink(curN *n, uint8_t key, uint8_t key_par, N *parent);

  // API for tree classes
  static void insertOrUpdateNode(N *node, N *parentNode, uint8_t keyParent, uint8_t key, N *val);

  static void removeNode(N *node, N *parentNode, uint8_t keyParent, uint8_t key);

  static void change(N *node, uint8_t key, N *val);

  static N *setLeaf(N *node);

  static N *getValueFromLeaf(N *leaf);

  static bool isLeaf(N *node);

  static N *getChild(uint8_t key, N *node);

  static void getChildren(N *node, uint8_t start, uint8_t end, uint8_t *children_key, N **children_p, int &cnt);

  static N *getFirstChild(N *node);

  static N *getLastChild(N *node);

  // [k, 255)
  static N *getNextChild(N *node, uint8_t k);

  //[0,k)
  static N *getPrevChild(N *node, uint8_t k);

  static void deleteChildren(N *node);

  static void deleteNode(N *node);

  static void checkNode(N *node);
};

class N4 : public N {
 public:
  uint8_t keys[4];
  N *children[4] = {nullptr};

  N4(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N4, prefix, prefix_len) { cnt_N4++; };

  ~N4() { cnt_N4--; }

  bool insert(uint8_t key, N *node);

  bool remove(uint8_t key);

  void change(uint8_t key, N *val);

  N *getChild(uint8_t key);

  N *getFirstChild();

  N *getLastChild();

  N *getNextChild(uint8_t k);

  N *getPrevChild(uint8_t k);

  void deleteChildren();

  template <class NODE>
  void copyTo(NODE *n) const;

  void checkNode();
};

class N16 : public N {
 public:
  uint8_t keys[16];
  N *children[16] = {nullptr};

  N16(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N16, prefix, prefix_len) { cnt_N16++; };

  ~N16() { cnt_N16--; }

  static uint8_t flipSign(uint8_t keyByte) {
    // Flip the sign bit, enables signed SSE comparison of unsigned values, used by Node16
    return keyByte ^ 128;
  };

  bool insert(uint8_t key, N *node);

  bool remove(uint8_t key);

  void change(uint8_t key, N *val);

  N *getChild(uint8_t key);

  N *getFirstChild();

  N *getLastChild();

  N *getNextChild(uint8_t k);

  N *getPrevChild(uint8_t k);

  void deleteChildren();

  template <class NODE>
  void copyTo(NODE *n) const;

  void checkNode();
};

class N48 : public N {
 public:
  const uint8_t empty_marker = 48;
  uint8_t child_index[256];
  N *children[48] = {nullptr};

  N48(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N48, prefix, prefix_len) {
    std::fill_n(child_index, 256, empty_marker);
    cnt_N48++;
  };

  ~N48() { cnt_N48--; }

  bool insert(uint8_t key, N *n);

  bool remove(uint8_t key);

  void change(uint8_t key, N *val);

  N *getChild(uint8_t k) const;

  N *getFirstChild();

  N *getLastChild();

  N *getNextChild(uint8_t k);

  N *getPrevChild(uint8_t k);

  void deleteChildren();

  template <class NODE>
  void copyTo(NODE *n) const;

  void checkNode();
};

class N256 : public N {
 public:
  N *children[256] = {nullptr};

  N256(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N256, prefix, prefix_len) { cnt_N256++; };

  ~N256() { cnt_N256--; }

  bool insert(uint8_t key, N *n);

  bool remove(uint8_t key);

  void change(uint8_t key, N *val);

  N *getChild(uint8_t k) const;

  N *getFirstChild();

  N *getLastChild();

  N *getNextChild(uint8_t k);

  N *getPrevChild(uint8_t k);

  void deleteChildren();

  template <class NODE>
  void copyTo(NODE *n) const;

  void checkNode();
};

void N::setPrefix(const uint8_t *_prefix, int length) {
  if ((uint32_t)length > maxPrefixLen) {
    if (long_prefix != nullptr) {
      delete long_prefix;
      extra_size -= prefix_len;
    }
    long_prefix = new uint8_t[length];
    extra_size += length;
    for (int i = 0; i < length; i++) long_prefix[i] = _prefix[i];
  } else {
    for (int i = 0; i < length; i++) {
      prefix[i] = _prefix[i];
    }
  }
  prefix_len = length;
}

uint8_t *N::getPrefix() {
  if (prefix_len > maxPrefixLen) return long_prefix;
  return prefix;
}

void N::setPrefixLeaf(N *leaf) { prefix_leaf = leaf; }

void N::deletePrefixLeaf() { delete prefix_leaf; }

N *N::getPrefixLeaf() { return prefix_leaf; }

N *N::setLeaf(N *n) {
  // 7fff ffff  + 1
  // 8000 0000 | n
  return reinterpret_cast<N *>((static_cast<uint64_t>(1) << 63) | reinterpret_cast<uint64_t>(n));
}

bool N::isLeaf(N *n) {
  // use & 1 instead &ffffffffffffffff becuase of the difference
  // in treating >> in macos and linux
  return ((reinterpret_cast<uint64_t>(n) >> 63 & 1) == 1);
}

template <class curN, class biggerN>
void N::insertGrow(curN *n, uint8_t key, N *val, uint8_t key_par, N *node_par) {
  // current key has already existed
  if (n->getChild(key) != nullptr) {
    n->change(key, val);
    return;
  }
  // current node is not full
  if (n->insert(key, val)) return;
  // initialize a bigger node
  auto big_node = new biggerN(n->getPrefix(), n->prefix_len);
  // copy original keys and children
  n->copyTo(big_node);
  // insert key,val to the new node
  big_node->insert(key, val);
  // replace old node with new node
  N::change(node_par, key_par, big_node);
  // delete old node
  delete n;
}

template <class curN, class smallerN>
void N::removeAndShrink(curN *n, uint8_t key, uint8_t key_par, N *parent_node) {
  if (n->remove(key)) {
    return;
  }
  // initialize a smaller node
  auto nSmall = new smallerN(n->getPrefix(), n->prefix_len);
  // remove key
  n->remove(key);
  // copy to smaller node
  n->copyTo(nSmall);
  // replace old node with new node
  N::change(parent_node, key_par, nSmall);
  delete n;
}

void N::change(N *node, uint8_t key, N *val) {
  switch (node->type) {
    case NTypes::N4: {
      N4 *n = static_cast<N4 *>(node);
      n->change(key, val);
      return;
    }
    case NTypes::N16: {
      N16 *n = static_cast<N16 *>(node);
      n->change(key, val);
      return;
    }
    case NTypes::N48: {
      N48 *n = static_cast<N48 *>(node);
      n->change(key, val);
      return;
    }
    case NTypes::N256: {
      N256 *n = static_cast<N256 *>(node);
      n->change(key, val);
      return;
    }
    default:
      break;
  }
}

void N::insertOrUpdateNode(N *node, N *parent_node, uint8_t parent_key, uint8_t key, N *val) {
  switch (node->type) {
    case NTypes::N4: {
      auto n = static_cast<N4 *>(node);
      insertGrow<N4, N16>(n, key, val, parent_key, parent_node);
      //		N::checkNode(N::getChild(parent_key, parent_node));
      return;
    }
    case NTypes::N16: {
      auto n = static_cast<N16 *>(node);
      insertGrow<N16, N48>(n, key, val, parent_key, parent_node);
      //		N::checkNode(N::getChild(parent_key, parent_node));
      return;
    }
    case NTypes::N48: {
      auto n = static_cast<N48 *>(node);
      insertGrow<N48, N256>(n, key, val, parent_key, parent_node);
      //		N::checkNode(N::getChild(parent_key, parent_node));
      return;
    }
    case NTypes::N256: {
      auto n = static_cast<N256 *>(node);
      if (n->getChild(key) != nullptr)
        n->change(key, val);
      else
        n->insert(key, val);
      return;
    }
  }
}

void N::removeNode(N *node, N *parent_node, uint8_t key_par, uint8_t key) {
  switch (node->type) {
    case NTypes::N4: {
      auto n = static_cast<N4 *>(node);
      removeAndShrink<N4, N4>(n, key, key_par, parent_node);
      return;
    }
    case NTypes::N16: {
      auto n = static_cast<N16 *>(node);
      removeAndShrink<N16, N4>(n, key, key_par, parent_node);
      return;
    }
    case NTypes::N48: {
      auto n = static_cast<N48 *>(node);
      removeAndShrink<N48, N16>(n, key, key_par, parent_node);
      return;
    }
    case NTypes::N256: {
      auto n = static_cast<N256 *>(node);
      removeAndShrink<N256, N48>(n, key, key_par, parent_node);
      return;
    }
  }
}

N *N::duplicate() {
  switch (this->type) {
    case NTypes::N4: {
      N4 *new_node = new N4(prefix, prefix_len);
      N4 *node = reinterpret_cast<N4 *>(this);
      node->copyTo(new_node);
      return new_node;
    }
    case NTypes::N16: {
      N16 *new_node = new N16(prefix, prefix_len);
      N16 *node = reinterpret_cast<N16 *>(this);
      node->copyTo(new_node);
      return new_node;
    }
    case NTypes::N48: {
      N48 *new_node = new N48(prefix, prefix_len);
      N48 *node = reinterpret_cast<N48 *>(this);
      node->copyTo(new_node);
      return new_node;
    }
    case NTypes::N256: {
      N256 *new_node = new N256(prefix, prefix_len);
      N256 *node = reinterpret_cast<N256 *>(this);
      node->copyTo(new_node);
      return new_node;
    }
  }
  return nullptr;
}

bool N::insert(uint8_t key, N *node) {
  switch (this->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(this)->insert(key, node);
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(this)->insert(key, node);
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(this)->insert(key, node);
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(this)->insert(key, node);
    }
  }
  return false;
}

N *N::getChild(uint8_t key, N *node) {
  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->getChild(key);
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->getChild(key);
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->getChild(key);
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->getChild(key);
    }
  }
  return nullptr;
}

bool N::remove(uint8_t key) {
  switch (this->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(this)->remove(key);
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(this)->remove(key);
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(this)->remove(key);
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(this)->remove(key);
    }
  }
  return false;
}

void N::getChildren(N *node, uint8_t start, uint8_t end, uint8_t *children_key, N **children_p, int &child_cnt) {
  child_cnt = 0;
  for (uint8_t cur = start; cur < end; cur++) {
    N *child = getChild(cur, node);
    if (child == nullptr) continue;
    children_key[child_cnt] = cur;
    children_p[child_cnt] = child;
    child_cnt++;
  }
  if (start == 0 && end == 255 && child_cnt != node->count) {
    cout << "Child Count mismatch" << endl;
    cout << "Child count:" << unsigned(child_cnt) << endl;
    cout << "Record count:" << unsigned(node->count) << endl;
    assert(false);
  }
}

N *N::getValueFromLeaf(N *leaf) { return reinterpret_cast<N *>(reinterpret_cast<uint64_t>(leaf) & 0x7fffffffffffffff); }

N *N::getLastChild(N *node) {
  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->getLastChild();
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->getLastChild();
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->getLastChild();
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->getLastChild();
    }
  }
  __builtin_unreachable();
}

N *N::getFirstChild(N *node) {
  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->getFirstChild();
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->getFirstChild();
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->getFirstChild();
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->getFirstChild();
    }
  }
  __builtin_unreachable();
}

N *N::getNextChild(N *node, uint8_t k) {
  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->getNextChild(k);
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->getNextChild(k);
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->getNextChild(k);
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->getNextChild(k);
    }
  }
  __builtin_unreachable();
}

N *N::getPrevChild(N *node, uint8_t k) {
  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->getPrevChild(k);
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->getPrevChild(k);
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->getPrevChild(k);
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->getPrevChild(k);
    }
  }
  __builtin_unreachable();
}

void N::deleteChildren(N *node) {
  if (node == NULL) return;

  if (N::isLeaf(node)) {
    return;
  }

  if (node->prefix_leaf != nullptr) {
    delete N::getValueFromLeaf(node->prefix_leaf);
  }

  switch (node->type) {
    case NTypes::N4: {
      return reinterpret_cast<N4 *>(node)->deleteChildren();
    }
    case NTypes::N16: {
      return reinterpret_cast<N16 *>(node)->deleteChildren();
    }
    case NTypes::N48: {
      return reinterpret_cast<N48 *>(node)->deleteChildren();
    }
    case NTypes::N256: {
      return reinterpret_cast<N256 *>(node)->deleteChildren();
    }
  }
  __builtin_unreachable();
}

void N::deleteNode(N *node) {
  if (N::isLeaf(node)) {
    auto leaf = reinterpret_cast<LeafInfo *>(getValueFromLeaf(node));
    // Delete leaf object
    delete (leaf);
    return;
  }

  if (node->prefix_len > maxPrefixLen) {
    delete (node->long_prefix);
    extra_size -= node->prefix_len;
  }

  switch (node->type) {
    case NTypes::N4: {
      auto n = static_cast<N4 *>(node);
      delete n;
      return;
    }
    case NTypes::N16: {
      auto n = static_cast<N16 *>(node);
      delete n;
      return;
    }
    case NTypes::N48: {
      auto n = static_cast<N48 *>(node);
      delete n;
      return;
    }
    case NTypes::N256: {
      auto n = static_cast<N256 *>(node);
      delete n;
      return;
    }
  }
  assert(false);
}

void N::checkNode(ope::N *node) {
  switch (node->type) {
    case NTypes::N4: {
      reinterpret_cast<N4 *>(node)->checkNode();
    }
    case NTypes::N16: {
      reinterpret_cast<N16 *>(node)->checkNode();
    }
    case NTypes::N48: {
      reinterpret_cast<N48 *>(node)->checkNode();
    }
    case NTypes::N256: {
      reinterpret_cast<N256 *>(node)->checkNode();
    }
  }
}

bool N4::insert(uint8_t k, N *node) {
  if (count == 4) return false;
  int i = 0;
  while (i < count && k > keys[i]) i += 1;
  for (int j = count; j > i; j--) {
    keys[j] = keys[j - 1];
    children[j] = children[j - 1];
  }
  keys[i] = k;
  children[i] = node;
  count += 1;
  return true;
}

bool N4::remove(uint8_t k) {
  for (int i = 0; i < count; i++) {
    if (keys[i] == k) {
      for (int j = (int)i; j < count - 1; j++) {
        keys[j] = keys[j + 1];
        children[j] = children[j + 1];
      }
      count--;
      return true;
    }
  }
  __builtin_unreachable();
}

void N4::change(uint8_t key, N *val) {
  for (uint8_t i = 0; i < count; i++) {
    if (children[i] != nullptr && keys[i] == key) {
      children[i] = val;
      return;
    }
  }
}

N *N4::getChild(uint8_t k) {
  for (int i = 0; i < count; i++) {
    if (this->keys[i] == k) {
      return children[i];
    }
  }
  return nullptr;
}

N *N4::getFirstChild() {
  if (prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  return children[0];
}

N *N4::getLastChild() {
  // count does not include prefix_leaf
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  return children[count - 1];
}

N *N4::getNextChild(uint8_t k) {
  if (count == 0) return nullptr;
  for (int i = 0; i < count; i++) {
    if (keys[i] >= k) return children[i];
  }
  return nullptr;
}

N *N4::getPrevChild(uint8_t k) {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t i = k; i <= k; i--) {
    N *child = getChild(i);
    if (child != nullptr) return child;
  }
  return nullptr;
}

void N4::deleteChildren() {
  for (int i = 0; i < (int)count; i++) {
    N::deleteChildren(children[i]);
    N::deleteNode(children[i]);
  }
}

template <class NODE>
void N4::copyTo(NODE *n) const {
  if (prefix_leaf != nullptr) {
    n->setPrefixLeaf(prefix_leaf);
  }
  for (int i = 0; i < count; i++) {
    n->insert(keys[i], children[i]);
  }
}

void N4::checkNode() {
  for (int i = 0; i < (int)count - 1; i++) {
    assert(keys[i] != keys[i + 1]);
    assert(children[i] != nullptr);
  }
}

bool N16::insert(uint8_t k, N *node) {
  if (count == 16) return false;
  // TODO: use __mm__cmplt to speed up
  unsigned int i = 0;
  while (i < count && k > keys[i]) {
    i += 1;
  }

  //        uint8_t keyByteFlipped = flipSign(k);
  //        __m128i cmp = _mm_cmplt_epi8(_mm_set1_epi8(keyByteFlipped), _mm_loadu_si128(reinterpret_cast<__m128i
  //        *>(keys))); uint16_t bitfield = _mm_movemask_epi8(cmp) & (0xFFFF >> (16 - count)); unsigned i = bitfield ?
  //        __builtin_ctz(bitfield) : count;
  for (int j = count; j > (int)i; j--) {
    keys[j] = keys[j - 1];
    children[j] = children[j - 1];
  }
  keys[i] = k;
  children[i] = node;
  count += 1;
  return true;
}

N *N16::getChild(uint8_t k) {
  for (int i = 0; i < count; i++) {
    if (this->keys[i] == k) return children[i];
  }
  return nullptr;
}

bool N16::remove(uint8_t k) {
  // if we need shrink, return false
  if (count == 4) return false;

  for (int i = 0; i < count; i++) {
    if (keys[i] == k) {
      for (int j = int(i); j < count - 1; j--) {
        keys[j] = keys[j + 1];
        children[j] = children[j + 1];
      }
      count--;
      return true;
    }
  }
  cout << " [Error]Remove N16 element not exist" << endl;
  __builtin_unreachable();
}

void N16::change(uint8_t key, N *val) {
  for (int i = 0; i < count; i++) {
    if (keys[i] == key) {
      children[i] = val;
      return;
    }
  }
}

N *N16::getFirstChild() {
  if (prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  return getChild(keys[0]);
}

N *N16::getLastChild() {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  return getChild(keys[count - 1]);
}

N *N16::getNextChild(uint8_t k) {
  if (count == 0) return nullptr;
  for (int i = 0; i < count; i++) {
    if (keys[i] >= k) return children[i];
  }
  return nullptr;
}

N *N16::getPrevChild(uint8_t k) {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t i = k; i <= k; i--) {
    N *child = getChild(i);
    if (child != nullptr) return child;
  }
  if (prefix_leaf != nullptr) return prefix_leaf;
  return nullptr;
}

void N16::deleteChildren() {
  for (uint8_t i = 0; i < count; i++) {
    N::deleteChildren(children[i]);
    N::deleteNode(children[i]);
  }
}

template <class NODE>
void N16::copyTo(NODE *n) const {
  if (prefix_leaf != nullptr) {
    n->setPrefixLeaf(prefix_leaf);
  }
  for (int i = 0; i < count; i++) {
    n->insert(keys[i], children[i]);
  }
}

void N16::checkNode() {
  for (int i = 0; i < (int)count - 1; i++) {
    assert(keys[i] != keys[i + 1]);
    assert(children[i] != nullptr);
  }
}

bool N48::insert(uint8_t k, N *n) {
  if (count == 48) return false;
  unsigned int pos = 0;
  while (children[pos] != nullptr) {
    pos = (pos + 1) % 48;
  }
  children[pos] = n;
  child_index[k] = static_cast<uint8_t>(pos);
  count++;
  return true;
}

bool N48::remove(uint8_t k) {
  assert(child_index[k] != empty_marker);
  if (count == 16) return false;
  children[child_index[k]] = nullptr;
  child_index[k] = empty_marker;
  count--;
  assert(getChild(k) == nullptr);
  return true;
}

N *N48::getChild(uint8_t k) const {
  if (child_index[k] == empty_marker) return nullptr;
  return children[child_index[k]];
}

void N48::change(uint8_t key, N *val) { children[child_index[key]] = val; }

N *N48::getFirstChild() {
  if (prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (int k = 0; k < 256; k++) {
    N *child = getChild((uint8_t)k);
    if (child != nullptr) return child;
  }
  __builtin_unreachable();
}

N *N48::getLastChild() {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t k = 255; (int)k >= 0; k--) {
    N *child = getChild(k);
    if (child != nullptr) return child;
  }
  __builtin_unreachable();
}

N *N48::getNextChild(uint8_t k) {
  if (count == 0) return nullptr;
  for (int i = k; i < 256; i++) {
    N *child = getChild((uint8_t)i);
    if (child != nullptr) return child;
  }
  return nullptr;
}

N *N48::getPrevChild(uint8_t k) {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t i = k; i <= k; i--) {
    N *child = getChild((uint8_t)i);
    if (child != nullptr) return child;
  }
  return nullptr;
}

void N48::deleteChildren() {
  for (unsigned i : child_index) {
    if (i != empty_marker) {
      N::deleteChildren(children[i]);
      N::deleteNode(children[i]);
    }
  }
}

template <class NODE>
void N48::copyTo(NODE *n) const {
  if (prefix_leaf != nullptr) {
    n->setPrefixLeaf(prefix_leaf);
  }
  for (unsigned i = 0; i < 256; i++) {
    if (child_index[i] != empty_marker) {
      n->insert(i, children[child_index[i]]);
    }
  }
}

void N48::checkNode() {}

bool N256::insert(uint8_t k, N *n) {
  if (count == 256) {
    std::cout << "[Error]Node full" << std::endl;
    return false;
  }
  children[k] = n;
  count++;
  return true;
}

bool N256::remove(uint8_t k) {
  if (count == 48) return false;
  children[k] = nullptr;
  count--;
  return true;
}

N *N256::getChild(uint8_t k) const { return children[k]; }

void N256::change(uint8_t key, N *val) { children[key] = val; }

N *N256::getFirstChild() {
  if (prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) {
    return nullptr;
  }
  for (int k = 0; k < 256; k++) {
    N *child = getChild((uint8_t)k);
    if (child != nullptr) {
      return child;
    }
  }
  __builtin_unreachable();
}

N *N256::getLastChild() {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t k = 255; (int)k > 0; k--) {
    N *child = getChild(k);
    if (child != nullptr) return child;
  }
  __builtin_unreachable();
}

N *N256::getNextChild(uint8_t k) {
  if (count == 0) return nullptr;
  for (int i = k; i < 256; i++) {
    N *child = getChild((uint8_t)i);
    if (child != nullptr) return child;
  }
  return nullptr;
}

N *N256::getPrevChild(uint8_t k) {
  if (count == 0 && prefix_leaf != nullptr) return prefix_leaf;
  if (count == 0) return nullptr;
  for (uint8_t i = k; i <= k; i--) {
    N *child = getChild((uint8_t)i);
    if (child != nullptr) return child;
  }
  return nullptr;
}

void N256::deleteChildren() {
  for (int i = 0; i < 256; i++) {
    if (children[i] != nullptr) {
      N::deleteChildren(children[i]);
      N::deleteNode(children[i]);
    }
  }
}

template <class NODE>
void N256::copyTo(NODE *n) const {
  if (prefix_leaf != nullptr) {
    n->setPrefixLeaf(prefix_leaf);
  }
  for (unsigned i = 0; i < 256; i++) {
    if (children[i] != nullptr) n->insert(i, children[i]);
  }
}

void N256::checkNode() {}
}  // namespace ope

#endif
