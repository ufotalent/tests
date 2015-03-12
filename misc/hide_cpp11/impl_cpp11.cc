#include "def_cpp11.h"
#include <memory>

int cons_count;
int des_count;
class Test {
public:
    Test() {
        cons_count++;
    }
    Test(const Test&) {
        cons_count++;
    }
    virtual ~Test() {
        des_count++;
    }
};
int calc(int base) {
    std::shared_ptr<Test> ptr(new Test());
    ptr.reset();
    return cons_count + base;
}
