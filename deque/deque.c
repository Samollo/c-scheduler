#include <stdio.h>
#include "deque.h"

DEQUE*
deque_alloc()
{
    DEQUE *d = (DEQUE *)malloc(sizeof(*d));

    if (d != NULL) {
        pthread_mutex_init(&d->mutex, NULL);
        d->first = d->last = NULL;
    }
    return d;
}

void
push_top(DEQUE *d, taskfunc f, void *closure)
{
    Element *element = (Element *) malloc(sizeof(*element));
    element->function = f;
    element->args = closure;

    pthread_mutex_lock(&d->mutex);
    if (d->first == NULL) {
        element->next = NULL;
        d->first = element;
        d->last = element;
    } else {
        d->first->previous = element;
        element->next = d->first;
        element->previous = NULL;
        d->first = element;
    }
    pthread_mutex_unlock(&d->mutex);
}


Element*
pop_top(DEQUE *d)
{
    pthread_mutex_lock(&d->mutex);
    Element *first = d->first;

    if (first != NULL) {
        d->first = d->first->next;
        if (d->first != NULL)
            d->first->previous = NULL;
        else
            d->last = NULL;
        pthread_mutex_unlock(&d->mutex);
        return first;
    }
    pthread_mutex_unlock(&d->mutex);
    return NULL;
}


Element*
pop_bottom(DEQUE *d)
{
    if (pthread_mutex_trylock(&d->mutex) == 0) {
        Element *last = d->last;
        if (last != NULL) {
            d->last = d->last->previous;
            if (d->last != NULL)
                d->last->next = NULL;
            else
                d->first = NULL;
            pthread_mutex_unlock(&d->mutex);
            return last;
        }
        pthread_mutex_unlock(&d->mutex);
    }
    return NULL;
}
