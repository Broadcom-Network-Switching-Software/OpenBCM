/*! \file rm_alpm_util.c
 *
 * Utility functions for alpm
 *
 * This file contains the implementation for utilities
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */

#include <bsl/bsl.h>
#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_pt.h>

#include "rm_alpm.h"
#include "rm_alpm_device.h"
#include "rm_alpm_util.h"
#include "rm_alpm_level1.h"
#include "rm_alpm_leveln.h"

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMPTM_RMALPM

#define ALPM_SHIFT_LEFT(val, count) (((count) == 32) ? 0 : (val) << (count))
#define ALPM_SHIFT_RIGHT(val, count) (((count) == 32) ? 0 : (val) >> (count))


/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Translate mask into mask_len
 *
 * \param [in] u Device u.
 * \param [in] mask IP address mask
 * \param [out] mask_len IP mask len
 *
 * \return SHR_E_XXX.
 */
static inline int
ipmask2len(int u, int m, uint32_t mask, int *mask_len, int msb)
{
    while (mask & (1 << msb)) {
        *mask_len += 1;
        mask <<= 1;
    }

    if (msb < 31) {
        mask &= (1 << (msb + 1)) - 1;
    }

    return ((mask) ? SHR_E_PARAM : SHR_E_NONE);
}


/*!
 * \brief Shift ip_addr left or right to format_ip.
 *
 *  Src IP : hi -> lo : msb -> lsb
 *  Dst IP : hi -> lo : msb -> lsb  if reverse = false
 *  Dst IP: lo -> hi : msb -> lsb  if reverse = true
 *
 *  Dst ip assumed to have 4 elements.
 *
 * \param [in] src_ip Source IP address
 * \param [out] dst_ip Destination IP address
 * \param [in] shift Shift right if positive, else shift left.
 * \param [in] reverse If true reverse elements in dst_ip.
 *
 * \return SHR_E_NONE.
 */
static int
unified_key_shift(alpm_ip_t src_ip, alpm_ip_t dst_ip, int shift, bool reverse, int words)
{
    int start, left_shift;
    int right_shift;
    int i, j, msb, lsb;
    int incr;
    if (shift >= 0)  { /* shift right */
        right_shift = shift;
        start = right_shift / 32;
        right_shift = right_shift % 32;

        j = reverse ? (words -1)  : 0;
        incr = reverse ? -1 : 1;
        for (i = start; i < words; i++) { /* lo --> hi */
            /* lsb part */
            lsb = ALPM_SHIFT_RIGHT(src_ip[i], right_shift);
            /* msb part */
            if (i == (words - 1)) {
                msb = 0;
            } else {
                msb = src_ip[i + 1] & ((1 << right_shift) - 1);
                msb = ALPM_SHIFT_LEFT(msb, 32 - right_shift);
            }
            /* concat & assign */
            dst_ip[j] = msb | lsb;
            j += incr;
        }
    } else { /* shift left */
        left_shift = -shift;
        if (left_shift == (words * 32)) {
            sal_memset(dst_ip, 0, sizeof(alpm_ip_t));
        } else {
            ALPM_ASSERT(left_shift < (words * 32));
            if (reverse) {
                start = (left_shift / 32);
                left_shift = left_shift % 32;

                j = words - 1;
                for (i = start; i < words; i++) {
                    /* lsb part */
                    msb = ALPM_SHIFT_LEFT(src_ip[i], left_shift);
                    /* msb part */
                    if (i == (words - 1)) {
                        lsb = 0;
                    } else {
                        lsb = ALPM_SHIFT_RIGHT(src_ip[i + 1], 32 - left_shift);
                    }
                    /* concat & assign */
                    dst_ip[j] = msb | lsb;
                    j--;
                }
            } else {
                start = (words - 1) - (left_shift / 32);
                left_shift = left_shift % 32;

                j = 0;
                for (i = start; i >= 0; i--) {
                    /* lsb part */
                    msb = ALPM_SHIFT_LEFT(src_ip[i], left_shift);
                    /* msb part */
                    if (i == 0) {
                        lsb = 0;
                    } else {
                        lsb = ALPM_SHIFT_RIGHT(src_ip[i - 1], 32 - left_shift);
                    }
                    /* concat & assign */
                    dst_ip[j] = msb | lsb;
                    j++;
                }
            }
        }
    }
    return SHR_E_NONE;
}

static int
precise_key_bits(int u, int m, int v6, uint8_t app, alpm_vrf_type_t vt)
{
    int key_bits;
    if (app == APP_LPM || app == APP_COMP0) {
        key_bits = v6 ? 128 : 32;
        if (bcmptm_rm_alpm_is_large_vrf(u, m) && vt != VRF_OVERRIDE) {
            key_bits = 288;
        }
        return key_bits;
    } else if (app == APP_L3MC) {
        return 288;
    } else if (app == APP_COMP1) {
        if (bcmptm_rm_alpm_comp_key_type(u, m) == COMP_KEY_FULL) {
            key_bits = 180;
        } else {
            key_bits = v6 ? 128 : 32;
        }
        return key_bits;
    } else {
        assert(0);
        return 0;
    }
}


/*******************************************************************************
 * Public Functions
 */
const char *
bcmptm_rm_alpm_kt_name(int u, int m, alpm_key_type_t kt)
{
    char *name;
    switch (kt) {
    case KEY_IPV4:
        name = "IPv4";
        break;
    case KEY_IPV6_SINGLE:
        name = "IPv6-32";
        break;
    case KEY_IPV6_DOUBLE:
        name = "IPv6-64";
        break;
    case KEY_IPV6_QUAD:
        name = "IPv6-128";
        break;
    case KEY_L3MC_V4:
        name = "L3MC_V4";
        break;
    case KEY_L3MC_V6:
        name = "L3MC_V6";
        break;
    case KEY_COMP0_V4:
        name = "COMP0_V4";
        break;
    case KEY_COMP0_V6:
        name = "COMP0_V6";
        break;
    case KEY_COMP1_V4:
        name = "COMP1_V4";
        break;
    case KEY_COMP1_V6:
        name = "COMP1_V6";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}

const char *
bcmptm_rm_alpm_vt_name(int u, int m, alpm_vrf_type_t vt)
{
    char *name;
    switch (vt) {
    case VRF_PRIVATE:
        name = "private";
        break;
    case VRF_GLOBAL:
        name = "global";
        break;
    case VRF_OVERRIDE:
        name = "override";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}


const char *
bcmptm_rm_alpm_app_name(int u, int m, alpm_app_type_t app)
{
    char *name;
    switch (app) {
    case APP_LPM:
        name = "L3UC";
        break;
    case APP_L3MC:
        name = "L3MC";
        break;
    case APP_COMP0:
    case APP_COMP1:
        name = "FP_COMP";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}

const char *
bcmptm_rm_alpm_pkm_name(int u, int m, int pkm)
{
    char *name;
    switch (pkm) {
    case PKM_S:
        name = "SINGLE";
        break;
    case PKM_D:
        name = "DOUBLE";
        break;
    case PKM_Q:
        name = "QUAD";
        break;
    case PKM_WS:
        name = "WIDE_SINGLE";
        break;
    case PKM_WD:
        name = "WIDE_DOUBLE";
        break;
    case PKM_WQ:
        name = "WIDE_QUAD";
        break;
    case PKM_SS:
        name = "SINGLE_SRC";
        break;
    case PKM_SD:
        name = "DOUBLE_SRC";
        break;
    case PKM_SQ:
        name = "QUAD_SRC";
        break;
    case PKM_FP:
        name = "FP_COMP_DST";
        break;
    case PKM_SFP:
        name = "FP_COMP_SRC";
        break;
    default:
        name = "?";
        break;
    }
    return name;
}

int
bcmptm_rm_alpm_write(int u, int m, bcmdrd_sid_t sid,
                     int index, uint32_t *entry_words,
                     size_t wsize,
                     bcmptm_rm_op_t op_type,
                     uint32_t dt_bitmap)
{
    bcmbd_pt_dyn_info_t dyn_info;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    int cache_dt;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u, m);
    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;
    cache_dt = bcmptm_rm_alpm_cache_data_type(u, m);

    dyn_info.index      = index;
    dyn_info.tbl_inst   = ctrl->inst;

#ifdef ALPM_DEBUG
    if (LOG_CHECK_DEBUG(BSL_LOG_MODULE)) {
        static uint32_t accumulated_size = 0;
        static uint32_t curr_cseq_id = -1;
        if (curr_cseq_id == ctrl->cseq_id) {
            accumulated_size += wsize;
        } else {
            if (accumulated_size > 1000) {
                cli_out("Cseqid %d write size %d\n",
                        curr_cseq_id, accumulated_size);
            }
            accumulated_size = 0;
            curr_cseq_id = ctrl->cseq_id;
        }
    } while(0);
#endif

    SHR_IF_ERR_EXIT(
        bcmptm_cmdproc_write(u,
                             ctrl->req_flags,
                             ctrl->cseq_id,
                             sid,
                             &dyn_info,
                             NULL, /* misc_info */
                             entry_words,
                             ctrl->write_hw,
                             ctrl->write_cache,
                             ctrl->set_cache_vbit,
                             op_type, NULL,
                             ctrl->req_ltid,
                             cache_dt ? dt_bitmap : 0,
                             &ctrl->rsp_flags));
exit:
    SHR_FUNC_EXIT();
}

int
bcmptm_rm_alpm_read(int u, int m, bcmdrd_sid_t sid,
                    int index, size_t wsize, uint32_t *entry_words,
                    uint32_t *dt_bitmap)
{
    bcmbd_pt_dyn_info_t dyn_info;
    alpm_info_t         *alpm_info;
    alpm_cmd_ctrl_t     *ctrl;
    uint16_t            bitmap;
    SHR_FUNC_ENTER(u);

    alpm_info = bcmptm_rm_alpm_info_get(u, m);

    SHR_NULL_CHECK(alpm_info, SHR_E_PARAM);
    ctrl = &alpm_info->cmd_ctrl;

    dyn_info.index    = index;
    dyn_info.tbl_inst = ctrl->inst;

    SHR_IF_ERR_EXIT(
        bcmptm_cmdproc_read(u,
                            ctrl->req_flags,
                            ctrl->cseq_id,
                            sid,
                            &dyn_info,
                            NULL, /* misc_info */
                            wsize,
                            entry_words,
                            &ctrl->rsp_entry_cache_vbit,
                            &ctrl->rsp_ltid,
                            &bitmap,
                            &ctrl->rsp_flags));
    if (dt_bitmap) {
        *dt_bitmap = bitmap;
    }
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_arg_init(int u, int m, alpm_arg_t *arg)
{
    int i;

    if (arg) {
        sal_memset(arg, 0, sizeof(alpm_arg_t));
        for (i = 0; i < LEVELS; i++) {
            arg->index[i] = INVALID_INDEX;
            arg->log_bkt[i] = INVALID_LOG_BKT;
        }
        arg->lpm_large_vrf = bcmptm_rm_alpm_is_large_vrf(u, m);
        arg->comp_key_type = bcmptm_rm_alpm_comp_key_type(u, m);
    }
}


int
bcmptm_rm_alpm_len_to_mask(int u, int m, int max_bits, int len, uint32_t *mask)
{
    int max = ((max_bits + 31) / 32) - 1;
    int msb = (max_bits % 32);
    int i;
    uint32_t mask2;

    for (i = max; i >= 0; i--) {
        if (i == max && msb != 0) {
            mask2 = (1 << msb) - 1;
            if (len <= msb) {
                mask[i--] = ((1 << len) -1) << (msb - len);
                break;
            }
            mask[i] = mask2;
            len -= msb;
        } else {
            if (len < 32) {
                mask[i--] = ~ (0xffffffff >> len);
                break;
            } else if (len == 32) {
                mask[i--] = 0xffffffff;
                break;
            }
            mask[i] = 0xffffffff;
            len -= 32;
        }

    }

    while (i >= 0) {
        mask[i--] = 0;
    }
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_mask_to_len(int u, int m, int max_bits, uint32_t *mask, int *len)
{
    int i = 0;
    int msb = (max_bits % 32);
    int max = (max_bits + 31) / 32;
    int rv;

    if (msb == 0) {
        msb = 31;
    } else {
        msb--;
    }

    *len = 0;
    while (i < max && *len == 0) {
        if (mask[i]) {
            rv = ipmask2len(u, m, mask[i], len, (i == max - 1) ? msb : 31);
            if (SHR_FAILURE(rv)) {
                ALPM_ASSERT(0);
                return rv;
            }
        }
        i++;
    }

    while (i < max) {
        if (i == max - 1 && msb != 31) {
            /*
             * if (mask[i] + 1 != (1 << (msb + 1)) ) {
             *     return SHR_E_PARAM;
             * }
             */
            *len += msb + 1;
        } else {
            if (mask[i] != 0xFFFFFFFF) {
                return SHR_E_PARAM;
            }
            *len += 32;
        }
        i++;
    }
    return SHR_E_NONE;
}

/*
static void dump_api(uint32_t *api, int words, char *prefix)
{
    int i;
    cli_out("%s", prefix);
    for (i = 0; i < words; i++) {
        cli_out(" 0x%x", api[i]);
    }
}
static void dump_key(uint32_t *api, int words, char *prefix)
{
    int i;
    cli_out("%s", prefix);
    for (i = words -1 ; i >= 0; i--) {
        cli_out(" 0x%x", api[i]);
    }
}
*/
int
bcmptm_rm_alpm_trie_key_create(int u, int m, int bits, alpm_ip_t ip_addr, uint32_t len,
                               trie_ip_t *key)
{
    int key_words, api_words;

    key_words = (bits / 32) + 1;
    api_words = (bits + 31) /32;
/*    cli_out("Bits=%d Len=%d ", bits, len);
    dump_api(&ip_addr[0], api_words, "Before: API"); */
    unified_key_shift(&ip_addr[0], &key->key[key_words - api_words], bits - len, true, api_words);
/*    dump_key(&key->key[0], key_words, " ==> Trie_key");
    cli_out("\n"); */
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_api_key_create(int u, int m, int bits, trie_ip_t *key, uint32_t len,
                              alpm_ip_t ip_addr)
{
    int key_words, api_words;

    key_words = (bits / 32) + 1;
    api_words = (bits + 31) /32;
/*    cli_out("Bits=%d Len=%d ", bits, len); */
/*    dump_key(&key->key[0], key_words, "Before: Trie_key"); */
    unified_key_shift(&key->key[key_words - api_words], &ip_addr[0], len - bits, true, api_words);
/*    dump_api(&ip_addr[0], api_words, "==> API"); */
/*    cli_out("\n"); */
    return SHR_E_NONE;
}


int
bcmptm_rm_alpm_trie_mkl(int u, int m, int v6, uint8_t app, alpm_vrf_type_t vt)
{
    return precise_key_bits(u, m, v6, app, vt) + 1;
}

int
bcmptm_rm_alpm_max_key_bits(int u, int m, int v6, uint8_t app, alpm_vrf_type_t vt)
{
    return precise_key_bits(u, m, v6, app, vt);
}

void
bcmptm_rm_alpm_ipaddr_str(char *ipstr, alpm_ip_ver_t ipv, alpm_ip_t ipaddr)
{
    if (ipv == IP_VER_4) {
        sal_sprintf(ipstr, "%d.%d.%d.%d",
            (ipaddr[0] >> 24) & 0xff, (ipaddr[0] >> 16) & 0xff,
            (ipaddr[0] >> 8) & 0xff, ipaddr[0] & 0xff);
    } else {
        sal_sprintf(ipstr, "%04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x",
            ((ipaddr[3] >> 16) & 0xffff), (ipaddr[3] & 0xffff),
            ((ipaddr[2] >> 16) & 0xffff), (ipaddr[2] & 0xffff),
            ((ipaddr[1] >> 16) & 0xffff), (ipaddr[1] & 0xffff),
            ((ipaddr[0] >> 16) & 0xffff), (ipaddr[0] & 0xffff));
    }
}

