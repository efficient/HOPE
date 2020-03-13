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
      std::cout << "Type 0, fix" << std::endl;
      return new FixedLenDictCG();
#else
      std::cout << "Type 0, hutucker" << std::endl;
      return new HuTuckerCG();
#endif
    } else {
      std::cout << "Type 1, hutucker" << std::endl;
      return new HuTuckerCG();
    }
  }
};

}  // namespace ope

#endif  // CODE_GENERATOR_FACTORY_H
