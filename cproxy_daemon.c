#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
 
int init_daemon(void) 
{
    int pid;

    pid=fork();
    if(pid > 0) {
        exit(0);
    }
    else if(pid < 0) {
        return -1;
    }

    setsid();

    pid=fork();
    if(pid > 0) {
		printf("%d", pid);
        exit(0);
    }
    else if(pid < 0) {
        return -1;
    }

    signal(SIGCHLD, SIG_IGN); 

    return 0;
}
