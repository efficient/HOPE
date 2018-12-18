#include <assert.h>
#include <iostream>
#include "../include/N_ArtDic.h"

using namespace std;

namespace ARTDIC {

    bool N4::insert(uint8_t k, N *node) {
        if (count == 4)
            return false;
        int i = 0;
        while (i < count && k >= keys[i])
            i += 1;
        memcpy(keys + i + 1, keys + i, (count - i) * sizeof(k));
        memcpy(children + i + 1, children + i, (count - i) * sizeof(N * ));
        keys[i] = k;
        children[i] = node;
        count += 1;
        return true;
    }

    bool N4::remove(uint8_t k) {
        for (int i = 0; i < count; i++) {
            if (keys[i] == k) {
                memcpy(keys + i, keys + i + 1, (count - i - 1) * sizeof(k));
                memcpy(children + i, children + i + 1, (count - i - 1) * sizeof(N * ));
                count--;
                return true;
            }
        }
        cout << "[Error]Remove N4 element not exist" << endl;
        assert(false);
    }

    void N4::change(uint8_t key, N *val) {
        for (uint8_t i = 0; i < count; i++) {
            if (children[i] != nullptr && keys[i] == key) {
                children[i] = val;
                return;
            }
        }
    }

    N *N4::getChild(uint8_t k) {
        for (int i = 0; i < count; i++) {
            if (this->keys[i] == k) {
                return children[i];
            }
        }
        return nullptr;
    }

    N *N4::getFirstChild() {
        if (count == 0)
            return nullptr;
        return children[0];
    }

    N *N4::getLastChild() {
        if (count == 0)
            return nullptr;
        return children[count - 1];
    }

    N *N4::getNextChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        for (int i = 0; i < count; i++) {
            if(keys[i] >= k)
                return children[i];
        }
        return nullptr;
    }

    N *N4::getPrevChild(uint8_t k) {
        if (count == 0)
            return nullptr;
        int i = 0;
        for (; i < count; i++) {
            if (keys[i] >= k && i>= 1)
                return children[i-1];
        }
        if (i == count)
            return children[i-1];
        return nullptr;
    }

    void N4::deleteChildren() {
        for(uint8_t i = 0; i < count; i++) {
            N::deleteChildren(children[i]);
            N::deleteNode(children[i]);
        }
    }

    template<class NODE>
    void N4::copyTo(NODE *n) const {
        for (int i = 0; i < count; i++) {
            n->insert(keys[i], children[i]);
        }
    }
}

