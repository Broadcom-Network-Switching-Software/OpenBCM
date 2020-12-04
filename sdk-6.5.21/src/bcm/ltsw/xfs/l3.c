/*! \file l3.c
 *
 * XFS L3 Driver.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/xfs/l3.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/lt_intf.h>
#include <bcm_int/ltsw/profile_mgmt.h>

#include <shr/shr_debug.h>
#include <bcmltd/chip/bcmltd_str.h>
#include <shr/shr_bitop.h>
#include <sal/sal_mutex.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

typedef struct l3_info_s {
    /* L3 module initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

} l3_info_t;

static l3_info_t l3_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_INITED(_u) (l3_info[_u].inited)

#define L3_LOCK(_u)     \
    sal_mutex_take(l3_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_UNLOCK(_u)   \
    sal_mutex_give(l3_info[_u].mutex)

/******************************************************************************
 * Private functions
 */

/*!
 * \brief Add an entry into L3_MTU_PROFILE table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 * \param [in] mtu Mtu size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_set(int unit, int index, int mtu)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_MTU_PROFILEs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_MTU_PROFILE_IDs, index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_MTUs, mtu));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_INSERT,
                              BCMLT_PRIORITY_NORMAL));
exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an entry from L3_MTU_PROFILE table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 * \param [out] mtu Mtu size.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_get(int unit, int index, int *mtu)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;
    uint64_t value;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_MTU_PROFILEs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_MTU_PROFILE_IDs, index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_LOOKUP,
                              BCMLT_PRIORITY_NORMAL));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_get(eh, L3_MTUs, &value));

    *mtu = (int)value;

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete an entry from L3_MTU_PROFILE table.
 *
 * \param [in] unit Unit Number.
 * \param [in] index Profile entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_delete(int unit, int index)
{
    bcmlt_entry_handle_t eh = BCMLT_INVALID_HDL;
    int dunit = 0;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_allocate(dunit, L3_MTU_PROFILEs, &eh));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmlt_entry_field_add(eh, L3_MTU_PROFILE_IDs, index));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_commit(unit, eh, BCMLT_OPCODE_DELETE,
                              BCMLT_PRIORITY_NORMAL));

exit:
    if (eh != BCMLT_INVALID_HDL) {
        (void)bcmlt_entry_free(eh);
    }
    SHR_FUNC_EXIT();
}

/*! brief Calculate hash signature for L3 MTU profile.
 *
 * L3 MTU profile only has one data field L3_MTU, so set hash signature with
 * L3_MTU directly.
 *
 * \param [in] unit Unit Number.
 * \param [in] entries L3 MTU profile entry.
 * \param [in] entries_per_set Number of entries in a profile set.
 * \param [out] hash Hash signature.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_hash_cb(int unit, void *entries, int entries_per_set,
                       uint32_t *hash)
{
    SHR_FUNC_ENTER(unit);

    if (!entries || (entries_per_set != 1) || !hash) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    *hash = *(int *)entries;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Compare profile set of L3 MTU profile.
 *
 * \param [in] unit Unit Number.
 * \param [in] entries L3 MTU profile.
 * \param [in] entries_per_set Number of entries in the profile set.
 * \param [in] index Entry index of profile table.
 * \param [out] cmp Result of comparison.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_cmp_cb(int unit, void *entries, int entries_per_set, int index,
                      int *cmp)
{
    int mtu;

    SHR_FUNC_ENTER(unit);

    if (!entries || (entries_per_set != 1) || !cmp) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    mtu = *(int *)entries;

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_profile_get(unit, index, &mtu));

    *cmp = (mtu == *(int *)entries) ? 0 : 1;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize the information of L3 MTU profile.
 *
 * This function is used to initialize the information of L3 MTU profile.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_init(int unit)
{
    bcmi_ltsw_profile_hdl_t ph = BCMI_LTSW_PROFILE_L3_MTU;
    uint64_t idx_min, idx_max;
    int ent_idx_min, ent_idx_max;

    SHR_FUNC_ENTER(unit);

    if (bcmi_ltsw_profile_register_check(unit, ph)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_MTU_PROFILEs,
                                       L3_MTU_PROFILE_IDs,
                                       &idx_min, &idx_max));

    /* Reserve first entry of L3_MTU_PROFILE for no-mtu-check flow. */
    idx_min++;
    ent_idx_min = (int)idx_min;
    ent_idx_max = (int)idx_max;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_register(unit, &ph,
                                    &ent_idx_min, &ent_idx_max, 1,
                                    l3_mtu_profile_hash_cb,
                                    l3_mtu_profile_cmp_cb));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear the information of L3 MTU profile.
 *
 * This function is used to clear the information of L3 MTU profile.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_mtu_profile_clear(int unit)
{
    bcmi_ltsw_profile_hdl_t ph = BCMI_LTSW_PROFILE_L3_MTU;

    SHR_FUNC_ENTER(unit);

    if (!bcmi_ltsw_profile_register_check(unit, ph)) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_profile_unregister(unit, ph));

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_lt_clear(unit, L3_MTU_PROFILEs));
    }

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Internal HSDK functions
 */

int
xfs_ltsw_l3_init(int unit)
{
    l3_info_t *info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (info->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (xfs_ltsw_l3_deinit(unit));
    }

    sal_memset(info, 0, sizeof(l3_info_t));

    info->mutex = sal_mutex_create("bcmXFSL3info");
    SHR_NULL_CHECK(info->mutex, SHR_E_MEMORY);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_profile_init(unit));

    info->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (info->mutex) {
            sal_mutex_destroy(info->mutex);
        }
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_deinit(int unit)
{
    l3_info_t * info = &l3_info[unit];

    SHR_FUNC_ENTER(unit);

    if (info->inited) {
        SHR_NULL_CHECK(info->mutex, SHR_E_INIT);

        L3_LOCK(unit);
        info->inited = 0;
        L3_UNLOCK(unit);
        sal_mutex_destroy(info->mutex);

        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mtu_profile_clear(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_mtu_set(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    int rv = SHR_E_NONE;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    L3_LOCK(unit);
    locked = true;

    rv = bcmi_ltsw_profile_index_allocate(unit, BCMI_LTSW_PROFILE_L3_MTU,
                                          &cfg->mtu, 0, 1, &cfg->index);

    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Add new entry to L3_MTU_PROFILE if not yet exists. */
    if (SHR_SUCCESS(rv)) {
        rv = l3_mtu_profile_set(unit, cfg->index, cfg->mtu);
        if (SHR_FAILURE(rv)) {
            (void)bcmi_ltsw_profile_index_free
                      (unit, BCMI_LTSW_PROFILE_L3_MTU, cfg->index);
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_mtu_get(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(cfg, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_mtu_profile_get(unit, cfg->index, &cfg->mtu));

exit:
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_mtu_delete(int unit, bcmi_ltsw_l3_mtu_cfg_t *cfg)
{
    int rv = SHR_E_NONE;
    bool locked = false;
    int mtu_idx;
    SHR_FUNC_ENTER(unit);
    mtu_idx = cfg->index;
    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    rv = bcmi_ltsw_profile_index_free(unit, BCMI_LTSW_PROFILE_L3_MTU, mtu_idx);

    if (rv != SHR_E_BUSY) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    /* Delete the entry from L3_MTU_PROFILE if the profile index is freed. */
    if (SHR_SUCCESS(rv)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mtu_profile_delete(unit, mtu_idx));
    }

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_mtu_recover(int unit, int mtu_idx)
{
    uint32_t refcnt;
    int mtu;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    L3_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_profile_ref_count_get(unit, BCMI_LTSW_PROFILE_L3_MTU,
                                         mtu_idx, &refcnt));
    /* Recover the hash signature. */
    if (refcnt == 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (l3_mtu_profile_get(unit, mtu_idx, &mtu));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_profile_hash_update(unit, BCMI_LTSW_PROFILE_L3_MTU,
                                           &mtu, 1, mtu_idx));
    }

    /* Recover the reference count. */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_profile_ref_count_increase(unit, BCMI_LTSW_PROFILE_L3_MTU,
                                              1, mtu_idx, 1));

exit:
    if (locked) {
        L3_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

int
xfs_ltsw_l3_info_get(int unit, bcm_l3_info_t *l3info)
{
    uint64_t idx_min, idx_max;
    uint32_t value, value1, num, max_vrf;

    SHR_FUNC_ENTER(unit);

    if (!L3_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    sal_memset(l3info, 0, sizeof(bcm_l3_info_t));

    /* VRF. */
    /* Get the maximum vrf from FIB. */
    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_vrf_max_get(unit, &max_vrf));
    l3info->l3info_max_vrf = max_vrf;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, ING_VRF_TABLEs, &value));
    l3info->l3info_used_vrf = value;

    /* Egress interface. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, EGR_L3_OIF_1s, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, EGR_L3_OIF_2s, &value1));
    l3info->l3info_max_intf = value + value1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, EGR_L3_OIF_1s, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, EGR_L3_OIF_2s, &value1));
    l3info->l3info_used_intf = value + value1;

    /* Nexthop. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ING_L3_NEXT_HOP_1_TABLEs, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ING_L3_NEXT_HOP_2_TABLEs, &value1));
    l3info->l3info_max_nexthop = value + value1;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, ING_L3_NEXT_HOP_1_TABLEs, &value));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, ING_L3_NEXT_HOP_2_TABLEs, &value1));
    l3info->l3info_used_nexthop = value + value1;

    /* Tunnel init. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, EGR_L3_TUNNEL_0s, &value));
    l3info->l3info_max_tunnel_init = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, EGR_L3_TUNNEL_0s, &value));
    l3info->l3info_used_tunnel_init = value;

    /* Tunnel terminate. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, L3_IPV4_TUNNEL_TABLEs, &value));
    l3info->l3info_max_tunnel_term = value;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_entry_inuse_cnt_get(unit, L3_IPV4_TUNNEL_TABLEs, &value));
    l3info->l3info_used_tunnel_term = value;

    /* Interface group. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_field_value_range_get(unit, L3_IPV4_UNICAST_TABLEs, CLASS_IDs,
                                       &idx_min, &idx_max));
    l3info->l3info_max_intf_group = (int)idx_max;

    /* ECMP info. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_ecmp_max_paths_get(unit, &l3info->l3info_max_ecmp));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ECMP_LEVEL0s, &value));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_lt_capacity_get(unit, ECMP_LEVEL1s, &value1));

    /* TRUNK_VP shares same resource with ECMP group. */
    num = bcmi_ltsw_property_get(unit, BCMI_CPN_MAX_VP_LAGS, 0);
    l3info->l3info_max_ecmp_groups = value + value1 - num;

exit:
    SHR_FUNC_EXIT();
}
