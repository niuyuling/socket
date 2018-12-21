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

#include "iniparser.h"

#define COMMAND_OUT_SIZE 102400
#define PID_FILE_FLOW_SIZE 77
#define BUFFER_SIZE 1024
#define LO "127.0.0.1"          // 限制客服端IP

int _pclose(FILE * fp);
extern FILE *_popen(const char *cmdstring, const char *type);
int init_daemon(int nochdir, int noclose);
extern char help(void);

int command(int socket, char buffer[])
{
    char *inifile = "conf/config.ini";
    dictionary *ini = iniparser_load(inifile);
    const char *COMMAND_OUT_FILE =
        iniparser_getstring(ini, "server:COMMAND_OUT_FILE", NULL);

    FILE *fp = fopen(COMMAND_OUT_FILE, "a+");
    if (fp == NULL) {
        errno = 0;
        mkdir("log", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        perror("mkdir");
    }

    FILE *optput_buffer;        // FILE文件操作的指针
    char optput_flow[COMMAND_OUT_SIZE]; // 命令输出缓存

    optput_buffer = _popen(buffer, "r");

    fread(optput_flow, sizeof(char), COMMAND_OUT_SIZE, optput_buffer); // 命令输出写入文件指针
    fseek(fp, 0, SEEK_END);
    fwrite(optput_flow, strlen(optput_flow), 1, fp);

    errno = 0;
    send(socket, optput_flow, COMMAND_OUT_SIZE, 0); // 回传数据到客户端
    if (errno != 0) {
        printf("errno值: %d\n", errno);
        printf("错误信息: %s\n", strerror(errno));
    }
    printf("%s", optput_flow);
    memset(optput_flow, 0, sizeof(optput_flow));
    fclose(fp);
    _pclose(optput_buffer);
    iniparser_freedict(ini);
    return 0;
}

void _kill(char buffer[])
{
    printf("%s\n", buffer);
    kill(atol(buffer), SIGKILL);
}

int file(char buffer[], int client_socket, const char *s_name)
{
    bzero(buffer, BUFFER_SIZE); // 清空

    // 打开文件，准备写入
    char *name = strdup(s_name); // 固定的文件名
    printf("%s\n", name);
    FILE *fp = fopen(name, "wb");
    if (NULL == fp) {
        printf("File Can Not Open To Write\n");
        exit(1);
    }
    // 从服务器接收数据到buffer中
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
    int length = 0;
    int allcount = 0;
    while ((length = (int)recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (fwrite(buffer, sizeof(char), length, fp) < length) {
            printf("File: %s Write Failed\n", name);
            break;
        }
        allcount += length;
        bzero(buffer, BUFFER_SIZE);
    }
    bzero(buffer, BUFFER_SIZE);
    // 接收成功后，关闭文件
    printf("Receive File Total: %dK\n", allcount);
    fclose(fp);
    return 0;
}

int pid(int s, char *optarg)
{
    char *inifile = "conf/config.ini";
    dictionary *ini = iniparser_load(inifile);
    const char *PID_FILE = iniparser_getstring(ini, "server:PID_FILE", NULL);

    char *stop = "stop";
    char *status = "status";
    char pid[PID_FILE_FLOW_SIZE];
    char proc[PID_FILE_FLOW_SIZE + 20];

    if (optarg == NULL) {
        printf("ai");
        FILE *fp = fopen(PID_FILE, "w");
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }

    if (s == 's') {
        if (strcmp(optarg, stop) == 0) {
            FILE *fp = fopen(PID_FILE, "r");
            fgets(pid, sizeof(pid), fp);
            sprintf(proc, "/proc/%s", pid);
            if (access(proc, 0) == 0)
                printf("%s\n", pid);
            int s_pid = atoi(pid);
            kill(s_pid, SIGKILL);
            fclose(fp);
        } else if (strcmp(optarg, status) == 0) {
            FILE *fp = fopen(PID_FILE, "r");
            fgets(pid, sizeof(pid), fp);
            sprintf(proc, "/proc/%s", pid);
            if (access(proc, 0) == 0)
                printf("%s\n", pid);
            fclose(fp);
        }
    } else if (s == 'p') {
        FILE *fp = fopen(optarg, "w");
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }

    iniparser_freedict(ini);
    return 0;
}

char *del_strn(char *s, int n)
{
    return s = s + n;
}

int _main(int argc, char *argv[])
{
    char *inifile = "conf/config.ini";
    dictionary *ini = iniparser_load(inifile);

    const char *IP = iniparser_getstring(ini, "server:IP", NULL);
    int PORT = iniparser_getint(ini, "server:PORT", 0);
    const char *CLIENTIP = iniparser_getstring(ini, "server:CLIENTIP", NULL);
    const char *CLIENTIP1 = iniparser_getstring(ini, "server:CLIENTIP1", NULL);
    const char *PID_FILE = iniparser_getstring(ini, "server:PID_FILE", NULL);

    FILE *fp = fopen(PID_FILE, "r");
    if (fp == NULL) {
        errno = 0;
        mkdir("pid", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
        perror("mkdir");
        return 1;
    }
    fclose(fp);

    printf("%s\n", IP);
    printf("%d\n", PORT);
    printf("%s\n", CLIENTIP);
    printf("%s\n", CLIENTIP1);

    int ch;
    opterr = 0;
    while ((ch = getopt(argc, argv, "ds:p:h?")) != -1) {
        switch (ch) {
        case 'd':
            init_daemon(1, 1);
            pid(0, 0);
            break;
        case 's':
            pid('s', optarg);
            exit(0);
            break;
        case 'p':
            pid('p', optarg);
            break;
        case 'h':
        case '?':
            help();
            exit(0);
            break;
        default:
            ;
        }
    }

    int status;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); // 创建套接字
    if (server_socket == -1) {
        printf("Socket was not created.\n");
        exit(1);
    }
    struct sockaddr_in serv_addr; // 将套接字和IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr)); // 每个字节都用0填充
    serv_addr.sin_family = AF_INET; // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(IP); // 具体的IP地址
    serv_addr.sin_port = htons(PORT); // 端口
    if (bind(server_socket, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) <
        0) {
        printf("Failed to bind to socket.\n");
    }
    if (listen(server_socket, 9) < 0) { // 进入监听状态，等待用户发起请求
        printf("Failed to listen to socket.\n");
    }

    while (1) {
        struct sockaddr_in clnt_addr;
        /*
           socklen_t clnt_addr_size = sizeof(clnt_addr);
           int client_socket =
           accept(server_socket, (struct sockaddr *) &clnt_addr,
           &clnt_addr_size);

           socklen_t len = sizeof(clnt_addr);
           getpeername(server_socket, (struct sockaddr *) &clnt_addr, &len);
           const char *clnt_ip = inet_ntoa(clnt_addr.sin_addr);
           printf("other side IP：%s\n", inet_ntoa(clnt_addr.sin_addr));
           printf("other side PORT：%d\n", ntohs(clnt_addr.sin_port));
         */
        socklen_t addrlen = sizeof(clnt_addr);
        char ipstr[128];
        int client_socket =
            accept(server_socket, (struct sockaddr *)&clnt_addr, &addrlen);

        inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, ipstr, sizeof(ipstr));
        printf("Client Ip %s\tPort %d\n",
               inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, ipstr,
                         sizeof(ipstr)), ntohs(clnt_addr.sin_port));
        const char *clnt_ip =
            inet_ntop(AF_INET, &clnt_addr.sin_addr.s_addr, ipstr,
                      sizeof(ipstr));

        recv(client_socket, buffer, BUFFER_SIZE, 0); // 接收客户端发来消息

        if (strcmp(clnt_ip, LO) == 0 || strcmp(clnt_ip, CLIENTIP) == 0
            || strcmp(clnt_ip, CLIENTIP1) == 0) {
            if (buffer[0] == '-' && buffer[1] == 'f') {
                const char *name = strdup(del_strn(buffer, 3));
                char *n = NULL;
                if ((n = strrchr(name, '/')) != NULL) {
                    name = n + 1;
                }
                if ((n = strrchr(name, '\\')) != NULL) {
                    name = n + 1;
                }
                printf("%s\n", name);
                memset(buffer, 0, sizeof(buffer));
                file(buffer, client_socket, name);
                name = NULL;
            } else if (buffer[0] == '-' && buffer[1] == 'k') {
                char *k;
                k = buffer;
                k = k + 3;
                _kill(k);
            } else {
                if (fork() == 0) {
                    command(client_socket, buffer);
                    exit(5);
                } else {
                    wait(&status);
                }
            }
        } else {
            printf("Illegal IP: %s\n", clnt_ip);
        }
        memset(buffer, 0, sizeof(buffer));
        close(client_socket);   // 关闭套接字
    }
    iniparser_freedict(ini);
    close(server_socket);
    return 0;
}

int main(int argc, char *argv[])
{
    return _main(argc, argv);
}
