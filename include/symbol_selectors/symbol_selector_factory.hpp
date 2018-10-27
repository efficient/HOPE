#ifndef SYMBOL_SELECTOR_FACTORY_H
#define SYMBOL_SELECTOR_FACTORY_H

#include "symbol_selector.hpp"
#include "single_char_ss.hpp"

namespace ope {

class SymbolSelectorFactory {
public:
    static SymbolSelector* createSymbolSelector(const int type) {
        if (type == 0)
            return new SingleCharSS();
    }
};

} // namespace ope

#endif SYMBOL_SELECTOR_FACTORY_H
