#include "utils.h"
void spawn(const char *cmd[])
{
    if (fork() == 0)
    {
        if (fork() == 0)
        {
            setsid();
            execvp(cmd[0], (char *const *)cmd);
            perror("exec failed");
            exit(1);
        }
        exit(0);
    }
    wait(NULL);
}