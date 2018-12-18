#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>
#include <time.h>

#include <string>

//#define PRINT_BUILD_TIME_BREAKDOWN 1
//#define USE_ARRAY_DICT 1
//#define USE_FIXED_LEN_DICT_CODE 1

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

double getNow() {
  struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1000000.0;
}

} // namespace ope

#endif // COMMON_H
