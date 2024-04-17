#include "client.h"

int tcp_init(int *socket_fd){
    //获取本地ip,port
    char ip[128] = {0};
    getparameter("ip", ip);
    printf("%s\n", ip);
    char port[32] = {0};
    getparameter("port", port);
    printf("%s\n", port);

    //构建struct sockaddr_in类型
    struct sockaddr_in socketaddr;
    socketaddr.sin_family = AF_INET;
    socketaddr.sin_addr.s_addr = inet_addr(ip);
    socketaddr.sin_port = htons(atoi(port));

    int res_connect = connect(*socket_fd, (struct sockaddr *)&socketaddr, sizeof(socketaddr));
    ERROR_CHECK(res_connect, -1, "connect");

    return 0;
}
