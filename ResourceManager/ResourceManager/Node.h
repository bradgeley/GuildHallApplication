#pragma once

#include <list>
#include <string>



/* Node
   ----
   A Node keeps track of its own list of dependencies 
   with several functions for manipulating that list.

   If a Node points to another Node, then it is dependent
   on that resource.
*/



class Node {
public:

    /* Constructor/Destructor */

    Node(std::string name);

    Node(std::string name, Node* node);

    ~Node();

private:

    /* Private Data */

    std::string name = "";
    std::list<Node*>* dependencies;

public:


    /* Public Interface */


    /* getName
       -------
       Returns the name of the item in Node.
    */

    const std::string getName() const;


    /* containsDependency
       ------------------
       Returns true if the node contains a dependency
       with a name matching resourceName.
    */

    bool containsDependency(std::string resourceName) const;


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

    void removeDependency(std::string depName);


    /* getDependencies
       ---------------
       Returns the list of dependencies.
    */

    const std::list<Node*>* getDependencies() const;

};



