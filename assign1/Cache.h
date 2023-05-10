#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class BST{
public:
    class Node;
private:
    Node* root;
    bool delNode(Node* parent, Node* current, int dltKey);
    void printI(Node* root);
    void printP(Node* root);
public:
    BST(){root = NULL;}
    ~BST(){delete root;}
    Elem* get(int sKey);
    void put(int newKey, Elem* newData);
    bool pop(int dltKey);
    void printInOrder();
    void printPreOrder();
public:
    class Node{
    private:
        int key;
        Elem* data;
        Node* left;
        Node* right;
        friend class BST;
    public:
        Node(){data = NULL; left = NULL; right = NULL;}
        Node(int key,Elem* a):key(key),data(a),left(NULL),right(NULL){};
        ~Node(){delete left; delete right;}
    };
};

class Cache {
        Elem** arr;
        int p;
        BST* tree;
	public:
    Cache(int s) {
        arr = new Elem*[s];
        p = 0;
        tree = new BST();
    }
    ~Cache() {
        delete[] arr;
        delete tree;
    }
		Data* read(int addr);
		Elem* put(int addr, Data* cont);
		Elem* write(int addr, Data* cont);
		void print();
		void preOrder();
		void inOrder();
        Elem* remove(int addr);
        Elem* fifo();
        Elem* lifo();
};
#endif