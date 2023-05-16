/***************************/
/*        INCLUDE          */
/***************************/
#include "markov_chain.h"
#include "linked_list.h"
#include "string.h"
/***************************/

/***************************/
/*        DEFINES           */
/***************************/
#define DOT '.'
#define READ "r"
#define START 0
#define MAX_LINE_TWEET 20
#define MAX_LEN_LINE 1000
#define SPACE " \n\r"
#define FIRST_ARGC 4
#define SEC_ARGC 5
#define WRONG_ARGC "Usage: The program require the following parameters\n\
1) Seed number\n\
2) Number of lines to generate.\n\
3) Absolute path to file.\n\
4) Optional - Number of lines to read from file.\n\
If no value is given read the entire file.\n"
#define SEED 1
#define TWEETS 2
#define INPUT_FILE 3
#define NUM_TO_READ 4
#define BASE 10
#define NO_NUM_TO_READ -1
#define PRINT_TWEET "Tweet %d: "
#define ERROR_AT_FILE_OPEN "Error: Failed to open file, \
make sure you use absolute path.\n"
/***************************/
/***************************/
/*        FUNCTIONS        */
/***************************/
///or to free in main like before

/// checking the num arguments
static int check_argc (int argc);

/// fill database
static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain);

/// put arguments in variables
static void put_argv_arguments\
 (char *argv[], int argc\
, unsigned int *seed, int *tweets_num, char **input_file, int *num_to_read);

/// open the input file
static FILE *open_file (char *input_file);

/// read every line from the file
static int
read_lines (char *buffer, MarkovChain *markov_chain, int *words_to_read);

///create all the tweets
static void create_tweets (MarkovChain *markov_chain, int tweets_num);

///print function for char *
static void print (void *str);

///compare function for char *
static int compare (void *str1, void *str2);

///free function for char *
static void free_func (void *str);

///copy function for char *
static void *copy (void *str);

///is last function fo char *
static bool is_last_func (void *str);
/***************************/

static void print (void *str)
{
  char *cur = (char *) str;
  if(is_last_func (cur) == true)
  {
    printf ("%s", cur);// print the data in case last
  }
  else
  {
    printf ("%s ", cur);// print the data
  }
}

static int compare (void *str1, void *str2)
{
  char *str_1 = (char *) str1;
  char *str_2 = (char *) str2;
  return strcmp (str_1, str_2); //compare between the two data
}

static void free_func (void *str)
{
  char *cur = (char *) str;
  free (cur); //free the data
}

static void *copy (void *str)
{
  char *cur_str = (char *) str;
  char *new_str = malloc (strlen (cur_str) + 1);
  if (new_str == NULL)
  {
    return NULL;
  }
  strcpy (new_str, cur_str);// copy
  return new_str;
}

static bool is_last_func (void *str)
{
  char *cur = (char *) str;
  int size = (int) strlen (cur);
  return cur[size - 1] == DOT; //check if the last char id dot
}

int main (int argc, char *argv[])
{
  if (check_argc (argc) == EXIT_FAILURE) //check number of arguments
  {
    return EXIT_FAILURE;
  }
  unsigned int seed = START;
  int tweets_num = START;
  char *input_file = NULL;
  int num_to_read = NO_NUM_TO_READ;
  put_argv_arguments \
 (argv, argc, &seed, &tweets_num, \
  &input_file, &num_to_read); //put the arguments in var
  FILE *input = open_file (input_file);//open the file
  if (input == NULL)
  {
    return EXIT_FAILURE;
  }
  MarkovChain *markov_chain = create_markov_chain
      (free_func, print, compare,
       copy, is_last_func);
  if (markov_chain == NULL)
  {
    fclose (input);
    return EXIT_FAILURE;
  }

  if (fill_database (input, num_to_read, markov_chain) == EXIT_FAILURE)
  { //in case failed by allocation
    fclose (input);
    return EXIT_FAILURE;
  }
  fclose (input);//close file after transfer the data
  srand (seed);
  create_tweets (markov_chain, tweets_num);//create the tweets
  free_markov_chain (&markov_chain); //free all the allocation
  return EXIT_SUCCESS;
}

static int check_argc (int argc)
{
  if (argc != FIRST_ARGC && argc != SEC_ARGC)
  {
    fprintf (stdout, WRONG_ARGC);
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

static void put_argv_arguments\
 (char *argv[], int argc\
, unsigned int *seed, int *tweets_num, char **input_file, int *num_to_read)
{
  *seed = (unsigned int) strtol (argv[SEED], NULL, BASE);
  *tweets_num = (int) strtol (argv[TWEETS], NULL, BASE);
  *input_file = argv[INPUT_FILE];
  if (argc == SEC_ARGC)
  {//if we get number to read so we changing it to the number
    *num_to_read = (int) strtol (argv[NUM_TO_READ], NULL, BASE);
  }
}

static FILE *open_file (char *input_file)
{
  FILE *input = fopen (input_file, READ);
  if (input == NULL)
  {
    fprintf (stdout, ERROR_AT_FILE_OPEN);
    return NULL;
  }
  return input;
}

static int
fill_database (FILE *fp, int words_to_read, MarkovChain *markov_chain)
{
  char buffer[MAX_LEN_LINE];
  while ((words_to_read != START)
         && (fgets (buffer, MAX_LEN_LINE, fp) != NULL))
  { //read every line
    if (read_lines (buffer, markov_chain, &words_to_read) == EXIT_FAILURE)
    { // if the allocation failed
      fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
      return EXIT_FAILURE;
    }
  }
  return EXIT_SUCCESS;
}

static int
read_lines (char *buffer, MarkovChain *markov_chain, int *words_to_read)
{
  char *word = strtok (buffer, SPACE); // split the word by space
  Node *new_node = NULL;
  while (word != NULL && *words_to_read != START)
  {
    Node *recent_node = new_node;
    new_node = add_to_database (markov_chain, (void *) word);
    if (new_node == NULL)
    {
      return EXIT_FAILURE;
    }
    if (recent_node != NULL
        && markov_chain->is_last (recent_node->data->data) == false)
    { //check if to add to counter list
      if (add_node_to_counter_list
              (recent_node->data, new_node->data, markov_chain) == false)
      {
        return EXIT_FAILURE;
      }
    }
    (*words_to_read)--;
    word = strtok (NULL, SPACE);
  }
  return EXIT_SUCCESS;
}

static void create_tweets (MarkovChain *markov_chain, int tweets_num)
{
  for (int i = 1; i <= tweets_num; i++)
  { //create the tweets
    fprintf (stdout, PRINT_TWEET, i);
    generate_random_sequence (markov_chain, NULL, MAX_LINE_TWEET);
    fprintf (stdout, "\n");
  }
}


















