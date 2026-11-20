#include "stm32f1xx_hal.h"
#include "Myi2c.h"
#include "MPU6050_Reg.h"

#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
//IIC指定地址写入数据
{
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址
    MyI2C_ReceiveAck();					//等待ACK
    MyI2C_SendByte(RegAddress);
    MyI2C_ReceiveAck();			//发送寄存器地址
    MyI2C_SendByte(Data);				//发送数据
    MyI2C_ReceiveAck();
    MyI2C_Stop();
}

uint8_t MPU6050_ReadReg(uint8_t RegAddress)
//IIC指定地址读取数据
{    
    uint8_t Data;
    MyI2C_Start();
    MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址
    MyI2C_ReceiveAck();					//等待ACK
    MyI2C_SendByte(RegAddress);			//指定地址写 读写位为0，
    MyI2C_ReceiveAck();                 //此时指针指向该指定的地址
    
    MyI2C_Start();						//重复起始条件
    MyI2C_SendByte(MPU6050_ADDRESS | 0x01);//IIC当前地址读，当前地址就是上面指定的地址，读写位为1
    MyI2C_ReceiveAck();					//等待ACK
    Data=MyI2C_ReceiveByte();            //读取数据存到data
    MyI2C_SendAck(1);					//发送ACK（1）表示非应答只要一个字节
    MyI2C_Stop();                       //发送停止条件
    return Data;                      
}

void MPU6050_Init(void)
{
	MyI2C_Init();									//先初始化底层的I2C
	
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x09);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x06);			//配置寄存器，配置DLPF
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);	//陀螺仪配置寄存器，选择满量程为±2000°/s
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	//加速度计配置寄存器，选择满量程为±16g
}

uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		//返回WHO_AM_I寄存器的值
}

void MPU6050_GetData(int16_t* Accx, int16_t* Accy, int16_t* Accz, int16_t* Gyrox, int16_t* Gyroy, int16_t* Gyroz)//读取加速度计和陀螺仪数据
{
    uint8_t Accx_H, Accx_L, Accy_H, Accy_L, Accz_H, Accz_L;
    uint8_t Gyrox_H, Gyrox_L, Gyroy_H, Gyroy_L, Gyroz_H, Gyroz_L;
    
    Accx_H=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_H);
    Accx_L=MPU6050_ReadReg(MPU6050_ACCEL_XOUT_L);
    *Accx=Accx_H<<8 | Accx_L;	//合成16位加速度X轴数据
    Accy_H=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_H);
    Accy_L=MPU6050_ReadReg(MPU6050_ACCEL_YOUT_L);
    *Accy=Accy_H<<8 | Accy_L;	//合成16位加速度Y轴数据
    Accz_H=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_H);       
    Accz_L=MPU6050_ReadReg(MPU6050_ACCEL_ZOUT_L);
    *Accz=Accz_H<<8 | Accz_L;	//合成16位加速度Z轴数据

    Gyrox_H=MPU6050_ReadReg(MPU6050_GYRO_XOUT_H);
    Gyrox_L=MPU6050_ReadReg(MPU6050_GYRO_XOUT_L);
    *Gyrox=Gyrox_H<<8 | Gyrox_L;	//合成16位陀螺仪X轴数据
    Gyroy_H=MPU6050_ReadReg(MPU6050_GYRO_YOUT_H);
    Gyroy_L=MPU6050_ReadReg(MPU6050_GYRO_YOUT_L);
    *Gyroy=Gyroy_H<<8 | Gyroy_L;	//合成16位陀螺仪Y轴数据
    Gyroz_H=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_H);
    Gyroz_L=MPU6050_ReadReg(MPU6050_GYRO_ZOUT_L);
    *Gyroz=Gyroz_H<<8 | Gyroz_L;	//合成16位陀螺仪Z轴数据
}