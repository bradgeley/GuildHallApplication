// ResourceManager.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <exception>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;

string FILENAME = "Resources.txt";
const size_t ARRAY_SIZE = 100;

class Node {
public:
    Node() { }
    Node(string name) : name(name) { }
    Node(string name, Node* resource): name(name) { 
        resources[0] = resource;
        numResources++;
    }
    string name = "";
    Node* resources[ARRAY_SIZE];
    size_t numResources = 0;

    bool containsResource(string resourceName) {
        for (size_t i = 0; i < numResources; i++) {
            if (resources[i]->name == resourceName) {
                return true;
            }
        }
        return false;
    }

    void addResource(Node* resource) {
        resources[numResources] = resource;
        numResources++;
    }
};

bool containsNode(const Node nodes[], const size_t size, const string name) {
    for (size_t i = 0; i < size; i++) {
        if (nodes[i].name == name) {
            return true;
        }
    }
    return false;
}

void deleteNode(Node nodes[], size_t &numNodes, const string name) {
    for (size_t i = 0; i < numNodes; i++) {
        Node node = nodes[i];
        for (size_t j = 0; j < node.numResources; j++) {
            Node* resource = node.resources[j];
            if (name == resource->name) {
                memcpy(&node.resources[j], &node.resources[node.numResources - 1], sizeof(node.resources[j]));
                //node.resources[j] = node.resources[node.numResources - 1];
                nodes[i].numResources--;
            }
        }
    }
    for (size_t i = 0; i < numNodes; i++) {
        Node node = nodes[i];
        if (node.name == name) {
            memcpy(&nodes[i], &nodes[numNodes - 1], sizeof(nodes[i]));
            //nodes[i] = nodes[numNodes - 1];
            numNodes--;
            break;
        }
    }
}

Node* getNode(Node nodes[], const size_t size, const string name) {
    for (size_t i = 0; i < size; i++) {
        if (nodes[i].name == name) {
            return &nodes[i];
        }
    }
    return nullptr;
}

void printNodes(Node nodes[], const size_t size) {
    cout << endl << "|" << setw(12) << left << "Resources" << "|" << setw(32) << left << "Dependencies" << "|" << endl;
    cout << "-----------------------------------------------" << endl;
    for (size_t i = 0; i < size; i++) {
        cout << "|" << setw(12) << nodes[i].name << "|";
        string dependencies = "";
        for (size_t n = 0; n < nodes[i].numResources; n++) {
            dependencies += nodes[i].resources[n]->name;
            if (n != nodes[i].numResources - 1) { 
                dependencies += ", ";
            }
        }
        cout << left << setw(32) << dependencies << "|" << endl;
    }
    cout << "-----------------------------------------------" << endl;
}

void addNodes(Node nodes[], size_t &numNodes, const string lhs, const string rhs) {
    Node* lhsNode = nullptr;
    Node* rhsNode = nullptr;
    if (!containsNode(nodes, numNodes, rhs)) {
        nodes[numNodes] = Node(rhs);
        rhsNode = &nodes[numNodes];
        numNodes++;
    }
    else {
        rhsNode = getNode(nodes, numNodes, rhs);
    }
    if (!containsNode(nodes, numNodes, lhs)) {
        nodes[numNodes] = Node(lhs);
        lhsNode = &nodes[numNodes];
        numNodes++;
    }
    else {
        lhsNode = getNode(nodes, numNodes, lhs);
    }
    if (!lhsNode->containsResource(rhs)) {
        lhsNode->addResource(rhsNode);
    }
}

void init(Node nodes[], size_t &numNodes) {
    ifstream stream;
    try {
        stream.open(FILENAME);
        int indexOfWhiteSpace = 0;
        while (!stream.eof()) {
            string str = "", lhs = "", rhs = "";
            getline(stream, str);

            for (size_t i = 0; i < str.length(); i++) {
                if (str[i] == ' ') {
                    indexOfWhiteSpace = i;
                    lhs += str.substr(0, i);
                }
                if (i == str.length() - 1) {
                    rhs += str.substr(indexOfWhiteSpace + 1, str.length());
                }
            }
            addNodes(nodes, numNodes, lhs, rhs);
        }
        stream.close();
    }
    catch (exception error) {
        cout << "Invalid file." << endl;
    }
}

void userInputLoop(Node nodes[], size_t &numNodes) {
    while (true) {
        printNodes(nodes, numNodes);
        cout << "Enter command:";
        string input = "";
        cin >> input;
        if (input == "add") {
            string lhs, rhs;
            cin >> lhs;
            cin >> rhs;
            addNodes(nodes, numNodes, lhs, rhs);
        }
        if (input == "delete") {
            string name;
            cin >> name;
            deleteNode(nodes, numNodes, name);
        }
        if (input == "q") {
            break;
        }
    }
}


//TODO: Swap to vectors and use heap, take care of memory leaks
int main()
{
    //Set up Node data storage
    Node nodes[ARRAY_SIZE];
    size_t numNodes = 0;

    //Read input file
    init(nodes, numNodes);
    userInputLoop(nodes, numNodes);
}