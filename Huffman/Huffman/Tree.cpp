#include "Tree.h"

Tree::Tree() {}

Tree::Tree(Node* first) :
    first(first) {}

Tree::~Tree() {
    Node::destroyNode(first);
}