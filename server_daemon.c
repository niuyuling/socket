#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

int init_daemon(int nochdir, int noclose)
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        return -1;
    } else if (0 != pid) {
        exit(0);
    }
    //child 1 continues...

    //become session leader
    if (setsid() < 0) {
        return -1;
    }

    signal(SIGHUP, SIG_IGN);
    if ((pid = fork()) < 0) {
        return -1;
    } else if (0 != pid) {
        exit(0);
    }
    //child 2 continues...

    //change working directory
    if (0 == nochdir) {
        chdir("/");
    }
    //close off file descriptors
    /*
       int i;
       for(i=0; i<64; i++) {
       close(i);
       }
     */

    //redirect stdin,stdout,stderror to "/dev/null"
    if (0 == noclose) {
        open("/dev/null", O_RDONLY);
        open("/dev/null", O_RDWR);
        open("/dev/null", O_RDWR);
    }

    return 0;
}
