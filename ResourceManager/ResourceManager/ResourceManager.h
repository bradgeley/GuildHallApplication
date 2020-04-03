#pragma once

#include "Node.h"



/* Resource Manager
   ----------------
   Keeps track of a list of resources, and is responsible
   for the logic of adding and deleting resources, as well
   as printing itself and reading a file of formatted input.

   The resource Nodes themselves will keep track of their dependencies
   on other resources.
*/



class ResourceManager {
public:

    /* Constructor/Destructor */

    ResourceManager();

    ~ResourceManager();

private:

    /* Private data */

    std::list<Node*>* resources;

public:


    /* Public Interface */


    /* containsResource 
       ----------------
       Returns true if a resource of the given name appears
       in the current list of resources.
    */

    bool resourceExists(std::string resourceName) const;


    /* addResource
       -----------
       Creates new Nodes for each name given, if they do
       not already exist. Then connects the lhs Node to its
       dependency Node.
    */

    void addResource(std::string resourceName, std::string nameOfDependency);


    /* getResource
       -----------
       Returns a pointer to the Node corresponding to the
       resourceName given.
    */  

    const Node* getResource(std::string resourceName) const;

    Node* getResource(std::string resourceName);


    /* removeResource
       --------------
       Removes the resource with the given name from resources. 
       
       In order to maintain a functioning resource list,
       this function also calls on each Node in resources
       to purge that resource from its dependencies.
    */

    void removeResource(std::string resourceName);


    /* readFile
       --------
       Reads a file of data formatted as such:

       resource dependency
       resource2 dependency2
       resource3 resource2

       For each resource and dependency, create a Node if
       one did not already exist in the list. Then connect
       them if they are not already connected.
    */

    void readFile(std::string filename);


    /* saveFile
       --------
       Saves the new data added in this editing session
       to a given filename, formatted as such:

       resource dependency
       resource2 dependency2
       resource3 resource2
    */

    void saveFile(std::string filename);


    /* print
       -----
       Creates an output that looks like the following:

        Resource     Dependency
        -----------------------------
        |res1       |dep1           |
        |           |dep2           |
        -----------------------------
        |res2       |dep1           |
        |           |dep2           |
        -----------------------------
        |dep1       |no dependencies|
        -----------------------------
        |dep3       |unusable       |
        -----------------------------

        Resources with no dependencies are still valid,
        since they may be depended upon by another resource.

        Unusable nodes are not connected to anything and
        are not depended on by another resource.

    */

    void print() const;

private:

    /* Private print() Helper Methods */

    void printHeader(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const;

    void printUnusableResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const;

    void printNoDependencyResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const;

    void printNormalResources(const size_t& MAX_NAME_LEN, const size_t& totalWidth) const;

};