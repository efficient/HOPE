#ifndef DICTIONARY_FACTORY_H
#define DICTIONARY_FACTORY_H

#include "dictionary.hpp"
#include "array_dict.hpp"

namespace ope {

class DictionaryFactory {
public:
    static Dictionary* createDictionary(const int type) {
        if (type == 0)
            return new ArrayDict();
	else
	    return new ArrayDict();
    }
};

} // namespace ope

#endif // DICTIONARY_FACTORY_H
