#pragma once
#include <cstdint>
#include <fstream>

#include "FrequencyMap.h"
#include "Node.h"

using namespace std;



/* Tree
   ----
   Stores a pointer to the root of a binary tree. 
   
   The tree contains methods to write to and decode 
   itself from a file, which is necessary for Huffman
   compression.
*/



class Tree {
public:

    /* Sentintel Values for Writing/Reconstructing */

    static const uint8_t NODE = 0;
    static const uint8_t LEAF = 1;
    static const uint8_t DATA = 2;

    /* Constructors/Destructor */

    Tree(FrequencyMap freqMap);

    ~Tree();


    /* Variables */

    Node* root = nullptr;


    /* Public Interface */


    /* bitRep
       ------
       Stores the bit representation of a byte that has been
       found from traversing the binary tree, along with its length
       since we are returning a value that has a number of
       leading zeros.
    */

    struct bitRep {
        bitRep(uint32_t bits, uint32_t nBits) : bits(bits), nBits(nBits) {};

        uint32_t bits;
        uint32_t nBits;
    };


    /* writeTo
       -------
       Stores the tree into a file, depth first, post order,
       and installs a sentinel value to know when the tree
       ends.
    */

    void writeTo(ofstream& outfile);


    /* DecompressTree
       --------------
       Reconstructs the binary tree from the file, stopping
       at the sentinel value.
    */

    void decompressTree(ifstream& infile);


    /* findBitRepOfByte 
       ----------------
       Uses the tree to convert a byte to a (usually smaller) bit representation.
    */

    bitRep findBitRepOf(const uint8_t byte);


private:

    /* Private Methods */

    /* writeNode
       ---------
       Recursively writes each node in the tree to the outfile
       with a sentinel value telling if it is a leaf or a node.
    */

    void writeNode(ofstream& outfile, Node* node);


    /* createBinaryTree
       ----------------
       Creates the binary tree that we use to find unique bit values
       for each byte of data.
    */

    void createBinaryTree(FrequencyMap freqMap);


    /* findBitRepOfByteRecursive 
       -------------------------
       Updates the value of result when we reach a leaf that matches the
       desired character.
    */

    void findBitRepOfByteRecursive(const uint8_t byte, Node* node, bitRep temp, bitRep& result);

};




