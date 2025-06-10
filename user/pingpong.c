#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[])
{ 
    // create a pipe
    int p[2];
    char buf;

    if (pipe(p) < 0) {
        fprintf(2, "piped failed\n");
        exit(1);
    }

    if (fork() == 0) { // child
        if (read(p[0], &buf, 1) < 0) {
            fprintf(2, "child read failed\n");
        }
        printf("%d: received ping\n", getpid());
        write(p[1], "x", 1);
    } else { // parent
        write(p[1], "p", 1);
        if (read(p[0], &buf, 1) < 0) {
            fprintf(2, "parent read failed\n");
        }
        printf("%d: received pong\n", getpid());
    }

    return 0;
}