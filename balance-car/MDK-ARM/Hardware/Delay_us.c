#include "stm32f1xx_hal.h" // 确保包含此头文件

// 根据你的系统主频修改此值 (例如 72 for 72MHz, 48 for 48MHz)
#define CPU_FREQUENCY_MHZ 72

/**
 * @brief  精确的微秒级延时函数
 * @note   此函数会阻塞CPU，请勿在中断服务程序中使用。
 *         延时期间不能被其他中断长时间打断，否则会不准确。
 * @param  us: 需要延时的微秒数
 * @retval None
 */
void HAL_delay_us(uint32_t us)
{
    if (us == 0) return; // 处理0延时的边界情况

    uint32_t start = SysTick->VAL;
    // 计算需要等待的SysTick计数器滴答数
    // SysTick通常工作在CPU频率下，所以1个tick就是1/CPU_FREQ秒
    volatile uint32_t ticks = us * (CPU_FREQUENCY_MHZ);

    // SysTick是递减计数器，LOAD寄存器存储了重装载值
    uint32_t reload = SysTick->LOAD;

    while (ticks > 0)
    {
        uint32_t current = SysTick->VAL;
        uint32_t elapsed;

        // 检查SysTick计数器是否已经回绕（从0变回LOAD值）
        if (start >= current)
        {
            // 没有回绕，直接相减
            elapsed = start - current;
        }
        else
        {
            // 发生了回绕，需要计算两段的和
            elapsed = (start + 1) + (reload - current);
        }

        // 从总ticks中减去已经过去的ticks
        if (elapsed >= ticks)
        {
            break; // 已经延时足够
        }
        else
        {
            ticks -= elapsed;
            start = current; // 更新起始点
        }
    }
}