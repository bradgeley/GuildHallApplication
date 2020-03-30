// Huffman.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <map>
#include <list>
#include <bitset>
#include <stack>
#include <assert.h>
using namespace std;


/* Constants */

const string TEXT = "enwik8";
const string CMP = "Compressed.txt"; //See what I did there?
const string DECOMPRESSED = "Decompressed.txt";
const unsigned char LEAF = 1;
const unsigned char NODE = 0;


/* Custom Classes */

/* Node 
   ----
   Nodes for the binary tree we are using are split into two types: Nodes and Leafs
   A node has two pointers to other Nodes or Leafs. A leaf's pointers are nullptr,
   and contains a character.

   I could have created subclasses for Nodes and Leafs, but I simply used a boolean
   to determine which was which.
   */

class Node {
public:

    /* Creating a node with a character means it is a Leaf node */

    Node(char ch, size_t freq): 
        ch(ch), freq(freq) {
        isLeaf = true;
    }

    /* Creating a node with a right/left pointer means 
       it is a parent Node and thus not a Leaf */

    Node(size_t freq, Node* left, Node* right): 
        freq(freq), left(left), right(right) {
        isLeaf = false;
    }

    Node(Node* n) {
        isLeaf = n->isLeaf;
        ch = n->ch;
        freq = n->freq;
        left = n->left;
        right = n->right;
    }

    /* Variables */

    bool isLeaf = false;
    char ch = 0;
    size_t freq = 0;
    Node* left = nullptr;
    Node* right = nullptr;

    /* Operators for sorting function */
    bool operator > (Node const& n) { return freq > n.freq; }
    bool operator < (Node const& n) { return freq < n.freq; }
    bool operator >= (Node const& n) { return freq >= n.freq; }
    bool operator <= (Node const& n) { return freq <= n.freq; }
};

/* destroyNode 
   -----------
   Recursive function for destroying a tree of Nodes */

void destroyNode(Node* node) {
    if (node->isLeaf) {
        delete node;
    }
    else {
        destroyNode(node->left);
        destroyNode(node->right);
        delete node;
    }
}

class Tree {
public:

    Tree() {}

    Tree(Node* first):
        first(first) {}

    ~Tree() {
        destroyNode(first);
    }

    /* Variables */

    Node* first = nullptr;

    /* Tree.getLength() 
       ----------------
       Wrapper function for getLengthOfNode(Node* n)
       */

    unsigned char getLength() {
        return getLengthOfNode(first);
    }

private:

    /* Tree.getLength()
       ----------------
       Recursively adds the length of the branches of the binary tree's nodes
       */
    unsigned char getLengthOfNode(Node* n) {
        if (n == nullptr) return 0;
        unsigned char total = 1;
        if (first->left != nullptr) {
            total += getLengthOfNode(n->left);
        }
        if (first->right != nullptr) {
            total += getLengthOfNode(n->right);
        }
        return total;
    }
};





/* Binary Tree Functions */


/* createBinaryTree(Tree& t, map<char, size_t> m)
   ----------------------------------------------
   1. Creates the binary tree that we use to find unique bit values
   for each byte of data.
   */

void createBinaryTree(Tree& binaryTree, map<char, size_t>& freqMap) {

    //Iterate through the map and make a Node for each one
    list<Node> nodes;
    for (map<char, size_t>::iterator it = freqMap.begin(); it != freqMap.end(); ++it) {
        nodes.push_back(Node(it->first, it->second));
    }

    //Binary Tree Creation Loop:
    while (nodes.size() > 1) {

        //Sort elements lowest to highest
        nodes.sort();

        //Take the 2 first elements
        Node* first = new Node(nodes.front());
        nodes.pop_front();
        Node* second = new Node(nodes.front());
        nodes.pop_front();

        //Create new Node with these as its children, with combined frequency of children
        //Using address here is a stack address and will not be valid outside of this scope
        //So I need to do all of the tree operations in this function
        Node parent = new Node(first->freq + second->freq, first, second);

        //3. Add new Node back to the list
        nodes.push_back(parent);
    }
    binaryTree.first = new Node(nodes.front());
    nodes.pop_front();
}





/* Compression Functions */


/* getBinaryForChar 
   ----------------
   Recursively traverses tree, keeping track of the resulting
   binary representation of the result, as well as the length
   in bits.

   The bit representation of the number could end up being quite long. With
   an entirely inefficient binary tree, the 255 possible bytes could be spread
   out poorly such that it takes 64+ Nodes to reach one value. Is this necessary?
   So I use a 64-bit unsigned long long to store the potential bit representation.

   I store the potential bit representation in temp, and the potential size in nBitsTemp.
   When I get a match, I return those results into result and nBits, which are passed
   by reference.
   */

void getBinaryForChar(const char c, Node* node, unsigned long long bitRepTemp, unsigned long long& bitRepResult, unsigned int nBitsTemp, unsigned int& nBitsResult) {
    if (node->isLeaf) {
        if (node->ch == c) {
            bitRepResult = bitRepTemp;
            nBitsResult = nBitsTemp;
        }
    }
    else {
        getBinaryForChar(c, node->left, bitRepTemp << 1, bitRepResult, nBitsTemp + 1, nBitsResult);
        getBinaryForChar(c, node->right, (bitRepTemp << 1) + 1, bitRepResult, nBitsTemp + 1, nBitsResult);
    }
}


/* compressFile
   ------------
   Since the bit representation of each byte is of varying length,
   this is a quite complicated function. We step through the string of data,
   finding the binary representation and then filling the remainder of the current
   8-bit mask. 
   
   Once the 8-bit mask is full, it is ready to be pushed to the compressed file.
   At the end of the file there are anywhere between 0-7 trailing 0's.

   If the last mask ended with only one of the bits filled, the remaining 7 are trailing 0's
   and could be misread by the decompresser as another character. Thus we need to include
   the number of trailing 0's in the header to the compressed file.
   */

void compressFile(string data, Tree& freqTree, list<unsigned char>& compressed) {
    cout << "Compressing..." << endl;
    cout << "Bytes in file: " << data.length() << endl;

    unsigned char mask = 0b0;
    unsigned char filledBits = 0;

    for (size_t i = 0; i < data.length(); i++) {
        char currentChar = data[i];

        //Get the binary representation of the char and the length of its bits
        unsigned long long temp = 0, result = 0; //bits might be long due to a potentially large tree size
        unsigned int nBits = 0, nBitsTemp = 0;

        //Find the bit representation for the current character recursively using the binary tree
        if (!freqTree.first->isLeaf) {
            getBinaryForChar(currentChar, freqTree.first, temp, result, nBitsTemp, nBits);
        }
        else {
            //Special case where the root is a character. Only necessary in special case where the file is one repeated byte
            nBits = 1;
            result = 1;
        }

        //Add the bits to the current mask
        while (true) {

            if (filledBits == sizeof(mask) * CHAR_BIT) {
                //Create a new empty mask ready to go
                compressed.push_back(mask);
                mask = 0;
                filledBits = 0;
            }

            unsigned char unfilledBits = sizeof(mask) * CHAR_BIT - filledBits;

            if (nBits > unfilledBits) {

                //Shift result so that unfilledBits bits remain
                temp = result >> (nBits - unfilledBits);

                //Update mask
                mask = mask | (unsigned char)temp;
                filledBits += unfilledBits;

                //Update nBits. We filled all remaining bits
                //So the nBits needed to add will go down by that amount
                nBits -= unfilledBits;

                //Get rid of the bits from result 
                //that we just added to the mask
                result = result << (sizeof(result) * CHAR_BIT - nBits);
                result = result >> (sizeof(result) * CHAR_BIT - nBits);
            }
            else if (nBits <= unfilledBits) {

                //Shift the bits up so that they align in the mask
                temp = result << (unfilledBits - nBits);
                mask = mask | (unsigned char)temp;

                //Update filled bits to represent what we just filled
                filledBits += nBits;
                break;
            }
        }
    }
    compressed.push_back(mask);
    compressed.push_front((unsigned char) (8 - filledBits)); //tells me how many 0s are at the end
    cout << "Compressed file to: " << compressed.size() - 1 << " bytes (without header or tree)" << endl;
}




/* Decompression Functions */


void copyCompressedDataToList(ifstream& infile, list<unsigned char>& compressed) {
    char ch;
    unsigned int nBytesCopied = 0;
    while (infile.get(ch)) {
        compressed.push_back(ch);
        nBytesCopied++;
    }
}

/* Reconstruct the tree from the file.
   If you read a leaf, put it on a stack, if you read a node, take	2
   children from the stack and put the node back on.This should leave you
   just the root on the stack when you are done.  
   
   This function is given a list that contains blocks of chars in this format:

   | 0 or 1 |   |  data  |

   If we read a 0, then we copy the data as a Node,
   If we read a 1, we copy the data as a Leaf
   */

void decompressTree(list<unsigned char>& compressed, Tree& freqTree, unsigned char treeLength) {
    stack<Node*> s;
    int count = 0;
    for (list<unsigned char>::iterator it = compressed.begin(); count < treeLength; count++) {
        if (*it == LEAF) {
            it++;
            compressed.pop_front(); //pop the leaf sentinel
            Node* leaf = new Node(*it, 0); //frequency, or the second parameter here, does not matter in this tree
            s.push(leaf);
            it++;
            compressed.pop_front(); //pop char off the list
        }
        else {
            it++;
            compressed.pop_front(); //pop the node sentinel
            Node* right = s.top(); s.pop();
            Node* left = s.top(); s.pop();
            Node* node = new Node(0, left, right); //leaving freq empty
            s.push(node);
            it++;
            compressed.pop_front(); // pop the data off the list
        }
    }
    if (s.size() == 1) {
        freqTree.first = s.top();
    }
    if (s.size() == 2) {
        Node* right = s.top(); s.pop();
        Node* left = s.top(); s.pop();
        Node* node = new Node(0, left, right); //leaving freq empty
        s.push(node);
        freqTree.first = s.top();
    }
}

void decompressFile(ifstream& inf, Tree& freqTree, ofstream& of) {
    cout << "Decompressing..." << endl;

    inf.clear();
    inf.open(CMP, std::ios::binary);
    list<unsigned char> compressed;
    copyCompressedDataToList(inf, compressed);
    inf.close();

    of.open(DECOMPRESSED, std::ios::binary);

    //Gather header data
    unsigned char numZerosAtEnd = compressed.front();
    compressed.pop_front();
    unsigned char treeLength = compressed.front();
    compressed.pop_front();

    decompressTree(compressed, freqTree, treeLength);

    bool endOfFile = false; //bool for keeping track of when we have reached the last byte of compressed data

    if (freqTree.first->isLeaf) {
        //Handle special case of only one byte repeated once or many times
        int numRepeatedChars = compressed.size() * 8 - numZerosAtEnd;
        for (int i = 0; i < numRepeatedChars; i++) {
            of.put(freqTree.first->ch);
        }
        compressed.clear();
        endOfFile = true;
    }

    /* Main decompression loop for turning bits back into characters */

    unsigned char index = 0; //index of bit we are looking at in the current byte

    while (compressed.size() > 0) {

        //Reset search location to top of tree
        Node* currentNode = freqTree.first;

        while (!currentNode->isLeaf) {

            if (index == CHAR_BIT && compressed.size() != 1) {
                compressed.pop_front();
                index = 0;
            }

            if (compressed.size() <= 1) {
                if (CHAR_BIT - index == numZerosAtEnd) {
                    endOfFile = true;
                    compressed.pop_front();
                    break;
                }
            }

            unsigned char bits = compressed.front();
            unsigned char currentBit = bits;
            currentBit <<= index;
            currentBit >>= (CHAR_BIT - 1);

            if (currentBit == 1) {
                currentNode = currentNode->right;
            }
            else if (currentBit == 0) {
                currentNode = currentNode->left;
            }
            index++;
        }
        //At a leaf, now reset
        if (!endOfFile) {
            char ch = currentNode->ch;
            of.put(ch);
        }

    }
    of.close();
}




/* Writing Output to Compressed.txt */

void writeNode(Node* node, ofstream& outfile) {
    if (node->isLeaf) {
        outfile.put(LEAF);
        outfile.put(node->ch);
    }
    else {
        writeNode(node->left, outfile);
        writeNode(node->right, outfile);
        outfile.put(NODE);
        outfile.put((unsigned char) node->freq);
    }
}

//Wrapper function for recursive writing of Nodes
void writeBinaryTree(ofstream& outfile, Tree& freqTree) {
    writeNode(freqTree.first, outfile);
}

/*
    I first need to figure out how many chars length the tree will be,
    and insert that into the list first.

    The data itself will have a single char header that tells me how many
    trailing 0's are in the last byte of data.

    The data will be written to the file as follows:

    |Trailing 0's(1 byte)|TreeLength(1 byte)|Tree Data|CompressedData|
*/

void writeCompressedFile(ofstream& outfile, Tree& freqTree, list<unsigned char>& compressed) {
    outfile.open(CMP, std::ios::binary);

    //Compressed list already contains the trailing zeros at the front
    unsigned char trailingZeros = compressed.front();
    compressed.pop_front();
    outfile.put(trailingZeros);

    unsigned char treeLength = freqTree.getLength(); //length = the number of nodes in the tree
    outfile.put(treeLength);

    //Copy binary tree data
    writeBinaryTree(outfile, freqTree);

    //Copy compressed data
    for (list<unsigned char>::iterator it = compressed.begin(); it != compressed.end(); it++) {
        unsigned char c = *it;
        outfile.put(c);
    }

    outfile.close();
}





/* fillFrequencyMap 
   ----------------
   Maps each byte in the file to its frequency, while copying
   the entire file into a string.
*/

void fillFrequencyMap(ifstream& infile, string& out, map<char, size_t>& freqMap) {
    infile.open(TEXT, std::ios::binary);
    char ch;
    while (infile.get(ch)) {
        out += ch;
        if (freqMap.find(ch) == freqMap.end()) {
            //character not yet in the map
            freqMap.insert(pair<char, int>(ch, 1));
        }
        else {
            //increment the frequency of that byte
            (freqMap.find(ch)->second)++;
        }
    }
    infile.close();
}




int main()
{
    ifstream infile;
    ofstream outfile;
    string fileAsString;
    
    //Create the map of characters to their frequency
    map<char, size_t> freqMap;
    fillFrequencyMap(infile, fileAsString, freqMap);

    //Now create binary tree from the values calulated
    Tree freqTree;
    createBinaryTree(freqTree, freqMap);

    //Now loop through the string of the file and replace the bytes with the bits
    list<unsigned char> compressed;
    compressFile(fileAsString, freqTree, compressed);

    //Write all compressed info and tree to a file
    writeCompressedFile(outfile, freqTree, compressed);

    //Completely reconstruct the binary tree and the file from the compressed data
    Tree dFreqTree;
    ifstream cmpInfile;
    ofstream decompressed;
    decompressFile(cmpInfile, dFreqTree, decompressed);

    cout << "Done. Results are in " << DECOMPRESSED << endl;
}