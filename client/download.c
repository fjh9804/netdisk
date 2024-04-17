#include "client.h"

//处理download命令
int download_file(char *file_name, int net_fd){
    //首先发送给服务器指令类型为3
    int cmd_type = 3;
    int res0 = send(net_fd, &cmd_type, sizeof(int), MSG_NOSIGNAL);
    ERROR_CHECK(res0, -1, "send cmd_type");

    char name[1024] = {0};
    strcpy(name, file_name);
    //int name_length = strlen(file_name);

    //发送需要下载的文件名长度
    //int res1 = send(net_fd, &name_length, sizeof(int), 0);
    //ERROR_CHECK(res1, -1, "send name_length");

    //发送文件名
    int res2 = send(net_fd, file_name, strlen(file_name), MSG_NOSIGNAL);
    ERROR_CHECK(res2, -1, "send file_name");

    //接收文件长度
    int file_length;
    int res3 = recv(net_fd, &file_length, sizeof(int), MSG_WAITALL);
    printf("file_length:%d\n", file_length);
    ERROR_CHECK(res3, -1, "recv file_length");

    //根据接收到的文件名创建文件
    int file_fd = open(file_name, O_RDWR|O_CREAT, 0666);
    if(file_fd == -1){
        perror("file create");
        return -1;
    }

    //获取本地该文件内容长度
    int length_in_client = lseek(file_fd, 0, SEEK_SET);
    //发送给服务器本地已下载文件长度
    int res_sd_len = send(net_fd, &length_in_client, sizeof(int), 0);
    ERROR_CHECK(res_sd_len, -1, "send length_in_client");

    if(length_in_client == 0){
        //设置文件大小，分配足够空间
        ftruncate(file_fd, file_length);
    }

    //对此次下载的总文件大小进行调整
    file_length -= length_in_client;
    //将文件起始位置偏移
    lseek(file_fd, length_in_client, SEEK_SET);

    //创建缓冲区数组
    char buf[4096] = {0};

    //循环读取接收
    int write_count = 0;
    while(write_count < file_length){
        //将接收内容写入缓冲区
        int res4 = recv(net_fd, buf, sizeof(buf), 0);
        ERROR_CHECK(res4, -1, "recv file");
        //将缓冲区中内容写入文件
        write_count += write(file_fd, buf, sizeof(buf));
    }
    
    printf("文件下载完毕，下载大小%d字节\n", file_length);
    close(file_fd);
    return 0;
}
