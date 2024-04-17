#include "client.h"

//定义记录当前子线程数的全局变量
static int pid_count = 0;

struct info_pack{
    //token用于登录验证
    char *token;
    //区分上传还是下载
    int cmd_type;
    //文件路径
    char *file_path;
};

//子线程入口函数
void *thread_main(void *p){
    //指针类型转换
    struct info_pack *pack = (struct info_pack *)p;
    printf("结构体传入\n");

    //先进行连接
    int thread_sock_fd;
    tcp_init(&thread_sock_fd);

    //连接成功，进行用户名密码校验
    log_in(pack->token, thread_sock_fd);

    //进行上传或下载
    switch(pack->cmd_type){
    case 0:
        upload_file(pack->file_path, thread_sock_fd);
        pack->file_path = NULL;
        pid_count--;
        break;
    case 1:
        download_file(pack->file_path, thread_sock_fd);
        pack->file_path = NULL;
        pid_count--;
        break;
    default :
        printf("Incorrect cmd into thread!\n");
    }
    pthread_exit(NULL);
}


int main(void)
{
    //初始化socket
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    tcp_init(&socket_fd);

    //连接成功，进行用户名密码校验
    
    //定义字符数组token用于接收正确token
    char token[2048] = {0};
    log_in(token, socket_fd);

    //创建struct info_pack结构体
    struct info_pack *pack = (struct info_pack *)calloc(1, sizeof(struct info_pack));
    pack->token = token;

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
            break;
        }
        printf("\n--------------------------------------------------------------------------\n");
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
            
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("%s\n", msg);

        }else if(strcmp(Cmd->cmd, "ls") == 0){

            int cmd_type = 1;
            
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("current content:%s\n", msg);

        }else if(strcmp(Cmd->cmd, "upload") == 0){
            
            //修改pack结构体信息
            pack->cmd_type = 0;
            pack->file_path = Cmd->cmd;

            //创建子线程
            pthread_t *pthread_id = (pthread_t *)calloc(1, sizeof(pthread_t));
            pthread_create(pthread_id, NULL, thread_main, pack);
            pid_count++;

        }else if(strcmp(Cmd->cmd, "download") == 0){
            
            //修改pack结构体信息
            pack->cmd_type = 1;
            pack->file_path = Cmd->cmd;

            //创建子线程
            pthread_t *pthread_id = (pthread_t *)calloc(1, sizeof(pthread_t));
            pthread_create(pthread_id, NULL, thread_main, pack);
            pid_count++;

        }else if(strcmp(Cmd->cmd, "rm") == 0){

            int cmd_type = 4;
            
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            
            if((flag = recv(socket_fd, msg, sizeof(msg), 0)) != 0){
                printf("remove succeeded!\n");
            }else{
                printf("remove failed!");
            }

        }else if(strcmp(Cmd->cmd, "pwd") == 0){

            int cmd_type = 5;
            
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            
            recv(socket_fd, msg, sizeof(msg), 0);
            printf("current path:%s\n", msg);

        }else if(strcmp(Cmd->cmd, "mkdir") == 0){

            int cmd_type = 6;
            
            int res0 = send(socket_fd, &cmd_type, sizeof(int), 0);
            ERROR_CHECK(res0, -1, "send cmd_type");
            
            int res1 = send(socket_fd, Cmd->path, sizeof(Cmd->path), 0);
            ERROR_CHECK(res1, -1, "send path");
            
            if((flag = recv(socket_fd, msg, sizeof(msg), 0)) != 0){
                printf("mkdir succeeded!\n");
            }else{
                printf("mkdir failed!\n");
            }

        }else if(strcmp(Cmd->cmd, "logout") == 0){
            //To Do:
            //
            //
            //
            //
            //
            //
        }else{
            printf("Command invalid!\n");
        }
    }
    
    //用户选择退出
    close(socket_fd);
    
    //等待所有子进程退出
    if(pid_count > 0){
        printf("当前有传输任务正在执行，是否继续退出?(y/n):");
        char opt;
        scanf("%c", &opt);
        if(opt != 'y'){
            printf("任务结束后将自动退出\n");
            while(pid_count > 0);
        }
    }
    printf("已退出网盘\n");
    return 0;
}

