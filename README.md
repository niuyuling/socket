# socket
简单的socket服务器, 文件传输和远程命令.

    s version 1.0, s
    Author: aixiao aixiao@aixiao.me
    s usage: s [-d] [-s stop|status] [-p pid file] | [-?h]

        -?,-h     : this help
        -d        : daemon
        -s        : signal: stop,status
        -p,       : pid file(default: s.pid)

    c usage: c [IP address] & ["-f file | -k pid" | "command"] | [-?h]

        -?,-h     : this help
        -f file   : Transfer file
        -k pid    : Child process PID