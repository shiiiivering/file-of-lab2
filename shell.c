#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>

int main() {
    /* 输入的命令行 */
    char cmd[256];
    /* 命令行拆解成的各部分，以空指针结尾 */
    char *args[128];
    /*获取当前进程pid*/
    printf("set arrey succeed");
    pid_t pid_this = getpid();
    printf("getpid succeed");
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
        int ifpipe = 0;
        args[0] = cmd;
        for (i = 0; *args[i]; i++)
            for (args[i+1] = args[i] + 1; *args[i+1]; args[i+1]++)
                if(*args[i + 1] == '|')ifpipe = 1;
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
        int p;
        if(ifpipe){
            pid_ = fork();
            if(pid_ < 0){
                printf("fork error");
                exit(1);
            }
            else if(pid_ != 0){
                wait(NULL);
                continue;
            }
            for(p = ins; args[p]; p++){
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
                        dup2(pfd[0], STDIN_FILENO);
                        ins = p + 1;
                    }
                    else{
                        close(chfd[0]);
                        dup2(chfd[1], STDOUT_FILENO);
                        break;
                    }
                }
            }
        }
        if(!args[p]){
            close(chfd[1]);
            pfd[0] = chfd[0];
            dup2(pfd[0], STDIN_FILENO);
        }
        

        /* 内建命令 */
        if (strcmp(args[ins], "cd") == 0) {
            if (args[ins + 1])
                chdir(args[ins + 1]);
            
        }
        if (strcmp(args[ins], "pwd") == 0) {
            char wd[4096];
            puts(getcwd(wd, 4096));
            
        }
        if (strcmp(args[ins], "exit") == 0)
            return 0;

        /* 外部命令 */
        else{
            //dup2(fd[1], STDOUT_FILENO);
            execvp(args[ins], args + ins);
            /* execvp失败 */
            
        }
        /*当前部分执行完毕判断是否是父进程，并执行关闭管道等相应操作*/
        if(pid_this != getpid()){   //如果是子进程则结束进程
            close(chfd[1]);
            close(pfd[0]);
            wait(NULL);
            return 0;
        }
        /*else if(ifpipe){            //如果不是则等待子进程执行完毕后继续进入下一个循环
            close(chfd[1]);
            dup2(STDOUT_FILENO, STDOUT_FILENO);
            dup2(STDIN_FILENO, STDIN_FILENO);
        }*/
        /* 父进程 */
        wait(NULL);
        
    }
}