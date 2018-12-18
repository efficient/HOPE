#include <stdio.h>
#include "../include/N_ArtDic.h"

namespace ARTDIC {
    bool N256::insert(uint8_t k, N *n) {
        children[k] = n;
        count++;
        return true;
    }

    bool N256::remove(uint8_t k) {
        if (count == 48)
            return false;
        children[k] = nullptr;
        count--;
        return true;
    }

    N *N256::getChild(uint8_t k) const {
        return children[k];
    }

    void N256::change(uint8_t key, N *val) {
        children[key] = val;
    }

    N *N256::getFirstChild() {
        if (count == 0)
            return nullptr;
        for (int k = 0; k < 256; k++) {
            N *child = getChild((uint8_t)k);
            if(child != nullptr)
                return child;
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N256::getLastChild() {
        if (count == 0)
            return nullptr;
        for (uint8_t k = 255; (int)k > 0; k--) {
            N *child = getChild(k);
            if(child != nullptr)
                return child;
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N256::getNextChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        for (int i = k; i < 256; i++) {
            N *child = getChild((uint8_t)i);
            if(child != nullptr)
                return child;
        }
        return nullptr;
    }

    N *N256::getPrevChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        for (int i = 0; i < k; i++) {
            N *child = getChild((uint8_t)k);
            if(child != nullptr)
                return child;
        }
        return nullptr;
    }

    void N256::deleteChildren() {
        for (int i =0; i < 256; i++) {
            if (children[i] != nullptr) {
                N::deleteChildren(children[i]);
                N::deleteNode(children[i]);
            }
        }
    }

    template<class NODE>
    void N256::copyTo(NODE *n) const {
        for (int i = 0; i < count; i++) {
            n->insert(i, children[i]);
        }
    }
}
