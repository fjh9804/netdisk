#include "head.h"
#define THRESHOLD 1024*1024*100

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
//从配置文件中获取数据
int getparameter(char *key, char *value);
//计算文件哈希值
int get_file_hash(const char *filename, char *res, size_t res_size);
//计算密码哈希值
int get_pwd_hash(char *name, char *salt, char *hash);

