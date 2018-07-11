#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

void init_daemon(void)
{
    int pid;
    if (pid = fork()) {		// 是父进程，结束父进程
        exit(0);
    } else if (pid < 0) {
        exit(1);		// fork失败，退出
    }
    
    // 是第一子进程，后台继续执行
    // 第一子进程成为新的会话组长和进程组长
    setsid();
    
    // 并与控制终端分离
    if (pid = fork()) {
        exit(0);
	// 是第一子进程，结束第一子进程
    } else if (pid < 0) {
        exit(1);		// fork失败，退出
    }
}
