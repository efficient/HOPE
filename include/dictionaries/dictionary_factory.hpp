#ifndef DICTIONARY_FACTORY_H
#define DICTIONARY_FACTORY_H

#include "dictionary.hpp"
#include "array_3gram_dict.hpp"

namespace ope {

class DictionaryFactory {
public:
    static Dictionary* createDictionary(const int type) {
        if (type == 3)
            return new Array3GramDict();
	else
	    return new Array3GramDict();
    }
};

} // namespace ope

#endif // DICTIONARY_FACTORY_H
