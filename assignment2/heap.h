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
        int capacity;
        int curr_size = 0;

        class heapNode {
            public:
                int key {0}; 
                std::string id {""};
                void *data_ptr {nullptr};
        };

        std::vector<heapNode> nodes;

        hashTable hashtable;

        void percolateUp(int posCur); //used during insertion
        
        void percolateDown(int posCur); //used during deleteMin
        //also used by setKey and remove

        // getIndex - retrieves the index of the node within nodes 
        int getIndex(const std::string &id);

        // updateMapping - a helper function for writing to a node at index and updating pointer of corresponding
        //                 hashtable entry
        void updateMapping(int index, heapNode& node);


};

#endif


/*

very important to do:
- create a private helper function that writes the heap slot and updates the mapping of the 
slot's entry (in the hashtable) together. 

note:
- to get the node's index, subtract element 0 of the nodes vector from its memory address
to get it

- setKey can use either percolateUp or percolateDown or nothing, depending on the key size

- remove does percolateDown or percolateUp

--> all percolates will be hole based to minimize memory operations!
*/