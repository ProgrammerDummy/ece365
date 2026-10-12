#include "heap.h"

#include <cassert>
#include <algorithm>


heap::heap(int cap) : capacity(std::max(0, cap)), curr_size(0), nodes(capacity+1), hashtable(3*capacity) {

}  

/*

 Member init list: capacity, curr_size = 0, nodes sized capacity + 1, hashtable sized ~2 * capacity 
 (your λ is 0.5, so this is what makes a rehash unlikely)
 
 Verify member declaration order matches initialization order
*/

int heap::insert(const std::string &id, int key, void *pv) {
    return 0; //placeholder for now
}

int heap::setKey(const std::string &id, int key) {
    return 0; //placeholder for now
}

int heap::deleteMin(std::string *pId, int *pKey, void *ppData) {
    return 0; //placeholder for now
}

int heap::remove(const std::string &id, int *pKey, void *ppData) {
    return 0; //placeholder for now
}

void heap::percolateUp(int posCur) {
    //in this context, posCur is the index of the node that may be too small for its current position
    //while loop with checks
    //i/2 is parent node

    assert(posCur > 0);
    assert(posCur <= capacity);

    heapNode temp = nodes[posCur];
    
    //hole is at nodes[posCur] now

    while(posCur > 1 && temp.key < nodes[posCur/2].key) {
        updateMapping(posCur, nodes[posCur/2]);
        posCur = posCur/2;
    }

    updateMapping(posCur, temp);
    
}

void heap::percolateDown(int posCur) {

}

int heap::getIndex(const std::string& id) {
    void* ptr = hashtable.getPointer(id);

    if(ptr == nullptr) {
        //in this context 0 is impossible, signifies the id could not be found within hashtable
        return 0;
    }

    return static_cast<heapNode*>(ptr) - &nodes[0];
}

void heap::updateMapping(int index, heapNode& node) {

    nodes[index] = std::move(node);

    int result = hashtable.setPointer(nodes[index].id, &nodes[index]);
    //if -DNDEBUG is defined, assert would not run if hashtable.setPointer(etc) was argument, separated into two steps

    //by the time a node is relocated, it must be there
    assert(result == 0);
    (void)result; //due to -Wunused-variable, this will silence the warning, counts as a use
}
