#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9606
#define BUFFER_SIZE 1024

extern char help(void);

int main(int argc, char *argv[])
{
    char *IP = argv[1];
    char *buffer = argv[2];
	
    if(IP == NULL || buffer == NULL) {
        help();
        exit(0);
    }

    int sock = socket(AF_INET, SOCK_STREAM, 0);             //创建套接字
    struct sockaddr_in serv_addr;                           //向服务器(特定的IP和端口)发起请求
    memset(&serv_addr, 0, sizeof(serv_addr));               //每个字节都用0填充
    serv_addr.sin_family = AF_INET;                         //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(IP);              //具体的IP地址
    serv_addr.sin_port = htons(PORT);                       //端口
    connect(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    send(sock, buffer, BUFFER_SIZE, 0);

    if (buffer[0] == '-' && buffer[1] == 'f') {
        int l = strlen(buffer);
        int i;
        for (i = 0; i < l || buffer[i] == '\0'; i++) {
            buffer[i] = buffer[i + 3]; 
        }
        char *parameter;
        parameter = "-f";
        printf("%s\n", buffer);                             //打印文件名字

        FILE *fp = fopen(buffer, "r");
        if (NULL == fp) {
            printf("File: %s Not Found\n", buffer);
        } else {
            bzero(buffer, BUFFER_SIZE);
            int length = 0;
            int allCount = 0;
            // 每读取一段数据，便将其发送给客户端，循环直到文件读完为止
            while ((length =
                (int) fread(buffer, sizeof(char), BUFFER_SIZE,
                    fp)) > 0) {
                if (send(sock, buffer, length, 0) < 0) {
                    printf("Send File:%s Failed./n", buffer);
                    break;
                }
                allCount++;
                bzero(buffer, BUFFER_SIZE);
            }
            fclose(fp);		// 关闭文件
            printf("File: %s Transfer Successful! Total: %dK\n", buffer,
            allCount);
        }
    } else {
        send(sock, buffer, BUFFER_SIZE, 0);
        printf("%s\n", buffer);
        memset(buffer, 0, sizeof(buffer));
    }
    close(sock);                                            //关闭套接字
    return 0;
}
