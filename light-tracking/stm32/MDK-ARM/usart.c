#include "stm32f1xx_hal.h"
#include "usart.h"
#include <stdio.h>
#include <string.h>

UART_HandleTypeDef huart1;

static uint8_t rxByte;
static char lineBuf[32];
static uint8_t lineLen = 0;

volatile int     g_dx = 0;
volatile int     g_dy = 0;
volatile uint8_t g_haveTarget = 0;
volatile uint8_t g_newData = 0;

void Usart1_Init(void)
{
    GPIO_InitTypeDef gpio = {0};
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_USART1_CLK_ENABLE();

    /* PA9 = TX,推挽输出 */
    gpio.Mode=GPIO_MODE_AF_PP;
    gpio.Pin=GPIO_PIN_9;
    gpio.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&gpio);

    /* PA10 = RX,浮空输入 */
    gpio.Pin  = GPIO_PIN_10;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &gpio);

    huart1.Instance = USART1;
    huart1.Init.BaudRate=115200;
    huart1.Init.WordLength=UART_WORDLENGTH_8B;
    huart1.Init.StopBits=UART_STOPBITS_1;
    huart1.Init.Parity=UART_PARITY_NONE;
    huart1.Init.Mode=UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl=UART_HWCONTROL_NONE;
    huart1.Init.OverSampling=UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart1);

    HAL_NVIC_SetPriority(USART1_IRQn, 1,0);//设置中断优先级
    HAL_NVIC_EnableIRQ(USART1_IRQn);//开启NVIC的USART1通道

    HAL_UART_Receive_IT(&huart1, &rxByte, 1);//开启接收中断

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        char c = (char)rxByte;
        if(c=='\n')
        {
            //一条消息 收完了
            lineBuf[lineLen] = '\0';//字符串结束符
            if(strncmp(lineBuf,"none",4)==0)
            {
                g_haveTarget = 0;//收到的lineBuf是"none"，表示没有目标
            }
            else
            {
                int dx=0,dy=0;
                if(sscanf(lineBuf,"%d,%d",&dx,&dy)==2)
                {
                    g_dx = dx;
                    g_dy = dy;
                    g_haveTarget = 1;//收到的lineBuf是"dx,dy"，表示有目标
                }

            }

            g_newData = 1;//收到一条完整的消息
            lineLen = 0;//准备接收下一条消息
        }
        else if(c!='\r')
        {
            if(lineLen<sizeof(lineBuf)-1)
            {
                lineBuf[lineLen++] = c;
            }
            else
            {
                lineLen = 0;//消息太长，丢弃
            }
        }

        HAL_UART_Receive_IT(&huart1, &rxByte, 1);//继续接收下一个字节

    }
}
