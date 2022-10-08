/**
 * @file    agile_upgrade.c
 * @brief   Agile Upgrade 软件包源文件
 * @author  马龙伟 (2544047213@qq.com)
 * @date    2022-08-25
 *
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2022 Ma Longwei.
 * All rights reserved.</center></h2>
 *
 */

#include "agile_upgrade.h"
#include "agile_upgrade_def.h"
#include "agile_upgrade_crc.h"
#include "agile_upgrade_aes.h"
#include "agile_upgrade_fastlz.h"
#include "agile_upgrade_quicklz.h"
#include <string.h>

#define AGILE_UPGRADE_BUFSZ 4096

#if defined(AGILE_UPGRADE_ENABLE_FASTLZ) || defined(AGILE_UPGRADE_ENABLE_QUICKLZ)
#define AGILE_PGRADE_CMPRS_BUFSZ (AGILE_UPGRADE_BUFSZ + 1024)
#endif /* defined(AGILE_UPGRADE_ENABLE_FASTLZ) || defined(AGILE_UPGRADE_ENABLE_QUICKLZ) */

#define AGILE_UPGRADE_STEP_HOOK(step)     \
    do {                                  \
        if (_step_hook) _step_hook(step); \
    } while (0)

#define AGILE_UPGRADE_PROGRESS_HOOK(cur_size, total_size)         \
    do {                                                          \
        if (_progress_hook) _progress_hook(cur_size, total_size); \
    } while (0)

#define AGILE_UPGRADE_ERROR_HOOK(step, code)      \
    do {                                          \
        if (_error_hook) _error_hook(step, code); \
    } while (0)

static agile_upgrade_step_callback_t _step_hook = NULL;
static agile_upgrade_progress_callback_t _progress_hook = NULL;
static agile_upgrade_error_callback_t _error_hook = NULL;

static uint8_t _fw_buf[AGILE_UPGRADE_BUFSZ];

#if defined(AGILE_UPGRADE_ENABLE_AES) || defined(AGILE_UPGRADE_ENABLE_FASTLZ) || \
    defined(AGILE_UPGRADE_ENABLE_QUICKLZ)
static uint8_t _fw_algo_buf1[AGILE_UPGRADE_BUFSZ];
#endif /* defined(AGILE_UPGRADE_ENABLE_AES) || defined(AGILE_UPGRADE_ENABLE_FASTLZ) || \
          defined(AGILE_UPGRADE_ENABLE_QUICKLZ) */

#if defined(AGILE_UPGRADE_ENABLE_FASTLZ) || defined(AGILE_UPGRADE_ENABLE_QUICKLZ)
static uint8_t _fw_algo_buf2[AGILE_PGRADE_CMPRS_BUFSZ];
#endif /* defined(AGILE_UPGRADE_ENABLE_FASTLZ) || defined(AGILE_UPGRADE_ENABLE_QUICKLZ) */

static void agile_upgrade_print_progress(uint32_t cur_size, uint32_t total_size) {
    static uint8_t progress_sign[100 + 1];
    uint8_t i, per = cur_size * 100 / total_size;

    if (per > 100) {
        per = 100;
    }

    for (i = 0; i < 100; i++) {
        if (i < per) {
            progress_sign[i] = '=';
        } else if (i == per) {
            progress_sign[i] = '>';
        } else {
            progress_sign[i] = ' ';
        }
    }

    progress_sign[sizeof(progress_sign) - 1] = '\0';

    LOG_I("\033[2A");
    LOG_I("Write: [%s] %d%%", progress_sign, per);
}

static int agile_upgrade_internal_verify(agile_upgrade_t *agu, agile_upgrade_fw_info_t *fw,
                                         uint8_t is_tail) {
    uint32_t calc_crc = 0;
    int info_off = 0, body_off = 0;
    int total_len = 0;

    if (agu->ops == NULL || agu->ops->read == NULL) return AGILE_UPGRADE_ERR;

    if (is_tail) {
        info_off = -(int)sizeof(agile_upgrade_fw_info_t);
        body_off = 0;
    } else {
        info_off = 0;
        body_off = sizeof(agile_upgrade_fw_info_t);
    }

    if (agu->ops->read(agu, info_off, (uint8_t *)fw, sizeof(agile_upgrade_fw_info_t)) !=
        sizeof(agile_upgrade_fw_info_t)) {
        LOG_E("Name[%s] read firmware info failed.", agu->name);
        return AGILE_UPGRADE_EREAD;
    }

    agile_upgrade_crc32_init(&calc_crc);
    agile_upgrade_crc32_update(&calc_crc, (uint8_t *)fw, sizeof(agile_upgrade_fw_info_t) - 4);
    agile_upgrade_crc32_final(&calc_crc);
    if (calc_crc != fw->hdr_crc) {
        LOG_E("Name[%s] head CRC32 error!", agu->name);
        return AGILE_UPGRADE_EVERIFY;
    }

    if (strncmp(fw->type, "RBL", 3) != 0) {
        LOG_E("Name[%s] type[%s] not surport.", agu->name, fw->type);
        return AGILE_UPGRADE_EVERIFY;
    }

    if (agu->len > 0) {
        if (fw->pkg_size + body_off - info_off > agu->len) {
            LOG_E("Name[%s] len (%d) is smaller than (%d).", agu->name, agu->len,
                  fw->pkg_size + body_off - info_off);
            return AGILE_UPGRADE_ERR;
        }
    }

    agile_upgrade_crc32_init(&calc_crc);
    while (total_len < fw->pkg_size) {
        int read_len = agu->ops->read(agu, body_off + total_len, _fw_buf,
                                      (fw->pkg_size - total_len > AGILE_UPGRADE_BUFSZ)
                                          ? AGILE_UPGRADE_BUFSZ
                                          : (fw->pkg_size - total_len));
        if (read_len < 0) {
            LOG_E("Name[%s] read body failed.", agu->name);
            return AGILE_UPGRADE_EREAD;
        }
        if (read_len == 0) break;

        agile_upgrade_crc32_update(&calc_crc, _fw_buf, read_len);
        total_len += read_len;
    }
    agile_upgrade_crc32_final(&calc_crc);

    if (calc_crc != fw->body_crc) {
        LOG_E(
            "Get firmware header occur CRC32(calc.crc: %08X != hdr.info_crc32: %08X) error on "
            "name \'%s\'!",
            calc_crc, fw->body_crc, agu->name);
        return AGILE_UPGRADE_EVERIFY;
    }

    LOG_I("Verify name \'%s\' (fw ver: %s, timestamp: %u) success.", agu->name, fw->version_name,
          fw->time_stamp);

    return AGILE_UPGRADE_EOK;
}

static int agile_upgrade_algo_init(agile_upgrade_fw_info_t *fw) {
    int crypt_type = fw->algo & AGILE_UPGRADE_CRYPT_ALGO_MASK;
    int cmprs_type = fw->algo & AGILE_UPGRADE_CMPRS_ALGO_MASK;

    switch (crypt_type) {
        case AGILE_UPGRADE_ALGO_NONE:
            break;

#ifdef AGILE_UPGRADE_ENABLE_AES
        case AGILE_UPGRADE_CRYPT_ALGO_AES256:
            agile_upgrade_aes_init();
            break;
#endif

        default: {
            LOG_E("Unsupported crypt_type: %d", crypt_type);
            return AGILE_UPGRADE_EINVAL;
        }
    }

    switch (cmprs_type) {
        case AGILE_UPGRADE_ALGO_NONE:
            break;

#ifdef AGILE_UPGRADE_ENABLE_FASTLZ
        case AGILE_UPGRADE_CMPRS_ALGO_FASTLZ:
            agile_upgrade_fastlz_init();
            break;
#endif

#ifdef AGILE_UPGRADE_ENABLE_QUICKLZ
        case AGILE_UPGRADE_CMPRS_ALGO_QUICKLZ:
            agile_upgrade_quicklz_init();
            break;
#endif

        default: {
            LOG_E("Unsupported cmprs_type: %d", cmprs_type);
            return AGILE_UPGRADE_EINVAL;
        }
    }

    return AGILE_UPGRADE_EOK;
}

static int agile_upgrade_algo_read(agile_upgrade_t *agu, int pos, int max_len, int crypt_type) {
    int len = 0;

    if (agu->ops == NULL || agu->ops->read == NULL) return AGILE_UPGRADE_ERR;

    switch (crypt_type) {
        case AGILE_UPGRADE_ALGO_NONE:
            len = agu->ops->read(agu, pos, _fw_buf, max_len);
            break;

#ifdef AGILE_UPGRADE_ENABLE_AES
        case AGILE_UPGRADE_CRYPT_ALGO_AES256: {
            len = agu->ops->read(agu, pos, _fw_algo_buf1, max_len);
            if (len <= 0) break;
            len = agile_upgrade_aes_decrypt(_fw_algo_buf1, _fw_buf, max_len);
        } break;
#endif

        default: {
            LOG_E("Unsupported crypt_type: %d", crypt_type);
            return AGILE_UPGRADE_EINVAL;
        }
    }

    return len;
}

static int agile_upgrade_algo_write(agile_upgrade_t *agu, uint32_t *calc_crc, int pos,
                                    int total_len, int max_len, int cmprs_type) {
    int len = 0;

    if (agu->ops == NULL || agu->ops->write == NULL) return AGILE_UPGRADE_ERR;

    switch (cmprs_type) {
        case AGILE_UPGRADE_ALGO_NONE: {
            if (pos + total_len > max_len) total_len = max_len - pos;
            if (total_len <= 0) return total_len;

            len = agu->ops->write(agu, pos, _fw_buf, total_len);
            if (len != total_len) return AGILE_UPGRADE_EWRITE;

            agile_upgrade_crc32_update(calc_crc, _fw_buf, len);
        } break;

#ifdef AGILE_UPGRADE_ENABLE_FASTLZ
        case AGILE_UPGRADE_CMPRS_ALGO_FASTLZ:
#endif
#ifdef AGILE_UPGRADE_ENABLE_QUICKLZ
        case AGILE_UPGRADE_CMPRS_ALGO_QUICKLZ:
#endif
#if defined(AGILE_UPGRADE_ENABLE_FASTLZ) || defined(AGILE_UPGRADE_ENABLE_QUICKLZ)
        {
            int remain_len = total_len;
            int rc = 0;

            while (remain_len > 0) {
                if (pos == max_len) break;

                if (cmprs_type == AGILE_UPGRADE_CMPRS_ALGO_FASTLZ) {
#ifdef AGILE_UPGRADE_ENABLE_FASTLZ
                    rc = agile_upgrade_fastlz_decompress(
                        _fw_buf + total_len - remain_len, _fw_algo_buf2, AGILE_PGRADE_CMPRS_BUFSZ,
                        _fw_algo_buf1, AGILE_UPGRADE_BUFSZ, remain_len, &remain_len);
#endif
                } else {
#ifdef AGILE_UPGRADE_ENABLE_QUICKLZ
                    rc = agile_upgrade_quicklz_decompress(
                        _fw_buf + total_len - remain_len, _fw_algo_buf2, AGILE_PGRADE_CMPRS_BUFSZ,
                        _fw_algo_buf1, AGILE_UPGRADE_BUFSZ, remain_len, &remain_len);
#endif
                }

                if (rc <= 0) break;

                if (pos + rc > max_len) rc = max_len - pos;
                if (rc <= 0) break;

                if (agu->ops->write(agu, pos, _fw_algo_buf1, rc) != rc) return AGILE_UPGRADE_EWRITE;

                agile_upgrade_crc32_update(calc_crc, _fw_algo_buf1, rc);
                pos += rc;
                len += rc;
            }

            if (rc < 0) return AGILE_UPGRADE_EWRITE;
        } break;
#endif

        default: {
            LOG_E("Unsupported cmprs_type: %d", cmprs_type);
            return AGILE_UPGRADE_EINVAL;
        }
    }

    return len;
}

void agile_upgrade_set_step_hook(agile_upgrade_step_callback_t hook) { _step_hook = hook; }

void agile_upgrade_set_progress_hook(agile_upgrade_progress_callback_t hook) {
    _progress_hook = hook;
}

void agile_upgrade_set_error_hook(agile_upgrade_error_callback_t hook) { _error_hook = hook; }

int agile_upgrade_verify(agile_upgrade_t *agu, agile_upgrade_fw_info_t *fw_info, uint8_t is_tail) {
    agile_upgrade_fw_info_t fw = {0};
    int rc = AGILE_UPGRADE_ERR;

    if (agu->ops == NULL) return AGILE_UPGRADE_ERR;

    if (agu->name == NULL) agu->name = AGILE_UPGRADE_NAME_NULL;

    do {
        if (agu->ops->init) {
            rc = agu->ops->init(agu);
            if (rc != AGILE_UPGRADE_EOK) {
                LOG_E("Name[%s] init failed.", agu->name);
                break;
            }
        }

        rc = agile_upgrade_internal_verify(agu, &fw, is_tail);
        if (rc != AGILE_UPGRADE_EOK) break;
        if (fw_info) *fw_info = fw;

        rc = AGILE_UPGRADE_EOK;
    } while (0);

    if (agu->ops->deinit) agu->ops->deinit(agu);

    return rc;
}

int agile_upgrade_release(agile_upgrade_t *src_agu, agile_upgrade_t *dst_agu, uint8_t is_erase) {
    agile_upgrade_fw_info_t src_fw = {0}, dst_fw = {0};
    int step = AGILE_UPGRADE_STEP_INIT;
    int rc = AGILE_UPGRADE_ERR;
    uint32_t calc_crc = 0;
    int read_pos = 0;
    int write_pos = 0;

    if (src_agu == NULL || dst_agu == NULL) return AGILE_UPGRADE_ERR;
    if (src_agu->ops == NULL || dst_agu->ops == NULL) return AGILE_UPGRADE_ERR;
    if (dst_agu->ops->write == NULL) return AGILE_UPGRADE_ERR;

    do {
        /* AGILE_UPGRADE_STEP_INIT */
        step = AGILE_UPGRADE_STEP_INIT;
        AGILE_UPGRADE_STEP_HOOK(step);
        if (src_agu->name == NULL) src_agu->name = AGILE_UPGRADE_NAME_NULL;
        if (dst_agu->name == NULL) dst_agu->name = AGILE_UPGRADE_NAME_NULL;
        if (src_agu->ops->init) {
            rc = src_agu->ops->init(src_agu);
            if (rc != AGILE_UPGRADE_EOK) {
                LOG_E("Name[%s] init failed.", src_agu->name);
                break;
            }
        }
        if (dst_agu->ops->init) {
            rc = dst_agu->ops->init(dst_agu);
            if (rc != AGILE_UPGRADE_EOK) {
                LOG_E("Name[%s] init failed.", dst_agu->name);
                break;
            }
        }

        /* AGILE_UPGRADE_STEP_VERIFY */
        step = AGILE_UPGRADE_STEP_VERIFY;
        AGILE_UPGRADE_STEP_HOOK(step);
        rc = agile_upgrade_internal_verify(src_agu, &src_fw, 0);
        if (rc != AGILE_UPGRADE_EOK) break;
        if (dst_agu->len > 0) {
            if (src_fw.raw_size + sizeof(agile_upgrade_fw_info_t) > dst_agu->len) {
                LOG_E("Name[%s] len (%d) is smaller than (%d).", dst_agu->name, dst_agu->len,
                      src_fw.raw_size + sizeof(agile_upgrade_fw_info_t));
                rc = AGILE_UPGRADE_EFULL;
                break;
            }
        }

        /* AGILE_UPGRADE_STEP_ALGO */
        step = AGILE_UPGRADE_STEP_ALGO;
        AGILE_UPGRADE_STEP_HOOK(step);
        rc = agile_upgrade_algo_init(&src_fw);
        if (rc != AGILE_UPGRADE_EOK) break;

        /* AGILE_UPGRADE_STEP_UPGRADE */
        step = AGILE_UPGRADE_STEP_UPGRADE;
        AGILE_UPGRADE_STEP_HOOK(step);
        rc = agile_upgrade_internal_verify(dst_agu, &dst_fw, 1);
#ifdef AGILE_UPGRADE_ENABLE_VERSION_COMPARE
        if (rc == AGILE_UPGRADE_EOK && strcmp(src_fw.version_name, dst_fw.version_name) == 0) {
            LOG_I("Name[%s] and name[%s] have the same version: %s", src_agu->name, dst_agu->name,
                  dst_fw.version_name);

            step = AGILE_UPGRADE_STEP_FINISH;
            AGILE_UPGRADE_STEP_HOOK(step);
            break;
        }
#endif
        if (rc == AGILE_UPGRADE_EOK) {
            LOG_I("Name[%s] upgrade(%s->%s) startup.", dst_agu->name, dst_fw.version_name,
                  src_fw.version_name);
        } else {
            LOG_I("Name[%s] upgrade startup.", dst_agu->name);
        }

        if (dst_agu->ops->erase) {
            LOG_I("Name[%s] erasing...", dst_agu->name);
            if (dst_agu->ops->erase(dst_agu, 0, src_fw.raw_size) < 0 ||
                dst_agu->ops->erase(dst_agu, -(int)sizeof(agile_upgrade_fw_info_t),
                                    sizeof(agile_upgrade_fw_info_t)) < 0) {
                LOG_E("Name[%s] erase failed.", dst_agu->name);
                rc = AGILE_UPGRADE_EERASE;
                break;
            }
            LOG_I("Name[%s] erase success.\r\n", dst_agu->name);
        }

        rc = AGILE_UPGRADE_EOK;
        agile_upgrade_crc32_init(&calc_crc);
        while (read_pos < src_fw.pkg_size) {
            int read_len = agile_upgrade_algo_read(
                src_agu, sizeof(agile_upgrade_fw_info_t) + read_pos,
                (src_fw.pkg_size - read_pos > AGILE_UPGRADE_BUFSZ) ? AGILE_UPGRADE_BUFSZ
                                                                   : (src_fw.pkg_size - read_pos),
                src_fw.algo & AGILE_UPGRADE_CRYPT_ALGO_MASK);
            if (read_len < 0) {
                LOG_E("Name[%s] read body failed.", src_agu->name);
                rc = AGILE_UPGRADE_EREAD;
                break;
            }
            if (read_len == 0) break;
            read_pos += read_len;

            int write_len =
                agile_upgrade_algo_write(dst_agu, &calc_crc, write_pos, read_len, src_fw.raw_size,
                                         src_fw.algo & AGILE_UPGRADE_CMPRS_ALGO_MASK);
            if (write_len < 0) {
                LOG_E("Name[%s] write body failed.", dst_agu->name);
                rc = AGILE_UPGRADE_EWRITE;
                break;
            }
            write_pos += write_len;

            if (write_len > 0) {
                agile_upgrade_print_progress(write_pos, src_fw.raw_size);
                AGILE_UPGRADE_PROGRESS_HOOK(write_pos, src_fw.raw_size);
            }
        }
        agile_upgrade_crc32_final(&calc_crc);
        if (rc != AGILE_UPGRADE_EOK) break;
        if (write_pos != src_fw.raw_size) {
            LOG_E("Name[%s] write len (%d) is not equal to (%d).", dst_agu->name, write_pos,
                  src_fw.raw_size);
            rc = AGILE_UPGRADE_ERR;
            break;
        }

        /* update dst_fw and verify dst_agu */
        dst_fw = src_fw;
        dst_fw.algo = AGILE_UPGRADE_ALGO_NONE;
        dst_fw.algo2 = 0;
        dst_fw.body_crc = calc_crc;
        dst_fw.hash_code = 0;
        dst_fw.pkg_size = dst_fw.raw_size;
        agile_upgrade_crc32_init(&calc_crc);
        agile_upgrade_crc32_update(&calc_crc, (uint8_t *)&dst_fw,
                                   sizeof(agile_upgrade_fw_info_t) - 4);
        agile_upgrade_crc32_final(&calc_crc);
        dst_fw.hdr_crc = calc_crc;
        LOG_I("Name[%s] writing firmware info.", dst_agu->name);
        if (dst_agu->ops->write(dst_agu, -(int)sizeof(agile_upgrade_fw_info_t), (uint8_t *)&dst_fw,
                                sizeof(agile_upgrade_fw_info_t)) !=
            sizeof(agile_upgrade_fw_info_t)) {
            LOG_E("Name[%s] write firmware info failed.", dst_agu->name);
            rc = AGILE_UPGRADE_EWRITE;
            break;
        }
        LOG_I("Name[%s] write firmware info success.", dst_agu->name);
        rc = agile_upgrade_internal_verify(dst_agu, &dst_fw, 1);
        if (rc != AGILE_UPGRADE_EOK) break;

        /* AGILE_UPGRADE_STEP_FINISH */
        step = AGILE_UPGRADE_STEP_FINISH;
        AGILE_UPGRADE_STEP_HOOK(step);

        rc = AGILE_UPGRADE_EOK;
    } while (0);

    if (step == AGILE_UPGRADE_STEP_FINISH) {
        if (is_erase && src_agu->ops->erase) {
            LOG_I("Name[%s] erasing...", src_agu->name);
            if (src_agu->ops->erase(src_agu, 0, sizeof(agile_upgrade_fw_info_t) + src_fw.pkg_size) <
                0) {
                LOG_W("Name[%s] erase failed.", src_agu->name);
            } else {
                LOG_I("Name[%s] erase success.", src_agu->name);
            }
        }
    }

    if (src_agu->ops->deinit) src_agu->ops->deinit(src_agu);
    if (dst_agu->ops->deinit) dst_agu->ops->deinit(dst_agu);

    if (rc != AGILE_UPGRADE_EOK) {
        AGILE_UPGRADE_ERROR_HOOK(step, rc);
    }

    return rc;
}
