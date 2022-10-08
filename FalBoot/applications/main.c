#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <agile_upgrade.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#define BOOT_BKP      RTC_BKP_DR15
#define BOOT_APP_ADDR 0x08080000UL

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

int main(void) {
    agile_upgrade_t src_agu = {0};
    src_agu.name = "download";
    src_agu.user_data = "download_onchip";
    src_agu.ops = &agile_upgrade_fal_ops;

    agile_upgrade_t dst_agu = {0};
    dst_agu.name = "app";
    dst_agu.user_data = "app";
    dst_agu.ops = &agile_upgrade_fal_ops;

    int rc = agile_upgrade_release(&src_agu, &dst_agu, 0);
    if (rc != AGILE_UPGRADE_EOK) {
        rc = agile_upgrade_verify(&dst_agu, NULL, 1);
        if (rc != AGILE_UPGRADE_EOK) {
            LOG_W("Force the name:%s to run!", dst_agu.name);
        }
    }

    LOG_I("Starting run.....");
    boot_app_enable();

    return 0;
}
