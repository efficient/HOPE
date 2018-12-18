#ifndef ART_DIC_N_H
#define ART_DIC_N_H

#include <stdio.h>
#include <cstdint>
#include <algorithm>

namespace ARTDIC {

    static const unsigned maxPrefixLen = 300;
    enum class NTypes : uint8_t {
        N4 = 0,
        N16 = 1,
        N48 = 2,
        N256 = 3
    };


    class N {
    public:
        // node type
        NTypes type;

        // number of childrens
        uint8_t count = 0;

        // length of compressed path
        uint32_t prefix_len = 0;

        // compressed path
        uint8_t prefix[maxPrefixLen];

        N(NTypes _type, const uint8_t *_prefix, uint32_t _prefix_len)
                : type(_type), prefix_len(_prefix_len) {
            for (int i = 0; i < _prefix_len; i++)
                prefix[i] = _prefix[i];
        };

        void setPrefix(const uint8_t *prefix, int length);

        N *duplicate();

        bool insert(uint8_t key, N *node);

        bool remove(uint8_t key);

        template<class curN, class biggerN>
        static void insertGrow(curN *n, uint8_t k, N *node, uint8_t key_par, N *parent);

        template<class curN, class smallerN>
        static void removeAndShrink(curN *n, uint8_t key, uint8_t key_par, N *parent);

        // API for tree classes
        static void insertOrUpdateNode(N *node, N *parentNode, uint8_t keyParent, uint8_t key, N *val);

        static void removeNode(N *node, N *parentNode, uint8_t keyParent, uint8_t key);

        static void change(N *node, uint8_t key, N *val);

        static N *setLeaf(N *node);

        static N *getValueFromLeaf(N *leaf);

        static bool isLeaf(N *node);

        static N *getChild(uint8_t key, N *node);

        static void getChildren(N *node, uint8_t start, uint8_t end,
                                uint8_t *children_key, N **children_p, int &cnt);

        static N *getFirstChild(N *node);

        static N *getLastChild(N *node);

        // [k, 255)
        static N *getNextChild(N *node, uint8_t k);

        //[0,k)
        static N *getPrevChild(N *node, uint8_t k);

        static void deleteChildren(N *node);
    };

    class N4 : public N {
    public:
        uint8_t keys[4];
        N *children[4] = {nullptr};

        N4(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N4, prefix, prefix_len) {};

        bool insert(uint8_t key, N *node);

        bool remove(uint8_t key);

        void change(uint8_t key, N *val);

        N *getChild(uint8_t key);

        N *getFirstChild();

        N *getLastChild();

        N *getNextChild(uint8_t k);

        N *getPrevChild(uint8_t k);

        void deleteChildren();

        template<class NODE>
        void copyTo(NODE *n) const;
    };

    class N16 : public N {
    public:
        uint8_t keys[16];
        N *children[16] = {nullptr};

        N16(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N16, prefix, prefix_len) {};

        // TODO
        bool insert(uint8_t key, N *node);

        bool remove(uint8_t key);

        void change(uint8_t key, N *val);

        N *getChild(uint8_t key);

        N *getFirstChild();

        N *getLastChild();

        N *getNextChild(uint8_t k);

        N *getPrevChild(uint8_t k);

        void deleteChildren();

        template<class NODE>
        void copyTo(NODE *n) const;
    };

    class N48 : public N {
    public:
        const uint8_t empty_marker = 48;
        uint8_t child_index[256];
        N *children[48] = {nullptr};

        N48(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N48, prefix, prefix_len) {
            std::fill_n(child_index, 256, empty_marker);
        };

        bool insert(uint8_t key, N *n);

        bool remove(uint8_t key);

        void change(uint8_t key, N *val);

        N *getChild(uint8_t k) const;

        N *getFirstChild();

        N *getLastChild();

        N *getNextChild(uint8_t k);

        N *getPrevChild(uint8_t k);

        void deleteChildren();

        template<class NODE>
        void copyTo(NODE *n) const;
    };

    class N256 : public N {
    public:
        N *children[256] = {nullptr};

        N256(const uint8_t *prefix, uint32_t prefix_len) : N(NTypes::N256, prefix, prefix_len) {};

        bool insert(uint8_t key, N *n);

        bool remove(uint8_t key);

        void change(uint8_t key, N *val);

        N *getChild(uint8_t k) const;

        N *getFirstChild();

        N *getLastChild();

        N *getNextChild(uint8_t k);

        N *getPrevChild(uint8_t k);

        void deleteChildren();

        template<class NODE>
        void copyTo(NODE *n) const;
    };
}

#endif
