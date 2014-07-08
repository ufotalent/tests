#include <boost/thread.hpp>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <pthread.h>
#include <cstdio>

int poll_fd = -1;
int pipefd[2];
int main() {
    pipe(pipefd);
    poll_fd = epoll_create(128);
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int flags = fcntl(socket_fd, F_GETFL);
    assert(flags >= 0);
    int r1 = fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    assert(r1 == 0);
       

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

    epoll_event e;
    e.events = EPOLLIN | EPOLLET;
    e.data.fd = socket_fd;
    epoll_ctl(poll_fd, EPOLL_CTL_ADD, socket_fd, &e);

    while (true) {
        epoll_event events[256];
        int cnt = epoll_wait(poll_fd, events, 256, -1);
        for (int i = 0; i < cnt; i++) {
            int fd = events[i].data.fd;
            printf("polled fd %d\n", fd);
            if (fd == socket_fd) {
                sockaddr_in peer;
                socklen_t peer_len = sizeof(peer);
                int read_fd;
                if ((read_fd = accept4(socket_fd, (sockaddr*)&peer, &peer_len, 0)) == -1) {
                    perror("accept failed");
                    continue;
                }
                char dst[20];
                inet_ntop(AF_INET, &peer.sin_addr, dst, sizeof(dst));
                printf("accepted from %s %d\n", dst, peer.sin_port);

                char buf[256];
                int n;
                while ((n = read(read_fd, buf, 256)) > 0) {
                    printf("received %d bytes\n", n);
                    for (int i = 0; i < n; i++) {
                        putchar(buf[i]);
                    }
                }
                exit(0);
            }
        }
    }


}
