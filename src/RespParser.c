#include "RespParser.h"
#include <string.h>
#define RESP_BUFFER_MAX_SIZE 4 * 1024  // 4K

static int RespParseClient(struct RespClient* client);

/**
 * 状态转移
 * */
static int RespTransState(struct RespClient* client, char c);
static int RespProcessBulk(struct RespClient* client, const char* lineEnd);
static int RespProcessString(struct RespClient* client, const char* lineEnd);
static int RespProcessNumber(struct RespClient* client, const char* lineEnd);

int RespClientInit(struct RespClient* c)
{
    c->theBuf = NULL;
    c->nbBulk = 0;
    c->bulkStack = NULL;
    c->nbString = -1;
    c->state = RESP_STATE_NULL;
    c->rc = RESP_RC_OK;
    c->arg = NULL;
    c->ExecuteCommand = NULL;
    return 0;
}

int RespParse(struct RespClient* c)
{
    c->rc = RespParseClient(c);
    return c->rc;
}

int RespParseClient(struct RespClient* client)
{
    int rc = RESP_RC_OK;
    uint16_t len = 0, pos = 0;
    const char* newLine = NULL;

    // 检查当前状态
    if (client->rc == RESP_RC_ERROR) {
        return RESP_RC_ERROR;
    }

    while ((len = CStringGetLen(client->theBuf)) != 0) {
        cstr aBuf = client->theBuf;

        // 1 判断数据的最大大小，如果缓冲区数据超过一定大小，关闭当前套接字
        if (len > RESP_BUFFER_MAX_SIZE) {
            return RESP_RC_ERROR;
        }

        // 2 解析一行完整数据，不完整直接返回，但是之前的数据还在
        newLine = CStringFindChar(aBuf, '\r');
        if (newLine == NULL ||   // 没有检测到换行
            newLine - aBuf > len - 2)   // 没有检测到 \n
        {
            return RESP_RC_CONTINUE;
        }

        // 3 如果是初始状态，进行状态转移
        if (client->state == RESP_STATE_NULL) {
            if (RespTransState(client, aBuf[0]) == -1) {
                return RESP_RC_ERROR;
            }
        }

        // 4 处理 * 或者 $
        switch (client->state)
        {
        case RESP_STATE_BULK :
            rc = RespProcessBulk(client, newLine);
            client->state = RESP_STATE_NULL;
            break;
        case RESP_STATE_STRING_PREFIX :
            rc = RespProcessString(client, newLine);
            break;
        case RESP_STATE_NUMBER :
            rc = RespProcessNumber(client, newLine);
            break;
        default:
            return RESP_RC_ERROR;
        }

        if (rc == RESP_RC_ERROR) {
            return RESP_RC_ERROR;
        }
        else if (rc == RESP_RC_OK) {
            // 复位
            client->nbBulk = 0;
            client->nbString = -1;
            client->state = RESP_STATE_NULL;

            // 执行指令
            if (client->ExecuteCommand) {
                client->ExecuteCommand(&client->msg, client->arg);
            }
        }

        // 5 将已经处理完的字符串从缓冲区移除
        pos = newLine - client->theBuf + 2;
        CStringRange(client->theBuf, pos, len - pos);
    }

    return rc;
}

/**
 * 状态转移。成功返回0，失败返回-1
 * */
int RespTransState(struct RespClient* client, char c)
{
    switch (c)
    {
    case '*':
        if (client->nbBulk != 0) {
            --client->nbBulk;
            if (client->nbBulk > 0) {
                CQPushBack(client->bulkStack, client->nbBulk);
                client->nbBulk = 0;
            }
        }
        client->state = RESP_STATE_BULK;
        break;
    case '$':
        client->state = RESP_STATE_STRING_PREFIX;
        break;
    case ':':
        client->state = RESP_STATE_NUMBER;
        break;
    default:
        return -1;
    }

    return 0;
}

/**
 * 解析数组
 * */
int RespProcessBulk(struct RespClient* client, const char* lineEnd)
{
    // 解析数组长度
    if (client->nbBulk == 0) {
        int ii;
        int ok = String2Int(client->theBuf + 1, // 跳过开头的 '*' 符号
            lineEnd - (client->theBuf + 1), &ii);

        /* 字符串长度解析失败或者数组过长或者数组长度为负数，协议出错 */
        if (ok == 0 || ii > 1024 || ii < 0) {
            return RESP_RC_ERROR;
        }

        client->nbBulk = ii;
    }

    // 0 代表数组为空或者已经解析完毕
    if (client->nbBulk == 0) {
        return RESP_RC_OK;
    }

    return RESP_RC_CONTINUE;
}

/**
 * 解析字符串
 * */
int RespProcessString(struct RespClient* client, const char* lineEnd)
{
    // 解析字符串长度
    if (client->nbString == -1) {
        int ii;
        int ok = String2Int(client->theBuf + 1, // 跳过开头的 '$' 符号
            lineEnd - (client->theBuf + 1), &ii);

        /* 字符串长度解析失败或者字符串过长，协议出错 */
        if (ok == 0 || ii > 512*1024*1024) {
            return RESP_RC_ERROR;
        }

        client->nbString = ii;
        // -1 代表字符串不存在
        if (ii == -1) {
            return RESP_RC_OK;
        }
    }
    else {
        // 开始解析实际的数据了
        int realLen = lineEnd - client->theBuf;
        if (client->nbString != realLen) {
            return RESP_RC_ERROR;
        }

        /* 指令数据长度及指令数据 */
        client->msg.datalen = realLen;
        memcpy(client->msg.data, client->theBuf, realLen);

        if (client->nbBulk > 0) {--client->nbBulk;}

        if (client->nbBulk == 0 && CQSize(client->bulkStack) != 0) {
            client->nbBulk = CQPopBack(client->bulkStack);
        }
        else if (client->nbBulk == 0) {
            // 解析完成
            return RESP_RC_OK;
        }

        client->nbString = -1;
        client->state = RESP_STATE_NULL;
    }

    return RESP_RC_CONTINUE;
}

/**
 * 解析数字
 * */
int RespProcessNumber(struct RespClient* client, const char* lineEnd)
{
    int ii;
    int ok = String2Int(client->theBuf + 1, // 跳过开头的 '$' 符号
        lineEnd - (client->theBuf + 1), &ii);

    /* 字符串长度解析失败或者字符串过长，协议出错 */
    if (ok == 0) {
        return RESP_RC_ERROR;
    }

    /* 指令编号 */
    client->msg.code = ii;

    if (client->nbBulk > 0) {--client->nbBulk;}

    if (client->nbBulk == 0 && CQSize(client->bulkStack) != 0) {
        client->nbBulk = CQPopBack(client->bulkStack);
    }
    else if (client->nbBulk == 0) {
        // 解析完成
        return RESP_RC_OK;
    }

    client->state = RESP_STATE_NULL;

    return RESP_RC_CONTINUE;
}









