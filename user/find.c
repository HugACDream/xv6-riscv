#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

void find(char* path, char* tarname) {
    int fd;
    struct stat st;
    struct dirent de;
    char buf[512], *p;
    
    if((fd = open(path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", path);
        exit(1);
    }

    if(fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        exit(1);
    }

    if(st.type == T_FILE) {
        p = path + strlen(path);
        while (p > path && *p != '/') p--;
        p++;
        if(strcmp(p, tarname) == 0) {
            printf("%s\n", path);
        }
    } else if (st.type == T_DIR) {
        int n = strlen(path);
        strcpy(buf, path);
        if(buf[n-1] != '/') {
            buf[n] = '/';
            n++;
        }
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0) continue;
            if(strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0) continue;
            memmove(buf+n, de.name, DIRSIZ);
            char* end = buf+n;
            while(*end && *end != ' ') end++;
            *end = 0;
            find(buf, tarname);
        }   
    }
    close(fd);
}

int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(2, "Usage: find path filename\n");
        exit(1);
    }

    find(argv[1], argv[2]);
    exit(0);
}