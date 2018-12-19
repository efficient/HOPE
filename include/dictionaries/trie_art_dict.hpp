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
        ope::Tree* tree;
    };

    TrieArtDict::TrieArtDict() {
        tree = new ope::Tree();
    }

    TrieArtDict::~TrieArtDict() {
        delete tree;
    }

    bool TrieArtDict::build(const std::vector<ope::SymbolCode> &symbol_code_list) {
        return tree->build(symbol_code_list);
    }

    Code TrieArtDict::lookup(const char *symbol, const int symbol_len, int &prefix_len) const {
        return tree->lookup(symbol, symbol_len, prefix_len);
    }

    // TODO
    int TrieArtDict::numEntries() const {
        return 0;
    }

    // TODO
    int64_t TrieArtDict::memoryUse() const {
        return 0;
    }

} // namespace ope

#endif // TRIE_ART_DICT_H