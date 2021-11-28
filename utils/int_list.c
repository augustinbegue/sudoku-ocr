#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "int_list.h"

static int_list_node *lin_create(int val)
{
    int_list_node *el = malloc(sizeof(int_list_node));
    el->prev = malloc(sizeof(int_list_node *));
    el->next = malloc(sizeof(int_list_node *));
    el->prev = NULL;
    el->next = NULL;
    el->value = val;

    return el;
}

int_list *li_create()
{
    int_list *l = malloc(sizeof(int_list));

    l->head = malloc(sizeof(int_list_node *));
    l->tail = malloc(sizeof(int_list_node *));
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void li_free(int_list *l)
{
    while (l->head != NULL)
    {
        int_list_node *el = l->head;

        l->head = l->head->next;

        free(el);
    }

    free(l->head);
    free(l);
}

void li_append(int_list *l, int val)
{
    int_list_node *el = lin_create(val);

    if (l->tail == NULL && l->head == NULL)
    {
        // List is empty so we initialize it
        l->tail = el;
        l->head = el;
    }
    else
    {
        int_list_node *last = l->tail;

        el->next = NULL;
        el->prev = last;

        last->next = el;
        l->tail = el;
    }
}

int_list_node *l_find(int_list *l, int val)
{
    int_list_node *c = l->head;

    while (c != NULL || c->value != val)
        c = c->next;

    return c;
}

void li_remove(int_list *l, int val)
{
    int_list_node *c = l_find(l, val);

    if (c == l->tail)
    {
        l->tail = c->prev;
    }

    if (c == l->head)
    {
        l->head = c->next;
    }

    c->prev->next = c->next;
}

bool li_empty(int_list *l)
{
    return l->head == NULL && l->tail == NULL;
}

void li_pop(int_list *l)
{
    int_list_node *el = l->tail;
    if (l->tail == l->head)
    {
        l->head = NULL;
        l->tail = NULL;

        free(el->prev);
        free(el);

        return;
    }
    l->tail = l->tail->prev;
    free(el);
}

int li_size(int_list *l)
{
    int size = 0;

    int_list_node *e = l->head;
    while (e != NULL)
    {
        e = e->next;
        size++;
    }

    return size;
}

int **coord_lists_to_arr(int_list *edges_x, int_list *edges_y, int size)
{
    int_list_node *x_el = edges_x->head;
    int_list_node *y_el = edges_y->head;

    int **edges = malloc(sizeof(int *) * size + 1);
    for (int i = 0; i < size / 2; i++)
    {
        edges[i] = malloc(sizeof(int) * 4 + 1);

        edges[i][0] = x_el->value;
        edges[i][1] = y_el->value;

        x_el = x_el->next;
        y_el = y_el->next;

        edges[i][2] = x_el->value;
        edges[i][3] = y_el->value;

        x_el = x_el->next;
        y_el = y_el->next;
    }

    return edges;
}