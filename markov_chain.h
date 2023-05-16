#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H



/***************************/
/*        INCLUDE          */
/***************************/
#include "linked_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <stdbool.h>
/***************************/


/***************************/
/*        DEFINE           */
/***************************/
#define START 0
#define FIRST_IN 1
#define ADD_PLACE 1
#define MATCH 0
#define COUNT(x, y) ((x)<(y))
#define ALLOCATION_ERROR_MASSAGE \
"Allocation failure: Failed to allocate new memory\n"
/***************************/


/***************************/
/*        TYPEDEF          */
/***************************/
typedef void (*print_func) (void *);
typedef int (*comp_func) (void *, void *);
typedef void (*free_data) (void *);
typedef void *(*copy_func) (void *);
typedef bool (*is_last) (void *);
typedef struct NextNodeCounter NextNodeCounter;
typedef struct MarkovNode MarkovNode;
typedef struct MarkovChain MarkovChain;
/***************************/



/***************************/
/*        STRUCTS          */
/***************************/

struct MarkovNode
{
    void *data;
    NextNodeCounter *counter_list; //markov counter list
    int size_next;//counter list size
    int total_next;//amount of frequency in counter list

};

struct NextNodeCounter
{
    MarkovNode *markov_node;
    int frequency; //amount of time the word was the next one
};

/* DO NOT ADD or CHANGE variable names in this struct */
struct MarkovChain
{
    LinkedList *database;

    // pointer to a func that receives data from a generic type and prints it
    // returns void.
    print_func print_func;

    // pointer to a func that gets 2 pointers of generic data type(same one)
    // and compare between them */
    // returns: - a positive value if the first is bigger
    //          - a negative value if the second is bigger
    //          - 0 if equal
    comp_func comp_func;

    // a pointer to a function that gets a pointer of generic
    // data type and frees it.
    // returns void.
    free_data free_data;

    // a pointer to a function that  gets a pointer of generic
    // data type and returns a newly allocated copy of it
    // returns a generic pointer.
    copy_func copy_func;

    //  a pointer to function that gets a pointer of generic
    //  data type and returns:
    //      - true if it's the last state.
    //      - false otherwise.
    is_last is_last;

};
/***************************/




/***************************/
/*        FUNCTION         */
/***************************/

/**
 * Get one random state from the given markov_chain's database.
 * @param markov_chain
 * @return
 */
MarkovNode *get_first_random_node (MarkovChain *markov_chain);

/**
 * Choose randomly the next state, depend on it's occurrence frequency.
 * @param state_struct_ptr MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr);

/**
 * Receive markov_chain, generate and print random sentence out of it. The
 * sentence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose
 * a random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length);

/**
 * Free markov_chain and all of it's content from memory
 * @param markov_chain markov_chain to free
 */
void free_markov_chain (MarkovChain **markov_chain);

/**
 * Add the second markov_node to the counter list of the first markov_node.
 * If already in list, update it's counter value.
 * @param first_node
 * @param second_node
 * @param markov_chain
 * @return success/failure: true if the process was successful, false if in
 * case of allocation error.
 */
bool add_node_to_counter_list (MarkovNode *first_node, MarkovNode
*second_node, MarkovChain *markov_chain);

/**
* Check if data_ptr is in database. If so, return the markov_node
 * wrapping it in
 * the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return it's markov_node. Otherwise, create new
 * markov_node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return markov_node wrapping given data_ptr in given chain's database
 */
Node *add_to_database (MarkovChain *markov_chain, void *data_ptr);

/**
*find the next word in the tweet
 * @param markov_node current nod
 * @param rand num the number of the next word
 * @return MarkovNode * - the next node
 */
MarkovNode *find_next_node (MarkovNode *cur, int rand_num);

/**
*random number
 * @param max_number - the maximal number we can get
 * @return random number
 */
int get_random_number (int max_number);

/**
*check if the second is already in the first counter list
 * @param MarkovNode first node
 * @param MarkovNode second node
 * @return bool - true if there is,else false
 */
bool check_in_next
    (MarkovNode *first_node, MarkovNode *second_node
     , MarkovChain *markov_chain);
/**
*create markov_chain
 * @param MarkovNode free_data func
 * @param MarkovNode print_func
 * @param MarkovNode comp_func
 * @param MarkovNode copy_func
 * @param MarkovNode is_last func
 * @return bool - markov_cahin if allocation succeed else NULL
 */
MarkovChain *create_markov_chain
    (free_data free_func, print_func print_func,
     comp_func comp_func, copy_func copy_func, is_last is_last_func);

/***************************/

#endif /* MARKOV_CHAIN_H */
