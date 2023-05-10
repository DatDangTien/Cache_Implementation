#ifndef CACHE_H
#define CACHE_H

#include "main.h"

class SearchEngine {
    //TODO
public:
//    SearchEngine();
    virtual ~SearchEngine() = 0;
    virtual Elem* get(int sKey, int** index) = 0;
    virtual void put(int newKey, Elem* newData, int* newIndex) = 0;
    virtual bool pop(int dltKey) = 0;
    virtual void printInOrder() = 0;
    virtual void printPreOrder() = 0;
};

class Heap; //forward declaration
class MaxHeap; //forward declaration
class MinHeap; //forward declaration
class PrimeMem; //forward declaration
class Hash; //forward declaration

class ReplacementPolicy {
protected:
    Heap* heap;
    PrimeMem** mem;
    Hash* hash;
    int memCount;
    int cacheAcc;
public:
    ReplacementPolicy();
    ~ReplacementPolicy();
    bool isFull();
    void insert(int* index, Elem* elem);
    void changeKey(int* index);
    void printLB();
    //Delete and call insert
    virtual Elem* replace(int* newIndex, Elem* elem) = 0;
    virtual void printRP() = 0;
};

class MFU : public ReplacementPolicy {
    //TODO
public:
    MFU();
    ~MFU();
    Elem* replace(int* newIndex, Elem* elem);
    void printRP();
};

class LFU : public ReplacementPolicy {
    //TODO
public:
    LFU();
    ~LFU();
    Elem* replace(int* newIndex, Elem* elem);
    void printRP();
};
class MFRU: public ReplacementPolicy {
    //TODO
public:
    MFRU();
    ~MFRU();
    Elem* replace(int* newIndex, Elem* elem);
    void printRP();
};

class LFRU: public ReplacementPolicy {
    //TODO
public:
    LFRU();
    ~LFRU();
    Elem* replace(int* index, Elem* elem);
    void printRP();
};

class PrimeMem{
private:
    Elem *elem;
    int *heapIndex;
public:
    PrimeMem(){elem = NULL; heapIndex = NULL;}
    PrimeMem(Elem* e, int *idx):elem(e),heapIndex(idx){}
    ~PrimeMem(){}
    friend class ReplacementPolicy;
    friend class MFU;
    friend class LFU;
    friend class MFRU;
    friend class LFRU;
};
class HashNode{
private: 
    Elem *elem;
    int key;
public:
    HashNode(){elem = NULL; key = 0;}
    HashNode(Elem *e, int k):elem(e), key(k){}
    ~HashNode(){}
    friend class Hash;
};
class Hash{
private:
    HashNode **map;
    int count;
    int cap;
public:
    Hash();
    ~Hash(){delete[] map; map = NULL;}
    int hashCode(int key);
    void insert(Elem *elem);
    void print();
};
class BST : public SearchEngine {
    //TODO
public:
    class Node;
private:
    Node* root;
    bool delNode(Node* parent, Node* current, int dltKey);
    void printI(Node* root);
    void printP(Node* root);
public:
    BST(){root = NULL;}
    ~BST(){delete root; root = NULL;}
    Elem* get(int sKey, int** index);
    void put(int newKey, Elem* newData, int* newIndex);
    bool pop(int dltKey);
    void printInOrder();
    void printPreOrder();
public:
    class Node{
    private:
        int key;
        int *memIndex;
        Elem* data;
        Node* left;
        Node* right;
        friend class BST;
    public:
        Node(){data = NULL; left = NULL; right = NULL; memIndex = NULL;}
        Node(int key,Elem* a, int* m):key(key),data(a), memIndex(m), left(NULL),right(NULL){};
        ~Node(){delete left; delete right; delete memIndex; 
                left = NULL; right = NULL; memIndex = NULL;}
    };
};

//Heap
class HeapData{
private:
    Elem *elem;
    int *heapIndex;
    int recent;
    int memIndex;
public: 
    HeapData();
    HeapData(Elem *e, int *h, int r, int m);
    ~HeapData(){delete heapIndex; heapIndex = NULL; 
                //std::cout << "~HeapData(): Deleted HeapIndex.\n";
    }
    void operator = (const HeapData &H);
    friend class Heap;
    friend class MaxHeap;
    friend class MinHeap;
    friend class ReplacementPolicy;
    friend class MFU;
    friend class LFU;
    friend class MFRU;
    friend class LFRU;
};
class HeapNode{
private:
    int key;
    HeapData* data;
public:   
    HeapNode(){key = 0; data = NULL;}
    HeapNode(int k, HeapData* d):key(k),data(d){};
    HeapNode(const HeapNode &H);    
    ~HeapNode(){delete data; data = NULL; 
                //std::cout << "~HeapNode(): Deleted HeapData.\n";
    }
    void operator = (const HeapNode &H);    
    friend class Heap;
    friend class MaxHeap;
    friend class MinHeap;
};

class Heap{
protected:
    HeapNode** heap;
    int count;
    int capacity;
public:
    Heap(){capacity = MAXSIZE; heap = new HeapNode* [capacity];}
    ~Heap(){delete [] heap; heap = NULL;}
    void swap(HeapNode* a, HeapNode* b);
    void printRecent();
    HeapData* top();
    HeapData* topRecent();
    int getKey(int pos, HeapData** data);
    void insert(int key, HeapData* data);
    void del(int pos = 0);
    virtual void findTopRecent(int current, int *top) = 0;
    virtual void reheapUp(int current) = 0;
    virtual void reheapDown(int current) = 0;
    virtual void print() = 0;
//    void increaseCap();
};
class MaxHeap : public Heap{
public:
//    MaxHeap();
    ~MaxHeap();
    void reheapUp(int current);
    void reheapDown(int current);
    void print();
    void findTopRecent(int current, int *top);
//    void increaseCap();
};
class MinHeap : public Heap{
public:
//    MinHeap();
    ~MinHeap();
    void reheapUp(int current);
    void reheapDown(int current);
    void print();
    void findTopRecent(int current, int *top);
//    void increaseCap();
};

#endif