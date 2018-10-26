#ifndef ENCODER_H
#define ENCODER_H

#include <string>
#include <vector>

namespace ope {

class Encoder {
public:
    virtual int encode (const std::string& key, uint8_t* buffer) = 0;
};

} // namespace ope

#endif ENCODER_H
