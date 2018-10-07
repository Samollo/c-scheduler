#ifndef DEQUE_H
#define DEQUE_H

#include <stdlib.h>
#include <stdio.h>
#include "sched.h"

typedef struct Element Element;
struct Element
{
    taskfunc function;
    void *args;
    Element *next;
    Element *previous;
};


typedef struct DEQUE DEQUE;
struct DEQUE
{
    Element *first;
    Element *last;
    pthread_t *owner;
    pthread_mutex_t mutex;
};

DEQUE* deque_alloc();
Element* pop_top(DEQUE *d); // Pop en haut de la DEQUE par le thread propriétaire
void push_top(DEQUE *d, taskfunc, void*); // Push en haut de la DEQUE par le thread propriétaire
Element* pop_bottom(DEQUE *d); // Pop en bas de la DEQUE par le thread voleur

#endif
