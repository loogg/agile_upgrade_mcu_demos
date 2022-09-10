#include "spi.h"
#include "board.h"

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F407开发板
// SPI驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/4/13
//版本：V1.0
//版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

SPI_HandleTypeDef SPI1_Handler;  // SPI1句柄

//以下是SPI模块的初始化代码，配置成主机模式
// SPI口初始化
//这里针是对SPI1的初始化
void SPI1_Init(void) {
    SPI1_Handler.Instance = SPI1;
    SPI1_Handler.Init.Mode = SPI_MODE_MASTER;
    SPI1_Handler.Init.Direction = SPI_DIRECTION_2LINES;
    SPI1_Handler.Init.DataSize = SPI_DATASIZE_8BIT;
    SPI1_Handler.Init.CLKPolarity = SPI_POLARITY_HIGH;
    SPI1_Handler.Init.CLKPhase = SPI_PHASE_2EDGE;
    SPI1_Handler.Init.NSS = SPI_NSS_SOFT;
    SPI1_Handler.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    SPI1_Handler.Init.FirstBit = SPI_FIRSTBIT_MSB;
    SPI1_Handler.Init.TIMode = SPI_TIMODE_DISABLE;
    SPI1_Handler.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    SPI1_Handler.Init.CRCPolynomial = 7;
    HAL_SPI_Init(&SPI1_Handler);

    __HAL_SPI_ENABLE(&SPI1_Handler);  //使能SPI1

    SPI1_ReadWriteByte(0Xff);  //启动传输
}

// SPI速度设置函数
// SPI速度=fAPB1/分频系数
//@ref SPI_BaudRate_Prescaler:SPI_BAUDRATEPRESCALER_2~SPI_BAUDRATEPRESCALER_2 256
// fAPB1时钟一般为42Mhz：
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler) {
    assert_param(IS_SPI_BAUDRATE_PRESCALER(SPI_BaudRatePrescaler));  //判断有效性
    __HAL_SPI_DISABLE(&SPI1_Handler);                                //关闭SPI
    SPI1_Handler.Instance->CR1 &= 0XFFC7;                 //位3-5清零，用来设置波特率
    SPI1_Handler.Instance->CR1 |= SPI_BaudRatePrescaler;  //设置SPI速度
    __HAL_SPI_ENABLE(&SPI1_Handler);                      //使能SPI
}

// SPI1 读写一个字节
// TxData:要写入的字节
//返回值:读取到的字节
uint8_t SPI1_ReadWriteByte(uint8_t TxData) {
    uint8_t Rxdata;
    HAL_SPI_TransmitReceive(&SPI1_Handler, &TxData, &Rxdata, 1, 1000);
    return Rxdata;  //返回收到的数据
}
