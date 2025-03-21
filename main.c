#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define DEFAULT_CAPACITY 16
#define LOAD_MAX 0.75

// Declartion of structs
struct hashmap;
struct pair;

// Declartion of functions
uint32_t hash(char *key, uint32_t seed);
uint32_t get_index(char *key, uint32_t size);
struct pair *find_pair(struct pair *first, char *key);
struct pair *create_pair(char *key, uint32_t value, struct pair *next);
void insert(struct hashmap *map, char *key, uint32_t value);
bool check_load(struct hashmap *map);
void resize(struct hashmap *map);
uint32_t get(struct hashmap *map, char *key);
struct hashmap *init_map();
uint32_t scramble(uint32_t hash, uint32_t k);
uint32_t rotate(uint32_t value, uint32_t rotations);
uint32_t get_chunk(char* start, int bytes);


// Map of strings to int
struct hashmap {
    // Array of pointers to the first element of the linked list of key-value
    // pairs
    struct pair **pairs;
    uint32_t size;
    uint32_t entries;
};

// Key-value pair linked list
struct pair {
    char *key;
    uint32_t value;
    struct pair *next;
};

// Rotation function, will rotate the bits by the amount
// Returns rotated number
uint32_t rotate(uint32_t value, uint32_t rotations)
{
    // Making sure the rotations don't overflow the max amount
    return (value << rotations) | (value >> (32 - rotations));
}

// Scramble functions, will perform the operations for Murmur hashing with c1,c2 and rotations
// Will return hash of the block
uint32_t scramble(uint32_t hash, uint32_t k)
{
    // Constants
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    k *= c1; 
    k = rotate(k, 15);
    k *= c2;
    hash ^= k;
    return hash;
}

uint32_t get_chunk(char* start, int bytes)
{
    uint32_t chunk = 0;
    for (int i = 0; i < bytes; i++)
    {
        chunk |= start[i] << (i * 8);
    }
    return chunk;
}

// Hash function
// Returns the hash of the key
// Uses murmur3 hash function
uint32_t hash(char *key, uint32_t seed) 
{
    // Constants
    const uint32_t m = 0x5;
    const uint32_t n = 0xe6546b64;
    uint32_t hash = seed;
    uint32_t len = strlen(key);
    // Seperate the keys into 4 bytes chunks
    // Multiply and rotate the each chunk
    for (uint32_t i = 0; i < len - len % 4; i+= 4)  
    {
        uint32_t k = get_chunk(key + i, 4);
        hash = scramble(hash, k);
        hash = rotate(hash, 13);
        hash = hash * m + n;
    }

    // Check if there are any remaining bytes, if no, continue, if yes, follow the next step
    // For the remaining bytes, shift the the required amount and then scramble
    uint32_t k = 0; 
    k = get_chunk(key + len - len % 4, len % 4);
    hash = scramble(hash, k); 


    // Final Avalanche
    hash ^= len;
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= (hash >> 13);
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);

    return hash;
}

// Index function
// Returns the index of the key based on the hash
uint32_t get_index(char *key, uint32_t size) 
{
    return hash(key, 123) % size; 
}

// Go over the linked list and return the pair struct
// Returns the value of the key
struct pair *find_pair(struct pair *first, char *key) {
    // Go over the linked list seeing if the key matches
    struct pair *curr = first;
    while (NULL != curr && strcmp(curr->key, key) != 0) {
        curr = curr->next;
    }
    return curr;
}

struct pair *create_pair(char *key, uint32_t value, struct pair *next) {
    // Allocate memory to a pair and init it
    struct pair *p = (struct pair *)malloc(sizeof(struct pair ));
    p->value = value;
    // Copy the key to the pair
    // Allocate memory for the key
    p->key = (char *)malloc(strlen(key) + 1);
    strcpy(p->key, key);
    p->next = next;
    return p;
}

void insert(struct hashmap *map, char *key, uint32_t value) {
    // Update the value of pair, insert at the start if gotten NULL
    uint32_t index = get_index(key, map->size);
    struct pair *p = find_pair(map->pairs[index], key);
    if (NULL == p) {
        struct pair *new_pair = create_pair(key, value, map->pairs[index]);
        map->entries++;
        map->pairs[index] = new_pair;
        if (check_load(map)) {
            resize(map);
        }
        return;
    }
    p->value = value;
}

bool check_load(struct hashmap *map) {
    return (float)map->entries / map->size > LOAD_MAX;
}

void resize(struct hashmap *map) {
    // Create a new hashmap with double the size
    map->size *= 2;
    map->entries = 0;
    struct pair **new = (struct pair **)malloc(map->size * sizeof(struct pair *));
    // Initialize all pairs to NULL
    for (int i = 0; i < map->size; i++) {
        new[i] = NULL;
    }
    struct pair **old = map->pairs;
    map->pairs = new;
    // Go over the old map and rehash all the entries
    for (uint32_t i = 0; i < map->size / 2; i++) {
        struct pair *curr = old[i];
        while (NULL != curr) {
            // Insert into the new map
            insert(map, curr->key, curr->value);
            free(curr->key);
            curr = curr->next;
        }
    }
    free(old);
}

uint32_t get(struct hashmap *map, char *key) {
    // Get pair of the key, and return the value
    uint32_t index = get_index(key, map->size);
    struct pair *p = find_pair(map->pairs[index], key);
    // Return NULL if not found
    if (NULL == p) {
        return -999999999;
    }
    return p->value;
}

// Init a new map
// Returns a pointer to the map
struct hashmap *init_map() {
    struct hashmap *map = (struct hashmap *)malloc(sizeof(struct hashmap));  // Allocate entire struct
    if (!map) {
        return NULL;  // Handle memory allocation failure
    }

    map->size = DEFAULT_CAPACITY;
    map->entries = 0;

    // Allocate an array of pointers to `struct pair`
    map->pairs = (struct pair **)malloc(DEFAULT_CAPACITY * sizeof(struct pair *));
    if (!map->pairs) {
        free(map);  // Avoid memory leak
        return NULL;
    }

    // Initialize all pairs to NULL
    for (int i = 0; i < DEFAULT_CAPACITY; i++) {
        map->pairs[i] = NULL;
    }

    return map;
}



int main() {
    struct hashmap *map = init_map();
    if (!map) {
        printf("Failed to initialize hashmap.\n");
        return 1;
    }

    printf("### Test 1: Insert and Retrieve ###\n");
    insert(map, "apple", 10);
    insert(map, "banana", 20);
    insert(map, "cherry", 30);

    printf("apple -> %u\n", get(map, "apple"));   // Expected: 10
    printf("banana -> %u\n", get(map, "banana")); // Expected: 20
    printf("cherry -> %u\n", get(map, "cherry")); // Expected: 30

    printf("\n### Test 2: Handling Missing Keys ###\n");
    printf("grape -> %d (should be 0 or error)\n", get(map, "grape")); // Expected: Not found

    printf("\n### Test 3: Duplicate Key Overwrite ###\n");
    insert(map, "apple", 42);  // Overwrite previous value
    printf("apple -> %u (should be 42)\n", get(map, "apple"));  // Expected: 42

    printf("\n### Test 4: Edge Cases ###\n");
    insert(map, "", 99);  // Empty string as key
    printf("\"\" -> %u (should be 99)\n", get(map, ""));  // Expected: 99

    char long_key[256];
    memset(long_key, 'a', 255);
    long_key[255] = '\0';
    insert(map, long_key, 77);
    printf("long_key[255] -> %u (should be 77)\n", get(map, long_key)); // Expected: 77

    printf("\n### Test 5: Insert More Keys to Trigger Resize ###\n");
    for (int i = 0; i < 100; i++) {
        char key[20];
        snprintf(key, sizeof(key), "key%d", i);
        insert(map, key, i);
    }
    printf("key50 -> %u (should be 50)\n", get(map, "key50"));  // Expected: 50
    printf("key99 -> %u (should be 99)\n", get(map, "key99"));  // Expected: 99

    printf("\n### All tests completed successfully! ###\n");

    return 0;
}
