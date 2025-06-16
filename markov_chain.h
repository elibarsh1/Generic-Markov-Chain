#ifndef _MARKOV_CHAIN_H
#define _MARKOV_CHAIN_H

#include "linked_list.h"
#include <stdio.h>  // For printf(), sscanf()
#include <stdlib.h> // For exit(), malloc()
#include <stdbool.h> // for bool

//Don't change the macros!
#define ALLOCATION_ERROR_MESSAGE "Allocation failure: Failed to allocate"\
         "new memory\n"


/***************************/
/*   insert typedefs here  */
/***************************/
/**
 * Function pointer type for printing data
 * @param data pointer to generic data to print
 */
typedef void (*print_func)(void *data);

/**
 * Function pointer type for comparing two data elements
 * @param first_data pointer to first data element
 * @param second_data pointer to second data element
 * @return positive value if first > second, negative if first < second, 0 if equal
 */
typedef int (*comp_func)(void *first_data, void *second_data);

/**
 * Function pointer type for freeing data
 * @param data pointer to data to free
 */
typedef void (*free_data)(void *data);

/**
 * Function pointer type for copying data
 * @param data pointer to data to copy
 * @return pointer to newly allocated copy of data
 */
typedef void* (*copy_func)(void *data);

/**
 * Function pointer type for checking if data should be last in sequence
 * @param data pointer to data to check
 * @return true if data should be last in sequence, false otherwise
 */
typedef bool (*is_last)(void *data);


/***************************/
/*        STRUCTS          */
/***************************/

typedef struct MarkovNode {
    void *data;
    struct MarkovNodeFrequency *frequency_list;
    // any other fields you need
    int frequency_list_size;
} MarkovNode;

typedef struct MarkovNodeFrequency {
    struct MarkovNode *markov_node;
    int frequency;
    // any other fields you need
} MarkovNodeFrequency;

/* DO NOT ADD or CHANGE variable names in this struct */
typedef struct MarkovChain {
    LinkedList *database;

    // Function pointers for generic operations
    print_func print_func;
    comp_func comp_func;
    free_data free_data;
    copy_func copy_func;
    is_last is_last;
} MarkovChain;

/**
 * Check if data_ptr is in database. If so, return the markov_node wrapping
 * it in the markov_chain, otherwise return NULL.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return Pointer to the Node wrapping given state, NULL if state not in
 * database.
 */
Node *get_node_from_database(MarkovChain *markov_chain, void *data_ptr);

/**
* If data_ptr in markov_chain, return its node. Otherwise, create new
 * node, add to end of markov_chain's database and return it.
 * @param markov_chain the chain to look in its database
 * @param data_ptr the state to look for
 * @return node wrapping given data_ptr in given chain's database
 */
Node *add_to_database(MarkovChain *markov_chain, void *data_ptr);

/**
 * Add the second markov_node to the frequency list of the first markov_node.
 * If already in list, update its frequency value.
 * @param first_node
 * @param second_node
 * @return success/failure: 0 if the process was successful, 1 if in
 * case of allocation error.
 */
int add_node_to_frequency_list(MarkovNode *first_node,
                               MarkovNode *second_node);

/**
 * Free markov_chain and all of it's content from memory
 * @param chain_ptr markov_chain to free
 */
void free_database(MarkovChain **chain_ptr);

/**
 * Get one random markov node from the given markov_chain's database.
 * @param markov_chain
 * @return MarkovNode of the chosen state that is not a "last state"
 * in sequence.
 */
MarkovNode *get_first_random_node(MarkovChain *markov_chain);

/**
 * Choose the next node, by its occurrence frequency in current node.
 * @param cur_markov_node MarkovNode to choose from
 * @return MarkovNode of the chosen state
 */
MarkovNode *get_next_random_node(MarkovNode *cur_markov_node);

/**
 * Receive markov_chain, generate and print random sequences out of it. The
 * sequence most have at least 2 words in it.
 * @param markov_chain
 * @param first_node markov_node to start with, if NULL- choose a
 * random markov_node
 * @param  max_length maximum length of chain to generate
 */
void generate_random_sequence(MarkovChain *markov_chain,
                              MarkovNode *first_node, int max_length);

#endif /* MARKOV_CHAIN_H */
