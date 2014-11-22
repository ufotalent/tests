#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <cstdio>
#include <assert.h>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <errno.h>

int poll_fd;
void do_poll() {
    while (true) {

        break;
    }
}

int get_status(int fd) {
    int opt;
    socklen_t opt_len = sizeof(int);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &opt, &opt_len) != 0) {
        perror("getsockopt failed");
        return -1;
    }
    return opt;
}
int main(int argc, char* argv[]) {
    poll_fd = epoll_create(128);
    printf("created pollfd %d\n", poll_fd);

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    printf("created socket fd %d\n", socket_fd);
    int flags = fcntl(socket_fd, F_GETFL);
    assert(flags >= 0);
    int r1 = fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
    assert(r1 == 0);


    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    
    /*
    addr.sin_port =  htons(8080);
    inet_pton(AF_INET, "115.239.210.27", &addr.sin_addr);
    */
    
    addr.sin_port =  htons(10086);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    

    printf("%d %x\n", socket_fd, addr.sin_addr.s_addr);

    epoll_event e;
    e.events = EPOLLOUT | EPOLLET | EPOLLIN;
    e.data.fd = socket_fd;
    epoll_ctl(poll_fd, EPOLL_CTL_ADD, socket_fd, &e);

    if (connect(socket_fd, (sockaddr*)&addr, sizeof(addr))) {
        if (errno != EINPROGRESS) {
            perror("connect failed");
            return 1;
        }
    };

    int opt = get_status(socket_fd);
    printf("error status: %d\n", opt);
    opt = get_status(socket_fd);
    printf("error status: %d\n", opt);

    char peer_ip[20];
    sockaddr_in peer;
    socklen_t peer_len = sizeof(peer);
    int c = getpeername(socket_fd, (sockaddr*)&peer, &peer_len);
    inet_ntop(AF_INET, &peer.sin_addr, peer_ip, sizeof(peer_ip));
    printf("code %d, peer %s %d\n", c, peer_ip, peer.sin_port);

    sleep(10);
    printf("pollfd %d\n", poll_fd);
    while (true)  {
        epoll_event events[256];
        //printf("will poll\n");
        int cnt = epoll_wait(poll_fd, events, 256, -1);
        //printf("res %d\n", cnt);
        //if (cnt < 0)
        //    perror("poll failed");
        for (int i = 0; i < cnt; i++) {
            int fd = events[i].data.fd;
            printf("polled fd %d\n", fd);
            if (fd == socket_fd && (events[i].events & EPOLLOUT)) {
                int opt = get_status(socket_fd);
                printf("error status: %d\n", opt);
                if (opt == 0) {
                    printf("connected\n");
                    char dst[20];
                    sockaddr_in me;
                    me.sin_port = 0;
                    socklen_t me_len = sizeof(me);
                    getsockname(socket_fd, (sockaddr*)&me, &me_len);
                    inet_ntop(AF_INET, &me.sin_addr, dst, sizeof(dst));
                    printf("me %s %d\n", dst, me.sin_port);

                    char peer_ip[20];
                    sockaddr_in peer;
                    socklen_t peer_len = sizeof(peer);
                    int c = getpeername(socket_fd, (sockaddr*)&peer, &peer_len);
                    inet_ntop(AF_INET, &peer.sin_addr, peer_ip, sizeof(peer_ip));
                    printf("code %d, peer %s %d\n", c, peer_ip, peer.sin_port);

                    int cnt = 0;
                    while (true) {
                        char buf[255];
                        sprintf(buf, "message %s:%d\n", argv[1], cnt++);
                        int sz = send(socket_fd, buf, strlen(buf)+1, ::MSG_NOSIGNAL);
                        if (sz == 0) {
                            printf("write return 0\n");
                            return 1;
                        }
                        if (sz < 0) {
                            perror("write failed");
                            int opt = get_status(socket_fd);
                            printf("error status: %d\n", opt);
                            char peer_ip[20];
                            sockaddr_in peer;
                            socklen_t peer_len = sizeof(peer);
                            int c = getpeername(socket_fd, (sockaddr*)&peer, &peer_len);
                            inet_ntop(AF_INET, &peer.sin_addr, peer_ip, sizeof(peer_ip));
                            printf("code %d, peer %s %d\n", c, peer_ip, peer.sin_port);

                            return 0;
                        }
                        printf("write %d bytes\n", sz);
                        sleep(1);
                    }
                } else {
                    printf("connection failed\n");
                }
            }
        }

    }


}
