/**
 * @file    agile_upgrade_fastlz.h
 * @brief   Agile Upgrade 软件包 fastlz 头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-09-04
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_FASTLZ_H
#define __AGILE_UPGRADE_FASTLZ_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

void agile_upgrade_fastlz_init(void);
int agile_upgrade_fastlz_decompress(uint8_t *buf, uint8_t *cmprs_buf, int cmprs_bufsz,
                                    uint8_t *decmprs_buf, int decmprs_bufsz, int total_len,
                                    int *remain_len);

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_FASTLZ_H */
