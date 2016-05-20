#ifndef _MY_DUP_H_
#define _MY_DUP_H_

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#include <fcntl.h>
#include <unistd.h>

int is_stdout_redir = 0;
int is_stdin_redir = 0;
int oldstdoutfd;
int oldstdinfd;

int redirectstdin_fd(int fd)
{
    if(is_stdin_redir)
        return -1;
        
    oldstdinfd = dup(STDIN);
    dup2(fd, STDIN);
    //close(fd);

    is_stdin_redir = 1;
    return 0;
}

int redirectstdin(char * filename)
{
    if(is_stdin_redir)
        return -1;
        
    oldstdinfd = dup(STDIN);
    int filefd = open(filename, O_CREAT | O_RDWR, S_IREAD | S_IWRITE); 
    dup2(filefd, STDIN);
    close(filefd);
    is_stdin_redir = 1;
    return 0;
}

int restorestdin()
{
    int curstdin;
    if(!is_stdin_redir)
        return -1;
    curstdin = dup(STDIN);
    dup2(oldstdinfd, STDIN);
    close(oldstdinfd);
    is_stdin_redir = 0;
    
    return curstdin;
}

int redirectstdout_fd(int fd)
{
    if(is_stdout_redir)
        return -1;
        
    oldstdoutfd = dup(STDOUT);
    dup2(fd, STDOUT);
    //close(fd);
    
    is_stdout_redir = 1;
    return 0;
}

int redirectstdout(char * filename)
{
    if(is_stdout_redir)
        return -1;
        
    oldstdoutfd = dup(STDOUT);
    int filefd = open(filename, O_CREAT | O_RDWR, S_IREAD | S_IWRITE); 
    dup2(filefd, STDOUT);
    close(filefd);
    is_stdout_redir = 1;
    return 0;
}

int restorestdout()
{
    int curstdout;
    if(!is_stdout_redir)
        return -1;
        
    curstdout = dup(STDOUT);
    dup2(oldstdoutfd, STDOUT);
    close(oldstdoutfd);
    is_stdout_redir = 0;
    
    return curstdout;
}

#endif