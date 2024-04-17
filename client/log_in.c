#include "head.h"

int log_in(char *token, int socket_fd){
    //连接成功，进行用户名密码校验

    //先发送token
    int sd_tk = send(socket_fd, token, sizeof(token), 0);
    ERROR_CHECK(sd_tk, -1, "send token");
    //接收验证信息
    int flag = 1;
    int res_flag = recv(socket_fd, &flag, sizeof(int), 0);
    ERROR_CHECK(res_flag, -1, "recv flag");

    //如果验证未通过,进行登录或注册
    if(flag == 1){
        
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
                //接收有效的token
                int res_tk = recv(socket_fd, token, sizeof(token), 0);
                ERROR_CHECK(res_tk, -1, "recv correct token");
                
                printf("密码正确!\n");
                break;
            }else if(check_result == 2){
                //注册成功
                printf("用户注册成功!\n");
                break;
            }
            check_result = -1;
            printf("用户名未注册！\n");
            printf("账户初始化:\n");
            continue;
        }    
    }
    //验证通过，直接跳过登录

    return 0;
}

