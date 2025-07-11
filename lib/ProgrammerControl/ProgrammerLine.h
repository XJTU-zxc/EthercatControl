#ifndef LIB_PROGRAMMERCONTROL_PROGRAMMERLINE_H_
#define LIB_PROGRAMMERCONTROL_PROGRAMMERLINE_H_


#include <stdint.h>
#include <string.h>

#include "ProgrammerSystemParams.h"

class ProgrammerLine {
public:
    ProgrammerLine() : size(0) {}

    /**
     * @brief 获取该行代码数据
     * */
    pmc_char_t* Get()
    {
        return this->buf;
    }

    /**
     * @brief 设置该行代码数据
     * */
    bool Set(const pmc_char_t* s, uint16_t len)
    {
        if (len > PROG_LINE_BYTES_MAX_SIZE) {
            return false;
        }

        memcpy(this->buf, s, len);
        this->buf[len] = 0;
        this->size = len;

        return true;
    }

    /**
     * @brief 返回该行代码长度
     * */
    uint16_t Size()
    {
        return size;
    }
private:
    /* 行程序字节数 */
    uint16_t size;
    /* 程序内容 */
    pmc_char_t buf[PROG_LINE_BYTES_MAX_SIZE + 1];
};


#endif