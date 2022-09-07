/**
 * @file    agile_upgrade_quicklz.c
 * @brief   Agile Upgrade 软件包 quicklz 源文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-09-05
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#include "agile_upgrade.h"
#include "agile_upgrade_def.h"

#ifdef AGILE_UPGRADE_ENABLE_QUICKLZ

#include <quicklz.h>
#include <string.h>

#define AGILE_UPGRADE_QUICKLZ_BLOCK_HDR_SIZE 4

enum {
    QUICKLZ_DECOMPRESS_STEP_HDR = 0,
    QUICKLZ_DECOMPRESS_STEP_BODY,
    QUICKLZ_DECOMPRESS_STEP_FINISH
};

static qlz_state_decompress _decmprs_state;
static int _step = QUICKLZ_DECOMPRESS_STEP_HDR;
static int _data_len = 0;
static int _len_to_read = 0;
static uint8_t _hdr_buf[AGILE_UPGRADE_QUICKLZ_BLOCK_HDR_SIZE];

void agile_upgrade_quicklz_init(void) {
    memset(&_decmprs_state, 0, sizeof(_decmprs_state));
    _step = QUICKLZ_DECOMPRESS_STEP_HDR;
    _data_len = 0;
    _len_to_read = AGILE_UPGRADE_QUICKLZ_BLOCK_HDR_SIZE;
}

static int agile_upgrade_quicklz_get_block_len(void) {
    int len = 0;

    for (int i = 0; i < AGILE_UPGRADE_QUICKLZ_BLOCK_HDR_SIZE; i++) {
        len <<= 8;
        len |= _hdr_buf[i];
    }

    return len;
}

int agile_upgrade_quicklz_decompress(uint8_t *buf, uint8_t *cmprs_buf, int cmprs_bufsz,
                                     uint8_t *decmprs_buf, int decmprs_bufsz, int total_len,
                                     int *remain_len) {
    int rc = 0;
    uint8_t *ptr = buf;
    *remain_len = 0;

    while (total_len > 0) {
        switch (_step) {
            case QUICKLZ_DECOMPRESS_STEP_HDR: {
                if (total_len < _len_to_read) {
                    memcpy(_hdr_buf + _data_len, ptr, total_len);
                    rc = total_len;
                    _data_len += rc;
                    _len_to_read -= rc;
                } else {
                    memcpy(_hdr_buf + _data_len, ptr, _len_to_read);
                    rc = _len_to_read;
                    _data_len = 0;
                    _len_to_read = agile_upgrade_quicklz_get_block_len();
                    if (_len_to_read < 0 || _len_to_read > cmprs_bufsz) return AGILE_UPGRADE_ERR;
                    _step = QUICKLZ_DECOMPRESS_STEP_BODY;
                }
            } break;

            case QUICKLZ_DECOMPRESS_STEP_BODY: {
                if (total_len < _len_to_read) {
                    memcpy(cmprs_buf + _data_len, ptr, total_len);
                    rc = total_len;
                    _data_len += rc;
                    _len_to_read -= rc;
                } else {
                    memcpy(cmprs_buf + _data_len, ptr, _len_to_read);
                    rc = _len_to_read;
                    _step = QUICKLZ_DECOMPRESS_STEP_FINISH;
                }
            } break;
        }

        ptr += rc;
        total_len -= rc;

        if (_step == QUICKLZ_DECOMPRESS_STEP_FINISH) {
            _step = QUICKLZ_DECOMPRESS_STEP_HDR;
            _data_len = 0;
            _len_to_read = AGILE_UPGRADE_QUICKLZ_BLOCK_HDR_SIZE;
            *remain_len = total_len;

            return qlz_decompress((const char *)cmprs_buf, decmprs_buf, &_decmprs_state);
        }
    }

    return 0;
}

#endif /* AGILE_UPGRADE_ENABLE_QUICKLZ */
