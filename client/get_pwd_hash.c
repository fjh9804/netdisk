#include "head.h"

int get_pwd_hash(char *name, char *salt, char *hash){	
	char *tmp = crypt(name, "$6$accounts$");
	
    //char salt[12] = {0};
    strcat(salt, "$6$");
    strncat(salt, tmp + 12, 8);
    strcat(salt, "$");
    printf("salt:%s\n", salt);

    //char *hash = crypt("password", salt);
    printf("hash:%s\n", hash);
	
    return 0;
}
