#ifndef COMMON_H
#define COMMON_H

#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <string>

// Use array-based dictionary only for comparison purposes
// #define USE_ARRAY_DICT 1

// #define USE_FIXED_LEN_DICT_CODE 1

// The current decoder implementation is experimental and less optimized
#define INCLUDE_DECODE 1

// For benchmark only
// #define PRINT_BUILD_TIME_BREAKDOWN 1

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

void setStopWatch(double &cur_time, int msg_select) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  std::cout << "------------------------Build ";
  if (msg_select == 1)
    std::cout << "SingleChar";
  else if (msg_select == 2)
    std::cout << "DoubleChar";
  else if (msg_select == 3 || msg_select == 4)
    std::cout << msg_select << "-Gram";
  else if (msg_select == 5)
    std::cout << "ALM";
  else if (msg_select == 6)
    std::cout << "ALM-Improved";
  std::cout << " Encoder-----------------------" << std::endl;
#endif
  cur_time = getNow();
}
    
void printElapsedTime(double &cur_time, int msg_select) {
#ifdef PRINT_BUILD_TIME_BREAKDOWN
  double elapsed_time = getNow() - cur_time;
  if (msg_select == 0)
    std::cout << "Symbol Select ";
  else if (msg_select == 1)
    std::cout << "Code Assign ";
  else if (msg_select == 2)
    std::cout << "Build Dictionary ";
  std::cout << "time = " << elapsed_time << std::endl;
#endif
  cur_time = getNow();
}

}  // namespace ope

#endif  // COMMON_H
