/**
 * @file    agile_upgrade.h
 * @brief   Agile Upgrade 软件包头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-29
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_H
#define __AGILE_UPGRADE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    AGILE_UPGRADE_STEP_INIT = 0,
    AGILE_UPGRADE_STEP_VERIFY,
    AGILE_UPGRADE_STEP_ALGO,
    AGILE_UPGRADE_STEP_UPGRADE,
    AGILE_UPGRADE_STEP_FINISH
} agile_upgrade_step_t;

typedef enum {
    AGILE_UPGRADE_EOK = 0,
    AGILE_UPGRADE_ERR = -1,
    AGILE_UPGRADE_EVERIFY = -2,
    AGILE_UPGRADE_EFULL = -3,
    AGILE_UPGRADE_EREAD = -4,
    AGILE_UPGRADE_EWRITE = -5,
    AGILE_UPGRADE_EERASE = -6,
    AGILE_UPGRADE_EINVAL = -7
} agile_upgrade_err_t;

typedef enum {
    AGILE_UPGRADE_ALGO_NONE = 0x0L,
    AGILE_UPGRADE_CRYPT_ALGO_XOR = 0x1L,
    AGILE_UPGRADE_CRYPT_ALGO_AES256 = 0x2L,
    AGILE_UPGRADE_CMPRS_ALGO_GZIP = 0x1L << 8,
    AGILE_UPGRADE_CMPRS_ALGO_QUICKLZ = 0x2L << 8,
    AGILE_UPGRADE_CMPRS_ALGO_FASTLZ = 0x3L << 8,
    AGILE_UPGRADE_CRYPT_ALGO_MASK = 0xFL,
    AGILE_UPGRADE_CMPRS_ALGO_MASK = 0xFL << 8,
} agile_upgrade_algo_t;

typedef struct agile_upgrade_fw_info {
    char type[4];
    uint16_t algo;
    uint16_t algo2;
    uint32_t time_stamp;
    char app_part_name[16];
    char version_name[24];
    char res[24];
    uint32_t body_crc;
    uint32_t hash_code;
    uint32_t raw_size;
    uint32_t pkg_size;
    uint32_t hdr_crc;
} __attribute__((packed)) agile_upgrade_fw_info_t;

typedef struct agile_upgrade agile_upgrade_t;

struct agile_upgrade_ops {
    int (*init)(agile_upgrade_t *agu);
    int (*read)(agile_upgrade_t *agu, int offset, uint8_t *buf, int size);
    int (*write)(agile_upgrade_t *agu, int offset, const uint8_t *buf, int size);
    int (*erase)(agile_upgrade_t *agu, int offset, int size);
    int (*deinit)(agile_upgrade_t *agu);
};

struct agile_upgrade {
    const char *name;
    int len;
    const void *user_data;
    const struct agile_upgrade_ops *ops;
    const void *ops_data;
};

typedef void (*agile_upgrade_step_callback_t)(int step);
typedef void (*agile_upgrade_progress_callback_t)(uint32_t cur_size, uint32_t total_size);
typedef void (*agile_upgrade_error_callback_t)(int step, int code);

void agile_upgrade_set_step_hook(agile_upgrade_step_callback_t hook);
void agile_upgrade_set_progress_hook(agile_upgrade_progress_callback_t hook);
void agile_upgrade_set_error_hook(agile_upgrade_error_callback_t hook);

int agile_upgrade_verify(agile_upgrade_t *agu, agile_upgrade_fw_info_t *fw_info, uint8_t is_tail);
int agile_upgrade_release(agile_upgrade_t *src_agu, agile_upgrade_t *dst_agu, uint8_t is_erase);

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_H */
