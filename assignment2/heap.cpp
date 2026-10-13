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
    if(curr_size == capacity) {
        return 1;
    }

    if(getIndex(id)) {
        return 2;
    }

    nodes[curr_size+1].id = id;
    nodes[curr_size+1].key = key;
    nodes[curr_size+1].data_ptr = pv;

    int result = hashtable.insert(id, &nodes[curr_size+1]);

    assert(result != 2);

    curr_size++;

    percolateUp(curr_size);

    (void)result;
    return 0;
}

int heap::setKey(const std::string &id, int key) {
    
    int index = getIndex(id);

    if(index == 0) {
        return 1;
    }

    int old_key = nodes[index].key;

    if(key == old_key) {
        return 0;
    }

    nodes[index].key = key;
    
    if(key < old_key) {
        percolateUp(index);
    }

    else {
        percolateDown(index);
    }

    return 0; 
}

int heap::deleteMin(std::string *pId, int *pKey, void *ppData) {
    if(curr_size == 0) {
        return 1;
    }

    if(pId != nullptr) {
        *pId = nodes[1].id;
    }

    if(pKey != nullptr) {
        *pKey = nodes[1].key;
    }

    if(ppData != nullptr) {
        *static_cast<void**>(ppData) = nodes[1].data_ptr;
    }

    hashtable.remove(nodes[1].id);

    if(curr_size == 1) {
        curr_size--;
        return 0;
    }

    updateMapping(1, nodes[curr_size]);

    curr_size--;

    percolateDown(1);

    return 0; 
}

int heap::remove(const std::string &id, int *pKey, void *ppData) {
    int index = getIndex(id);

    if(index == 0) {
        return 1;
    }


    if(pKey != nullptr) {
        *pKey = nodes[index].key;
    }

    if(ppData != nullptr) {
        *static_cast<void**>(ppData) = nodes[index].data_ptr;
    }

    int old_key = nodes[index].key;

    hashtable.remove(nodes[index].id);

    if(index == curr_size) {
        curr_size--;
        return 0;
    }

    updateMapping(index, nodes[curr_size]);

    curr_size--;

    if(nodes[index].key < old_key) {
        percolateUp(index);
    }

    else {
        percolateDown(index);
    }

    return 0; 
}

void heap::percolateUp(int posCur) {
    //in this context, posCur is the index of the node that may be too small for its current position
    //while loop with checks
    //i/2 is parent node

    assert(posCur > 0);
    assert(posCur <= curr_size);

    heapNode temp = std::move(nodes[posCur]);
    
    //hole is at nodes[posCur] now

    while(posCur > 1 && temp.key < nodes[posCur/2].key) {
        updateMapping(posCur, nodes[posCur/2]);
        posCur = posCur/2;
    }

    updateMapping(posCur, temp);
    
}

void heap::percolateDown(int posCur) {
    assert(posCur > 0);
    assert(posCur <= curr_size);

    heapNode temp = std::move(nodes[posCur]);

    while(2*posCur <= curr_size) {
        int child = 2*posCur;
        //start by assuming left child

        if(child+1 <= curr_size && nodes[child].key > nodes[child+1].key) {
            child += 1;
        }

        if(nodes[child].key < temp.key) {
            updateMapping(posCur, nodes[child]);
            posCur = child;
        }

        else {
            break;
        }
    }

    updateMapping(posCur, temp);

}

int heap::getIndex(const std::string& id) {
    bool found = false;
    void* ptr = hashtable.getPointer(id, &found);

    if(found == false) {
        //in this context 0 is impossible, signifies the id could not be found within hashtable
        return 0;
    }

    assert(ptr != nullptr);
    //this assertion fails if there was a hashtable entry found but the pv of said entry is nullptr

    return static_cast<int>(static_cast<heapNode*>(ptr) - nodes.data());
}

void heap::updateMapping(int index, heapNode& node) {

    nodes[index] = std::move(node);

    int result = hashtable.setPointer(nodes[index].id, &nodes[index]);
    //if -DNDEBUG is defined, assert would not run if hashtable.setPointer(etc) was argument, separated into two steps

    //by the time a node is relocated, it must be there
    assert(result == 0);
    (void)result; //due to -Wunused-variable, this will silence the warning, counts as a use
}
