#ifndef COMMON_H
#define COMMON_H

#include <string>

namespace ope {

typedef struct {
    int64_t code;
    int8_t len;
} Code;

typedef struct {
    uint8_t common_prefix_len;
    Code code;
} IntervalCode;

typedef typename std::pair<std::string, int64_t> SymbolFreq;

typedef typename std::pair<std::string, Code> SymbolCode;

typedef struct {
    char start_key[3];
    uint8_t common_prefix_len;
    Code code;
} Interval3Gram;

typedef struct {
    char start_key[4];
    uint8_t common_prefix_len;
    Code code;
} Interval4Gram;

} // namespace ope

#endif // COMMON_H
