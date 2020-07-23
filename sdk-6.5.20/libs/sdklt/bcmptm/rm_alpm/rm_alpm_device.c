/*! \file rm_alpm_device.c
 *
 * RM ALPM device specific info
 *
 * This file contains device specific info which are internal to
 * ALPM Resource Manager
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

#include "rm_alpm_device.h"

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMPTM_RMALPM


/*******************************************************************************
 * Private variables
 */
static alpm_dev_info_t *alpm_dev_info[BCMDRD_CONFIG_MAX_UNITS][ALPMS];
static bcmptm_rm_alpm_driver_t *alpm_driver[BCMDRD_CONFIG_MAX_UNITS][ALPMS];


/*******************************************************************************
 * Private Functions
 */
/*!
 * \brief Populate device info into data structure
 *
 * \param [in] u Device u.
 * \param [out] dev Device info data structure
 *
 * \return SHR_E_XXX.
 */
static int
rm_alpm_device_info_init(int u, int m, alpm_dev_info_t *dev)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, device_info_init)) {
        rv = ALPM_DRV_EXEC(u, m, device_info_init)(u, m, dev);
    }
    return rv;
}

/*******************************************************************************
 * Public Functions
 */
uint8_t
bcmptm_rm_alpm_hit_version(int u, int m)
{
    if (alpm_dev_info[u][m]) {
        return alpm_dev_info[u][m]->hit_ver;
    }
    return ALPM_HIT_VERSION_0;
}

alpm_dev_info_t *
bcmptm_rm_alpm_device_info_get(int u, int m)
{
    return ALPM_DEV(u, m);
}

int
bcmptm_rm_alpm_l1_encode(int u, int m, alpm_arg_t *arg, void *de, void *se, uint8_t l1v)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_encode)) {
        rv = ALPM_DRV_EXEC(u, m, l1_encode) (u, m, arg, de, se, l1v);
    }
    return rv;
}

int
bcmptm_rm_alpm_l1_decode(int u, int m, alpm_arg_t *arg, void *de, void *se, uint8_t l1v)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_decode)) {
        rv = ALPM_DRV_EXEC(u, m, l1_decode) (u, m, arg, de, se, l1v);
    }
    return rv;
}

int
bcmptm_rm_alpm_l1_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_sid_index_map)) {
        rv = ALPM_DRV_EXEC(u, m, l1_sid_index_map) (u, m, map_info);
    }
    return rv;
}


int
bcmptm_rm_alpm_ln_sid_index_map(int u, int m, alpm_sid_index_map_t *map_info)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_sid_index_map)) {
        rv = ALPM_DRV_EXEC(u, m, ln_sid_index_map) (u, m, map_info);
    }
    return rv;
}

int
bcmptm_rm_alpm_l1_half_entry_move(int u, int m, void *src, void *dst,
                                  int x, int y, int pkm)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_half_entry_move)) {
        rv = ALPM_DRV_EXEC(u, m, l1_half_entry_move)(u, m, src, dst, x, y, pkm);
    }
    return rv;
}
int
bcmptm_rm_alpm_l1_half_key_clear(int u, int m, void *de, int x, int pkm)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_half_key_clear)) {
        rv = ALPM_DRV_EXEC(u, m, l1_half_key_clear)(u, m, de, x, pkm);
    }
    return rv;
}
int
bcmptm_rm_alpm_l1_half_key_move(int u, int m, void *src, void *dst,
                                  int x, int y, int pkm)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_half_key_move)) {
        rv = ALPM_DRV_EXEC(u, m, l1_half_key_move)(u, m, src, dst, x, y, pkm);
    }
    return rv;
}

int
bcmptm_rm_alpm_l1_sid_index_reverse_map(int u, int m, bcmdrd_sid_t hw_sid,
                                        alpm_sid_index_out_t *out_info)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, l1_sid_index_reverse_map)) {
        rv = ALPM_DRV_EXEC(u, m, l1_sid_index_reverse_map) (u, m, hw_sid, out_info);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_encode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_base_entry_encode)) {
        rv = ALPM_DRV_EXEC(u, m, ln_base_entry_encode)(u, m, format_type,
                                                    format_value,
                                                    kshift,
                                                    arg,
                                                    entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_decode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    uint8_t kshift,
                                    alpm_arg_t *arg,
                                    uint32_t *entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_base_entry_decode)) {
        rv = ALPM_DRV_EXEC(u, m, ln_base_entry_decode)(u, m, format_type,
                                                    format_value,
                                                    kshift,
                                                    arg,
                                                    entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_raw_bucket_encode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    ln_base_entry_t *base_entry,
                                    int entry_count,
                                    void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_raw_bucket_encode)) {
        rv = ALPM_DRV_EXEC(u, m, ln_raw_bucket_encode)(u, m, format_type,
                                                    format_value,
                                                    base_entry,
                                                    entry_count,
                                                    bucket);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_raw_bucket_decode(int u, int m, format_type_t format_type,
                                    uint8_t format_value,
                                    ln_base_entry_t *base_entry,
                                    int entry_count,
                                    void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_raw_bucket_decode)) {
        rv = ALPM_DRV_EXEC(u, m, ln_raw_bucket_decode)(u, m, format_type,
                                                    format_value,
                                                    base_entry,
                                                    entry_count,
                                                    bucket);
    }
    return rv;
}
int
bcmptm_rm_alpm_ln_raw_bucket_entry_set(int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_raw_bucket_entry_set)) {
        rv = ALPM_DRV_EXEC(u, m, ln_raw_bucket_entry_set)(u, m, format_type,
                                                       format_value,
                                                       base_entry,
                                                       ent_ofs,
                                                       bucket);
    }
    return rv;
}


int
bcmptm_rm_alpm_ln_raw_bucket_entry_get(int u, int m, format_type_t format_type,
                                       uint8_t format_value,
                                       uint32_t *base_entry,
                                       int ent_ofs,
                                       void *bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_raw_bucket_entry_get)) {
        rv = ALPM_DRV_EXEC(u, m, ln_raw_bucket_entry_get)(u, m, format_type,
                                                       format_value,
                                                       base_entry,
                                                       ent_ofs,
                                                       bucket);
    }
    return rv;
}


int
bcmptm_rm_alpm_ln_base_entry_info(int u, int m,
                                  format_type_t format_type,
                                  uint8_t format_value,
                                  int *prefix_len,
                                  int *full,
                                  int *num_entry)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_base_entry_info)) {
        rv = ALPM_DRV_EXEC(u, m, ln_base_entry_info)(u, m,
                                                  format_type,
                                                  format_value,
                                                  prefix_len,
                                                  full,
                                                  num_entry);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_base_entry_move(int u, int m, format_type_t format_type,
                                  uint8_t src_format_value,
                                  int src_ent_ofs,
                                  key_tuple_t *src_t,
                                  void *src_bucket,
                                  alpm_data_type_t src_dt,

                                  uint8_t dst_format_value,
                                  int dst_ent_ofs,
                                  key_tuple_t *dst_t,
                                  void *dst_bucket)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_base_entry_move)) {
        rv = ALPM_DRV_EXEC(u, m, ln_base_entry_move)(u, m, format_type,
                                                  src_format_value,
                                                  src_ent_ofs,
                                                  src_t,
                                                  src_bucket,
                                                  src_dt,

                                                  dst_format_value,
                                                  dst_ent_ofs,
                                                  dst_t,
                                                  dst_bucket);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_hit_get(int u, int m, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t *hit)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_hit_get)) {
        rv = ALPM_DRV_EXEC(u, m, ln_hit_get)(u, ln, ofs, hit_entry, hit);
    }
    return rv;
}

int
bcmptm_rm_alpm_ln_hit_set(int u, int m, int ln, uint8_t ofs, uint32_t *hit_entry,
                          uint8_t hit)
{
    int rv = SHR_E_UNAVAIL;
    if (ALPM_DRV(u, m) && ALPM_DRV_EXEC(u, m, ln_hit_set)) {
        rv = ALPM_DRV_EXEC(u, m, ln_hit_set)(u, ln, ofs, hit_entry, hit);
    }
    return rv;
}

int
bcmptm_rm_alpm_wvrf_get(int u, int m, int app, alpm_vrf_type_t vt, int decode_vrf)
{

    int alpm_mode = bcmptm_rm_alpm_mode(u, m);
    int w_vrf = 0;
    int max_private_vrf;

    if (bcmptm_rm_alpm_is_large_vrf(u, m)) {
        if (app == APP_LPM) {
            decode_vrf = 0;
        }
        max_private_vrf = 0;
    } else {
        if (app == APP_COMP0 || app == APP_COMP1) {
            max_private_vrf = alpm_dev_info[u][m]->max_fp_op;
        } else {
            max_private_vrf = alpm_dev_info[u][m]->max_vrf;
        }
    }

    if (app == APP_COMP1) {
        if (vt == VRF_GLOBAL) {
            if (bcmptm_rm_alpm_comp_key_type(u, m) == COMP_KEY_FULL) {
                w_vrf = decode_vrf; /* must be 0, share with Private */
            } else {
                w_vrf = (max_private_vrf + 1);
            }
        } else if (vt == VRF_PRIVATE) {
            w_vrf = decode_vrf;
        }
    } else if (app == APP_L3MC) {
        w_vrf = (max_private_vrf + 1);
    } else if (app == APP_LPM) {
        if (alpm_mode & ALPM_MODE_PARALLEL) {
        /*
         * Parallel :
         * DB0: Private, L3MC (+1)
         * DB1: Global (0), OVERRIDE (1)
         */
            if (vt == VRF_OVERRIDE) {
                w_vrf = 1;
            } else if (vt == VRF_GLOBAL) {
                w_vrf = 0;
            } else if (vt == VRF_PRIVATE) {
                w_vrf = decode_vrf;
            }
        } else {
        /* Combined : Private, L3MC (+1), Global (+2), OVERRIDE (+3) */
            if (vt == VRF_OVERRIDE) {
                w_vrf = (max_private_vrf + 3);
            } else if (vt == VRF_GLOBAL) {
                if (bcmptm_rm_alpm_is_large_vrf(u, m)) {
                    w_vrf = decode_vrf; /* must be 0, share with Private */
                } else {
                    w_vrf = (max_private_vrf + 2);
                }
            } else if (vt == VRF_PRIVATE) {
                w_vrf = decode_vrf;
            }
        }
    } else if (app == APP_COMP0) {
        if (vt == VRF_GLOBAL) {
            if (bcmptm_rm_alpm_is_large_l4_port(u, m)) {
                w_vrf = decode_vrf; /* must be 0, share with Private */
            } else {
                w_vrf = (max_private_vrf + 1);
            }
        } else if (vt == VRF_PRIVATE) {
            w_vrf = decode_vrf;
        }
    }

    if (app == APP_COMP0 || app == APP_COMP1) {
        assert(w_vrf < WVRF_COUNT_FP(alpm_dev_info[u][m]->max_fp_op));
    } else {
        if ((alpm_mode & ALPM_MODE_PARALLEL) &&
            (vt == VRF_OVERRIDE || vt == VRF_GLOBAL)) {
            assert(w_vrf < WVRF_COUNT_PARALLEL_PUBLIC);
        } else {
            assert(w_vrf < WVRF_COUNT(alpm_dev_info[u][m]->max_vrf));
        }
    }

    return w_vrf;
}


int
bcmptm_rm_alpm_device_init(int u, int m)
{
    int rv;
    SHR_FUNC_ENTER(u);

    ALPM_ALLOC(ALPM_DEV(u, m), sizeof(alpm_dev_info_t), "bcmptmRmalpmDev");
    sal_memset(ALPM_DEV(u, m), 0, sizeof(alpm_dev_info_t));

    ALPM_ALLOC(ALPM_DRV(u, m), sizeof(bcmptm_rm_alpm_driver_t), "bcmptmRmalpmDrv");
    sal_memset(ALPM_DRV(u, m), 0, sizeof(bcmptm_rm_alpm_driver_t));

    rv = bcmptm_rm_alpm_driver_register(u, ALPM_DRV(u, m));
    if (rv == SHR_E_UNAVAIL) {
        LOG_INFO(BSL_LOG_MODULE,
                  (BSL_META_U(u, "RM ALPM driver missing.\n")));
        SHR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    } else {
        SHR_IF_ERR_EXIT(rv);
    }
    SHR_IF_ERR_EXIT(rm_alpm_device_info_init(u, m, ALPM_DEV(u, m)));
exit:
    SHR_FUNC_EXIT();
}

void
bcmptm_rm_alpm_device_cleanup(int u, int m)
{
    SHR_FREE(ALPM_DEV(u, m));
    SHR_FREE(ALPM_DRV(u, m));
}


