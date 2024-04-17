#include "head.h"

// 函数用于使用 sha1sum 命令生成文件哈希值
int get_file_hash(const char *filename, char *res, size_t res_size) {
    char cmd[1024] = {0};
    // 拼接命令
    snprintf(cmd, sizeof(cmd), "sha1sum \"%s\"", filename);

    FILE *pipe = popen(cmd, "r");
    if (pipe == NULL) {
        perror("popen");
        return -1;
    }

    size_t total_read = 0;
    while (total_read < res_size - 1 && fgets(res + total_read, res_size - total_read, pipe)) {
        total_read = strlen(res);
    }
    
    pclose(pipe);
    return 0;
}
