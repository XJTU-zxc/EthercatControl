#ifndef LIB_RESPPARSER_H_
#define LIB_RESPPARSER_H_

#include "util.h"


#ifdef __cplusplus
extern "C" {
#endif

struct RespClient;

typedef struct RespMessage {
    /* 指令类型 如0x4B */
    int code;
    /* 指令长度 */
    int datalen;
    /* 指令数据 */
    char data[80];
} RespMessage;

enum RespState {
    RESP_STATE_NULL,
    RESP_STATE_BULK,
    RESP_STATE_STRING_PREFIX,
    RESP_STATE_NUMBER,
    RESP_STATE_FAILED,
    RESP_STATE_SUCCESS
};

enum RsepRunCode {
    RESP_RC_CONTINUE, // 数据没接收完
    RESP_RC_ERROR,   // 协议出错 || 缓冲区溢出
    RESP_RC_OK    // 处理协议成功
};

/**
 * Resp 客户端
 * */
typedef struct RespClient {
    // 接收消息的缓冲区
    cstr theBuf;

    // bulk 数组长度
    int nbBulk;

    struct CircularQueue* bulkStack;  // 存放之前的 nbBulk

    // 字符串长度
    int nbString;

    // 当前解析状态
    enum RespState state;

    // 当前运行状态
    int rc;

    RespMessage msg;

    void* arg;
    void (*ExecuteCommand)(RespMessage* msg, void* arg);
} RespClient;

/**
 * 初始化 RespClient
 * */
int RespClientInit(struct RespClient* c);

/**
 * 解析 resp 协议
 * */
int RespParse(struct RespClient* c);



#ifdef __cplusplus
}
#endif

#endif /* LIB_RESPPARSER_H_ */




