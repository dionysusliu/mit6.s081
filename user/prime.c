// first process start from 2 to 35, it would also hold p=2

// the following process would only print one same number, p
//  if it receives a number N from upstream s.t. p doesn't divide N
//  fork a new process and set its p' = N

#include "kernel/types.h"
#include "user/user.h"

void emitter_proc();
void filter_proc(int, int);

void emitter_proc() {
    int div = 2;
    int have_child = 0;
    int child_pid = 0;
    int p[2];

    pipe(p); // create pipe
    printf("%d\n", div);
    
    for (int n = 2; n < 35; n++) {
        if (n % div == 0) continue; // n is not a prime
        
        if (!have_child) {
            if ((child_pid = fork()) == 0) { // create child process
                filter_proc(p[0], n);
            }
            have_child = 1;
        }

        write(p[1], &n, sizeof(n));
    }

    close(p[1]); // no more numbers to write, close the out_pipe fd
    wait((int *)0);
}

__attribute__((noreturn))
void filter_proc(int out_pipe, int div) {
    printf("%d\n", div);
    int p[2];
    int n; // number of bytes read from pipe
    int candidate; // number read from in_pipe
    int have_child = 0;
    int child_pid = 0;
    pipe(p);
    
    while((n = read(out_pipe, &candidate, sizeof(candidate))) != 0) {
        if (n < 0) {
            fprintf(2, "filter_proc: read error\n");
            exit(1);
        }

        if (candidate % div == 0) continue;

        // candidate is a prime
        if (!have_child) {
            if ((child_pid = fork()) == 0) {
                close(p[1]); // child process doesn't use this write portal
                filter_proc(p[0], candidate);
            }
            have_child = 1;
        }

        write(p[1], &candidate, sizeof(candidate));
    }

    close(out_pipe); // clean the in_pipe
    close(p[1]); // clean the out_pipe
    exit(0);
}

int
main(int argc, char *argv[])
{
    emitter_proc();
    return 0;
}