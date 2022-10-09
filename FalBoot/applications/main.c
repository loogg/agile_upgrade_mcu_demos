#include <rtthread.h>
#include <board.h>
#include <agile_upgrade.h>
#include <fal.h>
#ifdef RT_USING_FINSH
#include "shell.h"
#endif

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define BOOT_BKP               RTC_BKP_DR15
#define BOOT_APP_ADDR          0x08080000UL
#define BOOT_SHELL_KEY_TIMEOUT 3

extern const struct agile_upgrade_ops agile_upgrade_fal_ops;

static void boot_app_enable(void) {
    __disable_irq();
    RTC_HandleTypeDef RTC_Handler = {0};
    RTC_Handler.Instance = RTC;
    HAL_RTCEx_BKUPWrite(&RTC_Handler, BOOT_BKP, 0xA5A5);
    HAL_NVIC_SystemReset();
}

typedef void (*boot_app_func)(void);
void boot_start_application(void) {
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    RTC_HandleTypeDef RTC_Handler = {0};
    RTC_Handler.Instance = RTC;
    uint32_t bkp_data = HAL_RTCEx_BKUPRead(&RTC_Handler, BOOT_BKP);
    HAL_RTCEx_BKUPWrite(&RTC_Handler, BOOT_BKP, 0);

    if (bkp_data != 0xA5A5) return;

    boot_app_func app_func = NULL;
    uint32_t app_addr = BOOT_APP_ADDR;
    if (((*(__IO uint32_t *)(app_addr + 4)) & 0xff000000) != 0x08000000) return;

    /* 栈顶地址在 128K RAM 间 */
    if (((*(__IO uint32_t *)app_addr) - 0x20000000) >= (STM32_SRAM_SIZE * 1024)) return;

    app_func = (boot_app_func) * (__IO uint32_t *)(app_addr + 4);
    /* Configure main stack */
    __set_MSP(*(__IO uint32_t *)app_addr);
    /* jump to application */
    app_func();
}

#ifdef RT_USING_FINSH
static int boot_shell_key_check(void) {
    rt_tick_t tick_timeout =
        rt_tick_get() + rt_tick_from_millisecond(BOOT_SHELL_KEY_TIMEOUT * 1000);

    rt_device_t console_dev = rt_console_get_device();
    if (console_dev == RT_NULL) {
        LOG_W("There is no console device.");
        return -RT_ERROR;
    }

    LOG_D("Press [Enter] key into shell in %d s...", BOOT_SHELL_KEY_TIMEOUT);

    while (rt_tick_get() - tick_timeout >= (RT_TICK_MAX / 2)) {
        uint8_t ch = 0;
        rt_device_read(console_dev, 0, &ch, 1);
        if (ch == '\r' || ch == '\n') return RT_EOK;

        rt_thread_mdelay(5);
    }

    LOG_W("Wait shell key timeout.");
    return -RT_ERROR;
}
#endif

int main(void) {
    fal_init();

#ifdef BSP_USING_SPI_FLASH
    agile_upgrade_t src_agu = {0};
    src_agu.name = "download";
    src_agu.user_data = "download_w25q";
    src_agu.ops = &agile_upgrade_fal_ops;
#else
    agile_upgrade_t src_agu = {0};
    src_agu.name = "download";
    src_agu.user_data = "download_onchip";
    src_agu.ops = &agile_upgrade_fal_ops;
#endif

    agile_upgrade_t dst_agu = {0};
    dst_agu.name = "app";
    dst_agu.user_data = "app";
    dst_agu.ops = &agile_upgrade_fal_ops;

    int rc = agile_upgrade_release(&src_agu, &dst_agu, 0);
    if (rc != AGILE_UPGRADE_EOK) {
#ifdef RT_USING_FINSH
        rc = boot_shell_key_check();
        if (rc == RT_EOK) {
            finsh_system_init();
            return 0;
        }
#endif

        rc = agile_upgrade_verify(&dst_agu, NULL, 1);
        if (rc != AGILE_UPGRADE_EOK) {
            LOG_W("Force the name:%s to run!", dst_agu.name);
        }
    }

    LOG_I("Starting run.....");
    boot_app_enable();

    return 0;
}
