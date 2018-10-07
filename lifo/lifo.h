#ifndef H_LIFO
#define H_LIFO

#include "sched.h"

typedef struct LIFO_Element Element;
struct LIFO_Element
{
    taskfunc function;
    void *args;

    Element *next;
};

typedef struct LIFO LIFO;
struct LIFO
{
    Element *first;
};

LIFO *lifo_alloc();
void push(LIFO *, taskfunc, void*);
Element *pop(LIFO *);

#endif
