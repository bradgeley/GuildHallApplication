#include <iostream>

#include "HuffmanCompressor.h"

using namespace std;


/* File Names */

const string TEXT = "Text.txt";
const string CMP = "Compressed.txt";
const string DECOMPRESSED = "Decompressed.txt";


/* main 
   ----
   Compresses an input file given by TEXT, and compresses it
   into CMP, only to decompress it back into DECOMPRESSED.
   */

uint32_t main()
{

    HuffmanCompressor c;

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