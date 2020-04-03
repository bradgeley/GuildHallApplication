#include "Node.h"

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



/* Constructor/Destructor */


/* Node(string name)
   -----------------
   Set the name and create an empty list of dependencies on the heap.
 */

Node::Node(string name) : name(name) {
    dependencies = new list<Node*>();
}


/* Node(string name, Node* resource)
   ---------------------------------
   Set the name and create an empty list of dependencies on the heap,
   adding the pointer to the resource as the first item.
 */

Node::Node(string name, Node* resource) : name(name) {
    dependencies = new list<Node*>();
    dependencies->push_back(resource);
}


/* ~Node
   -----
   Does not free any memory of the pointers contained in dependencies,
   but rather leaves that job to the ResourceManager. The only job here
   is to free dependencies itself.
 */

Node::~Node() {
    delete dependencies;
}



/* Methods */



/* getName
   -------
   Returns the name of the Node.
*/

const string Node::getName() const {
    return name;
}


/* containsDependency
   ------------------
   Returns true if the resource contains a dependency
   with a name matching resourceName.
*/

bool Node::containsDependency(string resourceName) const {
    for (list<Node*>::const_iterator it = dependencies->begin(); it != dependencies->end(); it++) {
        Node* node = *it;
        if (node->name == resourceName) {
            return true;
        }
    }
    return false;
}


/* addDependency
   -------------
   Adds a dependency to the current list, if it does not
   already contain it.

   This function should be given a pointer to a resource
   contained in the Resource Manager's std::list.
*/

void Node::addDependency(Node* dep) {
    dependencies->push_back(dep);
}


/* removeDependency
   ----------------
   If the resource contains the dependency with depName,
   then remove the pointer from our list.
*/

void Node::removeDependency(string depName) {
    for (list<Node*>::iterator it = dependencies->begin(); it != dependencies->end(); it++) {
        Node* dep = *it;
        if (dep->getName() == depName) {
            dependencies->erase(it);
            break;
        }
    }
}


/* getDependencies
   ---------------
   Returns the list of dependencies.
*/

const list<Node*>* Node::getDependencies() const {
    return dependencies;
}