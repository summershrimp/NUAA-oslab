#ifndef __MY_SYSTEM_H__
#define __MY_SYSTEM_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void my_system(char *command)
{
    pid_t pid;
    char *cmd = NULL, *p;
    char *argv[64];
    char *file = NULL;
    int i, cmd_len, argc = 0, strspos;
    pid = fork();
    if(pid == -1)
    {
        printf("fork failed!");
        exit(-1);
    }
    else if(pid != 0)
    {   
        waitpid(pid, NULL, 0);
        return ;
    }
    else
    {
        cmd = malloc((strlen(command) + 1)*sizeof(char));
        strcpy(cmd, command);
        cmd_len = strlen(cmd);
        p = cmd + cmd_len - 1;
        while(*p == ' ' || *p == '\n')
        {
            *p = 0;
            --p;
        }
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
        }

        free(cmd);
        exit(0);
    }
}


#endif
