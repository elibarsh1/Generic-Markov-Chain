#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"

#define MAX_LINE_LENGTH 1000
#define MAX_TWEET_LENGTH 20

//Don't change the macros!
#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

#define DELIMITERS " \n\t\r"

int get_random_number(int max_number);

/**
 * Reads lines from the specified file, adds words to the markov chain, and builds the connections
 * between them according to the text.
 * @param fp File pointer to the corpus file
 * @param words_to_read Maximum number of words to read, or -1 for unlimited
 * @param markov_chain The markov chain to update
 * @return 0 on success, 1 on failure (memory allocation error)
 */
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    char line[MAX_LINE_LENGTH];
    int words_read = 0;
    MarkovNode *prev_node = NULL;

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL) {
        char *word = strtok(line, DELIMITERS);

        while (word != NULL) {
            // Check if we reached the word limit
            if (words_to_read != -1 && words_read >= words_to_read) {
                return 0; // Successfully read the required number of words
            }

            // Add the current word to the database
            Node *word_node = add_to_database(markov_chain, word);
            if (word_node == NULL) {
                return 1; // Memory allocation error
            }

            // Get the MarkovNode for this word
            MarkovNode *current_node = word_node->data;
            words_read++;

            // If there was a previous word, connect it to the current word
            if (prev_node != NULL) {
                if (add_node_to_frequency_list(prev_node, current_node) != 0) {
                    return 1; // Memory allocation error
                }
            }

            // Check if this word ends with a period (end of sentence)
            int length = strlen(word);
            if (length > 0 && word[length - 1] == '.') {
                // This is the end of a sentence
                prev_node = NULL; // Reset for the next sentence
            } else {
                // Move to the next word
                prev_node = current_node;
            }

            // Get the next word
            word = strtok(NULL, DELIMITERS);
        }
    }

    return 0; // Success
}


/**
 * Returns a random first node from the database that isn't a sentence-ending word
 * @param markov_chain The markov chain
 * @return A random MarkovNode that isn't a sentence-ending word
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain) {
    if (markov_chain == NULL || markov_chain->database == NULL ||
        markov_chain->database->size == 0) {
        return NULL;
        }

    // Get a random node from the database
    Node *current = NULL;
    MarkovNode *random_node = NULL;
    int random_index;

    // Keep selecting random nodes until we find one that doesn't end with a period
    do {
        // Generate a random index between 0 and size-1
        random_index = get_random_number(markov_chain->database->size);

        // Get the node at the random index
        current = markov_chain->database->first;
        for (int i = 0; i < random_index && current != NULL; i++) {
            current = current->next;
        }

        // Get the MarkovNode from the current node
        if (current != NULL) {
            random_node = (MarkovNode*)current->data;
        } else {
            // This should never happen if the database is properly set up
            return NULL;
        }

        // Check if the node's data ends with a period (sentence-ending word)
        int length = strlen(random_node->data);
        if (length > 0 && random_node->data[length - 1] != '.') {
            // Found a suitable node (not a sentence-ending word)
            return random_node;
        }

        // If we're here, the selected node was a sentence-ending word
        // We'll try again with a new random index
    } while (1); // Keep trying until we find a suitable node

    // This line should never be reached
    return NULL;
}

/**
 * Returns a random next node from the given node's frequency list
 * The random selection is weighted by the frequencies of each following word
 * @param cur_markov_node Current MarkovNode to find a successor for
 * @return A random MarkovNode from the frequency list
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node) {
    // Check for NULL input or empty frequency list
    if (cur_markov_node == NULL ||
        cur_markov_node->frequency_list == NULL ||
        cur_markov_node->frequency_list_size == 0) {
        return NULL;
        }

    // Calculate the total frequency of all words that follow the current word
    int total_frequency = 0;
    for (int i = 0; i < cur_markov_node->frequency_list_size; i++) {
        total_frequency += cur_markov_node->frequency_list[i].frequency;
    }

    // Generate a random number between 0 and total_frequency - 1
    int random_num = get_random_number(total_frequency);

    // Select a word based on weighted probabilities
    int cumulative_frequency = 0;
    for (int i = 0; i < cur_markov_node->frequency_list_size; i++) {
        cumulative_frequency += cur_markov_node->frequency_list[i].frequency;
        if (random_num < cumulative_frequency) {
            return cur_markov_node->frequency_list[i].markov_node;
        }
    }

    // This should never happen if the frequency list is properly set up
    return NULL;
}



/**
 * Generates a tweet starting from the given first_node
 * Continues to select random next words based on the Markov chain probabilities
 * until reaching a sentence-ending word or max_length
 * @param first_node The first word in the tweet
 * @param max_length Maximum number of words to include in the tweet
 */
void generate_tweet(MarkovNode *first_node, int max_length) {
    if (first_node == NULL || max_length <= 0) {
        return;
    }

    MarkovNode *current_node = first_node;
    int word_count = 0;

    // Print the first word without a leading space
    printf("%s", current_node->data);
    word_count++;

    // Continue generating words until we reach max_length or a sentence-ending word
    while (word_count < max_length) {
        // Check if the current word ends with a period (end of sentence)
        int length = strlen(current_node->data);
        if (length > 0 && current_node->data[length - 1] == '.') {
            break; // End tweet generation when we reach a sentence-ending word
        }

        // Get the next random node
        current_node = get_next_random_node(current_node);

        // If there's no valid next node, end the tweet
        if (current_node == NULL) {
            break;
        }

        // Print the next word with a leading space
        printf(" %s", current_node->data);
        word_count++;
    }
}



int main(int argc, char *argv[]) {
    // Check if the correct number of arguments was provided
    if (argc != 4 && argc != 5) {
        fprintf(stdout, "%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    // Parse and validate the seed argument
    char *endptr;
    unsigned int seed = (unsigned int)strtol(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stdout, "Error: Invalid seed value.\n");
        return EXIT_FAILURE;
    }
    srand(seed);

    // Parse and validate the number of tweets argument
    int num_tweets = (int)strtol(argv[2], &endptr, 10);
    if (*endptr != '\0') {
        fprintf(stdout, "Error: Invalid number of tweets.\n");
        return EXIT_FAILURE;
    }
    if (num_tweets <= 0) {
        fprintf(stdout, "Error: Number of tweets must be a positive integer.\n");
        return EXIT_FAILURE;
    }

    // Open the text corpus file
    FILE *fp = fopen(argv[3], "r");
    if (fp == NULL) {
        fprintf(stdout, "%s\n", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }

    // Parse the number of words to read (optional argument)
    int words_to_read = -1; // Default: read all words
    if (argc == 5) {
        words_to_read = (int)strtol(argv[4], &endptr, 10);
        if (*endptr != '\0') {
            fprintf(stdout, "Error: Invalid number of words to read.\n");
            fclose(fp);
            return EXIT_FAILURE;
        }
        if (words_to_read <= 0) {
            fprintf(stdout, "Error: Number of words to read must be a positive integer.\n");
            fclose(fp);
            return EXIT_FAILURE;
        }
    }

    // Create and initialize the markov chain
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (markov_chain == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Initialize the database (linked list)
    markov_chain->database = malloc(sizeof(LinkedList));
    if (markov_chain->database == NULL) {
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        free(markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Initialize the linked list fields
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;

    // Fill the markov chain from the file
    if (fill_database(fp, words_to_read, markov_chain) != 0) {
        // Memory allocation error occurred
        fprintf(stderr, ALLOCATION_ERROR_MESSAGE);
        free_database(&markov_chain);
        fclose(fp);
        return EXIT_FAILURE;
    }

    // Close the file
    fclose(fp);

    // Generate and print the random tweets
    for (int i = 0; i < num_tweets; i++) {
        // Print the tweet header
        printf("Tweet %d: ", i + 1);

        // Get a random first node to start the tweet
        MarkovNode *first_node = get_first_random_node(markov_chain);
        if (first_node == NULL) {
            fprintf(stderr, "Error: Could not get a random starting node.\n");
            free_database(&markov_chain);
            return EXIT_FAILURE;
        }

        // Generate and print the tweet
        generate_tweet(first_node, MAX_TWEET_LENGTH);

        // Add a newline after each tweet
        printf("\n");
    }

    // Free the allocated memory
    free_database(&markov_chain);

    return EXIT_SUCCESS;
}