#ifndef LIB_PARAMSCPP_H_
#define LIB_PARAMSCPP_H_


/**
 * 保存了一些只能用于 .cpp 中文件的全局参数
 */
#include "ProgrammerControl/ProgrammerBulk.h"
#include "SpeedFilterQueue.h"
#include "ThreadSafeQueue.h"
#include "RespParser.h"

extern ProgrammerBulk g_programmerBulk;
extern SpeedFilterQueue g_speedFilterQueueA;
extern SpeedFilterQueue g_speedFilterQueueB;
extern SpeedFilterQueue g_speedFilterQueueC;
extern SpeedFilterQueue g_speedFilterQueueD;
extern SpeedFilterQueue g_speedFilterQueueE;
extern SpeedFilterQueue g_speedFilterQueueF;
extern SpeedFilterQueue g_speedFilterQueueG;
extern SpeedFilterQueue g_speedFilterQueueH;
extern ThreadSafeQueue<RespMessage> threadSafeQueue;
extern ThreadSafeQueue<std::string>  threadSafeQueue2; 

// 定义一个结构体，用于存储命令标识和结果
// struct CommandResult {
//     uint32_t commandId;  // 命令标识
//     std::string result;  // 处理结果

//     CommandResult(uint32_t id, std::string res)
//         : commandId(id), result(std::move(res)) {}
// };
// extern ThreadSafeQueue<CommandResult>  threadSafeQueue2; 




#include <iostream>
#include <sstream>
#include <string>
#include <cstring>

// 解析函数 by zxc for test
inline void parseMessage(const char* message, int len) {
    std::string msg(message, len);
    std::istringstream iss(msg);
    std::string line;

    if (std::getline(iss, line)) {
        if (!line.empty()) {
            if (line[0] == '$') {
                // $ 类型，打印字符串
                int strLength;
                try {
                    strLength = std::stoi(line.substr(1));
                    std::string strContent;
                    if (std::getline(iss, strContent)) {
                        std::cout << "String content: " << strContent.substr(0, strLength) << std::endl;
                    }
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid format for $ type." << std::endl;
                }
            } else if (line[0] == '*') {
                // * 类型，一行打印全部数字，用空格隔开
                int arrayLength;
                try {
                    arrayLength = std::stoi(line.substr(1));
                    std::cout << "Array content: ";
                    for (int i = 0; i < arrayLength; ++i) {
                        if (std::getline(iss, line)) {
                            if (line[0] == ':') {
                                try {
                                    int num = std::stoi(line.substr(1));
                                    std::cout << num << " ";
                                } catch (const std::invalid_argument&) {
                                    std::cerr << "Invalid number format in array." << std::endl;
                                }
                            }
                        }
                    }
                    std::cout << std::endl;
                } catch (const std::invalid_argument&) {
                    std::cerr << "Invalid format for * type." << std::endl;
                }
            } else {
                std::cerr << "Unrecognized message type." << std::endl;
            }
        }
    }
}


#endif /* LIB_PARAMSCPP_H_ */