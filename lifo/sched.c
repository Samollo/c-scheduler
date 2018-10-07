#include "lifo.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void
task_alloc(void* s)
{
    Element *value;
    int comp;
    struct scheduler *sched = (struct scheduler *)s;

    while (1) {
        pthread_mutex_lock(&mutex);
        value = pop(sched->task_stack);
        comp = sched->comp;
        pthread_mutex_unlock(&mutex);

        if (value == NULL) {
            if(comp == 0) {
                pthread_cond_broadcast(&cond);
                pthread_exit(NULL);
            }
            pthread_mutex_lock(&mutex);
            pthread_cond_wait (&cond, &mutex);
            pthread_mutex_unlock(&mutex);

        }
        else {
            pthread_mutex_lock(&mutex);
            sched->comp++;
            pthread_mutex_unlock(&mutex);

            value->function (value->args, sched);

            pthread_mutex_lock(&mutex);
            sched->comp--;
            pthread_mutex_unlock(&mutex);
        }
    }
}

int
sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{
    struct scheduler *s = (struct scheduler *) malloc(sizeof(struct scheduler));
    if (s == NULL)
        return -1;

    if (nthreads == -1)
        s->nthreads = sched_default_threads();
    else
        s->nthreads = nthreads;

    s->comp = 0;
    s->qlen = qlen;
    s->task_stack = lifo_alloc();
    if (s->task_stack == NULL)
        return -1;

    push(s->task_stack, f, closure);

    s->threads = (pthread_t *) malloc(sizeof(pthread_t) * s->nthreads);
    if (s->threads == NULL)
        return -1;

    pthread_mutex_lock(&mutex);

    for (int i = 0; i < s->nthreads; i++) {
        if (pthread_create (&(s->threads[i]), NULL, (void *(*)(void *)) task_alloc, (void *)s) != 0)
            return -1;
    }

    pthread_mutex_unlock(&mutex);

    for (int i = 0; i < s->nthreads; i++) {
        (void) pthread_join(s->threads[i], NULL);
    }

    free(s);

    return 1;
}

int sched_spawn(taskfunc f, void *closure, struct scheduler *s) {

    pthread_mutex_lock(&mutex);
    push(s->task_stack, f, closure);
    pthread_mutex_unlock(&mutex);
    pthread_cond_broadcast(&cond);

    return 1;
}
