#include "head.h"

//定义命令参数结构体cmd_arg
typedef struct cmd_arg{
    char cmd[60];
    char path[1024]; 
}CMD;

void cmd_tok(struct cmd_arg*, char *buf);
int upload_file(char *path,  int net_fd);
int download_file(char *file_name, int net_fd);
int recv_msg(char *msg, int net_fd);

int main()
{
    char *ip = "192.168.201.128";
    char *port = "8080";

    //初始化socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    //构建struct sockaddr_in类型
    struct sockaddr_in socketaddr;
    socketaddr.sin_family = AF_INET;
    socketaddr.sin_addr.s_addr = inet_addr(ip);
    socketaddr.sin_port = htons(atoi(port));

    //向服务器发起连接请求
    //int res_connect = connect(socket_fd, (struct sockaddr *)&socketaddr, sizeof(socketaddr));
    //ERROR_CHECK(res_connect, -1, "connect");


    while(1){
        //获取标准输入
        char buf[60] = {0};
        int res_read = read(STDIN_FILENO, buf, sizeof(buf));
        if(res_read == 0){
            printf("failed in get cmd from stdin!\n");
        }
        //如果用户选择退出
        if(strcmp(buf, "exit\n") == 0){
            break;
        }
        printf("\n");
        //切割命令
        struct cmd_arg *Cmd = (struct cmd_arg*)calloc(1, sizeof(struct cmd_arg));
        cmd_tok(Cmd, buf);

        printf("Cmd->path:%s\n", Cmd->path);

        //创建用于接收服务器回复消息的字符数组
        char msg[4096] = {0};
        //根据具体命令，进行对应操作
        if(strcmp(Cmd->cmd, "cd") == 0){
            int cmd_type = 0;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            recv_msg(msg, socket_fd);
        }else if(strcmp(Cmd->cmd, "ls") == 0){
            int cmd_type = 1;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            recv_msg(msg, socket_fd);
        }else if(strcmp(Cmd->cmd, "upload") == 0){
            upload_file(Cmd->path, socket_fd);
        }else if(strcmp(Cmd->cmd, "download") == 0){
            download_file(Cmd->path, socket_fd);
        }else if(strcmp(Cmd->cmd, "rm") == 0){
            int cmd_type = 3;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            recv_msg(msg, socket_fd);
        }else if(strcmp(Cmd->cmd, "pwd") == 0){
            int cmd_type = 4;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            recv_msg(msg, socket_fd);
        }else if(strcmp(Cmd->cmd, "mkdir") == 0){
            int cmd_type = 4;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            recv_msg(msg, socket_fd);
        }else{
            printf("Command invalid!\n");
        }
    }

    close(socket_fd);
    return 0;
}

//对输入的命令参数进行切割分离
void cmd_tok(struct cmd_arg *Cmd ,char *buf){
    bzero(Cmd->path, sizeof(Cmd->path));
    char *tmp = strtok(buf, " ");
    strcpy(Cmd->cmd, tmp);
    printf("cmd:%s\n", Cmd->cmd);

    char *token = strtok(NULL, " ");
    while (token != NULL) {
        strcat(Cmd->path, token);
        token = strtok(NULL, " ");
    }
    printf("path:%s\n", Cmd->path); 
    bzero(buf, sizeof(buf));
}

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

//接收服务器返回的一般消息
int recv_msg(char *msg, int net_fd){
    int res = recv(net_fd, msg, sizeof(msg), 0);
    ERROR_CHECK(res, -1, "recv msg");

    bzero(msg, sizeof(msg));
}
