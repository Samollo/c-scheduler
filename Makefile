stealing:
	gcc -pthread -o work_stealing deque/quicksort.c deque/sched.c deque/deque.c -std=c11 -D_POSIX_C_SOURCE=199309L

torture:
	gcc -pthread -o work_torture deque/torture.c deque/sched.c deque/deque.c -std=c11 -D_POSIX_C_SOURCE=199309L

lifo:
	gcc -pthread -o work_lifo lifo/quicksort.c lifo/sched.c lifo/lifo.c -std=c11 -D_POSIX_C_SOURCE=199309L

clean:
	rm -rf work_*
