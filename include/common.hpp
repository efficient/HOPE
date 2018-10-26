#ifndef COMMON_H
#define COMMON_H

namespace ope {

typedef struct {
    int64_t code;
    int8_t len;
} Code;

typedef typename std::pair<std::string, int64_t> SymbolFreq;

typedef typename std::pair<std::string, Code> SymbolCode;

} // namespace ope

#endif COMMON_H
