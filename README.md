# socket
简单的socket服务器, 文件传输和远程命令.

    s version 1.4, server
    Author: aixiao aixiao@aixiao.me
    s usage: s [-d] [-s stop|status] [-p pid file] | [-?|h]

        -?,-h     : this help
        -d        : daemon
        -s        : signal: stop,status
        -p,       : pid file(default: pid/s.pid)

    c version 1.4, client
    Author: aixiao aixiao@aixiao.me
    c usage: c [-l Ip Address] & [-p Port] & [-b "-f file" | "-k pid" | "command"] | [-?|h]

        -?|-h Help    : this help
        -l IP         : Ip Address
        -p Port       : Port
        -b Buffer     : send buffer
                        "-f file : Transfer file"
                        "-k pid  : Child process PID"
                        "command : Shell command"


# 编译
    git clone https://github.com/niuyuling/socket.git
    git clone https://github.com/ndevilla/iniparser.git
    cd iniparser
    make
    cd ../socket
    make clean; make
