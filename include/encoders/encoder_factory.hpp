#ifndef ENCODER_FACTORY_H
#define ENCODER_FACTORY_H

#include "ALMImproved_encoder.hpp"
#include "double_char_encoder.hpp"
#include "encoder.hpp"
#include "ALM_encoder.hpp"
#include "ngram_encoder.hpp"
#include "single_char_encoder.hpp"

namespace hope {

class EncoderFactory {
 public:
  static Encoder *createEncoder(const int type, int W = 10000) {
    if (type == 1)
      return new SingleCharEncoder();
    else if (type == 2)
      return new DoubleCharEncoder();
    else if (type == 3)
      return new NGramEncoder(3);
    else if (type == 4)
      return new NGramEncoder(4);
    else if (type == 5)
      return new ALMEncoder(W);
    else if (type == 6)
      return new ALMImprovedEncoder(W);
    else
      return new DoubleCharEncoder();
  }
};

}  // namespace hope

#endif  // ENCODER_FACTORY_H
