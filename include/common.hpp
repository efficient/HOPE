#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <string>

#define PRINT_BUILD_TIME_BREAKDOWN 1
//#define USE_ARRAY_DICT 1
//#define USE_FIXED_LEN_DICT_CODE 1
#define INCLUDE_DECODE 1
#define MAX_STR_LEN 50
//#define BATCH_DRY_ENCODE

const int dict_size_list[9] = {1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};

const int64_t three_gram_input_dict_size[4][7] = {{831, 1983, 4608, 10496, 24064, 52224, 80000},
                                                  {799, 1919, 4352, 9600, 20480, 41984, 80000},
                                                  {815, 1983, 4352, 9472, 19968, 41984, 80000},
                                                  {0, 0, 0, 0, 0, 0, 0}};

const int64_t four_gram_input_dict_size[4][9] = {{815, 1919, 4160, 8704, 18176, 37888, 79687, 168750, 351562},
                                                 {767, 1823, 3967, 8320, 17408, 35840, 75000, 151551, 309375},
                                                 {799, 1855, 3967, 8448, 17408, 35328, 71875, 147455, 295312},
                                                 {0, 0, 0, 0, 0, 0, 0, 0, 0}};

const int ALM_W[3][9] = {{399219, 206250, 99999, 49999, 25780, 13280, 6639, 3319, 1561},
                         {146092, 70311, 37499, 20701, 11326, 6053, 3319, 2048, 1316},
                         {4843749, 2656249, 1445311, 820311, 468749, 263670, 161131, 90330, 46384}};

const int ALM_W_improved[3][9] = {{13274, 7726, 2462, 3436, 2071, 1131, 252, 359, 186},
                                  {7874, 4030, 2249, 1264, 725, 397, 215, 115, 65},
                                  {42000, 35000, 21875, 13436, 8436, 4999, 2889, 1717, 1014}};

namespace ope {

static const int kNumSingleChar = 256;
static const int kNumDoubleChar = 65536;

typedef struct {
  // int64_t code;
  int32_t code;
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

}  // namespace ope

#endif  // COMMON_H
