#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>

void my_system(char *command, int read_fd, int write_fd, int need_wait)
{
    pid_t pid;
    char *cmd = NULL, *p;
    char *argv[64];
    char *file = NULL;
    int i, cmd_len, argc = 0, strspos, status;
    pid = fork();
    if(pid == -1)
    {
        printf("fork failed!");
        exit(-1);
    }
    else if(pid != 0)
    {
        if(read_fd != -1)
            close(read_fd);
        if(write_fd != -1)
            close(write_fd);
        
        if(need_wait)
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
        if(read_fd != -1 )
        {
            close(STDIN_FILENO);
            status = dup2(read_fd, STDIN_FILENO);
            close(read_fd);
            if(status < 0) 
            {
                printf("%s dup stdin failed: %s\n", cmd, strerror(errno));
            }
        }
        if(write_fd != -1)
        {
            close(STDOUT_FILENO);
            status = dup2(write_fd, STDOUT_FILENO);
            close(write_fd);
            if(status < 0)  
            {
                printf("%s dup stdout failed: %s\n", cmd, strerror(errno));
            }
        }
        if(execvp(cmd, argv) == -1)
        {
            printf("%s exec failed: %s\n", cmd, strerror(errno));
        }

        free(cmd);
        exit(0);
    }
}

void findfilename(char *src, char **dst)
{
    int i, len;
    while(*src == ' ') 
        ++src;
    *dst = src;
    len = strlen(src);
    for(i=0; i<len; ++i)
        if(src[i] == ' ' || src[i] == '\n')
            src[i] = 0;
}

void dup_handler(char *command, int pipe_read, int pipe_write, int need_wait)
{
    
    int i, len = strlen(command), t;
    int r_stdin = 0, r_stdout = 0;
    char *stdin, *stdout;
    int read_fd = -1, write_fd = -1;
    for(i=0; i<len; ++i)
    {
        if(command[i] == '>' )
        {
            command[i] = 0;
            findfilename(command + i + 1, &stdout);
            //printf("stdout: %s\n", stdout);
            r_stdout = 1;
        } 
        else if(command[i] == '<')
        {
            command[i] = 0;
            r_stdin = 1;
            findfilename(command + i + 1, &stdin);
            //printf("stdin: %s\n", stdin);
        }
    }
    //printf("cmd: %s, ", command);
    if(r_stdin)
    {
        //printf("read redir, ");
        read_fd = open(stdin, O_CREAT | O_RDWR, S_IREAD | S_IWRITE); 
    }
    else if(pipe_read > -1)
    {
        //printf("read pipe, ");
        read_fd = pipe_read;
    }
    else 
    {
        //printf("read stdin, ");
        read_fd = -1;
    }
    
    if(r_stdout)
    {   
        //printf("write redir.");
        write_fd = open(stdout, O_CREAT | O_RDWR, S_IREAD | S_IWRITE);
    }
    else if(pipe_write > -1)
    {
        //printf("write pipe.");
        write_fd = pipe_write;
    }
    else 
    {
        //printf("write stdout.");
        write_fd = -1;
    }
    //printf("\n");
    my_system(command, read_fd, write_fd, need_wait);
    
}

void pipe_handler(char *command)
{
    int fildes[2];//0 read pipe  1 write pipe
    char *cmds[64], *p;
    int cmdc = 0, len = strlen(command);
    int status, i, t; 
    
    cmds[cmdc++] = command;
    
    for(i=0; i < len; ++i)
    {
        if(command[i] == '|')
        {
            command[i] = 0;
            p = command + i - 1;
            while(*p == ' ' || *p == '\n')
            {
                *p = 0;
                --p;
            }
            p = command + i + 1;
            while(*p && (*p == ' ' || *p == '\n'))
            {
                *p = 0;
                ++p;
                ++i;
            }
            cmds[cmdc++] = p;
        }
    }
    if(cmdc == 1)
    {
        dup_handler(cmds[0], -1, -1, 1);
        return;
    }
    
    status = pipe(fildes); //0读 1写
    if(status == -1)
    {
        printf("cannot open pipe.\n");
        return ;
    }
    
    for(i=0; i<cmdc; ++i)
    {
        if(i == 0)
        {
            dup_handler(cmds[i], -1, fildes[1], 0);
        } 
        else if(i != cmdc - 1)
        {
            t = fildes[0];
            status = pipe(fildes);
            if(status == -1)
            {
                printf("cannot open pipe.\n");
                return ;
            }
            dup_handler(cmds[i], t, fildes[1], 0);
        }
        else
        {
            dup_handler(cmds[i], fildes[0], -1, 1);
        }
       
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
    if(strstr(p, "cd") == p)
    {
        if(strlen(p) > 2)
        {
            sscanf(p, "cd %s", dir);
            chdir(dir);
        }
    }
    else if (strstr(p, "pwd") == p)
    {
        getcwd(buf, 256);
        puts(buf);
    }
    else if(strstr(p, "exit") == p)
    {
        exit(0);
    }
    else if(strlen(p) > 0)
    {
        pipe_handler(p);
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

