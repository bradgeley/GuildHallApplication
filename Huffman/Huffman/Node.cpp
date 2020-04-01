#include "Node.h"

Node::Node(unsigned char ch, size_t freq) :
    ch(ch), freq(freq) {
    isLeaf = true;
}

/* Creating a node with a right/left pointer means
   it is a parent Node and thus not a Leaf */

Node::Node(unsigned int freq, Node* left, Node* right) :
    freq(freq), left(left), right(right) {
    isLeaf = false;
}

Node::Node(Node* n) {
    isLeaf = n->isLeaf;
    ch = n->ch;
    freq = n->freq;
    left = n->left;
    right = n->right;
}

/* Variables */

bool isLeaf = false;
unsigned char ch = 0;
unsigned int freq = 0;
Node* left = nullptr;
Node* right = nullptr;

/* Operators for sorting function */

bool Node::operator > (Node const& n) { return freq > n.freq; }
bool Node::operator < (Node const& n) { return freq < n.freq; }
bool Node::operator >= (Node const& n) { return freq >= n.freq; }
bool Node::operator <= (Node const& n) { return freq <= n.freq; }

/* Functions */

/* destroyNode
   -----------
   Destroys all child nodes before self.
   */

void Node::destroyNode(Node* node) {
    if (node->isLeaf) {
        delete node;
    }
    else {
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }
}