#include <rtthread.h>
#include <agile_upgrade.h>
#include "board.h"
#ifdef RT_USING_FINSH
#include "shell.h"
#endif

#ifdef BSP_UPGRADE_USING_W25Q
#include "w25qxx.h"
#endif

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define BOOT_BKP               RTC_BKP_DR15
#define BOOT_APP_ADDR          0x08080000UL
#define BOOT_SHELL_KEY_TIMEOUT 3

#ifdef RT_USING_FINSH
extern char rt_hw_console_readchar(void);
#endif

#ifdef BSP_UPGRADE_USING_W25Q
extern const struct agile_upgrade_ops agile_upgrade_w25q_ops;
#endif

extern const struct agile_upgrade_ops agile_upgrade_onchip_ops;

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

    while (rt_tick_get() - tick_timeout >= (RT_TICK_MAX / 2)) {
        char ch = rt_hw_console_readchar();
        if (ch == '\r' || ch == '\n') return RT_EOK;

        rt_thread_mdelay(10);
    }

    return -RT_ERROR;
}
#endif

int main(void) {
#ifdef BSP_UPGRADE_USING_W25Q
    W25QXX_Init();
    uint32_t src_addr = 0;
    agile_upgrade_t src_agu = {0};
    src_agu.name = "download";
    src_agu.len = 0x60000;
    src_agu.user_data = &src_addr;
    src_agu.ops = &agile_upgrade_w25q_ops;
#else
    uint32_t src_addr = 0x20000;
    agile_upgrade_t src_agu = {0};
    src_agu.name = "download";
    src_agu.len = 0x60000;
    src_agu.user_data = &src_addr;
    src_agu.ops = &agile_upgrade_onchip_ops;
#endif

    uint32_t dst_addr = 0x80000;
    agile_upgrade_t dst_agu = {0};
    dst_agu.name = "app";
    dst_agu.len = 0x60000;
    dst_agu.user_data = &dst_addr;
    dst_agu.ops = &agile_upgrade_onchip_ops;

    int rc = agile_upgrade_release(&src_agu, &dst_agu, 0);
    if (rc != AGILE_UPGRADE_EOK) {
#ifdef RT_USING_FINSH
        LOG_D("Press [Enter] key into shell in %d s...", BOOT_SHELL_KEY_TIMEOUT);
        rc = boot_shell_key_check();
        if (rc == RT_EOK) {
            finsh_system_init();
            return 0;
        }
        LOG_W("Wait shell key timeout.");
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
