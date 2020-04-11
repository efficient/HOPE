#ifndef DICTIONARY_FACTORY_H
#define DICTIONARY_FACTORY_H

#include "array_3gram_dict.hpp"
#include "array_4gram_dict.hpp"
#include "dictionary.hpp"
#include "trie_3gram_dict.hpp"
#include "trie_4gram_dict.hpp"
#include "trie_art_dict.hpp"

namespace hope {

class DictionaryFactory {
 public:
  static Dictionary *createDictionary(const int type) {
    if (type == 3)
#ifdef USE_ARRAY_DICT
      return new Array3GramDict();
#else
      return new Trie3GramDict();
#endif
    else if (type == 4)
#ifdef USE_ARRAY_DICT
      return new Array4GramDict();
#else
      return new Trie4GramDict();
#endif
    else if (type == 5)
      return new TrieArtDict();
    else
      return nullptr;
  }
};

}  // namespace hope

#endif  // DICTIONARY_FACTORY_H
