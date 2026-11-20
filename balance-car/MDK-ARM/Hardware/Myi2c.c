#include "stm32f1xx_hal.h"
#include "Delay_us.h"

#define MYI2C_GPIO_PORT GPIOB
#define MYI2C_SCL_PIN   GPIO_PIN_10
#define MYI2C_SDA_PIN   GPIO_PIN_11


void MyI2C_Init(void)   
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Pin = MYI2C_SCL_PIN | MYI2C_SDA_PIN;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(MYI2C_GPIO_PORT, &GPIO_InitStructure);

    HAL_GPIO_WritePin(MYI2C_GPIO_PORT, MYI2C_SCL_PIN | MYI2C_SDA_PIN, GPIO_PIN_SET);

}
void MyI2C_W_SCL(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MYI2C_GPIO_PORT, MYI2C_SCL_PIN, (GPIO_PinState)BitValue);
    HAL_delay_us(10); // 添加适当的延迟以满足I2C时序要求
}

void MyI2C_W_SDA(uint8_t BitValue)
{
    HAL_GPIO_WritePin(MYI2C_GPIO_PORT, MYI2C_SDA_PIN, (GPIO_PinState)BitValue);
    HAL_delay_us(10); // 添加适当的延迟以满足I2C时序要求
}

uint8_t MyI2C_R_SDA(void)
{
   uint8_t BitValue;
   BitValue=HAL_GPIO_ReadPin(MYI2C_GPIO_PORT, MYI2C_SDA_PIN);
   HAL_delay_us(10);
    return BitValue;
}



void MyI2C_Start(void)
{
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(0);
}

void MyI2C_Stop(void)
{
    MyI2C_W_SDA(0);
    MyI2C_W_SCL(1);
    MyI2C_W_SDA(1);
}

void MyI2C_SendByte(uint8_t Byte)
{
    uint8_t i;
    for (i = 0; i < 8; i++)
    {
        MyI2C_W_SDA(!!(Byte & (0x80 >> i)));//！！（0）=0，！！（非0）=1.10101110&10000000=10000000，！！（10000000）=1
        MyI2C_W_SCL(1);
        MyI2C_W_SCL(0);
    }
}
uint8_t MyI2C_ReceiveByte(void)
{
    uint8_t i, Byte=0x00;
    MyI2C_W_SDA(1);
    
    for(i=0;i<8;i++)
    {
        MyI2C_W_SCL(1);
        if(MyI2C_R_SDA())//MyI2C_R_SDA()返回1或0，1时Byte的对应位需要置1,0时保持为0
        {
            Byte |= (0x80 >> i); 
           
        } 
        MyI2C_W_SCL(0);
    }
    return Byte;
}
void MyI2C_SendAck(uint8_t Ack)
{
    MyI2C_W_SDA(Ack);
    MyI2C_W_SCL(1);
    MyI2C_W_SCL(0);
}
uint8_t MyI2C_ReceiveAck(void)
{
    uint8_t Ack;
    MyI2C_W_SDA(1);
    MyI2C_W_SCL(1);
    Ack = MyI2C_R_SDA();
    MyI2C_W_SCL(0);
    return Ack;
}