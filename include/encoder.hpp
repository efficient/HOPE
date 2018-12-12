#ifndef ENCODER_H
#define ENCODER_H

#include <assert.h>
#include <string>
#include <vector>

namespace ope {

class Encoder {
public:
    virtual ~Encoder() {};
    
    virtual bool build (const std::vector<std::string>& key_list,
                        const int64_t dict_size_limit) = 0;

    virtual int encode (const std::string& key, uint8_t* buffer) const = 0;

    virtual int numEntries () const = 0;

    virtual int64_t memoryUse () const = 0;
};

} // namespace ope

#endif // ENCODER_H
