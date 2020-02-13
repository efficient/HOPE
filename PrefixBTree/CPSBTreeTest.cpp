#include <sys/time.h>
#include <fstream>
#include <iostream>
#include <vector>
#include "include/BTreeOLC.h"
#include "include/CPSBTreeOLC_Op.h"
#include "include/tlx/btree_map.hpp"

static const std::string email_dir = "/Users/xiaoxuanliu/Documents/Research/OPE/datasets/emails.txt";

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

int64_t loadKeys(const std::string &file_name, std::vector<std::string> &keys, std::vector<std::string> &keys_shuffle,
                 int64_t &total_len) {
  std::ifstream infile(file_name);
  std::string key;
  total_len = 0;
  int cnt = 0;
  while (infile.good() && cnt < 1000) {
    infile >> key;
    cnt++;
    keys.push_back(key);
    keys_shuffle.push_back(key);
    total_len += key.length();
  }
  std::cout << "Num of keys:" << keys.size() << std::endl;
  std::random_shuffle(keys_shuffle.begin(), keys_shuffle.end());
  return total_len;
}

int main() {
  std::vector<std::string> emails;
  std::vector<std::string> emails_shuffle;
  int64_t total_key_len = 0;
  loadKeys(email_dir, emails, emails_shuffle, total_key_len);
  auto cpstree = new cpsbtreeolc::BTree();
  auto btree = new btreeolc::BTree<std::string, std::string>();
  typedef tlx::btree_map<std::string, std::string, std::less<std::string> > btree_type;
  btree_type *tlx_btree = new btree_type();

  int insert_cnt = 0;
  double cps_insert_start = getNow();
  for (auto &email : emails_shuffle) {
    insert_cnt++;
    cpsbtreeolc::Key key;
    key.setKeyStr(email.c_str(), email.length());
    cpstree->insert(key, email);
  }
  double cps_insert_end = getNow();
  double cps_tput = insert_cnt / (cps_insert_end - cps_insert_start) / 1000000;  // M items / s

  insert_cnt = 0;
  double btree_insert_start = getNow();
  //  for (const auto &email : emails_shuffle) {
  //    insert_cnt++;
  //    btree->insert(email, email);
  //  }
  double btree_insert_end = getNow();
  double btree_tput = insert_cnt / (btree_insert_end - btree_insert_start) / 1000000;  // M items / s

  insert_cnt = 0;
  double tlxbtree_insert_start = getNow();
  //  for (const auto &email : emails_shuffle) {
  //    insert_cnt++;
  //    tlx_btree->insert2(email, email);
  //  }
  double tlxbtree_insert_end = getNow();
  double tlxbtree_tput = insert_cnt / (tlxbtree_insert_end - tlxbtree_insert_start) / 1000000;  // M items / s

  int64_t cpstree_size = cpstree->getSize();
  int64_t btree_size = btree->getSize() + total_key_len;
  int64_t tlxbtree_size = 256 * tlx_btree->get_stats().nodes() + total_key_len;
  std::cout << total_key_len << "\nCompressed BTree = " << cpstree_size << std::endl
            << "Compressed BTree Insert Throughput = " << cps_tput << " M items/s " << std::endl
            << "Victor BTree=" << btree_size << std::endl
            << "Victor BTree Insert Throughput = " << btree_tput << " M items/s " << std::endl
            << "TLXBTree=" << tlxbtree_size << std::endl
            << "TLXBTree Insert Throughput = " << tlxbtree_tput << " M items/s " << std::endl;

  /*******************************************LOOKUP**********************************************************/
  int lookup_cnt = 0;
  double cps_lookup_start = getNow();
  for (const auto &email : emails) {
    lookup_cnt++;
    std::string cpsvalue;
    cpsbtreeolc::Key key;
    key.setKeyStr(email.c_str(), email.length());
    cpstree->lookup(key, cpsvalue);
    assert(cpsvalue.compare(email) == 0);
  }
  double cps_lookup_end = getNow();
  double cps_lookup_tput = lookup_cnt / (cps_lookup_end - cps_lookup_start) / 1000000;

  lookup_cnt = 0;
  double victor_lookup_start = getNow();
  //  for (const auto &email : emails) {
  //    lookup_cnt++;
  //    std::string btreevalue;
  //    btree->lookup(email, btreevalue);
  //    assert(btreevalue.compare(email) == 0);
  //  }
  double victor_lookup_end = getNow();
  double victor_lookup_tput = lookup_cnt / (victor_lookup_end - victor_lookup_start) / 1000000;

  lookup_cnt = 0;
  double tlx_lookup_start = getNow();
  //  for (const auto &email : emails) {
  //    lookup_cnt++;
  //    std::string tlxvalue;
  //    cpsbtreeolc::Key key;
  //    auto it = tlx_btree->find(email);
  //    assert((it->second).compare(email) == 0);
  //  }
  double tlx_lookup_end = getNow();
  double tlx_lookup_tput = lookup_cnt / (tlx_lookup_end - tlx_lookup_start) / 1000000;

  delete btree;
  delete cpstree;
  delete tlx_btree;
  std::cout << "Lookup Throughput" << std::endl;
  std::cout << "Compressed B tree=" << cps_lookup_tput << " M items/s" << std::endl;
  std::cout << "Victor B tree=" << victor_lookup_tput << " M items/s" << std::endl;
  std::cout << "TLX B tree=" << tlx_lookup_tput << " M items/s" << std::endl;
}
