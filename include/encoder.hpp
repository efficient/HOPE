#ifndef ENCODER_H
#define ENCODER_H

#include <assert.h>
#include <string>
#include <vector>

namespace ope {

class Encoder {
public:
    virtual bool build (const std::vector<std::string>& key_list,
                        const int64_t dict_size_limit) = 0;

    virtual int encode (const std::string& key, uint8_t* buffer) = 0;

    virtual int numEntries () = 0;

    virtual int64_t memoryUse () = 0;
};

} // namespace ope

#endif // ENCODER_H
