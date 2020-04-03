#include <iomanip>
#include <iostream>
#include <string>

#include "ResourceManager.h"

using namespace std;



/* Resources Filename */

string FILENAME = "Resource.txt";



/* Function Prototypes */

void printHelpMessage();

void deleteResource(ResourceManager& rm);

void addResource(ResourceManager& rm);

void saveData(ResourceManager& rm, string filename);



/* main
   ----
   Creates a ResourceManager, then asks it to read an input 
   file of initial Resources in the following format:

   Resource Dependency
   Resource2 Dependency2
   Resource Dependency2

   Then starts an input loop where a user may add or delete resources
   as they see fit, and may choose to save at any time.
*/

int main()
{
    ResourceManager rm = ResourceManager();
    rm.readFile(FILENAME);

    /* User Interface Loop */

    while (true) {
        cout << "Enter \"help\" for options." << endl;
        rm.print();
        string input;
        cout << "Enter >: ";
        cin >> input;
        if (input == "q" || input == "Q") {
            break;
        }
        else if (input == "help") {
            printHelpMessage();
        }
        else if (input == "delete") {
            deleteResource(rm);
        }
        else if (input == "add") {
            addResource(rm);
        }
        else if (input == "save") {
            saveData(rm, FILENAME);
        }
    }
}



/* Functions */


/* printHelpMessage
   ----------------
   Prints a short summary of the command options.
   */

void printHelpMessage() {
    cout << endl << endl << setw(20) << setfill('-') << "HELP MENU" << endl;
    cout << "To add resources type:" << endl;
    cout << "add objectName resourceName" << endl;

    cout << "To delete objects type:" << endl;
    cout << "delete name" << endl;

    cout << "To save type:" << endl;
    cout << "save" << endl;

    cout << "Type q to quit." << endl;
    cout << setw(20) << setfill('-') << "" << endl << endl;

}



/* deleteResource
   --------------
   Takes 1 input and, if that resource is present
   in the data, deletes it.
   */

void deleteResource(ResourceManager& rm) {
    string res;
    cin >> res;
    rm.removeResource(res);
}



/* addResource
   -----------
   Takes 2 inputs from the user and treats the first one
   as a resource and the second input as a dependency for
   that resource.
   */

void addResource(ResourceManager& rm) {
    string obj, res;
    cin >> obj;
    cin >> res;
    rm.addResource(obj, res);
}


/* saveData
   --------
   Overwrites Resource.txt with the new data entered in this
   session.
   */

void saveData(ResourceManager& rm, string filename) {
    rm.saveFile(filename);
}
