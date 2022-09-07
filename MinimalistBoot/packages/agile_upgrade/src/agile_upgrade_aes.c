/**
 * @file    agile_upgrade_aes.c
 * @brief   Agile Upgrade 软件包 AES 源文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-09-01
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#include "agile_upgrade.h"
#include "agile_upgrade_def.h"

#ifdef AGILE_UPGRADE_ENABLE_AES

#include <tinycrypt_config.h>
#include <tinycrypt.h>
#include <string.h>

static tiny_aes_context _ctx;
static uint8_t _iv[16 + 1];
static uint8_t _key[32 + 1];

void agile_upgrade_aes_init(void) {
    memset(&_ctx, 0, sizeof(_ctx));
    memset(_iv, 0, sizeof(_iv));
    memset(_key, 0, sizeof(_key));

    int len = strlen(AGILE_UPGRADE_AES_IV);
    if (len > 16) len = 16;
    memcpy(_iv, AGILE_UPGRADE_AES_IV, len);

    len = strlen(AGILE_UPGRADE_AES_KEY);
    if (len > 32) len = 32;
    memcpy(_key, AGILE_UPGRADE_AES_KEY, len);

    tiny_aes_setkey_dec(&_ctx, _key, 256);
}

int agile_upgrade_aes_decrypt(uint8_t *input, uint8_t *output, int len) {
    if (len % 16 != 0) {
        LOG_E("AES length is not a multiple of 16");
        return AGILE_UPGRADE_ERR;
    }

    tiny_aes_crypt_cbc(&_ctx, AES_DECRYPT, len, _iv, input, output);

    return len;
}

#endif /* AGILE_UPGRADE_ENABLE_AES */
