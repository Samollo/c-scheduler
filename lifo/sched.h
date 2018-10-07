#include <unistd.h>
#include <pthread.h>

typedef void (*taskfunc)(void*, struct scheduler *);
typedef struct LIFO LIFO;

struct scheduler
{
    LIFO *task_stack;
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
