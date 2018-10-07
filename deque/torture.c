#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <stdatomic.h>

#include "sched.h"

atomic_int counter;

void
tree_fn(void *closure, struct scheduler *s)
{
    int rc;
    long a = (long)closure;
    if(a > 20)
        return;
    atomic_fetch_add(&counter, 1);
    rc = sched_spawn(tree_fn, (void*)(a + 1), s);
    if(rc < 0) {
        perror("sched_spawn");
        return;
    }
    tree_fn((void*)(a + 1), s);
}

int
tree(int nthreads)
{
    printf("nthreads : %d\n", nthreads);
    int rc;
    rc = sched_init(nthreads, 1024 * 1024, tree_fn, (void*)0L);
    if(rc < 0)
        perror("sched_init");
    return rc;
}

void
linear_fn(void *closure, struct scheduler *s)
{
    int rc;
    long a = (long)closure;
    if(a >= 1024 * 1024)
        return;
    atomic_fetch_add(&counter, 1);
    rc = sched_spawn(linear_fn, (void*)(a + 1), s);
    if(rc < 0) {
        perror("sched_spawn");
        return;
    }
}

int
linear(int nthreads)
{
    int rc;
    rc = sched_init(nthreads, 1024 * 1024, linear_fn, (void*)0);
    if(rc < 0)
        perror("sched_init");
    return rc;
}

void
overflow_fn(void *closure, struct scheduler *s)
{
    int rc;
    long a = (long)closure;
    struct timespec tv = {0, 100 * 1000 * 1000};
    if(a > 200)
        return;
    rc = sched_spawn(tree_fn, (void*)(a + 1), s);
    if(rc < 0) {
        perror("sched_spawn");
        return;
    }
    rc = sched_spawn(tree_fn, (void*)(a + 1), s);
    if(rc < 0) {
        perror("sched_spawn");
        return;
    }
    rc = nanosleep(&tv, NULL);
    if(rc < 0) {
        perror("nanosleep");
        return;
    }
    atomic_fetch_add(&counter, 1);
}

int
overflow(int nthreads)
{
    int rc;
    rc = sched_init(nthreads, 10, overflow_fn, (void*)0);
    if(rc < 0)
        perror("sched_init");
    return rc;
}

static double
timespec_minus(const struct timespec *t1, const struct timespec *t2)
{
    return t1->tv_sec - t2->tv_sec + (t1->tv_nsec - t2->tv_nsec) / 1000000000.0;
}

int
main()
{
    struct timespec start, end;
    for(int i = 0; i < 16; i++) {
        printf("Tree %d...\n", i);        
        atomic_store(&counter, 0);
        clock_gettime(CLOCK_MONOTONIC, &start);;
        tree(i);
        clock_gettime(CLOCK_MONOTONIC, &end);
        printf("...%d %lfs\n",
               atomic_load(&counter), timespec_minus(&end, &start));
    }
    for(int i = 0; i < 16; i++) {
        printf("Linear %d...\n", i);
        atomic_store(&counter, 0);
        clock_gettime(CLOCK_MONOTONIC, &start);
        linear(i);
        clock_gettime(CLOCK_MONOTONIC, &end);
        printf("...%d %lfs\n",
               atomic_load(&counter), timespec_minus(&end, &start));
    }
    for(int i = 0; i < 16; i++) {
        printf("Overflow %d...\n", i);
        atomic_store(&counter, 0);
        clock_gettime(CLOCK_MONOTONIC, &start);
        linear(i);
        clock_gettime(CLOCK_MONOTONIC, &end);
        printf("...%d %lfs\n",
               atomic_load(&counter), timespec_minus(&end, &start));
    }
    return 0;
}
