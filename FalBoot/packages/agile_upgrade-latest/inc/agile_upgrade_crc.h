/**
 * @file    agile_upgrade_crc.h
 * @brief   Agile Upgrade 软件包 CRC 头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-25
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_CRC_H
#define __AGILE_UPGRADE_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void agile_upgrade_crc32_init(uint32_t *crc);
void agile_upgrade_crc32_update(uint32_t *crc, const uint8_t *data, int len);
void agile_upgrade_crc32_final(uint32_t *crc);

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_CRC_H */
