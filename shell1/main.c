#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
void my_system(char *command)
{
    pid_t pid;
    char *cmd = NULL;
    char *argv[256];
    char *file = NULL;
    int i, cmd_len, argc, strspos;
    pid = fork();
    printf("child process\n");
    if(pid == -1)
    {
        
        printf("fork failed!");
        exit(-1);
    }
    else if(pid != 0)
    {
        printf("parent process\n");
        waitpid(pid, NULL, 0);
        return ;
    }
    else
    {
        printf("child process\n");
        cmd = malloc((strlen(command) + 1)*sizeof(char));
        strcpy(cmd, command);
        cmd_len = strlen(cmd);
        for(i = 0, strspos = 0; i<=cmd_len; ++i)
        {
            if((cmd[i] == ' ' && cmd[i-1] != '\\' ) || cmd[i] == 0)
            {
                cmd[i] = 0;
                argv[argc] = cmd + strspos;
                //printf("argv[%d]: %s\n", argc, argv[argc]);
                ++argc;
                strspos = i + 1;
            }
        }
        argv[argc] = NULL;
        if(execvp(cmd, argv) == -1)
        {
            printf("exec failed\n");
            printf("%d\n", errno);
        }

        free(cmd);
        exit(0);
    }
}

void loop()
{
    char buf[256];
    char *p;
    char dir[256];
    int i = 0;
    getcwd(buf, 256);
    printf("%s ", buf);
    putchar('$');
    putchar(' ');
    fgets(buf, 256, stdin);
        p = buf + strlen(buf) - 1; 
    while(*p == ' ' || *p == '\n')
    {
        *p = 0;
        --p;
    }
    p = buf;
    while((*p == ' ' || *p == '\n' ) && *p)
        ++p;
    if(strstr(p, "cd") == buf)
    {
        if(strlen(p) > 2)
        {
            sscanf(p, "cd %s", dir);
            chdir(dir);
        }
    }
    else if (strstr(p, "pwd") == buf)
    {
        getcwd(buf, 256);
        puts(buf);
    }
    else if(strstr(p, "exit") == buf)
    {
        exit(0);
    }
    else if(strlen(p) > 0)
    {
        my_system(p);
    }
}

int main(int argc, char **argv)
{
    while(1)
    {
        loop();
    }
    return 0;
}
