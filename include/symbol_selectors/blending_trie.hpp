#ifndef BLENDING_TRI_H
#define BLENDING_TRI_H

#include<map>
#include<list>
#include<vector>

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
		prefix = new_prefix;
	}
	
	void addChild(char key, TrieNode *child) {
		children.insert(std::make_pair(key, child));
	}

	std::map<char, TrieNode*>::iterator getChild(char key) {
		return children.find(key);
	}

	std::map<char, TrieNode*>::iterator getBegin() {
		return children.begin();
	}

	std::map<char, TrieNode*>::iterator getEnd() {
		return children.end();
	}

	bool hasChildren() {
		return children.size() > 0;
	}

private:
	int freq_;
	std::string prefix = std::string();
	std::map<char, TrieNode*> children = {};
};


class BlendTrie {
public:
	BlendTrie();
	~BlendTrie() {delete root_;};
	void build(const std::map<std::string, int64_t>& freq_map);
	void insert(std::string key, int64_t freq);
	void blendingAndGetLeaves(std::vector<SymbolFreq>& freq_vec);

private:
	TrieNode *root_;

};

BlendTrie::BlendTrie() {
	root_ = nullptr;
}

void BlendTrie::build(const std::map<std::string, int64_t>& freq_map) {
	root_ = new TrieNode();
	for (auto iter = freq_map.begin(); iter != freq_map.end(); iter++) {
		insert(iter->first, iter->second);
	}
}

void BlendTrie::insert(std::string key, int64_t freq) {
	TrieNode *node = root_;
	for (int i=0; i < key.size(); i++) {
		std::map<char, TrieNode*>::iterator child = node->getChild(key[i]);
		if (child != node->getEnd()) {
			node = child->second;
		} else {
			TrieNode *new_node = new TrieNode();
			node->addChild(key[i], new_node);
			node = new_node;
		}
		node->setFreq(freq);
	}
}

void BlendTrie::blendingAndGetLeaves(std::vector<SymbolFreq>& freq_vec) {
	TrieNode *node = root_;
	std::list<TrieNode *> l;
	l.push_back(node);
	while (!l.empty()) {
		TrieNode *top_node = l.front();
		l.pop_front();
		TrieNode *high_freq_child = nullptr;
		int64_t high_freq = -1;
		for(auto iter = node->getBegin(); iter != node->getEnd(); iter++) {
			l.push_front(iter->second);
			iter->second->setPrefix(top_node->getPrefix() + std::string(1, iter->first));
			if(iter->second->getFreq() > high_freq) {
				high_freq = iter->second->getFreq();
				high_freq_child = iter->second;
			}
		}
		if (node->hasChildren()) {
			high_freq_child->setFreq(high_freq_child->getFreq()+top_node->getFreq());
			top_node->setFreq(0);
		} else {
			freq_vec.push_back(std::make_pair(top_node->getPrefix(), top_node->getFreq()));
		}
	}
}

} // namespace ope

#endif