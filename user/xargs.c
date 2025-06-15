#include "kernel/types.h"
#include "kernel/param.h"
#include "user/user.h"

// #define DEBUG

#ifdef DEBUG
#define debug_printf(format, ...) printf("[DEBUG] " format, ##__VA_ARGS__)
#else
#define debug_printf(format, ...)
#endif

void xargs_exec_one_line(char *buf, int buf_pos, char *cmd, char *cmd_args[]) {
    buf[buf_pos] = '\0';
    // create child process to exec the new command
    int pid = fork();

    if (pid < 0) {
        fprintf(2, "xargs: fork failed\n");
        exit(1);
    } else if (pid == 0) {
        // prepare argv for the new exec
        char *new_argv[MAXARG+1];
        new_argv[0] = cmd;
        debug_printf("new command to execute: %s\n", cmd);
        int i;
        for (i = 0; i < MAXARG && cmd_args[i] != 0; i++) { // args from xargs
            debug_printf("cmd_args[%d] = %s\n", i, cmd_args[i]);
            new_argv[i+1] = cmd_args[i];
        }
        new_argv[i+1] = buf; // args from stdin
        debug_printf("cmd_args[%d] = %s\n", i+1, cmd_args[i+1]);
        new_argv[i+2] = 0; // last args is 0

        exec(cmd, new_argv);

        // exec failed
        fprintf(2, "xargs: exec %s failed\n", cmd);
        exit(1);
    } else {
        wait((int*)0); // wait for child process
    }
}

int
main(int argc, char  *argv[]) {
    if (argc < 2) {
        fprintf(2, "Usage: xargs command [arguments...]");
        exit(1);
    }

    debug_printf("commands received by xargs runtime:\n");
    for (int i = 0; i < argc; i++) {
        debug_printf("(%d) %s\n", i, argv[i]);
    }

    char *cmd = argv[1];
    char *cmd_args[MAXARG];

    // args provided to xargs
    for (int i = 0; i < MAXARG && i < argc-2; i++) {
        cmd_args[i] = argv[i+2];
    }
    cmd_args[MAXARG-1] = 0; // set last arg as NULL

    // readline buffer
    char buf[512];
    int buf_pos = 0;

    while(read(0, buf+buf_pos, 1) > 0) {
        if (!(buf[buf_pos] == '\n' || buf_pos >= sizeof(buf) - 1)) { // keep reading the line from stdin
            buf_pos++;
            continue;
        }
        debug_printf("line collected from stdin: %s\n", buf);
        xargs_exec_one_line(buf, buf_pos, cmd, cmd_args);
        buf_pos = 0; // reset the line buffer
    }

    // if line buffer isn't empty (not ending with \n)
    if (buf_pos > 0) {
        xargs_exec_one_line(buf, buf_pos, cmd, cmd_args);
    }

    exit(0);
}