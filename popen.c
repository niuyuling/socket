/*  
 *  popen.c     Written by W. Richard Stevens
 *  write 20180623 email: aixiao@aixiao.me
 */    

#include    <stdio.h>
#include    <unistd.h>
#include    <stdlib.h>
#include    <sys/wait.h>
#include    <errno.h>
#include    <fcntl.h> 
#include    <sys/resource.h>
#define CPID "pid/cprocess.pid"
    
static pid_t    *childpid = NULL;    
                        /* ptr to array allocated at run-time */    
static int      maxfd;  /* from our open_max(), {Prog openmax} */    
    
#define SHELL   "/bin/sh"

int cpid(char PID[]) {
	char *pid_file = PID;
	FILE *fp = fopen(pid_file, "w");
	fprintf(fp, "%d\n", getpid());
	fclose(fp);
}

FILE *    
_popen(const char *cmdstring, const char *type)    
{    
    int     i, pfd[2];    
    pid_t   pid;    
    FILE    *fp;    
    
            /* only allow "r" or "w" */    
    if ((type[0] != 'r' && type[0] != 'w') || type[1] != 0) {    
        errno = EINVAL;     /* required by POSIX.2 */    
        return(NULL);    
    }    
    
    if (childpid == NULL) {     /* first time through */    
                /* allocate zeroed out array for child pids */    
#if 0  
        maxfd = open_max();    
#else  
        /* OPEN_MAX is canceled after linux-2.6.24, and become a part of RLIMIT_NOFILE.  
        Execute shell 'ulimit -a' to get more information */  
        struct rlimit limit;  
        if(getrlimit(RLIMIT_NOFILE, &limit) == -1)  
        {  
            perror("getrlimit fail");  
            return(NULL);  
        }  
        maxfd = limit.rlim_cur;  
        //print("rlimit = %d.\n", maxfd);  
#endif  
        if ( (childpid = calloc(maxfd, sizeof(pid_t))) == NULL)    
            return(NULL);    
    }    
    
    if (pipe(pfd) < 0)    
        return(NULL);   /* errno set by pipe() */    
    
    if ( (pid = fork()) < 0)    
        return(NULL);   /* errno set by fork() */    
    else if (pid == 0) {                            /* child */
		printf("Parent process ID: %d\n", getpid()-1);
        printf("Child process ID: %d\n", getpid());
		cpid(CPID);
		if (*type == 'r') {    
            close(pfd[0]);    
            if (pfd[1] != STDOUT_FILENO) {    
                dup2(pfd[1], STDOUT_FILENO);    
                close(pfd[1]);    
            }    
        } else {    
            close(pfd[1]);    
            if (pfd[0] != STDIN_FILENO) {    
                dup2(pfd[0], STDIN_FILENO);    
                close(pfd[0]);    
            }    
        }    
            /* close all descriptors in childpid[] */    
        for (i = 0; i < maxfd; i++)    
            if (childpid[ i ] > 0)    
                close(i);    
    
        execl(SHELL, "sh", "-c", cmdstring, (char *) 0);    
        exit(127);
    }    
                                /* parent */    
    if (*type == 'r') {    
        close(pfd[1]);    
        if ( (fp = fdopen(pfd[0], type)) == NULL)    
            return(NULL);    
    } else {    
        close(pfd[0]);    
        if ( (fp = fdopen(pfd[1], type)) == NULL)    
            return(NULL);    
    }    
    childpid[fileno(fp)] = pid; /* remember child pid for this fd */    
    return(fp);    
}    
    
int    
_pclose(FILE *fp)
{    
    
    int     fd, stat;    
    pid_t   pid;    
    
    if (childpid == NULL)    
        return(-1);     /* popen() has never been called */    
    
    fd = fileno(fp);    
    if ( (pid = childpid[fd]) == 0)    
        return(-1);     /* fp wasn't opened by popen() */    
    
    childpid[fd] = 0;    
    if (fclose(fp) == EOF)    
        return(-1);    
    
    while (waitpid(pid, &stat, 0) < 0)    
        if (errno != EINTR)    
            return(-1); /* error other than EINTR from waitpid() */    
    
    return(stat);   /* return child's termination status */    
}
