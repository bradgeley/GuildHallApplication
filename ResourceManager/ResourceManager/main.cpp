// ResourceManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <list>
#include "ResourceManager.h"
//#include <vld.h>

using namespace std;



/* Constants */

string FILENAME = "Resources.txt";



/* main
   ----
   Creates a ResourceManager, then asks it to read an input 
   file of initial Resources in the following format:

   Resource Dependency
   Resource2 Dependency2
   Resource Dependency2

   Then starts an input loop where a user may add or delete resources
   as they see fit.
*/

int main()
{
    ResourceManager rm = ResourceManager();
    rm.readFile(FILENAME);
    rm.print();
    rm.remResource("tower");
    rm.print();
    
    while (true) {
        string input;
        cin >> input;
        if (input == "q" || input == "Q") {
            break;
        }
        //add resource dependency
        //delete resource
    }
}