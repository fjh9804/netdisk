//#include "head.h"
//#include <my_headfile.h>
#include <l8w8jwt/encode.h>
#include <l8w8jwt/decode.h>
#include <l8w8jwt/algs.h>
#include <l8w8jwt/base64.h>
#include <l8w8jwt/claim.h>
#include <l8w8jwt/retcodes.h>
#include <l8w8jwt/util.h>
#include <l8w8jwt/version.h>

//传入客户端发送的token,  name和flag为传入传出参数,name表示解密得到的用户名,flag为校验结果标志位
int de_token(char *token, int *flag, char *name){
    //初始化解码参数结构体
    struct l8w8jwt_decoding_params params;
    l8w8jwt_decoding_params_init(&params);

    //设置JWT使用的算法，这里使用HS512
    params.alg = L8W8JWT_ALG_HS512;

    //token值
    char *str = token;
    printf("接收到token:%s\n", token);
    //填入token字符串和长度
    params.jwt = (char *)str;
    params.jwt_length = strlen(str);

    //设置密钥
    char *key = "server string token key";
    params.verification_key = (unsigned char *)key;
    params.verification_key_length = strlen(key);

    //定义：接受结果的指针和变量
    struct l8w8jwt_claim *claims = NULL;
    size_t claim_count = 0;
    enum l8w8jwt_validation_result validation_result;

    //解码：获得token中存储的信息
    int decode_result = l8w8jwt_decode(&params, &validation_result, &claims, &claim_count);
    //判断是否是正确的token
    if(decode_result == L8W8JWT_SUCCESS && validation_result == L8W8JWT_VALID){
        printf("Token validation successful!\n");
        //0表示正确
        *flag = 0;
        //打印token中解码出来的荷载信息
        for(size_t i = 0; i < claim_count; i++){
            printf("Claim [%zu]: %s = %s\n", i, claims[i].key, claims[i].value);
            if(i == 5){
                strcpy(name, claims[i].value);
            }
        }
    }else{
        printf("Token validation failed!\n");
        //1表示错误
        *flag = 1;
    }

    l8w8jwt_free_claims(claims, claim_count);

    return 0;
}
