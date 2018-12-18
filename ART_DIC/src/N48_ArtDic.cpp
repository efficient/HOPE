#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "../include/N_ArtDic.h"

using namespace std;

namespace ARTDIC {
    bool N48::insert(uint8_t k, N *n) {
        if (count == 48)
            return false;
        unsigned int pos = 0;
        while (children[pos] != nullptr) {
            pos = (pos + 1) % 48;
        }
        children[pos] = n;
        child_index[k] = pos;
        count++;
        return true;
    }

    bool N48::remove(uint8_t k) {
        assert(child_index[k] != empty_marker);
        if (count == 16)
            return false;
        children[child_index[k]] = nullptr;
        child_index[k] = empty_marker;
        count--;
        assert(getChild(k) == nullptr);
        return true;
    }

    N *N48::getChild(uint8_t k) const {
        if (child_index[k] == empty_marker)
            return nullptr;
        return children[child_index[k]];
    }

    void N48::change(uint8_t key, N *val) {
        children[child_index[key]] = val;
    }

    N *N48::getFirstChild() {
        if (count == 0)
            return nullptr;
        for (int k = 0; k < 256; k++) {
            N * child = getChild((uint8_t)k);
            if (child != nullptr)
                return child;
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N48::getLastChild() {
        if (count == 0)
            return nullptr;
        for (uint8_t k = 255; (int)k >= 0; k--) {
            N * child = getChild(k);
            if (child != nullptr)
                return child;
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N48::getNextChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        for (int i = k; i < 256; i++) {
            N *child = getChild((uint8_t)i);
            if(child != nullptr)
                return child;
        }
        return nullptr;
    }

    N *N48::getPrevChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        for (int i = 0; i < k; i++) {
            N *child = getChild((uint8_t) i);
            if(child != nullptr)
                return child;
        }
        return nullptr;
    }

    void N48::deleteChildren() {
        for (unsigned char i : child_index) {
            if (i != empty_marker) {
                N::deleteChildren(children[i]);
                N::deleteNode(children[i]);
            }
        }
    }

    template<class NODE>
    void N48::copyTo(NODE *n) const {
        for (int i = 0; i < 256; i++) {
            if (child_index[i] != empty_marker) {
                n->insert(i, children[child_index[i]]);
            }
        }
    }
}
