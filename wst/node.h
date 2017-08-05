#ifndef NODE_H
#define NODE_H

//#include <vector>
//
//template <class T>
//class Node {
//public:
//    bool contains(T val);
//    
//private:
//    int num_keys;
//    bool is_leaf;
//    std::vector<T> keys;
//    std::vector<T*> children;
//};

#include <memory>
#include <vector>

const int DEFAULT_MIN_DEGR = 2;

template <class T>
struct Element {
	T key;
	//Element *prev;
	//Element *next;
	Element<T> *prev;
	Element<T> *next;

	Element() {
		key = NULL;
		prev = nullptr;
		next = nullptr;
	}

	Element(T k, Element *p, Element *n) {
		key = k;
		prev = p;
		next = n;
	}
	//~Element() {
	//	delete prev;
	//	delete next;
	//}
};

template <class T>
std::string key_to_string(const Element<T> &element) {
	std::string str = "#";
	str += std::to_string(element.key) + "," + std::to_string(element.next->key) + "," + std::to_string(element.prev->key) + "#";
	return str;
}

template <class T>
struct Node {
	int num_keys;
	bool is_leaf;
	int min_degree;
	//Node<T> *parent; // shared_ptr????
	std::vector<Element<T>> keys;
	//Element<T> *keys;
	//Node<T> **children;
	std::vector<Node<T>*> children;

	Node() : num_keys(0), is_leaf(true), min_degree(DEFAULT_MIN_DEGR) {
		//parent = nullptr;
		//keys = new Element<T>[min_degree * 2 - 1];
		//children = new Node<T>*[min_degree * 2];
		keys.resize(min_degree * 2 - 1);
		children.resize(min_degree * 2);
	}

	Node(int md) : num_keys(0), is_leaf(true), min_degree(md) {
		//parent = nullptr;
		//keys = new Element<T>[min_degree * 2 - 1];
		//children = new Node<T>*[min_degree * 2];
		keys.resize(min_degree * 2 - 1);
		children.resize(min_degree * 2);
	}
	/*
	~Node() {
	if (!is_leaf) {
	for (int i = 0; i <= num_keys; ++i) {
	delete children[i];
	}
	}
	}
	*/


};


template <class T>
std::string node_to_string(const Node<T> &node) {
	std::string str = "( ";
	str += "*" + std::to_string(node.num_keys) + "," + std::to_string(node.keys.size()) + "," + std::to_string(node.children.size()) + "," + std::to_string(node.is_leaf) + "* ";
	for (int i = 0; i < node.num_keys; ++i) {
		str += key_to_string(node.keys[i]) + " ";
	}
	return str + ")";
}
#endif /* NODE_H */
