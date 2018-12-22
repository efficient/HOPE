#ifndef OPE_TREE_H
#define OPE_TREE_H

#include <string>
#include <common.hpp>
#include "art_dic_N.hpp"

namespace ope {

    struct LeafInfo {
        const ope::SymbolCode *symbol_code;
        LeafInfo *prev_leaf;
        uint32_t prefix_len;
        int visit_cnt = 0;
    };

    class ArtDicTree {
    public:
        ArtDicTree();

        ~ArtDicTree();

        ope::Code lookup(const char *symbol, const int symbol_len, int &prefix_len) const;

        bool build(const std::vector<ope::SymbolCode> &symbol_code_list);

        void countFreq(std::vector<std::string> keys, std::vector<ope::SymbolFreq> *symbol_freq_list);

        //private:
        N *root;

        void insert(LeafInfo* leafinfo);

        bool prefixMatch(N *node, uint8_t *key, int key_size, int &key_level,
                         int &node_level, uint8_t *common_prefix) const;

        N *spawn(uint8_t *common_prefix, N *node,
                 std::string key, N *val,
                 int node_level, int key_level, uint8_t parent_key);

        void addLeaf(int insertkey_level, std::string& key,
                     N *node, N *val, N *parent_node, uint8_t parent_key);

        void changeStringToUint8(std::string org, uint8_t* des) {
            for (int i = 0; i < org.size(); i++)
                des[i] = reinterpret_cast<uint8_t&>(org[i]);
        }

        void skipIfEmpty(N *node_new, N *leaf_dup, uint8_t key);

        void subKey(int start, int end, uint8_t *subKey, uint8_t *org);

        int getCommonPrefixLen(std::string& str1, std::string& str2);

        LeafInfo* getLeftBottom(N *node)const;

        LeafInfo* getRightBottom(N *node)const;

    };


    ArtDicTree::ArtDicTree() : root(new N256(nullptr, 0)) {};

    ArtDicTree::~ArtDicTree() {
        N::deleteChildren(root);
        N::deleteNode(root);
    }

    ope::Code ArtDicTree::lookup(const char *symbol, const int symbol_len, int &prefix_len) const {
        N *node = nullptr;
        N *next_node = root;
        int key_level = 0;
        int node_level = 0;
        uint8_t common_prefix[maxPrefixLen];

        while (true) {
            node = next_node;
            if (prefixMatch(node, (uint8_t *) symbol, symbol_len,
                            key_level, node_level, common_prefix)) {
                if (key_level == symbol_len) {
                    LeafInfo* leaf_info = reinterpret_cast<LeafInfo *>(N::getValueFromLeaf(N::getChild(0, node)));
                    if (leaf_info != nullptr) {
                        prefix_len = leaf_info->prefix_len;
                        return leaf_info->symbol_code->second;
                    }
                    N* next = N::getFirstChild(node);
                    leaf_info = getLeftBottom(next)->prev_leaf;
                    prefix_len = leaf_info->prefix_len;
                    return leaf_info->symbol_code->second;
                }
                next_node = N::getChild(reinterpret_cast<const uint8_t &>(symbol[key_level]), node);
                if (next_node == nullptr) {
                    // Get previous child
                    N *prev = N::getPrevChild(node, reinterpret_cast<const uint8_t &>(symbol[key_level]));
                    if (prev == nullptr) {
                        N *next = N::getNextChild(node, reinterpret_cast<const uint8_t &>(symbol[key_level]));
                        LeafInfo *next_leaf_info = getLeftBottom(next);
                        LeafInfo *leaf_info = next_leaf_info->prev_leaf;
                        // result_leaf_info = leaf_info;
                        prefix_len = leaf_info->prefix_len;
                        return leaf_info->symbol_code->second;
                    } else {
                        LeafInfo *leaf_info = getRightBottom(prev);
                        prefix_len = leaf_info->prefix_len;
                        // result_leaf_info = leaf_info;
                        return leaf_info->symbol_code->second;
                    }
                }
                if (N::isLeaf(next_node)) {
                    // result_leaf_info = reinterpret_cast<LeafInfo *>(N::getValueFromLeaf(next_node));
                    // return result_leaf_info->symbol_code->second;
                    LeafInfo *leaf_info = reinterpret_cast<LeafInfo *>(N::getValueFromLeaf(next_node));
                    prefix_len = leaf_info->prefix_len;
                    return leaf_info->symbol_code->second;
                }
            } else {
                LeafInfo *leaf_info = nullptr;
                if (key_level == symbol_len || symbol[key_level] < node->prefix[node_level]) {
                    N *prev = N::getFirstChild(node);
                    leaf_info = getLeftBottom(prev)->prev_leaf;
                } else {
                    N *next = N::getLastChild(node);
                    leaf_info = getRightBottom(next);
                }
                prefix_len = leaf_info->prefix_len;
                // result_leaf_info = leaf_info;
                return leaf_info->symbol_code->second;
            }
            key_level++;
        }
    }

    bool ArtDicTree::build(const std::vector<ope::SymbolCode> &symbol_code_list) {
        // REQUIRE: symbol has been sorted
        LeafInfo *prev_leaf = nullptr;

        for (auto iter = symbol_code_list.begin(); iter != symbol_code_list.end(); iter++) {
            LeafInfo *lf = new LeafInfo();
            std::string start_interval = iter->first;
            if (iter != symbol_code_list.end() - 1) {
                std::string end_interval = (iter + 1)->first;
                lf->prefix_len = (uint32_t) getCommonPrefixLen(start_interval, end_interval);
                assert(lf->prefix_len > 0);
            }
            else
                lf->prefix_len = 1;
            lf->prev_leaf = prev_leaf;
            lf->symbol_code = &(*iter);
            insert(lf);
            prev_leaf = lf;
        }
        return true;
    }

    void ArtDicTree::insert(LeafInfo *leafInfo) {
        const ope::SymbolCode *symbol_code = leafInfo->symbol_code;
        std::string key = symbol_code->first;
        N *node = nullptr;
        N *next_node = root;
        N *parent_node = nullptr;
        uint8_t parent_key = 0;
        uint8_t node_key = 0;
        int key_level = 0;
        int node_level = 0;

        while (true) {
            parent_node = node;
            parent_key = node_key;
            node = next_node;
            uint8_t common_prefix[maxPrefixLen];
            uint8_t uint8_key[maxPrefixLen];
            changeStringToUint8(key, uint8_key);
            N *val = N::setLeaf(reinterpret_cast<N *>(reinterpret_cast<int64_t >(leafInfo)));
            if (prefixMatch(node, uint8_key, (uint32_t)key.size(), key_level, node_level, common_prefix)) {
                // key == prefix
                if (node->prefix_len == key.size()) {
                    N::insertOrUpdateNode(node, parent_node, parent_key, 0, val);
                    return;
                }
                // size of remain key == 1
                if (key_level == key.size() - 1) {
                    N::insertOrUpdateNode(node, parent_node, parent_key,
                                          reinterpret_cast<uint8_t &>(key[key_level]), val);
                    return;
                }
                node_key = reinterpret_cast<uint8_t &>(key[key_level]);
                next_node = N::getChild(node_key, node);
                if (next_node == nullptr) {
                    addLeaf(key_level, key, node, val,
                            parent_node, parent_key);
                    return;
                } else if (N::isLeaf(next_node)) {
                    uint8_t prefix[maxPrefixLen];
                    N *new_node = new N4(prefix, 0);
                    new_node->insert(0, next_node);
                    if (key_level == key.size() - 2) {
                        new_node->insert(reinterpret_cast<uint8_t &>(key[key_level + 1]), val);
                    } else {
                        uint8_t leaf_prefix[maxPrefixLen];
                        changeStringToUint8(key.substr((uint32_t)(key_level + 2), key.size() - key_level - 2), leaf_prefix);
                        N *leaf = new N4(leaf_prefix, (uint32_t)(key.size() - key_level - 2));
                        leaf->insert(0, val);
                        new_node->insert(reinterpret_cast<uint8_t &>(key[key_level + 1]), leaf);
                    }
                    N::insertOrUpdateNode(node, parent_node, parent_key,
                                          reinterpret_cast<uint8_t &>(key[key_level]), new_node);
                    return;
                }
                key_level++;
            } else {
                auto new_node = spawn(common_prefix, node,
                                      key, val,
                                      node_level, key_level, parent_key);
                N::change(parent_node, parent_key, new_node);
                return;
            }
        }
    }

    void ArtDicTree::countFreq(std::vector<std::string> keys, std::vector<ope::SymbolFreq> *symbol_freq_list) {
        for (auto iter = keys.begin(); iter != keys.end(); iter++) {
            LeafInfo *result_leaf_info = nullptr;
            int prefix_len = -1;
            lookup(iter->c_str(), iter->size(), prefix_len);
            assert(result_leaf_info != nullptr);
            result_leaf_info->visit_cnt++;

        }
    }

    void ArtDicTree::addLeaf(int insertkey_level, std::string& key,
                       N *node, N *val, N *parent_node, uint8_t parent_key) {
        if (insertkey_level == key.size() - 1) {
            N::insertOrUpdateNode(node, parent_node, parent_key,
                                  reinterpret_cast<uint8_t &>(key[insertkey_level]), val);
            return;
        }
        uint8_t prefix[maxPrefixLen];
        uint8_t uint8_key[maxPrefixLen];
        changeStringToUint8(key, uint8_key);
        subKey(insertkey_level + 1, (uint8_t)key.size(), prefix, uint8_key);

        N *leaf = new N4(prefix, (uint32_t )(key.size() - insertkey_level - 1));
        leaf->insert(0, N::setLeaf(val));
        N::insertOrUpdateNode(node, parent_node, parent_key,
                              reinterpret_cast<uint8_t &>(key[insertkey_level]), leaf);

    }

    N *ArtDicTree::spawn(uint8_t *common_prefix, N *node,
                   std::string key, N *val,
                   int node_level, int key_level, uint8_t parent_key) {
        auto node_new = new N4(common_prefix, node_level);
        addLeaf(key_level, key, node_new, val,
                node, parent_key);
        N *leaf_dup = node->duplicate();
        uint8_t leaf_key[maxPrefixLen];
        subKey(node_level + 1, node->prefix_len, leaf_key, node->prefix);
        leaf_dup->setPrefix(leaf_key, node->prefix_len - node_level - 1);
        leaf_dup->prefix_len = node->prefix_len - node_level - 1;
        node_new->insert(node->prefix[node_level], leaf_dup);
        skipIfEmpty(node_new, leaf_dup, node->prefix[node_level]);
        return node_new;
    }

    void ArtDicTree::skipIfEmpty(N *node_new, N *leaf_dup, uint8_t key) {
        // TODO: remove std
        uint8_t children_key[256];
        N *children_p[256];
        int child_cnt = 0;
        N::getChildren(leaf_dup, 0, 255, children_key, children_p, child_cnt);
        if (leaf_dup->prefix_len == 0 && leaf_dup->count == 1 && children_key[0] == 0) {
            N::insertOrUpdateNode(node_new, nullptr, 0,
                                  key, children_p[0]);
            delete leaf_dup;
        }
    }

    bool ArtDicTree::prefixMatch(N *node, uint8_t *key,
                           int key_size, int &key_level,
                           int &node_level, uint8_t *common_prefix) const {
        int i = 0;
        uint8_t *node_prefix = node->prefix;
        for (; i < node->prefix_len; i++) {
            if (key_level + i >= key_size || key[key_level + i] != node_prefix[i]) {
                node_level = i;
                key_level += i;
                return false;
            }
            common_prefix[i] = node_prefix[i];
        }
        node_level = i;
        key_level += node_level;
        return true;
    }

    void ArtDicTree::subKey(int start, int end, uint8_t *subKey, uint8_t *org) {
        for (int i = start; i < end; i++) {
            subKey[i - start] = org[i];
        }
    }

    LeafInfo *ArtDicTree::getLeftBottom(N *node) const {
        while (true) {
            if (N::isLeaf(node))
                return reinterpret_cast<LeafInfo *>(N::getValueFromLeaf(node));
            node = N::getFirstChild(node);
        }
    }

    LeafInfo *ArtDicTree::getRightBottom(N *node) const {
        while (true) {
            if (N::isLeaf(node))
                return reinterpret_cast<LeafInfo *>(N::getValueFromLeaf(node));
            node = N::getLastChild(node);
        }
    }

    int ArtDicTree::getCommonPrefixLen(std::string &str1, std::string &str2) {
        int min_len = (int) std::min(str1.size(), str2.size());
        int i = 0;
        for (; i < min_len; i++) {
            if (str1[i] != str2[i])
                return i;
        }
        return i;
    }

}

#endif //OPE_TREE_H