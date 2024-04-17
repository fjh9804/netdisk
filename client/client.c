#include "client.h"

int main(void)
{

    char ip[128] = {0};
    getparameter("ip", ip);
    printf("%s\n", ip);
    char port[32] = {0};
    getparameter("port", port);
    printf("%s\n", port);


    //初始化socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    //构建struct sockaddr_in类型
    struct sockaddr_in socketaddr;
    socketaddr.sin_family = AF_INET;
    socketaddr.sin_addr.s_addr = inet_addr(ip);
    socketaddr.sin_port = htons(atoi(port));

    //向服务器发起连接请求
    int res_connect = connect(socket_fd, (struct sockaddr *)&socketaddr, sizeof(socketaddr));
    ERROR_CHECK(res_connect, -1, "connect");

    //连接成功，进行用户名密码校验
    printf("请输入用户名\n");
    char name[60] = {0};
    char password[1024] = {0};
    char password_check[1024] = {0};
    scanf("%s", name);
    //发送用户名长度
    int name_len = strlen(name);
    int res_sd_namelen = send(socket_fd, &name_len, sizeof(int), 0);
    ERROR_CHECK(res_sd_namelen, -1, "send name_len");
    //发送用户名
    int res_sd_name = send(socket_fd, name, strlen(name), 0);
    ERROR_CHECK(res_sd_name, -1, "send name");

    int check_result = 0;

    while(1){
        printf("请输入密码:\n");
        scanf("%s", password);
        while(check_result == -1){
            printf("请再次输入密码\n");
            scanf("%s", password_check);

            if(strcmp(password, password_check) == 0){
                //设置密码成功
                break;
            }
        }
        //发送密码长度
        int pwd_len = strlen(password);
        int res_sd_pwdlen = send(socket_fd, &pwd_len, sizeof(int), 0);
        ERROR_CHECK(res_sd_pwdlen, -1, "send pwd_len");
        //发送密码
        int res_sd_pwd = send(socket_fd, password, strlen(password), 0);
        ERROR_CHECK(res_sd_pwd, -1, "send password");

        //接收密码校验结果
        int res_sd_check = recv(socket_fd, &check_result, sizeof(int), 0);
        ERROR_CHECK(res_sd_check, -1, "recv check_result");
        if(check_result == 1){
            printf("密码错误!\n");
            continue;
        }else if(check_result == 0){
            printf("密码正确!\n");
            break;
        }
        check_result = -1;
        printf("用户名未注册！\n");
        printf("账户初始化:\n");
        continue;
    }


    while(1){
        //获取标准输入
        char buf[60] = {0};
        memset(buf, 0, sizeof(buf));

        printf("请输入命令\n");
        int res_read = read(STDIN_FILENO, buf, sizeof(buf));
        if(res_read == 0){
            printf("failed in get cmd from stdin!\n");
            continue;
        }
        buf[res_read - 1] = 0;
        //如果用户选择退出
        if(strcmp(buf, "exit") == 0){
            printf("已退出网盘\n");
            break;
        }
        printf("-------------------------------------\n");
        //切割命令
        struct cmd_arg *Cmd = (struct cmd_arg*)calloc(1, sizeof(struct cmd_arg));
        cmd_tok(Cmd, buf);


        //创建用于接收服务器回复消息的字符数组
        char msg[4096] = {0};
        memset(msg, 0, sizeof(msg));
        //根据具体命令，进行对应操作
        int flag = 0;
        if(strcmp(Cmd->cmd, "cd") == 0){

            int cmd_type = 0;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            //recv_msg(msg, socket_fd);
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("%s\n", msg);
        }else if(strcmp(Cmd->cmd, "ls") == 0){

            int cmd_type = 1;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            //crecv_msg(msg, socket_fd);
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("current content:%s\n", msg);
        }else if(strcmp(Cmd->cmd, "upload") == 0){

            upload_file(Cmd->path, socket_fd);
        }else if(strcmp(Cmd->cmd, "download") == 0){

            download_file(Cmd->path, socket_fd);
        }else if(strcmp(Cmd->cmd, "rm") == 0){

            int cmd_type = 4;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            //recv_msg(msg, socket_fd);
            if((flag = recv(socket_fd, msg, sizeof(msg), 0)) != 0){
                printf("remove succeeded!\n");
            }else{
                printf("remove failed!");
            }
        }else if(strcmp(Cmd->cmd, "pwd") == 0){

            int cmd_type = 5;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            //int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            //ERROR_CHECK(res1, -1, "send path");
            //recv_msg(msg, socket_fd);
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("current path:%s\n", msg);
        }else if(strcmp(Cmd->cmd, "mkdir") == 0){

            int cmd_type = 6;
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            //recv_msg(msg, socket_fd);
            if((flag = recv(socket_fd, msg, sizeof(msg), 0)) != 0){
                printf("mkdir succeeded!\n");
            }else{
                printf("mkdir failed!\n");
            }
        }else{
            printf("Command invalid!\n");
        }
    }

    close(socket_fd);
    return 0;
}

