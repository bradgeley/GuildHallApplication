#pragma once
#include <list>
#include <string>

using namespace std;


/* Resource
   --------
   Keeps track of its own dependencies in a list on the heap.
   The dependency pointers need to point to the actual Resource
   rather than pointing to new instances.
   */

class Resource {
public:

    Resource(string name) : name(name) {
        dependencies = new list<Resource*>();
    }

    Resource(string name, Resource* resource) : name(name) {
        dependencies = new list<Resource*>();
        dependencies->push_back(resource);
    }

    ~Resource() {
        delete dependencies;
    }

private:
    string name = "";

public:

    list<Resource*>* dependencies;

    string getName() {
        return name;
    }

    bool containsDependency(string resourceName) {
        for (list<Resource*>::iterator it = dependencies->begin(); it != dependencies->end(); it++) {
            Resource* node = *it;
            if (node->name == resourceName) {
                return true;
            }
        }
        return false;
    }

    void addDependency(Resource* dep) {
        dependencies->push_back(dep);
    }

    void removeDependency(string depName) {
        for (list<Resource*>::iterator it = dependencies->begin(); it != dependencies->end(); it++) {
            Resource* dep = *it;
            if (dep->getName() == depName) {
                dependencies->erase(it);
                break;
            }
        }
    }

};



