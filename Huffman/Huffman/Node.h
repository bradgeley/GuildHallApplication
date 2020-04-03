#pragma once
#include <cstdint>



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

    /* Constructors */

    Node(uint8_t ch, uint32_t freq);

    Node(uint32_t freq, Node* left, Node* right);

    Node(Node* n);


    /* Variables */

    bool isLeaf;
    uint8_t byte = 0;
    uint32_t freq = 0;
    Node* left = nullptr;
    Node* right = nullptr;


    /* Operators for sorting function */

    bool operator > (Node const& n);
    bool operator < (Node const& n);
    bool operator >= (Node const& n);
    bool operator <= (Node const& n);


    /* Methods */


    /* destroyNode
       -----------
       Destroys all child nodes before destroying self.
       */

    static void destroyNode(Node* node);
};

