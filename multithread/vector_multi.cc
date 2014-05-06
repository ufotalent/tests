#include <boost/thread.hpp>
#include <cstdio>
std::vector<char> data;
//bool data[1000000];
void gao(int i) {
    for (int s = 0; s < 100000; s++) 
        data[s * 10 + i] = true;
}

int main() {
    data.resize(1000000);
    boost::thread *threads[10];
    for (int i = 0; i < 10; i++) {
        threads[i] = new boost::thread(gao, i);
    }
    for (int i = 0; i < 10; i++) {
        threads[i]->join();
    }
    for (int i = 0; i < 1000000; i++) {
        assert(data[i] == true);
    }
}
