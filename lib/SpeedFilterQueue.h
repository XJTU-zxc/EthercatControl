#ifndef LIB_SPEEDFILTERQUEUE_H_
#define LIB_SPEEDFILTERQUEUE_H_


#include <stdint.h>


/* 用于计算速度 */
class SpeedFilterQueue {
public:
    SpeedFilterQueue() : m_sum(0), m_head(0), m_len(0)
    {}

    /**
     * @brief 加入一个速度
     */
    void Push(int32_t val)
    {
        /* 如果队列已满，将头部出队 */
        if (m_len == SPEED_FILTER_BUFFER_SIZE) {
            /* 获取头部数据 */
            int32_t head_val = m_speedBuf[m_head];
            /* 队列头部出队 */
            m_head = (m_head + 1) % SPEED_FILTER_BUFFER_SIZE;
            /*  */
            --m_len;
            /*  */
            m_sum -= head_val;
        }

        /* 将最新数据加入队列 */
        int idx = (m_head + m_len) % SPEED_FILTER_BUFFER_SIZE;
        m_speedBuf[idx] = val;
        m_sum += val;
        ++m_len;
    }

    /**
     * @brief 获取速度值
     */
    double Get()
    {
        if (m_len == 0) {
            return 0;
        }

        return (double)m_sum / m_len;
    }

    /**
     * @brief 清空数据
     */
    void Clear()
    {
        m_sum = 0;
        m_head = 0;
        m_len = 0;
    }

public:
enum {
    SPEED_FILTER_BUFFER_SIZE = 5
};

private:
    int32_t m_speedBuf[SPEED_FILTER_BUFFER_SIZE];
    int32_t m_sum;
    int16_t m_head;
    int16_t m_len;
};




#endif /* LIB_SPEEDFILTERQUEUE_H_ */