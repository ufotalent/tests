#include <boost/thread.hpp>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <signal.h>
#include <cstdio>

int poll_fd;
void do_poll() {
    while (true) {
        
        break;
    }
}

int main(int argc, char* argv[]) {
    poll_fd = epoll_create(0);
    boost::thread thread_poll(do_poll);
   
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port =  htons(10086);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);
    printf("%d %x\n", socket_fd, addr.sin_addr.s_addr);
    if (connect(socket_fd, (sockaddr*)&addr, sizeof(addr))) {
        perror("connect failed");
        return 1;
    };
    printf("connected\n");

    char dst[20];

    sockaddr_in me;
    me.sin_port = 0;
    socklen_t me_len = sizeof(me);
    getsockname(socket_fd, (sockaddr*)&me, &me_len);
    inet_ntop(AF_INET, &me.sin_addr, dst, sizeof(dst));
    printf("me %s %d\n", dst, me.sin_port);

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
            return 0;
        }
        printf("write %d bytes\n", sz);
        sleep(1);
    }
}
