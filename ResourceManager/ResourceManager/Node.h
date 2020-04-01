#pragma once

#include <list>
#include <string>

using namespace std;



/* Node
   ----
   Keeps track of its own dependencies in a list on the heap.
   The dependency pointers need to point to the actual Nodes
   within the Resource Manager's list.

   Resource manager keeps track of it's full list of resources in
   a std::list, therefore the memory addresses of those values will
   not change as we remove and add items to it. 
   
   So each resources safely stores its list of connected nodes as 
   pointers to the items in the resource manager's list.
*/



class Node {
public:

    /* Constructor/Destructor */

    Node(string name);

    Node(string name, Node* node);

    ~Node();

private:

    /* Private Variables */

    string name = "";
    list<Node*>* dependencies;

public:

    /* Public Interface */

    /* getName
       -------
       Returns the name of the item in Node.
    */

    const string getName() const;

    /* containsDependency
       ------------------
       Returns true if the node contains a dependency
       with a name matching resourceName.
    */

    bool containsDependency(string resourceName) const;

    /* addDependency
       -------------
       Adds a dependency to the current list, if it does not
       already contain it.

       This function should be given a pointer to a Node
       contained in the Resource Manager's list. That way,
       we do not have to copy resource data, but only the pointer to it.
    */

    void addDependency(Node* dep);

    /* removeDependency
       ----------------
       If the resource contains the dependency with depName,
       then remove the pointer from our list.
    */

    void removeDependency(string depName);

    /* getDependencies
       ---------------
       Returns the list of dependencies.
    */

    const list<Node*>* getDependencies() const;

};



