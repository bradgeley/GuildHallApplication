#include <iostream>
#include <list>
#include <stack>

#include "Node.h"
#include "Tree.h"

using namespace std;



/* Tree
   ----
   Stores a pointer to the root of a binary tree. Also
   knows how to write to and decode itself from a file.
*/



/* Constructors/Destructor */

Tree::Tree() {}

Tree::Tree(FrequencyMap freqMap) {
    createBinaryTree(freqMap);
}

Tree::~Tree() {
    Node::destroyNode(root);
}



/* Compression Methods */

/* createBinaryTree
   ----------------
   Creates the binary tree that we use to find unique bit values
   for each byte of data.

   Takes a map of size 256 each character value (the index in the array) 
   to its frequency in the file. All of the higher frequency nodes will 
   be added higher in the tree. Lower frequency values are added first, 
   meaning they are deeper into the tree and will have longer bit representations.
   */

void Tree::createBinaryTree(FrequencyMap freqMap) {
    /* Iterate through the map and make a Node for each one */

    list<Node> nodes;
    for (size_t index = 0; index < freqMap.size(); index++) {
        uint8_t byte = (uint8_t)index;
        uint32_t freq = freqMap.getFreq(index);
        if (freq != 0) {
            nodes.push_back(Node(byte, freq));
        }
    }

    /* Binary Tree Creation Loop */

    while (nodes.size() > 1) {

        //Sort elements lowest to highest by frequency
        nodes.sort();

        //Take the 2 first, lowest frequency elements
        Node* first = new Node(nodes.front());
        nodes.pop_front();
        Node* second = new Node(nodes.front());
        nodes.pop_front();

        //Add back a new Node with the combined frequency of the children
        Node parent(first->freq + second->freq, first, second);
        nodes.push_back(parent);
    }

    /* Set the root of the tree */

    root = new Node(nodes.front());
    nodes.pop_front(); //now empty
}

/* writeTo
   -------
   Stores itself into a file, depth first, post order.
   Ends the written data with a sentinel value of 2 to
   indicate that the tree is finished.

   Serves as the wrapper for the recursive function writeNode.
*/

void Tree::writeTo(ofstream& outfile) {
    writeNode(outfile, root);
    outfile << DATA; //delimeter for knowing when the tree is over
}

/* writeNode
   ---------
   Recursively writes the tree depth first, post order
   into the open outfile.
*/

void Tree::writeNode(ofstream& outfile, Node* node) {
    if (node->isLeaf) {
        outfile << LEAF;
        outfile << node->byte;
    }
    else {
        writeNode(outfile, node->left);
        writeNode(outfile, node->right);
        outfile << NODE;
        outfile << NODE;
    }
}


/* findBitRepOfByteRecursive
   -------------------------
   Recursively traverses tree, keeping track of the resulting
   binary representation of the result, as well as the length
   in bits.

   When we traverse left, add a 0 to the bitRep and 1 to nBits.
   When we traverse right, add 1 to the bitRep and 1 to nBits.
   When we reach a leaf, return the values into &result.
*/

void Tree::findBitRepOfByteRecursive(const uint8_t byte, Node* node, Tree::bitRep temp, Tree::bitRep& result) {
    if (node->isLeaf) {
        if (node->byte == byte) {
            result = temp;
        }
    }
    else {
        findBitRepOfByteRecursive(byte, node->left, bitRep(temp.bits << 1, temp.nBits + 1), result);
        findBitRepOfByteRecursive(byte, node->right, bitRep((temp.bits << 1) + 1, temp.nBits + 1), result);
    }
}


/* findBitRepOfByte
   ----------------
   If the root is a leaf, this means that there is only
   one possible byte in the file.

   Therefore, we return the bitRep of 1 with a length of 1.

   In all other cases, traverse the tree and find the
   corresponding bitRep for that byte and return it.
*/

Tree::bitRep Tree::findBitRepOf(const uint8_t byte) {
    if (root->isLeaf) {
        return Tree::bitRep(1, 1);
    }
    else {
        Tree::bitRep result(0, 0);
        findBitRepOfByteRecursive(byte, root, bitRep(0, 0), result);
        return result;
    }
}



/* Decompression Methods */

/* DecompressTree
   --------------
   Reconstructs the binary tree from the file using a stack.
   If you read a leaf, put it on a stack, if you read a node, take 2
   children from the stack and put the node back on.

   This function is given a list that contains blocks of chars in this format:

   |sentinel| |data| |sentinel| |data| ... etc ...   |ending sentinel|

   For the sentinels:
   If we read a 0 (NODE), then we copy the data as a Node and ignore the char,
   If we read a 1 (LEAF), we copy the data as a Leaf and grab the char
   If we read a 2 (DATA), then the tree is finished and the data begins
*/

void Tree::decompressTree(ifstream& infile) {
    stack<Node*> s;
    char ch;
    uint8_t sentinel, byte;
    while (true) {

        //get the sentinel for the next Node
        infile.get(ch);
        sentinel = (uint8_t)ch;

        if (sentinel == LEAF) {
            //get the byte after the sentinel value
            infile.get(ch);
            byte = (uint8_t)ch;
            Node* leaf = new Node(byte, 0); //frequency, or the second parameter here, does not matter in this tree
            s.push(leaf);
        }
        else if (sentinel == NODE) {
            //get the next value which is not used.
            infile.get(ch);
            Node* right = s.top(); s.pop();
            Node* left = s.top(); s.pop();
            Node* node = new Node(0, left, right); //leaving freq empty
            s.push(node);
        }
        else if (sentinel == DATA) {
            break;
        }
    }
    root = s.top();
}
