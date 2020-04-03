#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <string>

#include "ResourceManager.h"

using namespace std;



/* Resource Manager
   ----------------
   Keeps track of a list of resources, and is responsible
   for the logic of adding and deleting resources, as well
   as printing itself and reading a file of formatted input.

   Resources themselves will keep track of their dependencies
   on other resources, which they do by pointing back to
   the Nodes on this resource manager's list.
*/



/* Constructor/Destructor */


/* ResourceManager() 
   -----------------
   Creates an empty list of resources on the heap.
*/

ResourceManager::ResourceManager() {
    resources = new list<Node*>();
}


/* ~ResourceManager()
   ------------------
   Deletes each resource in the list, then the list itself.
*/

ResourceManager::~ResourceManager() {
    for (list<Node*>::iterator it = resources->begin(); it != resources->end(); it++) {
        delete* it;
    }
    delete resources;
}



/* Methods */

/* resourceExists
   --------------
   Returns true if a resource of the given name appears
   in the current list of resources.
*/

bool ResourceManager::resourceExists(string resourceName) const {
    for (list<Node*>::iterator it = resources->begin(); it != resources->end(); it++) {
        Node* node = *it;
        if (node->getName() == resourceName) {
            return true;
        }
    }
    return false;
}


/* addResource
   -----------
   Creates new Nodes for each name given, if they do
   not already exist. Then connects the Node with name
   to the dependency node.
*/

void ResourceManager::addResource(string name, string nameOfDependency) {
    if (!resourceExists(name)) {
        resources->push_back(new Node(name));
    }
    if (!resourceExists(nameOfDependency)) {
        resources->push_back(new Node(nameOfDependency));
    }
    Node* dep = getResource(nameOfDependency);
    Node* res = getResource(name);
    if (!res->containsDependency(nameOfDependency) && dep->getName() != res->getName()) {
        //Do not add dependency if the Nodes have the same name.
        res->addDependency(dep);
    }
}


/* getResource
   -----------
   Returns a pointer to the Node corresponding to the
   resourceName given. We keep track of Nodes in a std::list
   rather than a vector because we need the pointers to not
   become invalidated as the list grows and shrinks.
*/

const Node* ResourceManager::getResource(string resourceName) const {
    Node* result = nullptr;
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {
        Node* node = *it;
        if (node->getName() == resourceName) {
            result = node;
            break;
        }
    }
    return result;
}

Node* ResourceManager::getResource(string resourceName) {
    Node* result = nullptr;
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {
        Node* node = *it;
        if (node->getName() == resourceName) {
            result = node;
            break;
        }
    }
    return result;
}


/* removeResource
   --------------
   Removes the resource with the given name from resources.

   However, in order to maintain a functioning resource list,
   this function also calls each Node in the list
   to purge that resource from its dependencies first.
*/

void ResourceManager::removeResource(string name) {
    list<Node*>::iterator it;
    list<Node*>::iterator toRemove;
    for (it = resources->begin(); it != resources->end(); it++) {
        Node* r = *it;
        if (r->getName() == name) {
            toRemove = it;
        }
        if (r->containsDependency(name)) {
            r->removeDependency(name);
        }
    }
    resources->erase(toRemove);
}


/* readFile
   --------
   Reads a file of data formatted as such:

   resource dependency
   resource2 dependency2
   resource3 dependency2

   For each resource and dependency, create a Node if
   one did not already exist in the list. Then connect
   them if they are not already connected.
*/

void ResourceManager::readFile(string filename) {
    cout << "Reading " << filename << "..." << endl;
    ifstream infile;
    try {
        infile.open(filename);
        string resName, depName;
        while (infile >> resName) {
            infile >> depName;
            addResource(resName, depName);
        }
    }
    catch (ifstream::failure f) {
        cout << "Could not open file named: " << filename << endl;
    }
    cout << "Success." << endl << endl;
}


/* saveFile
   --------
   Runs through all resources and prints its
   name along with every dependency. 
   
   **Nodes that are "unused to will not get saved.**
*/

void ResourceManager::saveFile(string filename) {
    ofstream outfile;
    outfile.open(filename);
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {
        Node* node = (*it);
        const list<Node*>* deps = node->getDependencies();
        for (list<Node*>::const_iterator d = deps->begin(); d != deps->end(); d++) {
            Node* dep = (*d);
            outfile << node->getName() << " " << dep->getName() << '\n';
        }
    }
    outfile.close();
}


/* print
   -----
   Makes heavy use of <iomanip> formatters in order to
   create an output that looks like the following:

   Resource     Dependency
   -------------------------
   |res1       |dep1       |
   |           |dep2       |
   -------------------------
   |res2       |dep1       |
   |           |dep2       |
   -------------------------
   |res3       |no deps    |
   -------------------------
   |res4       |unusable   |
   -------------------------
*/

void ResourceManager::print() const {

    const size_t MAX_NAME_LEN = 15;
    const size_t totalWidth = MAX_NAME_LEN * 2 + 3;

    printHeader(MAX_NAME_LEN, totalWidth);
    printNormalResources(MAX_NAME_LEN, totalWidth);
    printNoDependencyResources(MAX_NAME_LEN, totalWidth);
    printUnusableResources(MAX_NAME_LEN, totalWidth);
}


/* printHeader 
   -----------
   Prints the top line and header of the UI.
   */

void ResourceManager::printHeader(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const
{
    cout << endl << setw(MAX_NAME_LEN + 2) << setfill(' ') << left << "Resource" << "Dependencies" << endl;
    cout << setw(totalWidth) << setfill('-') << "" << endl;
}


/* printUnusableResources
   ----------------------
   Prints all resources that have no dependencies
   and upon which no resources depend. These are not
   saved when the user saves the file.
   */

void ResourceManager::printUnusableResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const
{
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {

        const Node* node = *it;
        string name = node->getName();
        if (name.length() > MAX_NAME_LEN) { name = name.substr(0, MAX_NAME_LEN - 1); } //cut off long names

        bool isDependedOnBySomething = false;

        for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {
            if ((*it)->containsDependency(node->getName())) {
                isDependedOnBySomething = true;
            }
        }
        if (!isDependedOnBySomething && node->getDependencies()->size() == 0) {
            cout << setfill(' ') << "|" << left << setw(MAX_NAME_LEN) << name << "|" << setw(MAX_NAME_LEN) << "Unconnected" << "|" << endl;
            cout << setfill('-') << setw(totalWidth) << "" << endl;
        }
    }
}


/* printNoDependencyResources
   --------------------------
   Prints all resources which have no dependencies,
   but are still depended upon, and therefore could
   be thought of as "base" resources.
   */

void ResourceManager::printNoDependencyResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const
{
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {

        const Node* node = *it;
        string name = node->getName();
        if (name.length() > MAX_NAME_LEN) { name = name.substr(0, MAX_NAME_LEN - 1); } //cut off long names

        const list<Node*>* deps = node->getDependencies();

        if (deps->size() == 0) {
            cout << setfill(' ') << "|" << left << setw(MAX_NAME_LEN) << name << "|" << setw(MAX_NAME_LEN) << "No dependencies" << "|" << endl;
            cout << setfill('-') << setw(totalWidth) << "" << endl;
        }
    }
}


/* printNormalResources
   --------------------
   Prints all resources which have dependencies.
   */

void ResourceManager::printNormalResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const
{
    for (list<Node*>::const_iterator it = resources->begin(); it != resources->end(); it++) {

        const Node* node = *it;
        string name = node->getName();
        if (name.length() > MAX_NAME_LEN) { name = name.substr(0, MAX_NAME_LEN - 1); } //cut off long names

        const list<Node*>* deps = node->getDependencies();

        /* Inner loop to print all dependencies of a resource */

        if (deps->size() > 0) {
            bool firstLine = true; //only print the resource name on the first line
            for (list<Node*>::const_iterator d = deps->begin(); d != deps->end(); d++) {
                string depName = (*d)->getName();
                if (depName.length() > MAX_NAME_LEN) { depName = depName.substr(0, MAX_NAME_LEN - 1); } //cut off long names
                if (!firstLine) name = "";
                cout << setfill(' ') << "|" << left << setw(MAX_NAME_LEN) << name << "|" << setw(MAX_NAME_LEN) << depName << "|" << endl;
                firstLine = false;
            }
            cout << setfill('-') << setw(totalWidth) << "" << endl;
        }
    }
}
