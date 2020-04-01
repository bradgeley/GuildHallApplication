#pragma once


/* Node
   ----
   Nodes for the binary tree we are using are split into two types: Nodes and Leafs
   A node has two pointers to other Nodes or Leafs. A leaf's pointers are nullptr,
   and contains a character.

   I could have created subclasses for Nodes and Leafs, but I decided to use a boolean
   to determine which was which.
   */

class Node {
public:

    /* Creating a node with a character means it is a Leaf node */

    Node(unsigned char ch, size_t freq);

    /* Creating a node with a right/left pointer means
       it is a parent Node and thus not a Leaf */

    Node(unsigned int freq, Node* left, Node* right);

    Node(Node* n);

    /* Variables */

    bool isLeaf = false;
    unsigned char ch = 0;
    unsigned int freq = 0;
    Node* left = nullptr;
    Node* right = nullptr;

    /* Operators for sorting function */

    bool operator > (Node const& n);
    bool operator < (Node const& n);
    bool operator >= (Node const& n);
    bool operator <= (Node const& n);

    /* Functions */

    /* destroyNode
       -----------
       Destroys all child nodes before self.
       */

    static void destroyNode(Node* node);
};

