#ifndef __SPI_H
#define __SPI_H

#include <stdint.h>

//////////////////////////////////////////////////////////////////////////////////
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F407开发板
// SPI驱动代码
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2017/3/13
//版本：V1.0
//版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

void SPI1_Init(void);
void SPI1_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SPI1_ReadWriteByte(uint8_t TxData);

#endif
