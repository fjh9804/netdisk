#include "client.h"

//处理upload命令
int upload_file(char *path, int net_fd){
    //首先发送给服务器指令类型为2
    int cmd_type = 2; 
    int res0 = send(net_fd, &cmd_type, sizeof(int), MSG_NOSIGNAL);
    ERROR_CHECK(res0, -1, "send cmd_type");

    //打开目标文件流
    int file_fd = open(path, O_RDWR);
    if(file_fd == -1){
        perror("open");
        return -1;
    }

    //计算文件哈希值
    char hash[1024] = {0};
    get_file_hash(path, hash, sizeof(hash));
    
    //发送哈希值长度
    int hash_len = strlen(hash);
    int sd_hash_len = send(net_fd, &hash_len, sizeof(int), MSG_NOSIGNAL);
    ERROR_CHECK(sd_hash_len, -1, "send hash_len");

    //发送哈希值
    int sd_hash = send(net_fd, hash, hash_len, MSG_NOSIGNAL);
    ERROR_CHECK(sd_hash, -1, "send hash");

    //从文件路径中切割得到文件名
    char *token = strtok(path, "/");
    char file_name[1024];
    while(token != NULL){
        bzero(file_name, sizeof(file_name));
        strcpy(file_name, token);
        token = strtok(NULL, "/");
    }

    //发送文件名长度
    int name_length = strlen(file_name);
    int res1 = send(net_fd, &name_length, sizeof(int), MSG_NOSIGNAL);
    ERROR_CHECK(res1, -1, "send name_length");

    //发送文件名
    int res2 = send(net_fd, file_name, strlen(file_name), MSG_NOSIGNAL);
    ERROR_CHECK(res2, -1, "send file_name");
    
    int length_in_server;

    //获得文件长度
    off_t file_length = lseek(file_fd, 0, SEEK_END);

    //发送文件长度
    int res3 = send(net_fd, &file_length, sizeof(off_t), MSG_NOSIGNAL);
    ERROR_CHECK(res3, -1, "send file_length");

    //获取服务器上已上传的大小
    int res_length = recv(net_fd, &length_in_server, sizeof(int), MSG_NOSIGNAL);
    ERROR_CHECK(res_length, -1, "recv length_in_server");
    
    //判断是否完整
    if(file_length == length_in_server){
        //文件完整则不需要再次上传
        return 0;
    }

    //对此次传输的总文件大小进行调整
    file_length -= length_in_server;
    //将文件起始位置偏移
    lseek(file_fd, length_in_server, SEEK_SET);


    char buf[4096];
	//如果文件大小小于100MB
	if(file_length < THRESHOLD){
        
        //循环读取发送
		int read_count = 0;
		while(read_count < file_length){
			int read_once = read(file_fd, buf, sizeof(buf));
            read_count += read_once;
            //发送本次循环发送的长度
            int res_len = send(net_fd, &read_once, sizeof(int), MSG_NOSIGNAL);
            ERROR_CHECK(res_len, -1, "send lenth this round");

			//将缓冲区内容按长度发送给服务器
			int res4 = send(net_fd, buf, sizeof(buf), MSG_NOSIGNAL);
			ERROR_CHECK(res4, -1, "send file");
		}
		
	}else{
		//文件大小大于100MB时，使用mmap传输
        
		off_t offset = 0;
		off_t cp_one_round;
		while(offset < file_length){
		
		//确定本轮映射的字节长度
		if(offset + sizeof(buf) > file_length){
			cp_one_round = file_length - offset;
		}else{
			cp_one_round = sizeof(buf);
		}

        //发送本轮发送的大小
        int res_len_mmap = send(net_fd, &cp_one_round, sizeof(off_t), MSG_NOSIGNAL);

		char *tmp = (char *)mmap(NULL, cp_one_round, PROT_READ|PROT_WRITE, MAP_SHARED, 
		file_fd, offset);
        if(tmp == MAP_FAILED){
            perror("mmap");
            close(file_fd);
            return -1;
        }

		int res5 = send(net_fd, tmp, cp_one_round, MSG_NOSIGNAL);
		ERROR_CHECK(res5, -1, "send file");
        //解除映射
        int res_unmap = munmap(tmp, cp_one_round);
        ERROR_CHECK(res_unmap, -1, "munmap");

        //累加偏移量
		offset += cp_one_round;
		}
	}
	
	printf("文件上传完毕，上传大小%ld字节\n", file_length);
    close(file_fd);
}
