#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>

int main() {
    /* 输入的命令行 */
    char cmd[256];
    /* 命令行拆解成的各部分，以空指针结尾 */
    char *args[128];
    /*获取当前进程pid*/
    pid_t pid_this = getpid();
    while (1) {
        /* 提示符 */
        printf("# ");
        fflush(stdin);
        fgets(cmd, 256, stdin);
        /* 清理结尾的换行符 */
        int i;
        for (i = 0; cmd[i] != '\n'; i++)
            ;
        cmd[i] = '\0';
        /* 拆解命令行 */
        args[0] = cmd;
        for (i = 0; *args[i]; i++)
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if (*args[i+1] == ' ') {
                    *args[i+1] = '\0';
                    args[i+1]++;
                    break;
                }
        args[i] = NULL;

        /* 没有输入命令 */
        if (!args[0])
            continue;
        /* 管道相关 */
        int ins = 0;
        pid_t pid_;
        char buf[1024];
        int pfd[2], chfd[2];

        
        for(int p = ins; args[p]; p++){
            if(*args[p] == '|'){
                *args[p] = '\n';
                if(pipe(chfd) == -1){
                    printf("pipe error");
                    exit(1);
                }
                pid_ = fork();
                if(pid_ < 0){
                    printf("fork error");
                    exit(1);
                }
                else if(pid_ == 0){
                    close(chfd[1]);
                    pfd[0] = chfd[0];
                    dup2(pfd[0], STDIN_FINENO);
                    ins = p + 1;
                }
                else{
                    close(chfd[0]);
                    dup2(fd[1], STDOUT_FILENO);
                    break;
                }
            }
        }
        

        /* 内建命令 */
        if (strcmp(args[0], "cd") == 0) {
            if (args[1])
                chdir(args[1]);
            continue;
        }
        if (strcmp(args[0], "pwd") == 0) {
            char wd[4096];
            puts(getcwd(wd, 4096));
            continue;
        }
        if (strcmp(args[0], "exit") == 0)
            return 0;

        /* 外部命令 */
        pid_t pid = fork();
        if (pid == 0) {
            /* 子进程 */
            execvp(args[0], args);
            /* execvp失败 */
            return 255;
        }
        /* 父进程 */
        wait(NULL);
        if()
    }
}