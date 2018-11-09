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
#include "config.c"


#define COMMAND_OUT_SIZE 1024000
#define PID_FILE_FLOW_SIZE 77
#define BUFFER_SIZE 1024
// 限制客服端IP
#define LO "127.0.0.1"

int _pclose(FILE *fp);
extern FILE * _popen(const char *cmdstring, const char *type);
extern void init_daemon(void);
extern char help(void);

int command(int socket, char buffer[])
{
    signed char *COMMAND_OUT_FILE = _config(4);
    FILE *fp = fopen(COMMAND_OUT_FILE, "a+");
    FILE *optput_buffer;                                                        // FILE文件操作的指针
    char optput_flow[COMMAND_OUT_SIZE + 1];                                     // 命令输出缓存

    optput_buffer = _popen(buffer, "r");
    fread(optput_flow, sizeof(char), COMMAND_OUT_SIZE, optput_buffer);          // 命令输出写入文件指针
    fseek(fp, 0, SEEK_END);
    fwrite(optput_flow, strlen(optput_flow), 1, fp);

    send(socket, optput_flow, COMMAND_OUT_SIZE+1, 0);                           // 回传数据到客户端
    printf("%s", optput_flow);
    memset(optput_flow, 0, sizeof(optput_flow));
    fclose(fp);
    _pclose(optput_buffer);
}

void _kill(char buffer[]) {
    printf("%s\n", buffer);
    kill(atol(buffer), SIGKILL);
}

int file(char buffer[], int client_socket, char *s_name)
{
    bzero(buffer, BUFFER_SIZE);                                                 // 清空


    // 打开文件，准备写入
    char *name = strdup(s_name);                                                // 固定的文件名
    printf("%s\n", name);
    FILE *fp = fopen(name, "w");
    if (NULL == fp) {
        printf("File Can Not Open To Write\n");
        exit(1);
    }
    // 从服务器接收数据到buffer中
    // 每接收一段数据，便将其写入文件中，循环直到文件接收完并写完为止
    int length = 0;
    int allCount = 0;
    while ((length =
        (int) recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        if (fwrite(buffer, sizeof(char), length, fp) < length) {
            printf("File: %s Write Failed\n", name);
            break;
    }
        allCount += length;
        bzero(buffer, BUFFER_SIZE);
    }
    bzero(buffer, BUFFER_SIZE);
    // 接收成功后，关闭文件
    printf("Receive File Total: %dK\n",
        allCount);
    fclose(fp);
}

int pid(int s, char *optarg)
{
	signed char *PID_FILE = _config(3);
    char *stop = "stop";
    char *status = "status";
    char pid[PID_FILE_FLOW_SIZE];
    char proc[PID_FILE_FLOW_SIZE];

    if(optarg == NULL) {
        FILE *fp = fopen(PID_FILE, "w");
        fprintf(fp, "%d", getpid());
        fclose(fp);
    }

    if(s == 's'){
        if(strcmp(optarg, stop) == 0) {
            FILE *fp = fopen(PID_FILE, "r");
            fgets(pid, sizeof(pid), fp);
            sprintf(proc, "/proc/%s", pid);
            if(access(proc, 0) == 0)
                printf("%s\n", pid);
            int s_pid = atoi(pid);
            kill(s_pid, SIGKILL);
            fclose(fp);
        } else if(strcmp(optarg, status) == 0) {
            FILE *fp = fopen(PID_FILE, "r");
            fgets(pid, sizeof(pid), fp);
            sprintf(proc, "/proc/%s", pid);
            if(access(proc, 0) == 0)
                printf("%s\n", pid);
            fclose(fp);
        }
    } else if(s == 'p') {
        FILE *fp = fopen(optarg, "w");
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    }
}

int _main(int argc, char *argv[])
{
    signed char *IP = _config(0);
    int PORT = atoi(_config(1));
    signed char *CLIENTIP = _config(2);

    int ch;
    opterr = 0;
    while((ch = getopt(argc, argv, "ds:p:h?")) != -1)
    switch (ch) {
    case 'd':
        init_daemon();
        pid(0, 0);
        break;
    case 's':
        pid('s', optarg);
        exit(0);
        break;
    case 'p':
        pid('p', optarg);
        break;
    case 'h': case '?':
        help();
        exit(0);
        break;
    default:
        ;
    }

    pid_t pid;
    int status, i;
    char buffer[BUFFER_SIZE];
    int l = strlen(buffer);

    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);              // 创建套接字
    struct sockaddr_in serv_addr;                                               // 将套接字和IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr));                                   // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;                                             // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(IP);                                  // 具体的IP地址
    serv_addr.sin_port = htons(PORT);                                           // 端口
    bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(server_socket, 9);                                                   // 进入监听状态，等待用户发起请求

    while (1) {
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int client_socket =
            accept(server_socket, (struct sockaddr *) &clnt_addr,
                &clnt_addr_size);

        int len = sizeof(clnt_addr);
        getpeername(server_socket, (struct sockaddr *) &clnt_addr, &len);
        char *clnt_ip = inet_ntoa(clnt_addr.sin_addr);
        printf("other side IP：%s\n", inet_ntoa(clnt_addr.sin_addr));
        printf("other side PORT：%d\n", ntohs(clnt_addr.sin_port));

        memset(buffer, 0, sizeof(buffer));
        recv(client_socket, buffer, BUFFER_SIZE, 0);                            // 接收客户端发来消息
		
        if(strcmp(clnt_ip, LO) == 0 || strcmp(clnt_ip, CLIENTIP) == 0) {
            if(buffer[0] == '-' && buffer[1] == 'f') {
                for (i = 0; i < l || buffer[i] == '\0'; i++) {
                    buffer[i] = buffer[i + 3]; 
                }
                char *name = strdup(buffer);
                memset(buffer, 0, sizeof(buffer));
                file(buffer, client_socket, name);
            } else if (buffer[0] == '-' && buffer[1] == 'k') {
                for (i = 0; i < l || buffer[i] == '\0'; i++) {
                    buffer[i] = buffer[i + 3]; 
                }
                _kill(buffer);
            } else {
                if(fork()==0){
                    command(client_socket, buffer);
                    exit(5);
                } else {
                    pid=wait(&status);
                }
            }
        } else {
            printf("%s\n", clnt_ip);
        }
        close(client_socket);                                                   // 关闭套接字
    }
    close(server_socket);
    return 0;
}

int main(int argc, char *argv[])
{
    return _main(argc,argv);
}
