#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <cstdio>
#include <cstdlib>

using namespace boost::asio;
using namespace boost;


io_service service;
void call_run() {
    service.run();
    printf("run finished\n");
}

const int nt = 8;
const int nn = 10000000;
bool data[nn];
int main() {
    io_service::work* work = new io_service::work(service);
    thread* posttr[nt];
    thread* runtr[nt];
    for (int i = 0; i < nt; i++) {
        runtr[i] = new thread(call_run);
    }
    for (int i = 0; i < nt; i++) {
        posttr[i] = new thread([i]() {
            for (int j = 0; j < nn/nt; j++) {
                int offset = i + j * nt;
                service.post([offset](){data[offset] = true;});
            }
        });
    }
    printf("waiting for posts\n");
    for (int i = 0; i < nt; i++) {
        posttr[i]->join();
    }
    delete work;
    printf("waiting for runs\n");
    for (int i = 0; i < nt; i++) {
        runtr[i]->join();
    }
}
