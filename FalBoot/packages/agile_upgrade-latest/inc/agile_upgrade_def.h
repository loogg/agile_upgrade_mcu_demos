/**
 * @file    agile_upgrade_def.h
 * @brief   Agile Upgrade 软件包定义头文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-29
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#ifndef __AGILE_UPGRADE_DEF_H
#define __AGILE_UPGRADE_DEF_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "agile_upgrade_config.h"

#ifdef AGILE_UPGRADE_ENABLE_AES
#ifndef AGILE_UPGRADE_AES_IV
#define AGILE_UPGRADE_AES_IV "0123456789ABCDEF"
#endif /* AGILE_UPGRADE_AES_IV */
#ifndef AGILE_UPGRADE_AES_KEY
#define AGILE_UPGRADE_AES_KEY "0123456789ABCDEF0123456789ABCDEF"
#endif /* AGILE_UPGRADE_AES_KEY */
#endif /* AGILE_UPGRADE_ENABLE_AES */

#define AGILE_UPGRADE_NAME_NULL "UNKNOWN"

#ifdef AGILE_UPGRADE_ENABLE_LOG
#ifndef AGILE_UPGRADE_LOG_PRINTF
#define AGILE_UPGRADE_LOG_PRINTF(...)
#endif /* AGILE_UPGRADE_LOG_PRINTF */
#define LOG_D(...)                                   \
    do {                                             \
        AGILE_UPGRADE_LOG_PRINTF("\033[0m[D/AUG] "); \
        AGILE_UPGRADE_LOG_PRINTF(__VA_ARGS__);       \
        AGILE_UPGRADE_LOG_PRINTF("\033[0m\n");       \
    } while (0)

#define LOG_I(...)                                    \
    do {                                              \
        AGILE_UPGRADE_LOG_PRINTF("\033[32m[I/AUG] "); \
        AGILE_UPGRADE_LOG_PRINTF(__VA_ARGS__);        \
        AGILE_UPGRADE_LOG_PRINTF("\033[0m\n");        \
    } while (0)

#define LOG_W(...)                                    \
    do {                                              \
        AGILE_UPGRADE_LOG_PRINTF("\033[33m[W/AUG] "); \
        AGILE_UPGRADE_LOG_PRINTF(__VA_ARGS__);        \
        AGILE_UPGRADE_LOG_PRINTF("\033[0m\n");        \
    } while (0)

#define LOG_E(...)                                    \
    do {                                              \
        AGILE_UPGRADE_LOG_PRINTF("\033[31m[E/AUG] "); \
        AGILE_UPGRADE_LOG_PRINTF(__VA_ARGS__);        \
        AGILE_UPGRADE_LOG_PRINTF("\033[0m\n");        \
    } while (0)
#else
#define LOG_D(...)
#define LOG_I(...)
#define LOG_W(...)
#define LOG_E(...)
#endif /* AGILE_UPGRADE_ENABLE_LOG */

#ifdef __cplusplus
}
#endif

#endif /* __AGILE_UPGRADE_DEF_H */
