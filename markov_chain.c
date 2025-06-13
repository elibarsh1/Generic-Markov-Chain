#include "markov_chain.h"

#include <string.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number
 * @return Random number
 */
int get_random_number(int max_number)
{
    return rand() % max_number;
}

Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr) {
    // Check for NULL inputs
    if (markov_chain == NULL || data_ptr == NULL || markov_chain->database == NULL) {
        return NULL;
    }

    // Get the first node in the linked list
    Node *current = markov_chain->database->first;

    // Traverse the linked list
    while (current != NULL) {
        // Access the MarkovNode's data field
        MarkovNode *current_markov_node = current->data;

        // Compare the strings to check if this is the node we're looking for
        if (current_markov_node != NULL && current_markov_node->data != NULL) {
            if (strcmp(current_markov_node->data, data_ptr) == 0) {
                // Found the node containing the data
                return current;
            }
        }

        // Move to the next node in the linked list
        current = current->next;
    }

    // Data not found in the database
    return NULL;
}

Node* add_to_database(MarkovChain *markov_chain, char *data_ptr) {
    // Check for NULL inputs
    if (markov_chain == NULL || data_ptr == NULL) {
        return NULL;
    }

    // First, check if the node already exists in the database
    Node *existing_node = get_node_from_database(markov_chain, data_ptr);

    // If the node already exists, return it
    if (existing_node != NULL) {
        return existing_node;
    }

    // Create a new MarkovNode for the data
    MarkovNode *new_markov_node = malloc(sizeof(MarkovNode));
    if (new_markov_node == NULL) {
        // Memory allocation failed
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        return NULL;
    }

    // Allocate memory for the string data and copy it
    new_markov_node->data = malloc(strlen(data_ptr) + 1); // +1 for null terminator
    if (new_markov_node->data == NULL) {
        // Memory allocation failed
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        free(new_markov_node); // Clean up the previously allocated memory
        return NULL;
    }

    // Copy the string data
    strcpy(new_markov_node->data, data_ptr);

    // Initialize frequency list to NULL (empty) and size to 0
    new_markov_node->frequency_list = NULL;
    new_markov_node->frequency_list_size = 0;

    // Add the new MarkovNode to the linked list
    int add_result = add(markov_chain->database, new_markov_node);

    if (add_result != 0) {
        // Failed to add the node to the database
        free(new_markov_node->data);  // Clean up allocated memory
        free(new_markov_node);
        return NULL;
    }

    // Return the newly added node
    return markov_chain->database->last; // Assuming the node was added at the end
}


int add_node_to_frequency_list(MarkovNode *first_node, MarkovNode *second_node) {
    // Check for NULL inputs
    if (first_node == NULL || second_node == NULL) {
        return 1;
    }

    // Case 1: first_node has no frequency list yet (empty list)
    if (first_node->frequency_list == NULL) {
        // Allocate memory for the first item in frequency list
        first_node->frequency_list = malloc(sizeof(MarkovNodeFrequency));
        if (first_node->frequency_list == NULL) {
            // Memory allocation failed
            fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
            return 1;
        }

        // Initialize the first entry
        first_node->frequency_list[0].markov_node = second_node;
        first_node->frequency_list[0].frequency = 1;

        // We need to track how many items are in the frequency list
        // You might want to add a field to MarkovNode for this, for example:
        // first_node->frequency_list_size = 1;
        first_node->frequency_list_size = 1;

        return 0; // Success
    }

    // Case 2: first_node already has a frequency list
    // We need to find out how many items are in the frequency list

    // Iterate through the existing frequency list to find if second_node is already in it
    int i;
    int frequency_list_size = first_node->frequency_list_size;

    for (i = 0; i < frequency_list_size; i++) {
        if (first_node->frequency_list[i].markov_node == second_node) {
            // Found the node, update its frequency
            first_node->frequency_list[i].frequency++;
            return 0; // Success
        }
    }

    // If we get here, second_node is not yet in the frequency list
    // We need to resize the list to add one more item
    MarkovNodeFrequency *new_list = realloc(first_node->frequency_list,
                                          (frequency_list_size + 1) * sizeof(MarkovNodeFrequency));
    if (new_list == NULL) {
        // Memory reallocation failed
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        return 1;
    }

    // Update the list pointer to the reallocated memory
    first_node->frequency_list = new_list;

    // Add the new node to the end of the list
    first_node->frequency_list[frequency_list_size].markov_node = second_node;
    first_node->frequency_list[frequency_list_size].frequency = 1;

    // Increment the size of the frequency list
    first_node->frequency_list_size++;

    return 0; // Success
}



/**
 * Free markov_chain and all of its content from memory
 * @param ptr_chain pointer to markov_chain to free
 */
void free_database(MarkovChain **ptr_chain) {
    if (ptr_chain == NULL || *ptr_chain == NULL) {
        return;
    }

    MarkovChain *chain = *ptr_chain;
    LinkedList *database = chain->database;

    if (database != NULL) {
        Node *current = database->first;

        // Traverse the linked list and free all MarkovNodes
        while (current != NULL) {
            MarkovNode *markov_node = (MarkovNode *)current->data;

            if (markov_node != NULL) {
                // Free the string data
                if (markov_node->data != NULL) {
                    free(markov_node->data);
                }

                // Free the frequency list
                if (markov_node->frequency_list != NULL) {
                    free(markov_node->frequency_list);
                }

                // Free the MarkovNode itself
                free(markov_node);
            }

            // Move to the next node
            Node *temp = current;
            current = current->next;

            // Free the Node wrapper
            free(temp);
        }

        // Free the LinkedList
        free(database);
    }

    // Free the MarkovChain
    free(chain);

    // Set the pointer to NULL
    *ptr_chain = NULL;
}