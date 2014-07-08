#include <boost/thread.hpp>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <pthread.h>
#include <cstdio>

int poll_fd = -1;
int pipefd[2];
std::set<int> read_fds;
pthread_mutex_t set_mutex = PTHREAD_MUTEX_INITIALIZER;
void do_poll() {
    epoll_event events[8];
    while (true) {
        if (poll_fd == -1)
            continue;
        int n = epoll_wait(poll_fd, events, 8, -1);
        for (int i = 0; i < n; i++) {
            int fd = events[i].data.fd;
            printf("polled fd %d mask %u\n", fd, events[i].events);
            pthread_mutex_lock(&set_mutex);
            if (!read_fds.count(fd))
                continue;
            pthread_mutex_unlock(&set_mutex);
            if (events[i].events & EPOLLIN) {
                char buf[128];
                int len;
                while ((len = recv(fd, buf, 128, ::MSG_DONTWAIT)) > 0) {
                    for (int s = 0; s < len; s++)
                        putchar(buf[s]);
                }
                printf("last read result %d\n", len);
            }
            if (events[i].events & EPOLLRDHUP) {
                printf("remote drop?\n");
                int len;
                char buf[128];
                len = recv(fd, buf, 128, MSG_DONTWAIT);
                printf("after drop %d\n", len);
            }
        }


    }
}


int main() {
    pipe(pipefd);
    poll_fd = epoll_create(128);
    boost::thread thread_poll(do_poll);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port =  htons(10086);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    printf("%d %x\n", socket_fd, addr.sin_addr.s_addr);
    if (bind(socket_fd, (sockaddr*)&addr, sizeof(addr))) {
        perror("bind failed");
        return 1;
    };

    if (listen(socket_fd, 128)) {
        perror("listen failed");
        return 1;
    }

    while (true) {
        sockaddr_in peer;
        socklen_t peer_len = sizeof(peer);
        int read_fd;
        if ((read_fd = accept4(socket_fd, (sockaddr*)&peer, &peer_len, SOCK_NONBLOCK)) == -1) {
            perror("accept failed");
            return 1;
        }
        char dst[20];
        inet_ntop(AF_INET, &peer.sin_addr, dst, sizeof(dst));
        printf("accepted from %s %d\n", dst, peer.sin_port);
        pthread_mutex_lock(&set_mutex);
        read_fds.insert(read_fd);
        pthread_mutex_unlock(&set_mutex);
        epoll_event e;
        e.events = EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLOUT;
        e.data.fd = read_fd;
        epoll_ctl(poll_fd, EPOLL_CTL_ADD, read_fd, &e);
    }


}
