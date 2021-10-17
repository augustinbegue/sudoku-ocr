#ifndef LIST_INT_H
#define LIST_INT_H

#include <stdbool.h>
#include <stdio.h>

typedef struct int_list_node int_list_node;
struct int_list_node
{
    int_list_node *prev;
    int_list_node *next;
    int value;
};

struct int_list
{
    int_list_node *head;
    int_list_node *tail;
};
typedef struct int_list int_list;

int_list *li_create();

void li_free(int_list *l);

void li_append(int_list *l, int val);

int_list_node *l_find(int_list *l, int val);

void li_remove(int_list *l, int val);

void li_pop(int_list *l);

bool li_empty(int_list *l);

void li_remove(int_list *l, int val);

int li_size(int_list *l);

int **coord_lists_to_arr(int_list *edges_x, int_list *edges_y, int size);

#endif