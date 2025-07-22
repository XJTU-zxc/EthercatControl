#include "util.h"

#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// 清空字符串
void CStringClear(cstr s)
{
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    sh->free = sh->free + sh->len; // 恢复剩余空间
    sh->len = 0;                   // 重置长度为 0
    sh->buf[0] = '\0';             // 确保字符串以 '\0' 结尾
}
/**
 * 将 string 转换成 int，成功返回1，失败返回0
 * */
int String2Int(const char *s, uint16_t slen, int32_t *ii)
{
    int flag = 1;
    int32_t cur = 0;
    int32_t res = 0;

    if (cur < slen && s[cur] == '-')
    {
        flag = 0;
        cur++;
    }

    while (cur < slen && isdigit(s[cur]))
    {
        int32_t num = s[cur] - '0';
        if (res > LONG_MAX / 10 || (res == LONG_MAX / 10 && num > LONG_MAX % 10))
        {
            return 0; // 溢出返回 0
        }

        res = res * 10 + num;
        cur++;
    }

    if (cur < slen)
    {
        return 0;
    }

    res = flag == 1 ? res : -res;
    *ii = res;

    return 1;
}

int Int2String(char *s, int val)
{
    char buf[20] = {0};
    int len = 0;
    // ltoa(val, buf, 10);
    sprintf(buf, "%d", val);
    len = strlen(buf);
    memcpy(s, buf, len);

    return len;
}

/**
 * 用一个已经分配好内存的数组初始化 CString
 */
cstr CStringInit(char *p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct CString))
    {
        return NULL;
    }

    struct CString *sh = (struct CString *)p;
    sh->len = 0;
    sh->free = size - sizeof(struct CString) - 1; // -1 是因为字符串需要以 \0 结尾

    return sh->buf;
}

/**
 * 获取字符串长度
 */
uint16_t CStringGetLen(const cstr s)
{
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    return sh->len;
}

/**
 * 获取字符串空闲空间长度
 */
uint16_t CStringGetFree(const cstr s)
{
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    return sh->free;
}

/**
 * 增加 len 大小
 * */
void CStringIncrLen(cstr s, uint16_t len)
{
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    if (sh->free < len)
    {
        return;
    }
    sh->len += len;
    sh->free -= len;
}

/**
 * 拼接
 * */
int CStringCat(cstr s, const char *des, int len)
{
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    if (sh->free < len)
    {
        return -1;
    }

    memcpy(s + sh->len, des, len);
    sh->len += len;
    sh->free -= len;
    sh->buf[sh->len] = 0;

    return 0;
}

/**
 * 查找
 * */
const char *CStringFindChar(cstr s, char c)
{
    uint16_t i;
    uint16_t len = CStringGetLen(s);
    for (i = 0; i < len; ++i)
    {
        if (s[i] == c)
        {
            return s + i;
        }
    }

    return NULL;
}

/**
 * 保留字符串选定范围内的数据
 * */
int CStringRange(cstr s, uint16_t start, uint16_t len)
{
    uint32_t alen;
    struct CString *sh = (struct CString *)(s - sizeof(struct CString));
    if (len == 0)
    {
        sh->free += sh->len;
        sh->len = len;
        sh->buf[len] = 0;
        return 0;
    }

    if (start > sh->len || start + len > sh->len)
    {
        return -1;
    }

    alen = sh->free + sh->len;
    memmove(sh->buf, sh->buf + start, len);
    sh->len = len;
    sh->free = alen - len;
    sh->buf[len] = 0;

    return 0;
}

int CStringAddRespOk(cstr s)
{
    int len = CStringGetLen(s);
    int free = CStringGetFree(s);
    if (free < 8)
    {
        return -1;
    }
    s[len++] = '$';
    s[len++] = '2';
    s[len++] = '\r';
    s[len++] = '\n';
    s[len++] = 'o';
    s[len++] = 'k';
    s[len++] = '\r';
    s[len++] = '\n';
    CStringIncrLen(s, 8);
    s[len] = 0;

    return 0;
}

int CStringAddRespFail(cstr s)
{
    int len = CStringGetLen(s);
    int free = CStringGetFree(s);
    if (free < 10)
    {
        return -1;
    }
    s[len++] = '$';
    s[len++] = '4';
    s[len++] = '\r';
    s[len++] = '\n';
    s[len++] = 'f';
    s[len++] = 'a';
    s[len++] = 'i';
    s[len++] = 'l';
    s[len++] = '\r';
    s[len++] = '\n';
    CStringIncrLen(s, 10);
    s[len] = 0;

    return 0;
}

/* 给上位机返回一条ct指令，继续进行程序下传wyf */
int CStringAddRespCt(cstr s)
{
    int len = CStringGetLen(s);
    int free = CStringGetFree(s);
    if (free < 8)
    {
        return -1;
    }
    s[len++] = '$';
    s[len++] = '2';
    s[len++] = '\r';
    s[len++] = '\n';
    s[len++] = 'c';
    s[len++] = 't';
    s[len++] = '\r';
    s[len++] = '\n';
    CStringIncrLen(s, 8);
    s[len] = 0;

    return 0;
}

int CStringAddRespNumber(cstr s, int num)
{
    int len = CStringGetLen(s);
    int free = CStringGetFree(s);
    char buf[12] = {0};
    /* num 的长度 */
    int nlen = 0;
    /* 将 num 转换到 buf 中 */
    // ltoa(num, buf, 10);
    sprintf(buf, "%d", num);
    nlen = strlen(buf);

    /* 如果空闲空间不足，返回 -1 */
    if (free < nlen + 3)
    {
        return -1;
    }

    s[len++] = ':';
    /* 将 buf 中的内容 copy 到 s 中 */
    memcpy(s + len, buf, nlen);
    len += nlen;
    s[len++] = '\r';
    s[len++] = '\n';

    /* 更新字符串长度 */
    CStringIncrLen(s, nlen + 3);
    s[len] = 0;

    return 0;
}

int CStringAddRespString(cstr s, const char *str, int slen)
{
    /* 原始长度 */
    int ilen = CStringGetLen(s);
    /* 最终长度 */
    int len = ilen;
    int free = CStringGetFree(s);

    if (free < 5 + slen + 2)
    {
        return -1;
    }

    s[len++] = '$';
    // ltoa(slen, s+len, 10);
    sprintf(s + len, "%d", slen);
    len += strlen(s + len);
    s[len++] = '\r';
    s[len++] = '\n';
    memcpy(s + len, str, slen);
    len += slen;
    s[len++] = '\r';
    s[len++] = '\n';

    CStringIncrLen(s, len - ilen);
    s[len] = 0;

    return 0;
}

int CStringAddRespInt32Array(cstr s, int32_t *array, int len)
{
    int str_len, i;
    char buf[15];

    str_len = sprintf(s + CStringGetLen(s), "*%d\r\n", len);
    CStringIncrLen(s, str_len);

    for (i = 0; i < len; ++i)
    {
        // ltoa(array[i], buf, 10);
        sprintf(buf, "%ld", array[i]);
        str_len = sprintf(s + CStringGetLen(s), ":%s\r\n", buf);
        CStringIncrLen(s, str_len);
    }

    return 0;
}

int CStringAddRespUint16Array(cstr s, uint16_t *array, int len)
{
    int str_len, i;
    char buf[15];

    str_len = sprintf(s + CStringGetLen(s), "*%d\r\n", len);
    CStringIncrLen(s, str_len);

    for (i = 0; i < len; ++i)
    {
        // ltoa(array[i], buf, 10);
        sprintf(buf, "%ld", array[i]);
        str_len = sprintf(s + CStringGetLen(s), ":%s\r\n", buf);
        CStringIncrLen(s, str_len);
    }

    return 0;
}

/**
 * 初始化循环队列
 * */
struct CircularQueue *CQInit(const char *p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct CircularQueue))
    {
        return NULL;
    }

    struct CircularQueue *sh = (struct CircularQueue *)p;
    sh->head = 0;
    sh->len = 0;
    sh->free = (size - sizeof(struct CircularQueue)) / sizeof(int);

    return sh;
}

/**
 * 放入循环队列队尾
 * */
int CQPushBack(struct CircularQueue *cq, int value)
{
    if (cq->free == 0)
    {
        return -1;
    }

    int idx = (cq->head + cq->len) % (cq->len + cq->free);
    cq->ary[idx] = value;
    ++cq->len;
    --cq->free;

    return 0;
}

/**
 * 放入循环队列队首
 * */
int CQPushFront(struct CircularQueue *cq, int value)
{
    if (cq->free == 0)
    {
        return -1;
    }

    int alen = cq->len + cq->free;
    cq->head = (cq->head - 1 + alen) % alen;
    cq->ary[cq->head] = value;
    ++cq->len;
    --cq->free;

    return 0;
}

/**
 * 出队
 * */
int CQPopFront(struct CircularQueue *cq)
{
    if (cq->len == 0)
    {
        return 0;
    }

    int front = cq->ary[cq->head];
    cq->head = (cq->head + 1) % (cq->len + cq->free);
    --cq->len;
    ++cq->free;

    return front;
}

/**
 * 出队
 * */
int CQPopBack(struct CircularQueue *cq)
{
    if (cq->len == 0)
    {
        return 0;
    }

    int bidx = (cq->head + cq->len - 1) % (cq->len + cq->free);
    --cq->len;
    ++cq->free;

    return cq->ary[bidx];
}

int CQFront(struct CircularQueue *cq)
{
    if (cq->len == 0)
    {
        return 0;
    }
    return cq->ary[cq->head];
}

int CQBack(struct CircularQueue *cq)
{
    if (cq->len == 0)
    {
        return 0;
    }
    int bidx = (cq->head + cq->len - 1) % (cq->len + cq->free);
    return cq->ary[bidx];
}

uint16_t CQSize(struct CircularQueue *cq)
{
    return cq->len;
}

/**
 * 数组
 * */

/**
 * 初始化 ZipArray
 * */
ZipArray *ZipArrayInit(char *p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct ZipArray))
    {
        return NULL;
    }

    struct ZipArray *sh = (struct ZipArray *)p;
    if (CStringInit((char *)&sh->buf, size - (sizeof(struct ZipArray))) == NULL)
    {
        return NULL;
    }
    sh->nsize = 0;

    return sh;
}

/**
 * 清空 ZipArray
 * */
void ZipArrayClear(ZipArray *array)
{
    uint16_t alen = array->buf.free + array->buf.len;
    array->nsize = 0;
    array->buf.free = alen;
    array->buf.len = 0;
    array->buf.buf[0] = 0;
}

int ZipArrayPushElement(ZipArray *array, int encode, void *data, uint16_t esize)
{
    uint16_t rlen = sizeof(ZipData) + esize;
    ;

    // 1 查看剩余容量
    if (array->buf.free < rlen)
    {
        return -1;
    }

    // 2 将元素放到末尾
    ZipData *zd = (ZipData *)(array->buf.buf + array->buf.len);
    zd->encode = encode;
    memcpy(zd->buf, data, esize);

    array->buf.len += rlen;
    array->buf.free -= rlen;
    ++array->nsize;

    return 0;
}

int ZipArrayPushInt16(ZipArray *array, int16_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_INT16, &value, sizeof(int16_t));
}

int ZipArrayPushUInt16(ZipArray *array, uint16_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_UINT16, &value, sizeof(uint16_t));
}

int ZipArrayPushInt32(ZipArray *array, int32_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_INT32, &value, sizeof(int32_t));
}

int ZipArrayPushUInt32(ZipArray *array, uint32_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_UINT32, &value, sizeof(uint32_t));
}

int ZipArrayPushString(ZipArray *array, const char *s, uint16_t len)
{
    ZipData *zd;
    ZipArrayString *zsh;
    uint16_t rlen = sizeof(ZipData) + sizeof(ZipArrayString) + len + 1;
    // 1 查看剩余容量
    if (array->buf.free < rlen)
    {
        return -1;
    }

    // 2 ZipData
    zd = (ZipData *)(array->buf.buf + array->buf.len);
    zd->encode = ZD_ENCODE_STRING;

    // 3 ZipArrayString
    zsh = (ZipArrayString *)zd->buf;
    zsh->len = len;
    memcpy(zsh->buf, s, len);
    zsh->buf[len] = 0;

    // 3 ZipArray
    array->buf.len += rlen;
    array->buf.free -= rlen;
    ++array->nsize;

    return 0;
}

ZipArrayIterator ZipArrayGetIterator(ZipArray *array)
{
    ZipArrayIterator iter;
    iter.next = (ZipData *)array->buf.buf;
    iter.nsize = array->nsize;
    return iter;
}

ZipData *ZipArrayNext(ZipArrayIterator *iter)
{
    ZipData *next = iter->next;
    if (iter->nsize == 0)
    {
        return NULL;
    }

    if (iter->next->encode != ZD_ENCODE_STRING)
    {
        iter->next = (ZipData *)(iter->next->buf + iter->next->encode / 2);
    }
    else
    {
        ZipArrayString *zs = (ZipArrayString *)iter->next->buf;
        iter->next = (ZipData *)(iter->next->buf + sizeof(ZipArrayString) + zs->len + 1); // 字符串末尾有 \0
    }

    --iter->nsize;

    return next;
}

/*
 * 将收到的命令处理并缓存
 * @Param from CNCdata数组
 * @Param to BCPBuffer数组
 * @Param fromLen CNCdata里的命令长度
 * @Param toLen BCPBuffer需要保存的数据长度
 * @Param addr flash下一条命令的地址
 * @Param BCPBufferPtr 当前BCPBuffer缓冲区的写位置
 * */
int packArray(uint16_t *from, uint16_t *to, uint16_t fromLen, uint16_t toLen, uint32_t addr, uint16_t *BCPBufferPtr)
{
    int i;
    if (fromLen + 3 != toLen)
    {
        return -1;
    }
    // 1字节命令长度
    to[(*BCPBufferPtr)++] = fromLen;
    // fromLen字节内容
    for (i = 0; i < fromLen; i++)
    {
        to[(*BCPBufferPtr)++] = from[i];
    }
    // 2字节地址
    uint16_t low = addr & 0xFFFF;
    uint16_t high = (addr >> 16) & 0xFFFF;
    to[(*BCPBufferPtr)++] = low;
    to[(*BCPBufferPtr)++] = high;

    return 0;
}

/* 封装程序名 */
// fromLen 已经包含了1字节名字长度length，length字节内容,fromLen-1-length字节地址（10进制的string）
// 因此只需要将这个写进buffer中加上下一次发过来的计算出来的首地址
int dealProgName(uint16_t *from, uint16_t *to, uint16_t fromLen, uint32_t addr, uint16_t *BCPBufferPtr)
{
    if (fromLen < 2)
    {
        // 没有名字传下来
        return -1;
    }
    int idx = 0;
    uint16_t ProgLen = 0;
    // 读取名字长度
    ProgLen = from[idx];
    // 1字节名字长度
    to[(*BCPBufferPtr)++] = ProgLen;
    // ProgLen字节程序名字
    for (idx = 1; idx <= ProgLen; idx++)
    {
        to[(*BCPBufferPtr)++] = from[idx];
    }
    // 程序名下一条程序的地址
    uint32_t ProgAddr = 0;
    for (; idx < fromLen; idx++)
    {
        ProgAddr = ProgAddr * 10 + (from[idx] - 48);
    }
    // 2字节地址
    uint16_t low = ProgAddr & 0xFFFF;
    uint16_t high = (ProgAddr >> 16) & 0xFFFF;
    to[(*BCPBufferPtr)++] = low;
    to[(*BCPBufferPtr)++] = high;

    low = addr & 0xFFFF;
    high = (addr >> 16) & 0xFFFF;
    to[(*BCPBufferPtr)++] = low;
    to[(*BCPBufferPtr)++] = high;
    return 0;
}

/* 处理传下来的程序名数量+ 读程序的地址  */
/* 使用三个地址 0x98000-0x98002 */
int dealProgAddr(uint16_t *from, uint16_t *to, uint16_t fromLen, uint16_t *BCPBufferPtr)
{
    /* fromlen 最小是 1字节程序名数量 + '622592'字符串长度 */
    if (fromLen < 6)
    {
        return -1;
    }
    uint32_t addr = 0;
    int i;
    for (i = 0; i < fromLen; i++)
    {
        addr = addr * 10 + (from[i] - 48); // acsii 48 = '0'
    }
    // 2字节地址
    uint16_t low = addr & 0xFFFF;
    uint16_t high = (addr >> 16) & 0xFFFF;
    to[(*BCPBufferPtr)++] = low;
    to[(*BCPBufferPtr)++] = high;

    return 0;
}

int dealProgNum(uint16_t *from, uint16_t *to, uint16_t fromLen, uint16_t *BCPBufferPtr)
{

    uint16_t num = 0;
    int i;
    for (i = 0; i < fromLen; i++)
    {
        num = num * 10 + (from[i] - 48); // acsii 48 = '0'
    }
    to[(*BCPBufferPtr)++] = num;
    return 0;
}

int PersistBPM()
{
}

/* 校验程序名 */
int checkName(char *c1, int c1Len, char *c2, int c2Len)
{
    if (c1Len != c2Len)
    {
        return -1;
    }
    int i;
    for (i = 0; i < c1Len; i++)
    {
        if (c1[i] != c2[i])
        {
            return -1;
        }
    }

    return 0;
}
