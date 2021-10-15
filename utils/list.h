#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdio.h>

typedef struct list_int list_int;
struct list_int
{
    list_int *prev;
    list_int *next;
    int value;
};

struct list
{
    list_int *head;
    list_int *tail;
};
typedef struct list list;

void l_append(list *l, int val);

list_int *l_find(list *l, int val);

void l_remove(list *l, int val);

void l_pop(list *l);

bool l_empty(list *l);

void l_remove(list *l, int val);

#endif