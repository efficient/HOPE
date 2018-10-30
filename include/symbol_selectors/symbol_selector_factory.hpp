#ifndef SYMBOL_SELECTOR_FACTORY_H
#define SYMBOL_SELECTOR_FACTORY_H

#include "symbol_selector.hpp"
#include "single_char_ss.hpp"
#include "double_char_ss.hpp"

namespace ope {

class SymbolSelectorFactory {
public:
    static SymbolSelector* createSymbolSelector(const int type) {
        if (type == 0)
            return new SingleCharSS();
	else if (type == 1)
            return new DoubleCharSS();
	else
	    return new SingleCharSS();
    }
};

} // namespace ope

#endif // SYMBOL_SELECTOR_FACTORY_H
