#ifndef TRIE_ART_DICT_H
#define TRIE_ART_DICT_H

#include "dictionary.hpp"
#include "art_dic_tree.hpp"

namespace ope {
    class TrieArtDict : public Dictionary {
    public:
        TrieArtDict();
        ~TrieArtDict();
        bool build (const std::vector<SymbolCode>& symbol_code_list);
        Code lookup (const char* symbol, const int symbol_len, int& prefix_len) const;
        int numEntries () const;
        int64_t memoryUse () const;

    private:
        int num_entries = 0;
        ArtDicTree* tree;
    };

    TrieArtDict::TrieArtDict() {
        tree = new ope::ArtDicTree();
    }

    TrieArtDict::~TrieArtDict() {
        delete tree;
    }

    bool TrieArtDict::build(const std::vector<SymbolCode> &symbol_code_list) {
        num_entries = int(symbol_code_list.size());
        bool result = tree->build(symbol_code_list);
        return result;
    }

    Code TrieArtDict::lookup(const char *symbol, const int symbol_len, int &prefix_len) const {
        return tree->lookup(symbol, symbol_len, prefix_len);
    }

    int TrieArtDict::numEntries() const {
        return num_entries;
    }

    int64_t TrieArtDict::memoryUse() const {
        return sizeof(N4) * tree->getN4Num() +
                sizeof(N16) * tree->getN16Num() +
                sizeof(N48) * tree->getN48Num() +
                sizeof(N256) * tree->getN256Num();
    }

} // namespace ope

#endif // TRIE_ART_DICT_H