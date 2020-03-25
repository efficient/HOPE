#ifndef CODE_GENERATOR_FACTORY_H
#define CODE_GENERATOR_FACTORY_H

#include "code_generator.hpp"
#include "hu_tucker_cg.hpp"

#ifdef USE_FIXED_LEN_DICT_CODE
#include "fixed_len_dict_cg.hpp"
#endif

namespace ope {

class CodeGeneratorFactory {
 public:
  static CodeGenerator *createCodeGenerator(const int type) {
    if (type == 0) {
#ifdef USE_FIXED_LEN_DICT_CODE
      return new FixedLenDictCG();
#else
      return new HuTuckerCG();
#endif
    } else {
      return new HuTuckerCG();
    }
  }
};

}  // namespace ope

#endif  // CODE_GENERATOR_FACTORY_H
