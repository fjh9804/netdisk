#include "head.h"

int get_pwd_hash(char *name, char *password, char *salt, char *hash){	
    char *tmp = crypt(name, "$6$accounts$");

    if(strlen(salt) == 0){
        strcat(salt, "$6$");
        strncat(salt, tmp + 12, 8);
        strcat(salt, "$");
        printf("salt:%s\n", salt);
    }
    strcat(hash, crypt("password", salt));
    printf("hash:%s\n", hash);

    return 0;
}
