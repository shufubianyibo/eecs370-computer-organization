#include "stddef.h"
#include "stdio.h"

int pairty(int input) {
    int count = 0;
    int a = 1;
    for (size_t i = 0; i < sizeof(input)*8; i++){
        if ((input & a) != 0) {
            count++;
        }
        a *= 2;
    }
    return count % 2;
}

int main() {
    printf("%d\n",pairty(0));
    printf("%d\n",pairty(15));
    printf("%d\n",pairty(2));
    printf("%d\n",pairty(-1));
    printf("%d\n",pairty(-2));
    return 0;
}
