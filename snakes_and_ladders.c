/***************************/
/*        INCLUDE          */
/***************************/
#include <string.h> // For strlen(), strcmp(), strcpy()
#include "markov_chain.h"
/***************************/

/***************************/
/*        DEFINES          */
/***************************/
#define MAX(X, Y) (((X) < (Y)) ? (Y) : (X))
#define EMPTY -1
#define BOARD_SIZE 100
#define MAX_GENERATION_LENGTH 60
#define DICE_MAX 6
#define NUM_OF_TRANSITIONS 20
#define ARGC_NUM 3
#define WRONG_ARGC "Usage: The program require the following parameters\n\
1) Seed number.\n\
2) Number of lines to generate.\n"
#define BASE 10
#define SEED 1
#define NUM_ROUTES 2
#define NOT_MATCH 1
#define RANDOM_WALK "Random Walk %d: "
#define P_REGULAR_CEIL "[%d] -> "
#define P_LAST_CEIL "[%d]"
#define P_SNAKE_CEIL "[%d]-snake to %d -> "
#define P_LADDER_CEIL "[%d]-ladder to %d -> "
#define DOWN_LINE "\n"
/***************************/

/***************************/
/*        STRUCTS          */
/***************************/
typedef struct Cell
{
    int number; // Cell number 1-100
    int ladder_to;// ladder_to represents the jump of the ladder in case
    // there is one from this square
    int snake_to;  // snake_to represents the jump of the snake in case
    // there is one from this square both ladder_to and snake_to should be
    // -1 if the Cell doesn't have them
} Cell;
/***************************/

/***************************/
/*        FUNCTION         */
/***************************/
///handle errors - free all allocations
static int handle_error (char *error_msg, MarkovChain **database);

///create new board for the game
static int create_board (Cell *cells[BOARD_SIZE]);

///fill the markov_chain
static int fill_database (MarkovChain *markov_chain);

///check if the argc are correct
static int check_argc (int argc);

///print function for cells
static void print (void *ceil);

///compare function for cells
static int compare (void *ceil1, void *ceil2);

///free functions for cells
static void free_func (void *ceil);

///copy functions for cells
static void *copy (void *ceil);

///is last function for cells - check if the cell is the last one
static bool is_last_func (void *ceil);

/// create game routes
static void create_routes (MarkovChain *markov_chain, int num_routes);
/***************************/


/**
 * represents the transitions by ladders and snakes in the game
 * each tuple (x,y) represents a ladder from x to if x<y or a snake otherwise
 */
const int transitions[][2] = {{13, 4},
                              {85, 17},
                              {95, 67},
                              {97, 58},
                              {66, 89},
                              {87, 31},
                              {57, 83},
                              {91, 25},
                              {28, 50},
                              {35, 11},
                              {8,  30},
                              {41, 62},
                              {81, 43},
                              {69, 32},
                              {20, 39},
                              {33, 70},
                              {79, 99},
                              {23, 76},
                              {15, 47},
                              {61, 14}};

/**
 * struct represents a Cell in the game board
 */


/** Error handler **/
static int handle_error (char *error_msg, MarkovChain **database)
{
  ///TODO CHECK FORM
  printf ("%s", error_msg);
  if (database != NULL)
  {
    free_markov_chain (database);
  }
  return EXIT_FAILURE;
}

static int create_board (Cell *cells[BOARD_SIZE])
{
  for (int i = 0; i < BOARD_SIZE; i++)
  {
    cells[i] = malloc (sizeof (Cell));
    if (cells[i] == NULL)
    {
      for (int j = 0; j < i; j++)
      {
        free (cells[j]);
      }
      handle_error (ALLOCATION_ERROR_MASSAGE, NULL);
      return EXIT_FAILURE;
    }
    *(cells[i]) = (Cell) {i + 1, EMPTY, EMPTY};
  }

  for (int i = 0; i < NUM_OF_TRANSITIONS; i++)
  {
    int from = transitions[i][0];
    int to = transitions[i][1];
    if (from < to)
    {
      cells[from - 1]->ladder_to = to;
    }
    else
    {
      cells[from - 1]->snake_to = to;
    }
  }
  return EXIT_SUCCESS;
}

/**
 * fills database
 * @param markov_chain
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */
static int fill_database (MarkovChain *markov_chain)
{
  Cell *cells[BOARD_SIZE];
  if (create_board (cells) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  MarkovNode *from_node = NULL, *to_node = NULL;
  size_t index_to;
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    add_to_database (markov_chain, cells[i]);
  }

  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    from_node = get_node_from_database (markov_chain, cells[i])->data;

    if (cells[i]->snake_to != EMPTY || cells[i]->ladder_to != EMPTY)
    {
      index_to = MAX(cells[i]->snake_to, cells[i]->ladder_to) - 1;
      to_node = get_node_from_database (markov_chain, cells[index_to])
          ->data;
      add_node_to_counter_list (from_node, to_node, markov_chain);
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
        to_node = get_node_from_database (markov_chain, cells[index_to])
            ->data;
        add_node_to_counter_list (from_node, to_node, markov_chain);
      }
    }
  }
  //free temp arr
  for (size_t i = 0; i < BOARD_SIZE; i++)
  {
    free (cells[i]);
  }
  return EXIT_SUCCESS;
}

/**
 * @param argc num of arguments
 * @param argv 1) Seed
 *             2) Number of sentences to generate
 * @return EXIT_SUCCESS or EXIT_FAILURE
 */

static void *copy (void *ceil)
{
  Cell *cur_ceil = (Cell *) ceil;
  Cell *new_ceil = malloc (sizeof (Cell));
  if (new_ceil == NULL)
  {
    return NULL;
  }
  new_ceil->number = cur_ceil->number;
  new_ceil->ladder_to = cur_ceil->ladder_to;
  new_ceil->snake_to = cur_ceil->snake_to;
  return new_ceil;
}

static int compare (void *ceil1, void *ceil2)
{
  Cell *ceil_1 = (Cell *) ceil1;
  Cell *ceil_2 = (Cell *) ceil2;
  if (ceil_1->number != ceil_2->number)
  {
    return NOT_MATCH;
  }
  if (ceil_1->ladder_to != ceil_2->ladder_to)
  {
    return NOT_MATCH;
  }
  if (ceil_1->snake_to != ceil_2->snake_to)
  {
    return NOT_MATCH;
  }
  return MATCH;
}

static bool is_last_func (void *ceil)
{
  Cell *cur_ceil = (Cell *) ceil;
  return (cur_ceil->number == BOARD_SIZE);
}

static void free_func (void *ceil)
{
  Cell *cur_ceil = (Cell *) ceil;
  free (cur_ceil);
}

static void print (void *ceil)
{
  Cell *cur_ceil = (Cell *) ceil;
  if (cur_ceil->ladder_to != EMPTY)
  {
    fprintf
        (stdout, P_LADDER_CEIL,
         cur_ceil->number, cur_ceil->ladder_to);
    return;
  }
  else if (cur_ceil->snake_to != EMPTY)
  {
    fprintf
        (stdout, P_SNAKE_CEIL,
         cur_ceil->number, cur_ceil->snake_to);
    return;
  }
  if (cur_ceil->number == BOARD_SIZE)
  {
    fprintf (stdout, P_LAST_CEIL, cur_ceil->number);
  }
  else
  {
    fprintf (stdout, P_REGULAR_CEIL, cur_ceil->number);
  }
}

int main (int argc, char *argv[])
{
  if (check_argc (argc) == EXIT_FAILURE)
  {
    return EXIT_FAILURE;
  }
  unsigned seed = (unsigned int) strtol (argv[SEED], NULL, BASE);
  srand (seed);
  int num_routes = (int) strtol (argv[NUM_ROUTES], NULL, BASE);
  MarkovChain *markov_chain = create_markov_chain
      (free_func, print, compare,
       copy, is_last_func);
  if (markov_chain == NULL)
  {
    return EXIT_FAILURE;
  }
  if (fill_database (markov_chain) == EXIT_FAILURE)
  { //in case failed by allocation
    return EXIT_FAILURE;
  }
  create_routes (markov_chain, num_routes);//create the tweets
  free_markov_chain (&markov_chain);
  return EXIT_SUCCESS;
}

static int check_argc (int argc)
{
  if (argc != ARGC_NUM)
  {
    fprintf (stdout, WRONG_ARGC);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void create_routes (MarkovChain *markov_chain, int num_routes)
{
  MarkovNode *first = markov_chain->database->first->data;
  for (int i = 1; i <= num_routes; i++)
  { //create the tweets
    fprintf (stdout, RANDOM_WALK, i);
    generate_random_sequence
    (markov_chain, first, MAX_GENERATION_LENGTH);
    fprintf (stdout, DOWN_LINE);
  }
}

