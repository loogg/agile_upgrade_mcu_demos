/**
 * @file    agile_upgrade_conf_template.h
 * @brief   Agile Upgrade 软件包配置头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-29
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_CONFIG_H
#define __AGILE_UPGRADE_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

#define AGILE_UPGRADE_USING_CRC_TAB_CONST
// #define AGILE_UPGRADE_ENABLE_VERSION_COMPARE

#define AGILE_UPGRADE_ENABLE_LOG
#define AGILE_UPGRADE_LOG_PRINTF printf

#define AGILE_UPGRADE_ENABLE_AES
#define AGILE_UPGRADE_AES_IV  "0123456789ABCDEF"
#define AGILE_UPGRADE_AES_KEY "0123456789ABCDEF0123456789ABCDEF"

#define AGILE_UPGRADE_ENABLE_FASTLZ
#define AGILE_UPGRADE_ENABLE_QUICKLZ

//#define AGILE_UPGRADE_ENABLE_FAL
#define AGILE_UPGRADE_ENABLE_FILE

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_CONFIG_H */
