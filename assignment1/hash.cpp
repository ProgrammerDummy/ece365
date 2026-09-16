#include "hash.h"
#include <cstdint>
#include <cassert>

const float REHASH_RATIO = 0.5;


static const std::vector<int> primeCapacities = {
    2,
    3,
    5,
    11,
    17,
    37,
    67,
    131,
    257,
    521,
    1031,
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
    67108879,
};


unsigned int hashTable::getPrime(int size) {
    for(int i = 0; i < primeCapacities.size(); i++) {
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

std::pair<int, int> hashTable::findPos(const std::string &key) {
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
                return {index, first_tombstone_index};
            }
        }

        index = (index+1)%capacity;
        //wraps around, exit is guaranteed since capacity > filled and rehash ratio is at 0.5
    }

    return {index, first_tombstone_index};
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

}

int hashTable::insert(const std::string &key, void *pv) {

}

bool hashTable::rehash() {
    
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