/**
 * @file    agile_upgrade_aes.h
 * @brief   Agile Upgrade 软件包 AES 头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-09-01
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_AES_H
#define __AGILE_UPGRADE_AES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void agile_upgrade_aes_init(void);
int agile_upgrade_aes_decrypt(uint8_t *input, uint8_t *output, int len);

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_AES_H */
