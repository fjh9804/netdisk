#include "client.h"

//接收服务器返回的一般消息
int recv_msg(char *msg, int net_fd){
    int res = recv(net_fd, msg, sizeof(msg), 0);
    ERROR_CHECK(res, -1, "recv msg");

    bzero(msg, sizeof(msg));
}