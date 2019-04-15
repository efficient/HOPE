#ifndef SYMBOL_SELECTOR_FACTORY_H
#define SYMBOL_SELECTOR_FACTORY_H

#include "symbol_selector.hpp"
#include "single_char_ss.hpp"
#include "double_char_ss.hpp"
#include "ngram_ss.hpp"
#include "heuristic_ss.hpp"

namespace ope {

class SymbolSelectorFactory {
public:
    static SymbolSelector* createSymbolSelector(const int type) {
    if (type == 1)
    	return new SingleCharSS();
    else if (type == 2)
    	return new DoubleCharSS();
	else if (type == 3)
		return new NGramSS(3);
	else if (type == 4)
		return new NGramSS(4);
	else if (type == 5)
		return new HeuristicSS();
	else
		return new SingleCharSS();
    }
};

} // namespace ope

#endif // SYMBOL_SELECTOR_FACTORY_H
