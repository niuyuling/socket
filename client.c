#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9606
#define BUFFER_SIZE 1024

extern char help(void);

int file(char buffer[], int sock, char *file_name)
{
    FILE *fp = fopen(buffer, "r");
    if (NULL == fp) {
        printf("File: %s Not Found\n", file_name);
    } else {
        bzero(buffer, BUFFER_SIZE);
        int length = 0;
        int allCount = 0;
        while ((length = (int)fread(buffer, sizeof(char), BUFFER_SIZE, fp)) > 0) {
            if (send(sock, buffer, length, 0) < 0) {
                printf("Send File:%s Failed./n", file_name);
                break;
            }
            allCount++;
            bzero(buffer, BUFFER_SIZE);
        }
        fclose(fp);    // 关闭文件
        printf("File: %s Transfer Successful! Total: %dK\n", file_name,
               allCount);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        help();
        exit(0);
    }
    char *IP = argv[1];
    char *buffer = argv[2];
    printf("%s\n", IP);
    printf("%s\n", buffer);
    int l = strlen(buffer);
    int i;
    char file_name[l];

    int sock = socket(AF_INET, SOCK_STREAM, 0);    // 创建套接字
    if (sock == -1) {
        printf("Socket was not created.\n");
        exit(1);
    }
    struct sockaddr_in serv_addr;                  // 向服务器(特定的IP和端口)发起请求
    memset(&serv_addr, 0, sizeof(serv_addr));      // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;                // 使用IPv4地址
    //serv_addr.sin_addr.s_addr = inet_addr(IP);   // 具体的IP地址
    inet_pton(AF_INET, IP, &serv_addr.sin_addr.s_addr);
    serv_addr.sin_port = htons(PORT);              // 端口
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Failed to connect to socket.\n");
        exit(3);
    }

    if (send(sock, buffer, l, 0) < 0) {
        printf("Failed to send data to socket.\n");
    }

    if (buffer[0] == '-' && buffer[1] == 'f') {
        for (i = 0; i < l || buffer[i] == '\0'; i++) {
            buffer[i] = buffer[i + 3];
        }
        printf("%s\n", buffer);              // 打印文件名字
        sprintf(file_name, "%s", buffer);    // 复制文件名字
        file(buffer, sock, file_name);
    } else {
        printf("%s\n", buffer);
        memset(buffer, 0, l);

        recv(sock, buffer, BUFFER_SIZE, 0);  // 接收服务端发来的buffer
        printf("%s", buffer);
    }

    close(sock);    //关闭套接字
    return 0;
}
