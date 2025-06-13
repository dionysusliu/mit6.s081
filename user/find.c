#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"

char*
fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // 

    // find first character after last slash
    for (p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    // return blank-padded name
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), ' ', DIRSIZ-strlen(p));
    return buf;
}

int name_matched(char *this, char *target) {
    return strcmp(fmtname(this), target) == 0;
}

void find_helper(char *target, char *curr_path) {
    // @target: name to be matched
    // @curr_path: current entry to be matched
    
}

void
find(char *target) {
    char buf[512], *p, curr_dir[512]=".";
    int fd;
    struct dirent de;
    struct stat st;

    // start from .
    if ((fd=open(curr_dir, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", curr_dir);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", curr_dir);
        close(fd);
        return;
    }

    
}