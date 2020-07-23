/*! \file l3_fib.c
 *
 * L3 Forwarding Information Base management.
 * This file contains the management for L3 FIB.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include <bcm/l3.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/control.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_fib_int.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/mbcm/l3_fib.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/lt_intf.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <bcmltd/chip/bcmltd_device_constants.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/* ALPM associated data mode in a VRF. */
typedef struct alpm_vrf_data_mode_s {
    /* IPv4 ALPM route data mode. */
    bcm_l3_vrf_route_data_mode_t v4_dmode;

    /* IPv6 ALPM route data mode. */
    bcm_l3_vrf_route_data_mode_t v6_dmode;

} alpm_vrf_data_mode_t;

typedef struct l3_fib_info_s {
    /* L3 fib manager initialized. */
    bool inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Max VRF. */
    uint16_t max_vrf_id;

    /* ALPM associated data mode. */
    alpm_vrf_data_mode_t *alpm_data_mode;

} l3_fib_info_t;

static l3_fib_info_t l3_fib_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_FIB_INITED(_u) (l3_fib_info[_u].inited)

#define L3_FIB_LOCK(_u)     \
        sal_mutex_take(l3_fib_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_FIB_UNLOCK(_u)   \
        sal_mutex_give(l3_fib_info[_u].mutex)

#define L3_VRF_MAX(_u) (l3_fib_info[_u].max_vrf_id)

/* ALPM VRF Data Mode Macros */
#define L3_ALPM_VRF_IDX_GLO(_u) (L3_VRF_MAX(_u) + 1)
#define L3_ALPM_VRF_IDX_GHI(_u) (L3_VRF_MAX(_u) + 2)
#define L3_ALPM_VRF_IDX_CNT(_u) (L3_VRF_MAX(_u) + 3)

#define L3_ALPM_VRF_TO_VRF_IDX(u, _vrf)                         \
    ((_vrf) == BCM_L3_VRF_OVERRIDE ? L3_ALPM_VRF_IDX_GHI(u) :   \
     (_vrf) == BCM_L3_VRF_GLOBAL ? L3_ALPM_VRF_IDX_GLO(u) :     \
     (_vrf))

#define L3_ALPM4_VRF_DATA_MODE(_u, _vrf_idx)                \
    (l3_fib_info[_u].alpm_data_mode[_vrf_idx].v4_dmode)

#define L3_ALPM4_VRF_DATA_MODE_SET(_u, _vrf_idx, _dmode)    \
    (l3_fib_info[_u].alpm_data_mode[_vrf_idx].v4_dmode = _dmode)

#define L3_ALPM6_VRF_DATA_MODE(_u, _vrf_idx)                \
    (l3_fib_info[_u].alpm_data_mode[_vrf_idx].v6_dmode)

#define L3_ALPM6_VRF_DATA_MODE_SET(_u, _vrf_idx, _dmode)    \
    (l3_fib_info[_u].alpm_data_mode[_vrf_idx].v6_dmode = _dmode)

static uint8 ip6_zero[BCM_IP6_ADDRLEN] = {0};

/* 5% of usage as minimum for ALPM route projection */
#define ALPM_PRJ_MIN_USAGE 5

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Get the capacity of given tables. (Unit is IPv4 unicast.)
 *
 * \param [in] unit Unit number.
 * \param [in] ltname Array of logical table name.
 * \param [in] ltcnt Number of elements in ltname array.
 * \param [out] Number of base entry.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_base_entries_get(int unit, const char **ltname, int ltcnt, int *entries)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    bcmlt_transaction_hdl_t trh = BCMLT_INVALID_HDL;
    bcmlt_entry_info_t ei;
    const char *em_group[DEVICE_EM_GROUP_CNT_MAX] = {0};
    const char *em_bank[DEVICE_EM_BANK_CNT_MAX] = {0};
    const char *grp_arr[DEVICE_EM_GROUP_CNT_MAX] = {0};
    const char *bk_arr[DEVICE_EM_BANK_CNT_MAX] = {0};
    uint64_t val;
    int dunit, i, j, n, grp = 0, bk = 0;
    uint32_t cnt, base_ent = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Retrieve EM_GROUP of given LT. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    for (i = 0; i < ltcnt; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, TABLE_EM_INFOs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, TABLE_IDs, ltname[i]));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));
        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < ltcnt; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_get(ei.entry_hdl,
                                                DEVICE_EM_GROUP_IDs,
                                                0, grp_arr,
                                                DEVICE_EM_GROUP_CNT_MAX, &cnt));

        for (n = 0; n < cnt; n++) {
            if (!(sal_strcmp(grp_arr[n], INVALIDs))) {
                continue;
            }
            for (j = 0; j < grp; j++) {
                if (!sal_strcmp(grp_arr[n], em_group[j])) {
                    break;
                }
            }
            if (j == grp) {
                if (grp == DEVICE_EM_GROUP_CNT_MAX) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                em_group[grp] = grp_arr[n];
                grp ++;

            }
        }
    }

    if (!grp) {
        *entries = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_free(trh));
    trh = BCMLT_INVALID_HDL;

    /* Retrieve fixed EM_BANK of given LT. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    for (i = 0; i < grp; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, DEVICE_EM_GROUP_INFOs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, DEVICE_EM_GROUP_IDs,
                                          em_group[i]));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));
        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < grp; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_get(ei.entry_hdl,
                                                FIXED_DEVICE_EM_BANK_IDs,
                                                0, bk_arr,
                                                DEVICE_EM_BANK_CNT_MAX, &cnt));
        for (n = 0; n < cnt; n++) {
            if (!(sal_strcmp(bk_arr[n], INVALIDs))) {
                continue;
            }
            for (j = 0; j < bk; j++) {
                if (!sal_strcmp(bk_arr[n], em_bank[j])) {
                    break;
                }
            }
            if (j == bk) {
                if (bk == DEVICE_EM_BANK_CNT_MAX) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                em_bank[bk] = bk_arr[n];
                bk ++;
            }
        }
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_free(trh));
    trh = BCMLT_INVALID_HDL;

    /* Retrieve flexible EM_BANK of given LT. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    for (i = 0; i < grp; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, DEVICE_EM_GROUPs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, DEVICE_EM_GROUP_IDs,
                                          em_group[i]));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));
        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < grp; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_array_symbol_get(ei.entry_hdl,
                                                DEVICE_EM_BANK_IDs, 0, bk_arr,
                                                DEVICE_EM_BANK_CNT_MAX, &cnt));
        for (n = 0; n < cnt; n++) {
            if (!(sal_strcmp(bk_arr[n], INVALIDs))) {
                continue;
            }
            for (j = 0; j < bk; j++) {
                if (!sal_strcmp(bk_arr[n], em_bank[j])) {
                    break;
                }
            }
            if (j == bk) {
                if (bk == DEVICE_EM_BANK_CNT_MAX) {
                    SHR_ERR_EXIT(SHR_E_INTERNAL);
                }
                em_bank[bk] = bk_arr[n];
                bk ++;
            }
        }
    }

    if (!bk) {
        *entries = 0;
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlt_transaction_free(trh));
    trh = BCMLT_INVALID_HDL;

    /* Retrieve base entries of given LT. */
    SHR_IF_ERR_EXIT
        (bcmlt_transaction_allocate(BCMLT_TRANS_TYPE_BATCH, &trh));

    for (i = 0; i < bk; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_entry_allocate(dunit, DEVICE_EM_BANK_INFOs, &eh));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add(eh, DEVICE_EM_BANK_IDs,
                                          em_bank[i]));

        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_add(trh, BCMLT_OPCODE_LOOKUP, eh));
        eh = BCMLT_INVALID_HDL;
    }

    SHR_IF_ERR_EXIT
        (bcmi_lt_transaction_commit(unit, trh, BCMLT_PRIORITY_NORMAL));

    for (i = 0; i < bk; i++) {
        SHR_IF_ERR_EXIT
            (bcmlt_transaction_entry_num_get(trh, i, &ei));

        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get(ei.entry_hdl, NUM_BASE_ENTRIESs, &val));

        base_ent += val;
    }

    *entries = base_ent;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    if (trh != BCMLT_INVALID_HDL) {
        (void)bcmlt_transaction_free(trh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the capacity of a FIB object.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FIB table attributes.
 * \param [in] flags Operation flags.
 * \param [out] Number of base entry.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_object_capacity_get(int unit, uint32_t attr, uint32_t flags, int *entries)
{
    bcmint_ltsw_l3_fib_tbl_db_t tbl_db = {0};
    int i, ltcnt = 0, num = 0;
    const char **lt_arr = NULL;
    bool base = flags & BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES ? true : false;
    bool rm_hash = false;
    bool alpm_en = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 0) ?
                   true : false;
    uint32_t val = 0;

    SHR_FUNC_ENTER(unit);

    alpm_en = ltsw_feature(unit, LTSW_FT_NO_LPM_ROUTE) ? true : alpm_en;
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_tbl_db_get(unit, &tbl_db));

    SHR_ALLOC(lt_arr, sizeof(char *) * BCMINT_LTSW_L3_FIB_TBL_CNT,
              "bcmLtswL3FibTbl");
    SHR_NULL_CHECK(lt_arr, SHR_E_MEMORY);
    sal_memset(lt_arr, 0, sizeof(char *) * BCMINT_LTSW_L3_FIB_TBL_CNT);

    for (i = 0; i < BCMINT_LTSW_L3_FIB_TBL_CNT; i++) {
        if ((!(tbl_db.tbl_bmp & (1 << i))) ||
            (tbl_db.tbls[i].attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC)) {
            continue;
        }

        if ((attr & tbl_db.tbls[i].attr) == attr) {
            if ((attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) && alpm_en &&
                (tbl_db.tbls[i].rm_type != BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM)) {
                continue;
            }
            if (base &&
                tbl_db.tbls[i].rm_type == BCMINT_LTSW_L3_FIB_TBL_RM_T_HASH) {
                rm_hash = true;
                lt_arr[ltcnt] = tbl_db.tbls[i].name;
                ltcnt++;
            } else {
                SHR_IF_ERR_EXIT
                    (bcmi_lt_capacity_get(unit, tbl_db.tbls[i].name, &val));
                if (base) {
                    val *= tbl_db.tbls[i].multiple;
                }
                num += val;
            }
        }
    }

    if (rm_hash) {
        SHR_IF_ERR_EXIT
            (l3_fib_base_entries_get(unit, lt_arr, ltcnt, &num));

    }

    *entries = num;

exit:
    SHR_FREE(lt_arr);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the utilization of a FIB object.
 *
 * \param [in] unit Unit number.
 * \param [in] attr FIB table attributes.
 * \param [in] flags Operation flags.
 * \param [out] Number of base entry.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_fib_object_utilization_get(int unit, uint32_t attr, uint32_t flags, int *entries)
{
    uint32_t val = 0;
    bcmint_ltsw_l3_fib_tbl_db_t tbl_db = {0};
    int i, num = 0;
    bool base = flags & BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES ? true : false;
    bool alpm_en = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 0) ?
                   true : false;

    SHR_FUNC_ENTER(unit);

    alpm_en = ltsw_feature(unit, LTSW_FT_NO_LPM_ROUTE) ? true : alpm_en;
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_tbl_db_get(unit, &tbl_db));

    for (i = 0; i < BCMINT_LTSW_L3_FIB_TBL_CNT; i++) {
        if ((!(tbl_db.tbl_bmp & (1 << i))) ||
            (tbl_db.tbls[i].attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SRC)) {
            continue;
        }

        if ((attr & tbl_db.tbls[i].attr) == attr) {
            if ((attr & BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET) && alpm_en &&
                (tbl_db.tbls[i].rm_type != BCMINT_LTSW_L3_FIB_TBL_RM_T_ALPM)) {
                continue;
            }
            SHR_IF_ERR_EXIT
                (bcmi_lt_entry_inuse_cnt_get(unit, tbl_db.tbls[i].name, &val));
            if (base) {
                val *= tbl_db.tbls[i].multiple;
            }
            num += val;
        }
    }

    *entries = num;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the number of inserted entries in a given route group.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Route group.
 * \param [out] cnt Installed route number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_route_grp_entry_cnt_get(int unit, bcm_l3_route_group_t grp, uint32_t *cnt)
{
    bcmint_ltsw_l3_fib_tbl_id_t fib_tbl_id;
    const bcmint_ltsw_l3_fib_tbl_t *ti = NULL;

    SHR_FUNC_ENTER(unit);

    switch (grp) {
        case bcmL3RouteGroupPrivateV4:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_VRF;
            break;
        case bcmL3RouteGroupGlobalV4:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GL;
            break;
        case bcmL3RouteGroupOverrideV4:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV4_UC_GH;
            break;
        case bcmL3RouteGroupPrivateV6:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_VRF;
            break;
        case bcmL3RouteGroupGlobalV6:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GL;
            break;
        case bcmL3RouteGroupOverrideV6:
            fib_tbl_id = BCMINT_LTSW_L3_FIB_TBL_ROUTE_IPV6_UC_GH;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, fib_tbl_id, &ti));

    SHR_IF_ERR_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, ti->name, cnt));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ALPM level 1 resource usage statistics.
 *
 * \param [in] unit Unit number.
 * \param [in] key_type Level 1 entry key type.
 * \param [out] max_ent Maximum number of entries.
 * \param [out] inuse_ent In use number of entries.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_level_1_usage_get(int unit, bcm_l3_route_group_t grp, int *max_ent,
                          int *inuse_ent)
{
    int alpm_128b_en, key_type, dunit;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_fib_tbl_t *ti;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    const char *sym_val;
    uint64_t val;

    SHR_FUNC_ENTER(unit);

    alpm_128b_en = bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE,
                                          1);
    switch (grp) {
        case bcmL3RouteGroupPrivateV4:
            key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_VRF;
            break;
        case bcmL3RouteGroupGlobalV4:
            key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_GL;
            break;
        case bcmL3RouteGroupOverrideV4:
            key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV4_UC_GH;
            break;
        case bcmL3RouteGroupPrivateV6:
            if (alpm_128b_en) {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_VRF;
            } else {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_VRF;
            }
            break;
        case bcmL3RouteGroupGlobalV6:
            if (alpm_128b_en) {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_GL;
            } else {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_GL;
            }
            break;
        case bcmL3RouteGroupOverrideV6:
            if (alpm_128b_en) {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_QUAD_GH;
            } else {
                key_type = BCMINT_LTSW_L3_ALPM_USAGE_KT_IPV6_UC_DOUBLE_GH;
            }
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    dunit = bcmi_ltsw_dev_dunit(unit);

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit,
                                    BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_1_USAGE,
                                    &ti));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_KEY_TYPE];

    SHR_NULL_CHECK(fi->scalar_to_symbol, SHR_E_INTERNAL);

    SHR_IF_ERR_EXIT
        (fi->scalar_to_symbol(unit, key_type, &sym_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_field_symbol_add_by_id(eh, fi->fld_id, sym_val));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    if (max_ent) {
        fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_ENT];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get_by_id(eh, fi->fld_id, &val));
        *max_ent = val;
    }

    if (inuse_ent) {
        fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_ENT];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get_by_id(eh, fi->fld_id, &val));
        *inuse_ent = val;
    }

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ALPM level 2/3 resource usage statistics.
 *
 * \param [in] unit Unit number.
 * \param [in] tbl_id Table ID.
 * \param [out] max_bkt Maximum number of raw buckets.
 * \param [out] inuse_bkt In use number of raw buckets.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_level_n_usage_get(int unit, int tbl_id, int *max_bkt, int *inuse_bkt)
{
    int dunit, bk_cnt, i, fld;
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    const bcmint_ltsw_l3_fib_tbl_t *ti;
    const bcmint_ltsw_l3_fib_fld_t *fi;
    const char *sym_val;
    uint64_t val, db_bmp = 0, max = 0, inuse = 0;
    int bk_db[] = {
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_0,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_1,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_2,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_3,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_4,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_5,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_6,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_7,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_8,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_DB_LVL_1_BLK_9
    };
    int bk_key_input[] = {
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_0,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_1,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_2,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_3,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_4,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_5,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_6,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_7,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_8,
        BCMINT_LTSW_L3_ALPM_CTRL_TBL_KEY_INPUT_LVL_1_BLK_9
    };

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    /* Get L3_ALPM_CONTROL info. */
    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, BCMINT_LTSW_L3_FIB_TBL_ALPM_CONTROL,
                                    &ti));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP, BCMLT_PRIORITY_NORMAL));

    /* Get the Databases used for DST route lookup. */
    fi = ti->flds;
    bk_cnt = COUNTOF(bk_db);
    for (i = 0; i < bk_cnt; i++) {
        fld = bk_key_input[i];
        if (!(ti->fld_bmp & (1ULL << fld))) {
            continue;
        }
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get_by_id(eh, fi[fld].fld_id, &sym_val));
        SHR_IF_ERR_EXIT
            (fi[fld].symbol_to_scalar(unit, sym_val, &val));
        if ((val != BCMINT_LTSW_L3_ALPM_KI_LPM_DST_QUAD) &&
            (val != BCMINT_LTSW_L3_ALPM_KI_LPM_DST_DOUBLE) &&
            (val != BCMINT_LTSW_L3_ALPM_KI_LPM_DST_SINGLE)) {
            continue;
        }

        fld = bk_db[i];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_get_by_id(eh, fi[fld].fld_id, &sym_val));
        SHR_IF_ERR_EXIT
            (fi[fld].symbol_to_scalar(unit, sym_val, &val));
        db_bmp |= (1 << val);
    }

    if (db_bmp == 0) {
        /* No resource for Route. */
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmlt_entry_free(eh));

    SHR_IF_ERR_EXIT
        (bcmint_ltsw_l3_fib_tbl_get(unit, tbl_id, &ti));

    SHR_IF_ERR_EXIT
        (bcmlt_entry_allocate_by_id(dunit, ti->tbl_id, ti->fld_cnt, &eh));

    if (!(ti->fld_bmp & (1ULL << BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB)) ||
        !(ti->fld_bmp & (1ULL << BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT)) ||
        !(ti->fld_bmp & (1ULL << BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT))) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    for (i = 0; i < BCMINT_LTSW_L3_ALPM_DB_CNT; i++) {
        if (!(db_bmp & (1 << i))) {
            continue;
        }
        fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_DB];
        SHR_IF_ERR_EXIT
            (fi->scalar_to_symbol(unit, i, &sym_val));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_symbol_add_by_id(eh, fi->fld_id, sym_val));
        SHR_IF_ERR_EXIT
            (bcmlt_entry_commit(eh, BCMLT_OPCODE_LOOKUP,
                                BCMLT_PRIORITY_NORMAL));

        fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_MAX_RBKT];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get_by_id(eh, fi->fld_id, &val));
        max += val;

        fi = &ti->flds[BCMINT_LTSW_L3_ALPM_USAGE_TBL_INUSE_RBKT];
        SHR_IF_ERR_EXIT
            (bcmlt_entry_field_get_by_id(eh, fi->fld_id, &val));
        inuse += val;
        SHR_IF_ERR_EXIT
            (bcmlt_entry_clear(eh));
    }

exit:
    if (!SHR_FUNC_ERR()) {
        if (max_bkt) {
            *max_bkt = max;
        }
        if (inuse_bkt) {
            *inuse_bkt = inuse;
        }
    }
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Uni-dimensional ALPM route projection based on current route trend.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Route group.
 * \param [out] cnt Projection route number.
 * \param [out] max_usage_level The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_route_grp_projection(int unit, bcm_l3_route_group_t grp,
                             int *prj_cnt, int *max_usage_lvl, int *max_usage)
{
    bool alpm_mode_combined;
    int alpm_temp, i, route_cnt, max_cnt, inuse_cnt;
    int max_lvl, max_usage100, lvl_usage, lvl_thres;
    uint32_t grp_route_cnt[bcmL3RouteGroupCounter];

    SHR_FUNC_ENTER(unit);

    alpm_temp = bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ALPM_TEMPLATE, 1);
    if ((alpm_temp == 1) || (alpm_temp == 3) || (alpm_temp == 5) ||
        (alpm_temp == 7) || (alpm_temp == 9) || (alpm_temp == 11)) {
        alpm_mode_combined = true;
    } else if ((alpm_temp == 2) || (alpm_temp == 4) || (alpm_temp == 6) ||
               (alpm_temp == 10)) {
        alpm_mode_combined = false;
    } else {
        SHR_EXIT();
    }

    for (i = 0; i < bcmL3RouteGroupCounter; i++)  {
        SHR_IF_ERR_EXIT
            (l3_route_grp_entry_cnt_get(unit, i, &grp_route_cnt[i]));
    }

    /* In Combined mode, route projection is for combined private & global
     * groups */
    if (alpm_mode_combined) {
        if (grp < bcmL3RouteGroupPrivateV6) {
            route_cnt = grp_route_cnt[bcmL3RouteGroupPrivateV4] +
                        grp_route_cnt[bcmL3RouteGroupGlobalV4];
        } else {
            route_cnt = grp_route_cnt[bcmL3RouteGroupPrivateV6] +
                        grp_route_cnt[bcmL3RouteGroupGlobalV6];
        }
    } else {
        route_cnt = grp_route_cnt[grp];
    }

    /* Get level 1 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_1_usage_get(unit, grp, &max_cnt, &inuse_cnt));
    if (!max_cnt || !inuse_cnt) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }
    max_lvl = 1;
    max_usage100 = inuse_cnt * 10000L / max_cnt;
    lvl_thres = 100;

    /* Get level 2 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_n_usage_get(unit,
                                   BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE,
                                   &max_cnt, &inuse_cnt));
    if (max_cnt && inuse_cnt) {
        lvl_usage = inuse_cnt * 10000L / max_cnt;
        if (lvl_usage > max_usage100) {
            max_lvl = 2;
            max_usage100 = lvl_usage;
            lvl_thres = 100;
        }
    }

    /* Get level 3 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_n_usage_get(unit,
                                   BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE,
                                   &max_cnt, &inuse_cnt));
    if (max_cnt && inuse_cnt) {
        lvl_usage = inuse_cnt * 10000L / max_cnt;
        /* Give 1% preference to L3 usage. */
        if (lvl_usage + 100 > max_usage100) {
            max_lvl = 3;
            max_usage100 = lvl_usage;
            lvl_thres = bcmi_ltsw_property_get(unit,
                                               BCMI_CPN_L3_ALPM_BNK_THRESHOLD,
                                               95);
        }
    }

    /*
     * Projection based on current route trend only when current resource
     * usage is above 5%, and No mixed group routes with shared ALPM resource.
     */
    if ((((max_usage100 + 50)/ 100) < ALPM_PRJ_MIN_USAGE) ||
        ((max_lvl == 1) &&
        ((grp_route_cnt[bcmL3RouteGroupOverrideV4] +
          grp_route_cnt[bcmL3RouteGroupOverrideV6]) > 2))) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Use projection formula: prj_route = route_cnt*lvl_thres/max_usage */
    if (lvl_thres * 100 < max_usage100) {
        /* Actual usage is bigger than configured threshold */
        max_cnt = route_cnt;
    } else {
        max_cnt = route_cnt * lvl_thres * 100L / max_usage100;
    }

    if (prj_cnt) {
        *prj_cnt = max_cnt;
    }

    if (max_usage_lvl) {
        *max_usage_lvl = max_lvl;
    }

    if (max_usage) {
        *max_usage = lvl_thres;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief ALPM route projection based on testDB.
 *
 * \param [in] unit Unit number.
 * \param [in] grp Route group.
 * \param [in] v4v6_ratio Expected V4_route_cnt/V6_route_cnt ratio.
 * \param [out] uni_prj_cnt Uni-dimensional projected maximum number of route.
 * \param [out] mix_prj_cnt Mixed DB route projection.
 * \param [out] max_usage_level The level with maximum resource usage.
 * \param [out] max_usage Maximum resource usage.
 *
 * The formula for mixed DB route project:
 *      V4_mix_prj = V4_uni_prj / (1 + V4_uni_prj / (V6_uni_prj * Ratio))
 *      V6_mix_prj = V6_uni_prj / (1 + V6_uni_prj * Ratio / V4_uni_prj)
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_alpm_route_grp_testdb(int unit, bcm_l3_route_group_t grp, int v4v6_ratio,
                         int *uni_prj_cnt, int *mix_prj_cnt,
                         int *max_usage_lvl, int *max_usage)
{
    int ip6, grp_opp, v4_uni_prj, v6_uni_prj, prj_cnt;

    SHR_FUNC_ENTER(unit);

    switch (grp) {
        case bcmL3RouteGroupPrivateV4:
            grp_opp = bcmL3RouteGroupPrivateV6;
            ip6 = false;
            break;
        case bcmL3RouteGroupGlobalV4:
            grp_opp = bcmL3RouteGroupGlobalV6;
            ip6 = false;
            break;
        case bcmL3RouteGroupPrivateV6:
            grp_opp = bcmL3RouteGroupPrivateV4;
            ip6 = true;
            break;
        case bcmL3RouteGroupGlobalV6:
            grp_opp = bcmL3RouteGroupGlobalV4;
            ip6 = true;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Get the uni-dimensional projected maximum number. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_alpm_grp_prj_db_get(unit, grp, &prj_cnt,
                                          max_usage_lvl, max_usage));

    *uni_prj_cnt = prj_cnt;

    /* v4v6_ratio 0 means uni-dimensional projection only. */
    if (!v4v6_ratio || !mix_prj_cnt) {
        SHR_EXIT();
    }

    if (ip6) {
        v6_uni_prj = prj_cnt;
    } else {
        v4_uni_prj = prj_cnt;
    }

    /* Get the uni-dimensional projected maximum number of opposite IP type. */
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_alpm_grp_prj_db_get(unit, grp_opp, &prj_cnt, NULL, NULL));

    if (ip6) {
        v4_uni_prj = prj_cnt;
    } else {
        v6_uni_prj = prj_cnt;
    }

     if (ip6) {
         *mix_prj_cnt = v6_uni_prj /
             (1.0f + (long)v6_uni_prj * v4v6_ratio / (v4_uni_prj * 100.0f));
     } else {
         *mix_prj_cnt = v4_uni_prj /
             (1.0f + v4_uni_prj * 100.0f / ((long)v6_uni_prj * v4v6_ratio));
     }

exit:
     SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal Module functions
 */

int
bcmint_ltsw_l3_fib_tbl_get(int unit,
                           bcmint_ltsw_l3_fib_tbl_id_t tbl_id,
                           const bcmint_ltsw_l3_fib_tbl_t **tbl_info)
{
    bcmint_ltsw_l3_fib_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_tbl_db_get(unit, &tbl_db));

    if (!(tbl_db.tbl_bmp & (1 << tbl_id))) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *tbl_info = &tbl_db.tbls[tbl_id];

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_l3_alpm_control_set(
    int unit,
    int alpm_temp,
    bcmi_ltsw_uft_mode_t uft_mode)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_alpm_control_set(unit, alpm_temp, uft_mode));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_l3_fib_init(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];
    uint32_t val, sz, alloc_sz, i;
    bcm_l3_vrf_route_data_mode_t def_dmode;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_L3)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_ERR_EXIT(SHR_E_DISABLED);
    }

    if (fi->inited) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_fib_deinit(unit));
    }

    sal_memset(fi, 0, sizeof(l3_fib_info_t));

    fi->mutex = sal_mutex_create("ltswl3FibInfo");
    SHR_NULL_CHECK(fi->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_vrf_max_get(unit, &val));

    fi->max_vrf_id = val;

    sz = L3_ALPM_VRF_IDX_CNT(unit) * sizeof(alpm_vrf_data_mode_t);
    alloc_sz = sz;
    fi->alpm_data_mode = bcmi_ltsw_ha_mem_alloc(unit,
                                            BCMI_HA_COMP_ID_L3_FIB,
                                            BCMINT_LTSW_L3_FIB_HA_ALPM_DMODE,
                                            "bcmL3AlpmDMode",
                                            &alloc_sz);
    SHR_NULL_CHECK(fi->alpm_data_mode, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!bcmi_warmboot_get(unit)) {
        def_dmode = bcmi_ltsw_property_get(unit,
                                           BCMI_CPN_L3_ALPM_DEFAULT_DATA_MODE,
                                           1);
        for (i = 0; i < L3_ALPM_VRF_IDX_CNT(unit); i++) {
            fi->alpm_data_mode[i].v4_dmode = def_dmode;
            fi->alpm_data_mode[i].v6_dmode = def_dmode;
        }
    }

    fi->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (fi->mutex) {
            sal_mutex_destroy(fi->mutex);
        }
        if (!bcmi_warmboot_get(unit)) {
            (void)bcmi_ltsw_ha_mem_free(unit, fi->alpm_data_mode);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_deinit(int unit)
{
    l3_fib_info_t *fi = &l3_fib_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!fi->inited || !fi->mutex) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(fi->mutex, SAL_MUTEX_FOREVER);

    fi->inited = 0;

    sal_mutex_give(fi->mutex);

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_fib_deinit(unit));

    sal_mutex_destroy(fi->mutex);

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_vrf_max_get(int unit, uint32_t *max_vrf)
{
    return mbcm_ltsw_l3_fib_vrf_max_get(unit, max_vrf);
}

int
bcmi_ltsw_l3_fib_mc_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_add(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_add(unit, fib));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_add(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_add(unit, fib));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_delete(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_delete(unit, fib));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_delete(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_delete(unit, fib));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_delete_all(int unit, uint32_t attrib)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(attrib & BCMI_LTSW_L3_FIB_ATTR_MC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_HOST &&
        !ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_host_del_all(unit, attrib));
    }

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_NETWORK) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_del_all(unit, attrib));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_find(unit, false, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_find(unit, false, fib));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_find(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_find(unit, fib));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_traverse(int unit, uint32_t attrib,
                             bcm_ipmc_traverse_cb trav_fn,
                             bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                             void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!trav_fn || !cb || !(attrib & BCMI_LTSW_L3_FIB_ATTR_MC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (attrib & BCMI_LTSW_L3_FIB_ATTR_NETWORK) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_traverse(unit, attrib, trav_fn,
                                                     cb, user_data));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4){
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_traverse(unit, attrib, trav_fn,
                                                     cb, user_data));
        }
    }
    if ((attrib & BCMI_LTSW_L3_FIB_ATTR_HOST) &&
        !ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_traverse(unit, attrib, trav_fn,
                                                    cb, user_data));
        }
        if (attrib & BCMI_LTSW_L3_FIB_ATTR_IPV4) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_traverse(unit, attrib, trav_fn,
                                                    cb, user_data));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_age(int unit, uint32_t attrib,
                        bcm_ipmc_traverse_cb age_cb,
                        bcmi_ltsw_l3_fib_to_ipmc_cb cb,
                        void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!cb || !(attrib & BCMI_LTSW_L3_FIB_ATTR_MC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ipmc_age(unit, attrib, age_cb, cb, user_data));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_flexctr_attach(int unit, bcmi_ltsw_l3_fib_t *fib,
                                   bcmi_ltsw_flexctr_counter_info_t *ci)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_flexctr_attach(unit, fib, ci));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_flexctr_attach(unit, fib, ci));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_flexctr_attach(unit, fib, ci));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_flexctr_attach(unit, fib, ci));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_flexctr_detach(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_flexctr_detach(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_flexctr_detach(unit, fib));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_flexctr_detach(unit, fib));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_flexctr_detach(unit, fib));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_mc_flexctr_info_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                     bcmi_ltsw_flexctr_counter_info_t *ci)
{
    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!(fib->flags & BCM_L3_IPMC)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if (fib->network || ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv6_mc_flexctr_info_get(unit, fib, ci));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_route_ipv4_mc_flexctr_info_get(unit, fib, ci));
        }
    } else {
        if (fib->flags & BCM_L3_IP6) {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_mc_flexctr_info_get(unit, fib, ci));
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_mc_flexctr_info_get(unit, fib, ci));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_nat_host_add(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmi_ltsw_l3_fib_t orig_fib;
    bool locked = false;
    int rv;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (fib->network || !(fib->nat_id || fib->realm_id) ||
        (fib->flags & (BCM_L3_IP6 | BCM_L3_IPMC))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(&orig_fib, fib, sizeof(bcmi_ltsw_l3_fib_t));

    L3_FIB_LOCK(unit);
    locked = true;

    rv = mbcm_ltsw_l3_host_ipv4_uc_find(unit, &orig_fib);

    if (SHR_SUCCESS(rv)) {
        if(!(fib->flags & BCM_L3_REPLACE)) {
            SHR_ERR_EXIT(SHR_E_EXISTS);
        }
        if (!(orig_fib.nat_id || orig_fib.realm_id)) {
            /* Can't replace the entry that created by host API. */
            SHR_ERR_EXIT(SHR_E_CONFIG);
        }
    } else if (rv == SHR_E_NOT_FOUND) {
        if (fib->flags & BCM_L3_REPLACE) {
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    } else {
        SHR_IF_ERR_EXIT(rv);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_ipv4_uc_add(unit, fib));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_nat_host_delete(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmi_ltsw_l3_fib_t orig_fib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (fib->network || (fib->flags & (BCM_L3_IP6 | BCM_L3_IPMC))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(&orig_fib, fib, sizeof(bcmi_ltsw_l3_fib_t));

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_ipv4_uc_find(unit, &orig_fib));

    if (!(orig_fib.nat_id || orig_fib.realm_id)) {
        /* Can't delete the entry that created by host API. */
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_ipv4_uc_delete(unit, fib));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_nat_host_find(int unit, bcmi_ltsw_l3_fib_t *fib)
{
    bcmi_ltsw_l3_fib_t orig_fib;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(fib, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (fib->network || (fib->flags & (BCM_L3_IP6 | BCM_L3_IPMC))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(&orig_fib, fib, sizeof(bcmi_ltsw_l3_fib_t));
    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_ipv4_uc_find(unit, &orig_fib));

    if (!(orig_fib.nat_id || orig_fib.realm_id)) {
        /* Not return the entry that created by host API. */
        SHR_ERR_EXIT(SHR_E_NOT_FOUND);
    }

    sal_memcpy(fib, &orig_fib, sizeof(bcmi_ltsw_l3_fib_t));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_nat_host_delete_all(int unit)
{
    uint32_t attrib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    attrib = BCMI_LTSW_L3_FIB_ATTR_IPV4 | BCMI_LTSW_L3_FIB_ATTR_UC |
             BCMI_LTSW_L3_FIB_ATTR_NAT;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_del_all(unit, attrib));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_nat_host_traverse(int unit, uint32_t flags,
                               bcm_l3_nat_ingress_traverse_cb trav_fn,
                               bcmi_ltsw_l3_fib_to_nat_cb cb,
                               void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (!trav_fn || !cb) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_nat_host_traverse(unit, flags, trav_fn, cb, user_data));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_route_hit_clear(int unit)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_hit_clear(unit));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_host_hit_clear(int unit)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (ltsw_feature(unit, LTSW_FT_NO_HOST)) {
        SHR_EXIT();
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_hit_clear(unit));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ipmc_flexctr_object_set(int unit, bcmi_ltsw_l3_fib_t *fib,
                                     uint32_t value)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ipmc_flexctr_object_set(unit, fib, value));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ipmc_flexctr_object_get(int unit, bcmi_ltsw_l3_fib_t *fib,
                                     uint32_t *value)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ipmc_flexctr_object_get(unit, fib, value));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_fib_object_cnt_get(int unit, bcmi_ltsw_l3_fib_object_t object,
                                uint32_t flags, int *entries)
{
    uint32_t attr = 0;
    bool cap = false;
    bool alpm_128b_enable;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(entries, SHR_E_PARAM);

    alpm_128b_enable = bcmi_ltsw_property_get(unit,
                                              BCMI_CPN_IPV6_LPM_128B_ENABLE,
                                              1);

    switch (object) {
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_USED:
            flags |= BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES;
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_V4_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_V4_USED:
            flags |= BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES;
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_V6_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_V6_USED:
            flags |= BCMI_LTSW_L3_FIB_OBJ_F_BASE_ENTRIES;
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_USED:
            if (ltsw_feature(unit, LTSW_FT_NO_HOST)) {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            } else {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            }
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V4_USED:
            if (ltsw_feature(unit, LTSW_FT_NO_HOST)) {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            } else {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V4 |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            }
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_V6_USED:
            if (ltsw_feature(unit, LTSW_FT_NO_HOST)) {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            } else {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6 |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_MC;
            }
            break;
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_MAX:
            cap = true;
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_MAX:
            cap = true;
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V4_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_MAX:
            cap = true;
            if (alpm_128b_enable) {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
                break;
            } else {
                *entries = 0;
                SHR_EXIT();
            }
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_128_USED:
            if (alpm_128b_enable) {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
                break;
            } else {
                *entries = 0;
                SHR_EXIT();
            }
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_MAX:
            cap = true;
            if (alpm_128b_enable) {
                *entries = 0;
                SHR_EXIT();
            } else {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_VRF |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
                break;
            }
        case BCMI_LTSW_L3_FIB_OBJ_ROUTE_V6_64_USED:
            if (alpm_128b_enable) {
                *entries = 0;
                SHR_EXIT();
            } else {
                attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_SUBNET |
                       BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
                break;
            }
        case BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V4_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_UC_V6_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_UC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V4_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_SG_V6_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_SG |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V4_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V4;
            break;
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_MAX:
            cap = true;
            /*FALLTROUGH*/
        case BCMI_LTSW_L3_FIB_OBJ_HOST_MC_G_V6_USED:
            attr = BCMINT_LTSW_L3_FIB_TBL_ATTR_HOST |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_MC |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_G |
                   BCMINT_LTSW_L3_FIB_TBL_ATTR_V6;
            break;
        default:
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (cap) {
        SHR_IF_ERR_EXIT
            (l3_fib_object_capacity_get(unit, attr, flags, entries));
    } else {
        SHR_IF_ERR_EXIT
            (l3_fib_object_utilization_get(unit, attr, flags, entries));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_same_intf_drop_set(int unit, int enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_same_intf_drop_set(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_same_intf_drop_get(int unit, int *enable)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_same_intf_drop_get(unit, enable));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_route_grp_projection(int unit, bcm_l3_route_group_t grp,
                                  int *prj_cnt, int *max_usage_lvl,
                                  int *max_usage)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(prj_cnt, SHR_E_PARAM);

    if ((grp == bcmL3RouteGroupOverrideV4) ||
        (grp == bcmL3RouteGroupOverrideV6)) {
        /* Direct routes return L1 max_entries as projection. */
        SHR_IF_ERR_EXIT
            (l3_alpm_level_1_usage_get(unit, grp, prj_cnt, NULL));

        if (max_usage_lvl) {
            *max_usage_lvl = 1;
        }
        if (max_usage) {
            *max_usage = 100;
        }

    } else {
        SHR_IF_ERR_EXIT
            (l3_alpm_route_grp_projection(unit, grp, prj_cnt, max_usage_lvl,
                                          max_usage));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_route_grp_testdb(int unit, bcm_l3_route_group_t grp,
                              int v4v6_ratio, int *uni_prj_cnt,
                              int *mix_prj_cnt, int *max_usage_lvl,
                              int *max_usage)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(uni_prj_cnt, SHR_E_PARAM);

    if ((grp == bcmL3RouteGroupOverrideV4) ||
        (grp == bcmL3RouteGroupOverrideV6)) {
        /* Direct routes return L1 max_entries as projection. */
        SHR_IF_ERR_EXIT
            (l3_alpm_level_1_usage_get(unit, grp, uni_prj_cnt, NULL));

        if (mix_prj_cnt) {
            *mix_prj_cnt = *uni_prj_cnt;
        }
        if (max_usage_lvl) {
            *max_usage_lvl = 1;
        }
        if (max_usage) {
            *max_usage = 100;
        }

    } else {
        SHR_IF_ERR_EXIT
            (l3_alpm_route_grp_testdb(unit, grp, v4v6_ratio, uni_prj_cnt,
                                      mix_prj_cnt, max_usage_lvl, max_usage));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_l3_route_perf_v4_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint32 ip,
    uint32 ip_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_perf_v4_lt_op(unit, op, vrf, ip, ip_mask,
                                          intf, alpm_dmode));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_l3_route_perf_v6_lt_op(
    int unit,
    bcmi_ltsw_l3_route_perf_opcode_t op,
    int vrf,
    uint64_t *v6,
    uint64_t *v6_mask,
    int intf,
    int (*alpm_dmode)[2])
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_perf_v6_lt_op(unit, op, vrf, v6, v6_mask,
                                          intf, alpm_dmode));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Add an entry to the L3 host table.
 *
 * Add an entry to the L3 table.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to bcm_l3_host_t containing fields related to IP
 *                  table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_host_add(int unit, bcm_l3_host_t *info)
{
    bcmi_ltsw_l3_fib_t fib, orig_fib;
    bool locked = false;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_DEFAULT) ||
        (info->l3a_intf < 0) ||
        !BCM_RPE_PRIORITY_VALID(info->l3a_pri)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.vrf = info->l3a_vrf;
    fib.ol_intf = info->l3a_intf;
    fib.ul_intf = info->l3a_ul_intf;
    fib.class_id = info->l3a_lookup_class;
    fib.opaque_ctrl_id = info->l3a_opaque_ctrl_id;
    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_addr, sizeof(bcm_ip6_t));
        if (BCM_IP6_MULTICAST(info->l3a_ip6_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            sal_memcpy(&orig_fib, &fib, sizeof(bcmi_ltsw_l3_fib_t));

            L3_FIB_LOCK(unit);
            locked = true;
            rv = mbcm_ltsw_l3_host_ipv6_uc_find(unit, &orig_fib);
            if (SHR_SUCCESS(rv)) {
                if (!(fib.flags & BCM_L3_REPLACE)) {
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                }
            } else if (rv == SHR_E_NOT_FOUND) {
                fib.flags &= ~BCM_L3_REPLACE;
            } else {
                SHR_IF_ERR_EXIT(rv);
            }

            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_uc_add(unit, &fib));
        }
    } else {
        sal_memcpy(&fib.ip_addr, &info->l3a_ip_addr, sizeof(bcm_ip_t));
        if (BCM_IP4_MULTICAST(info->l3a_ip_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            sal_memcpy(&orig_fib, &fib, sizeof(bcmi_ltsw_l3_fib_t));

            L3_FIB_LOCK(unit);
            locked = true;

            rv = mbcm_ltsw_l3_host_ipv4_uc_find(unit, &orig_fib);
            if (SHR_SUCCESS(rv)) {
                if(!(fib.flags & BCM_L3_REPLACE)) {
                    SHR_ERR_EXIT(SHR_E_EXISTS);
                }
                if (orig_fib.nat_id || orig_fib.realm_id) {
                    /* Can't replace the entry that created by NAT API. */
                    SHR_ERR_EXIT(SHR_E_CONFIG);
                }
            } else if (rv == SHR_E_NOT_FOUND) {
                fib.flags &= ~BCM_L3_REPLACE;
            } else {
                SHR_IF_ERR_EXIT(rv);
            }

            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_uc_add(unit, &fib));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from the L3 host table.
 *
 * \param [in] unit Unit number.
 * \param [in] ip_addr Pointer to bcm_l3_host_t containing fields related to
 *                     IP address to be deleted from L3 host table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_host_delete(int unit, bcm_l3_host_t *ip_addr)
{
    bcmi_ltsw_l3_fib_t fib, orig_fib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ip_addr, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (ip_addr->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((ip_addr->l3a_vrf > L3_VRF_MAX(unit)) ||
        (ip_addr->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = ip_addr->l3a_flags;
    fib.flags2 = ip_addr->l3a_flags2;
    fib.vrf = ip_addr->l3a_vrf;
    if (ip_addr->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, ip_addr->l3a_ip6_addr, sizeof(bcm_ip6_t));
        if (BCM_IP6_MULTICAST(ip_addr->l3a_ip6_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_uc_delete(unit, &fib));
        }
    } else {
        sal_memcpy(&fib.ip_addr, &ip_addr->l3a_ip_addr, sizeof(bcm_ip_t));
        if (BCM_IP4_MULTICAST(ip_addr->l3a_ip_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            sal_memcpy(&orig_fib, &fib, sizeof(bcmi_ltsw_l3_fib_t));

            L3_FIB_LOCK(unit);
            locked = true;

            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_uc_find(unit, &orig_fib));

            if (orig_fib.nat_id || orig_fib.realm_id) {
                /* Can't delete the entry that created by NAT API. */
                SHR_ERR_EXIT(SHR_E_CONFIG);
            }

            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_uc_delete(unit, &fib));
        }
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 host entries based on IP prefix.
 *
 * \param [in] unit Unit number.
 * \param [in] net_addr Pointer to bcm_l3_host_t containing fields related to
 *                      network address to be deleted from L3 host table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_host_delete_by_network(int unit, bcm_l3_route_t *net_addr)
{
    bcmi_ltsw_l3_fib_t fib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(net_addr, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (net_addr->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((net_addr->l3a_vrf > L3_VRF_MAX(unit)) ||
        (net_addr->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = net_addr->l3a_flags;
    fib.vrf = net_addr->l3a_vrf;
    if (net_addr->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, net_addr->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, net_addr->l3a_ip6_mask,
                   sizeof(bcm_ip6_t));

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_host_ipv6_uc_del_by_prefix(unit, &fib));
    } else {
        sal_memcpy(&fib.ip_addr, &net_addr->l3a_subnet, sizeof(bcm_ip_t));
        sal_memcpy(&fib.ip_addr_mask, &net_addr->l3a_ip_mask,
                   sizeof(bcm_ip_t));

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_host_ipv4_uc_del_by_prefix(unit, &fib));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 host entries that match or do not match a specified L3
 *        interface number.
 *
 * \param [in] unit Unit number.
 * \param [in] info Structure specifying the L3 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_host_delete_by_interface(int unit, bcm_l3_host_t *info)
{
    bcmi_ltsw_l3_fib_t fib;
    int negate;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Only for the overlay egress object match. */
    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.ol_intf = info->l3a_intf;
    fib.ul_intf = info->l3a_ul_intf;
    negate = info->l3a_flags & BCM_L3_NEGATE ? true : false;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_del_by_intf(unit, &fib, negate));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all L3 host entries.
 *
 * \param [in] unit Unit number.
 * \param [in] info Structure containing flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_host_delete_all(int unit, bcm_l3_host_t *info)
{
    uint32_t attrib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (info) {
        attrib = (info->l3a_flags & BCM_L3_IP6) ?
                 BCMI_LTSW_L3_FIB_ATTR_IPV6 : BCMI_LTSW_L3_FIB_ATTR_IPV4;
    } else {
        attrib = BCMI_LTSW_L3_FIB_ATTR_IPV6 | BCMI_LTSW_L3_FIB_ATTR_IPV4;
    }

    attrib |= BCMI_LTSW_L3_FIB_ATTR_UC;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_host_del_all(unit, attrib));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find a L3 host entry based on given IP address.
 *
 * \param [in] unit Unit number.
 * \param [in/out] info L3 host entry information.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_host_find(int unit, bcm_l3_host_t *info)
{
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.vrf = info->l3a_vrf;
    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_addr, sizeof(bcm_ip6_t));
        if (BCM_IP6_MULTICAST(info->l3a_ip6_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv6_uc_find(unit, &fib));
        }
    } else {
        sal_memcpy(&fib.ip_addr, &info->l3a_ip_addr, sizeof(bcm_ip_t));
        if (BCM_IP4_MULTICAST(info->l3a_ip_addr)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        } else {
            SHR_IF_ERR_EXIT
                (mbcm_ltsw_l3_host_ipv4_uc_find(unit, &fib));
        }
        if (fib.nat_id || fib.realm_id) {
            /* Not return the entry that created by NAT API. */
            SHR_ERR_EXIT(SHR_E_NOT_FOUND);
        }
    }

    info->l3a_flags = fib.flags;
    info->l3a_flags2 = fib.flags2;
    info->l3a_intf = fib.ol_intf;
    info->l3a_ul_intf = fib.ul_intf;
    info->l3a_lookup_class = fib.class_id;
    info->l3a_opaque_ctrl_id = fib.opaque_ctrl_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse through the L3 table and run callback at each valid L3 entry.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation control flags.
 * \param [in] start The starting valid entry number to be traversed.
 * \param [in] end The last valid entry number to be traversed.
 * \param [in] cb Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_host_traverse(int unit, uint32_t flags,
                          uint32_t start, uint32_t end,
                          bcm_l3_host_traverse_cb cb, void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cb, SHR_E_PARAM);

    /* Only support for UC host entry. MC host entry is covered by IPMC. */
    if (flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (flags & BCM_L3_IP6) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_host_ipv6_uc_traverse(unit, flags, start, end,
                                                cb, user_data));
    } else {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_host_ipv4_uc_traverse(unit, flags, start, end,
                                                cb, user_data));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add an IP route to the L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to bcm_l3_route_t containing fields related to
 *                  routing table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_add(int unit, bcm_l3_route_t *info)
{
    bcmi_ltsw_l3_fib_t fib;
    uint8_t ip6_zero[BCM_IP6_ADDRLEN] = {0};
    int vrf_idx;
    int max, min;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_GLOBAL) ||
        (info->l3a_intf < 0) ||
        !BCM_RPE_PRIORITY_VALID(info->l3a_pri) ||
        ((info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_FULL) &&
         (info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_REDUCED))) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    vrf_idx = L3_ALPM_VRF_TO_VRF_IDX(unit, info->l3a_vrf);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ing_class_id_range_get(unit, &min, &max, NULL));

    if ((info->l3a_lookup_class > max) ||
        (info->l3a_lookup_class < min)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = (info->l3a_vrf >= BCM_L3_VRF_DEFAULT) ? L3_VRF_MAX(unit) : 0;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    fib.class_id = info->l3a_lookup_class;
    fib.int_act_profile_id = info->l3a_int_action_profile_id;
    fib.int_opaque_data_profile_id = info->l3a_int_opaque_data_profile_id;
    fib.opaque_ctrl_id = info->l3a_opaque_ctrl_id;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (!bcmi_ltsw_property_get(unit, BCMI_CPN_IPV6_LPM_128B_ENABLE, 1) &&
            (bcm_ip6_mask_length(info->l3a_ip6_mask) > 64)) {
            SHR_ERR_EXIT(SHR_E_RESOURCE);
        }

        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));

        if (info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_FULL) {
            fib.alpm_data_mode = bcmL3VrfRouteDataModeFull;
        } else if (info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_REDUCED) {
            fib.alpm_data_mode = bcmL3VrfRouteDataModeReduced;
        } else {
            fib.alpm_data_mode = L3_ALPM6_VRF_DATA_MODE(unit, vrf_idx);
        }

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_add(unit, &fib));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;

        if (info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_FULL) {
            fib.alpm_data_mode = bcmL3VrfRouteDataModeFull;
        } else if (info->l3a_flags3 & BCM_L3_FLAGS3_DATA_MODE_REDUCED) {
            fib.alpm_data_mode = bcmL3VrfRouteDataModeReduced;
        } else {
            fib.alpm_data_mode = L3_ALPM4_VRF_DATA_MODE(unit, vrf_idx);
        }

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_add(unit, &fib));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an IP route from the L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [in] info Pointer to bcm_l3_route_t containing fields related to
 *                  routing table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_delete(int unit, bcm_l3_route_t *info)
{
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = (info->l3a_vrf >= BCM_L3_VRF_DEFAULT) ? L3_VRF_MAX(unit) : 0;
    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_delete(unit, &fib));
    } else {
        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_delete(unit, &fib));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an IP route from the L3 route table.
 *
 * \param [in] unit Unit number.
 * \param [out] info Pointer to bcm_l3_route_t containing fields related to
 *                   routing table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_get(int unit, bcm_l3_route_t *info)
{
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((info->l3a_vrf > L3_VRF_MAX(unit)) ||
        (info->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = (info->l3a_vrf >= BCM_L3_VRF_DEFAULT) ? L3_VRF_MAX(unit) : 0;
    if (info->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_find(unit, false, &fib));
    } else {
        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_find(unit, false, &fib));
    }

    info->l3a_flags = fib.flags;
    info->l3a_flags2 = fib.flags2;
    info->l3a_flags3 = fib.flags3;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        info->l3a_intf = fib.ol_intf;
        info->l3a_ul_intf = fib.ul_intf;
    } else {
        info->l3a_intf = fib.ul_intf;
    }

    info->l3a_pri = fib.int_pri;
    info->l3a_lookup_class = fib.class_id;
    info->l3a_int_action_profile_id = fib.int_act_profile_id;
    info->l3a_int_opaque_data_profile_id = fib.int_opaque_data_profile_id;
    info->l3a_opaque_ctrl_id = fib.opaque_ctrl_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a Longest Prefix Matched entry from the route table for
 *        a host address.
 *
 * \param [in] unit Unit number.
 * \param [in] host Given host address.
 * \param [out] info Pointer to bcm_l3_route_t containing fields related to
 *                   routing table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_find(int unit, bcm_l3_host_t *host, bcm_l3_route_t *route)
{
    bcmi_ltsw_l3_fib_t fib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(host, SHR_E_PARAM);
    SHR_NULL_CHECK(route, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (host->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((host->l3a_vrf > L3_VRF_MAX(unit)) ||
        (host->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = host->l3a_flags;
    fib.flags2 = host->l3a_flags2;
    fib.vrf = host->l3a_vrf;
    fib.vrf_mask = L3_VRF_MAX(unit);
    if (host->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, host->l3a_ip6_addr, sizeof(bcm_ip6_t));
        bcm_ip6_mask_create(fib.ip6_addr_mask, 128);
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_find(unit, true, &fib));
    } else {
        sal_memcpy(&fib.ip_addr, &host->l3a_ip_addr, sizeof(bcm_ip_t));
        fib.ip_addr_mask = 0xffffffff;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_find(unit, true, &fib));
    }

    L3_FIB_UNLOCK(unit);
    locked = false;

    bcm_l3_route_t_init(route);
    route->l3a_flags = fib.flags;
    route->l3a_flags2 = fib.flags2;
    route->l3a_flags3 = fib.flags3;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        route->l3a_intf = fib.ol_intf;
        route->l3a_ul_intf = fib.ul_intf;
    } else {
        route->l3a_intf = fib.ul_intf;
    }

    if (fib.flags & BCM_L3_IP6) {
        sal_memcpy(route->l3a_ip6_net, fib.ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(route->l3a_ip6_mask, fib.ip6_addr_mask, BCM_IP6_ADDRLEN);
    } else {
        route->l3a_subnet = fib.ip_addr;
        route->l3a_ip_mask = fib.ip_addr_mask;
    }
    route->l3a_lookup_class = fib.class_id;
    route->l3a_int_action_profile_id = fib.int_act_profile_id;
    route->l3a_int_opaque_data_profile_id = fib.int_opaque_data_profile_id;
    route->l3a_opaque_ctrl_id = fib.opaque_ctrl_id;

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Age on L3 routing table, clear HIT bit of entry if set.
 *
 * \param [in] unit Unit number.
 * \param [in] flags L3 route entry flags.
 * \param [in] age_out User supplied callback function.
 * \param [in] user_data  User provided cookie for callback.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_age(int unit, uint32 flags, bcm_l3_route_traverse_cb age_out,
                      void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_age(unit, age_out, user_data));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();


}

/*!
 * \brief Get a Longest Prefix Matched entry from the route table for a subnet.
 *
 * \param [in] unit Unit number.
 * \param [in] host Given subnet.
 * \param [out] info Pointer to bcm_l3_route_t containing fields related to
 *                   routing table.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_subnet_route_find(int unit, bcm_l3_route_t *input,
                              bcm_l3_route_t *route)
{
    bcmi_ltsw_l3_fib_t fib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(input, SHR_E_PARAM);
    SHR_NULL_CHECK(route, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (input->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Validation */
    if ((input->l3a_vrf > L3_VRF_MAX(unit)) ||
        (input->l3a_vrf < BCM_L3_VRF_GLOBAL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = input->l3a_flags;
    fib.vrf = input->l3a_vrf;
    fib.vrf_mask = (input->l3a_vrf >= BCM_L3_VRF_DEFAULT) ? L3_VRF_MAX(unit) : 0;
    if (input->l3a_flags & BCM_L3_IP6) {
        sal_memcpy(&fib.ip6_addr, input->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, input->l3a_ip6_mask, sizeof(bcm_ip6_t));
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_find(unit, true, &fib));
    } else {
        fib.ip_addr = input->l3a_subnet & input->l3a_ip_mask;
        fib.ip_addr_mask = input->l3a_ip_mask;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_find(unit, true, &fib));
    }

    L3_FIB_UNLOCK(unit);
    locked = false;

    bcm_l3_route_t_init(route);
    route->l3a_flags = fib.flags;
    route->l3a_flags2 = fib.flags2;
    route->l3a_flags3 = fib.flags3;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        route->l3a_intf = fib.ol_intf;
        route->l3a_ul_intf = fib.ul_intf;
    } else {
        route->l3a_intf = fib.ul_intf;
    }

    if (fib.flags & BCM_L3_IP6) {
        sal_memcpy(route->l3a_ip6_net, fib.ip6_addr, BCM_IP6_ADDRLEN);
        sal_memcpy(route->l3a_ip6_mask, fib.ip6_addr_mask, BCM_IP6_ADDRLEN);
    } else {
        route->l3a_subnet = fib.ip_addr;
        route->l3a_ip_mask = fib.ip_addr_mask;
    }

    route->l3a_lookup_class = fib.class_id;
    route->l3a_int_action_profile_id = fib.int_act_profile_id;
    route->l3a_int_opaque_data_profile_id = fib.int_opaque_data_profile_id;
    route->l3a_opaque_ctrl_id = fib.opaque_ctrl_id;

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 route entries that match or do not match a specified L3
 *        interface number.
 *
 * \param [in] unit Unit number.
 * \param [in] info Structure specifying the L3 interface.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_delete_by_interface(int unit, bcm_l3_route_t *info)
{
    bcmi_ltsw_l3_fib_t fib;
    int negate;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    /* Only support for UC route entry. MC route entry is covered by IPMC. */
    if (info->l3a_flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Only for the overlay egress object match. */
    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.flags = info->l3a_flags;
    negate = info->l3a_flags & BCM_L3_NEGATE ? true : false;
    fib.flags = info->l3a_flags;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_del_by_intf(unit, &fib, negate));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all L3 route entries.
 *
 * \param [in] unit Unit number.
 * \param [in] info Structure containing flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_delete_all(int unit, bcm_l3_route_t *info)
{
    uint32_t attrib;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if (info) {
        attrib = (info->l3a_flags & BCM_L3_IP6) ?
                 BCMI_LTSW_L3_FIB_ATTR_IPV6 : BCMI_LTSW_L3_FIB_ATTR_IPV4;
    } else {
        attrib = BCMI_LTSW_L3_FIB_ATTR_IPV6 | BCMI_LTSW_L3_FIB_ATTR_IPV4;
    }

    attrib |= BCMI_LTSW_L3_FIB_ATTR_UC;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_del_all(unit, attrib));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse through the L3 table and run callback at each valid L3 entry.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation control flags.
 * \param [in] start The starting valid entry number to be traversed.
 * \param [in] end The last valid entry number to be traversed.
 * \param [in] trav_fn Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_traverse(int unit, uint32_t flags,
                           uint32_t start, uint32_t end,
                           bcm_l3_route_traverse_cb trav_fn,
                           void *user_data)
{
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    if (flags & BCM_L3_IPMC) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    if (flags & BCM_L3_IP6) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv6_uc_traverse(unit, flags, start, end,
                                                 trav_fn, user_data));
    } else {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_route_ipv4_uc_traverse(unit, flags, start, end,
                                                 trav_fn, user_data));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the maximum ECMP paths allowed for a route.
 *
 * \param [in] unit Unit number.
 * \param [in] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_max_ecmp_set(int unit, int max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ecmp_max_paths_set(unit, max));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the maximum ECMP paths allowed for a route.
 *
 * \param [in] unit Unit number.
 * \param [out] max Maximum number of paths for ECMP.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_max_ecmp_get(int unit, int *max)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_ecmp_max_paths_get(unit, max));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int bcm_ltsw_l3_vrf_stat_attach(int unit, bcm_vrf_t vrf,
                                uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if ((vrf > L3_VRF_MAX(unit)) || (vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    pre_ci.direction = ci.direction;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_vrf_flexctr_info_get(unit, vrf, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_vrf_flexctr_attach(unit, vrf, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 *
 * \retval SHR_E_NONE No errors.
 */
int bcm_ltsw_l3_vrf_stat_detach(int unit, bcm_vrf_t vrf)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if ((vrf > L3_VRF_MAX(unit)) || (vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_vrf_flexctr_info_get(unit, vrf, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_vrf_flexctr_detach(unit, vrf));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with the given VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] stat Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int bcm_ltsw_l3_vrf_stat_id_get(int unit, bcm_vrf_t vrf,
                                bcm_l3_vrf_stat_t stat,
                                uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if ((vrf > L3_VRF_MAX(unit)) || (vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    if ((stat == bcmL3VrfStatIngressPackets) ||
        (stat == bcmL3VrfStatIngressBytes)) {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    } else {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_vrf_flexctr_info_get(unit, vrf, &ci));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ALPM route associated data mode in the VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] flags IP type flag (BCM_L3_IP6).
 * \param [in] ALPM data mode to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 * \retval SHR_E_FAIL ALPM data mode is not allowed to be changed in the VRF.
 */
int
bcm_ltsw_l3_vrf_route_data_mode_set(int unit, bcm_vrf_t vrf, uint32 flags,
                                    bcm_l3_vrf_route_data_mode_t mode)
{
    int vrf_idx = 0;
    uint32_t cnt = 0;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_ROUTE_DATA_TYPE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if ((vrf > L3_VRF_MAX(unit)) || (vrf < BCM_L3_VRF_GLOBAL)) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    /* Global High routes are always Full Mode. */
    if (vrf == BCM_L3_VRF_OVERRIDE) {
        if (mode == bcmL3VrfRouteDataModeFull) {
            SHR_EXIT();
        } else {
            SHR_IF_ERR_EXIT(BCM_E_FAIL);
        }
    }

    vrf_idx = L3_ALPM_VRF_TO_VRF_IDX(unit, vrf);

    L3_FIB_LOCK(unit);
    locked = true;

    if (flags & BCM_L3_IP6) {

        if (mode == L3_ALPM6_VRF_DATA_MODE(unit, vrf_idx)) {
            /* No change. */
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_vrf_route_cnt_get(unit, vrf, true, &cnt));
        if (cnt > 0) {
            /* Shall not change mode if VRF route has been inserted. */
            SHR_IF_ERR_EXIT(BCM_E_FAIL);
        }

        L3_ALPM6_VRF_DATA_MODE_SET(unit, vrf_idx, mode);

    } else {

        if (mode == L3_ALPM4_VRF_DATA_MODE(unit, vrf_idx)) {
            /* No change. */
            SHR_EXIT();
        }

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_vrf_route_cnt_get(unit, vrf, false, &cnt));
        if (cnt > 0) {
            /* Shall not change mode if VRF route has been inserted. */
            SHR_IF_ERR_EXIT(BCM_E_FAIL);
        }

        L3_ALPM4_VRF_DATA_MODE_SET(unit, vrf_idx, mode);
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ALPM route associated data mode in the VRF.
 *
 * \param [in] unit Unit number.
 * \param [in] vrf Virtual Router Instance.
 * \param [in] flags IP type flag (BCM_L3_IP6).
 * \param [out] ALPM data mode to set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameters.
 */
int
bcm_ltsw_l3_vrf_route_data_mode_get(int unit, bcm_vrf_t vrf, uint32 flags,
                                    bcm_l3_vrf_route_data_mode_t *mode)
{
    int vrf_idx;

    SHR_FUNC_ENTER(unit);

    if (ltsw_feature(unit, LTSW_FT_NO_ROUTE_DATA_TYPE)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    if ((vrf > L3_VRF_MAX(unit)) || (vrf < BCM_L3_VRF_GLOBAL) || !mode) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    vrf_idx = L3_ALPM_VRF_TO_VRF_IDX(unit, vrf);

    if (flags & BCM_L3_IP6) {
        *mode = L3_ALPM6_VRF_DATA_MODE(unit, vrf_idx);
    } else {
        *mode = L3_ALPM4_VRF_DATA_MODE(unit, vrf_idx);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given L3 route.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route info.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_stat_attach(int unit, bcm_l3_route_t *info,
                              uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));

    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = info->l3a_vrf;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (ci.source != bcmFlexctrSourceL3Route) {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_info_get(unit, &fib, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_route_flexctr_attach(unit, &fib, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given L3 route.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route info.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_stat_detach(int unit, bcm_l3_route_t *info)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bool locked = false;
    bcmi_ltsw_l3_fib_t fib;
    uint32_t ctr_id;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));
    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = info->l3a_vrf;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_info_get(unit, &fib, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_detach(unit, &fib));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with the given L3 route.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route info.
 * \param [in] stat Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_route_stat_id_get(int unit, bcm_l3_route_t *info,
                              bcm_l3_route_stat_t stat,
                              uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bool locked = false;
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));

    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = info->l3a_vrf;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
    }

    if ((stat == bcmL3RouteInPackets) ||
        (stat == bcmL3RouteInBytes)) {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    } else {
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_info_get(unit, &fib, &ci));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set flex counter object value for the given L3 route.
 *
 * \param [in] unit Unit number.
 * \param [in] info L3 route info.
 * \param [in] value Flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_flexctr_object_set(int unit, bcm_l3_route_t *info,
                                     uint32_t value)
{
    bool locked = false;
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));

    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.flags3 = info->l3a_flags3;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = info->l3a_vrf;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_object_set(unit, &fib,
                                               value));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given L3 route.
 *
 * \param [in] unit Unit Number.
 * \param [in] info L3 route info.
 * \param [out] value Flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_route_flexctr_object_get(int unit, bcm_l3_route_t *info,
                                      uint32_t *value)
{
    bool locked = false;
    bcmi_ltsw_l3_fib_t fib;

    SHR_FUNC_ENTER(unit);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);
    SHR_NULL_CHECK(value, SHR_E_PARAM);

    sal_memset(&fib, 0, sizeof(bcmi_ltsw_l3_fib_t));

    fib.flags = info->l3a_flags;
    fib.flags2 = info->l3a_flags2;
    fib.vrf = info->l3a_vrf;
    fib.vrf_mask = info->l3a_vrf;
    if (!ltsw_feature(unit, LTSW_FT_L3_FIB_SINGLE_POINTER)) {
        fib.ol_intf = info->l3a_intf;
        fib.ul_intf = info->l3a_ul_intf;
    } else {
        fib.ul_intf = info->l3a_intf;
    }
    fib.int_pri = info->l3a_pri;
    if (info->l3a_flags & BCM_L3_IP6) {
        if (bcm_ip6_mask_length(info->l3a_ip6_mask) == 0 &&
            sal_memcmp(info->l3a_ip6_net, ip6_zero, BCM_IP6_ADDRLEN) != 0) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        sal_memcpy(&fib.ip6_addr, info->l3a_ip6_net, sizeof(bcm_ip6_t));
        sal_memcpy(&fib.ip6_addr_mask, info->l3a_ip6_mask, sizeof(bcm_ip6_t));
    } else {
        if ((info->l3a_subnet != 0) && (info->l3a_ip_mask == 0)) {
            SHR_ERR_EXIT(SHR_E_PARAM);
        }

        fib.ip_addr = info->l3a_subnet & info->l3a_ip_mask;
        fib.ip_addr_mask = info->l3a_ip_mask;
    }

    L3_FIB_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_route_flexctr_object_get(unit, &fib, value));

exit:
    if (locked) {
        L3_FIB_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_aacl_add(
    int unit,
    uint32_t options,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_add(unit, options, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_aacl_delete(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_delete(unit, aacl));

exit:
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_aacl_delete_all(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_delete_all(unit, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_aacl_find(
    int unit,
    bcm_l3_aacl_t *aacl)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_find(unit, aacl));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}


int
bcm_ltsw_l3_aacl_traverse(
    int unit,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_traverse(unit, trav_fn, user_data));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

int
bcm_ltsw_l3_aacl_matched_traverse(
    int unit,
    uint32 flags,
    bcm_l3_aacl_traverse_cb trav_fn,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    L3_FIB_LOCK(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_fib_aacl_matched_traverse(unit, flags, trav_fn, user_data));

exit:
    L3_FIB_UNLOCK(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief Retrieve current ALPM resource usage for a given route group.
 *
 * \param[in] unit Unit number.
 * \param[in] grp ALPM route group.
 * \param[out] resource Pointer to ALPM resource structure.
 *
 * \retval SHR_E_NONE No error.
 * \retval SHR_E_UNAVAIL This feature is not supported.
 */
int
bcm_ltsw_l3_alpm_resource_get(int unit, bcm_l3_route_group_t grp,
                              bcm_l3_alpm_resource_t *resource)
{
    uint32_t route_cnt;
    int inuse_cnt, max_cnt;

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(resource, SHR_E_PARAM);

    if (!L3_FIB_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    sal_memset(resource, 0, sizeof(bcm_l3_alpm_resource_t));

    /* Get installed route number. */
    SHR_IF_ERR_EXIT
        (l3_route_grp_entry_cnt_get(unit, grp, &route_cnt));
    resource->route_cnt = route_cnt;

    /* Get level 1 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_1_usage_get(unit, grp, &max_cnt, &inuse_cnt));

    resource->lvl_usage[0].cnt_used = inuse_cnt;
    resource->lvl_usage[0].cnt_total = max_cnt;

    if (grp == bcmL3RouteGroupOverrideV4 || grp == bcmL3RouteGroupOverrideV6) {
       SHR_EXIT();
    }

    /* Get level 2 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_n_usage_get(unit,
                                   BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_2_USAGE,
                                   &max_cnt, &inuse_cnt));
    resource->lvl_usage[1].cnt_used = inuse_cnt;
    resource->lvl_usage[1].cnt_total = max_cnt;

    /* Get level 3 usage. */
    SHR_IF_ERR_EXIT
        (l3_alpm_level_n_usage_get(unit,
                                   BCMINT_LTSW_L3_FIB_TBL_ALPM_LVL_3_USAGE,
                                   &max_cnt, &inuse_cnt));
    resource->lvl_usage[2].cnt_used = inuse_cnt;
    resource->lvl_usage[2].cnt_total = max_cnt;

exit:
    SHR_FUNC_EXIT();
}
