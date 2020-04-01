#pragma once

#include "Node.h"

/* Tree
   ----
   Simply stores a pointer to the root of the tree
   */

class Tree {
public:

    Tree();

    Tree(Node* first);

    ~Tree();

    /* Variables */

    Node* first = nullptr;

};




