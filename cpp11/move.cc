#include <cstdio>
#include <vector>
template <typename T>
class C {
private:
    std::vector<T> data_;
public:
    void push(T&& s) {
        printf("before push_back\n");
        data_.push_back(s);
    }
    void pop(T& s) {
        s = std::move(data_[0]);
        data_.erase(data_.begin());
    }
};

class B {
public:
    int s;
    B(int t) {
        s = t;
        printf("cons\n");
    }
    B(const B& b) {
        s = b.s;
        printf("copy cons\n");
    }
    
    B& operator=(B&& b) {
        s = b.s;
        printf("move assign\n");
        return *this;
    }
    B(B&& b) {
        s = b.s;
        printf("move cons\n");
    }
    
    B& operator=(B& b) {
        s = b.s;
        printf("assign\n");
        return *this;
    }
    ~B() {
        printf("des\n");
    }
};

template<typename T>
void gao(const T& t) {
    printf("gao&\n");
}
template<typename T>
void gao(T&& t) {
    T s = t;
    printf("gao&& %d\n", s.s);
}
int main() {
    /*
    C<B> c;
    B a;
    c.push(std::move(a));
    printf("push done\n");
    B b;
    c.pop(b);
    */
    std::vector<B> v;
    B b1(1), b2(2);
    printf("init\n");
    B&& & x = std::move(b1);
    v.push_back(std::forward<B>(x));
    v.push_back(std::move(b2));

    printf("will gao\n");
    gao(x);
    printf("%d %d\n", v[0].s, v[1].s);
}
