#pragma once

#include "Node.h"

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



class ResourceManager {
public:

    /* Constructor/Destructor */

    ResourceManager();

    ~ResourceManager();

private:

    list<Node*>* resources;

public:

    /* containsResource 
       ----------------
       Returns true if a resource of the given name appears
       in the current list of resources.
    */

    bool containsResource(string resourceName) const;


    /* addResource
       -----------
       Creates new Nodes for each name given, if they do
       not already exist. Then connects the Node with name
       to the dependency node.
    */

    void addResource(string name, string nameOfDependency);


    /* getResource
       -----------
       Returns a pointer to the Node corresponding to the
       resourceName given.
    */  

    const Node* getResource(string resourceName) const;

    Node* getResource(string resourceName);


    /* removeResource
       --------------
       Removes the resource with the given name from resources. 
       
       However, in order to maintain a functioning resource list,
       this function also calls each Node in the list
       to purge that resource from its dependencies.
    */

    void removeResource(string name);


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

    void readFile(string filename);


    /* print
       -----
       Makes heavy use of <iomanip> formatters in order to
       create an output that looks like the following:

       Resources Dependencies
       -------------------
       |res1    |dep1    |
       |        |dep2    |
       -------------------
       |res2    |dep1    |
       |        |dep2    |
       -------------------

       Cuts down each name to be a maximum length, and does
       not show resources with no dependencies since they are
       currently invalid. Resources that only exist as a 
       dependency for another Node are in this category.

    */

    const void print() const;

};