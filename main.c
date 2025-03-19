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
    rotations = rotations % sizeof(int);

    uint32_t rotated_num = 0;
    rotated_num = value >> (sizeof(int) - rotations);
    rotated_num |= value << rotations;

    return rotated_num;
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
    return hash ^ k;
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
    for (uint32_t i = 0; i < len; i+= 4)  
    {
        hash = scramble(hash, key[i]);
        hash = rotate(hash, 13);
        hash = hash * m + n;
    }
    
    // Check if there are any remaining bytes, if no, continue, if yes, follow the next step
    // For the remaining bytes, shift the the required amount and then scramble
    if (len % 4)
    {
        uint32_t k = key[len - (len % 4)] >> 1;
        switch (len % 4)
        {
            case 1: k = k << 16; break;
            case 2: k = k << 8; break;
            case 3: break;
        }
        hash = scramble(hash, k);
    }
    
    // Final Avalanche
    hash ^= (hash >> 16);
    hash *= 0x85ebca6b;
    hash ^= hash >> 13;
    hash *= 0xc2b2ae35;
    hash ^= (hash >> 16);
    
    // Final step
    hash ^= len;
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
  while (NULL != curr && !strcmp(curr->key, key)) {
    curr = curr->next;
  }
  return curr;
}

struct pair *create_pair(char *key, uint32_t value, struct pair *next) {
  // Allocate memory to a pair and init it
  struct pair *p = (struct pair *)malloc(sizeof(struct pair *));
  p->value = value;
  p->key = key;
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
  struct pair **new =
      (struct pair **)malloc(map->size * sizeof(struct pair **));
  struct pair **old = map->pairs;
  // Go over the old map and rehash all the entries
  for (uint32_t i = 0; i < map->size / 2; i++) {
    struct pair *curr = old[i];
    while (NULL != curr) {
      struct pair *next = curr->next;
      // Get the new index and insert into the new map
      uint32_t index = get_index(curr->key, map->size);
      curr->next = new[index];
      new[index] = curr;
      curr = next;
    }
  }
  free(old);
  map->pairs = new;
}

uint32_t get(struct hashmap *map, char *key) {
  // Get pair of the key, and return the value
  uint32_t index = get_index(key, map->size);
  struct pair *p = find_pair(map->pairs[index], key);
  // Return NULL if not found
  if (NULL == p) {
    return NULL;
  }
  return p->value;
}

// Init a new map
// Returns a pointer to the map
struct hashmap *init_map() {

  struct hashmap *map = (struct hashmap *)malloc(sizeof(struct hashmap *));
  map->size = DEFAULT_CAPACITY;
  map->pairs = (struct pair **)malloc(sizeof(struct pair **));
  map->entries = 0;
  return map;
}

int main() {
  printf("Hello, World!\n");
  printf("%" PRIu32, hash("abcde", 123));
  return 0;
}
