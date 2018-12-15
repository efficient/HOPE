#ifndef DICTIONARY_FACTORY_H
#define DICTIONARY_FACTORY_H

#include "dictionary.hpp"
#include "array_3gram_dict.hpp"
#include "array_4gram_dict.hpp"
#include "trie_3gram_dict.hpp"
#include "trie_4gram_dict.hpp"

namespace ope {

class DictionaryFactory {
public:
    static Dictionary* createDictionary(const int type) {
        if (type == 3)
            //return new Array3GramDict();
	    return new Trie3GramDict();
	else if (type == 4)
	    //return new Array4GramDict();
	    return new Trie4GramDict();
	else
	    return nullptr;
    }
};

} // namespace ope

#endif // DICTIONARY_FACTORY_H
