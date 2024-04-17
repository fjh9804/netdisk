#include "head.h"

int getparameter(char *key, char *value){

    FILE *file = fopen("connect.ini", "r");
    while(1){
        char line[128] = {0};

        //读一行数据
        char *res = fgets(line, sizeof(line), file);
        if(res == NULL){
            char buf[] = "无对应键值\n";
            memcpy(value, buf, strlen(buf));
            return -1;
        }
        //处理数据
        char *line_key = strtok(line, "=");
        if(strcmp(key, line_key) == 0){
            //找到对应键值
            char *line_value = strtok(NULL, "=");
            memcpy(value, line_value, strlen(line_value));
            return 0;
        }
    }
}
