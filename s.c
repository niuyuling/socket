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
//#include "popen.c"                                                            // 包含_popen()函数

#define	IP "0.0.0.0"                                                            // IP
#define PORT 9606                                                               // 端口
#define COMMAND_OUT_SIZE 1024
#define PID_FILE_FLOW_SIZE 77
#define BUFFER_SIZE 1024
#define PID "pid/socket.pid"                                                    // 主进程PID存储文件
#define COMMAND_OUT_FILE "log/command.log"                                               // 命令输出文件
// 限制客服端IP
#define I "127.0.0.1"
#define P "192.168.99.109"


extern FILE *
_popen(const char *cmdstring, const char *type);

extern void init_daemon(void);

char help(void)
{
    static const char my_name[] = "socket";
    static const char version[] = "version 1.0";
    static const char subject[] = "socket";

    static const struct {
	const char *f, *s, *e;
    } author = {
    "aixiao@aixiao.me", "aixiao", "Author:",};
    static const char usage[] = "Usage: socket [ip add] & [\"-f|-k file|pid\" || \"command\" ] | [-?h]";

    static const char *s_help[] = {
	"",
	"    -?,-h     : this help",
	"    -d        : daemon",
	"    -f file   : Transfer file",
	"    -k pid    : Child process PID",
	"    -p,       : pid file(default: socket.pid)",
	"",
	"",
	0
    };


    fprintf(stderr, "%s %s, %s\n", my_name, version, subject);
    fprintf(stderr, "%s %s %s\n", author.e, author.s, author.f);
    fprintf(stderr, "%s %s\n", my_name, usage);

    int l;
    for (l = 0; s_help[l]; l++)
	fprintf(stderr, "%s\n", s_help[l]);
}

int command(char buffer[])
{
    FILE *fp = fopen(COMMAND_OUT_FILE, "a+");

    FILE *optput_buffer;                                                        // FILE文件操作的指针
    char optput_flow[COMMAND_OUT_SIZE + 1];                                                    // 命令输出缓存
    optput_buffer = _popen(buffer, "r");
    fread(optput_flow, sizeof(char), COMMAND_OUT_SIZE, optput_buffer);                         // 命令输出写入文件指针
    fseek(fp, 0, SEEK_END);
    fwrite(optput_flow, strlen(optput_flow), 1, fp);

    printf("%s", optput_flow);
    memset(optput_flow, 0, sizeof(optput_flow));
    fclose(fp);
    pclose(optput_buffer);
}

void _kill(char buffer[]) {
	printf("%s\n", buffer);
	kill(atol(buffer), SIGKILL);
}

int file(char buffer[], int client_socket, char *s_name)
{
    bzero(buffer, BUFFER_SIZE);                                                 // 清空


    // 打开文件，准备写入
    char *name = strdup(s_name);                                                      // 固定的文件名
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

int pid(char *optarg)
{
    char *start = "start";
    char *stop = "stop";
    char *reload = "reload";
    char *status = "status";
    char pid[PID_FILE_FLOW_SIZE];
    char dev[PID_FILE_FLOW_SIZE];

    if(optarg == NULL) {
        char *pid_file = PID;                                                   // 固定的文件名
        FILE *fp = fopen(pid_file, "w");
        fprintf(fp, "%d\n", getpid());
        fclose(fp);
    } else if(strcmp(optarg, stop) == 0) {
        char *pid_file = PID;
        FILE *fp = fopen(pid_file, "r");
        fgets(pid, sizeof(pid), fp);
        sprintf(dev, "/dev/%s", pid);
        if(access(dev, 0) == 0)
            printf("%s", pid);
        int s_pid = atoi(pid);
        kill(s_pid, SIGKILL);
        fclose(fp);
    }
}

int _main(int argc, char *argv[])
{
    int ch;
    opterr = 0;
    while((ch = getopt(argc, argv, "ds:h?")) != -1)
    switch (ch) {
    case 'd':
        init_daemon();
        pid(0);
        break;
    case 's':
        pid(optarg);
        exit(0);
        break;
    case 'h': case '?':
        help();
        exit(0);
        break;
    default:
        ;
    }

	static const struct {
		const char *i, *p;
    } ip = {
		I, P,};

    int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);              // 创建套接字
    struct sockaddr_in serv_addr;                                               // 将套接字和IP、端口绑定
    memset(&serv_addr, 0, sizeof(serv_addr));                                   // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;                                             // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(IP);                                  // 具体的IP地址
    serv_addr.sin_port = htons(PORT);                                           // 端口
    bind(server_socket, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
    listen(server_socket, 9);                                                   // 进入监听状态，等待用户发起请求
    char buffer[BUFFER_SIZE];
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
		
		if(strcmp(clnt_ip, ip.i) == 0 || strcmp(clnt_ip, ip.p) == 0) {
        if (buffer[0] == '-' && buffer[1] == 'f') {
			int l = strlen(buffer);
			for (int i = 0; i < l || buffer[i] == '\0'; i++) {
				buffer[i] = buffer[i + 3]; 
			}
			char *name = strdup(buffer);
			memset(buffer, 0, sizeof(buffer));
            file(buffer, client_socket, name);
		}
		else if (buffer[0] == '-' && buffer[1] == 'k') {
			int l = strlen(buffer);
			for (int i = 0; i < l || buffer[i] == '\0'; i++) {
				buffer[i] = buffer[i + 3]; 
			}
			_kill(buffer);
        } else {
			pid_t pid;
			int status,i;
			if(fork()==0){		// 子进程
				command(buffer);
				exit(1);
			}
        }
		}
		else {
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
