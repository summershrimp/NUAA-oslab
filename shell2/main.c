#include <stdio.h>
#include <stdlib.h>
#include "../my_system/my_system.h"
#include "my_dup.h"


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

void command_handler(char *command)
{
    //printf("command: %s\n", command);
    int i, len = strlen(command);
    int r_stdin = 0, r_stdout = 0;
    char *stdin, *stdout;
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
    
    if(r_stdin)
        redirectstdin(stdin);
    if(r_stdout)
        redirectstdout(stdout);
        
    my_system(command);
    
    if(r_stdin)
        restorestdin();
    if(r_stdout)
        restorestdout();
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
        command_handler(p);
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

