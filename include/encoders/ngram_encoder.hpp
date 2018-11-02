#ifndef NGRAM_ENCODER_H
#define NGRAM_ENCODER_H

#include "encoder.hpp"

#include "dictionary_factory.hpp"
#include "code_generator_factory.hpp"
#include "symbol_selector_factory.hpp"

namespace ope {

class NGramEncoder : public Encoder {
public:
    static const int kCgType = 0;
    static const int kDictType = 0;
    
    NGramEncoder(int n) : n_(n) {};

    bool build (const std::vector<std::string>& key_list,
                const int64_t dict_size_limit);

    int encode (const std::string& key, uint8_t* buffer) const;

    int numEntries () const;
    int64_t memoryUse () const;

private:
    int n_;
    Dictionary* dict_;
};

bool NGramEncoder::build (const std::vector<std::string>& key_list,
			  const int64_t dict_size_limit) {
    std::vector<SymbolFreq> symbol_freq_list;
    SymbolSelector* symbol_selector = SymbolSelectorFactory::createSymbolSelector(n_);
    symbol_selector->selectSymbols(key_list, dict_size_limit, &symbol_freq_list);

    std::vector<SymbolCode> symbol_code_list;
    CodeGenerator* code_generator = CodeGeneratorFactory::createCodeGenerator(kCgType);
    code_generator->genCodes(symbol_freq_list, &symbol_code_list);

    dict_ = DictionaryFactory::createDictionary(kDictType);
    return dict_->build(); //TODO
}

int NGramEncoder::encode (const std::string& key, uint8_t* buffer) {
    //TODO
}

int NGramEncoder::numEntries () {
    return dict_->numEntries();
}

int64_t NGramEncoder::memoryUse () {
    return dict_->memoryUse();
}

} // namespace ope

#endif // NGRAM_ENCODER_H
