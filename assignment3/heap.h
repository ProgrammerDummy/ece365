
// heap.h
//
// Matthew Jeong
// ECE365 assignment #2
//
// Declaration of the heap class, a priority queue implemented with a binary
// heap. Each item has a unique string id, an integer key, and an optional
// void pointer to arbitrary associated data. The heap is paired with a hash
// table (from assignment #1) that maps each id to the address of its node.


#ifndef HEAP_H
#define HEAP_H

#include "hash.h"

#include <vector>
#include <string>

class heap {
    public:
        //
        // heap - The constructor allocates space for the nodes of the heap
        // and the mapping (hash table) based on the specified capacity
        //
        heap(int cap);

        //
        // insert - Inserts a new node into the binary heap
        //
        // Inserts a node with the specified id string, key,
        // and optionally a pointer.  They key is used to
        // determine the final position of the new node.
        //
        // Returns:
        //   0 on success
        //   1 if the heap is already filled to capacity
        //   2 if a node with the given id already exists (but the heap
        //     is not filled to capacity)
        //
        int insert(const std::string &id, int key, void *pv = nullptr);

        //
        // setKey - set the key of the specified node to the specified value
        //
        // I have decided that the class should provide this member function
        // instead of two separate increaseKey and decreaseKey functions.
        //
        // Returns:
        //   0 on success
        //   1 if a node with the given id does not exist
        //
        int setKey(const std::string &id, int key);

        //
        // deleteMin - return the data associated with the smallest key
        //             and delete that node from the binary heap
        //
        // If pId is supplied (i.e., it is not nullptr), write to that address
        // the id of the node being deleted. If pKey is supplied, write to
        // that address the key of the node being deleted. If ppData is
        // supplied, write to that address the associated void pointer.
        //
        // Returns:
        //   0 on success
        //   1 if the heap is empty
        //
        int deleteMin(std::string *pId = nullptr, int *pKey = nullptr, void *ppData = nullptr);

        //
        // remove - delete the node with the specified id from the binary heap
        //
        // If pKey is supplied, write to that address the key of the node
        // being deleted. If ppData is supplied, write to that address the
        // associated void pointer.
        //
        // Returns:
        //   0 on success
        //   1 if a node with the given id does not exist
        //
        int remove(const std::string &id, int *pKey = nullptr, void *ppData = nullptr);


    private:
        int capacity;      // Maximum number of items the heap may hold.
        int curr_size = 0; // Number of items currently in the heap.

        //
        // Each node in the heap contains:
        // key - the integer used to order the heap; may be negative.
        // id - the unique string naming this item.
        // data_ptr - a pointer to arbitrary data supplied by the caller;
        //            nullptr if no pointer was provided to insert.
        //
        class heapNode {
            public:
                int key {0};
                std::string id {""};
                void *data_ptr {nullptr};
        };

        // The heap itself. Sized capacity + 1 because index 0 is unused.
        // Valid node positions are 1 through curr_size.
        std::vector<heapNode> nodes;

        // Maps each id to the address of its node in the vector above.
        // Sized generously so that a rehash is unlikely; a rehash is still
        // possible because removals are only lazily deleted from the table.
        hashTable hashtable;

        //
        // percolateUp - move the node at posCur toward the root until heap
        //               order holds
        //
        // Used when a node may be too small for its position: by insert, by
        // setKey when the key decreased, and by remove when the replacement
        // node is smaller than the one it replaced.
        //
        void percolateUp(int posCur);

        //
        // percolateDown - move the node at posCur toward the leaves until
        //                 heap order holds
        //
        // Used when a node may be too large for its position: by deleteMin,
        // by setKey when the key increased, and by remove when the
        // replacement node is larger than the one it replaced.
        //
        void percolateDown(int posCur);

        //
        // getIndex - find the index of the node with the specified id
        //
        // Returns the node's position in the nodes vector, or 0 if the id is
        // not in the heap. Index 0 is never a valid node position, so it can
        // safely be used to mean "not found".
        //
        int getIndex(const std::string &id);

        //
        // updateMapping - move a node into the slot at index and update that
        //                 node's entry in the hash table
        //
        // Every relocation of a node goes through this function, so the hash
        // table can never be left pointing at a node's old position. Note
        // that it is only for relocations: setKey changes a key in place
        // without moving the node, so it must not call this (doing so would
        // be a self-move).
        //
        void updateMapping(int index, heapNode& node);
};

#endif