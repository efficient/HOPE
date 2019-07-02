#include <assert.h>
#include <algorithm>
#include <iostream>
#include "Tree.h"
#include "Epoche.cpp"
#include "N.cpp"

namespace ART_unsynchronized {

    Tree::Tree(LoadKeyFunction loadKey) : root(new N256(nullptr, 0)), loadKey(loadKey) {
    }

    Tree::~Tree() {
        N::deleteChildren(root);
        N::deleteNode(root);
    }

    TID Tree::lookup(const Key &k) const {
        N *node = nullptr;
        N *nextNode = root;
        uint32_t level = 0;
        bool optimisticPrefixMatch = false;

        while (true) {
            node = nextNode;
            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    return 0;
                case CheckPrefixResult::OptimisticMatch:
                    optimisticPrefixMatch = true;
                    // fallthrough
                case CheckPrefixResult::Match:
                    if (k.getKeyLen() <= level) {
                        return 0;
                    }
                    nextNode = N::getChild(k[level], node);

                    if (nextNode == nullptr) {
                        return 0;
                    }
                    if (N::isLeaf(nextNode)) {
                        TID tid = N::getLeaf(nextNode);
                        if (level < k.getKeyLen() - 1 || optimisticPrefixMatch) {
                            return checkKey(tid, k);
                        }
                        return tid;
                    }
                    level++;
            }
        }
    }

    bool Tree::lookupRange(const Key &, const Key &, Key &, TID [],
                                std::size_t , std::size_t &) const {
        return false;
        /*for (uint32_t i = 0; i < std::min(start.getKeyLen(), end.getKeyLen()); ++i) {
            if (start[i] > end[i]) {
                resultsFound = 0;
                return false;
            } else if (start[i] < end[i]) {
                break;
            }
        }
        TID toContinue = 0;
        std::function<void(const N *)> copy = [&result, &resultSize, &resultsFound, &toContinue, &copy](const N *node) {
            if (N::isLeaf(node)) {
                if (resultsFound == resultSize) {
                    toContinue = N::getLeaf(node);
                    return;
                }
                result[resultsFound] = N::getLeaf(node);
                resultsFound++;
            } else {
                std::tuple<uint8_t, N *> children[256];
                uint32_t childrenCount = 0;
                N::getChildren(node, 0u, 255u, children, childrenCount);
                for (uint32_t i = 0; i < childrenCount; ++i) {
                    const N *n = std::get<1>(children[i]);
                    copy(n);
                    if (toContinue != 0) {
                        break;
                    }
                }
            }
        };
        std::function<void(const N *, uint32_t)> findStart = [&copy, &start, &findStart, &toContinue, &restart, this](
                const N *node, uint32_t level) {
            if (N::isLeaf(node)) {
                copy(node);
                return;
            }

            uint64_t v;
            PCCompareResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixCompare(node, start, level, loadKey);
            } while (!node->stopReading(v));
            switch (prefixResult) {
                case PCCompareResults::Bigger:
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, startLevel, 255, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == startLevel) {
                            findStart(n, level + 1);
                        } else if (k > startLevel) {
                            copy(n);
                        }
                        if (toContinue != 0 || restart) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::SkippedLevel:
                    restart = true;
                    break;
                case PCCompareResults::Smaller:
                    break;
            }
        };
        std::function<void(const N *, uint32_t)> findEnd = [&copy, &end, &toContinue, &restart, &findEnd, this](
                const N *node, uint32_t level) {
            if (N::isLeaf(node)) {
                return;
            }
            uint64_t v;
            PCCompareResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixCompare(node, end, level, loadKey);
            } while (!node->stopReading(v));

            switch (prefixResult) {
                case PCCompareResults::Smaller:
                    copy(node);
                    break;
                case PCCompareResults::Equal: {
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, 0, endLevel, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == endLevel) {
                            findEnd(n, level + 1);
                        } else if (k < endLevel) {
                            copy(n);
                        }
                        if (toContinue != 0 || restart) {
                            break;
                        }
                    }
                    break;
                }
                case PCCompareResults::Bigger:
                    break;
                case PCCompareResults::SkippedLevel:
                    restart = true;
                    break;
            }
        };

        restart:
        restart = false;
        resultsFound = 0;

        uint32_t level = 0;
        N *node = nullptr;
        N *nextNode = root;

        while (true) {
            node = nextNode;
            uint64_t v;
            PCEqualsResults prefixResult;
            do {
                v = node->startReading();
                prefixResult = checkPrefixEquals(node, level, start, end, loadKey);
            } while (!node->stopReading(v));
            switch (prefixResult) {
                case PCEqualsResults::SkippedLevel:
                    goto restart;
                case PCEqualsResults::NoMatch: {
                    return false;
                }
                case PCEqualsResults::Contained: {
                    copy(node);
                    break;
                }
                case PCEqualsResults::StartMatch: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    std::tuple<uint8_t, N *> children[256];
                    uint32_t childrenCount = 0;
                    N::getChildren(node, startLevel, 255, children, childrenCount);
                    for (uint32_t i = 0; i < childrenCount; ++i) {
                        const uint8_t k = std::get<0>(children[i]);
                        const N *n = std::get<1>(children[i]);
                        if (k == startLevel) {
                            findStart(n, level + 1);
                        } else if (k > startLevel) {
                            copy(n);
                        }
                        if (restart) {
                            goto restart;
                        }
                        if (toContinue) {
                            break;
                        }
                    }
                    break;
                }
                case PCEqualsResults::BothMatch: {
                    uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                    uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 255;
                    if (startLevel != endLevel) {
                        std::tuple<uint8_t, N *> children[256];
                        uint32_t childrenCount = 0;
                        N::getChildren(node, startLevel, endLevel, children, childrenCount);
                        for (uint32_t i = 0; i < childrenCount; ++i) {
                            const uint8_t k = std::get<0>(children[i]);
                            const N *n = std::get<1>(children[i]);
                            if (k == startLevel) {
                                findStart(n, level + 1);
                            } else if (k > startLevel && k < endLevel) {
                                copy(n);
                            } else if (k == endLevel) {
                                findEnd(n, level + 1);
                            }
                            if (restart) {
                                goto restart;
                            }
                            if (toContinue) {
                                break;
                            }
                        }
                    } else {
                        nextNode = N::getChild(startLevel, node);
                        if (!node->stopReading(v)) {
                            goto restart;
                        }
                        level++;
                        continue;
                    }
                    break;
                }
            }
            break;
        }
        if (toContinue != 0) {
            loadKey(toContinue, continueKey);
            return true;
        } else {
            return false;
        }*/
    }

    //huanchen
    void Tree::traverse(double& memory, double& avg_height,
                        int& cnt_N4, int& cnt_N16,
                        int& cnt_N48, int& cnt_N256,
                        uint64_t&  waste_child_mem, uint64_t& skip_prefix_mem ,
                        uint64_t& waste_prefix_mem) const {
	uint64_t mem = 0;
	uint64_t height_sum = 0;
	uint64_t node_count = 1;
	std::queue<N*> node_queue;
	node_queue.push(root);
	int height = 0;
	int node_count_cur_level = 0;
	int node_count_next_level = 1;
	while (!node_queue.empty()) {
	    if (node_count_cur_level == 0) {
		node_count_cur_level = node_count_next_level;
		node_count_next_level = 0;
		height++;
	    }
	    
	    N* node = node_queue.front();
	    
	    NTypes type = node->getType();
        uint64_t child_cnt = node->getCount();
        uint64_t prefix_cnt = node->getPrefixLength();
            //std::cout << "Prefix length:" << prefix_cnt << std::endl;
        if (prefix_cnt < ART_unsynchronized::maxStoredPrefixLength) {
            waste_prefix_mem += (ART_unsynchronized::maxStoredPrefixLength - prefix_cnt) * sizeof(uint8_t);
	    } else {
		    skip_prefix_mem += (prefix_cnt - ART_unsynchronized::maxStoredPrefixLength) * sizeof(uint8_t);
        }
	    if (type == NTypes::N4) {
            mem += sizeof(N4);
            waste_child_mem += (4 - child_cnt) * (sizeof(uint8_t) + sizeof(N*));
		    cnt_N4++;
	    } else if (type == NTypes::N16) {
		    mem += sizeof(N16);
            waste_child_mem += (16 - child_cnt) * (sizeof(uint8_t) + sizeof(N*));
            cnt_N16++;
	    } else if (type == NTypes::N48) {
	    	mem += sizeof(N48);
            waste_child_mem += (48 - child_cnt) * sizeof(N*) + (256 - child_cnt) * sizeof(uint8_t);
            cnt_N48++;
	    } else if (type == NTypes::N256) {
            mem += sizeof(N256);
            waste_child_mem += (256 - child_cnt) * (sizeof(N*));
            cnt_N256++;
        }
	    
	    std::tuple<uint8_t, N*> children[256];
	    uint32_t children_count = 0;
	    N::getChildren(node, 0u, 255u, children, children_count);
	    for (uint32_t i = 0; i < children_count; ++i) {
		N* n = std::get<1>(children[i]);
		if (N::isLeaf(n)) {
		    height_sum += height;
		} else {
		    node_queue.push(n);
		    node_count_next_level++;
		}
		node_count++;
	    }

	    node_queue.pop();
	    node_count_cur_level--;
        }
        std::cout << "Waste prefix mem:" << waste_prefix_mem << std::endl;
        std::cout << "Skip prefix mem:" << skip_prefix_mem << std::endl;
        std::cout << "Waste child mem:" << waste_child_mem << std::endl;
        memory = (mem + 0.0) / 1000000;
        avg_height = (height_sum + 0.0) / node_count;
        std::cout << "node count = " << node_count << std::endl;
        std::cout << "mem = " << memory << " MB" << "(" << cnt_N4 << "," << cnt_N16 << "," << cnt_N48 << "," << cnt_N256 << ")" << std::endl;
        std::cout << "avg height = " << avg_height << std::endl;
    }


    TID Tree::checkKey(const TID tid, const Key &k) const {
        Key kt;
        this->loadKey(tid, kt);
        if (k == kt) {
            return tid;
        }
        return 0;
    }

    void Tree::insert(const Key &k, TID tid) {
        N *node = nullptr;
        N *nextNode = root;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint32_t level = 0;

        while (true) {
            parentNode = node;
            parentKey = nodeKey;
            node = nextNode;

            uint32_t nextLevel = level;

            uint8_t nonMatchingKey;
            Prefix remainingPrefix;
            switch (checkPrefixPessimistic(node, k, nextLevel, nonMatchingKey, remainingPrefix,
                                                           this->loadKey)) { // increases level
                case CheckPrefixPessimisticResult::NoMatch: {
                    assert(nextLevel < k.getKeyLen()); //prevent duplicate key
                    // 1) Create new node which will be parent of node, Set common prefix, level to this node
                    auto newNode = new N4(node->getPrefix(), nextLevel - level);

                    // 2)  add node and (tid, *k) as children
                    newNode->insert(k[nextLevel], N::setLeaf(tid));
                    newNode->insert(nonMatchingKey, node);

                    // 3) update parentNode to point to the new node
                    N::change(parentNode, parentKey, newNode);

                    // 4) update prefix of node
                    node->setPrefix(remainingPrefix,
                                    node->getPrefixLength() - ((nextLevel - level) + 1));

                    return;
                }
                case CheckPrefixPessimisticResult::Match:
                    break;
            }
            assert(nextLevel < k.getKeyLen()); //prevent duplicate key
            level = nextLevel;
            nodeKey = k[level];
            nextNode = N::getChild(nodeKey, node);

            if (nextNode == nullptr) {
                N::insertA(node, parentNode, parentKey, nodeKey, N::setLeaf(tid));
                return;
            }
            if (N::isLeaf(nextNode)) {
                Key key;
                loadKey(N::getLeaf(nextNode), key);

                level++;
                assert(level < key.getKeyLen()); //prevent inserting when prefix of key exists already
                uint32_t prefixLength = 0;
                while (key[level + prefixLength] == k[level + prefixLength]) {
                    prefixLength++;
                }

                auto n4 = new N4(&k[level], prefixLength);
                n4->insert(k[level + prefixLength], N::setLeaf(tid));
                n4->insert(key[level + prefixLength], nextNode);
                N::change(node, k[level - 1], n4);
                return;
            }

            level++;
        }
    }

    void Tree::remove(const Key &k, TID tid) {
        N *node = nullptr;
        N *nextNode = root;
        N *parentNode = nullptr;
        uint8_t parentKey, nodeKey = 0;
        uint32_t level = 0;
        //bool optimisticPrefixMatch = false;

        while (true) {
            parentNode = node;
            parentKey = nodeKey;
            node = nextNode;

            switch (checkPrefix(node, k, level)) { // increases level
                case CheckPrefixResult::NoMatch:
                    return;
                case CheckPrefixResult::OptimisticMatch:
                    // fallthrough
                case CheckPrefixResult::Match: {
                    nodeKey = k[level];
                    nextNode = N::getChild(nodeKey, node);

                    if (nextNode == nullptr) {
                        return;
                    }
                    if (N::isLeaf(nextNode)) {
                        if (N::getLeaf(nextNode) != tid) {
                            return;
                        }
                        assert(parentNode == nullptr || node->getCount() != 1);
                        if (node->getCount() == 2 && node != root) {
                            // 1. check remaining entries
                            N *secondNodeN;
                            uint8_t secondNodeK;
                            std::tie(secondNodeN, secondNodeK) = N::getSecondChild(node, nodeKey);
                            if (N::isLeaf(secondNodeN)) {

                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);

                                delete node;
                            } else {
                                //N::remove(node, k[level]); not necessary
                                N::change(parentNode, parentKey, secondNodeN);
                                secondNodeN->addPrefixBefore(node, secondNodeK);

                                delete node;
                            }
                        } else {
                            N::removeA(node, k[level], parentNode, parentKey);
                        }
                        return;
                    }
                    level++;
                }
            }
        }
    }


    inline typename Tree::CheckPrefixResult Tree::checkPrefix(N *n, const Key &k, uint32_t &level) {
        if (k.getKeyLen() <= level + n->getPrefixLength()) {
            return CheckPrefixResult::NoMatch;
        }
        if (n->hasPrefix()) {
            for (uint32_t i = 0; i < std::min(n->getPrefixLength(), maxStoredPrefixLength); ++i) {
                if (n->getPrefix()[i] != k[level]) {
                    return CheckPrefixResult::NoMatch;
                }
                ++level;
            }
            if (n->getPrefixLength() > maxStoredPrefixLength) {
                level += n->getPrefixLength() - maxStoredPrefixLength;
                return CheckPrefixResult::OptimisticMatch;
            }
        }
        return CheckPrefixResult::Match;
    }

    typename Tree::CheckPrefixPessimisticResult Tree::checkPrefixPessimistic(N *n, const Key &k, uint32_t &level,
                                                                        uint8_t &nonMatchingKey,
                                                                        Prefix &nonMatchingPrefix,
                                                                        LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            uint32_t prevLevel = level;
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey != k[level]) {
                    nonMatchingKey = curKey;
                    if (n->getPrefixLength() > maxStoredPrefixLength) {
                        if (i < maxStoredPrefixLength) {
                            loadKey(N::getAnyChildTid(n), kt);
                        }
                        for (uint32_t j = 0; j < std::min((n->getPrefixLength() - (level - prevLevel) - 1),
                                                          maxStoredPrefixLength); ++j) {
                            nonMatchingPrefix[j] = kt[level + j + 1];
                        }
                    } else {
                        for (uint32_t j = 0; j < n->getPrefixLength() - i - 1; ++j) {
                            nonMatchingPrefix[j] = n->getPrefix()[i + j + 1];
                        }
                    }
                    return CheckPrefixPessimisticResult::NoMatch;
                }
                ++level;
            }
        }
        return CheckPrefixPessimisticResult::Match;
    }

    typename Tree::PCCompareResults Tree::checkPrefixCompare(N *n, const Key &k, uint32_t &level,
                                                        LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t kLevel = (k.getKeyLen() > level) ? k[level] : 0;

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey < kLevel) {
                    return PCCompareResults::Smaller;
                } else if (curKey > kLevel) {
                    return PCCompareResults::Bigger;
                }
                ++level;
            }
        }
        return PCCompareResults::Equal;
    }

    typename Tree::PCEqualsResults Tree::checkPrefixEquals(N *n, uint32_t &level, const Key &start, const Key &end,
                                                      LoadKeyFunction loadKey) {
        if (n->hasPrefix()) {
            bool endMatches = true;
            Key kt;
            for (uint32_t i = 0; i < n->getPrefixLength(); ++i) {
                if (i == maxStoredPrefixLength) {
                    loadKey(N::getAnyChildTid(n), kt);
                }
                uint8_t startLevel = (start.getKeyLen() > level) ? start[level] : 0;
                uint8_t endLevel = (end.getKeyLen() > level) ? end[level] : 0;

                uint8_t curKey = i >= maxStoredPrefixLength ? kt[level] : n->getPrefix()[i];
                if (curKey > startLevel && curKey < endLevel) {
                    return PCEqualsResults::Contained;
                } else if (curKey < startLevel || curKey > endLevel) {
                    return PCEqualsResults::NoMatch;
                } else if (curKey != endLevel) {
                    endMatches = false;
                }
                ++level;
            }
            if (!endMatches) {
                return PCEqualsResults::StartMatch;
            }
        }
        return PCEqualsResults::BothMatch;
    }
}
