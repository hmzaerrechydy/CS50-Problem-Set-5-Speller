// Implements a dictionary's functionality

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "dictionary.h"

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// TODO: Choose number of buckets in hash table
const unsigned int N = 26;

// Hash table
node *table[N];

// Returns true if word is in dictionary, else false
bool check(const char *word)
{
    // TODO
    int h = hash(word);
    node *cursor = table[h];

    while(cursor != NULL){
        if(strcasecmp(word, cursor->word) == 0){
            return true;
        }
        cursor = cursor->next;
    }
    return false;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    // TODO: Improve this hash function
    return toupper(word[0]) - 'A';
}

int word_count = 0;
// Loads dictionary into memory, returning true if successful, else false
bool load(const char *dictionary)
{
    // TODO
    FILE *dict = fopen(dictionary, "r");

    if(dict == NULL){
        return false;
    }

    // to store the current word
    char word[LENGTH + 1];

    // keep reading strings from dict until fscanf returns EOF which is the end of the file
    while(fscanf(dict, "%s", word) != EOF){
        node *n = malloc(sizeof(node)); // create a node and allocate memory to it
        if(n == NULL){
            return false;
        }

        strcpy(n->word, word);

        int hash_index = hash(word);

        node *head = table[hash_index];

        if(head == NULL){
            table[hash_index] = n;
        }else{
            n->next = table[hash_index];
            table[hash_index] = n;
        }
        word_count++;
    }



    fclose(dict);
    return true;
}

// Returns number of words in dictionary if loaded, else 0 if not yet loaded
unsigned int size(void)
{
    // TODO
    if(word_count > 0){
        return word_count;
    }
    return 0;
}

// Unloads dictionary from memory, returning true if successful, else false
bool unload(void)
{
    // TODO
    int count = 0;
    for(int i = 0; i < N; i++){
        node *cursor = table[i];

        while(cursor != NULL){
            node *tmp = cursor;
            cursor = cursor->next;
            free(tmp);
        }

        if (cursor == NULL && i == N - 1)
        {
            return true;
        }

    }

    return false;
}
