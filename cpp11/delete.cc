class A {
public:
    A() {}
    A& operator= (const A&) = delete;
};

class B : public A {

};
int main() {
    A a;
    A aa;
    B b;
    B bb;
    
    bb=b;

}
