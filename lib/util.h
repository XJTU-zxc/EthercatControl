/**
 * 基本数据结构以及一些常用的辅助函数
 * */

#ifndef LIB_UTIL_H_
#define LIB_UTIL_H_

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct CString;
struct CircularQueue;
struct ZipArray;

/**
 * 将 string 转换成 int，成功返回1，失败返回0
 **/
int String2Int(const char* s, uint16_t slen, int32_t* ii);

/* 将整型转换成字符串 */
int Int2String(char* s, int val);

/**
 * 字符串
 * */
typedef char* cstr;

typedef struct CString {
    uint16_t len;
    uint16_t free;
    char buf[];
} CString;

/**
 * 用一个已经分配好内存的数组初始化 CString
 */
cstr CStringInit(char* p, uint16_t size);


/**
 * 获取字符串长度
 */
uint16_t CStringGetLen(const cstr s);

/**
 * 获取字符串空闲空间长度
 */
uint16_t CStringGetFree(const cstr s);

/**
 * 增加 len 大小
 * */
void CStringIncrLen(cstr s, uint16_t len);

/**
 * 拼接、替换。成功返回0，失败返回-1。
 * */
int CStringCat(cstr s, const char* des, int len);

/**
 * 查找
 * */
const char* CStringFindChar(cstr s, char c);

/**
 * 保留字符串选定范围内的数据,左闭右开原则
 * */
int CStringRange(cstr s, uint16_t start, uint16_t len);

/**
 * 向 CString 添加 ok，形式为 resp 格式
 * */
int CStringAddRespOk(cstr s);

/**
 * 向 CString 添加 fail，形式为 resp 格式
 * */
int CStringAddRespFail(cstr s);

/**
 * 向CString添加ct消息，要求继续下传wyf
 * */
int CStringAddRespCt(cstr s);

/**
 * 向 CString 添加 数字，形式为 resp 格式
 * */
int CStringAddRespNumber(cstr s, int num);

/**
 * 向 CString 添加字符串，形式为 resp 格式
 * */
int CStringAddRespString(cstr s, const char* str, int slen);

/**
 * 向 CString 添加 Int 数组，形式为 resp 格式
 * */
int CStringAddRespInt32Array(cstr s, int32_t* array, int len);

int CStringAddRespUint16Array(cstr s, uint16_t* array, int len);

/**
 * 循环队列
 * */
typedef struct CircularQueue {
    uint16_t head;
    uint16_t len;
    uint16_t free;

    char ary[];
} CircularQueue;

/**
 * 初始化循环队列
 * */
struct CircularQueue* CQInit(const char* p, uint16_t size);
int CQPushBack(struct CircularQueue* cq, int value);
int CQPushFront(struct CircularQueue* cq, int value);
int CQPopFront(struct CircularQueue* cq);
int CQPopBack(struct CircularQueue* cq);
int CQFront(struct CircularQueue* cq);
int CQBack(struct CircularQueue* cq);
uint16_t CQSize(struct CircularQueue* cq);

/**
 * 一维数组
 * */

/**
 * 数据编码
 * */
enum ZipDataEncode {
    ZD_ENCODE_INT16 = 4,        // int_16
    ZD_ENCODE_UINT16 = 5,       // uint_16
    ZD_ENCODE_INT32 = 8,        // int_32
    ZD_ENCODE_UINT32 = 9,       // uint_32
    ZD_ENCODE_STRING        // string
};

/**
 * String 类型
 * */
typedef struct ZipArrayString {
    uint16_t len;   // 长度
    char buf[];     // 数据
} ZipArrayString;

typedef struct ZipData {
    uint16_t encode;    // 编码
    char buf[];         // 数据
} ZipData;

typedef struct ZipArray {
    uint16_t nsize;     // 元素数量
    CString buf;        // 存放数据
} ZipArray;

/* 获得 int 数据 */
#define ZipArrayInt32(data) *((int32_t*)data->buf)


/**
 * 初始化 ZipArray
 **/
ZipArray* ZipArrayInit(char* p, uint16_t size);
void ZipArrayClear(ZipArray* array);
static int ZipArrayPushElement(ZipArray* array, int encode, void* data, uint16_t esize);
int ZipArrayPushInt16(ZipArray* array, int16_t value);
int ZipArrayPushUInt16(ZipArray* array, uint16_t value);
int ZipArrayPushInt32(ZipArray* array, int32_t value);
int ZipArrayPushUInt32(ZipArray* array, uint32_t value);
int ZipArrayPushString(ZipArray* array, const char* s, uint16_t len);

/* ZipArray 的迭代器 */
typedef struct ZipArrayIterator {
    ZipData* next;
    uint16_t nsize;
} ZipArrayIterator;

/* ZipArray的迭代器 */
ZipArrayIterator ZipArrayGetIterator(ZipArray* array);

/* 返回迭代器当前所指的 ZipData
 *
 * 用法举例：
 *   ZipArrayIterator iter = ZipArrayGetIterator(array);
 *   ZipData* data = NULL;
 *   while((data = ZipArrayNext(&iter)) != NULL) {
 *      Process(data);
 *   }
 */
ZipData* ZipArrayNext(ZipArrayIterator* iter);

/* flash烧写相关 */
/* 将收到的命令处理并缓存  */
int packArray(uint16_t* from, uint16_t* to, uint16_t fromLen, uint16_t toLen, uint32_t addr, uint16_t* BCPBufferPtr);
/* 程序名处理并缓存 */
int dealProgName(uint16_t* from, uint16_t* to, uint16_t fromLen, uint32_t addr, uint16_t* BCPBufferPtr);
/* 程序数量+读程序地址处理并缓存 */
int dealProgNum(uint16_t* from, uint16_t* to, uint16_t fromLen, uint16_t* BCPBufferPtr);
int dealProgAddr(uint16_t* from, uint16_t* to, uint16_t fromLen, uint16_t* BCPBufferPtr);
/* 考虑溢出情况下修改最后一条的命令指向的下一条命令地址 */
int modifyLastAddr(uint16_t* data, uint16_t* BCPBufferPtr, uint32_t* codeCurAddr);

/* 更新当前写地址，如果超过了某个扇区的末尾值就换到下一个扇区  */
int UpdataWriteAddr(uint32_t* addr, uint32_t length);

/* 比较程序名 */
int checkName(char* c1, int c1Len, char* c2, int c2Len);

/* 烧写BPM参数 */
int PersistBPM();
#ifdef __cplusplus
}
#endif

#endif /* LIB_UTIL_H_ */



