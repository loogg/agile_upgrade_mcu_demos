#include "board.h"
#include <stdint.h>
#ifdef RT_USING_FINSH
#include <rtdevice.h>
#endif

static uint8_t _is_init = 0;
static UART_HandleTypeDef _huart1;

#ifdef RT_USING_FINSH
static struct rt_semaphore _rx_notice = {0};
static struct rt_ringbuffer _rx_rb = {0};
static uint8_t _rx_rb_pool[128];
#endif

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

#ifdef RT_USING_FINSH
    rt_sem_init(&_rx_notice, "con_rx", 0, RT_IPC_FLAG_FIFO);
    rt_ringbuffer_init(&_rx_rb, _rx_rb_pool, sizeof(_rx_rb_pool));
    /* enable rx irq */
    HAL_NVIC_SetPriority(USART1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* enable interrupt */
    __HAL_UART_ENABLE_IT(&_huart1, UART_IT_RXNE);
#endif

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

#ifdef RT_USING_FINSH

void USART1_IRQHandler(void) {
    /* enter interrupt */
    rt_interrupt_enter();

    if ((__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_RXNE) != RESET) &&
        (__HAL_UART_GET_IT_SOURCE(&_huart1, UART_IT_RXNE) != RESET)) {
        uint8_t ch = _huart1.Instance->DR & 0xff;
        rt_ringbuffer_putchar(&_rx_rb, ch);

        rt_sem_release(&_rx_notice);
    } else {
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_ORE) != RESET) {
            __HAL_UART_CLEAR_OREFLAG(&_huart1);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_NE) != RESET) {
            __HAL_UART_CLEAR_NEFLAG(&_huart1);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_FE) != RESET) {
            __HAL_UART_CLEAR_FEFLAG(&_huart1);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_PE) != RESET) {
            __HAL_UART_CLEAR_PEFLAG(&_huart1);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_LBD) != RESET) {
            __HAL_UART_CLEAR_FLAG(&_huart1, UART_FLAG_LBD);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_CTS) != RESET) {
            __HAL_UART_CLEAR_FLAG(&_huart1, UART_FLAG_CTS);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_TC) != RESET) {
            __HAL_UART_CLEAR_FLAG(&_huart1, UART_FLAG_TC);
        }
        if (__HAL_UART_GET_FLAG(&_huart1, UART_FLAG_RXNE) != RESET) {
            __HAL_UART_CLEAR_FLAG(&_huart1, UART_FLAG_RXNE);
        }
    }

    /* leave interrupt */
    rt_interrupt_leave();
}

char rt_hw_console_getchar(void) {
    char ch = 0;

    rt_sem_control(&_rx_notice, RT_IPC_CMD_RESET, RT_NULL);
    while (rt_ringbuffer_getchar(&_rx_rb, (rt_uint8_t *)&ch) != 1) {
        rt_sem_take(&_rx_notice, RT_WAITING_FOREVER);
    }
    return ch;
}

char rt_hw_console_readchar(void) {
    char ch = 0;

    rt_ringbuffer_getchar(&_rx_rb, (rt_uint8_t *)&ch);

    return ch;
}

#endif
