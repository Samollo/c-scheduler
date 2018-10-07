#include "deque.h"

static pthread_mutex_t mutex_sched = PTHREAD_MUTEX_INITIALIZER;

void
try_steal(struct scheduler *sched)
{
    Element *value;
    while (1) {
        int r = rand() % (sched->nthreads);
        value = pop_bottom(sched->deque_list[r]);

        if (value == NULL) {
            pthread_mutex_lock(&mutex_sched);
            if (sched->comp == 0) {
                pthread_mutex_unlock(&mutex_sched);
                pthread_exit(NULL);
            } else {
                pthread_mutex_unlock(&mutex_sched);
                sleep(0.01);
            }
        } else {
            pthread_mutex_lock(&mutex_sched);
            sched->comp++;
            pthread_mutex_unlock(&mutex_sched);

            value->function(value->args, sched);

            pthread_mutex_lock(&mutex_sched);
            sched->comp--;
            pthread_mutex_unlock(&mutex_sched);
            return;
        }
    }
}

void
task_alloc(void *s)
{
    Element *value;
    int i;
    struct scheduler *sched = (struct scheduler *) s;
    int verif = 0;

    while (verif != 1) {
        for (i = 0; i < sched->nthreads; i++) {
            if (pthread_self() == *(sched->deque_list[i]->owner)) {
                verif = 1;
                break;
            }
        }
    }

    while (1) {
        value = pop_top(sched->deque_list[i]);

        if (value == NULL) {
            pthread_mutex_lock(&mutex_sched);
            if (sched->comp == 0) {
                pthread_mutex_unlock(&mutex_sched);
                pthread_exit(NULL);
            } else {
                pthread_mutex_unlock(&mutex_sched);
                try_steal(sched);
            }
        } else {
            pthread_mutex_lock(&mutex_sched);
            sched->comp++;
            pthread_mutex_unlock(&mutex_sched);

            value->function(value->args, sched);

            pthread_mutex_lock(&mutex_sched);
            sched->comp--;
            pthread_mutex_unlock(&mutex_sched);
        }
    }
}

int
sched_init(int nthreads, int qlen, taskfunc f, void *closure)
{

    struct scheduler *s = (struct scheduler *) malloc(sizeof(struct scheduler));
    if (nthreads == -1) {
        s->nthreads = sched_default_threads();
    } else if(nthreads == 0){
      s->nthreads = 1;
    }else{
        s->nthreads = nthreads;
    }
    s->comp = 0;
    s->qlen = qlen;
    s->deque_list = (DEQUE **) malloc(sizeof(DEQUE *) * s->nthreads);
    if (s->deque_list == NULL)
        return -1;
    s->threads = (pthread_t *) malloc(sizeof(pthread_t) * s->nthreads);
    if (s->threads == NULL)
        return -1;

    for (int i = 0; i < s->nthreads; i++) {
        s->deque_list[i] = deque_alloc();
        if (s->deque_list[i] == NULL)
            return -1;
        s->deque_list[i]->owner = &(s->threads[i]);
    }

    push_top(s->deque_list[0], f, closure);

    for (int i = 0; i < s->nthreads; i++) {
        if (pthread_create(&(s->threads[i]), NULL, (void *(*)(void *)) task_alloc, (void *)s) !=  0)
            return -1;
    }
    for (int i = 0; i < s->nthreads; i++) {
        (void) pthread_join(s->threads[i], NULL);
    }

    free(s);
    return 1;
}

int
sched_spawn(taskfunc f, void *closure, struct scheduler *s)
{
    int i;
    int verif = 0;

    while (verif != 1) {
        for (i = 0; i < s->nthreads; i++) {
            if (pthread_self() == *(s->deque_list[i]->owner)) {
                verif = 1;
                break;
            }
        }
    }

    DEQUE *d = s->deque_list[i];
    push_top(d, f, closure);
    return 1;
}
