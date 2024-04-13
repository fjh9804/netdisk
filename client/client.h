#include "head.h"

//定义命令参数结构体cmd_arg
typedef struct cmd_arg{
    char cmd[60];
    char path[1024]; 
}CMD;

//指令切割
void cmd_tok(struct cmd_arg*, char *buf);
//上传文件
int upload_file(char *path,  int net_fd);
//下载文件
int download_file(char *file_name, int net_fd);
//接收消息
int recv_msg(char *msg, int net_fd);