#include <stdio.h>
#include <stdlib.h>

void main() {
    unsigned long long p1 = 248693223;
    unsigned long long p2 = 248693221;

    unsigned long long i = llabs(p1*p2);

    printf("%llu\n", i);
}