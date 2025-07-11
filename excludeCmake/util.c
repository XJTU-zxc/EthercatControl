#include "util.h"

#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * 将 string 转换成 int，成功返回1，失败返回0
 * */
int String2Int(const char* s, uint16_t slen, int* ii)
{
    int flag = 1;
    int cur = 0;
    int res = 0;

    if (cur < slen && s[cur] == '-') {
        flag = 0;
        cur++;
    }

    while (cur < slen && isdigit(s[cur])) {
        int num = s[cur] - '0';
        if (res > INT_MAX / 10 || (res == INT_MAX / 10 && num > INT_MAX % 10)) {
            return 0;   // 溢出返回 0
        }

        res = res * 10 + num;
        cur++;
    }

    if (cur < slen) {
        return 0;
    }

    res = flag == 1? res : -res;
    *ii = res;

    return 1;
}

/**
 * 用一个已经分配好内存的数组初始化 CString
 */
cstr CStringInit(char* p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct CString)) {return NULL;}

    struct CString* sh = (struct CString*)p;
    sh->len = 0;
    sh->free = size - sizeof(struct CString) - 1;   // -1 是因为字符串需要以 \0 结尾

    return sh->buf;
}

/**
 * 获取字符串长度
 */
uint16_t CStringGetLen(const cstr s)
{
    struct CString* sh = (struct CString*)(s - sizeof(struct CString));
    return sh->len;
}

/**
 * 增加 len 大小
 * */
void CStringIncrLen(cstr s, uint16_t len)
{
    struct CString* sh = (struct CString*)(s - sizeof(struct CString));
    sh->len += len;
    sh->free -= len;
}

/**
 * 拼接
 * */
int CStringCat(cstr s, const char* des)
{
    uint16_t len = strlen(des);
    struct CString* sh = (struct CString*)(s - sizeof(struct CString));
    if (sh->free < len) {return -1;}

    memcpy(s+sh->len, des, len);
    sh->len += len;
    sh->free -= len;
    sh->buf[sh->len] = 0;

    return 0;
}

/**
 * 查找
 * */
const char* CStringFindChar(cstr s, char c)
{
    uint16_t i;
    uint16_t len = CStringGetLen(s);
    for (i = 0; i < len; ++i) {
        if (s[i] == c) {
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
    struct CString* sh = (struct CString*)(s - sizeof(struct CString));

    if (start > sh->len || start + len > sh->len) {return -1;}

    alen = sh->free + sh->len;
    memmove(sh->buf, sh->buf + start, len);
    sh->len = len;
    sh->free = alen - len;
    sh->buf[len] = 0;

    return 0;
}

/**
 * 初始化循环队列
 * */
struct CircularQueue* CQInit(const char* p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct CircularQueue)) {return NULL;}

    struct CircularQueue* sh = (struct CircularQueue*)p;
    sh->head = 0;
    sh->len = 0;
    sh->free = (size - sizeof(struct CircularQueue)) / sizeof(int);

    return sh;
}

/**
 * 放入循环队列队尾
 * */
int CQPushBack(struct CircularQueue* cq, int value)
{
    if (cq->free == 0) {
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
int CQPushFront(struct CircularQueue* cq, int value)
{
    if (cq->free == 0) {
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
int CQPopFront(struct CircularQueue* cq)
{
    if (cq->len == 0) {return 0;}

    int front = cq->ary[cq->head];
    cq->head = (cq->head + 1) % (cq->len + cq->free);
    --cq->len;
    ++cq->free;

    return front;
}

/**
 * 出队
 * */
int CQPopBack(struct CircularQueue* cq)
{
    if (cq->len == 0) {return 0;}

    int bidx = (cq->head + cq->len - 1) % (cq->len + cq->free);
    --cq->len;
    ++cq->free;

    return cq->ary[bidx];
}

int CQFront(struct CircularQueue* cq)
{
    if (cq->len == 0) {return 0;}
    return cq->ary[cq->head];
}

int CQBack(struct CircularQueue* cq)
{
    if (cq->len == 0) {return 0;}
    int bidx = (cq->head + cq->len - 1) % (cq->len + cq->free);
    return cq->ary[bidx];
}

uint16_t CQSize(struct CircularQueue* cq)
{
    return cq->len;
}

/**
 * 数组
 * */

/**
 * 初始化 ZipArray
 * */
ZipArray* ZipArrayInit(char* p, uint16_t size)
{
    if (p == NULL || size < sizeof(struct ZipArray)) {return NULL;}

    struct ZipArray* sh = (struct ZipArray*)p;
    if (CStringInit((char*)&sh->buf, size - (sizeof(struct ZipArray))) == NULL) {
        return NULL;
    }
    sh->nsize = 0;

    return sh;
}

/**
 * 清空 ZipArray
 * */
void ZipArrayClear(ZipArray* array)
{
    uint16_t alen = array->buf.free + array->buf.len;
    array->nsize = 0;
    array->buf.free = alen;
    array->buf.len = 0;
    array->buf.buf[0] = 0;
}

int ZipArrayPushElement(ZipArray* array, int encode, void* data, uint16_t esize)
{
    uint16_t rlen = sizeof(ZipData) + esize;;

    // 1 查看剩余容量
    if (array->buf.free < rlen) {
        return -1;
    }

    // 2 将元素放到末尾
    ZipData* zd = (ZipData*)(array->buf.buf + array->buf.len);
    zd->encode = encode;
    memcpy(zd->buf, data, esize);

    array->buf.len += rlen;
    array->buf.free -= rlen;
    ++array->nsize;

    return 0;
}

int ZipArrayPushInt16(ZipArray* array, int16_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_INT16, &value, sizeof(int16_t));
}

int ZipArrayPushUInt16(ZipArray* array, uint16_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_UINT16, &value, sizeof(uint16_t));
}

int ZipArrayPushInt32(ZipArray* array, int32_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_INT32, &value, sizeof(int32_t));
}

int ZipArrayPushUInt32(ZipArray* array, uint32_t value)
{
    return ZipArrayPushElement(array, ZD_ENCODE_UINT32, &value, sizeof(uint32_t));
}

int ZipArrayPushString(ZipArray* array, const char* s, uint16_t len)
{
    ZipData* zd;
    ZipArrayString* zsh;
    uint16_t rlen = sizeof(ZipData) + sizeof(ZipArrayString) + len + 1;
    // 1 查看剩余容量
    if (array->buf.free < rlen) {
        return -1;
    }

    // 2 ZipData
    zd = (ZipData*)(array->buf.buf + array->buf.len);
    zd->encode = ZD_ENCODE_STRING;

    // 3 ZipArrayString
    zsh = (ZipArrayString*) zd->buf;
    zsh->len = len;
    memcpy(zsh->buf, s, len);
    zsh->buf[len] = 0;

    // 3 ZipArray
    array->buf.len += rlen;
    array->buf.free -= rlen;
    ++array->nsize;

    return 0;
}

ZipArrayIterator ZipArrayGetIterator(ZipArray* array)
{
    ZipArrayIterator iter;
    iter.next = (ZipData*)array->buf.buf;
    iter.nsize = array->nsize;
    return iter;
}

ZipData* ZipArrayNext(ZipArrayIterator* iter)
{
    ZipData* next = iter->next;
    if(iter->nsize == 0) {return NULL;}

    if (iter->next->encode != ZD_ENCODE_STRING) {
        iter->next = (ZipData*)(iter->next->buf + iter->next->encode / 2);
    }
    else {
        ZipArrayString* zs = (ZipArrayString*)iter->next->buf;
        iter->next = (ZipData*)(iter->next->buf + sizeof(ZipArrayString) + zs->len + 1);  // 字符串末尾有 \0
    }

    --iter->nsize;

    return next;
}

int CStringAddRespInt8Array(cstr s, uint8_t* array, int len)
{
    int str_len, i;
    char buf[15];

    str_len = sprintf(s + CStringGetLen(s), "*%d\r\n", len);
    CStringIncrLen(s, str_len);

    for (i=0; i<len; ++i) {
        // ltoa(array[i], buf, 10);//LINUX没有ltoa
        sprintf(buf, "%ld", array[i]);
        str_len = sprintf(s + CStringGetLen(s), ":%s\r\n", buf);
        CStringIncrLen(s, str_len);
    }

    return 0;
}






