#include <cstdio>
#include <memory>
class B : public std::enable_shared_from_this<B> {
public:
    B() { printf("cons\n"); }
    void gao() {printf ("gaoed\n");}
    ~B() { printf("des\n"); }
};

int main() {
    std::shared_ptr<B> x;
    {
        std::shared_ptr<B> b(new B());
        std::shared_ptr<B> bb = b->shared_from_this();
        x = bb;
    }
    x->gao();
}
