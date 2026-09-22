#include "hash.h"

#include <cstdint>
#include <cassert>
#include <stdexcept>
#include <iostream>


const double REHASH_RATIO = 0.5;


static const std::vector<int> primeCapacities = {
    2053,
    4099,
    8209,
    16411,
    32771,
    65537,
    131101,
    262147,
    524309,
    1048583,
    2097169,
    4194319,
    8388617,
    16777259,
    33554467,
    67108879
};


unsigned int hashTable::getPrime(int size) {
    for(int i = 0; i < (int)primeCapacities.size(); i++) {
        if(primeCapacities[i] >= size) {
            return primeCapacities[i];
        } 
    }

    return 67108879;
}

hashTable::hashTable(int size) {

    int prime = hashTable::getPrime(size);

    data.resize(prime);

    capacity = prime;
    filled = 0;
}

int hashTable::hash(const std::string &key) {
    //using FNV-1a hash function
    const uint32_t FNV_PRIME = 0x01000193;
    const uint32_t FNV_OFFSET_BASIS = 0x811c9dc5;

    uint32_t hash = FNV_OFFSET_BASIS;

    for(char c : key) {
        hash ^= static_cast<uint8_t>(c);
        hash *= FNV_PRIME;
    }

    return hash%capacity;

}

hashTable::findPosResult hashTable::findPos(const std::string &key) {
    int index = hash(key);
    int first_tombstone_index = -1;
    
    assert(filled < capacity);

    while(data[index].isOccupied) {
        //if it reaches an empty slot that never held anything
        //then exit
        
        if(data[index].isDeleted) {
            if(first_tombstone_index == -1) {
                first_tombstone_index = index;
                //record only first appearing tombstone
            }
        } else {
            if(data[index].key == key) {
                return findPosResult(index, first_tombstone_index, true);
            }
        }

        index = (index+1)%capacity;
        //wraps around, exit is guaranteed since capacity > filled and rehash ratio is at 0.5
    }

    return findPosResult(index, first_tombstone_index, false);
    //if called by insert, the pair returned will be the index of the first empty slot and the 
    //first tombstone seen after initial index from hash()
}

/*
note:
isOccupied == false: empty slot from creation
isOccupied == true && isDeleted == false: holds a key
isOccupied == true && isDeleted == true: tombstone from deletion

*/

bool hashTable::contains(const std::string &key) {
    return findPos(key).found;
}

int hashTable::insert(const std::string &key, void *pv) {

    if(REHASH_RATIO <= (double)(filled+1)/capacity) {
        if(!rehash()) {
            return 2;
        }
    }

    findPosResult result = findPos(key);
    
    if(result.found) {
        return 1;
        //duplicate was found
    }

    //empty tombstone found
    if(result.tombstone != -1) {
        data[result.tombstone].isOccupied = true;
        data[result.tombstone].isDeleted = false;
        data[result.tombstone].key = key;
        data[result.tombstone].pv = pv;
    }

    else {
        data[result.index].isOccupied = true;
        data[result.index].key = key;
        data[result.index].pv = pv;
        filled += 1;
    }

    return 0;
}

bool hashTable::rehash() {

    std::vector<hashItem> tmp;
    
    int new_capacity = getPrime(capacity+1);
    
    if(new_capacity == capacity) {
        //max capacity reached already, cannot grow any larger
        return false;
    }

    try {
        tmp.resize(new_capacity);
    }

    catch (const std::bad_alloc& e){
        std::cerr << "memory allocation for rehashing failed" << e.what() << std::endl;
        return false;
    }

    catch (const std::length_error& e) {
        std::cerr << "length error with vector resizing" << e.what() << std::endl;
        return false;
    } 

    std::swap(tmp, data);

    capacity = new_capacity;
    filled = 0;

    for(auto& item : tmp) {
        if(!item.isOccupied || item.isDeleted) {
            //skip the tombstones and empty slots, this will not include them in rehashed vector
            //a sort of garbage collection per rehash
            continue;
        }

        int index = hash(item.key);

        while(data[index].isOccupied) {
            index = (index+1)%capacity;
        }

        data[index] = std::move(item);
        filled++;
    }

    
    //at the end of scope, tmp should destruct along with the old data as well
    return true;
    
  
}

void* hashTable::getPointer(const std::string &key, bool *b) {
    findPosResult result = findPos(key);

    if(result.found) {
        if(b != nullptr) {
            *b = true;
        }

        return data[result.index].pv;
    }
    
    if(b != nullptr) {
        *b = false;
    }

    return nullptr;
}

int hashTable::setPointer(const std::string &key, void *pv) {
    findPosResult result = findPos(key);

    if(result.found) {
        data[result.index].pv = pv;
        return 0;
    }

    return 1;
}

bool hashTable::remove(const std::string &key) {
    findPosResult result = findPos(key);

    if(result.found) {
        data[result.index].isDeleted = true;
        return true;
    }

    return false;

    //note: remove leaves pv within the tombstone
}



/*
some decisions

- linear probing
- what hashing algorithm to use? fnv-1a
- what load factor? 0.5 
- check before insertion

- filled keeps track of both tombstones and live slots
maybe implement garbage collection rehash?

- findPos used for contains and inserts

for inserts if the index is empty place it there?
also let insertion reuse a tombstone and do not increment filled

- probe and place for rehashing while skipping tombstones
move the keys out and dont copy

- geometric growth of map size every rehash
choose primes greater than curr_size * 2^rehash_count?

-default size?

- tokenize outside of insert/contains


*/