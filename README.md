Link: https://cs50.harvard.edu/x/2023/psets/5/speller/


### dictionary.h


dictionary.h is a header file that we can include in a file to access code in another file. 

Open up dictionary.h, and you’ll see some new syntax, including a few lines that mention DICTIONARY_H. No need to worry about those, but, if curious, those lines just ensure that, even though dictionary.c and speller.c (which you’ll see in a moment) #include this file, clang will only compile it once.

stdbool.h is the file in which bool (data type boolean: true or false) is defined.

Notice our use of #define, a “preprocessor directive” that defines a “constant” called LENGTH that has a value of 45. It’s a constant in the sense that you can’t (accidentally) change it in your own code. In fact, clang will replace any mentions of LENGTH in your own code with, literally, 45. In other words, it’s not a variable, just a find-and-replace trick.

Below are prototypes of five functions that are written in dictionary.c: 

```
bool check(const char *word);
unsigned int hash(const char *word);
bool load(const char *dictionary);
unsigned int size(void);
bool unload(void);
```

const says that those strings, when passed in as arguments, must remain constant; you won’t be able to change them, accidentally or otherwise!


texts/


Includes texts that we can use to test our code. 


Makefile


And, lastly, recall that make automates compilation of your code so that you don’t have to execute clang manually along with a whole bunch of switches. However, as your programs grow in size, make won’t be able to infer from context anymore how to compile your code; you’ll need to start telling make how to compile your program, particularly when they involve multiple source (i.e., .c) files, as in the case of this problem. And so we’ll utilize a Makefile, a configuration file that tells make exactly what to do. Open up Makefile, and you should see four lines:

1. The first line tells make to execute the subsequent lines whenever you yourself execute make speller (or just make).
2. The second line tells make how to compile speller.c into machine code (i.e., speller.o).
3. The third line tells make how to compile dictionary.c into machine code (i.e., dictionary.o).
4. The fourth line tells make to link speller.o and dictionary.o in a file called speller.

The challenge is to implement, in order, load, hash, size, check, and unload as efficiently as possible using a hash table in such a way that TIME IN load, TIME IN check, TIME IN size, and TIME IN unload are all minimized.

Your implementation of check() must be case-insensitive. In other words, if foo is in dictionary, then check should return true given any capitalization thereof; none of foo, foO, fOo, fOO, fOO, Foo, FoO, FOo, and FOO should be considered misspelled.

Capitalization aside, your implementation of check should only return true for words actually in dictionary. Beware hard-coding common words (e.g., the), lest we pass your implementation a dictionary without those same words. Moreover, the only possessives allowed are those actually in dictionary. In other words, even if foo is in dictionary, check should return false given foo's if foo's is not also in dictionary.

You may assume that any dictionary passed to your program will be structured exactly like ours, alphabetically sorted from top to bottom with one word per line, each of which ends with \n. You may also assume that dictionary will contain at least one word, that no word will be longer than LENGTH (a constant defined in dictionary.h) characters, that no word will appear more than once, that each word will contain only lowercase alphabetical characters and possibly apostrophes, and that no word will start with an apostrophe.


Hints 


To compare two strings case-insensitively, you may find strcasecmp (declared in strings.h). 

Be sure to free in unload any memory that you allocated in load. 


speller.c


Within the default dictionary, mind you, are 143,091 words, all of which must be loaded into memory! In fact, take a peek at that file to get a sense of its structure and size. Notice that every word in that file appears in lowercase (even, for simplicity, proper nouns and acronyms). From top to bottom, the file is sorted lexicographically, with only one word per line (each of which ends with \n). No word is longer than 45 characters, and no word appears more than once.

```
// Determine dictionary to use
    char *dictionary = (argc == 3) ? argv[1] : DICTIONARY;
```

If user entered 3 arguments then the string dictionary will be assigned argv[1] else assign the default path in DICTIONARY to the dictionary we will be using.  

```
// Structures for timing data
    struct rusage before, after;
```

We created two variables before and after, and both are a data structure to store the resource usages returned from the getrusage() function. 

```
// Load dictionary
    getrusage(RUSAGE_SELF, &before);
    bool loaded = load(dictionary);
    getrusage(RUSAGE_SELF, &after);
```

We call getrusage() before calling load() and store the data in before, then we call load(), and call getrusage() again and store data in after.  

Helpful article to learn more about getrusage: https://man7.org/linux/man-pages/man2/getrusage.2.html

```
    // Exit if dictionary not loaded
    if (!loaded)
    {
        printf("Could not load %s.\n", dictionary);
        return 1;
    }
```

Print an error text and stop the program if we can't load dictionary. 

We calculate time to load dictionary using the load() function using the function calculate() that takes two arguments before and after and store that value in the time_load variable: 

```
    // Calculate time to load dictionary
    time_load = calculate(&before, &after);
```

In the following block we open the text file provided by the use, if argc is 3 then the file name is at argv[2] else it's at argv[1], we store the name in text, open the file, and if it returns NULL we print an error message, call unload() and stop the program. 

```
    // Try to open text
    char *text = (argc == 3) ? argv[2] : argv[1];
    FILE *file = fopen(text, "r");
    if (file == NULL)
    {
        printf("Could not open %s.\n", text);
        unload();
        return 1;
    }
```

Next we will spell check each word in the text file we opened: 

First we create 3 variables: 
1. index: stores the current index of character in a word. 
2. misspellings: counts the number of misspellings in the text. 
3. words: counts number of words in the text. 

word is a char array or string to store the current word in the text. 

c stores the current character in the text. 

```
    // Prepare to spell-check
    int index = 0, misspellings = 0, words = 0;
    char word[LENGTH + 1];
// Spell-check each word in text
    char c;
```

Now we will read each character in the text using this while loop and copy it to c.

```
while (fread(&c, sizeof(char), 1, file)){...}
```

Inside this loop: 

```
// Allow only alphabetical characters and apostrophes
        if (isalpha(c) || (c == '\'' && index > 0))
        {
            // Append character to word
            word[index] = c;
            index++;

            // Ignore alphabetical strings too long to be words
            if (index > LENGTH)
            {
                // Consume remainder of alphabetical string
                while (fread(&c, sizeof(char), 1, file) && isalpha(c));

                // Prepare for new word
                index = 0;
            }
        }
```

if  c or current character is in the alphabet or an apostrophe then add it to word, and update the index by 1, if the word passes LENGTH then read the rest of it and reset index to 0. 

```
        // Ignore words with numbers (like MS Word can)
        else if (isdigit(c))
        {
            // Consume remainder of alphanumeric string
            while (fread(&c, sizeof(char), 1, file) && isalnum(c));

            // Prepare for new word
            index = 0;
        }
```

else if character is a digit then read the rest of the word and reset index to 0 but we will not pass this word to check() for spell checking because it contains a digit.  

```
// We must have found a whole word
        else if (index > 0)
        {
            // Terminate current word
            word[index] = '\0';

            // Update counter
            words++;

            // Check word's spelling
            getrusage(RUSAGE_SELF, &before);
            bool misspelled = !check(word);
            getrusage(RUSAGE_SELF, &after);

            // Update benchmark
            time_check += calculate(&before, &after);

            // Print word if misspelled
            if (misspelled)
            {
                printf("%s\n", word);
                misspellings++;
            }

            // Prepare for next word
            index = 0;
        }
```

if the character is not an alphabet, apostrophe or a digit and the index is bigger than 0 then we have reached the end of a word, we pass \0 to the current index, add 1 to words counter. Using getrusage() we get resource usage measures before and after calling check(), we store the data in before and after. Afterward we calculate the time check() takes to run and add it to time_check to get the total time the function takes to spell check the whole text. If the word is misspelled we print the word, update misspellings by 1 and finally reset index to 0. 

```
    // Check whether there was an error
    if (ferror(file))
    {
        fclose(file);
        printf("Error reading %s.\n", text);
        unload();
        return 1;
    }
```

ferror() tutorial: https://www.educative.io/answers/what-is-ferror-in-c

If there was an error reading the file then close it, print an error message, unload the dictionary and stop the program. 

We close now the file because we don't need it anymore after spell checking all the words: 

```
    // Close text
    fclose(file);
```

```
    // Determine dictionary's size
    getrusage(RUSAGE_SELF, &before);
    unsigned int n = size();
    getrusage(RUSAGE_SELF, &after);

    // Calculate time to determine dictionary's size
    time_size = calculate(&before, &after);
```

Using size() return number of words in the dictionary and store it in n and use getrusage() before and after. 

unsigned int in c: 
As the name suggests unsigned int in a C programming language is a datatype that represents an integer value without a sign. It can hold zero, and positive integers but it is not allowed to store or hold negative values.
Source: https://www.prepbytes.com/blog/c-programming/unsigned-int-in-c

Calculate time to determine dictionary's size and store it in time_size. 

```
    // Unload dictionary
    getrusage(RUSAGE_SELF, &before);
    bool unloaded = unload();
    getrusage(RUSAGE_SELF, &after);

    // Abort if dictionary not unloaded
    if (!unloaded)
    {
        printf("Could not unload %s.\n", dictionary);
        return 1;
    }

    // Calculate time to unload dictionary
    time_unload = calculate(&before, &after);
```

Call unload() to free the memory load() used and use getrusage() again before and after. 

If dictionary not unloaded stop the program and print an error message. 

In time_unload calculate time to unload dictionary. 

Report all this data that we have gathered at the end of the main function: 

```
    // Report benchmarks
    printf("\nWORDS MISSPELLED:     %d\n", misspellings);
    printf("WORDS IN DICTIONARY:  %d\n", n);
    printf("WORDS IN TEXT:        %d\n", words);
    printf("TIME IN load:         %.2f\n", time_load);
    printf("TIME IN check:        %.2f\n", time_check);
    printf("TIME IN size:         %.2f\n", time_size);
    printf("TIME IN unload:       %.2f\n", time_unload);
    printf("TIME IN TOTAL:        %.2f\n\n",
           time_load + time_check + time_size + time_unload);

    // Success
    return 0;
```

Finally that how the calculate function works to return the number of seconds it takes to run the functions: load, check, size and unload. 

```
// Returns number of seconds between b and a
double calculate(const struct rusage *b, const struct rusage *a)
{
    if (b == NULL || a == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((a->ru_utime.tv_sec * 1000000 + a->ru_utime.tv_usec) -
                  (b->ru_utime.tv_sec * 1000000 + b->ru_utime.tv_usec)) +
                 ((a->ru_stime.tv_sec * 1000000 + a->ru_stime.tv_usec) -
                  (b->ru_stime.tv_sec * 1000000 + b->ru_stime.tv_usec)))
                / 1000000.0);
    }
}
```


dictionary.c


We’ve defined a struct called node that represents a node in a hash table.

```
// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;
```

We’ve declared a global pointer array, table, which will represent the hash table you will use to keep track of words in the dictionary.

```
// Hash table
node *table[N];
```


load()


A hash table (a data structure) is an array of linked lists. 



A hash function pick which linked list to insert a word into. 

The hash function takes a word as input and output a number corresponding to which bucket to store the word in. 



To open the dictionary and return false if the file returns NULL: 

```
    FILE *dict = fopen(dictionary, "r");

    if(dict == NULL){
        return false;
    }
```

Read each word in the dictionary: 

```
    // to store the current word
    char *word[LENGTH];
    // keep reading strings from dict until fscanf returns EOF which is the end of the file
    while(fscanf(dict, "%s", word) != EOF){

    }
```

Create a node for each word, copy word into that node: 

```
node *n = malloc(sizeof(node)); // create a node and allocate memory to it
        if(n == NULL){
            return false;
        }

        strcpy(n->word, word);
```

Now let us hash this word: 

```
int hash_index = hash(word);
```

Here's how the hash function works: 

```
// Hashes word to a number
unsigned int hash(const char *word)
{
    return toupper(word[0]) - 'A';
}
```

It takes word as an argument, the take the index of the first character of the word in the alphabet by subtracting 'A' from it, for example Z - A is 25 which is the last index in our hash table. 


check() 


This function takes a word from the text as an argument and return true if it is in dictionary else it returns false. 

```
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
```

First hash the word and determine in which bucket in our hash table this word might belong to. 

Then create a node pointer cursor to point to the first node in table[h]. 

While our cursor is not a NULL compare word with the word inside the node cursor points to and return true if they are equal, else update the cursor to point to the next node in the linked list. 

If you went trough the whole linked list and the word is not there then return false at the end. 


unload()


Finally we are going to write a function that free the memory we allocate for each node in our hash table: 

```
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
```

I loop inside each row or linked list in the hash table. 

Set cursor to point to a the first node in the current linked list. 

While cursor doesn't point to NULL create a temporary node pointer to point to cursor's node, then update cursor to point to the next node, then free tmp that points to the current node, that way we free the current node while saving access to the next node in cursor for the next iteration. 

Then check in each iteration in the hash table whether cursor is NULL and i is N - 1, that mean check if cursor points to NULL and we have reached the last bucket or linked list in our hash table, if that's true then we have freed all the nodes in the hash table and we can return true, if that is not true we return false at the end of the function. 

Output example of running the program: 
