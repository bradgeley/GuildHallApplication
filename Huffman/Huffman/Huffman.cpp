#include <iostream>

#include "HuffmanCompressor.h"

using namespace std;


/* File Names */

const string TEXT = "Text.txt";
const string CMP = "Compressed.txt";
const string DECOMPRESSED = "Decompressed.txt";


/* Functions */

void createTestFile(const string filename);


/* main 
   ----
   Uses an instance of a HuffmanCompressor to take an input 
   file given by TEXT, and compresses it into CMP, only to 
   decompress it back into DECOMPRESSED.
*/

uint32_t main() {

    HuffmanCompressor c;

    /* Optional Create Test File */

    createTestFile(TEXT);

    /* Compress file */

    {
        cout << "Compressing..." << endl;
        c.compressFile(TEXT, CMP);
        cout << "Done." << endl;
    }

    /* Decompress File */

    {
        cout << "Decompressing..." << endl;
        c.decompressFile(CMP, DECOMPRESSED);
        cout << "Done. Results are in " << DECOMPRESSED << endl;
    }
}


/* createTestFile 
   --------------
   Puts random input bytes from 0 to 255 into the TEXT
   file to be compressed and decompressed. This is an important
   test as it tests our compressor's functionality when the binary
   tree gets very large and the bytes are diverse.
*/

void createTestFile(const string filename) {
    ofstream outfile;
    outfile.open(filename, ios::binary);
    for (uint32_t i = 0; i < 1024; i++) {
        for (uint32_t j = 0; j < 256; j++) {
            uint8_t byte = (uint8_t)j;
            outfile.put(byte);
        }
    }
    outfile.close();
}