
#include <vector>
#include <string>
#include <iostream>
#include <cassert>

#include "allocator.h"

using namespace std;

class KeyNotFoundException: public exception {};
class WrongTreeSizeException: public exception {};
class TreeNotSortedException: public exception {};

struct Payload {
    virtual void recursiveTeardown() = 0;
};

struct BasicPayload: public Payload {
    vector<int> array;
    string s;

    void recursiveTeardown() {
        delete this;
    }
};

struct CompoundPayload: public Payload {
    Payload* left;
    Payload* right;
    
    void recursiveTeardown() {
        this->left->recursiveTeardown();
        this->right->recursiveTeardown();
        delete this;
    }
};

//template <typename T>
typedef Payload* T;
class SplayTree {
public:
    class SplayTreeNode {
    public:
        double key;
        T value;
        SplayTreeNode* left;
        SplayTreeNode* right;

        SplayTreeNode(double key, T value):
            key(key), value(value), left(nullptr), right(nullptr) {}

        template<typename U>
        void traverse(U f) {
            SplayTreeNode* current = this;
            while (current != nullptr) {
                SplayTreeNode* left = current->left;
                if (left != nullptr) {
                    left->traverse(f);
                }
                f(current);
                current = current->right;
            }
        }


        void recursiveTeardown(Allocator<SplayTreeNode>* nodeAllocator) {
            if (this->left != nullptr) {
                this->left->recursiveTeardown(nodeAllocator);
            }
            if (this->right != nullptr) {
                this->right->recursiveTeardown(nodeAllocator);
            }
            this->value->recursiveTeardown();
            //delete this;
            this->~SplayTreeNode();
            nodeAllocator->deallocate(this);
        }

        ~SplayTreeNode() {
            // assert(this->left == nullptr);
            // assert(this->right == nullptr);
            //this->value->recursiveTeardown();
        }
    };

private:
    SplayTreeNode* root;

public:

    Allocator<SplayTreeNode>* nodeAllocator;

    SplayTree(): root(nullptr) {}
    ~SplayTree() {
        //this->root->recursiveTeardown();
    }


    bool isEmpty() const {
        return root == nullptr;
    }

    void insert(double key, T value) {
        if (this->isEmpty()) {
            this->root = new (nodeAllocator->allocate()) SplayTreeNode(key, value);
            return;
        }
        this->splay(key);
        if (this->root->key == key) {
            return;
        }
        SplayTreeNode* node = new (nodeAllocator->allocate()) SplayTreeNode(key, value);
        if (key > this->root->key) {
            node->left = this->root;
            node->right = this->root->right;
            this->root->right = nullptr;
        } else {
            node->right = this->root;
            node->left = this->root->left;
            this->root->left = nullptr;
        }
        this->root = node;
    }

    void remove(double key) {
        if (this->isEmpty()) {
            throw KeyNotFoundException();
        }
        this->splay(key);
        if (this->root->key != key) {
            throw KeyNotFoundException();
        }
        SplayTreeNode* removed = this->root;
        if (this->root->left == nullptr) {
            this->root = this->root->right;
        } else {
            SplayTreeNode* right = this->root->right;
            this->root = this->root->left;
            this->splay(key);
            this->root->right = right;
        }
        //delete removed;
        removed->~SplayTreeNode();
        nodeAllocator->deallocate(removed);
    }

    SplayTreeNode* find(double key) {
        if (this->isEmpty()) {
            return nullptr;
        }
        this->splay(key);
        return this->root->key == key ? this->root : nullptr;
    }

    SplayTreeNode* findMax(SplayTreeNode* opt_startNode) {
        if (this->isEmpty()) {
            return nullptr;
        }
        SplayTreeNode* current = opt_startNode == nullptr ? this->root : opt_startNode;
        while (current->right != nullptr) {
            current = current->right;
        }
        return current;
    }

    SplayTreeNode* findGreatestLessThan(double key) {
        if (this->isEmpty()) {
            return nullptr;
        }
        this->splay(key);
        if (this->root->key < key) {
            return this->root;
        }
        else if (this->root->left != nullptr) {
            return this->findMax(this->root->left);
        }
        else {
            return nullptr;
        }
    }

    vector<double> exportKeys() {
        vector<double> result;
        this->root->traverse([&result](SplayTreeNode* node) {
            result.push_back(node->key);
        });
        return result;
    }

    void splay(double key) {
        if (this->isEmpty()) {
            return;
        }
        SplayTreeNode dummy = SplayTreeNode(-1, nullptr);
        SplayTreeNode* left = &dummy;
        SplayTreeNode* right = &dummy;
        SplayTreeNode* current = this->root;
        while (true) {
            if (key < current->key) {
                if (current->left == nullptr) {
                    break;
                }
                if (key < current->left->key) {
                    SplayTreeNode* tmp = current->left;
                    current->left = tmp->right;
                    tmp->right = current;
                    current = tmp;
                    if (current->left == nullptr) {
                        break;
                    }
                }
                right->left = current;
                right = current;
                current = current->left;
            }
            else if (key > current->key) {
                if (current->right == nullptr) {
                    break;
                }
                if (key > current->right->key) {
                    SplayTreeNode* tmp = current->right;
                    if (current->right == nullptr) {
                        break;
                    }
                }
                left->right = current;
                left = current;
                current = current->right;
            }
            else {
                break;
            }
        }
        left->right = current->left;
        right->left = current->right;
        current->left = dummy.right;
        current->right = dummy.left;
        this->root = current;
    }
};


Payload* generatePayloadTree(int depth, string tag) {
    if (depth == 0) {
        BasicPayload* out = new BasicPayload();
        out->array = vector<int>{0, 1, 2, 3, 4, 5 ,6, 7, 8, 9};
        out->s = "String for key " + tag + " in leaf node";
        return out;
    }
    CompoundPayload* payload = new CompoundPayload();
    payload->left = generatePayloadTree(depth - 1, tag);
    payload->right = generatePayloadTree(depth - 1, tag);
    return payload;
}

SplayTree* splayTree = nullptr;
int kSplayTreeSize = 80000;
int kSplayTreeModifications = 80;
int kSplayTreePayloadDepth = 5;


double GenerateKey() {
    return rand() / (double)RAND_MAX;
}

double InsertNewNode() {
    double key = -1;
    do {
        key = GenerateKey();
    }
    while (splayTree->find(key) != nullptr);
    Payload* payload = generatePayloadTree(kSplayTreePayloadDepth, to_string(key));
    splayTree->insert(key, payload);
    return key;
}

void splaySetup() {
    splayTree = new SplayTree();
    splayTree->nodeAllocator = new Allocator<SplayTree::SplayTreeNode>(kSplayTreeSize + 1);
    for (int i = 0; i < kSplayTreeSize; i++) {
        InsertNewNode();
    }
}

void splayTearDown() {
    vector<double> keys = splayTree->exportKeys();
    // delete splayTree;
    splayTree = nullptr;

    int length = keys.size();
    if (length != kSplayTreeSize) {
        throw WrongTreeSizeException();
    }

    for (int i = 0; i < length - 1; i++) {
        if (keys[i] >= keys[i + 1]) {
            throw TreeNotSortedException();
        }
    }
}

void splayRun() {
    for (int i = 0; i < kSplayTreeModifications; i++) {
        double key = InsertNewNode();
        SplayTree::SplayTreeNode* greatest = splayTree->findGreatestLessThan(key);
        if (greatest == nullptr) {
            splayTree->remove(key);
        }
        else {
            splayTree->remove(greatest->key);
        }
    }
}

int main(int argc, char** argv) {
    
    int iterations = atoi(argv[1]);
    kSplayTreeModifications = atoi(argv[2]);
    kSplayTreeSize = atoi(argv[3]);
    for (int i = 0; i < iterations; i++) {
        splaySetup();
        splayRun();
        splayTearDown();
    }
    return 0;
}
