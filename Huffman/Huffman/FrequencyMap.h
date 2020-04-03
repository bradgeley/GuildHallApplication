#pragma once
#include <fstream>

using namespace std;



/* FrequencyMap 
   ------------
   Maps each combination of bits that can appear in one
   byte, from 0 to 255, to the frequency that it appears
   in a file.
*/



class FrequencyMap
{
private:

    /* Constants */

    static const size_t MAP_SIZE = 256;


public:

	/* Constructor */

	FrequencyMap(string filename);


    /* Public Interface */


    /* getFreq
       -------
       Returns the frequency associated with a byte
    */

    const uint32_t getFreq(size_t index) const;


    /* size()
       -------
       Returns the size of the array within the FrequencyMap.
    */

    const size_t size() const;


private:

	/* Private Variables */

    uint32_t freqs[MAP_SIZE] = { 0 };


    /* fillFrequencyMap
       ----------------
       Maps each byte in the file to its frequency.
    */

    void fillFrequencyMap(const string filename);

};

