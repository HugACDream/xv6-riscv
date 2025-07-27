#include "kernel/types.h"
#include "user/user.h"

void seive(int pfd[2]) {
    close(pfd[1]);
    int prime;
    if(read(pfd[0], &prime, sizeof(int)) == 0){
        close(pfd[0]);
        return;
    }
    fprintf(1, "prime %d\n", prime);

    int pfd_next[2];
    pipe(pfd_next);

    int pid = fork();
    if(pid == 0) {
        close(pfd[0]);
        seive(pfd_next);
    } else {
        int num;
        while (read(pfd[0], &num, sizeof(int))) {
            if(num % prime) {
                write(pfd_next[1], &num, sizeof(int));
            }
        }
        close(pfd[0]);
        close(pfd_next[1]);
        wait(0);
        exit(0);
    }
}

int main(int argc, char* argv[]) {
    if(argc != 1) {
        fprintf(2, "Usage: prime\n");
        exit(1);
    }
    
    int pfd[2];
    pipe(pfd);

    int pid = fork();
    if(pid == 0) {
        seive(pfd);
    } else {
        int i;
        for(i = 2; i <= 35; i++) {
            write(pfd[1], &i, sizeof(int));
        }
        close(pfd[1]);
        wait(0);
    }
    exit(0);
}