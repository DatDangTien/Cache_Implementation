#include "main.h"
Cache::Cache(SearchEngine* s,ReplacementPolicy* r):rp(r),s_engine(s) {}
Cache::~Cache(){
	delete rp;
	delete s_engine;
}
Data* Cache::read(int addr) {
//    cout << "1_______________________________________" << endl;
    int *memIndex;
    Elem *search = s_engine->get(addr, &memIndex);
//  Debug
//    cout << ((search == NULL)?"NULL":"Found") << endl;
    if (search){
        // Heap update
        //Debug
//        std::cout << "Ready to Changkey at index: " << *memIndex << std::endl ;
        rp->changeKey(memIndex);
//        std::cout << "Complete Changkey at index: " << *memIndex << std::endl ;
        //Debug
        //rp->printLB();
        //std::cout << "Print Heap:\n";
        //rp->printRP();
//        std::cout << search->addr << std::endl;
        return search->data;
    }
//    std::cout << "Print Heap:\n";
//    rp->printRP();
//    std::cout << "_________\n";
    return NULL;
}
Elem* Cache::put(int addr, Data* cont) {
//    cout << "2_______________________________________" << endl;
    Elem* newElem = new Elem(addr, cont, 1);
    Elem* elemOut = NULL;
    int *memIndex = new int();
    if (rp->isFull()){
        //Remove from rp and search
        elemOut = rp->replace(memIndex, newElem);
        s_engine->pop(elemOut->addr);
        //Debug
//        s_engine->printInOrder();
    }
    else {
        rp->insert(memIndex, newElem);
    }
    s_engine->put(addr, newElem, memIndex);
    //Debug
//    s_engine->printInOrder();
//    std::cout << "Print Heap:\n";
//    rp->printRP();
//    std::cout << "_________\n";
    return elemOut;
}
Elem* Cache::write(int addr, Data* cont) {
//    cout << "3_______________________________________" << endl;
    //Search
    int *memIndex;
    Elem* search = s_engine->get(addr, &memIndex);
    if (search){
            search->data = cont;
            search->sync = 0;
//            std::cout << "Ready to Changkey at index: " << *memIndex << std::endl ;
            rp->changeKey(memIndex);
//            std::cout << "Complete Changkey at index: " << *memIndex << std::endl ;
            //rp->printLB();
            //std::cout << "Print Heap:\n";
            //rp->printRP();
        }
    else{
        //Add new Elem
        Elem* newElem = new Elem(addr, cont, 0);
        Elem* elemOut = NULL;
        int *memIndex = new int();
        if (rp->isFull()){
            //Remove from rp and search
            //Debug
//            std::cout << "Ready to Replace at index: " << *memIndex << std::endl ;
            elemOut = rp->replace(memIndex, newElem);
            s_engine->pop(elemOut->addr);
            //Debug
//            s_engine->printInOrder();
        }
        else {
            rp->insert(memIndex, newElem);
        }
        s_engine->put(addr, newElem, memIndex);
        //Debug
//        s_engine->printInOrder();
        return elemOut;
    }
//    std::cout << "Print Heap:\n";
//    rp->printRP();
//    std::cout << "_________\n";
    return NULL;
}
void Cache::printRP() {
    rp->printRP();
}
void Cache::printSE() {
    std::cout << "Print BST in inorder:\n";
    s_engine->printInOrder();
    std::cout << "Print BST in preorder:\n";
    s_engine->printPreOrder();
}
void Cache::printLP() {
    rp->printLB();
}

///////////////////////////////////////////////
///////                RP               ///////
///////////////////////////////////////////////

ReplacementPolicy::ReplacementPolicy(){
    this->heap = NULL; 
    this->mem = new PrimeMem* [MAXSIZE]; 
    this->hash = new Hash();
    cacheAcc = 0;
    memCount = 0;
    }
ReplacementPolicy::~ReplacementPolicy(){
    delete this->heap;
    heap = NULL;
    delete [] this->mem;
    mem = NULL;
    delete this->hash;
    hash = NULL;
}
bool ReplacementPolicy::isFull(){
    return memCount >= MAXSIZE;
}
void ReplacementPolicy::insert(int* index, Elem* elem){
    //Add mem table
    int *heapIdx = new int();
    PrimeMem *newMem = new PrimeMem(elem, heapIdx);
    if (!isFull()){
        *index = memCount;
        memCount++;
    }
    mem[*index] = newMem;
    //Add to heap
    cacheAcc++;
    HeapData *data = new HeapData(elem, heapIdx, cacheAcc, *index);
    heap->insert(1, data);
}
void ReplacementPolicy::changeKey(int* index){
    if (heap != NULL){
        //Debug
//        std::cout << "Heap:\n";
//        heap->print();
        int *heapIndex = mem[*index]->heapIndex;
        HeapData* data;
        int key = heap->getKey(*heapIndex, &data);
        key++;
//        std::cout << "Ready to delete heapNode at index " << *(heapIndex) << ", new key = " << key << std::endl;
        heap->del(*heapIndex);
        //Debug
//        std::cout << "Delete heapNode, new key = " << key << std::endl;
        //Create new HeapData
        int *newheapIndex = new int();
        mem[*index]->heapIndex = newheapIndex;
        cacheAcc++;
        Elem *elem = mem[*index]->elem;
        HeapData *newData = new HeapData(elem, newheapIndex, cacheAcc, *index);
        //Debug
//        std::cout << "Ready to insert with key: " << key << " ";
//        elem->print();
        heap->insert(key, newData);
    }
}
void ReplacementPolicy::printLB(){
    std::cout << "Prime memory\n";
//    std::cout << "Mem count = " << memCount << std::endl;
    for (int i = 0; i < memCount; i++){
//        std::cout << "i = " << i << std::endl;
        mem[i]->elem->print();
    }
    //Create hash
    for (int i = 0; i < memCount; i++){
        hash->insert(mem[i]->elem);
    }
    std::cout << "Hash table memory\n";
    hash->print();
}
MFU::MFU(){heap = new MaxHeap();}
LFU::LFU(){heap = new MinHeap();}
MFRU::MFRU(){heap = new MaxHeap();}
LFRU::LFRU(){heap = new MinHeap();}
Elem* MFU::replace(int* newIndex, Elem* elem){
    HeapData *data = heap->top();
    //Copy index
    *newIndex = data->memIndex;
    Elem *elemOut = data->elem;
    //Delete
    heap->del();
    delete mem[*newIndex];
    //Insert
    insert(newIndex, elem);
    return elemOut;
}
Elem* LFU::replace(int* newIndex, Elem* elem){
    HeapData *data = heap->top();
    //Copy index
    *newIndex = data->memIndex;
    Elem *elemOut = data->elem;
    //Delete
    heap->del();
    delete mem[*newIndex];
    //Insert
    insert(newIndex, elem);
    return elemOut;
}
Elem* MFRU::replace(int* newIndex, Elem* elem){
    HeapData *data = heap->topRecent();
    //Copy index
    *newIndex = data->memIndex;
    Elem *elemOut = data->elem;
    //Delete
    heap->del(*data->heapIndex);
    delete mem[*newIndex];
    //Insert
    insert(newIndex, elem);
    return elemOut;
}
Elem* LFRU::replace(int* newIndex, Elem* elem){
    HeapData *data = heap->topRecent();
    //Copy index
    *newIndex = data->memIndex;
    Elem *elemOut = data->elem;
    //Delete
    heap->del(*data->heapIndex);
    delete mem[*newIndex];
    //Insert
    insert(newIndex, elem);
    return elemOut;
}
void MFU::printRP(){
    heap->print();
}
void LFU::printRP(){
    heap->print();
}

void MFRU::printRP(){
    heap->printRecent();
}
void LFRU::printRP(){
    heap->printRecent();
}

///////////////////////////////////////////////
///////               HASH              ///////
///////////////////////////////////////////////

Hash::Hash(){
    cap = MAXSIZE; 
    map = new HashNode* [cap]; 
    for (int i = 0; i < cap; i++){
        map[i] = NULL;
    }
    count = 0;
}
int Hash::hashCode(int key){
    return key%cap;
}
void Hash::insert(Elem *elem){
    int key = elem->addr;
    if (count <= cap){
        HashNode *newNode = new HashNode(elem, key);
        int index = hashCode(key);
        int i = 0;
        while (map[index + i] != NULL){
            i++;
        }
        if (map[index + i] == NULL){
                map[index + i] = newNode;
                count++;
            //Debug
//            std::cout << "Hash inserted: ";
//            map[index + i]->elem->print();
        }
    }
}
void Hash::print(){
    for (int i = 0; i < cap; i++){
        if (map[i])
            map[i]->elem->print();
    }
}

///////////////////////////////////////////////
///////              SEARCH             ///////
///////////////////////////////////////////////

//Pure virtual destructor required body
SearchEngine::~SearchEngine(){}

Elem* BST::get(int sKey, int** index){
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
            *index = current->memIndex;
            return current->data;
        }
    }
    return NULL;
}

void BST::put(int newKey, Elem* newData, int* newIndex){
//    cout << "put: " << newKey << " " << newData->data->getValue() << endl;
    Node* newNode = new Node(newKey, newData, newIndex);
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
    return delNode(this->root, this->root, dltKey);
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
            if (parent != root){
                if (dltKey < parent->key){
                    parent->left = root->right;
                }
                else parent->right = root->right;
            }
            else {
                //Delete at this->root
                this->root = root->right;
            }
            root->right = NULL;
            delete root;
            return 1;
        }
        else if (root->right == NULL){
            if (parent != root){
                if (dltKey < parent->key)
                    parent->left = root->left;
                else parent->right = root->left;
            }
            else{
                this->root = root->left;
            }
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
    //Debug
//    std::cout << "In BST:\n";
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


///////////////////////////////////////////////
///////                HEAP             ///////
///////////////////////////////////////////////

HeapData::HeapData(){
    elem = NULL;
    heapIndex = NULL;
    recent = 0;
    memIndex = 0;
}
HeapData::HeapData(Elem *e, int *h, int r, int m){
    elem = e;
    heapIndex = h;
    recent = r;
    memIndex = m;
}
void HeapData::operator = (const HeapData &H){
        elem = H.elem;
        heapIndex = H.heapIndex;
        recent = H.recent;
        memIndex = H.memIndex;
    }
HeapNode::HeapNode(const HeapNode &H){
    data = new HeapData();
    *data = *H.data;
    key = H.key;    
}
void HeapNode::operator = (const HeapNode &H){
    *data = *H.data;
    key = H.key;
}
void Heap::swap(HeapNode* a, HeapNode* b){
    HeapNode tmp(*a);
    tmp = *a;
    *a = *b;
    *b = tmp;
    tmp.data->heapIndex = NULL;
    int index = *(a->data->heapIndex);
    *(a->data->heapIndex) = *(b->data->heapIndex);
    *(b->data->heapIndex) = index;
    //Debug
//    std::cout << "Swaped: " << b->data->elem->addr << " " << a->data->elem->addr << std::endl;

}


//DEBUG
/*void Heap::print(){
    std::cout << "Heap include:\n";
    for (int i = 0; i < count; i++){
        std::cout << *(heap[i]->data->heapIndex) << " " << heap[i]->key << std::endl;
    }
}*/

void Heap::printRecent(){
    while (count > 0){
        int *top = new int (0);
        findTopRecent(0, top);
        heap[*top]->data->elem->print();
        del(*top);
        delete top;
    }
}
HeapData* Heap::top(){
    return heap[0]->data;
}
HeapData* Heap::topRecent(){
    int *top = new int(0);
    findTopRecent(0, top);
    int pos = *top;
    delete top;
    return heap[pos]->data;
}
int Heap::getKey(int pos, HeapData** data){
    *data = heap[pos]->data;
    return heap[pos]->key;
}
void Heap::insert(int key, HeapData* data){
    if (this->count < this->capacity){
        HeapNode* tmp = new HeapNode(key, data);
        *(tmp->data->heapIndex) = this->count;
        heap[this->count] = tmp;
        reheapUp(this->count); 
        this->count++;
        //Debug 
//        print();
    }    
}
void Heap::del(int pos){
    int last = this->count - 1;
    if (pos != last){
        swap(heap[pos], heap[last]);
    }
    //Debug
//    print();
    this->count--;
    HeapNode* tmp = heap[last];
    heap[last] = NULL;
    delete tmp;
    //Debug
//    print();
//    std::cout << "Deleted heapNode, ready to reheapUp Down\n";
    reheapUp(pos);
    reheapDown(pos);
    //Debug
//    print();
}

void MaxHeap::reheapUp(int current){
    if (!heap[current])
        return;
    int parent = current/2;
    if (current % 2 == 0)
        parent--;
    if (parent >= 0){
        if (heap[current]->key > heap[parent]->key){
            swap(heap[parent], heap[current]);
//            std::cout << "reheapUp: " << heap[parent]->key << " " << heap[current]->key << std::endl;
            reheapUp(parent);
        }
    }
}
void MaxHeap::reheapDown(int current){
    int left = current*2 + 1;
    int right = current*2 + 2;
    if (left < this->count){
        int child;
        if (right < this->count && heap[right]->key >= heap[left]->key){
            child = right;
        }
        else child = left;
        if (heap[current]->key <= heap[child]->key){
            swap(heap[current], heap[child]);
//            std::cout << "reheapDown: " << heap[current]->key << " " << heap[child]->key << std::endl;
            reheapDown(child);
        }    
    }
}
void MaxHeap::print(){
//    std::cout << "In heap: \n";
    for (int i = count - 1; i >= 0; i--){
        //Debug
//        std::cout << "i = " << i << ", key = " << heap[i]->key << ", "; 
        heap[i]->data->elem->print();
    }
}
void MaxHeap::findTopRecent(int current, int *top){
    int left = current*2 + 1;
    int right = current*2 + 2;
    if (left < count){
        if (heap[left]->key == heap[*top]->key){
            if (heap[left]->data->recent > heap[*top]->data->recent){
                *top = left;
            }
            findTopRecent(left, top);
        }
        if (right < count){
            if (heap[right]->key == heap[*top]->key){
                if (heap[right]->data->recent > heap[*top]->data->recent){
                    *top = right;
                }
                findTopRecent(right, top);   
            }
        }
    }
}

void MinHeap::reheapUp(int current){
    if (!heap[current])
        return;
    int parent = current/2;
    if (current % 2 == 0)
        parent--;
    if (parent >= 0){
        if (heap[current]->key < heap[parent]->key){
            swap(heap[parent], heap[current]);
//            std::cout << "reheapUp: " << heap[parent]->key << " " << heap[current]->key << std::endl;
            reheapUp(parent);
        }
    }
}
void MinHeap::reheapDown(int current){
    int left = current*2 + 1;
    int right = current*2 + 2;
    if (left < this->count){
        int child;
        if (right < this->count && heap[right]->key < heap[left]->key){
            child = right;
        }
        else child = left;
        if (heap[current]->key >= heap[child]->key){
            swap(heap[current], heap[child]);
//            std::cout << "reheapDown: " << heap[current]->key << " " << heap[child]->key << std::endl;
            reheapDown(child);
        }    
    }
}
void MinHeap::print(){
//    std::cout << "In heap: \n";
    for (int i = 0; i < count; i++){
//        std::cout << "i = " << i << ", key = " << heap[i]->key << ", "; 
        heap[i]->data->elem->print();
    }
}
void MinHeap::findTopRecent(int current, int *top){
    int left = current*2 + 1;
    int right = current*2 + 2;
    if (left < count){
        if (heap[left]->key == heap[*top]->key){
            if (heap[left]->data->recent < heap[*top]->data->recent){
                *top = left;
            }
            findTopRecent(left, top);
        }
        if (right < count){
            if (heap[right]->key == heap[*top]->key){
                if (heap[right]->data->recent < heap[*top]->data->recent){
                    *top = right;
                }
                findTopRecent(right, top);   
            }
        }
    }
}
