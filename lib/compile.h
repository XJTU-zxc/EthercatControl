#ifndef __COMPILE_H__
#define __COMPILE_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 表示符的 token id
 * */
#define IDENTIFIER_POUND 0
#define IDENTIFIER_START 1
#define IDENTIFIER_SEMI 2
#define IDENTIFIER_NEWLINE1 3
#define IDENTIFIER_TAB 4
#define IDENTIFIER_NEWLINE2 5
#define IDENTIFIER_SPACE 6
#define IDENTIFIER_A 7
#define IDENTIFIER_B 8
#define IDENTIFIER_C 9
#define IDENTIFIER_D 10
#define IDENTIFIER_E 11
#define IDENTIFIER_F 12
#define IDENTIFIER_G 13
#define IDENTIFIER_H 14
#define IDENTIFIER_I 15
#define IDENTIFIER_J 16
#define IDENTIFIER_K 17
#define IDENTIFIER_S 18
#define IDENTIFIER_EQUAL 19
#define IDENTIFIER_COMMA 20
#define IDENTIFIER_GREATER 21
#define IDENTIFIER_LESS 22
#define IDENTIFIER_GEQUAL 23
#define IDENTIFIER_LEQUAL 24
#define IDENTIFIER_NUMBER 25
#define IDENTIFIER_FNUMBER 26
#define IDENTIFIER_STRING 27
#define IDENTIFIER_NULL 28

/* 状态矩阵行数 */
#define COMPILER_STATE_MATRIX_ROW 33
/* 状态矩阵列数 */
#define COMPILER_STATE_MATRIX_COL 97

/* 用于处理字符串 */
struct StringWrapper;

/**
 * @brief 编码模型，编译的结果保存在这个结构体中
 */
struct EncodeObject;

/**
 * @brief 用于编译三指令的结构体
 */
typedef struct Compiler {
    /* 初始状态 */
    int initState;

    /* 当前编译器处在的状态 */
    int cstate;

    /* 编号最大的终止态，小于等于这个值的数都是终止态 */
    int mestate;

    /* 字母映射表 */
    signed char* charMap;

    /* 状态转移矩阵 */
    int16_t (*stateMatrix)[COMPILER_STATE_MATRIX_COL];

    /* 当前保存的 token ，最大 ? 个字节 */
    char token[80];

    /* token 的长度 */
    int tokenlen;
} Compiler;

/**
 * @brief 初始化 Compiler
 */
int CompilerInit(Compiler* compiler);

/**
 * @brief Compiler 从 cstream 中读取一个 token
 * @return 该 token 的 id
 */
int CompilerReadToken(Compiler* compiler, struct StringWrapper* wrapper);

/**
 * @brief 跳过 tab 和空格，读取一个 token
 */
int CompilerReadImportantToken(Compiler* compiler, struct StringWrapper* wrapper);

/**
 * @brief 解析一行指令
 * */
int CompilerParse(Compiler* compiler, const char* buf, int n, bool isMdi);


/**
 * @brief 用于装下一串字符串，便于处理字符串
 */
typedef struct StringWrapper {
    /* 待处理字符串 */
    const char* buf;
    /* 待处理字符串长度 */
    int len;
} StringWrapper;

/**
 * @brief 跳过空格
 * */
void Strip(StringWrapper* wrapper);

int isProgName(const char* buf, int length);

#ifdef __cplusplus
}
#endif

#endif