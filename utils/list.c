#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static list_int *li_create(int val)
{
    list_int *el = malloc(sizeof(list_int));
    el->prev = malloc(sizeof(list_int *));
    el->next = malloc(sizeof(list_int *));
    el->prev = NULL;
    el->next = NULL;
    el->value = val;

    return el;
}

list *l_create()
{
    list *l = malloc(sizeof(list));

    l->head = malloc(sizeof(list_int *));
    l->tail = malloc(sizeof(list_int *));
    l->head = NULL;
    l->tail = NULL;

    return l;
}

void l_free(list *l)
{
    while (l->head != NULL)
    {
        list_int *el = l->head;

        l->head = l->head->next;

        free(el);
    }

    free(l->head);
    free(l);
}

void l_append(list *l, int val)
{
    list_int *el = li_create(val);

    if (l->tail == NULL && l->head == NULL)
    {
        // List is empty so we initialize it
        l->tail = el;
        l->head = el;
    }
    else
    {
        list_int *last = l->tail;

        el->next = NULL;
        el->prev = last;

        last->next = el;
        l->tail = el;
    }
}

list_int *l_find(list *l, int val)
{
    list_int *c = l->head;

    while (c != NULL || c->value != val)
        c = c->next;

    return c;
}

void l_remove(list *l, int val)
{
    list_int *c = l_find(l, val);

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

bool l_empty(list *l)
{
    return l->head == NULL && l->tail == NULL;
}

void l_pop(list *l)
{
    list_int *el = l->tail;
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