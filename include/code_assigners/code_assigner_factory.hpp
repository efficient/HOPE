#ifndef CODE_ASSIGNER_FACTORY_H
#define CODE_ASSIGNER_FACTORY_H

#include "code_assigner.hpp"
#include "hu_tucker_ca.hpp"

#ifdef USE_FIXED_LEN_DICT_CODE
#include "fixed_len_dict_ca.hpp"
#endif

namespace ope {

class CodeAssignerFactory {
 public:
  static CodeAssigner *createCodeAssigner(const int type) {
    if (type == 0) {
#ifdef USE_FIXED_LEN_DICT_CODE
      return new FixedLenDictCA();
#else
      return new HuTuckerCA();
#endif
    } else {
      return new HuTuckerCA();
    }
  }
};

}  // namespace ope

#endif  // CODE_ASSIGNER_FACTORY_H
