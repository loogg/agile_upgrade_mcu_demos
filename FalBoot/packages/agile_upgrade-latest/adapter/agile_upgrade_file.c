/**
 * @file    agile_upgrade_file.c
 * @brief   Agile Upgrade 软件包 file 适配源文件
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
#include <stdlib.h>

#ifdef AGILE_UPGRADE_ENABLE_FILE

#include <unistd.h>
#include <fcntl.h>

static int adapter_init(agile_upgrade_t *agu) {
    agu->ops_data = NULL;
    const char *path = (const char *)agu->user_data;
    if (path == NULL) return AGILE_UPGRADE_ERR;

    int *ptr = malloc(sizeof(int));
    if (ptr == NULL) return AGILE_UPGRADE_ERR;

    int fd = open(path, O_CREAT | O_RDWR);
    if (fd < 0) {
        free(ptr);
        return AGILE_UPGRADE_ERR;
    }

    *ptr = fd;
    agu->len = -1;
    agu->ops_data = (const void *)ptr;

    return AGILE_UPGRADE_EOK;
}

static int adapter_read(agile_upgrade_t *agu, int offset, uint8_t *buf, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    int fd = *(const int *)agu->ops_data;
    if (fd < 0) return AGILE_UPGRADE_ERR;

    if (lseek(fd, offset, (offset < 0) ? SEEK_END : SEEK_SET) < 0) return AGILE_UPGRADE_EREAD;

    return read(fd, buf, size);
}

static int adapter_write(agile_upgrade_t *agu, int offset, const uint8_t *buf, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    int fd = *(const int *)agu->ops_data;
    if (fd < 0) return AGILE_UPGRADE_ERR;

    if (lseek(fd, (offset < 0) ? 0 : offset, (offset < 0) ? SEEK_END : SEEK_SET) < 0)
        return AGILE_UPGRADE_EWRITE;

    return write(fd, buf, size);
}

static int adapter_erase(agile_upgrade_t *agu, int offset, int size) {
    if (agu->ops_data == NULL) return AGILE_UPGRADE_ERR;

    int fd = *(const int *)agu->ops_data;
    if (fd < 0) return AGILE_UPGRADE_ERR;

    if (ftruncate(fd, 0) < 0) return AGILE_UPGRADE_EERASE;

    if (lseek(fd, 0, SEEK_SET) < 0) return AGILE_UPGRADE_EERASE;

    return size;
}

static int adapter_deinit(agile_upgrade_t *agu) {
    if (agu->ops_data != NULL) {
        int fd = *(const int *)agu->ops_data;
        if (fd >= 0) close(fd);
        free((void *)agu->ops_data);
    }

    agu->ops_data = NULL;

    return AGILE_UPGRADE_EOK;
}

const struct agile_upgrade_ops agile_upgrade_file_ops = {adapter_init, adapter_read, adapter_write,
                                                         adapter_erase, adapter_deinit};

#endif /* AGILE_UPGRADE_ENABLE_FILE */
