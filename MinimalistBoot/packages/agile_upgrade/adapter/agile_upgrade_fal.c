/**
 * @file    agile_upgrade_fal.c
 * @brief   Agile Upgrade 软件包 fal 适配源文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-30
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#include "agile_upgrade.h"
#include "agile_upgrade_def.h"

#ifdef AGILE_UPGRADE_ENABLE_FAL

#include <fal.h>

static int adapter_init(agile_upgrade_t *agu) {
    agu->ops_data = NULL;
    const char *name = (const char *)agu->user_data;
    if (name == NULL) return AGILE_UPGRADE_ERR;

    const struct fal_partition *part = fal_partition_find(name);
    if (part == NULL) return AGILE_UPGRADE_ERR;

    agu->len = part->len;
    agu->ops_data = (const void *)part;

    return AGILE_UPGRADE_EOK;
}

static int adapter_read(agile_upgrade_t *agu, int offset, uint8_t *buf, int size) {
    const struct fal_partition *part = (const struct fal_partition *)agu->ops_data;
    if (part == NULL) return AGILE_UPGRADE_ERR;

    if (offset < 0) offset += part->len;

    return fal_partition_read(part, offset, buf, size);
}

static int adapter_write(agile_upgrade_t *agu, int offset, const uint8_t *buf, int size) {
    const struct fal_partition *part = (const struct fal_partition *)agu->ops_data;
    if (part == NULL) return AGILE_UPGRADE_ERR;

    if (offset < 0) offset += part->len;

    return fal_partition_write(part, offset, buf, size);
}

static int adapter_erase(agile_upgrade_t *agu, int offset, int size) {
    const struct fal_partition *part = (const struct fal_partition *)agu->ops_data;
    if (part == NULL) return AGILE_UPGRADE_ERR;

    if (offset < 0) offset += part->len;

    return fal_partition_erase(part, offset, size);
}

static int adapter_deinit(agile_upgrade_t *agu) {
    agu->ops_data = NULL;

    return AGILE_UPGRADE_EOK;
}

const struct agile_upgrade_ops agile_upgrade_fal_ops = {adapter_init, adapter_read, adapter_write,
                                                        adapter_erase, adapter_deinit};

#endif /* AGILE_UPGRADE_ENABLE_FAL */
