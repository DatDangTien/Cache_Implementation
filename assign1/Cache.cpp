#include "main.h"

Data* Cache::read(int addr) {
//    cout << "1" << endl;
    //int index = 0;
    //Search
    /*while (index < MAXSIZE && index < p){
        if (addr == arr[index]->addr){
            return arr[index]->data;
        }
        index++;
    }*/
    Elem* search = this->tree->get(addr);
//    cout << ((search == NULL)?"NULL":"Found") << endl;
    if (search) return search->data;
    return NULL;
}
Elem* Cache::fifo(){
    //Remove first elem
    Elem* tmp = arr[0];
    //Shift Elem 
    for (int i = 1; i < MAXSIZE; i++){
        arr[i-1] = arr[i];
    }
    arr[MAXSIZE-1] = NULL;
    p--;
    //Remove from BST
    this->tree->pop(tmp->addr);
    return tmp;
}
Elem* Cache::lifo(){
    //Remove last elem
    Elem* tmp = arr[MAXSIZE-1];
    arr[MAXSIZE-1] = NULL;
    p--;
    //Remove from BST
    this->tree->pop(tmp->addr);
    return tmp;
}
Elem* Cache::remove(int addr){
    return ((addr%2 == 0)?fifo():lifo());
}
Elem* Cache::put(int addr, Data* cont) {
//    cout << "2" << endl;
    Elem* newElem = new Elem(addr, cont, 1);
    Elem* tmp = NULL;
    if (p == MAXSIZE){
        tmp = remove(addr);
    }
    arr[p] = newElem;
    p++;
    this->tree->put(addr, newElem);
    return tmp;
}
Elem* Cache::write(int addr, Data* cont) {
//    cout << "3" << endl;
    bool isFound = 0;
    if (p > 0){
        //Search
        //int index = 0;
        Elem* search = this->tree->get(addr);
        if (search){
            search->data = cont;
            search->sync = 0;
            isFound = 1;
        }
    }
    if (!isFound){
        //Add new Ele
        Elem* newElem = new Elem(addr, cont, 0);  
        Elem* tmp = NULL;
        if (p == MAXSIZE){
            tmp = remove(addr);
        }
        arr[p] = newElem;
        p++;
        this->tree->put(addr, newElem);
        return tmp;   
    }
    return NULL;
}
void Cache::print() {
	for (int i = p-1; i >= 0; i--)
        arr[i]->print();
}
void Cache::preOrder() {
	this->tree->printPreOrder();
}
void Cache::inOrder() {
    this->tree->printInOrder();
}


Elem* BST::get(int sKey){
//    cout << "get: " << sKey << endl;
    if (root == NULL)
        return NULL;
    Node* current = root;
    while (current != NULL){
        if (sKey < current->key){
            current = current->left;
        }
        else if (sKey > current->key){
            current = current->right;
        }
        //Found Node
        else {
            return current->data;
        }
    }
    return NULL;
}

void BST::put(int newKey, Elem* newData){
//    cout << "put: " << newKey << " " << newData->data->getValue() << endl;
    Node* newNode = new Node(newKey, newData);
    if (root == NULL)
        root = newNode;
    else{
        Node* current, * parent;
        current = root;
        while (current != NULL){
            parent = current;
            if (newKey < current->key)
                current = current->left;
            else current = current->right;
        }
        if (newKey < parent->key){
            parent->left = newNode;
        }
        else {
            parent->right = newNode;
        }
    }
}


bool BST::pop(int dltKey){
//    cout << "DEL: " << dltKey << endl;
    return delNode(NULL, this->root, dltKey);
}                                                                                                  


bool BST::delNode(Node* parent, Node* current, int dltKey){
    Node* root = current;
    if (root == NULL) return 0;
    if (dltKey < root->key) 
        return delNode(root, root->left, dltKey);
    else if (dltKey > root->key)
        return delNode(root, root->right, dltKey);
    else {
        if (root->left == NULL){
            if (dltKey < parent->key)
                parent->left = root->right;
            else parent->right = root->right;
            root->right = NULL;
            delete root;
            return 1;
        }
        else if (root->right == NULL){
            if (dltKey < parent->key)
                parent->left = root->left;
            else parent->right = root->left;
            root->left = NULL;
            delete root;
            return 1;
        }
        else {
            Node* tmp = root->right;
            Node* tmp2 = root;
            while (tmp->left != NULL){
                tmp2 = tmp;
                tmp = tmp->left;
            }
            root->key = tmp->key;
            root->data = tmp->data;
            return delNode(tmp2, tmp, tmp->key);
        }
    }
    return 0;
}


void BST::printInOrder(){
    return printI(this->root);
}

void BST::printI(Node* root){
    if (root != NULL){
        printI(root->left);
        //Print root
        root->data->print();
        printI(root->right);
    }
}

void BST::printPreOrder(){
    return printP(this->root);
}

void BST::printP(Node* root){
    if (root != NULL){
        //Print root
        root->data->print();
        printP(root->left);
        printP(root->right);
    }
}