#include <fstream>
#include <iostream>
#include <list>
#include <stack>

#include "Tree.h"

using namespace std;



/* Constants */

const size_t MAP_SIZE = 256;

/* File Names */

const string TEXT = "Text.txt";
const string CMP = "Compressed.txt";
const string DECOMPRESSED = "Decompressed.txt";

/* Sentinels for serializing/deserializing the binary tree */

const unsigned char LEAF = 1;
const unsigned char NODE = 0;
const unsigned char DATA = 2;



/* Frequency Map Functions */

/* fillFrequencyMap
   ----------------
   Maps each byte in the file to its frequency. We must first
   get the byte as a char, then cast to an unsigned char due to
   the istream >> (unsigned char) not picking up certain ascii 
   values, including returns and spaces.
*/

void fillFrequencyMap(ifstream& infile, unsigned int freqMap[]) {
    infile.open(TEXT, ios::binary);
    cout << "Filling Frequency Map..." << endl;
    char ch;
    unsigned char byte;
    while (infile.get(ch)) {
        byte = (unsigned char) ch;
        freqMap[byte]++;
    }
    infile.close();
    cout << "Map complete." << endl;
}




/* Binary Tree Functions */

/* createBinaryTree
   ----------------
   Creates the binary tree that we use to find unique bit values
   for each byte of data.

   Takes a map of each character value (the index in the array) to a frequency.
   All of the higher frequency nodes will be added higher in the tree. Lower
   frequency values are added first, meaning they are deeper into the tree
   and will have longer bit representations.
   */

void createBinaryTree(Tree& binaryTree, unsigned int freqMap[]) {
    cout << "Creating Binary Tree..." << endl;

    /* Iterate through the map and make a Node for each one */

    list<Node> nodes;
    for (int i = 0; i < MAP_SIZE; i++) {
        unsigned char ch = (unsigned char)i;
        if (freqMap[ch] != 0) {
            nodes.push_back(Node(ch, freqMap[ch]));
        }
    }

    /* Debugging Variable */

    int treeNodes = 0;

    /* Binary Tree Creation Loop */

    while (nodes.size() > 1) {

        //Sort elements lowest to highest by frequency
        nodes.sort();

        //Take the 2 first, lowest frequency elements
        Node* first = new Node(nodes.front());
        treeNodes++;
        nodes.pop_front();
        Node* second = new Node(nodes.front());
        treeNodes++;
        nodes.pop_front();

        //Add back a new Node with the combined frequency of the children
        Node parent(first->freq + second->freq, first, second);
        nodes.push_back(parent);
    }

    /* Set the root of the tree */

    binaryTree.first = new Node(nodes.front());
    nodes.pop_front(); //now empty
    treeNodes++;

    cout << "Done. Nodes in tree: " << treeNodes << endl;
}




/* Compression Functions */


/* findBitRepOfByte 
   ----------------
   Recursively traverses tree, keeping track of the resulting
   binary representation of the result, as well as the length
   in bits.

   The bit representation of the number could end up being quite long. With
   an entirely inefficient binary tree, the 255 possible bytes could be spread
   out poorly such that it takes 8+ Node traversals to reach one value.
   So I use an u_integer to store the bit representation, rather than a u_char
   but I am not sure if this is totally necessary.

   I store the potential bit representation in temp, and the potential size in nBitsTemp.

   When I get a match to a Leaf, I return those results into bitRepResult and nBitsResult.
   */

void findBitRepOfByte(const unsigned char c, Node* node, unsigned int bitRepTemp, unsigned int& bitRepResult, unsigned int nBitsTemp, unsigned int& nBitsResult) {
    if (node->isLeaf) {
        if (node->ch == c) {
            bitRepResult = bitRepTemp;
            nBitsResult = nBitsTemp;
        }
    }
    else {
        findBitRepOfByte(c, node->left, bitRepTemp << 1, bitRepResult, nBitsTemp + 1, nBitsResult);
        findBitRepOfByte(c, node->right, (bitRepTemp << 1) + 1, bitRepResult, nBitsTemp + 1, nBitsResult);
    }
}

/* writeNode 
   ---------
   Recursively writes the tree depth first, post order
   into the open outfile.
   */

void writeNode(Node* node, ofstream& outfile) {
    if (node->isLeaf) {
        outfile << LEAF;
        outfile << node->ch;
    }
    else {
        writeNode(node->left, outfile);
        writeNode(node->right, outfile);
        outfile << NODE;
        outfile << NODE;
    }
}

/* writeBinaryTree
   ---------------
   First writes the header value that tells the decompressor
   how many Nodes were in the original tree.
   
   Then serves as the wrapper for writeNode() 
   */

void writeBinaryTree(ofstream& outfile, Tree& freqTree) {
    writeNode(freqTree.first, outfile);
    outfile << DATA; //delimeter for knowing when the tree is over
}

/* compressFile
   ------------
   Since the bit representation of each byte is of varying length,
   this is a quite complicated function. We step through the string of data,
   finding the binary representation and then filling the remainder of the current
   8-bit mask. 
   
   Once the 8-bit mask is full, it is ready to be pushed to the compressed file.
   At the end of the file there are anywhere between 0-7 trailing 0's.

   (e.g.) If the last mask ended with only one of the bits filled, the remaining 7 are trailing 0's
   and could be misread by the decompresser as another character. Thus we need to include
   the number of trailing 0's as the last byte of data in the file.
   */

void compressFile(ifstream& infile, ofstream& outfile, Tree& freqTree) {
    cout << "Compressing..." << endl;

    infile.open(TEXT, ios::binary);
    outfile.open(CMP, ios::binary);

    writeBinaryTree(outfile, freqTree);

    /* Compression Variables */

    unsigned int nBytesInFile = 0;
    unsigned int nBytesInCompressed = 0;

    unsigned char mask = 0b0;
    unsigned char filledBits = 0;

    char ch;
    unsigned char byte;

    /* Outer Loop for receiving bytes from the uncompressed file */

    while (infile.get(ch)) {
        byte = (unsigned char)ch;
        nBytesInFile++;

        unsigned int tempBitRep = 0, bitRep = 0; //Values to store the bit representation of a char
        unsigned int nBitsTemp = 0, nBits = 0; //Values that store the bit length of the bit rep

        findBitRepOfByte(byte, freqTree.first, tempBitRep, bitRep, nBitsTemp, nBits);
        if (freqTree.first->isLeaf) nBits = 1, bitRep = 1; //Special case

        /* Inner loop for filling 8 bit masks */

        while (true) {

            if (filledBits == sizeof(mask) * CHAR_BIT) {
                outfile << mask;
                nBytesInCompressed++;
                mask = 0;
                filledBits = 0;
            }

            unsigned char unfilledBits = sizeof(mask) * CHAR_BIT - filledBits;

            if (nBits > unfilledBits) {

                /* Transfer all the bits we can to the current mask,
                   and keep track of how many are left. */

                tempBitRep = bitRep >> (nBits - unfilledBits);

                mask = mask | (unsigned char)tempBitRep;
                filledBits += unfilledBits;
                nBits -= unfilledBits;

                bitRep = bitRep << (sizeof(bitRep) * CHAR_BIT - nBits);
                bitRep = bitRep >> (sizeof(bitRep) * CHAR_BIT - nBits);
            }
            else if (nBits <= unfilledBits) {

                /* Transfer all bits to the current mask */

                tempBitRep = bitRep << (unfilledBits - nBits);
                mask = mask | (unsigned char)tempBitRep;
                filledBits += nBits;
                break; //We've filled all we can with the current byte
            }
        }
    }

    /* Put the very last mask into the outfile, as well as the trailing zeros footer */

    outfile << mask;
    nBytesInCompressed++;
    unsigned char trailingZeros = CHAR_BIT - filledBits;
    outfile << trailingZeros;

    cout << "Bytes in file: " << nBytesInFile << endl;
    cout << "Compressed file to: " << nBytesInCompressed << " bytes (without headers or tree (~1KB))" << endl;
    cout << "Compression ratio: " << (double) nBytesInCompressed / nBytesInFile * 100 << "%" << endl;

    infile.close();
    outfile.close();
}




/* Decompression Functions */


/* DecompressTree
   --------------
   Reconstructs the binary tree from the file using a stack.
   If you read a leaf, put it on a stack, if you read a node, take 2
   children from the stack and put the node back on.
   
   This function is given a list that contains blocks of chars in this format:

   | sentinel |   |  char  |

   If we read a 0 (NODE), then we copy the data as a Node and ignore the char,
   If we read a 1 (LEAF), we copy the data as a Leaf and grab the char
   If we read a 2 (DATA), then the tree is finished and the data begins
   */

void decompressTree(ifstream& infile, Tree& freqTree) {
    stack<Node*> s;
    int numNodesInReconstructedTree = 0;
    char ch;
    unsigned char sentinel, byte;
    while (true) {

        //get the sentinel for the next Node
        infile.get(ch);
        sentinel = (unsigned char)ch;
        
        if (sentinel == LEAF) {
            //get the byte after the sentinel value
            infile.get(ch);
            byte = (unsigned char)ch;
            Node* leaf = new Node(byte, 0); //frequency, or the second parameter here, does not matter in this tree
            numNodesInReconstructedTree++;
            s.push(leaf);
        }
        else if (sentinel == NODE) {
            //get the next value which is not used.
            infile.get(ch);
            Node* right = s.top(); s.pop();
            Node* left = s.top(); s.pop();
            Node* node = new Node(0, left, right); //leaving freq empty
            numNodesInReconstructedTree++;
            s.push(node);
        }
        else if (sentinel == DATA) {
            break;
        }
    }
    cout << "Reconstructed Tree Nodes: " << numNodesInReconstructedTree << endl;
    freqTree.first = s.top();
}

/* DecompressFile
   --------------
   Decompresses the binary tree and uses it to step bit by bit through a file.
   This process is rather slow for large files since we look through the whole
   tree for each bit we find, and gather data only one byte at a time.

   The data in the file contains a number of trailing zeros, the number of which
   is contained in a 1 byte footer at the end of the file. Once we read this,
   we know how many zeros to ignore at the end of the file.
   */

void decompressFile(ifstream& infile, ofstream& outfile, Tree& freqTree) {
    cout << "Decompressing..." << endl;

    infile.open(CMP, ios::binary);
    outfile.open(DECOMPRESSED, ios::binary);

    decompressTree(infile, freqTree);

    /* Gather footer data (last 1 byte), then reset back to where we were */

    streampos currentPos = infile.tellg();
    infile.seekg(-1, infile.end);
    unsigned int length = (unsigned int) infile.tellg();
    char trailingZeros;
    infile.get(trailingZeros);
    infile.seekg(currentPos);

    /* Debugging variable to ensure the output has the same bytes as input */

    unsigned int decompressedBytes = 0;

    /* Special case if the file input was only one character repeated and thus our tree is only one node */

    if (freqTree.first->isLeaf) {
        unsigned char ch = freqTree.first->ch;
        int bytesOfData = length - (int) infile.tellg();
        int numRepeatedChars = bytesOfData * CHAR_BIT - trailingZeros;
        decompressedBytes = numRepeatedChars;
        for (int i = 0; i < numRepeatedChars; i++) {
            outfile << ch;
        }
    }
    else {

        /* Decompression Variables */

        char ch;
        unsigned char byte;
        unsigned char index = CHAR_BIT; //index of the bit we are looking at in the current byte

        /* Outer Loop that searches each bit representation in the file */

        while (infile.tellg() <= length) {
            Node* currentNode = freqTree.first; //Reset search location to top of tree

            /* Inner loop that searches binary tree until hitting a leaf */

            while (!currentNode->isLeaf) {
                if (index == CHAR_BIT) {
                    //if the index reaches 8, we need to gather a new byte of data
                    infile.get(ch);
                    byte = (unsigned char)ch;
                    index = 0;
                }

                if (infile.tellg() == length && CHAR_BIT - index == trailingZeros) {
                    //If we are at the end of the file, then (8 - index) = trailingZeros
                    infile.get(ch);
                    byte = (unsigned char)ch;
                    break;
                }

                /* Isolate the bit at index */

                unsigned char currentBit = byte;
                unsigned char indexBit = (1 << (CHAR_BIT - 1 - index));
                currentBit &= indexBit;

                /* Step further into the tree based on that bit */

                if (currentBit >= 1) {
                    currentNode = currentNode->right;
                }
                else if (currentBit == 0) {
                    currentNode = currentNode->left;
                }
                index++;
            }

            /* A leaf has been found */

            if (infile.tellg() <= length) {
                unsigned char correctChar = currentNode->ch;
                outfile << correctChar;
                decompressedBytes++;
            }
        }
    }

    cout << "Size of decompressed file: " << decompressedBytes << endl;

    infile.close();
    outfile.close();
}




/* main 
   ----
   Compresses an input file given by TEXT, and compresses it
   into CMP, only to decompress it back into DECOMPRESSED.
   */

int main()
{
    ifstream infile;
    ofstream outfile;
    
    unsigned int freqMap[MAP_SIZE] = { 0 };
    fillFrequencyMap(infile, freqMap);

    Tree freqTree;
    createBinaryTree(freqTree, freqMap);

    compressFile(infile, outfile, freqTree);
    freqTree.~Tree();
    infile.clear(), outfile.clear();

    decompressFile(infile, outfile, freqTree);

    cout << "Done. Results are in " << DECOMPRESSED << endl;
}