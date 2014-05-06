#include <boost/thread.hpp>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>

int poll_fd = -1;
int pipefd[2];
std::set<int> read_fds;
struct fs {
    FILE *fp;
    std::vector<char> buf;
    size_t buf_head;
    int fd;
    fs() {
        fp = NULL;
        buf_head =0;
        buf.clear();
    }
};

pthread_mutex_t set_mutex = PTHREAD_MUTEX_INITIALIZER;
void do_poll() {
    epoll_event events[8];
    while (true) {
        if (poll_fd == -1)
            continue;
        int n = epoll_wait(poll_fd, events, 8, -1);
        for (int i = 0; i < n; i++) {
            fs* cfs = (fs*)events[i].data.ptr;
            int fd = cfs->fd;
            pthread_mutex_lock(&set_mutex);
            if (!read_fds.count(fd))
                continue;
            pthread_mutex_unlock(&set_mutex);
            if (events[i].events & EPOLLIN) {
                char buf[128];
                int len;
                while ((len = read(fd, buf, 128)) > 0) {
                    cfs->buf.insert(cfs->buf.end(), buf, buf + len);
                }
                if (cfs->fp == NULL && cfs->buf.size() >= 128) {
                    char *fn = &cfs->buf[0];
                    cfs->fp = fopen(fn, "wb");
                    cfs->buf_head = 128;
                    printf("file name %s\n", fn);
                }
                if (cfs->buf_head < cfs->buf.size()) {
                    size_t nn = cfs->buf.size() - cfs->buf_head;
                    fwrite(&cfs->buf[cfs->buf_head], 1, nn, cfs->fp);
                    cfs->buf_head += nn;
                }
            }
            if (events[i].events & EPOLLRDHUP) {
                fflush(cfs->fp);
                printf("remote drop\n");
            }
        }


    }
}


int main() {
    poll_fd = epoll_create(128);

    char b;
    pipe(pipefd);
    write(pipefd[1], &b, 1);
    read(pipefd[0], &b, 1);

    epoll_event pe;
    pe.events = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLET;
    pe.data.fd = pipefd[0];
    if (epoll_ctl(poll_fd, EPOLL_CTL_ADD, pipefd[0], &pe)) {
        perror("epoll_ctl:");
    }

    boost::thread thread_poll(do_poll);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port =  htons(10086);
    inet_pton(AF_INET, "10.76.0.129", &addr.sin_addr);
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
        e.events = EPOLLIN | EPOLLRDHUP | EPOLLET;

        fs *cfs = new fs();
        cfs->fd = read_fd;
        e.data.ptr = cfs;
        epoll_ctl(poll_fd, EPOLL_CTL_ADD, read_fd, &e);
    }


}
