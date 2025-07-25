#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char* argv[]) {

    if(argc != 1) {
        fprintf(2, "Usage: pingpong\n");
        exit(1);
    }

    int p1[2], p2[2];
    pipe(p1);
    pipe(p2);

    char buf = 'a';
    int pid = fork();

    if(pid < 0) {
        fprintf(2, "fork error\n");
        exit(1);
    }

    if(pid > 0) {
        close(p2[0]);
        close(p1[1]);

        write(p2[1], &buf, sizeof(char));
        close(p2[1]);

        read(p1[0], &buf, sizeof(char));
        fprintf(1, "received pong\n");
        close(p1[0]);

        wait(0);
        exit(0);
    } else {
        close(p1[0]);
        close(p2[1]);
        
        read(p2[0], &buf, sizeof(char));
        fprintf(1, "received ping\n");
        close(p2[0]);

        write(p1[1], &buf, sizeof(char));
        close(p1[1]);

        exit(0);
    }
}