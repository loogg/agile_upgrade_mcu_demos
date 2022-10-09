#include "board.h"
#include <stdint.h>

#ifndef RT_USING_DEVICE

static uint8_t _is_init = 0;
static UART_HandleTypeDef _huart1;

static int hw_console_uart_init(void) {
    _huart1.Instance = USART1;
    _huart1.Init.BaudRate = 115200;
    _huart1.Init.WordLength = UART_WORDLENGTH_8B;
    _huart1.Init.StopBits = UART_STOPBITS_1;
    _huart1.Init.Parity = UART_PARITY_NONE;
    _huart1.Init.Mode = UART_MODE_TX_RX;
    _huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    _huart1.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&_huart1) != HAL_OK) {
        Error_Handler();
    }

    _is_init = 1;

    return 0;
}
INIT_BOARD_EXPORT(hw_console_uart_init);

void rt_hw_console_output(const char *str) {
    if (!_is_init) return;

    rt_size_t i = 0, size = 0;
    char a = '\r';

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            HAL_UART_Transmit(&_huart1, (uint8_t *)&a, 1, 1);
        }
        HAL_UART_Transmit(&_huart1, (uint8_t *)(str + i), 1, 1);
    }
}

#endif
