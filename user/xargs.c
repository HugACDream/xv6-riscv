#include "kernel/param.h"
#include "kernel/types.h"
#include "user/user.h"

#define buf_size 512

int main(int argc, char* argv[]) {
    char buf[buf_size+1] = {0};
    uint cur_len = 0;
    char* xargv[MAXARG] = {0};
    int in_end = 0;

    //将xargs的参数提取出来
    for(int i = 1; i < argc; i++) {
        xargv[i-1] = argv[i];   //数组指针，所以可以直接赋值
    }

    while(!(in_end && !cur_len)) {
        if(!in_end) {
            int remain_size = buf_size - cur_len;
            int read_len = read(0, buf + cur_len, remain_size);

            if(read_len < 0) {
                fprintf(2, "xargs: read returns -1 error\n");
                exit(1);
            } else if (read_len == 0) {
                close(0);
                in_end = 1;
            }

            cur_len += read_len;
        }

        char *line_end = strchr(buf, '\n');
        while(line_end) {
            char xbuf[buf_size+1] = {0};
            memcpy(xbuf, buf, line_end - buf);
            xargv[argc - 1] = xbuf;

            int pid = fork();
            if(pid == 0) {
                if(!in_end) close(0);
                if(exec(argv[1], xargv) < 0) {
                    fprintf(2, "xargs: exec fails with -1\n");
                    exit(1);
                }
            } else {
                memmove(buf, line_end + 1, cur_len - (line_end - buf) - 1); // 清除已处理的行
                cur_len -= line_end - buf + 1;
                buf[cur_len] = 0;
                wait(0);

                line_end = strchr(buf, '\n');
            }
        }
    }

    exit(0);
}