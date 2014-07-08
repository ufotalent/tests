#include <stdlib.h>
int main() {
    int *p = new int[4];

    p[4] = 2;

    delete []p;
}
