#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "list.h"

static list_node *li_create(void *val)
{
    list_node *el = malloc(sizeof(list_node));
    el->prev = malloc(sizeof(list_node *));
    el->next = malloc(sizeof(list_node *));
    el->prev = NULL;
    el->next = NULL;
    el->value = val;

    return el;
}

/**
 * @brief creates and returns an empty list stored in the heap
 *
 * @return list*
 */
list *l_create()
{
    list *l = malloc(sizeof(list));

    l->head = malloc(sizeof(list_node *));
    l->tail = malloc(sizeof(list_node *));
    l->head = NULL;
    l->tail = NULL;

    return l;
}

/**
 * @brief frees l and the pointers contained
 *
 * @param l
 */
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

void l_free_values(list *l)
{
    list_node *el = l->head;
    while (el != NULL)
    {
        free(el->value);
        el = el->next;
    }

    l_free(l);
}

/**
 * @brief apprends val to l
 *
 * @param l
 * @param val
 */
void l_append(list *l, void *val)
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

/**
 * @brief returns true if l is empty
 *
 * @param l
 * @return true
 * @return false
 */
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

/**
 * @brief Returns the size of l
 *
 * @param l
 * @return int
 */
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

/**
 * @brief Merges l2 into l1
 *
 * @param l1
 * @param l2
 */
void l_merge(list *l1, list *l2)
{
    list_node *el = l2->head;

    while (el != NULL)
    {
        l_append(l1, el->value);
        el = el->next;
    }

    // if (l_empty(l1))
    // {
    //     l1->head = l2->head;
    //     l1->tail = l2->tail;
    //     return;
    // }

    // if (l_empty(l2))
    // {
    //     return;
    // }

    // l1->tail->next = l2->head;
    // l1->tail = l2->tail;
}