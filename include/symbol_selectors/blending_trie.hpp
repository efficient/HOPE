#include <utility>

#include <utility>

#ifndef BLENDING_TRI_H
#define BLENDING_TRI_H

#include<map>
#include <unordered_map>
#include<list>
#include<vector>
#include<iostream>
#include<fstream>

#include "common.hpp"

namespace ope {


    class TrieNode {
    public:
        TrieNode() {
            freq_ = 0;
        }

        int64_t getFreq() {
            return freq_;
        }

        void setFreq(int64_t new_freq) {
            freq_ = new_freq;
        }

        std::string getPrefix() {
            return prefix;
        }

        void setPrefix(std::string new_prefix) {
            prefix = std::move(new_prefix);
        }

        void addChild(char key, TrieNode *child) {
            children.insert(std::make_pair(key, child));
        }

        std::map<char, TrieNode *>::iterator getChild(char key) {
            return children.find(key);
        }

        std::map<char, TrieNode *>::iterator getBegin() {
            return children.begin();
        }

        std::map<char, TrieNode *>::iterator getEnd() {
            return children.end();
        }

        bool hasChildren() {
            return !children.empty();
        }

        std::map<char, TrieNode *> getChildren() {
            return children;
        }

    private:
        int64_t freq_;
        std::string prefix = std::string();
        std::map<char, TrieNode *> children = {};
    };


    class BlendTrie {
    public:
        BlendTrie(int _blend_type = 1);

        ~BlendTrie();

        void build(std::vector<std::string> key_list);
//        void build(const std::unordered_map<std::string, int64_t> &freq_map);

        void insert(std::string &key, int64_t freq);

        void clear(TrieNode *node);

        void blendingAndGetLeaves(std::vector<SymbolFreq> &freq_vec);

        void vis(const std::string &filename);

    private:
        TrieNode *root_;
        int blend_type;
    };

    BlendTrie::BlendTrie(int _blend_type) {
        root_ = nullptr;
        blend_type = _blend_type;
    }

    BlendTrie::~BlendTrie() {
        // Delete all the nodes
        clear(root_);
    }
/*
    void BlendTrie::build(std::vector<std::string> key_list) {
        root_ = new TrieNode();
        int maxkey_len = 50;
        for (int i = 0; i < (int)key_list.size(); i++) {
            std::string key = key_list[i];
            for (int j = 0; j < (int)key.size(); j++) {
                for (int k = 1; k <= (int)key.size() - j && k < maxkey_len; k++) {
                    std::string substring = key.substr(j, k);
                    //std::cout << substring << "*" << std::endl;
                    insert(substring, 1);
                }
            }
        }
    }
*/

   // Only calculate the frequency of suffix
   void BlendTrie::build(std::vector<std::string> key_list) {
        std::cout << "Key list size:" << key_list.size() << std::endl;
        root_ = new TrieNode();
        int maxkey_len = 50;
        if (blend_type == 0) {
            for (int i = 0; i < (int)key_list.size(); i++) {
                std::string key = key_list[i].substr(0, maxkey_len);
                for (int j = 0; j < (int)key.length(); j++) {
                    for (int k = 1; k <= (int)key.length() - j; k++) {
                        std::string substring = key.substr(j, k);
                        //std::cout << substring << "*" << std::endl;
                        insert(substring, 1);
                    }
                }
            }
            /*for (int i = 0; i < (int)key_list.size(); i++) {
                std::string key = key_list[i];
                for (int j = 0; j < (int)key.size(); j++) {
                    for (int k = 1; k <= (int)key.size() - j && k < maxkey_len; k++) {
                        std::string substring = key.substr(j, k);
                        //std::cout << substring << "*" << std::endl;
                        insert(substring, 1);
                    }
                }
            }*/
        } else if (blend_type == 1) {
            for (int i = 0; i < (int)key_list.size(); i++) {
                std::string key = key_list[i].substr(0, maxkey_len);
                int str_len = key.length();
                for (int j = 0; j < str_len; j++) {
                    std::string substring = key.substr(j, str_len - j);
                    insert(substring, 1);
                }
            }
        }
    }

    void BlendTrie::insert(std::string &key, int64_t freq) {
        TrieNode *node = root_;
        for (int i = 0; i < (int)key.length(); i++) {
            std::map<char, TrieNode *>::iterator child = node->getChild(key[i]);
            if (child != node->getEnd()) {
                node = child->second;
            } else {
                auto new_node = new TrieNode();
                node->addChild(key[i], new_node);
                node = new_node;
            }
        }
        node->setFreq(freq + node->getFreq());
    }

    void BlendTrie::blendingAndGetLeaves(std::vector<SymbolFreq> &freq_vec) {
        std::list<TrieNode *> l;
        l.push_back(root_);
        while (!l.empty()) {
            TrieNode *top_node = l.front();
            l.pop_front();
            TrieNode *high_freq_child = nullptr;
            int64_t high_freq = -1;
            for (auto iter = top_node->getBegin(); iter != top_node->getEnd(); iter++) {
                l.push_back(iter->second);
                iter->second->setPrefix(top_node->getPrefix() + std::string(1, iter->first));
                if (iter->second->getFreq() > high_freq) {
                    high_freq = iter->second->getFreq();
                    high_freq_child = iter->second;
                }
            }
            if (top_node->hasChildren()) {
                high_freq_child->setFreq(high_freq_child->getFreq() + top_node->getFreq());
                top_node->setFreq(0);
            } else {
                freq_vec.push_back(std::make_pair(top_node->getPrefix(), top_node->getFreq()));
            }
        }
    }

    void BlendTrie::clear(ope::TrieNode *node) {
        if(!node->hasChildren()) {
            delete node;
            return;
        }
        std::map<char, TrieNode*> children = node->getChildren();
        for(auto iter = children.begin(); iter != children.end(); iter++) {
            clear(iter->second);
        }
        delete node;
    }

    void BlendTrie::vis(const std::string &filename) {
        std::ofstream out;
        out.open(filename);
        out << "digraph G {" << std::endl;
        std::list<TrieNode *> l;
        std::map<TrieNode *, int> id;
        int cnt = 0;
        id.insert(std::make_pair(root_, cnt));
        l.push_back(root_);
        while (!l.empty()) {
            TrieNode *cur_node = l.front();
            l.pop_front();
            for (auto iter = cur_node->getBegin(); iter != cur_node->getEnd(); iter++) {
                cnt++;
                id.insert(std::make_pair(iter->second, cnt));
                l.push_back(iter->second);
                out << "\t";
                out << id.find(cur_node)->second << "->" << cnt << std::endl;
            }
        }
        for (auto &iter : id) {
            out << "\t" << iter.second << "[label=\"";
            out << iter.first->getPrefix() << "\"];" << std::endl;
        }
        out << "}" << std::endl;
        out.close();
    }

} // namespace ope

#endif
