#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include "iniparser.h"

#define MAX_FD_NUM 5
#define COMMAND_OUT_SIZE 102400
#define PID_FILE_FLOW_SIZE 77
#define BUFFER_SIZE 1024
#define LO "127.0.0.1"          // 限制客服端IP

typedef struct CONF {
    char *ip;
    int port;
    char *clientip;
    char *clientip1;
    char *pid_file;
    
    int len_ip, len_clientip, len_clientip1, len_pid_file;
} conf;

void read_conf(char *file, conf *p);
void free_conf(conf *p);

#endif