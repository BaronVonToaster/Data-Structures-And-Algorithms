//============================================================================
// Name        : ProjectTwo.cpp
// Author      : James Cummings
// Version     : 1.0
// Copyright   : Copyright  2023 SNHU COCE
// Description : Project 2 Binary Search Tree
//============================================================================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>
#include <algorithm>

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// Forward declarations
double strToDouble(string str, char ch);

// Define Course struct to hold course info
struct Course {
    string courseNumber;
    string name;
    vector<string> prerequisites;
}; // Fixed: Added missing semicolon here

// Define a node structures to act as BST root, internal, and leaf nodes
struct Node {
    Course course; // Holds Course object inside node
    Node* left; //Left child
    Node* right; //Right child
    // Default Constructor
    Node() {
        // Set each child to nulll since root is only instantiated node of BST
        left = nullptr;
        right = nullptr;
    }
    // Overloaded Constructor for easy instantiation
    Node(Course aCourse) {
        course = aCourse;
        left = nullptr;
        right = nullptr;
    }
};

// Define BST class to keep program modular and object oriented
class BinarySearchTree {

private:
    // Root node is initial node of BST
    Node* root;

    void addNode(Node* node, Course course);
    void inOrder(Node* node);
    void clearTree(Node* node);
    Node* removeNode(Node* node, string courseNumber);
    Course* searchNode(Node* node, string courseNumber);

public:
    BinarySearchTree();
    virtual ~BinarySearchTree();
    void printInOrder();
    void Insert(Course course);
    bool isEmpty();
    void Remove(string courseNumber);
    Course* Search(string courseNumber); // Fixed: Changed return type to Course* to match implementation
    void clear();
};

// Constructor definition
BinarySearchTree::BinarySearchTree(){
    root = nullptr;
}
// Destructor helper
void BinarySearchTree::clear(){
    clearTree(root);
    root = nullptr;
}

// Destructor definition
BinarySearchTree::~BinarySearchTree(){
    clear();
}

// Define function to add a node to the BST
void BinarySearchTree::addNode(Node* node, Course course) {
    if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new Node(course);
        } else {
            addNode(node->left, course);
        }
    } else {
        if (node->right == nullptr) {
            node->right = new Node(course);
        } else {
            addNode(node->right, course);
        }
    }
}

// Define function to order the items in the tree
void BinarySearchTree::inOrder(Node* node) {
    if (node != nullptr) {
        inOrder(node->left);
        cout << node->course.courseNumber << ", " << node->course.name << endl;
        inOrder(node->right);
    }
}

// Helper to recursively clear tree memory
void BinarySearchTree::clearTree(Node* node) {
    if (node != nullptr) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
    }
}

// Define helper to print the tree in order
void BinarySearchTree::printInOrder() {
    if (root == nullptr) {
        cout << "No data loaded. Please load data first." << endl;
    } else {
        cout << "Here is a sample schedule:" << endl;
        inOrder(root);
    }
}

Node* BinarySearchTree::removeNode(Node* node, string courseNumber) {
    // Standard BST removal logic placeholder if needed by interface
    if (node == nullptr) return nullptr;
    if (courseNumber < node->course.courseNumber) {
        node->left = removeNode(node->left, courseNumber);
    } else if (courseNumber > node->course.courseNumber) {
        node->right = removeNode(node->right, courseNumber);
    } else {
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            return nullptr;
        } else if (node->left == nullptr) {
            Node* temp = node->right;
            delete node;
            return temp;
        } else if (node->right == nullptr) {
            Node* temp = node->left;
            delete node;
            return temp;
        }
    }
    return node;
}

void BinarySearchTree::Insert(Course course) {
    if (root == nullptr) {
        root = new Node(course);
    } else {
        addNode(root, course);
    }
}

// Recursive search helper
Course* BinarySearchTree::searchNode(Node* node, string courseNumber) {
    if (node == nullptr) {
        return nullptr;
    }
    if (node->course.courseNumber == courseNumber) {
        return &(node->course);
    }
    if (courseNumber < node->course.courseNumber) {
        return searchNode(node->left, courseNumber);
    } 
    else {
        return searchNode(node->right, courseNumber);
    }
}

// Function to determine if the BST is empty or not
bool BinarySearchTree::isEmpty() {
    return root == nullptr;
}

// Funciton to remove a specified course
void BinarySearchTree::Remove(string courseNumber) {
    root = removeNode(root, courseNumber);
}

// Function to search and locate a specified course from its course number
Course* BinarySearchTree::Search(string courseNumber) {
    return searchNode(root, courseNumber);
}

// Helper function to trim whitespace and carriage returns from CSV fields
string trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == string::npos) return "";
    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

// Function to load file data into the Binary Search Tree with two-pass validation
bool loadFileToBST(string filename, BinarySearchTree &bst) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Error: Could not open file " << filename << endl;
        return false;
    }

    // Clear any previously loaded data before reloading 
    bst.clear();
    vector<Course> tempCourseList;
    string curLine;

    // Pass 1: Read lines, validate fields, instantiate objects, and store in BST
    while (getline(file, curLine)) {
        if (curLine.empty()) continue;

        stringstream ss(curLine);
        string item;
        vector<string> elements;

        while (getline(ss, item, ',')) {
            elements.push_back(trim(item));
        }

        if (elements.size() < 2) {
            cout << "Error: Incomplete record encountered -> " << curLine << endl;
            continue;
        }

        Course newCourse;
        newCourse.courseNumber = elements[0];
        newCourse.name = elements[1];

        if (elements.size() > 2) {
            for (size_t i = 2; i < elements.size(); ++i) {
                if (!elements[i].empty()) {
                    newCourse.prerequisites.push_back(elements[i]);
                }
            }
        }

        bst.Insert(newCourse);
        tempCourseList.push_back(newCourse);
    }

    file.close();

    // Pass 2: Validate that every prerequisite exists in the loaded dataset after loading completes
    for (const auto& course : tempCourseList) {
        for (const auto& prereqID : course.prerequisites) {
            if (bst.Search(prereqID) == nullptr) {
                cout << "Error: Prerequisite " << prereqID << " referenced by " << course.courseNumber << " does not exist in file." << endl;
            }
        }
    }

    cout << "Data successfully loaded into Binary Search Tree." << endl;
    return true;
}

// Function to print specific course details and its prerequisites
void searchAndPrintCourseBST(BinarySearchTree &bst, string courseNumber) {
    // Convert search input to uppercase for robust matching
    transform(courseNumber.begin(), courseNumber.end(), courseNumber.begin(), ::toupper);

    Course* course = bst.Search(courseNumber);
    if (course == nullptr) {
        cout << "Error: Course " << courseNumber << " was not found." << endl;
        return;
    }

    cout << course->courseNumber << ", " << course->name << endl;
    if (course->prerequisites.empty()) {
        cout << "Prerequisites: None" << endl;
    } else {
        cout << "Prerequisites: ";
        for (size_t i = 0; i < course->prerequisites.size(); ++i) {
            cout << course->prerequisites[i];
            if (i < course->prerequisites.size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }
}

// Driver function
int main() {
    // Initialize main variables
    vector<Course> coursesVector;
    BinarySearchTree bst;
    int userSelection = 0;
    string filename = "";
    string key = "";

    cout << "Welcome to the course planner." << endl;

    // Loop actively to watch for user input for menu
    while (userSelection != 9) {
        cout << "\nMenu:" << endl;
        cout << "  1. Load Data Structure." << endl;
        cout << "  2. Print Course List" << endl;
        cout << "  3. Print Course" << endl;
        cout << "  9. Exit" << endl;
        cout << "What would you like to do? ";

        // Input validation check
        if (!(cin >> userSelection)) {
            cout << "Invalid input. Please enter a valid menu number." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        // Switch to handle each menu option
        switch (userSelection) {
            case 1: 
                // Requests load file name
                cout << "Enter name of file to load: ";
                cin >> filename;
                // Runs load file member function and clears previous data
                loadFileToBST(filename, bst);
                break; // Exit switch
                
            case 2:
                // Prints all courses in order of sort
                bst.printInOrder();
                break;
                
            case 3: 
                if (bst.isEmpty()) { 
                    cout << "No data loaded. Please load data first." << endl;
                } 
                else { 
                    cout << "What course do you want to know about? ";
                    cin >> key;
                    searchAndPrintCourseBST(bst, key);
                } 
                break;
                
            case 9: 
                cout << "Thank you for using the course planner!" << endl;
                break;
                
            // When userSelection does not match the cases above
            default: 
                cout << userSelection << " is not a valid option. Please try again." << endl;
                // Returns to loop beginning
                break;
        } 
    } 

    return 0;
}