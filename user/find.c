#include "kernel/types.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"

char*
fmtname(char *path) {
    static char buf[DIRSIZ+1];
    char *p;

    // 

    // find first character after last slash
    for (p=path+strlen(path); p >= path && *p != '/'; p--)
        ;
    p++;
    // return zero-padded name
    if (strlen(p) >= DIRSIZ)
        return p;
    memmove(buf, p, strlen(p));
    memset(buf+strlen(p), '\0', DIRSIZ-strlen(p));
    return buf;
}

int name_matched(char *this, const char *target) {
    // printf("        formatted name is %s\n", fmtname(this));
    // printf("        target  is %s\n", target);  
    // printf(" compare result is %d\n", strcmp(fmtname(this), target));
    return strcmp(fmtname(this), target) == 0;
}

void find_helper(const char *target, char *curr_path) {
    // @target: name to be matched
    // @curr_path: current entry to be matched

    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    // printf("@@ current path: %s\n", curr_path);

    if ((fd=open(curr_path, O_RDONLY)) < 0) {
        fprintf(2, "find: cannot open %s\n", curr_path);
        return;
    }

    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", curr_path);
        close(fd);
        return;
    }

    switch(st.type) {
    case T_FILE:
    case T_DEVICE:
        if (name_matched(curr_path, target)) {
            printf("%s\n", curr_path);
        }
        break;
    case T_DIR: 
        // match this entry
        if (name_matched(curr_path, target)) {
            printf("%s\n", curr_path);
        }

        // match sub-entries recursively
        strcpy(buf, curr_path);
        p = buf + strlen(buf); // pos next to the last char of string
        *p++ = '/';

        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if (de.inum == 0) 
                continue;
            if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            // entry length check
            if (strlen(curr_path) + 1 + strlen(de.name) + 1 > sizeof buf) {
                printf("find: path too long\n");
                continue;
            }
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            find_helper(target, buf);
        }
        break;
    }

    close(fd);
}

void
find(char *curr_path , char *target) {
    find_helper(target, curr_path);
    // printf("exit find helper\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        // printf("Usage: find <target>");
        exit(1);
    }

    char *curr_dir, *target;
    curr_dir = argv[1];
    target = argv[2];
    find(curr_dir, target);    
    // printf("exit find\n");
    exit(0);
} 