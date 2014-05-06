#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include <atomic>
const int tar = 100000000;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_add = PTHREAD_MUTEX_INITIALIZER;
std::atomic_int count;
volatile int s = 0;
void* thread(void *args) {
    while (true) {
        //        printf("thread %ld entering\n", (long)args);
        pthread_mutex_lock(&mutex);
        //        printf("thread %ld will wait\n", (long)args);
        while (count <=0) {
            pthread_cond_wait(&cond, &mutex);
        }

        count--;
        s++;
        if (s==tar) {
            printf("result reached\n");
        }
        //        printf("thread %ld wait finish\n", (long)args);
        pthread_mutex_unlock(&mutex);
        //        printf("thread %ld exiting\n", (long)args);
        
    }
    return NULL;
}
int main() {
    count.store(0);
    pthread_t t[10];
    for (int i = 0; i < 10; i++) {
        pthread_create(&t[i], NULL, &thread, (void*)i);
    }
    for (int i = 0; i < tar; i++) {
        count++;
        pthread_cond_signal(&cond);
    }

    printf("%d\n",s);
    for (int i = 0; i < 10; i++) {
        pthread_join(t[i], NULL);
    }
}
