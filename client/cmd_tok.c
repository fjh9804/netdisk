#include "client.h"

//对输入的命令参数进行切割分离
void cmd_tok(struct cmd_arg *Cmd ,char *buf){
    bzero(Cmd->path, sizeof(Cmd->path));
    strcpy(Cmd->cmd, strtok(buf, " "));

    char *token = strtok(NULL, " ");
    while (token != NULL) {
        strcat(Cmd->path, token);
        token = strtok(NULL, " ");
    }

}
