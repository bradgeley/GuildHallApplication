#include "HuffmanCompressor.h"


/* Compression Methods */

/* compressFile
   ------------
   Since the bit representation of each byte is of varying length,
   this is a quite complicated function. We step through the string of data,
   finding the binary representation and then filling the remainder of the current
   8-bit mask.

   Once the 8-bit mask is full, it is ready to be pushed to the compressed file.
   At the end of the file there are anywhere between 0-7 trailing 0's. If the last mask 
   ended with only one of the bits filled, the remaining 7 bits are trailing 0's.
   They could be misread by the decompresser as another character. Thus we need to include
   the number of trailing 0's as the last byte of data in the file.

   Compressed file structure:

   ------------------------------------------------------------------------------------------------------------------------
   |  Binary Tree(<1KB)  |  Sentinel(1B)  |  Compressed Data(Any size)  |  Trailing Zeros (<1B)  |  # Trailing Zeros(1B)  |
   ------------------------------------------------------------------------------------------------------------------------

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
   This process is rather slow for large files since we look through the whole
   tree for each bit we find, and gather data only one byte at a time.

   The data in the file contains a number of trailing zeros, the number of which
   is contained in a 1 byte footer at the end of the file. Once we read this,
   we know how many zeros to ignore at the end of the file.

   Compressed file structure:

   ------------------------------------------------------------------------------------------------------------------------
   |  Binary Tree(<1KB)  |  Sentinel(1B)  |  Compressed Data(Any size)  |  Trailing Zeros (<1B)  |  # Trailing Zeros(1B)  |
   ------------------------------------------------------------------------------------------------------------------------

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
   Step to the end of the file (- 1) and return the length
   so we can refer to it later.
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