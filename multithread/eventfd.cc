#include <boost/thread.hpp>
#include <sys/eventfd.h>
#include <cstdio>
#include <cstdlib>

using namespace boost;
int efd;
void read_t() {
    uint64_t buf;
    int s = read(efd, &buf, sizeof(buf));
    if (s!=0)
        printf("%lu\n", buf);
}
int main() {
    efd = eventfd(0, 0);
    thread thread1(read_t);
    thread thread2(read_t);
    thread thread3(read_t);
    sleep(4);
    uint64_t s = 2;
    write(efd, &s, sizeof(uint64_t));
    thread1.join();
    thread2.join();
    thread3.join();
}
