#include <stddef.h>
#include <drv_flash.h>
#include "board.h"
#include <agile_upgrade.h>

static int adapter_init(agile_upgrade_t *agu) {
    agu->ops_data = NULL;
    if (agu->len <= 0) return AGILE_UPGRADE_ERR;
    if (agu->user_data == NULL) return AGILE_UPGRADE_ERR;

    uint32_t addr = STM32_FLASH_START_ADRESS + *(const uint32_t *)agu->user_data;
    if (addr + agu->len > STM32_FLASH_END_ADDRESS) return AGILE_UPGRADE_ERR;

    agu->ops_data = agu->user_data;

    return AGILE_UPGRADE_EOK;
}

static int adapter_read(agile_upgrade_t *agu, int offset, uint8_t *buf, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    uint32_t addr = STM32_FLASH_START_ADRESS + *(const uint32_t *)agu->ops_data;
    if (offset < 0) {
        addr = addr + agu->len + offset;
    } else {
        addr += offset;
    }

    return stm32_flash_read(addr, buf, size);
}

static int adapter_write(agile_upgrade_t *agu, int offset, const uint8_t *buf, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    uint32_t addr = STM32_FLASH_START_ADRESS + *(const uint32_t *)agu->ops_data;
    if (offset < 0) {
        addr = addr + agu->len + offset;
    } else {
        addr += offset;
    }

    return stm32_flash_write(addr, buf, size);
}

static int adapter_erase(agile_upgrade_t *agu, int offset, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    uint32_t addr = STM32_FLASH_START_ADRESS + *(const uint32_t *)agu->ops_data;
    if (offset < 0) {
        addr = addr + agu->len + offset;
    } else {
        addr += offset;
    }

    return stm32_flash_erase(addr, size);
}

static int adapter_deinit(agile_upgrade_t *agu) {
    agu->ops_data = NULL;

    return AGILE_UPGRADE_EOK;
}

const struct agile_upgrade_ops agile_upgrade_onchip_ops = {
    adapter_init, adapter_read, adapter_write, adapter_erase, adapter_deinit};
