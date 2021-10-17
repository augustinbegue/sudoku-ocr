#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdio.h>

typedef struct list_node list_node;
struct list_node
{
    list_node *prev;
    list_node *next;
    int value;
};

struct list
{
    list_node *head;
    list_node *tail;
};
typedef struct list list;

list *l_create();

void l_free(list *l);

void l_append(list *l, int val);

void l_pop(list *l);

bool l_empty(list *l);

void l_remove(list *l, int val);

int l_size(list *l);

#endif