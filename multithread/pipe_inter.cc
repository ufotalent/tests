#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <boost/thread.hpp>
int pipefd[2];
void thread_func() {
    while (1) {
        if (pipefd[1] != 0) {
            char b;
            write(pipefd[1], &b, 1);
        }
        sleep(1);
    }

}

void sig(int sig) {
    printf("received signal %d\n", sig);
}

int main() {
    signal(SIGINT, sig);
    boost::thread thr(thread_func);
    pipe(pipefd);
    int pollfd = epoll_create(255);
    epoll_event ev;
    ev.events = EPOLLET | EPOLLIN;
    ev.data.fd = pipefd[0];
    epoll_ctl(pollfd, EPOLL_CTL_ADD, pipefd[0], &ev);
    while (true) {
        epoll_event ev_r[128];
        int n = epoll_wait(pollfd, ev_r, 128, -1);
        printf("poll return: ");
        for (int i = 0; i < n; i++) {
            printf("%d ", ev_r[i].data.fd);
        }
        printf("\n");

    }
}
