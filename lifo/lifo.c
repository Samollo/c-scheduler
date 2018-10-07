#include <stdio.h>
#include <stdlib.h>
#include "lifo.h"

LIFO *
lifo_alloc()
{
    LIFO *p = (LIFO *) malloc(sizeof(*p));
    if (p != NULL)
        p->first = NULL;
    return p;
}

void
push(LIFO *p, taskfunc f, void* args)
{
    Element *new = (Element *) malloc(sizeof(*new));
    if (new != NULL) {
        new->function = f;
        new->args = args;
        new->next = p->first;
        p->first = new;
    }
}

Element *
pop(LIFO *p)
{
    Element *new = p->first;
    if (new != NULL)
        p->first = new->next;
    else
        return NULL;

    return new;
}
