#ifndef BTREE_H
#define BTREE_H

#include <iostream>
#include <climits>
#include <queue>
#include <string>
#include <utility>   // for std::pair
#include "node.h"

const int DEFAULT_MIN_DEGREE = 2;
const int DEFAULT_MAX_HEIGHT = INT_MAX;
bool DEBUG = false;

template <class T>
class BTree {
public:
	BTree() : min_degree(DEFAULT_MIN_DEGREE), height(1), max_height(DEFAULT_MAX_HEIGHT) {
		root = new Node<T>(min_degree);
		Element<T> *sentinel = new Element<T>();
		sentinel->next = sentinel;
		sentinel->prev = sentinel;
		head = sentinel;
	}
	BTree(int min_deg, int max_hght = INT_MAX) : min_degree(min_deg), height(1), max_height(max_hght) {
		root = new Node<T>(min_degree);
		Element<T> *sentinel = new Element<T>();
		sentinel->next = sentinel;
		sentinel->prev = sentinel;
		head = sentinel;
	}
	~BTree();
	void destroy_tree(Node<T> *node);
	std::pair<Node<T>&, int> search(T val);
	int insert(T val);
	void insert_lru(T val);
	bool remove(T val); // returns whether val was found in the tree
	T remove_lru(); // remove the element at the back of the linked list
	T remove_mru(); // remove element at the beginning of the linked list
	int get_height();
	int get_max_height();
	bool is_empty();
	int size();
	std::string to_string();
	std::string print_ordered_mru();
	std::string print_ordered_tail();
private:
	Node<T> *root;
	int min_degree; // each node has (m_d-1) to (2*m_d-1) keys
	int height;
	int max_height;
	int size_;
	Element<T> *head;
	std::pair<Node<T>&, int> search_node(Node<T> &node, T val); // TODO implement iteratively. currently recursive
	void split_child(Node<T> *node, int index); // TODO when to pass shared_ptr?
	int insert_nonfull(Node<T> *node, T element);
	bool remove_helper(Node<T> *node, T val, bool modify_linked_list, Element<T> *new_pos); // returns whether val was found in the tree 
																							// new_pos is the new position of the removed element
	Element<T>* find_max_key(Node<T> *node); // returns the max key in the subtree rooted at node
	Element<T>* find_min_key(Node<T> *node); // returns the min key in the subtree rooted at node
};

template <class T>
BTree<T>::~BTree() {
	destroy_tree(root);
	delete head;
}

template <class T>
void BTree<T>::destroy_tree(Node<T> *node) {
	if (!node->is_leaf) {
		for (int i = 0; i <= node->num_keys; ++i) {
			destroy_tree(node->children[i]);
		}
	}
}

template <class T>
std::pair<Node<T>&, int> BTree<T>::search(T val) {
	return search_node(*root, val);
}

template <class T>
std::pair<Node<T>&, int> BTree<T>::search_node(Node<T> &node, T val) {
	int i = 0;
	while (i<node.num_keys && val>node.keys[i].key) {
		i++;
	}
	if (i < node.num_keys && val == node.keys[i].key) {
		//std::pair<Node<T>&, int> ans = std::make_pair(*node, i);

		// -------- see if the following can be done without typedef (and in else if)

		typedef std::pair<Node<T>&, int> node_index;
		return node_index(node, i);

		//return ans;
	}
	else if (node.is_leaf) {
		//std::pair<Node<T>&, int> ans(*node, -1);
		typedef std::pair<Node<T>&, int> node_index;
		return node_index(node, -1);
		//return ans; // returned index is -1 when element not found
	}
	else {
		return search_node(*node.children[i], val);
	}
}

template <class T>
int BTree<T>::insert(T val) {
	int levels_traversed = 0;
	if (root->num_keys == min_degree * 2 - 1) {

		//TODO new_root can be deleted, operation can be done on root after being copied to child node
		Node<T> *new_root = new Node<T>(min_degree);
		Node<T> *child_ptr = root;

		new_root->is_leaf = false;
		//new_root->children.push_back(child_ptr);
		new_root->children[0] = child_ptr;

		root = new_root;
		height++;
		//child_ptr->parent = root;
		split_child(root, 0);
		levels_traversed = insert_nonfull(root, val);
	}
	else {
		levels_traversed = insert_nonfull(root, val);
	}

	size_++;

	return levels_traversed;
}

template <class T>
void BTree<T>::insert_lru(T val) { // insert and add element to the back of the linked list
	insert(val);
	Element<T> *element = head->next;
	head->next = head->next->next;
	head->next->prev = head;
	head->prev->next = element;
	element->prev = head->prev;
	head->prev = element;
	element->next = head;

}

template <class T>
void BTree<T>::split_child(Node<T> *node, int index) {
	//std::shared_ptr<Node<T>> child1 = node->children[index];
	Node<T> *child1 = node->children[index];
	Node<T> *child2 = new Node<T>(min_degree);
	//child2->parent = node;
	child2->is_leaf = child1->is_leaf;
	child2->num_keys = min_degree - 1;

	for (int i = child2->num_keys - 1; i >= 0; --i) {
		//child2->keys[i] = child1->keys[min_degree + i];
		child2->keys[i] = child1->keys[min_degree + i];
		child2->keys[i].next->prev = &(child2->keys[i]);
		child2->keys[i].prev->next = &(child2->keys[i]);
	}


	// delete moved keys from child1
	//for (int i = 0; i < child2->num_keys; ++i) {
	//	child1->keys.pop_back();
	//}

	if (!child1->is_leaf) {
		for (int i = 0; i < min_degree; ++i) {
			//child2->children.push_back(child1->children[min_degree + i]);
			child2->children[i] = child1->children[min_degree + i];

			//child2->children[i]->parent = child2;
		}
		//delete moved children from child1
		//for (int j = 0; j < min_degree; ++j) {
		//	child1->children.pop_back();
		//}
	}


	child1->num_keys = min_degree - 1;

	//node->children.insert((node->children.begin() + index + 1), child2);
	for (int i = node->num_keys; i >= index + 1; i--) {
		node->children[i + 1] = node->children[i];
	}
	node->children[index + 1] = child2;

	//child2->parent = node;


	//node->keys.insert(node->keys.begin() + index, child1->keys[min_degree - 1]);
	for (int i = node->num_keys - 1; i >= index; i--) {
		node->keys[i + 1] = node->keys[i];
		node->keys[i + 1].prev->next = &(node->keys[i + 1]);
		node->keys[i + 1].next->prev = &(node->keys[i + 1]);
	}
	node->keys[index] = child1->keys[min_degree - 1];
	node->keys[index].prev->next = &(node->keys[index]);
	node->keys[index].next->prev = &(node->keys[index]);

	//child1->keys.pop_back();
	node->num_keys++;


}

template <class T>
int BTree<T>::insert_nonfull(Node<T> *node, T element) {

	int i = node->num_keys - 1;
	while (i >= 0 && element < node->keys[i].key) {
		i--;
	}
	i++;
	if (node->is_leaf) {

		for (int j = node->num_keys; j > i; j--) {
			node->keys[j] = node->keys[j - 1];
			node->keys[j].next->prev = &(node->keys[0]) + j;
			node->keys[j].prev->next = &(node->keys[0]) + j;

		}

		//node->keys.insert(node->keys.begin() + i, Element<T>(element, head, head->next));
		node->keys[i] = Element<T>(element, head, head->next);
		head->next->prev = &(node->keys[i]);
		head->next = &(node->keys[i]);

		node->num_keys++;


		//node.keys[i] = element;

		return 1;
	}
	else { // node is not leaf
		Node<T> *child = node->children[i];
		if (child->num_keys == min_degree * 2 - 1) {
			split_child(node, i);
			if (element > node->keys[i].key) {
				i++;
			}
		}
		return 1 + insert_nonfull(node->children[i], element);
	}
}

template <class T>
bool BTree<T>::remove(T value) {
	return remove_helper(root, value, true, nullptr);
	size_--;
}

template <class T>
bool BTree<T>::remove_helper(Node<T> *node, T val, bool modify_linked_list, Element<T> *new_pos) { // modify only leaves? for now

	int i = 0;
	while (i < node->num_keys && val > node->keys[i].key) {
		i++;
	}

	if (node->is_leaf) {
		if (i < node->num_keys && val == node->keys[i].key) {
			Element<T> *k = &(node->keys[i]);

			if (modify_linked_list) {
				k->next->prev = k->prev;
				k->prev->next = k->next;
			}
			else {
				new_pos->prev = k->prev;
				new_pos->next = k->next;
				k->prev->next = new_pos;
				k->next->prev = new_pos;

			}

			//node->keys.erase(node->keys.begin() + i);
			for (int j = i; j < node->num_keys - 1; j++) {
				node->keys[j] = node->keys[j + 1];
				node->keys[j].next->prev = &(node->keys[j]);
				node->keys[j].prev->next = &(node->keys[j]);
			}
			node->num_keys--;

			return true;
		}
		else {
			return false;
		}
	}
	else if (i < node->num_keys && val == node->keys[i].key) { // val found in internal node

		if (node->children[i]->num_keys >= min_degree) { // 2a

			node->keys[i].next->prev = node->keys[i].prev;
			node->keys[i].prev->next = node->keys[i].next;

			Element<T> *predecessor = find_max_key(node->children[i]);
			Element<T> *pred_copy = &(Element<T>());
			*pred_copy = *predecessor;

			//pred_copy->next->prev = &(node->keys[i]);
			//pred_copy->prev->next = &(node->keys[i]);

			remove_helper(root, predecessor->key, false, &(node->keys[i])); // need a remove without modifying maybe
			node->keys[i].key = pred_copy->key;


			//pred_copy->next->prev = &(node->keys[i]);
			//pred_copy->prev->next = &(node->keys[i]);

			size_++;


		}
		else if (node->children[i + 1]->num_keys >= min_degree) { //2b

			node->keys[i].next->prev = node->keys[i].prev;
			node->keys[i].prev->next = node->keys[i].next;

			Element<T> *successor = find_min_key(node->children[i + 1]);
			Element<T> *succ_copy = &(Element<T>());
			*succ_copy = *successor;

			//succ_copy->next->prev = &(node->keys[i]);
			//succ_copy->prev->next = &(node->keys[i]);

			remove_helper(root, successor->key, false, &(node->keys[i]));
			node->keys[i].key = succ_copy->key;
			//succ_copy->next->prev = &(node->keys[i]);
			//succ_copy->prev->next = &(node->keys[i]);
			size_++;

		}
		else {

			Node<T> *left_child = node->children[i];
			Node<T> *right_child = node->children[i + 1];

			// merge right child to left child
			//left_child->keys.push_back(node->keys[i]);
			left_child->keys[min_degree - 1] = node->keys[i];
			left_child->keys[min_degree - 1].prev->next = &(left_child->keys[min_degree - 1]);
			left_child->keys[min_degree - 1].next->prev = &(left_child->keys[min_degree - 1]);

			for (int j = 0; j < right_child->num_keys; ++j) {
				//left_child->keys.push_back(right_child->keys[j]);
				int index = j + min_degree;
				left_child->keys[index] = right_child->keys[j];
				left_child->keys[index].next->prev = &(left_child->keys[index]);
				left_child->keys[index].prev->next = &(left_child->keys[index]);

				//if (!right_child->is_leaf) {
				//left_child->children.push_back(right_child->children[j]);
				//left_child->children[index] = right_child->children[j];

				//}
			}
			if (!right_child->is_leaf) {
				//left_child->children.push_back(right_child->children.back());
				//left_child->children[min_degree * 2 - 1] = right_child->children[min_degree - 1];
				for (int j = 0; j <= right_child->num_keys; ++j) {
					left_child->children[min_degree + j] = right_child->children[j];
				}
			}


			left_child->num_keys += (right_child->num_keys + 1);

			//node->keys.erase(node->keys.begin() + i);
			//node->children.erase(node->children.begin() + i + 1);
			for (int j = i + 1; j < node->num_keys; j++) {
				node->keys[j - 1] = node->keys[j];
				node->keys[j - 1].next->prev = &(node->keys[j - 1]);
				node->keys[j - 1].prev->next = &(node->keys[j - 1]);

				//node->children[j] = node->children[j + 1];
			}

			for (int j = i + 1; j < node->num_keys; ++j) {
				node->children[j] = node->children[j + 1];
			}
			//node->children[min_degree * 2 - 2] = node->children[min_degree * 2 - 1];

			node->num_keys--;

			// TODO how to free right child
			//right_child->parent = NULL;
			delete right_child;

			if (node == root && node->num_keys == 0) {
				root = left_child;
				height--;
			}

			if (DEBUG) {
				std::cout << to_string() << std::endl;
			}
			remove_helper(left_child, val, modify_linked_list, new_pos);

		}

		return true;

	}
	else { // val not found in internal node
		if (node->children[i]->num_keys == min_degree - 1) {

			// if one of its siblings has at least t keys 
			if (i > 0 && node->children[i - 1]->num_keys >= min_degree) {

				Node<T> *left_child = node->children[i - 1];
				Node<T> *child = node->children[i];

				//child->keys.insert(child->keys.begin(), node->keys[i - 1]);
				for (int j = child->num_keys; j>0; j--) {
					child->keys[j] = child->keys[j - 1];
					child->keys[j].next->prev = &(child->keys[j]);
					child->keys[j].prev->next = &(child->keys[j]);
				}
				child->keys[0] = node->keys[i - 1];
				child->keys[0].next->prev = &(child->keys[0]);
				child->keys[0].prev->next = &(child->keys[0]);

				//node->keys[i - 1] = left_child->keys.back(); // set key to last element of left child 
				node->keys[i - 1] = left_child->keys[left_child->num_keys - 1];
				node->keys[i - 1].next->prev = &(node->keys[i - 1]);
				node->keys[i - 1].prev->next = &(node->keys[i - 1]);

				//left_child->keys.pop_back(); // pop last element of left child

				// move the left node's rightmost child pointer to the beginning of the right child node's children vector
				if (!child->is_leaf) {
					//child->children.insert(child->children.begin(), left_child->children.back());
					for (int j = child->num_keys; j >= 0; j--) {
						child->children[j + 1] = child->children[j];
					}
					child->children[0] = left_child->children[left_child->num_keys];

					//left_child->children.pop_back();

				}

				left_child->num_keys--;
				child->num_keys++;

			}
			else if (i < node->num_keys && node->children[i + 1]->num_keys >= min_degree) {
				Node<T> *child = node->children[i];
				Node<T> *right_child = node->children[i + 1];

				// move right child's first key to node, and push node key at index i to back of left child
				//child->keys.push_back(node->keys[i]);
				child->keys[child->num_keys] = node->keys[i];
				child->keys[child->num_keys].next->prev = &(child->keys[child->num_keys]);
				child->keys[child->num_keys].prev->next = &(child->keys[child->num_keys]);

				node->keys[i] = right_child->keys[0];
				node->keys[i].next->prev = &(node->keys[i]);
				node->keys[i].prev->next = &(node->keys[i]);

				//right_child->keys.erase(right_child->keys.begin());
				for (int j = 0; j<right_child->num_keys - 1; ++j) {
					right_child->keys[j] = right_child->keys[j + 1];
					right_child->keys[j].next->prev = &(right_child->keys[j]);
					right_child->keys[j].prev->next = &(right_child->keys[j]);
				}

				// move right child's first child pointer to be left child's last child pointer
				if (!child->is_leaf) {
					//child->children.push_back(right_child->children[0]);
					child->children[child->num_keys + 1] = right_child->children[0];

					//right_child->children.erase(right_child->children.begin());
					for (int j = 0; j < right_child->num_keys; ++j) {
						right_child->children[j] = right_child->children[j + 1];
					}
				}

				child->num_keys++;
				right_child->num_keys--;
			}
			else { // both siblings have t-1 keys
				if (i < node->num_keys) {
					Node<T> *child = node->children[i];
					Node<T> *right_child = node->children[i + 1]; // child to be merged

																  //child->keys.push_back(node->keys[i]);
					child->keys[child->num_keys] = node->keys[i];
					child->keys[child->num_keys].next->prev = &(child->keys[child->num_keys]);
					child->keys[child->num_keys].prev->next = &(child->keys[child->num_keys]);

					for (int j = 0; j < right_child->num_keys; ++j) {
						//child->keys.push_back(right_child->keys[j]);
						child->keys[min_degree + j] = right_child->keys[j];
						child->keys[min_degree + j].next->prev = &(child->keys[min_degree + j]);
						child->keys[min_degree + j].prev->next = &(child->keys[min_degree + j]);

						if (!right_child->is_leaf) {
							//child->children.push_back(right_child->children[j]);
							child->children[min_degree + j] = right_child->children[j];
						}
					}
					if (!right_child->is_leaf) {
						//child->children.push_back(right_child->children.back());
						child->children[min_degree * 2 - 1] = right_child->children[min_degree - 1];
					}

					// does this free all memory used by right child?
					//right_child->parent = NULL;
					delete right_child;

					//node->keys.erase(node->keys.begin() + i);
					for (int j = i; j<node->num_keys - 1; ++j) {
						node->keys[j] = node->keys[j + 1];
						node->keys[j].next->prev = &(node->keys[j]);
						node->keys[j].prev->next = &(node->keys[j]);
					}

					//node->children.erase(node->children.begin() + i + 1);
					for (int j = i + 1; j<node->num_keys; ++j) {
						node->children[j] = node->children[j + 1];
					}

					node->num_keys--;
					child->num_keys = 2 * min_degree - 1;

					if (node == root && node->num_keys == 0) {
						root = child;
						height--;
					}
				}
				else { // i = node.num_keys
					Node<T> *left_child = node->children[i - 1];
					Node<T> *right_child = node->children[i];

					//left_child->keys.push_back(node->keys.back());
					left_child->keys[left_child->num_keys] = node->keys[node->num_keys - 1];
					left_child->keys[left_child->num_keys].next->prev = &(left_child->keys[left_child->num_keys]);
					left_child->keys[left_child->num_keys].prev->next = &(left_child->keys[left_child->num_keys]);

					for (int j = 0; j < right_child->num_keys; ++j) {
						//left_child->keys.push_back(right_child->keys[j]);
						int index = min_degree + j;
						left_child->keys[index] = right_child->keys[j];
						left_child->keys[index].next->prev = &(left_child->keys[index]);
						left_child->keys[index].prev->next = &(left_child->keys[index]);

						if (!right_child->is_leaf) {
							//left_child->children.push_back(right_child->children[j]);
							left_child->children[index] = right_child->children[j];
						}
					}
					if (!left_child->is_leaf) {
						//left_child->children.push_back(right_child->children.back());
						left_child->children[min_degree * 2 - 1] = right_child->children[min_degree - 1];
					}
					//right_child->parent = NULL;
					delete right_child;

					//node->keys.erase(node->keys.begin() + i - 1);
					for (int j = i - 1; j<node->num_keys - 1; ++j) {
						node->keys[j] = node->keys[j + 1];
						node->keys[j].next->prev = &(node->keys[j]);
						node->keys[j].prev->next = &(node->keys[j]);
					}

					//node->children.erase(node->children.begin() + i);
					for (int j = i; j<node->num_keys; ++j) {
						node->children[j] = node->children[j + 1];
					}

					node->num_keys--;
					i--;
					left_child->num_keys = 2 * min_degree - 1;

					if (node == root && node->num_keys == 0) {
						root = left_child;
						height--;
					}
				}
			}

		}

		if (DEBUG) {
			//std::cout << node_to_string(*(node->children[i])) << std::endl;
			std::cout << to_string() << std::endl;
		}
		// recurse
		return remove_helper(node->children[i], val, modify_linked_list, new_pos);
	}
}

template <class T>
Element<T>* BTree<T>::find_max_key(Node<T> *node) { //recursive. can be implemented iteratively
	if (node->is_leaf) {
		return &node->keys[node->num_keys - 1];
	}
	else {
		return find_max_key(node->children[node->num_keys]);
	}
}

template <class T>
Element<T>* BTree<T>::find_min_key(Node<T> *node) {
	if (node->is_leaf) {
		return &node->keys[0];
	}
	else {
		return find_min_key(node->children[0]);
	}
}

template <class T>
int BTree<T>::get_height() {
	return height;
}

template <class T>
int BTree<T>::get_max_height() {
	return max_height;
}

template <class T>
bool BTree<T>::is_empty() {
	return (root->num_keys == 0);
}

template <class T>
int BTree<T>::size() {
	return size_;
}

template <class T>
std::string BTree<T>::to_string() {
	// reference: https://codereview.stackexchange.com/questions/35656/printing-out-a-binary-tree-level-by-level
	std::string str = "";
	if (&root == NULL) {
		return str;
	}
	Node<T> *n_ptr = root;
	int level = 0;

	// Use a queue for breadth-first traversal of the tree.  The pair is
	// to keep track of the depth of each node.  (Depth of root node is 1.)
	typedef std::pair<Node<T> *, int> node_level;
	std::queue<node_level> q;
	q.push(node_level(n_ptr, 1));

	while (!q.empty()) {
		node_level nl = q.front();
		q.pop();
		if (nullptr != (n_ptr = nl.first)) {
			if (level != nl.second) {
				str += "\nLevel " + std::to_string(nl.second) + ": ";
				level = nl.second;
			}
			str += node_to_string(*n_ptr);
			if (!n_ptr->is_leaf) {
				for (int i = 0; i <= n_ptr->num_keys; ++i) {
					q.push(node_level(n_ptr->children[i], level + 1));
				}
			}
		}
	}

	return str;
}

template <class T>
T BTree<T>::remove_lru() {
	if (head->next == head) {
		return NULL;
	}
	T lru = head->prev->key;
	remove(lru);
	return lru;
}

template <class T>
T BTree<T>::remove_mru() {
	if (head->next == head) {
		return NULL;
	}
	T mru = head->next->key;
	remove(mru);
	return mru;
}

template <class T>
std::string BTree<T>::print_ordered_mru() {
	/*
	std::shared_ptr<Element<T>> elmt = head;
	std::string str = "MRU-> ";
	while (elmt != nullptr) {
	str += key_to_string(*elmt) + " ";
	elmt = elmt->next;
	}
	str += " <-LRU";
	return str;
	*/

	Element<T> *elmt = head->next;
	std::string str = "MRU-> ";
	while (elmt != head) {
		str += key_to_string(*elmt) + " ";
		elmt = elmt->next;
	}
	str += " <-LRU";
	return str;

}

template <class T>
std::string BTree<T>::print_ordered_tail() {
	/*
	std::shared_ptr<Element<T>> elmt = tail;
	std::string str = "(tail) LRU-> ";
	while (elmt != nullptr) {
	str += key_to_string(*elmt) + " ";
	elmt = elmt->prev;
	}
	str += " <-MRU";
	return str;
	*/

	Element<T> *elmt = head->prev;
	std::string str = "(tail) LRU-> ";
	while (elmt != head) {
		str += key_to_string(*elmt) + " ";
		elmt = elmt->prev;
	}
	str += " <-MRU";
	return str;

}

#endif /* BTREE_H */