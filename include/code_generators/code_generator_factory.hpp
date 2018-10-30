#ifndef CODE_GENERATOR_FACTORY_H
#define CODE_GENERATOR_FACTORY_H

#include "code_generator.hpp"
#include "hu_tucker_cg.hpp"

namespace ope {

class CodeGeneratorFactory {
public:
    static CodeGenerator* createCodeGenerator(const int type) {
        if (type == 0)
            return new HuTuckerCG();
	else
	    return new HuTuckerCG();
    }
};

} // namespace ope

#endif // CODE_GENERATOR_FACTORY_H
