#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"

#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))

#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60

#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20

#define NUM_ARGS_ERROR "Usage: invalid number of arguments"

/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {
    {13, 4},
    {85, 17},
    {95, 67},
    {97, 58},
    {66, 89},
    {87, 31},
    {57, 83},
    {91, 25},
    {28, 50},
    {35, 11},
    {8, 30},
    {41, 62},
    {81, 43},
    {69, 32},
    {20, 39},
    {33, 70},
    {79, 99},
    {23, 76},
    {15, 47},
    {61, 14}
};

int get_random_number(int max_number);

/**
 * struct represents a Cell in the game board
 */
typedef struct Cell {
    int number; // Cell number 1-100
    int ladder_to; // cell which ladder leads to, if there is one
    int snake_to; // cell which snake leads to, if there is one
    //both ladder_to and snake_to should be -1 if the Cell doesn't have them
} Cell;

/**
 * Print function for Cell
 * @param data pointer to Cell data
 */
void print_cell(void *data) {
    if (data != NULL) {
        Cell *cell = (Cell*)data;
        printf("[%d]", cell->number);
    }
}

/**
 * Comparison function for Cells
 * @param first_data pointer to first Cell
 * @param second_data pointer to second Cell
 * @return comparison result
 */
int comp_cells(void *first_data, void *second_data) {
    if (first_data == NULL || second_data == NULL) {
        return 0;
    }
    Cell *first = (Cell*)first_data;
    Cell *second = (Cell*)second_data;
    return first->number - second->number;
}

/**
 * Free function for Cell (no dynamic allocation needed for Cell itself)
 * @param data pointer to Cell data
 */
void free_cell(void *data) {
    (void)data;
    // Cell doesn't contain dynamically allocated memory, so nothing to free
    // The Cell itself will be freed by the MarkovChain cleanup
}

/**
 * Copy function for Cell
 * @param data pointer to Cell data to copy
 * @return pointer to newly allocated copy
 */
void* copy_cell(void *data) {
    if (data == NULL) {
        return NULL;
    }

    Cell *original = (Cell*)data;
    Cell *copy = malloc(sizeof(Cell));
    if (copy == NULL) {
        return NULL;
    }

    *copy = *original;
    return copy;
}

/**
 * Check if Cell should be last in sequence (cell number 100)
 * @param data pointer to Cell data
 * @return true if cell number is 100, false otherwise
 */
bool is_last_cell(void *data) {
    if (data == NULL) {
        return false;
    }

    Cell *cell = (Cell*)data;
    return cell->number == BOARD_SIZE;
}

/**
 * allocates memory for cells on the board and initalizes them
 * @param cells Array of pointer to Cell, represents game board
 * @return EXIT_SUCCESS if successful, else EXIT_FAILURE
 */
int create_board(Cell *cells[BOARD_SIZE])
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        cells[i] = malloc(sizeof(Cell));
        if (cells[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(cells[j]);
            }
            printf(ALLOCATION_ERROR_MESSAGE);
            return EXIT_FAILURE;
        }
        *(cells[i]) = (Cell){i + 1, EMPTY, EMPTY};
    }

    for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
    {
        int from = transitions[i][0];
        int to = transitions[i][1];
        if (from < to)
        {
            cells[from - 1]->ladder_to = to;
        } else
        {
            cells[from - 1]->snake_to = to;
        }
    }
    return EXIT_SUCCESS;
}

int add_cells_to_database(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        Node *tmp = add_to_database(markov_chain, cells[i]);
        if (tmp == NULL)
        {
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

int set_nodes_frequencies(MarkovChain *markov_chain, Cell *cells[BOARD_SIZE])
{
    MarkovNode *from_node = NULL, *to_node = NULL;
    size_t index_to;

    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        from_node = get_node_from_database(markov_chain, cells[i])->data;
        if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
        {
            index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
            to_node = get_node_from_database(markov_chain,
                                             cells[index_to])->data;
            int res = add_node_to_frequency_list(from_node, to_node);
            if (res == EXIT_FAILURE)
            {
                return EXIT_FAILURE;
            }
        }
        else
        {
            for (int j = 1; j <= DICE_MAX; j++)
            {
                index_to = ((Cell *) (from_node->data))->number + j - 1;
                if (index_to >= BOARD_SIZE)
                {
                    break;
                }
                to_node = get_node_from_database(markov_chain,
                                                 cells[index_to])->data;
                int res = add_node_to_frequency_list(from_node, to_node);
                if (res == EXIT_FAILURE)
                {
                    return EXIT_FAILURE;
                }
            }
        }
    }
    return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int fill_database_snakes(MarkovChain *markov_chain)
{
    Cell *cells[BOARD_SIZE];
    if (create_board(cells) == EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    if (add_cells_to_database(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    if(set_nodes_frequencies(markov_chain, cells) == EXIT_FAILURE)
    {
        for (size_t i = 0; i < BOARD_SIZE; i++)
        {
            free(cells[i]);
        }
        return EXIT_FAILURE;
    }

    // free temp arr
    for (size_t i = 0; i < BOARD_SIZE; i++)
    {
        free(cells[i]);
    }
    return EXIT_SUCCESS;
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
 * Generate and print a random walk path
 * @param markov_chain The markov chain
 * @param first_node The starting cell (always cell 1)
 * @param max_length Maximum path length
 * @param path_num Path number for display
 */
void generate_random_walk(MarkovChain *markov_chain, MarkovNode *first_node, int max_length, int path_num) {
    if (markov_chain == NULL || first_node == NULL || max_length <= 0) {
        return;
    }

    printf("Random Walk %d: ", path_num);

    MarkovNode *current_node = first_node;
    MarkovNode *next_node = NULL;
    int step_count = 0;

    while (step_count < max_length) {
        Cell *current_cell = (Cell*)current_node->data;

        // Print current cell
        markov_chain->print_func(current_node->data);
        step_count++;

        // Check if we reached the end (cell 100)
        if (markov_chain->is_last(current_node->data)) {
            break;
        }

        // Get the next node
        next_node = get_next_random_node(current_node);
        if (next_node == NULL) {
            break;
        }

        Cell *next_cell = (Cell*)next_node->data;

        // Print transition arrow based on ladder/snake/normal move
        if (current_cell->ladder_to != EMPTY && next_cell->number == current_cell->ladder_to) {
            printf(" -> ladder to ");
        } else if (current_cell->snake_to != EMPTY && next_cell->number == current_cell->snake_to) {
            printf(" -> snake to ");
        } else {
            printf(" -> ");
        }

        current_node = next_node;
    }

    // If we didn't reach cell 100 but reached max length, print final arrow
    if (step_count >= max_length && !markov_chain->is_last(current_node->data)) {
        Cell *current_cell = (Cell*)current_node->data;
        if (current_cell->ladder_to != EMPTY) {
            printf("ladder to ");
        } else if (current_cell->snake_to != EMPTY) {
            printf("snake to ");
        }
    }

    printf("\n");
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of paths to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char *argv[])
{
    // Check argument count
    if (argc != 3) {
        printf("%s\n", NUM_ARGS_ERROR);
        return EXIT_FAILURE;
    }

    // Parse and validate seed
    char *endptr;
    unsigned int seed = (unsigned int)strtol(argv[1], &endptr, 10);
    if (*endptr != '\0') {
        printf("Error: Invalid seed value.\n");
        return EXIT_FAILURE;
    }
    srand(seed);

    // Parse and validate number of paths
    int num_paths = (int)strtol(argv[2], &endptr, 10);
    if (*endptr != '\0' || num_paths <= 0) {
        printf("Error: Invalid number of paths.\n");
        return EXIT_FAILURE;
    }

    // Create and initialize the markov chain
    MarkovChain *markov_chain = malloc(sizeof(MarkovChain));
    if (markov_chain == NULL) {
        printf(ALLOCATION_ERROR_MESSAGE);
        return EXIT_FAILURE;
    }

    // Initialize the database (linked list)
    markov_chain->database = malloc(sizeof(LinkedList));
    if (markov_chain->database == NULL) {
        printf(ALLOCATION_ERROR_MESSAGE);
        free(markov_chain);
        return EXIT_FAILURE;
    }

    // Initialize the linked list fields
    markov_chain->database->first = NULL;
    markov_chain->database->last = NULL;
    markov_chain->database->size = 0;

    // Set up the function pointers for Cell operations
    markov_chain->print_func = print_cell;
    markov_chain->comp_func = comp_cells;
    markov_chain->free_data = free_cell;
    markov_chain->copy_func = copy_cell;
    markov_chain->is_last = is_last_cell;

    // Fill the markov chain with the board
    if (fill_database_snakes(markov_chain) != EXIT_SUCCESS) {
        printf(ALLOCATION_ERROR_MESSAGE);
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }

    // Find cell 1 (starting cell)
    Cell start_cell = {1, EMPTY, EMPTY};
    Node *start_node = get_node_from_database(markov_chain, &start_cell);
    if (start_node == NULL) {
        printf("Error: Could not find starting cell.\n");
        free_database(&markov_chain);
        return EXIT_FAILURE;
    }

    MarkovNode *start_markov_node = (MarkovNode*)start_node->data;

    // Generate and print the random walks
    for (int i = 1; i <= num_paths; i++) {
        generate_random_walk(markov_chain, start_markov_node, MAX_GENERATION_LENGTH, i);
    }

    // Free the allocated memory
    free_database(&markov_chain);

    return EXIT_SUCCESS;
}
