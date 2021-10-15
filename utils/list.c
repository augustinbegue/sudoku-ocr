#include <stdbool.h>
#include <stdio.h>
#include "list.h"

void l_append(list *l, int val)
{
    list_int el;
    el.value = val;

    if (l->tail == NULL)
    {
        l->tail = &el;
        l->head = &el;
        return;
    }

    l->tail->next = &el;
    el.prev = l->tail;
    l->tail = &el;
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
    if (l->tail == l->head)
    {
        l->head = NULL;
        l->tail = NULL;
        return;
    }

    l->tail = l->tail->prev;
}