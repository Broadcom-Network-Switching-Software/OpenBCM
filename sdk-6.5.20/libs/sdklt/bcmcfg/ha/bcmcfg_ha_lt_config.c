/*! \file bcmcfg_ha_lt_config.c
 *
 * BCMCFG HA(High Availability) related functions for config LT data.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <shr/shr_debug.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_ha_internal.h>
#include <bcmcfg/comp/bcmcfg_cfg_error_control.h>
#include <bcmcfg/generated/bcmcfg_ha_config.h>
#include <bcmdrd_config.h>
#include <bcmltd/chip/bcmltd_limits.h>
#include <bcmltd/bcmltd_table.h>
#include <bcmlrd/bcmlrd_table.h>
#include <bcmlrd/bcmlrd_client.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmissu/issu_api.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_HA

/*******************************************************************************
 * Private variables
 */

static bcmcfg_ha_alloc_info_t bcmcfg_ha_lt_config[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private Functions
 */

/*!
 * \brief Get bcmcfg config data size.
 *
 * Get bcmcfg config data size.
 *
 * \param [in]    unit       Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_ha_data_lt_config_size_get(int unit)
{
    uint32_t sid;
    uint32_t fid;
    bcmcfg_config_data_t *user_config;
    uint32_t req_size = 0;
    size_t nfields;
    int rv;

    for (sid = 0; sid < BCMLTD_TABLE_COUNT; sid++) {
        rv = bcmcfg_tbl_user_config_get(unit, sid, &user_config);
        if (SHR_FAILURE(rv) ||
            user_config == NULL) {
            continue;
        }

        nfields = 0;
        rv = bcmlrd_field_count_get(unit, sid, &nfields);
        if (SHR_FAILURE(rv)) {
            continue;
        }

        for (fid = 0; fid < nfields; fid++) {
            uint32_t cnt;
            uint32_t first_idx;
            uint32_t fidx;
            uint32_t idx;

            rv = bcmltd_field_depth_get(sid, fid, &cnt);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            rv = bcmcfg_tbl_user_config_field_index_get(unit, sid, fid,
                                                       0, &first_idx);
            if (SHR_FAILURE(rv)) {
                continue;
            }
            for (fidx = 0; fidx < cnt; fidx++) {
                idx = first_idx + fidx;
                if ((user_config[idx].config_src == BCMCFG_CONFIG_SRC_TYPE_READER ||
                     user_config[idx].config_src == BCMCFG_CONFIG_SRC_TYPE_API)) {
                     req_size += sizeof(bcmcfg_basic_tuple_t);
                }
            }
        }
    }

    return req_size;
}

/*!
 * \brief Report LT config issu structure.
 *
 * Report LT config issu structure.
 *
 * \param [in]    unit       Unit number.
 *
 * \retval 0  OK
 * \retval <0 ERROR
 */
static int
bcmcfg_ha_lt_config_issu_report(int unit)
{
    size_t array_size;
    uint32_t hdr_size = sizeof(bcmcfg_ha_header_t);
    uint32_t size = bcmcfg_ha_lt_config[unit].size;

    SHR_FUNC_ENTER(unit);

    array_size = size / sizeof(bcmcfg_basic_tuple_t);
    if (array_size > 0) {
        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                 BCMMGMT_CFG_COMP_ID,
                 BCMCFG_HA_SUBID_LT_CONFIG,
                 0,
                 hdr_size, 1,
                 BCMCFG_HA_HEADER_T_ID));

        SHR_IF_ERR_EXIT
            (bcmissu_struct_info_report(unit,
                 BCMMGMT_CFG_COMP_ID,
                 BCMCFG_HA_SUBID_LT_CONFIG,
                 hdr_size,
                 sizeof(bcmcfg_basic_tuple_t),
                 array_size,
                 BCMCFG_BASIC_TUPLE_T_ID));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Functions
 */
int
bcmcfg_ha_lt_config_init(int unit, bool warm)
{
    uint32_t req_size;
    uint8_t *alloc_ptr = NULL;
    bcmcfg_ha_header_t *hdr;

    SHR_FUNC_ENTER(unit);

    req_size = bcmcfg_ha_data_lt_config_size_get(unit);

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_unit_alloc(unit, warm, req_size,
                             "BCMCFG_LT_CONFIG",
                             BCMCFG_HA_SUBID_LT_CONFIG,
                             "bcmcfgLtCfg",
                             BCMCFG_HA_SIGN_LT_CONFIG,
                             &alloc_ptr));
    hdr = (bcmcfg_ha_header_t *) alloc_ptr;
    bcmcfg_ha_lt_config[unit].alloc_ptr = alloc_ptr;
    bcmcfg_ha_lt_config[unit].size = hdr->size;

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_lt_config_save(int unit, bool warm)
{
    uint32_t sid, fid;
    bcmcfg_config_data_t *user_config;
    size_t nfields;
    int rv;
    bcmcfg_basic_tuple_t tup;
    uint8_t *ha_ptr = bcmcfg_ha_lt_config[unit].alloc_ptr;
    uint32_t size = bcmcfg_ha_lt_config[unit].size;
    uint32_t offset;

    SHR_FUNC_ENTER(unit);

    if (size == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(
        bcmcfg_ha_lt_config_issu_report(unit));

    SHR_IF_ERR_EXIT(
        bcmissu_struct_info_get(unit,
                                BCMCFG_BASIC_TUPLE_T_ID,
                                BCMMGMT_CFG_COMP_ID,
                                BCMCFG_HA_SUBID_LT_CONFIG,
                                &offset));
    ha_ptr += offset;

    for (sid = 0; sid < BCMLTD_TABLE_COUNT; sid++) {

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_get(unit, sid, &user_config));
        if (user_config == NULL) {
            continue;
        }

        rv = bcmlrd_field_count_get(unit, sid, &nfields);
        if (SHR_E_UNAVAIL == rv) {
            continue;
        } else {
            SHR_IF_ERR_EXIT(rv);
        }

        tup.sid = sid;
        for (fid = 0; fid < nfields; fid++) {
            uint32_t cnt;
            uint32_t first_idx;
            uint32_t fidx;
            uint32_t idx;

            SHR_IF_ERR_EXIT(
                bcmltd_field_depth_get(sid, fid, &cnt));

            SHR_IF_ERR_EXIT(
                bcmcfg_tbl_user_config_field_index_get(unit, sid, fid,
                                                       0, &first_idx));
            for (fidx = 0; fidx < cnt; fidx++) {
                idx = first_idx + fidx;
                if ((user_config[idx].config_src == BCMCFG_CONFIG_SRC_TYPE_READER ||
                     user_config[idx].config_src == BCMCFG_CONFIG_SRC_TYPE_API)) {
                    tup.fid = fid;
                    tup.fidx = fidx;
                    tup.val = user_config[idx].value;
                    tup.src_type = user_config[idx].config_src;
                    tup.rsvd = 0;

                    sal_memcpy(ha_ptr, &tup, sizeof(tup));

                    ha_ptr += sizeof(tup)/sizeof(uint8_t);
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmcfg_ha_lt_config_restore(int unit, bool warm)
{
    uint32_t sid, fid, fidx;
    bcmcfg_config_data_t *user_config;
    bcmcfg_basic_tuple_t *tup;
    uint8_t *ha_ptr = bcmcfg_ha_lt_config[unit].alloc_ptr;
    uint32_t size = bcmcfg_ha_lt_config[unit].size;
    uint32_t rsize = 0;
    uint32_t offset;
    size_t tup_size = sizeof(bcmcfg_basic_tuple_t);
    bcmcfg_cfg_error_control_config_t ec;
    uint32_t out_idx;

    SHR_FUNC_ENTER(unit);

    if (size == 0) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(
        bcmissu_struct_info_get(unit,
                                BCMCFG_BASIC_TUPLE_T_ID,
                                BCMMGMT_CFG_COMP_ID,
                                BCMCFG_HA_SUBID_LT_CONFIG,
                                &offset));
    ha_ptr += offset;

    while (rsize < size) {
        tup = (bcmcfg_basic_tuple_t *) ha_ptr;
        sid = tup->sid;
        fid = tup->fid;
        fidx = tup->fidx;
        if (sid == BCMLTD_SID_INVALID ||
            fid == BCMLTD_SID_INVALID ||
            !(tup->src_type == BCMCFG_CONFIG_SRC_TYPE_READER ||
              tup->src_type == BCMCFG_CONFIG_SRC_TYPE_API)) {
            ha_ptr += tup_size /sizeof(uint8_t);
            rsize += tup_size;
            continue;
        }

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_get(unit, sid, &user_config));
        if (user_config == NULL) {
            SHR_IF_ERR_EXIT(
                bcmcfg_config_table_field_alloc(unit, sid, &user_config));
        }
        /* Update HA contents if in ISSU. */
        if (bcmcfg_lt_init_is_issu(unit)) {
            const char *enum_type = NULL;
            uint32_t new_val = 0;
            int rv;

            rv = bcmlrd_enum_symbol_name(unit, sid, fid, &enum_type);
            if (SHR_SUCCESS(rv)) {
                rv = bcmissu_enum_sym_to_current(unit, enum_type,
                                                 (uint32_t) tup->val, &new_val);
                if (SHR_FAILURE(rv)) {
                    int tmp_rv;

                    tmp_rv = bcmcfg_cfg_error_control_config_get(&ec);
                    /* If stop_playback_on_error is not set, ignore error. */
                    if (SHR_SUCCESS(tmp_rv) && !ec.stop_playback_on_error) {
                        ha_ptr += tup_size /sizeof(uint8_t);
                        rsize += tup_size;
                        continue;
                    }
                    SHR_IF_ERR_EXIT(rv);
                }
                tup->val = (uint64_t) new_val;
            }
        }

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_field_index_get(unit,
                                                   sid,
                                                   fid,
                                                   fidx,
                                                   &out_idx));
        user_config[out_idx].value = tup->val;
        user_config[out_idx].config_src = tup->src_type;

        ha_ptr += tup_size /sizeof(uint8_t);
        rsize += tup_size;

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_hook(unit, sid, fid, tup->val));

        SHR_IF_ERR_EXIT(
            bcmcfg_tbl_user_config_set(unit, sid, user_config));
    }

exit:
    SHR_FUNC_EXIT();
}

