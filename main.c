#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <stdbool.h>

#define DEFAULT_CAPACITY 16
#define LOAD_MAX 0.75

// Declartion of structs
struct hashmap;
struct pair;


// Declartion of functions
int hash(char* key);
int get_index(char* key, int size);
struct pair* find_pair(struct pair* first, char* key);
struct pair* create_pair(char* key, int value, struct pair* next);
void insert(struct hashmap* map, char* key, int value);
bool check_load(struct hashmap* map);
void resize(struct hashmap* map);
int get(struct hashmap* map, char* key);
struct hashmap* init_map();


// Map of strings to int
struct hashmap {
    // Array of pointers to the first element of the linked list of key-value pairs 
    struct pair** pairs;
    int size;
    int entries;
};


// Key-value pair linked list
struct pair {
    char* key;
    int value;
    struct pair* next;
};

// Hash function
// Returns the hash of the key
// Uses murmur3 hash function
int hash(char* key)
{
    // TODO
    return 0;
}

// Index function
// Returns the index of the key based on the hash
int get_index(char* key, int size)
{
    return hash(key) % size;
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
        struct pair* new_pair = create_pair(key, value, map->pairs[index]);
        map->entries++;
        map->pairs[index] = new_pair;
        if (check_load(map))
        {
            resize(map);
        }
        return;
    }
    p->value = value;
}

bool check_load(struct hashmap* map)
{ 
    return (float) map->entries / map->size > LOAD_MAX;
}

void resize(struct hashmap* map)
{
    // Create a new hashmap with double the size
    map->size *= 2;
    struct pair** new = (struct pair**)malloc(map->size * sizeof(struct pair**));
    struct pair** old = map->pairs; 
    // Go over the old map and rehash all the entries
    for (int i = 0; i < map->size / 2; i++)
    {
        struct pair* curr = old[i];
        while (NULL != curr)
        {
            struct pair* next = curr->next;
            // Get the new index and insert into the new map
            int index = get_index(curr->key, map->size);
            curr->next = new[index];
            new[index] = curr;
            curr = next;
        }
    }
    free(old);
    map->pairs = new;
}

int get(struct hashmap* map, char* key)
{
    // Get pair of the key, and return the value
    int index = get_index(key, map->size);
    struct pair* p = find_pair(map->pairs[index], key);
    // Return NULL if not found
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

    struct hashmap* map = (struct hashmap*)malloc(sizeof(struct hashmap*));
    map->size = DEFAULT_CAPACITY;
    map->pairs = (struct pair**)malloc(sizeof(struct pair**));
    map->entries = 0;
    return map;
}



int main() {
    printf("Hello, World!\n");
    return 0;
}
