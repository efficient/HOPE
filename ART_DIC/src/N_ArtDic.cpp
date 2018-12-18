#include "../include/N_ArtDic.h"
#include "N4_ArtDic.cpp"
#include "N16_ArtDic.cpp"
#include "N48_ArtDic.cpp"
#include "N256_ArtDic.cpp"

#include <iostream>

using namespace std;

namespace ARTDIC {

    void N::setPrefix(const uint8_t *_prefix, int length) {
        for (int i = 0; i < length; i++) {
            prefix[i] = _prefix[i];
        }
        prefix_len = length;
    }

    N *N::setLeaf(N *n) {
        // 7fff ffff  + 1
        // 8000 0000 | n
        return reinterpret_cast<N *>((static_cast<uint64_t>(1) << 63) |
                                     reinterpret_cast<uint64_t>(n));
    }

    bool N::isLeaf(N *n) {
        // use & 1 instead &ffffffffffffffff becuase of the difference
        // in treating >> in macos and linux
        return ((reinterpret_cast<uint64_t>(n) >> 63 & 1) == 1);
    }

    template<class curN, class biggerN>
    void N::insertGrow(curN *n, uint8_t key, N *val, uint8_t key_par, N *node_par) {
        // current key has alreay existed
        if (n->getChild(key) != nullptr) {
            n->change(key, val);
            return;
        }
        // current node is not full
        if (n->insert(key, val))
            return;
        // initialize a bigger node
        auto big_node = new biggerN(n->prefix, n->prefix_len);
        // copy original keys and children
        n->copyTo(big_node);
        // insert key,val to the new node
        big_node->insert(key, val);
        // replace old node with new node
        N::change(node_par, key_par, big_node);

        // delete old node
        delete n;
    }

    template<class curN, class smallerN>
    void N::removeAndShrink(curN *n, uint8_t key, uint8_t key_par, N *parent_node) {
        if (n->remove(key)) {
            return;
        }
        // initialize a smaller node
        auto nSmall = new smallerN(n->prefix, n->prefix_len);
        // remove key
        n->remove(key);
        // copy to smaller node
        n->copyTo(nSmall);
        // replace old node with new node
        N::change(parent_node, key_par, nSmall);

        delete n;
    }

    void N::change(N *node, uint8_t key, N *val) {
        switch (node->type) {
            case NTypes::N4: {
                N4 *n = static_cast<N4 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N16: {
                N16 *n = static_cast<N16 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N48: {
                N48 *n = static_cast<N48 *>(node);
                n->change(key, val);
                return;
            }
            case NTypes::N256: {
                N256 *n = static_cast<N256 *>(node);
                n->change(key, val);
                return;
            }
            default:
                break;
        }
    }

    void N::insertOrUpdateNode(N *node, N *parent_node, uint8_t parent_key, uint8_t key, N *val) {
        switch (node->type) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                insertGrow<N4, N16>(n, key, val, parent_key, parent_node);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                insertGrow<N16, N48>(n, key, val, parent_key, parent_node);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                insertGrow<N48, N256>(n, key, val, parent_key, parent_node);
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                if (n->getChild(key) != nullptr)
                    n->change(key, val);
                else
                    n->insert(key, val);
                return;
            }
        }
        cout << "[Error]insert and grow Node" << endl;
        assert(false);
    }

    void N::removeNode(N *node, N *parent_node, uint8_t key_par, uint8_t key) {
        switch (node->type) {
            case NTypes::N4: {
                auto n = static_cast<N4 *>(node);
                removeAndShrink<N4, N4>(n, key, key_par, parent_node);
                //n->remove(key);
                return;
            }
            case NTypes::N16: {
                auto n = static_cast<N16 *>(node);
                removeAndShrink<N16, N4>(n, key, key_par, parent_node);
                return;
            }
            case NTypes::N48: {
                auto n = static_cast<N48 *>(node);
                removeAndShrink<N48, N16>(n, key, key_par, parent_node);
                return;
            }
            case NTypes::N256: {
                auto n = static_cast<N256 *>(node);
                removeAndShrink<N256, N48>(n, key, key_par, parent_node);
                return;
            }
        }
        cout << "[Error]remove and shrink Node" << endl;
        assert(false);
    }

    N *N::duplicate() {
        switch (this->type) {
            case NTypes::N4: {
                N4 *new_node = new N4(prefix, prefix_len);
                N4 *node = reinterpret_cast<N4 *>(this);
                node->copyTo(new_node);
                return new_node;
            }
            case NTypes::N16: {
                N16 *new_node = new N16(prefix, prefix_len);
                N16 *node = reinterpret_cast<N16 *>(this);
                node->copyTo(new_node);
                return new_node;
            }
            case NTypes::N48: {
                N48 *new_node = new N48(prefix, prefix_len);
                N48 *node = reinterpret_cast<N48 *>(this);
                node->copyTo(new_node);
                return new_node;
            }
            case NTypes::N256: {
                N256 *new_node = new N256(prefix, prefix_len);
                N256 *node = reinterpret_cast<N256 *>(this);
                node->copyTo(new_node);
                return new_node;
            }
        }
        return nullptr;
    }

    bool N::insert(uint8_t key, N *node) {
        switch (this->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(this)->insert(key, node);
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(this)->insert(key, node);
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(this)->insert(key, node);
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(this)->insert(key, node);
            }
        }
        return false;
    }

    N *N::getChild(uint8_t key, N *node) {
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->getChild(key);
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->getChild(key);
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->getChild(key);
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->getChild(key);
            }
        }
        return nullptr;
    }

    bool N::remove(uint8_t key) {
        switch (this->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(this)->remove(key);
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(this)->remove(key);
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(this)->remove(key);
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(this)->remove(key);
            }
        }
        return false;
    }

    void N::getChildren(N *node, uint8_t start, uint8_t end,
                        uint8_t *children_key, N **children_p, int &child_cnt) {
        child_cnt = 0;
        for (uint8_t cur = start; cur < end; cur++) {
            //cout << "Current key:"<<unsigned(cur)<<endl;
            N *child = getChild(cur, node);
            if (child == nullptr)
                continue;
            children_key[child_cnt] = cur;
            children_p[child_cnt] = child;
            child_cnt++;
        }
        if (start == 0 && end == 255 && child_cnt != node->count) {
            cout << "Child Count mismatch" << endl;
            cout << "Child count:" << unsigned(child_cnt) << endl;
            cout << "Record count:" << unsigned(node->count) << endl;
            assert(false);
        }
    }

    N *N::getValueFromLeaf(N *leaf) {
        return reinterpret_cast<N *>(reinterpret_cast<uint64_t>(leaf) & 0x7fffffffffffffff);
    }

    N *N::getLastChild(ARTDIC::N *node) {
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->getLastChild();
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->getLastChild();
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->getLastChild();
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->getLastChild();
            }
        }
        assert(false);
    }

    N *N::getFirstChild(ARTDIC::N *node) {
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->getFirstChild();
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->getFirstChild();
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->getFirstChild();
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->getFirstChild();
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N::getNextChild(ARTDIC::N *node, uint8_t k) {
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->getNextChild(k);
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->getNextChild(k);
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->getNextChild(k);
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->getNextChild(k);
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    N *N::getPrevChild(ARTDIC::N *node, uint8_t k) {
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->getPrevChild(k);
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->getPrevChild(k);
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->getPrevChild(k);
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->getPrevChild(k);
            }
        }
        assert(false);
        __builtin_unreachable();
    }

    void N::deleteChildren(ARTDIC::N *node) {
        if (N::isLeaf(node)) {
            return;
        }
        switch (node->type) {
            case NTypes::N4: {
                return reinterpret_cast<N4 *>(node)->deleteChildren();
            }
            case NTypes::N16: {
                return reinterpret_cast<N16 *>(node)->deleteChildren();
            }
            case NTypes::N48: {
                return reinterpret_cast<N48 *>(node)->deleteChildren();
            }
            case NTypes::N256: {
                return reinterpret_cast<N256 *>(node)->deleteChildren();
            }
        }
        assert(false);
        __builtin_unreachable();
    }

}