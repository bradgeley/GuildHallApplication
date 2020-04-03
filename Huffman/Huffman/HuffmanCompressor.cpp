#include "HuffmanCompressor.h"


/* Compression Methods */

/* compressFile
   ------------
   Write the binary tree key into the compressed file, then step through 
   the file of data byte by byte, doing the following for each:

   1. Find the bit representation of the byte as given by the binary tree
   2. Fill as much of an 8-bit mask as we can with those bits, then get a new mask if we need to
   3. If necessary, fill mask 2 with some of the bits we could not fit into mask 1

   Once the 8-bit mask is full, it is ready to be pushed to the compressed file.
   At the end of the file there are anywhere between 0-7 trailing 0's. If the last mask 
   ended with only one of the bits filled, the remaining 7 bits are trailing 0's.
   They could be misread by the decompresser as another character. Thus we need to include
   the number of trailing 0's as the last byte of data in the file.

   Compressed file structure:

   ------------------------------------------------------------------------------------------
   |               |            |                   |                  |                    |
   |  Binary Tree  |  Sentinel  |  Compressed Data  |  Trailing Zeros  |  # Trailing Zeros  |
   |    (<1KB)     |    (1B)    |    (Any Size)     |      (<1B)       |        (1B)        |
   ------------------------------------------------------------------------------------------

   */

void HuffmanCompressor::compressFile(string infileName, string outfileName) {
    FrequencyMap freqMap(infileName);
    Tree freqTree(freqMap);

    ifstream infile;
    ofstream outfile;
    infile.open(infileName, ios::binary);
    outfile.open(outfileName, ios::binary);

    freqTree.writeTo(outfile);

    /* Compression Variables */

    uint8_t mask = 0b0;
    uint8_t filledBits = 0;

    char ch;
    uint8_t byte;

    /* Outer Loop for receiving bytes from the uncompressed file */

    while (infile.get(ch)) {
        byte = (uint8_t)ch;

        /* Get the bit rep for the byte as an int and and its length in bits */

        Tree::bitRep bitRepPair = freqTree.findBitRepOf(byte);
        uint32_t bitRep = bitRepPair.bits;
        uint32_t nBits = bitRepPair.nBits;

        /* Inner loop for filling 8 bit masks */

        while (true) {

            if (filledBits == sizeof(mask) * 8) {
                outfile << mask;
                mask = 0;
                filledBits = 0;
            }

            uint8_t unfilledBits = sizeof(mask) * 8 - filledBits;

            if (nBits > unfilledBits) {

                /* Transfer all the bits we can to the current mask,
                   and keep track of how many are left. */

                uint32_t tempBitRep;
                tempBitRep = bitRep >> (nBits - unfilledBits);

                mask = mask | (uint8_t)tempBitRep;
                filledBits += unfilledBits;
                nBits -= unfilledBits;

                bitRep = bitRep << (sizeof(bitRep) * 8 - nBits);
                bitRep = bitRep >> (sizeof(bitRep) * 8 - nBits);
            }
            else if (nBits <= unfilledBits) {

                /* Transfer all bits to the current mask */

                uint8_t tempBitRep = bitRep << (unfilledBits - nBits);
                mask = mask | tempBitRep;
                filledBits += nBits;
                break; //We've filled all we can with the current byte
            }
        }
    }

    /* Put the very last mask into the outfile, as well as the trailing zeros footer */

    outfile << mask;
    uint8_t trailingZeros = 8 - filledBits;
    outfile << trailingZeros;

    infile.close();
    outfile.close();
}



/* Decompression Methods */

/* DecompressFile
   --------------
   Decompresses the binary tree and uses it to step bit by bit through a file.
   This process is rather slow for large files since we look at each bit
   one at a time. The process is as follows:

   1. Read a bit from the current byte being looked at
   2. Step through the binary tree (1 means right, 0 means left)
   3. If necessary, gather a new byte of data.
   4. If we find a leaf, then output the character on that leaf to the outfile, then reset tree.
   5. If we reach the second to last byte, compare the bit index to the number of trailing zeros,
        which was stored in the last byte.

   Compressed file structure:

   ------------------------------------------------------------------------------------------
   |               |            |                   |                  |                    |
   |  Binary Tree  |  Sentinel  |  Compressed Data  |  Trailing Zeros  |  # Trailing Zeros  |
   |    (<1KB)     |    (1B)    |    (Any Size)     |      (<1B)       |        (1B)        |
   ------------------------------------------------------------------------------------------

*/

void HuffmanCompressor::decompressFile(string compressedFile, string outputFile) {
    ifstream infile;
    ofstream outfile;

    infile.open(compressedFile, ios::binary);
    outfile.open(outputFile, ios::binary);

    Tree freqTree;
    freqTree.decompressTree(infile);

    /* Gather footer data (last 1 byte), then reset back to where we were */

    const uint32_t fileLength = getFileLength(infile);
    const uint8_t nTrailingZeros = getTrailingZeros(infile);

    /* Special case if the file input was only one character repeated and thus our tree is only one node */

    if (freqTree.root->isLeaf) {
        uint8_t byte = freqTree.root->byte;
        uint32_t bytesOfData = fileLength - (uint32_t)infile.tellg();
        uint32_t numRepeatedChars = bytesOfData * 8 - nTrailingZeros;
        for (uint32_t i = 0; i < numRepeatedChars; i++) {
            outfile << byte;
        }
    }
    else {

        /* Decompression Variables */

        char ch;
        uint8_t byte;
        uint8_t index = 8; //index of the bit we are looking at in the current byte

        /* Outer Loop that searches each compressed bit representation in the file */

        while (infile.tellg() <= fileLength) {
            Node* currentNode = freqTree.root;

            /* Inner loop that searches binary tree until hitting a leaf */

            while (!currentNode->isLeaf) {
                if (index == 8) {
                    //gather a new byte of data
                    infile.get(ch);
                    byte = (uint8_t)ch;
                    index = 0;
                }

                if (infile.tellg() == fileLength && (8 - index) == nTrailingZeros) {
                    /* If we are at the last byte, then when (8 - index) = trailingZeros, 
                       we have reached the end of the compressed data */
                    infile.get(ch);
                    break;
                }

                /* Isolate the bit at index */

                uint8_t currentBit = byte;
                uint8_t indexBit = (1 << (8 - 1 - index));
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

            if (infile.tellg() <= fileLength) {
                uint8_t correctChar = currentNode->byte;
                outfile << correctChar;
            }
        }
    }
    infile.close();
    outfile.close();
}


/* getFileLength 
   -------------
   Step to the second to last byte of data, excluding the footer,
   and retrieve the length.
*/

const uint32_t HuffmanCompressor::getFileLength(ifstream& infile) const {
    streampos currentPos = infile.tellg();
    infile.seekg(-1, infile.end);
    uint32_t length = (uint32_t)infile.tellg();
    infile.seekg(currentPos);
    return length;
}


/* getTrailingZeros
   ----------------
   Step to the end of the file and recover the last byte of
   data, which tells us the number of trailing zeros in the
   compressed data.
*/

const uint32_t HuffmanCompressor::getTrailingZeros(ifstream& infile) const {
    streampos currentPos = infile.tellg();
    infile.seekg(-1, infile.end);
    char trailingZeros;
    infile.get(trailingZeros);
    infile.seekg(currentPos);
    return (uint32_t)trailingZeros;
}