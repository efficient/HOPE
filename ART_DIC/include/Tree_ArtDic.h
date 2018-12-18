#ifndef OPE_TREE_H
#define OPE_TREE_H

#include <string>
#include <common.hpp>
#include "N_ArtDic.h"

namespace ARTDIC {

    struct LeafInfo {
        const ope::SymbolCode *symbol_code;
        LeafInfo *prev_leaf;
        uint32_t prefix_len;
    };

    class Tree {
    public:
        Tree();

        ~Tree();

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
}

#endif //OPE_TREE_H
