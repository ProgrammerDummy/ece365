//
// heap.cpp
//
// Matthew Jeong
// ECE 365 - Data Structures and Algorithms II
// Programming Assignment #2
//
// Implementation of the heap class declared in heap.h: a priority queue built
// on a 1-indexed binary heap, paired with a hash table that maps each item's
// string id to the address of its node so that setKey and remove can find an
// item without scanning the heap.
//
// Both percolate functions use the "hole" technique rather than swapping: the
// node being repositioned is moved out into a temporary, other nodes are moved
// into the hole one at a time as the hole travels, and the temporary is placed
// at the end. That is one move per level instead of the three assignments a
// swap would cost.
//

#include "heap.h"

#include <cassert>
#include <algorithm>


//
// Constructor. Clamps a negative capacity to 0 so the rest of the class can
// assume capacity >= 0. The node vector gets capacity + 1 entries because
// index 0 is unused. The hash table is sized at 3 * capacity: its load factor
// limit is 0.5, so this leaves room for every item plus some lazily deleted
// entries before a rehash becomes necessary.
//
// Note that capacity is declared before nodes and hashtable in heap.h, and
// members are initialized in declaration order, so the clamped member value is
// already set when the other two are initialized from it.
//
heap::heap(int cap) : capacity(std::max(0, cap)), curr_size(0),
                      nodes(capacity + 1), hashtable(3 * capacity) {

}


//
// insert - add a new item to the heap.
//
// The id is put into the hash table before percolateUp runs, because every
// relocation updates the mapping and the entry has to exist by then.
//
int heap::insert(const std::string &id, int key, void *pv) {
    if(curr_size == capacity) {
        return 1;
    }

    if(getIndex(id)) {
        return 2;
    }

    //place the new node in the only position that keeps the tree complete
    nodes[curr_size+1].id = id;
    nodes[curr_size+1].key = key;
    nodes[curr_size+1].data_ptr = pv;

    int result = hashtable.insert(id, &nodes[curr_size+1]);

    //the id cannot already be in the table (checked above), so the only other
    //outcome would be a failed rehash, which this class cannot recover from
    assert(result == 0);
    (void)result; //silences -Wunused-variable when NDEBUG removes the assert

    curr_size++;

    percolateUp(curr_size);

    return 0;
}


//
// setKey - change the key of an existing item.
//
// Which direction to repair is implied by the comparison: a smaller key may
// now be below its parent, a larger key may now be above a child. The key is
// written directly instead of through updateMapping because the node does not
// move, so its hash table entry is still correct.
//
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


//
// deleteMin - remove and return the item with the smallest key, which is
//             always at index 1.
//
// The caller's values are read out first, since moving the last node into the
// root overwrites them. The id is also removed from the hash table here;
// relocating nodes never retires an id on its own.
//
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

    //if the root is the only node there is nothing to move into it; this also
    //keeps the line below from moving nodes[1] onto itself
    if(curr_size == 1) {
        curr_size--;
        return 0;
    }

    //the last node is the only one that can be removed without leaving a gap,
    //so it takes the root's place and sinks to where it belongs
    updateMapping(1, nodes[curr_size]);

    curr_size--;

    percolateDown(1);

    return 0;
}


//
// remove - delete the item with the specified id.
//
// Same idea as deleteMin, except the hole can be anywhere, so the replacement
// may need to travel either up or down. Comparing the replacement's key with
// the removed node's key says which: the removed node satisfied heap order in
// both directions, so a smaller replacement can only be wrong with respect to
// its parent, and a larger one can only be wrong with respect to its children.
//
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

    //if the node being removed is already the last one there is nothing to
    //move; this also keeps the line below from moving a node onto itself
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


//
// percolateUp - posCur is the index of a node that may be smaller than its
//               parent. The node travels toward the root until its parent is
//               no larger than it, or until it reaches index 1.
//
void heap::percolateUp(int posCur) {
    assert(posCur > 0);
    assert(posCur <= curr_size);

    heapNode temp = std::move(nodes[posCur]);

    //the hole is now at nodes[posCur]

    //the test on posCur must come first: at the root, posCur/2 is 0, and
    //nodes[0] is the unused slot, whose key would be meaningless to compare
    while(posCur > 1 && temp.key < nodes[posCur/2].key) {
        updateMapping(posCur, nodes[posCur/2]);
        posCur = posCur/2;
    }

    updateMapping(posCur, temp);
}


//
// percolateDown - posCur is the index of a node that may be larger than one of
//                 its children. The node travels toward the leaves, always
//                 trading places with the smaller child, until both children
//                 are no smaller than it or it reaches a leaf.
//
void heap::percolateDown(int posCur) {
    assert(posCur > 0);
    assert(posCur <= curr_size);

    heapNode temp = std::move(nodes[posCur]);

    //loop while the hole has at least one child
    while(2*posCur <= curr_size) {
        int child = 2*posCur;
        //start by assuming left child

        //the existence test must come first: a node on the bottom level may
        //have a left child and no right child, and reading past curr_size
        //would compare against an unused slot
        if(child+1 <= curr_size && nodes[child].key > nodes[child+1].key) {
            child += 1;
        }

        if(nodes[child].key < temp.key) {
            updateMapping(posCur, nodes[child]);
            posCur = child;
        }

        else {
            //temp belongs at the hole, so stop here
            break;
        }
    }

    updateMapping(posCur, temp);
}


//
// getIndex - look the id up in the hash table and convert the stored node
//            address into an index.
//
// The subtraction is valid because the node vector is allocated once in the
// constructor and never resized, so every stored pointer refers to an element
// of that one array.
//
int heap::getIndex(const std::string& id) {
    bool found = false;
    void* ptr = hashtable.getPointer(id, &found);

    if(found == false) {
        //index 0 is never a valid node position, so it can mean "not found"
        return 0;
    }

    //every id in the table was stored with the address of its node, so a
    //null pointer here would mean the mapping had been corrupted
    assert(ptr != nullptr);

    return static_cast<int>(static_cast<heapNode*>(ptr) - nodes.data());
}


//
// updateMapping - move a node into nodes[index] and point that node's hash
//                 table entry at its new location.
//
// Every relocation in this class goes through this one function, so there is
// no path that can move a node and forget to update the mapping.
//
void heap::updateMapping(int index, heapNode& node) {
    nodes[index] = std::move(node);

    //the call is kept out of the assert: assert expands to nothing when NDEBUG
    //is defined, which would remove the setPointer call along with the check
    int result = hashtable.setPointer(nodes[index].id, &nodes[index]);

    //a node is only relocated while it is in the heap, so its id must be in
    //the table
    assert(result == 0);
    (void)result; //silences -Wunused-variable when NDEBUG removes the assert
}