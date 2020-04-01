#include "Node.h"

Node::Node(uint8_t ch, uint32_t freq) :
    byte(ch), freq(freq) {
    isLeaf = true;
}

/* Creating a node with a right/left pointer means
   it is a parent Node and thus not a Leaf */

Node::Node(uint32_t freq, Node* left, Node* right) :
    freq(freq), left(left), right(right) {
    isLeaf = false;
}

Node::Node(Node* n) {
    isLeaf = n->isLeaf;
    byte = n->byte;
    freq = n->freq;
    left = n->left;
    right = n->right;
}

/* Variables */

bool isLeaf = false;
uint8_t ch = 0;
uint32_t freq = 0;
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