#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#define DEFAULT_CAPACITY 16

// Map of strings to int
struct hashmap {
    // Array of pointers to the first element of the linked list of key-value pairs 
    struct pair** pairs;
    int size;
};

// Key-value pair linked list
struct pair {
    char* key;
    int value;
    struct pair* next;
};

// Hash function
// Returns the hash of the key
int hash(char* key)
{
    // TODO
    return 0;
}

// Index function
// Returns the index of the key based on the hash
int get_index(char* key, int size)
{
    // TODO
    return 0;
}

// Go over the linked list and return the pair struct
// Returns the value of the key
struct pair* find_pair(struct pair* first, char* key)
{
    // Go over the linked list seeing if the key matches
    struct pair* curr = first;
    while (NULL != curr && !strcmp(curr->key, key))
    {
        curr = curr->next;
    }
    return curr;
}


struct pair* create_pair(char* key, int value, struct pair* next)
{
    // Allocate memory to a pair and init it
    struct pair* p = (struct pair*)malloc(sizeof(struct pair*));
    p->value = value;
    p->key = key;
    p->next = next;
    return p;
}

void insert(struct hashmap* map, char* key, int value)
{
    // Update the value of pair, insert at the start if gotten NULL 
    int index = get_index(key, map->size);
    struct pair* p = find_pair(map->pairs[index], key);
    if (NULL == p)
    {
        struct pair* new_pair = create_pair(key, value, p);
        return;
    }
    p->value = value;
}

int get(struct hashmap* map, char* key)
{
    // TODO
    // Get pair of the key, and return the value
    int index = get_index(key, map->size);
    struct pair* p = find_pair(map->pairs[index], key);
    if (NULL == p)
    {
        return NULL;
    }
    return p->value;
}


// Init a new map
// Returns a pointer to the map
struct hashmap* init_map() 
{
    // TODO
    struct hashmap* map = (struct hashmap*)malloc(sizeof(struct hashmap*));
    map->size = DEFAULT_CAPACITY;
    map->pairs = NULL;
    return map;
}



int main() {
    printf("Hello, World!\n");
    return 0;
}
