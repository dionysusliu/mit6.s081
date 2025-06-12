#include "kernel/types.h"
#include "user/user.h"

int main() {
    close(1);
    close(2);
    int p[2];
    pipe(p);
    printf("p[0] = %d, p[1] = %d\n", p[0], p[1]);

    return 0;
}