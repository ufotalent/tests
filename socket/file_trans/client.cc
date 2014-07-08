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
    signal(SIGPIPE, SIG_IGN);
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

    char fn[128];
    sprintf(fn, "%s_new", argv[1]);
    FILE *fp = fopen(argv[1], "rb");
    write(socket_fd, fn, 128);
    while (true) {
        char buf[255];
        int sz = fread(buf, 1, 255, fp);
        if (sz <= 0)
            return 0;
        int ret = write(socket_fd, buf, sz);
        if (ret != sz) {
            printf("partial write\n");
            return 1;
        }
        if (ret < 0) {
            perror("write failed");
            return 0;
        }
        printf("write %d bytes\n", ret);
    }
}
