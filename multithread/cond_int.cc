#include <pthread.h>
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <atomic>
#include <execinfo.h>
const int tar = 100000000;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_add = PTHREAD_MUTEX_INITIALIZER;
std::atomic_int count;
void hander(int signum) {
    printf("%d\n", signum);
    void *array[100];
    size_t size;

    size = backtrace(array, 100);
    backtrace_symbols_fd(array, size, STDERR_FILENO);
}
volatile int s = 0;
void* thread(void *args) {
    sleep(10);
    pthread_cond_signal(&cond);
    sleep(1000);
    return NULL;
}
int main() {
    signal(SIGUSR1, hander);
    count.store(0);
    pthread_t t[10];
    for (int i = 0; i < 1; i++) {
        pthread_create(&t[i], NULL, &thread, (void*)i);
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond, &mutex);
    pthread_mutex_unlock(&mutex);
    printf("%d\n",s);
    for (int i = 0; i < 1; i++) {
        pthread_join(t[i], NULL);
    }
}
