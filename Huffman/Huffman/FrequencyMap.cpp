#include "FrequencyMap.h"



/* FrequencyMap
   ------------
   Maps each combination of bits that can appear in one
   byte, from 0 to 255, to the frequency that it appears
   in a file.
*/

FrequencyMap::FrequencyMap(string filename) {
    fillFrequencyMap(filename);
}


/* getFreq
   -------
   Returns the frequency associated with a byte
*/

const uint32_t FrequencyMap::getFreq(size_t index) const {
    return freqs[index];
}


/* size()
   ------
   Returns the size of the frequency map array
*/

const size_t FrequencyMap::size() const {
    return MAP_SIZE;
}


/* fillFrequencyMap
   ----------------
   Maps each byte in the file to its frequency. We must first
   get the byte as a char, then cast to an uint8_t due to
   the istream >> (uint8_t) not picking up certain ascii
   values, including returns and spaces.
*/

void FrequencyMap::fillFrequencyMap(const string filename) {
    ifstream infile;
    infile.open(filename, ios::binary);

    char ch;
    uint8_t byte;
    while (infile.get(ch)) {
        byte = (uint8_t)ch;
        freqs[byte]++;
    }

    infile.close();
}