#include <cstdio>
#include <memory>
class A {
public:
    virtual void gao() = 0;
};
class B : public A {
public:
    B() { printf("cons\n"); }
    void gao() {printf ("gaoed\n");}
    ~B() { printf("des\n"); }
};

int main() {
    std::function<void()> storage[2];
    {
    std::shared_ptr<B> b= std::make_shared<B>();
    std::function<void()> f = std::bind(&B::gao, b);
    storage[0] = f;
    }
    storage[0]();
}
