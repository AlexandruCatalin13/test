#ifndef __THREAD_WORK_H__
#define __THREAD_WORK_H__

void *first_thread_work(void *);
void *second_thread_work(void *);
void *third_thread_work(void *);

pthread_mutex_t list_mutex;
pthread_barrier_t barrier;

#endif
