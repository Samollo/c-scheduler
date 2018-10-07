#include <unistd.h>
#include <pthread.h>

typedef void (*taskfunc)(void*, struct scheduler *);

typedef struct DEQUE DEQUE;


struct scheduler
{
    DEQUE **deque_list;
    int qlen;
    int nthreads;
    pthread_t *threads;
    int comp;
};

static inline int
sched_default_threads()
{
    return sysconf(_SC_NPROCESSORS_ONLN);
}

int sched_init(int nthreads, int qlen, taskfunc f, void *closure);
int sched_spawn(taskfunc f, void *closure, struct scheduler *s);
void try_steal(struct scheduler *s);
