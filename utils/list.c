#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static list_node *li_create(int val)
{
    list_node *el = malloc(sizeof(list_node));
    el->prev = malloc(sizeof(list_node *));
    el->next = malloc(sizeof(list_node *));
    el->prev = NULL;
    el->next = NULL;
    el->value = val;

    return el;
}

list *l_create()
{
    list *l = malloc(sizeof(list));

    l->head = malloc(sizeof(list_node *));
    l->tail = malloc(sizeof(list_node *));
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void l_free(list *l)
{
    while (l->head != NULL)
    {
        list_node *el = l->head;

        l->head = l->head->next;

        free(el);
    }

    free(l->head);
    free(l);
}

void l_append(list *l, int val)
{
    list_node *el = li_create(val);

    if (l->tail == NULL && l->head == NULL)
    {
        // List is empty so we initialize it
        l->tail = el;
        l->head = el;
    }
    else
    {
        list_node *last = l->tail;

        el->next = NULL;
        el->prev = last;

        last->next = el;
        l->tail = el;
    }
}

bool l_empty(list *l)
{
    return l->head == NULL && l->tail == NULL;
}

void l_pop(list *l)
{
    list_node *el = l->tail;
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

int l_size(list *l)
{
    int size = 0;

    list_node *e = l->head;
    while (e != NULL)
    {
        e = e->next;
        size++;
    }

    return size;
}