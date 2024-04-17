#include "head.h"

//函数用于生成文件哈希值
int get_file_hash(char *filename, char *res, int res_length){
	char cmd[1024] = {0};
	//拼接命令
	strcpy(cmd, "sha1sum ");
	strcat(cmd, filename);

	FILE *pipe = popen(cmd, "r");
    if(pipe == NULL){
        printf("popen failed!\n");
    }

	fread(res, 1, res_length, pipe);
    strtok(res, " ");

    pclose(pipe);
	return 0;
}
