#include <cstdio>
#include <boost/asio/coroutine.hpp>
#include <boost/asio/yield.hpp>

boost::asio::coroutine co;
boost::asio::coroutine *cloned_co;

void gao(boost::asio::coroutine &coro) {
    reenter (coro) {
        printf("1\n");
        yield;
        printf("2\n");
        fork {
            cloned_co = new boost::asio::coroutine(coro);
            printf("forked\n");
        }
        printf("after fork\n");
        
    }
}
int main() {
    printf("----------------\n");
    gao(co);
    printf("----------------\n");
    gao(co);
    printf("----------------\n");
    gao(*cloned_co);
}
