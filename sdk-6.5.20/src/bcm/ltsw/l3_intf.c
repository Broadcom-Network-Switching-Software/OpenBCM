/*! \file l3_intf.c
 *
 * L3 ingress/egress interface management.
 * This file contains the management for L3 interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm/l3.h>
#include <bcm/l2.h>
#include <bcm_int/control.h>

#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/l3_fib.h>
#include <bcm_int/ltsw/l3_intf_int.h>
#include <bcm_int/ltsw/mbcm/l3_intf.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/port.h>

#include <shr/shr_debug.h>
#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

typedef struct l3_intf_info_s {
    /* L3 Interface manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* Ingress interface min index. */
    int ing_intf_min;

    /* Ingress interface max index. */
    int ing_intf_max;

    /* Egress interface min index. */
    int egr_intf_min[BCMI_LTSW_L3_EGR_INTF_T_CNT];

    /* Egress interface max index. */
    int egr_intf_max[BCMI_LTSW_L3_EGR_INTF_T_CNT];

    /* Egress interface index for CPU. */
    int cpu_intf_id;

    /* Bitmap of oif idx which add mac to L2 table.
     * Note, can't be used for tunnel termination. */
    SHR_BITDCL *add2arl_bmp;

    /* Enable creating L2 entry during L3 interface creation. */
    bool add2arl_enabled;

} l3_intf_info_t;

static l3_intf_info_t l3_intf_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_INTF_INITED(_u)  l3_intf_info[_u].inited

#define L3_INTF_LOCK(_u)    \
    sal_mutex_take(l3_intf_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_INTF_UNLOCK(_u)  \
    sal_mutex_give(l3_intf_info[_u].mutex)

#define L3_ING_INTF_ID_VALID(_u, _id)   \
    (((_id) >= l3_intf_info[_u].ing_intf_min) && \
     ((_id) <= l3_intf_info[_u].ing_intf_max))

#define L3_EGR_INTF_ID_IS_OL(_u, _id)   \
    (((_id) >= l3_intf_info[_u].egr_intf_min[BCMI_LTSW_L3_EGR_INTF_T_OL]) && \
     ((_id) <= l3_intf_info[_u].egr_intf_max[BCMI_LTSW_L3_EGR_INTF_T_OL]))

#define L3_EGR_INTF_ID_IS_UL(_u, _id)   \
    (((_id) >= l3_intf_info[_u].egr_intf_min[BCMI_LTSW_L3_EGR_INTF_T_UL]) && \
     ((_id) <= l3_intf_info[_u].egr_intf_max[BCMI_LTSW_L3_EGR_INTF_T_UL]))

#define L3_L2CPU_INTF_IDX(_u) l3_intf_info[_u].cpu_intf_id

#define L3_EGR_INTF_ID_VALID(_u, _id)   \
    ((L3_EGR_INTF_ID_IS_OL(_u, _id) || L3_EGR_INTF_ID_IS_UL(_u, _id)) && \
     ((_id) != l3_intf_info[_u].cpu_intf_id))

#define L3_EGR_INTF_ARL_GET(_u, _id)    \
    SHR_BITGET(l3_intf_info[_u].add2arl_bmp, (_id))

#define L3_EGR_INTF_ARL_SET(_u, _id)    \
    SHR_BITSET(l3_intf_info[_u].add2arl_bmp, (_id))

#define L3_EGR_INTF_ARL_CLR(_u, _id)    \
    SHR_BITCLR(l3_intf_info[_u].add2arl_bmp, (_id))

#define L3_EGR_INTF_ARL_ITER(_u, _i)    \
    SHR_BIT_ITER(l3_intf_info[_u].add2arl_bmp, l3_intf_info[_u].cpu_intf_id, _i)

#define L3_EGR_INTF_ADD2ARL_ENABLED(_u) (l3_intf_info[_u].add2arl_enabled)

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Setup CPU interface.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number or unit is not attached.
 * \retval SHR_E_INIT L3 interface module is not initialized.
 */
static int
l3_intf_cpu_setup(int unit)
{
    l3_intf_info_t *l3ii = &l3_intf_info[unit];
    bcmint_ltsw_l3_egr_intf_t eif;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_l2cpu_intf_id_get(unit, &l3ii->cpu_intf_id));

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    sal_memset(eif.mac_addr, 0xff, sizeof(bcm_mac_t));
    eif.flags = BCM_L3_L2ONLY | BCM_L3_WITH_ID;
    eif.intf_id = l3ii->cpu_intf_id;
    eif.mpls_label = BCM_MPLS_LABEL_INVALID;
    eif.cpu = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_create(unit, &eif));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set interface mac address in L2 table.
 *
 * \param [in] unit Unit number.
 * \prarm [in] eif Egress interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_intf_l2_addr_add(int unit, bcmint_ltsw_l3_egr_intf_t *eif)
{
    bcm_l2_addr_t l2addr;

    SHR_FUNC_ENTER(unit);

    bcm_l2_addr_t_init(&l2addr, eif->mac_addr, eif->vid);
    l2addr.flags = BCM_L2_L3LOOKUP | BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC;
    l2addr.port = bcmi_ltsw_port_cpu(unit);
    SHR_IF_ERR_EXIT
        (bcm_ltsw_stk_my_modid_get(unit, &l2addr.modid));

    /* Overwrite existing entry if any. */
    (void)bcm_ltsw_l2_addr_delete(unit, eif->mac_addr, eif->vid);

    /* Add entry to l2 table. */
    SHR_IF_ERR_EXIT
        (bcm_ltsw_l2_addr_add(unit, &l2addr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clear add2arl_bmp.
 *
 * \param [in]   unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_add2arl_bmp_clear(int unit)
{
    l3_intf_info_t *l3ii;
    int type, bit_size = 0, size = 0;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    l3ii = &l3_intf_info[unit];
    for (type = 0; type < BCMI_LTSW_L3_EGR_INTF_T_CNT; type++) {
        if (bit_size < l3ii->egr_intf_max[type]) {
            bit_size = l3ii->egr_intf_max[type];
        }
    }
    size = SHR_BITALLOCSIZE(bit_size + 1);
    sal_memset(l3ii->add2arl_bmp, 0, size);

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal Module functions
 */

int
bcmint_ltsw_l3_intf_tbl_get(int unit, int tbl_id,
                            const bcmint_ltsw_l3_intf_tbl_t **tbl_info)
{
    bcmint_ltsw_l3_intf_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_tbl_db_get(unit, &tbl_db));

    if (!(tbl_db.tbl_bmp & (1 << tbl_id))) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    *tbl_info = &tbl_db.tbls[tbl_id];

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_l3_intf_l2tocpu_idx_get(int unit)
{
    if (!L3_INTF_INITED(unit)) {
        return BCM_IF_INVALID;
    }

    return L3_L2CPU_INTF_IDX(unit);
}

int
bcmi_ltsw_l3_intf_init(int unit)
{
    l3_intf_info_t *info = &l3_intf_info[unit];
    uint32_t i, req_sz, alloc_sz, max;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_L3)) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_IF_ERR_EXIT(SHR_E_DISABLED);
    }

    if (!bcmi_ltsw_dev_exists(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    if (info->inited) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_l3_intf_deinit(unit));
    }

    sal_memset(info, 0, sizeof(l3_intf_info_t));

    info->mutex = sal_mutex_create("bcmLtswl3IntfInfo");
    SHR_NULL_CHECK(info->mutex, SHR_E_MEMORY);

    if (bcmi_ltsw_property_get(unit, BCMI_CPN_L3_DISABLE_ADD_TO_ARL, 0)) {
        info->add2arl_enabled = false;
    } else {
        info->add2arl_enabled = true;
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_init(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_id_range_get(unit, &info->ing_intf_min,
                                            &info->ing_intf_max));

    max = 0;
    for (i = 0; i < BCMI_LTSW_L3_EGR_INTF_T_CNT; i++) {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egr_intf_id_range_get(unit, i,
                                                &info->egr_intf_min[i],
                                                &info->egr_intf_max[i]));
        if (max < info->egr_intf_max[i] && (info->egr_intf_max[i] > 0)) {
            max = info->egr_intf_max[i];
        }
    }

    req_sz = SHR_BITALLOCSIZE(max + 1);
    alloc_sz = req_sz;
    info->add2arl_bmp = bcmi_ltsw_ha_mem_alloc(unit,
                                               BCMI_HA_COMP_ID_L3_INTF,
                                               BCMINT_LTSW_L3_INTF_HA_ADD2ARL,
                                               "bcmL3IntfAddToArl",
                                               &alloc_sz);
    SHR_NULL_CHECK(info->add2arl_bmp, SHR_E_MEMORY);
    SHR_IF_ERR_EXIT
        ((alloc_sz < req_sz) ? SHR_E_MEMORY : SHR_E_NONE);

    if (!bcmi_warmboot_get(unit)) {
        sal_memset(info->add2arl_bmp, 0, alloc_sz);

        SHR_IF_ERR_EXIT
            (l3_intf_cpu_setup(unit));
    } else {
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egr_l2cpu_intf_id_get(unit, &info->cpu_intf_id));
    }

    info->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (info->mutex) {
            sal_mutex_destroy(info->mutex);
        }
        if (!bcmi_warmboot_get(unit)) {
            bcmi_ltsw_ha_mem_free(unit, info->add2arl_bmp);
        }
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_deinit(int unit)
{
    l3_intf_info_t *info = &l3_intf_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!info->inited || !info->mutex) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    sal_mutex_take(info->mutex, SAL_MUTEX_FOREVER);
    info->inited = 0;
    sal_mutex_give(info->mutex);

    sal_mutex_destroy(info->mutex);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_deinit(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egr_intf_to_index(int unit, bcm_if_t intf_id, int *index,
                               bcmi_ltsw_l3_egr_intf_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!index || !type) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_to_index(unit, intf_id, index, type));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egr_index_to_intf(int unit, int index,
                                bcmi_ltsw_l3_egr_intf_type_t type,
                                bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if ((!intf) || (type >= BCMI_LTSW_L3_EGR_INTF_T_CNT)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_index_to_intf(unit, index, type, intf));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_tnl_init_set(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t tnl_type,
    int tnl_idx)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_tnl_init_set(unit, intf_id, tnl_type, tnl_idx));

exit:
   SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_tnl_init_get(
    int unit,
    int intf_id,
    bcmi_ltsw_tunnel_type_t *tnl_type,
    int *tnl_idx)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(tnl_idx, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_tnl_init_get(unit, intf_id, tnl_type, tnl_idx));

exit:
   SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_tnl_init_reset(int unit, int intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_tnl_init_reset(unit, intf_id));

exit:
   SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egr_intf_id_validate(int unit, int intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_EGR_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egr_intf_id_range_get(int unit, bcmi_ltsw_l3_egr_intf_type_t type,
                                   int *min, int *max)
{
    l3_intf_info_t *l3ii;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if ((!min) || (!max) ||
        (type >= BCMI_LTSW_L3_EGR_INTF_T_CNT)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    l3ii = &l3_intf_info[unit];

    *min = l3ii->egr_intf_min[type];
    *max = l3ii->egr_intf_max[type];

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_vfi_update(int unit, int vid,
                             bcmi_ltsw_l3_intf_vfi_t *vfi_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!vfi_info) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    if (!vfi_info->flags) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_vfi_update(unit, vid, vfi_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_intf_id_validate(int unit, int intf_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ING_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_intf_id_range_get(int unit, int *min, int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if ((!min) || (!max)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    *min = l3_intf_info[unit].ing_intf_min;
    *max = l3_intf_info[unit].ing_intf_max;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_class_id_range_get(int unit, int *min, int *max, int *width)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_class_id_range_get(unit, min, max, width));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_intf_rp_add(int unit, int intf_id, int rp_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_rp_op(unit, intf_id, rp_id, 1));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_intf_rp_delete(int unit, int intf_id, int rp_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_rp_op(unit, intf_id, rp_id, 0));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ing_intf_rp_get(int unit, int intf_id, uint32 *rp_bmp)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_rp_get(unit, intf_id, rp_bmp));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_mpls_recover_all(int unit,
                                   bcmi_ltsw_l3_intf_mpls_recover_cb cb,
                                   void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_mpls_recover_all(unit, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_intf_adapt_lookup_key_set(
    int unit,
    int intf_id,
    int key_type)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_intf_adapt_lookup_key_set(unit, intf_id, key_type));

exit:
   SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Create an L3 egress interface.
 *
 * The L3 interface ID is automatically assigned unless a specific
 * ID is requested with the BCM_L3_WITH_ID flag.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_create(int unit, bcm_l3_intf_t *intf)
{
    bcmint_ltsw_l3_egr_intf_t eif;
    bool intf_added = false;
    bool locked = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    if (!BCM_TTL_VALID(intf->l3a_ttl)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Overlay egress interface for MPLS doesn't require MAC. */
    if (!ltsw_feature(unit, LTSW_FT_L3_EGRESS_DEFAULT_UNDERLAY)) {
        if (!ltsw_feature(unit, LTSW_FT_L3_HIER)) {
            if (((intf->l3a_intf_flags & BCM_L3_INTF_UNDERLAY) ||
                 (intf->l3a_mpls_label == BCM_MPLS_LABEL_INVALID)) &&
                !(intf->l3a_flags & BCM_L3_KEEP_SRCMAC) &&
                BCM_MAC_IS_ZERO(intf->l3a_mac_addr)) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
        } else {
            if (((!(intf->l3a_intf_flags & BCM_L3_INTF_OVERLAY)) ||
                 (intf->l3a_mpls_label == BCM_MPLS_LABEL_INVALID)) &&
                !(intf->l3a_flags & BCM_L3_KEEP_SRCMAC) &&
                BCM_MAC_IS_ZERO(intf->l3a_mac_addr)) {
                SHR_IF_ERR_EXIT(SHR_E_PARAM);
            }
        }
    } else {
        if ((intf->l3a_mpls_label == BCM_MPLS_LABEL_INVALID) &&
            !(intf->l3a_flags & BCM_L3_KEEP_SRCMAC) &&
            BCM_MAC_IS_ZERO(intf->l3a_mac_addr)) {
            SHR_IF_ERR_EXIT(SHR_E_PARAM);\
        }
    }

    if ((intf->l3a_flags & BCM_L3_ADD_TO_ARL) &&
        !L3_EGR_INTF_ADD2ARL_ENABLED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_CONFIG);
    }

    if ((intf->l3a_flags & (BCM_L3_REPLACE | BCM_L3_WITH_ID)) &&
        !L3_EGR_INTF_ID_VALID(unit, intf->l3a_intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if ((intf->l3a_flags & BCM_L3_REPLACE) &&
        L3_EGR_INTF_ARL_GET(unit, intf->l3a_intf_id)) {
        sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
        eif.intf_id = intf->l3a_intf_id;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egr_intf_get(unit, &eif));
        rv = bcm_ltsw_l2_addr_delete(unit, eif.mac_addr, eif.vid);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }
        L3_EGR_INTF_ARL_CLR(unit, eif.intf_id);
    }

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    eif.intf_id = intf->l3a_intf_id;
    eif.flags = intf->l3a_flags;
    eif.intf_flags = intf->l3a_intf_flags;
    sal_memcpy(eif.mac_addr, intf->l3a_mac_addr, sizeof(bcm_mac_t));
    eif.vid = intf->l3a_vid;
    eif.class_id = intf->l3a_intf_class;
    eif.tunnel_idx = intf->l3a_tunnel_idx;
    eif.ttl = intf->l3a_ttl;
    eif.ttl_dec = intf->l3a_ttl_dec;
    eif.opaque_ctrl_id = intf->opaque_ctrl_id;
    eif.mpls_flags = intf->l3a_mpls_flags;
    eif.mpls_label = intf->l3a_mpls_label;
    eif.mpls_ttl = intf->l3a_mpls_ttl;
    eif.mpls_exp = intf->l3a_mpls_exp;
    eif.mtu = intf->l3a_mtu;

    sal_memcpy(&eif.dscp_qos, &intf->dscp_qos, sizeof(bcm_l3_intf_qos_t));
    sal_memcpy(&eif.vlan_qos, &intf->vlan_qos, sizeof(bcm_l3_intf_qos_t));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_create(unit, &eif));
    intf_added = true;

    if (intf->l3a_flags & BCM_L3_ADD_TO_ARL) {
        SHR_IF_ERR_EXIT
            (l3_intf_l2_addr_add(unit, &eif));
        L3_EGR_INTF_ARL_SET(unit, eif.intf_id);
    }

    if (!(intf->l3a_flags & BCM_L3_WITH_ID)) {
        intf->l3a_intf_id = eif.intf_id;
    }

exit:
    if (SHR_FUNC_ERR() && intf_added) {
        (void)mbcm_ltsw_l3_egr_intf_delete(unit, &eif);
    }
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete L3 interface based on L3 interface ID.
 *
 * \param [in] unit Unit number.
 * \param [in] intf Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_delete(int unit, bcm_l3_intf_t *intf)
{
    bcmint_ltsw_l3_egr_intf_t eif;
    bool locked = false;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    if (!L3_EGR_INTF_ID_VALID(unit, intf->l3a_intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    L3_INTF_LOCK(unit);
    locked = true;

    if (L3_EGR_INTF_ARL_GET(unit, intf->l3a_intf_id)) {
        sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
        eif.intf_id = intf->l3a_intf_id;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egr_intf_get(unit, &eif));
        rv = bcm_ltsw_l2_addr_delete(unit, eif.mac_addr, eif.vid);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }
        L3_EGR_INTF_ARL_CLR(unit, eif.intf_id);
    }

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    eif.flags = intf->l3a_flags;
    eif.intf_flags = intf->l3a_intf_flags;
    eif.intf_id = intf->l3a_intf_id;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_delete(unit, &eif));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all egress L3 interfaces.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_delete_all(int unit)
{
    bcmint_ltsw_l3_egr_intf_t eif;
    int intf_id;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    L3_EGR_INTF_ARL_ITER(unit, intf_id) {
        sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
        eif.intf_id = intf_id;
        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egr_intf_get(unit, &eif));
        rv = bcm_ltsw_l2_addr_delete(unit, eif.mac_addr, eif.vid);
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_EXIT(rv);
        }
    }
    SHR_IF_ERR_EXIT
        (l3_add2arl_bmp_clear(unit));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_delete_all(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an L3 egress interface.
 *
 * Find the L3 intf number based on (MAC, VLAN).
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_find(int unit, bcm_l3_intf_t *intf)
{
    bcmint_ltsw_l3_egr_intf_t eif;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    if ((BCM_MAC_IS_MCAST(intf->l3a_mac_addr)) ||
        (BCM_MAC_IS_ZERO(intf->l3a_mac_addr))) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    sal_memcpy(eif.mac_addr, intf->l3a_mac_addr, sizeof(bcm_mac_t));
    eif.vid = intf->l3a_vid;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_find(unit, &eif));

    intf->l3a_intf_id = eif.intf_id;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an L3 egress interface by VLAN.
 *
 * Find the L3 intf number based on VLAN.
 *
 * \param [in] unit Unit number.
 * \param [in/out] intf Interface info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_find_vlan(int unit, bcm_l3_intf_t *intf)
{
    bcmint_ltsw_l3_egr_intf_t eif;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    eif.vid = intf->l3a_vid;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_find(unit, &eif));

    intf->l3a_intf_id = eif.intf_id;
    sal_memcpy(&intf->dscp_qos, &eif.dscp_qos, sizeof(bcm_l3_intf_qos_t));
    sal_memcpy(intf->l3a_mac_addr, eif.mac_addr, sizeof(bcm_mac_t));
    intf->l3a_flags = eif.flags;
    intf->l3a_intf_class = eif.class_id;
    intf->l3a_tunnel_idx = eif.tunnel_idx;
    intf->l3a_ttl = eif.ttl;
    intf->l3a_mpls_flags = eif.mpls_flags;
    intf->l3a_mpls_label = eif.mpls_label;
    intf->l3a_mpls_ttl = eif.mpls_ttl;
    intf->l3a_mpls_exp = eif.mpls_exp;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an L3 egress interface.
 *
 * Given the L3 interface number, return interface info.
 *
 * \param [in]     unit         Unit number.
 * \param [in/out] intf         Interface info.
 *
 * \retval SHR_E_NONE           No errors.
 * \retval !SHR_E_NONE          Failure.
 */
int
bcm_ltsw_l3_intf_get(int unit, bcm_l3_intf_t *intf)
{
    bcmint_ltsw_l3_egr_intf_t eif;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    if (!L3_EGR_INTF_ID_VALID(unit, intf->l3a_intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&eif, 0, sizeof(bcmint_ltsw_l3_egr_intf_t));
    eif.intf_id = intf->l3a_intf_id;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_get(unit, &eif));

    sal_memcpy(&intf->dscp_qos, &eif.dscp_qos, sizeof(bcm_l3_intf_qos_t));
    sal_memcpy(intf->l3a_mac_addr, eif.mac_addr, sizeof(bcm_mac_t));
    intf->l3a_vid = eif.vid;
    intf->l3a_flags = eif.flags;
    intf->l3a_intf_flags = eif.intf_flags;
    intf->l3a_tunnel_idx = eif.tunnel_idx;
    intf->l3a_ttl = eif.ttl;
    intf->l3a_ttl_dec = eif.ttl_dec;
    intf->l3a_intf_id = eif.intf_id;
    intf->opaque_ctrl_id = eif.opaque_ctrl_id;
    intf->l3a_mpls_flags = eif.mpls_flags;
    intf->l3a_mpls_label = eif.mpls_label;
    intf->l3a_mpls_ttl = eif.mpls_ttl;
    intf->l3a_mpls_exp = eif.mpls_exp;

    if (L3_EGR_INTF_ARL_GET(unit, eif.intf_id)) {
        intf->l3a_flags |= BCM_L3_ADD_TO_ARL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an L3 ingress interface.
 *
 * The L3 interface ID is automatically assigned unless a specific
 * ID is requested with the BCM_L3_WITH_ID flag.
 *
 * \param [in] unit Unit number.
 * \param [in] ing_intf Interface info.
 * \param [In/out] intf_id L3 ingress interface id pointing to ingress object.
 *                         This is an IN argument if BCM_L3_INGRESS_WITH_ID or
 *                         BCM_L3_INGRESS_REPLACE is given in flags.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_create(int unit, bcm_l3_ingress_t *ing_intf,
                           bcm_if_t *intf_id)
{
    uint32_t max_vrf;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ing_intf, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_id, SHR_E_PARAM);

    if ((ing_intf->flags & (BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_REPLACE)) &&
        !L3_ING_INTF_ID_VALID(unit, *intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    
    if ((ing_intf->urpf_mode < bcmL3IngressUrpfDisable) ||
        (ing_intf->urpf_mode > bcmL3IngressUrpfStrict)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_vrf_max_get(unit, &max_vrf));

    if ((ing_intf->vrf > max_vrf) ||
        (ing_intf->vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_create(unit, ing_intf, intf_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an L3 ingress interface object.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id pointing to ingress object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_destroy(int unit, bcm_if_t intf_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ING_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_flexctr_info_get(unit, intf_id, &ci));

    if (ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_destroy(unit, intf_id));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an ingress interface object.
 *
 * \param [in] unit Unit number.
 * \param [in] ing_intf Ingress interface information.
 * \param [OUT] intf_id L3 ingress interface id pointing to ingress object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_find(int unit, bcm_l3_ingress_t *ing_intf,
                         bcm_if_t *intf_id)
{
    uint32_t max_vrf;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(ing_intf, SHR_E_PARAM);
    SHR_NULL_CHECK(intf_id, SHR_E_PARAM);

    if ((ing_intf->urpf_mode < bcmL3IngressUrpfDisable) ||
        (ing_intf->urpf_mode > bcmL3IngressUrpfStrict)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_l3_vrf_max_get(unit, &max_vrf));

    if ((ing_intf->vrf > max_vrf) ||
        (ing_intf->vrf < BCM_L3_VRF_DEFAULT)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_find(unit, ing_intf, intf_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an ingress interface object.
 *
 * \param [in] unit Unit number.
 * \param [In] intf_id L3 ingress interface id pointing to ingress object.
 * \param [OUT] ing_intf Ingress interface information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_get(int unit, bcm_if_t intf_id,
                        bcm_l3_ingress_t *ing_intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ING_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_NULL_CHECK(ing_intf, SHR_E_PARAM);

    bcm_l3_ingress_t_init(ing_intf);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_get(unit, intf_id, ing_intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse all valid ingress interface objects.
 *
 * \param [in] unit Unit number.
 * \param [In] trav_fn Callback function.
 * \param [In] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_traverse(int unit,
                             bcm_l3_ingress_traverse_cb trav_fn,
                             void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(trav_fn, SHR_E_PARAM);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_traverse(unit, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [in] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_stat_attach(int unit,
                                bcm_if_t intf_id,
                                uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ING_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (ci.source != bcmFlexctrSourceIngL3Intf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction  = ci.direction;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_flexctr_info_get(unit, intf_id, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_ing_intf_flexctr_attach(unit, intf_id, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_stat_detach(int unit, bcm_if_t intf_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t stat_ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_ING_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_flexctr_info_get(unit, intf_id, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &stat_ctr_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_flexctr_detach(unit, intf_id));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, stat_ctr_id));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [in] stat Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ingress_stat_id_get(int unit,
                                bcm_if_t intf_id,
                                bcm_l3_stat_t stat,
                                uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};

    SHR_FUNC_ENTER(unit);

    if ((stat == bcmL3StatInPackets) ||
        (stat == bcmL3StatInBytes)) {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;
    } else {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_ing_intf_flexctr_info_get(unit, intf_id, &ci));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 * \param [in] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_stat_attach(int unit,
                             bcm_if_t intf_id,
                             uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_EGR_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    pre_ci.direction  = ci.direction;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_flexctr_info_get(unit, intf_id, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_egr_intf_flexctr_attach(unit, intf_id, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given L3 egress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an egress L3 object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_stat_detach(int unit, bcm_if_t intf_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    uint32_t stat_ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_INTF_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!L3_EGR_INTF_ID_VALID(unit, intf_id)) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;

    L3_INTF_LOCK(unit);
    locked = true;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_flexctr_info_get(unit, intf_id, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &stat_ctr_id));

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_flexctr_detach(unit, intf_id));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit, stat_ctr_id));

exit:
    if (locked) {
        L3_INTF_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with given L3 ingress interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface ID of an Ingress L3 object.
 * \param [out] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_intf_stat_id_get(int unit,
                             bcm_if_t intf_id,
                             uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};

    SHR_FUNC_ENTER(unit);

    ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egr_intf_flexctr_info_get(unit, intf_id, &ci));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    SHR_FUNC_EXIT();
}

