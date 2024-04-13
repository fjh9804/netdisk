#ifndef MY_HEADER_H
#define MY_HEADER_H

#define ROOTDIR ~/wangpan/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/sendfile.h>
#include <mysql/mysql.h>
#include "task_queue.h"
// 检查命令行参数数量是否符合预期
#define ARGS_CHECK(argc, expected) \
    do { \
        if ((argc) != (expected)) { \
            fprintf(stderr, "args error!\n"); \
            exit(1); \
        } \
    } while (0)

// 检查返回值是否是错误标记,若是则打印msg和错误信息
#define ERROR_CHECK(ret, error_flag, msg) \
    do { \
        if ((ret) == (error_flag)) { \
            perror(msg); \
            exit(1); \
        } \
    } while (0)
#define THREAD_ERROR_CHECK(ret,msg) {if(ret!=0){fprintf(stderr,"%s:%s\n",msg,strerror(ret));}}

enum{
    CD,
    LS,
    PUTS,
    GETS,
    REMOVE,
    PWD,
    MKDIR
};

typedef struct {
    pthread_t* thread_ids;//线程 id;

    int thread_num;//子线程数量;

    queue_t queue;//任务队列

    pthread_mutex_t pool_lock;

    pthread_cond_t cond;

    int exitflag;//退出标记
}pool_t;

int init_pool(pool_t *ppool, int num);//创建线程池

void* thread_main(void* p);

int init_tcp_socket(int* socket_fd, char* ip, char* port);//初始化连接

int epoll_add(int epoll_fd, int file_fd);

void remove_file(const char * path);//删除文件函数

int mkdir_file(const char * path);//创建目录函数
#endif
