.PHONY: clean tweets snake

CC = gcc

CCFLAGS = -g -Wall -Wextra -Wvla -Werror -std=c99

OBJECT_MAIN = markov_chain.o linked_list.o

OBJECT_MARKOV_CHAIN = markov_chain.c markov_chain.h linked_list.h

LINKED_LIST_OBJECT = linked_list.c linked_list.h

OBJECT_MAIN_C = markov_chain.h linked_list.h

tweets:tweets_generator.o $(OBJECT_MAIN)
	$(CC) $^ -o tweets_generator

snake:snakes_and_ladders.o $(OBJECT_MAIN)
	$(CC) $^ -o snakes_and_ladders

tweets_generator.o:tweets_generator.c $(OBJECT_MAIN_C)
	$(CC) $(CCFLAGS) -c $<

snakes_and_ladders.o:snakes_and_ladders.c $(OBJECT_MAIN_C)
	$(CC) $(CCFLAGS) -c $<

markov_chain.o:$(OBJECT_MARKOV_CHAIN)
	$(CC) $(CCFLAGS) -c $<

linked_list.o:$(LINKED_LIST_OBJECT)
	$(CC) $(CCFLAGS) -c $<

clean:
	rm *.o
