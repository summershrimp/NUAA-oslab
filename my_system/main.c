#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "my_system.h"

void execute(char *command)
{
    my_system(command);
    printf(">> Command '%s' finished!\n", command);
}

int main(void)
{
    execute("ls /");
    execute("echo Hello World");
    execute("ping www.baidu.com");
    return 0;
}
