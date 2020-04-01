#pragma once
#include "Resource.h"
#include <string>
#include <fstream>

using namespace std;

/* Resource Manager
   ----------------
   Keeps track of a list of resources, and is responsible
   for the logic of adding and deleting resources.
   */

class ResourceManager {
public:

    /* Constructor/Destructor */

    ResourceManager() {
        res = new list<Resource*>();
    }

    ~ResourceManager() {
        for (list<Resource*>::iterator it = res->begin(); it != res->end(); it++) {
            delete* it;
        }
        delete res;
    }

private:
    list<Resource*>* res;

public:
    bool containsResource(string resourceName) {
        for (list<Resource*>::iterator it = res->begin(); it != res->end(); it++) {
            Resource* node = *it;
            if (node->getName() == resourceName) {
                return true;
            }
        }
        return false;
    }

    void addResource(string name) {
        if (!containsResource(name)) {
            res->push_back(new Resource(name));
        }
    }

    void addResource(string name, string nameOfDependency) {
        if (!containsResource(name)) {
            res->push_back(new Resource(name));
        }
        if (!containsResource(nameOfDependency)) {
            res->push_back(new Resource(nameOfDependency));
        }
        Resource* dep = getResource(nameOfDependency);
        Resource* res = getResource(name);
        if (!res->containsDependency(nameOfDependency)) {
            res->addDependency(dep);
        }
    }

    Resource* getResource(string resourceName) {
        for (list<Resource*>::iterator it = res->begin(); it != res->end(); it++) {
            Resource* node = *it;
            if (node->getName() == resourceName) {
                return node;
            }
        }
        return nullptr;
    }

    void remResource(string name) {
        list<Resource*>::iterator it;
        list<Resource*>::iterator toRemove;
        for (it = res->begin(); it != res->end(); it++) {
            Resource* r = *it;
            if (r->getName() == name) {
                toRemove = it;
            }
            if (r->containsDependency(name)) {
                r->removeDependency(name);
            }
        }
        res->erase(toRemove);
    }

    void readFile(string filename) {
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
        for (list<Resource*>::iterator it = res->begin(); it != res->end(); it++) {
            Resource* node = *it;
            cout << node->getName() << endl;
        }
    }

    void print() {

        /* Declare window width */

        const int MAX_NAME_LEN = 15;
        const int totalWidth = MAX_NAME_LEN * 2 + 3;

        /* Print Header */

        cout << endl << setw(MAX_NAME_LEN + 2) << setfill(' ') << left << "Resource" << "Dependencies" << endl;
        cout << setw(totalWidth) << setfill('-') << "" << endl;

        /* Print Resources, but only if they have dependencies */

        for (list<Resource*>::iterator it = res->begin(); it != res->end(); it++) {

            Resource* node = *it;
            string name = node->getName();
            if (name.length() > MAX_NAME_LEN) { name = name.substr(0, MAX_NAME_LEN - 1); }

            list<Resource*>* deps = node->dependencies;

            /* Print Lines */

            if (deps->size() > 0) {

                bool firstLine = true;
                for (list<Resource*>::iterator d = deps->begin(); d != deps->end(); d++) {
                    string depName = (*d)->getName();
                    if (depName.length() > MAX_NAME_LEN) { depName = depName.substr(0, MAX_NAME_LEN - 1); }
                    if (!firstLine) name = "";
                    cout << setfill(' ') << "|" << left << setw(MAX_NAME_LEN) << name << "|" << setw(MAX_NAME_LEN) << depName << "|" << endl;
                    firstLine = false;
                }

                cout << setfill('-') << setw(totalWidth) << "" << endl;

            }
        }
    }
};