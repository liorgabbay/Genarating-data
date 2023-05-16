#include "markov_chain.h"

int get_random_number (int max_number)
{
  return rand () % max_number;
}

MarkovNode *get_first_random_node (MarkovChain *markov_chain)
{
  int max = markov_chain->database->size; //the size of the words to rand
  bool flag = true;
  Node *cur = markov_chain->database->first; //cur node - help find the i node
  while (flag) //check if we found a word that don't end with dot
  {
    cur = markov_chain->database->first;
    int rand_num = get_random_number (max); //get random number
    for (int i = 0; i < rand_num; i++)
    {
      cur = cur->next;
    }
    if (markov_chain->is_last (cur->data->data) == false)
    {// check if it is the last in the state
      flag = false;
    }
  }
  return cur->data;
}

MarkovNode *get_next_random_node (MarkovNode *state_struct_ptr)
{
  //get rand number
  int rand_num = get_random_number (state_struct_ptr->total_next);
  // find the next node
  MarkovNode *next_node = find_next_node (state_struct_ptr, rand_num);
  return next_node;
}

MarkovNode *find_next_node (MarkovNode *cur, int rand_num)
{
  int sum = START;
  NextNodeCounter *cur_next = cur->counter_list;
  if (cur_next == NULL)
  {
    return NULL;
  }
  else
  {
    int cur_frequency = START;
    while (sum <= rand_num)
    {//continue until finding the next one
      cur_frequency = cur_next->frequency;
      if (sum + cur_frequency > rand_num) //checking if we got to the next one
      {
        return cur_next->markov_node;
      }
      else
      {
        sum += cur_frequency;
        cur_next++;
      }
    }
    return NULL;
  }
}

void generate_random_sequence (MarkovChain *markov_chain, MarkovNode *
first_node, int max_length)
{
  bool flag = true; //check if we got to the end of the tweet word with dot
  int count = START; // check tweet length
  MarkovNode *cur_node = NULL;
  if (first_node != NULL) //check if we got first node
  {
    cur_node = first_node;
    markov_chain->print_func (cur_node->data);
  }
  else
  { //in case we didn't get the first node
    cur_node = get_first_random_node (markov_chain);
    markov_chain->print_func (cur_node->data);
  }
  count++;
  while (flag && COUNT(count, max_length))
  { //until we finish the tweet
    cur_node = get_next_random_node (cur_node);//get the next one
    if (markov_chain->is_last (cur_node->data) == true)
    {//in case we in the last letter
      flag = false;
      markov_chain->print_func (cur_node->data);
      count++;
    }
    else
    {//in other case
      markov_chain->print_func (cur_node->data);
      count++;
    }
  }
}

void free_markov_chain (MarkovChain **ptr_chain)
{
  if((*ptr_chain)!=NULL)
  {
    if ((*ptr_chain)->database != NULL)
    {
      Node *cur = (*ptr_chain)->database->first;
      while (cur != NULL)
      {
        if (cur->data != NULL)
        {
          free (cur->data->counter_list);//free counter_list
          cur->data->counter_list = NULL;
          (*ptr_chain)->free_data (cur->data->data);//free markov data
          cur->data->data = NULL;
          free (cur->data);//free markov node
          cur->data = NULL;
        }
        Node *tmp = cur;
        cur = cur->next;
        (*ptr_chain)->database->first = cur;//delete the first
        free (tmp);//free node
        tmp = NULL;
      }
      free (((*ptr_chain)->database));//free linked list
      ((*ptr_chain)->database) = NULL;
    }
//free markov chain
    free ((*ptr_chain));
    (*ptr_chain) = NULL;
  }
}

bool add_node_to_counter_list
    (MarkovNode *first_node, MarkovNode *second_node,
     MarkovChain *markov_chain)
{
  if (check_in_next (first_node, second_node, markov_chain) == EXIT_SUCCESS)
  { //if the word already in
    (first_node->total_next)++;
    return true;
  }
  else
  {//create more space in the counter list
    NextNodeCounter *tmp =
        realloc\
 (first_node->counter_list, \
 (first_node->size_next + ADD_PLACE) * sizeof (NextNodeCounter));
    if (tmp == NULL)
    { //check realloc success
      free_markov_chain (&markov_chain);
      return false;
    }
    first_node->counter_list = tmp;
    int size = first_node->size_next;
    (first_node->counter_list[size]).markov_node = second_node;
    (first_node->counter_list[size]).frequency = FIRST_IN;
    (first_node->size_next)++;
    (first_node->total_next)++;
    return true;
  }
}

bool check_in_next
    (MarkovNode *first_node, MarkovNode *second_node,
     MarkovChain *markov_chain)
{ //check if the second node is already in count list
  for (int i = 0; i < first_node->size_next; i++)
  {
    if (markov_chain->comp_func
        (first_node->counter_list[i].markov_node->data, second_node->data)
        == MATCH)
    {
      ((first_node->counter_list) + i)->frequency++;
      return EXIT_SUCCESS;
    }
  }
  return EXIT_FAILURE;
}

Node *get_node_from_database (MarkovChain *markov_chain, void *data_ptr)
{// get the node with the data_ptr
  Node *cur = markov_chain->database->first;
  while (cur != NULL)
  {
    if (markov_chain->comp_func (cur->data->data, data_ptr) == MATCH)
    {
      return cur;
    }
    cur = cur->next;
  }
  return NULL;
}

Node *add_to_database (MarkovChain *markov_chain, void *data_ptr)
{
  Node *in_node = get_node_from_database (markov_chain, data_ptr);
  if (in_node != NULL)//check if the data is already in
  {
    return in_node;
  }
  void *data_insert = markov_chain->copy_func (data_ptr);
  if (data_insert == NULL)
  {
    free_markov_chain (&markov_chain);
    return NULL;
  }
  MarkovNode *new_node = malloc (sizeof (MarkovNode)); //create new markov
  if (new_node == NULL)
  {
    markov_chain->free_data (data_insert);
    data_insert = NULL;
    free_markov_chain (&markov_chain);
    return NULL;
  }
  new_node->data = data_insert;
  new_node->counter_list = NULL;
  new_node->size_next = START;
  new_node->total_next = START;
  if (add (markov_chain->database, new_node) == EXIT_FAILURE)
  {//create new node
    markov_chain->free_data (data_insert);
    free (new_node);
    new_node = NULL;
    free_markov_chain (&markov_chain);
    return NULL;
  }
  return markov_chain->database->last;
}

MarkovChain *create_markov_chain
    (free_data free_func, print_func print_func,
     comp_func comp_func, copy_func copy_func, is_last is_last_func)
{
  MarkovChain *markov_chain = malloc (sizeof (MarkovChain));
  if (markov_chain == NULL)
  {
    fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
    return NULL;
  }
  LinkedList *linked_list = malloc (sizeof (LinkedList));
  if (linked_list == NULL)
  {
    fprintf (stdout, ALLOCATION_ERROR_MASSAGE);
    free (markov_chain);
    return NULL;
  }
  markov_chain->database = linked_list;
  markov_chain->copy_func = copy_func;
  markov_chain->is_last = is_last_func;
  markov_chain->comp_func = comp_func;
  markov_chain->print_func = print_func;
  markov_chain->free_data = free_func;
  return markov_chain;
}


















