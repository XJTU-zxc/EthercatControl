#ifndef LIB_PROGRAMMERCONTROL_PROGRAMMERBULK_H_
#define LIB_PROGRAMMERCONTROL_PROGRAMMERBULK_H_


#include "ProgrammerLine.h"
#include "ProgrammerSystemParams.h"


class ProgrammerBulk {
public:
    ProgrammerBulk() : size(0) {}

    /**
     * @brief 添加一行程序
     * */
    bool AddLine(const pmc_char_t* s, uint16_t len)
    {
        if (this->size >= PROG_MAX_LINE_SIZE) {
            return false;
        }

        if (this->line[this->size].Set(s, len) == false) {
            return false;
        }
        ++this->size;

        return true;
    }

//    /* 获取指定行的程序，后面需要改动 */
//    pmc_char_t* GetLine(int i)
//    {
//        return this->line[i].Get();
//    }

    /* 获取指定行的程序，后面需要改动 */
    ProgrammerLine* GetLine(int i)
    {
        return &this->line[i];
    }

    /**
     * @brief 返回程序行数
     * */
    int16_t Size()  //wyf把size改成有符号的了，方便比较大小
    {
        return this->size;
    }


    /**
     * @brief 清空代码缓冲区
     * */
    void Clear()
    {
        this->size = 0;
    }

private:
    /* 程序行数量 */
    uint16_t size;
    /* 每行程序 */
    ProgrammerLine line[PROG_MAX_LINE_SIZE];
};


#endif