#pragma once
#include <string>

#include "Tree.h"

using namespace std;



/* HuffmanCompressor
   -----------------
   Compresses and decompresses a file byte by byte. The
   compression ratio can be anywhere around 50% to 70%
   based on the amount of repetition or diversity in the file.
*/



class HuffmanCompressor
{
public:

    /* compressFile
       ------------
       Compresses a file by finding repetitive bytes and representing
       them with smaller bit values. It must store the key along with the
       file in order to decompress it.
       */

    void compressFile(string infileName, string outfileName);


    /* decompressFile
       --------------
       Decompresses a file by reconstructing the binary tree contained within
       the compressed file, and then the file itself.
    */

    void decompressFile(string cmpFilename, string decompressedFilename);


private:

    const uint32_t getFileLength(ifstream& infile) const;

    const uint32_t getTrailingZeros(ifstream& infile) const;

};

