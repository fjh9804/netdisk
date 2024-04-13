#include "client.h"

//处理download命令
int download_file(char *file_name, int net_fd){
    //首先发送给服务器指令类型为2
    int cmd_type = 3;
    int res0 = send(net_fd, &cmd_type, sizeof(int), 0);
    ERROR_CHECK(res0, -1, "send cmd_type");

    char name[1024] = {0};
    strcpy(name, file_name);
    int name_length = strlen(file_name);

    //发送需要下载的文件名长度
    int res1 = send(net_fd, &name_length, sizeof(int), 0);
    ERROR_CHECK(res1, -1, "send name_length");

    //发送文件名
    int res2 = send(net_fd, file_name, strlen(file_name), 0);
    ERROR_CHECK(res2, -1, "send file_name");

    //接收文件长度
    int file_length;
    int res3 = recv(net_fd, &file_length, sizeof(int), 0);
    ERROR_CHECK(res3, -1, "recv file_length");

    //根据接收到的文件长度创建文件
    int file_fd = open(file_name, O_RDWR|O_CREAT, 0666);
    if(file_fd == -1){
        perror("file create");
    }

    //接收数据到缓冲区数组
    char buf[4096] = {0};
    int res4 = recv(net_fd, buf, file_length, 0);
    ERROR_CHECK(res4, -1, "recv file");

    //将缓冲区中内容写入文件
    int res5 = write(file_fd, buf, sizeof(buf));
    ERROR_CHECK(res5, -1, "write into file");
}