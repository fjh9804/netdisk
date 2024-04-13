#include "client.h"

//处理upload命令
int upload_file(char *path, int net_fd){
    //首先发送给服务器指令类型为2
    int cmd_type = 2; 
    int res0 = send(net_fd, &cmd_type, sizeof(int), 0);
    ERROR_CHECK(res0, -1, "send cmd_type");

    //打开目标文件流
    int file_fd = open(path, O_RDONLY);

    //从文件路径中切割得到文件名
    char *token = strtok(path, "/");
    char file_name[1024];
    while(token != NULL){
        bzero(file_name, sizeof(file_name));
        strcpy(file_name, token);
        token = strtok(NULL, "/");
    }

    //发送文件名长度
    int name_length = strlen(file_name);
    int res1 = send(net_fd, &name_length, sizeof(int), 0);
    ERROR_CHECK(res1, -1, "send name_length");

    //发送文件名
    int res2 = send(net_fd, file_name, strlen(file_name), 0);
    ERROR_CHECK(res2, -1, "send file_name");
    //获得文件长度
    int file_length = lseek(file_fd, 0, SEEK_END);
    //发送文件长度
    int res3 = send(net_fd, &file_length, sizeof(int), 0);
    ERROR_CHECK(res3, -1, "send file_length");

    char buf[1024];
    //将文件按长度读入缓冲区
    int res_read = read(file_fd, buf, file_length);

    //将缓冲区内容按长度发送给服务器
    int res4 = send(net_fd, buf, file_length, 0);
    ERROR_CHECK(res4, -1, "send file");

    close(file_fd);
}