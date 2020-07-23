/*! \file l3_egress.c
 *
 * L3 egress objects management.
 * This file contains the management for L3 egress objects.
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
#include <bcm_int/ltsw/port.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/l3_intf.h>
#include <bcm_int/ltsw/l3_egress.h>
#include <bcm_int/ltsw/l3_egress_int.h>
#include <bcm_int/ltsw/generated/l3_egress_ha.h>
#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/issu.h>

#include <sal/sal_mutex.h>
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE BSL_LS_BCM_L3

typedef struct l3_egr_info_s {
    /* L3 egress manager initialized. */
    uint8_t inited;

    /* Mutex. */
    sal_mutex_t mutex;

    /* L3 ECMP info. */
    bcmint_l3_ecmp_info_t *ecmp_info;

} l3_egr_info_t;

static l3_egr_info_t l3_egr_info[BCM_MAX_NUM_UNITS] = {{0}};

#define L3_EGRESS_INITED(_u) (l3_egr_info[_u].inited)

#define L3_EGRESS_LOCK(_u)      \
    sal_mutex_take(l3_egr_info[_u].mutex, SAL_MUTEX_FOREVER)

#define L3_EGRESS_UNLOCK(_u)    \
    sal_mutex_give(l3_egr_info[_u].mutex)

/******************************************************************************
 * Private functions
 */
/*!
 * \brief Validate ECMP group info.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
l3_ecmp_info_validate(int unit, bcm_l3_egress_ecmp_t *ecmp_info, int member_cnt)
{
    bcmint_l3_ecmp_info_t *ei = l3_egr_info[unit].ecmp_info;
    uint32_t attr = 0, dlb_ft = 0;
    int max_paths = 0;

    SHR_FUNC_ENTER(unit);

    if (member_cnt > ei->max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

    if (ecmp_info->max_paths > ei->max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_NORMAL ||
        ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_ASSIGNED ||
        ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_OPTIMAL) {
        if (!ltsw_feature(unit, LTSW_FT_DLB)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        attr |= BCMI_LTSW_ECMP_ATTR_DLB;
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_DGM) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_feature_get(unit, &dlb_ft));
        if (!(dlb_ft & BCMI_LTSW_DLB_F_ECMP_DGM)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
        }
        attr |= BCMI_LTSW_ECMP_ATTR_DLB | BCMI_LTSW_ECMP_ATTR_DGM;
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_REPLICATION_WEIGHTED) {
        attr |= BCMI_LTSW_ECMP_ATTR_WGT;
        if ((member_cnt != ecmp_info->max_paths) ||
            (ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DISABLED)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_MEMBER_WEIGHTED) {
        attr |= BCMI_LTSW_ECMP_ATTR_WGT;
        if ((member_cnt > ecmp_info->max_paths) ||
            (ecmp_info->dynamic_mode != BCM_L3_ECMP_DYNAMIC_MODE_DISABLED)) {
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
        }
    } else if (ecmp_info->dynamic_mode == BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT) {
        attr |= BCMI_LTSW_ECMP_ATTR_RH;
        if (member_cnt > ecmp_info->dynamic_size) {
            if (ecmp_info->flags & BCM_L3_ECMP_RH_REPLACE) {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
            } else {
                SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
            }
        }
    }

    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_OVERLAY) {
        attr |= BCMI_LTSW_ECMP_ATTR_OL;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_max_paths_get(unit, attr, &max_paths));

    if (ecmp_info->max_paths <= 0) {
        /* If max_paths <= 0, use the default max paths. */
       ecmp_info->max_paths = (max_paths < ei->max_paths) ?
                               max_paths : ei->max_paths;
    } else if (ecmp_info->max_paths > max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (member_cnt > ecmp_info->max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_RESOURCE);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal HSDK functions
 */

int
bcmi_ltsw_l3_egress_l2_rsv(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_l2_rsv(unit));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_init(int unit)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    uint32_t sz, alloc_sz;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_L3)) {
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if(!bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_IF_ERR_EXIT(SHR_E_DISABLED);
    }

    if (ei->inited) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_l3_egress_deinit(unit));
    }

    sal_memset(ei, 0, sizeof(l3_egr_info_t));

    ei->mutex = sal_mutex_create("bcmLtswL3EgrInfo");
    SHR_NULL_CHECK(ei->mutex, SHR_E_MEMORY);

    sz = sizeof(bcmint_l3_ecmp_info_t);
    alloc_sz = sz;
    ei->ecmp_info = bcmi_ltsw_ha_mem_alloc(unit,
                                           BCMI_HA_COMP_ID_L3_EGR,
                                           BCMINT_LTSW_L3_EGR_HA_ECMP_INFO,
                                           "bcmL3EcmpInfo",
                                           &alloc_sz);
    SHR_NULL_CHECK(ei->ecmp_info, SHR_E_MEMORY);
    SHR_IF_ERR_VERBOSE_EXIT
        ((alloc_sz < sz) ? SHR_E_MEMORY : SHR_E_NONE);

    rv = bcmi_ltsw_issu_struct_info_report(unit,
                                           BCMI_HA_COMP_ID_L3_EGR,
                                           BCMINT_LTSW_L3_EGR_HA_ECMP_INFO,
                                           0, sz, 1,
                                           BCMINT_L3_ECMP_INFO_T_ID);
    if (rv != SHR_E_EXISTS) {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_init(unit));

    if (!bcmi_warmboot_get(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (mbcm_ltsw_l3_ecmp_max_paths_get(unit, BCMI_LTSW_ECMP_ATTR_WGT,
                                             &ei->ecmp_info->max_paths));
    }

    ei->inited = 1;

exit:
    if (SHR_FUNC_ERR()) {
        if (ei->mutex) {
            sal_mutex_destroy(ei->mutex);
        }
        if (!bcmi_warmboot_get(unit) && ei->ecmp_info) {
            (void)bcmi_ltsw_ha_mem_free(unit, ei->ecmp_info);
        }
        (void)mbcm_ltsw_l3_egress_deinit(unit);
    }
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_deinit(int unit)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (ei->inited) {
        SHR_NULL_CHECK(ei->mutex, SHR_E_INIT);

        L3_EGRESS_LOCK(unit);
        ei->inited = 0;
        L3_EGRESS_UNLOCK(unit);

        sal_mutex_destroy(ei->mutex);
        ei->mutex = NULL;

        SHR_IF_ERR_EXIT
            (mbcm_ltsw_l3_egress_deinit(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_obj_id_resolve(int unit,
                                   bcm_if_t intf,
                                   int *nh_ecmp_idx,
                                   bcmi_ltsw_l3_egr_obj_type_t *type)
{
    SHR_FUNC_ENTER(unit);

    if (!nh_ecmp_idx && !type) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_obj_id_resolve(unit, intf, nh_ecmp_idx, type));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_obj_id_construct(int unit,
                                     int nh_ecmp_idx,
                                     bcmi_ltsw_l3_egr_obj_type_t type,
                                     bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if ((!intf) ||
        (type >= BCMI_LTSW_L3_EGR_OBJ_T_CNT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_obj_id_construct(unit, nh_ecmp_idx, type, intf));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_obj_info_get(int unit, bcm_if_t intf,
                                 bcmi_ltsw_l3_egr_obj_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_obj_info_get(unit, intf, info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_obj_cnt_get(int unit, bcmi_ltsw_l3_egr_obj_type_t type,
                                int *max, int *used)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_obj_cnt_get(unit, type, max, used));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_dvp_set(int unit, bcm_if_t intf, int dvp)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_egress_dvp_set(unit, intf, dvp));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_max_paths_set(int unit, int max_paths)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];
    int hw_max_paths;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_max_paths_get(unit, BCMI_LTSW_ECMP_ATTR_WGT,
                                         &hw_max_paths));

    if (max_paths < 2 || max_paths > hw_max_paths) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ei->ecmp_info->max_paths = max_paths;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_max_paths_get(int unit, int *max_paths)
{
    l3_egr_info_t *ei = &l3_egr_info[unit];

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    *max_paths = ei->ecmp_info->max_paths;

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_hash_ctrl_set(int unit, int underlay, int flow_based)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_ecmp_hash_ctrl_set(unit, underlay, flow_based));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_hash_ctrl_get(int unit, int underlay, int *flow_based)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_ecmp_hash_ctrl_get(unit, underlay, flow_based));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_flow_hash_set(int unit,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int arg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_ecmp_flow_hash_set(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_flow_hash_get(int unit,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_ecmp_flow_hash_get(unit, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_port_hash_set(int unit,
                                int port,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int arg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

   SHR_IF_ERR_VERBOSE_EXIT
       (mbcm_ltsw_l3_ecmp_port_hash_set(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_port_hash_get(int unit,
                                int port,
                                bcmi_ltsw_l3_ecmp_hash_cfg_type_t type,
                                int *arg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_port_hash_get(unit, port, type, arg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecmp_member_dest_alloc(int unit,
                                 bcmi_ltsw_ecmp_member_dest_info_t *dest_info,
                                 int *dest_index)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!dest_info || !dest_index ||
         (dest_info->type >= BCMI_LTSW_ECMP_MEMBER_DEST_T_CNT)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecmp_member_dest_alloc(unit, dest_info, dest_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecmp_member_dest_get(int unit, int dest_index,
                               bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(dest_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecmp_member_dest_get(unit, dest_index, dest_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecmp_member_dest_free(int unit, int dest_index)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecmp_member_dest_free(unit, dest_index));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_ecmp_member_dest_update(int unit,
                                  bcmi_ltsw_ecmp_member_dest_info_t *dest_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(dest_info, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_ecmp_member_dest_update(unit, dest_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_count_get(int unit, int *count)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_count_get(unit, count));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_size_get(int unit, int *size)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(size, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_size_get(unit, size));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_ecmp_count_get(int unit, int *count)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(count, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_count_get(unit, count));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_ifa_create(
    int unit,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa,
    bcm_if_t *intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(egr_ifa, SHR_E_PARAM);
    SHR_NULL_CHECK(intf, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_ifa_create(unit, egr_ifa, intf));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_ifa_destroy(int unit, bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_ifa_destroy(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_ifa_get(
    int unit,
    bcm_if_t intf,
    bcmi_ltsw_l3_egr_ifa_t *egr_ifa)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(egr_ifa, SHR_E_PARAM);

    sal_memset(egr_ifa, 0, sizeof(bcmi_ltsw_l3_egr_ifa_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_ifa_get(unit, intf, egr_ifa));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_ifa_traverse(
    int unit,
    bcm_ifa_header_traverse_cb trav_fn,
    bcmi_ltsw_l3_egr_to_ifa_header_cb cb,
    void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!trav_fn || !cb) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_ifa_traverse(unit, trav_fn, cb, user_data));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_add_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!l2_tag_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_add_l2tag(unit, nhidx, l2_tag_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_get_l2tag(
    int unit,
    int nhidx,
    bcmi_ltsw_l3_egr_l2_tag_info_t *l2_tag_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!l2_tag_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_get_l2tag(unit, nhidx, l2_tag_info));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_delete_l2tag(
    int unit,
    int nhidx)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_delete_l2tag(unit, nhidx));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_php_action_add(
    int unit,
    int nhidx)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_php_action_add(unit, nhidx));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_php_action_delete(
    int unit,
    int nhidx)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_php_action_delete(unit, nhidx));

exit:
    SHR_FUNC_EXIT();
}


int
bcmi_ltsw_l3_egress_id_range_get(
    int unit,
    int type,
    int *min,
    int *max)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_id_range_get(unit, type, min, max));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_trunk_member_delete(int unit, bcm_trunk_t trunk_id,
                                        int member_cnt, bcm_port_t *member_arr)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_DEST_SYSPORT_TO_UL_NH)) {
        SHR_EXIT();
    }

    if (!L3_EGRESS_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_trunk_member_delete(unit, trunk_id, member_cnt,
                                                 member_arr));

exit:
    SHR_FUNC_EXIT();
}

int
bcmi_ltsw_l3_egress_trunk_member_add(int unit, bcm_trunk_t trunk_id,
                                     int member_cnt, bcm_port_t *member_arr)
{
    SHR_FUNC_ENTER(unit);

    if (!ltsw_feature(unit, LTSW_FT_DEST_SYSPORT_TO_UL_NH)) {
        SHR_EXIT();
    }

    if (!L3_EGRESS_INITED(unit)) {
        SHR_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_trunk_member_add(unit, trunk_id, member_cnt,
                                              member_arr));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public Internal Module functions
 */
int
bcmint_ltsw_l3_egress_tbl_get(
    int unit,
    bcmint_ltsw_l3_egress_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info)
{
    bcmint_ltsw_l3_egress_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_tbl_db_get(unit, &tbl_db));

    if (!(tbl_db.tbl_bmp & (1 << tbl_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *tbl_info = &tbl_db.tbls[tbl_id];

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_l3_ecmp_tbl_get(
    int unit,
    bcmint_ltsw_l3_ecmp_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info)
{
    bcmint_ltsw_l3_egress_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_tbl_db_get(unit, &tbl_db));

    if (!(tbl_db.tbl_bmp & (1 << tbl_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *tbl_info = &tbl_db.tbls[tbl_id];

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_l3_ecmp_flexctr_action_tbl_get(
    int unit,
    bcmint_ltsw_l3_ecmp_flexctr_act_tbl_id_t tbl_id,
    const bcmint_ltsw_l3_egress_tbl_t **tbl_info)
{
    bcmint_ltsw_l3_egress_tbl_db_t tbl_db = {0};

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_action_tbl_db_get(unit, &tbl_db));

    if (!(tbl_db.tbl_bmp & (1 << tbl_id))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    *tbl_info = &tbl_db.tbls[tbl_id];

exit:
    SHR_FUNC_EXIT();
}

int
bcmint_ltsw_l3_overlay_table_init(
    int unit,
    bcmint_ltsw_l3_ol_tbl_info_t *info)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_overlay_table_init(unit, info));

exit:
    SHR_FUNC_EXIT();
}

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Create an Egress forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Operation flags.
 * \param [in] egr Egress forwarding destination.
 * \param [out] intf L3 interface id pointing to Egress object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_create(int unit, uint32_t flags, bcm_l3_egress_t *egr,
                          bcm_if_t *intf)
{
    bcm_l3_egress_t egr_int;
    int id;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!egr || !intf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    /* Copy provided structure to local so it can be modified. */
    sal_memcpy(&egr_int, egr, sizeof(bcm_l3_egress_t));
    egr_int.flags |= flags;

    if (BCM_GPORT_IS_SET(egr->port)) {
        if (!bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port) &&
            !BCM_GPORT_IS_BLACK_HOLE(egr->port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, egr->port,
                                              &egr_int.module,
                                              &egr_int.port,
                                              &egr_int.trunk,
                                              &id));
            if (egr_int.trunk != BCM_TRUNK_INVALID) {
                egr_int.flags |= BCM_L3_TGID;
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, egr->port, &egr_int.port));
    }

    /* If trap to cpu required overwrite mac/module/intf/port. */
    if (egr->flags & BCM_L3_L2TOCPU) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &egr_int.module));

        egr_int.port = bcmi_ltsw_port_cpu(unit);
        egr_int.intf = bcmi_ltsw_l3_intf_l2tocpu_idx_get(unit);
    }

    if (egr_int.intf == BCM_IF_INVALID) {
       egr_int.intf = bcmi_ltsw_l3_intf_l2tocpu_idx_get(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_create(unit, &egr_int, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_destroy(int unit, bcm_if_t intf)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_destroy(unit, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Egress object ID.
 * \param [out] egr Egress object info.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_get(int unit, bcm_if_t intf, bcm_l3_egress_t *egr)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(egr, SHR_E_PARAM);

    bcm_l3_egress_t_init(egr);

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_get(unit, intf, egr));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find a L3 egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] egr Egress object info to match.
 * \param [out] idx Egress object ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_find(int unit, bcm_l3_egress_t *egr, bcm_if_t *intf)
{
    bcm_l3_egress_t egr_int;
    int id;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!egr || !intf) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    sal_memcpy(&egr_int, egr, sizeof(bcm_l3_egress_t));

    if (BCM_GPORT_IS_SET(egr->port)) {
        if (!bcmi_ltsw_virtual_vp_is_vfi_type(unit, egr->port)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmi_ltsw_port_gport_resolve(unit, egr->port,
                                              &egr_int.module,
                                              &egr_int.port,
                                              &egr_int.trunk,
                                              &id));
            if (egr_int.trunk != BCM_TRUNK_INVALID) {
                egr_int.flags |= BCM_L3_TGID;
            }
        }
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_port_gport_validate(unit, egr->port, &egr_int.port));
    }

    /* If trap to cpu required overwrite mac/module/intf/port. */
    if (egr->flags & BCM_L3_L2TOCPU) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcm_ltsw_stk_my_modid_get(unit, &egr_int.module));

        egr_int.port = bcmi_ltsw_port_cpu(unit);
        egr_int.intf = bcmi_ltsw_l3_intf_l2tocpu_idx_get(unit);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_find(unit, &egr_int, intf));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse L3 egress objects.
 *
 * Goes through egress objects table and runs the user callback
 * function at each valid egress objects entry passing back the
 * information for that object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fn Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_traverse(int unit,
                            bcm_l3_egress_traverse_cb trav_fn,
                            void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    if (!trav_fn) {
        SHR_IF_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_traverse(unit, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 * \param [in] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_stat_attach(int unit, bcm_if_t intf_id,
                               uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if (((type == BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
         (ci.source != bcmFlexctrSourceL3EgressOverlay)) ||
        ((type == BCMI_LTSW_L3_EGR_OBJ_T_UL) &&
         (ci.source != bcmFlexctrSourceL3EgressUnderlay))) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_info_get(unit, intf_id, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_egress_flexctr_attach(unit, intf_id, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_stat_detach(int unit, bcm_if_t intf_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint32_t ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_info_get(unit, intf_id, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_detach(unit, intf_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 * \param [in] stat Type of the counter to retrieve.
 * \param [out] stat_counter_id Stat Counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_egress_stat_id_get(int unit, bcm_if_t intf_id,
                               bcm_l3_stat_t stat,
                               uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if ((stat == bcmL3StatOutPackets) ||
        (stat == bcmL3StatOutBytes)) {
        ci.direction = BCMI_LTSW_FLEXCTR_DIR_EGRESS;
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_info_get(unit, intf_id, &ci));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set flex counter object value for the given egress object.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id Interface id of a L3 egress object.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_flexctr_object_set(int unit, bcm_if_t intf_id,
                                      uint32_t value)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_object_set(unit,
                                                intf_id,
                                                value));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given egress object.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id L3 interface ID pointing to egress object.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_flexctr_object_get(int unit, bcm_if_t intf_id,
                                      uint32_t *value)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_egress_flexctr_object_get(unit,
                                                intf_id,
                                                value));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] options BCM_L3_ECMP_O_xxx flags.
 * \param [in/out] ecmp_info ECMP group information.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array Member array of egress forwarding objects.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_create(int unit,
                        uint32_t options,
                        bcm_l3_egress_ecmp_t *ecmp_info,
                        int ecmp_member_count,
                        bcm_l3_ecmp_member_t *ecmp_member_array)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info || !ecmp_member_array) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ecmp_info->flags &= ~(BCM_L3_WITH_ID | BCM_L3_REPLACE);

    if (options & BCM_L3_ECMP_O_CREATE_WITH_ID) {
        ecmp_info->flags |= BCM_L3_WITH_ID;
    }

    if (options & BCM_L3_ECMP_O_REPLACE) {
        ecmp_info->flags |= BCM_L3_REPLACE;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (l3_ecmp_info_validate(unit, ecmp_info, ecmp_member_count));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_create(unit, ecmp_info, ecmp_member_count,
                                  ecmp_member_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id L3 ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_destroy(int unit, bcm_if_t ecmp_group_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_destroy(unit, ecmp_group_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get an Egress ECMP forwarding object information.
 *
 * \param [in] unit Unit number.
 * \param [in/out] ecmp_info ECMP group information.
 * \param [in] ecmp_member_size Size of allocated entries in ecmp_member_array.
 * \param [out] ecmp_member_array Member array of egress forwarding objects.
 * \param [out] ecmp_member_count Number of elements in ecmp_member_array.
 *
 * If the ecmp_member_size is 0, then ecmp_member_array is ignored and
 * ecmp_member_count is filled in with the number of members that would have
 * been filled into ecmp_member_array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_get(int unit,
                     bcm_l3_egress_ecmp_t *ecmp_info,
                     int ecmp_member_size,
                     bcm_l3_ecmp_member_t *ecmp_member_array,
                     int *ecmp_member_count)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_info) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_get(unit, ecmp_info, ecmp_member_size,
                               ecmp_member_array, ecmp_member_count));

    /* Weighted ECMP are never sorted. */
    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        ecmp_info->ecmp_group_flags &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_member_add(int unit, bcm_if_t ecmp_group_id,
                            bcm_l3_ecmp_member_t *ecmp_member)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_member) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_member_add(unit, ecmp_group_id, ecmp_member));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 * \param [in] ecmp_member Pointer to ECMP objects member structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_member_delete(int unit, bcm_if_t ecmp_group_id,
                               bcm_l3_ecmp_member_t *ecmp_member)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_member) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_member_del(unit, ecmp_group_id, ecmp_member));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all members from an Egress ECMP forwarding object.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_group_id Egress ECMP forwarding object ID.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_member_delete_all(int unit, bcm_if_t ecmp_group_id)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_member_del_all(unit, ecmp_group_id));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Find an Egress ECMP forwarding object by given member info.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_member_count Number of elements in ecmp_member_array.
 * \param [in] ecmp_member_array ECMP member array.
 * \param [out] ecmp_info ECMP group info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_find(int unit, int ecmp_member_count,
                      bcm_l3_ecmp_member_t *ecmp_member_array,
                      bcm_l3_egress_ecmp_t *ecmp_info)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ecmp_member_array) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array,
                                ecmp_info));

    /* Weighted ECMP are never sorted. */
    if (ecmp_info->ecmp_group_flags & BCM_L3_ECMP_WEIGHTED) {
        ecmp_info->ecmp_group_flags &= ~BCM_L3_ECMP_PATH_NO_SORTING;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse L3 ECMP objects.
 *
 * Goes through Egress ECMP forwarding objects table and runs the
 * user callback function at each Egress ECMP forwarding objects
 * entry passing back the information for that ECMP object.
 *
 * \param [in] unit Unit number.
 * \param [in] trav_fun Callback function.
 * \param [in] user_data User data to be passed to callback function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_traverse(int unit, bcm_l3_ecmp_traverse_cb trav_fn,
                          void *user_data)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!trav_fn) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_trav(unit, trav_fn, user_data));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [in] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_ecmp_member_status_set(int unit, bcm_if_t intf, int status)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_member_status_set(unit, intf, status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP dynamic load balancing member status.
 *
 * \param [in] unit Unit number.
 * \param [in] intf L3 interface ID pointing to an Egress Object.
 * \param [out] status ECMP member status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_ecmp_member_status_get(int unit, bcm_if_t intf, int *status)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_member_status_get(unit, intf, status));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set ECMP DLB port quality attributes.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [in] quality_attr Pointer to ECMP DLB port quality attribute info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_dlb_port_quality_attr_set(
    int unit,
    bcm_port_t port,
    bcm_l3_ecmp_dlb_port_quality_attr_t *quality_attr)
{
    bcmi_ltsw_dlb_member_attribute_t attr;
    int local_port, sum_valid;
    int scaling_factor_valid = 1;
    int load_weight_valid = 1;
    int qsize_weight_valid = 1;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(quality_attr, SHR_E_PARAM);

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    if (quality_attr->scaling_factor ==
            BCM_L3_ECMP_DYNAMIC_SCALING_FACTOR_INVALID) {
        scaling_factor_valid = 0;
    }
    if (quality_attr->load_weight ==
            BCM_L3_ECMP_DYNAMIC_LOAD_WEIGHT_INVALID) {
        load_weight_valid = 0;
    }
    if (quality_attr->queue_size_weight ==
            BCM_L3_ECMP_DYNAMIC_QUEUE_SIZE_WEIGHT_INVALID) {
        qsize_weight_valid = 0;
    }

    sum_valid = scaling_factor_valid + load_weight_valid +
                qsize_weight_valid;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (local_port == bcmi_ltsw_port_cpu(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (sum_valid == 3) {
        attr.scaling_factor = quality_attr->scaling_factor;
        attr.load_weight = quality_attr->load_weight;
        attr.qsize_weight = quality_attr->queue_size_weight;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_member_attribute_set(unit, local_port,
                                                bcmi_dlb_type_ecmp, &attr));
    } else if (sum_valid == 0) {
        attr.scaling_factor = BCMI_LTSW_DLB_SCALING_FACTOR_INVALID;
        attr.load_weight = BCMI_LTSW_DLB_LOAD_WEIGHT_INVALID;
        attr.qsize_weight = BCMI_LTSW_DLB_QUEUE_SIZE_WEIGHT_INVALID;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_dlb_member_attribute_set(unit, local_port,
                                                bcmi_dlb_type_ecmp, &attr));
    } else {
        /* The scaling factor, load weight and qsize weight must
         * be all invalid or all valid.
         */
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get ECMP DLB port quality attribute.
 *
 * \param [in] unit Unit Number.
 * \param [in] port Port Number.
 * \param [out] quality_attr Pointer to ECMP DLB port quality attribute info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_dlb_port_quality_attr_get(
    int unit,
    bcm_port_t port,
    bcm_l3_ecmp_dlb_port_quality_attr_t *quality_attr)
{
    bcmi_ltsw_dlb_member_attribute_t attr;
    int local_port, rv;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(quality_attr, SHR_E_PARAM);

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_port_gport_validate(unit, port, &local_port));

    if (local_port == bcmi_ltsw_port_cpu(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    bcm_l3_ecmp_dlb_port_quality_attr_t_init(quality_attr);

    rv = bcmi_ltsw_dlb_member_attribute_get(unit, local_port,
                                            bcmi_dlb_type_ecmp, &attr);

    if (SHR_SUCCESS(rv)) {
        quality_attr->scaling_factor = attr.scaling_factor;
        quality_attr->load_weight = attr.load_weight;
        quality_attr->queue_size_weight = attr.qsize_weight;
    } else {
        if (rv != SHR_E_NOT_FOUND) {
            SHR_IF_ERR_VERBOSE_EXIT(rv);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [in] flags BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags.
 * \param [in] ethertype_count Ethertype count.
 * \param [in] ethertype_array Ethertype array.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_ecmp_ethertype_set(int unit, uint32_t flags,
                                      int ethertype_count,
                                      int *ethertype_array)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_ethertype_set(unit, flags, ethertype_count, ethertype_array));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get DLB Eligibility Ethertype.
 *
 * \param [in] unit Unit Number.
 * \param [out] flags BCM_L3_ECMP_DYNAMIC_ETHERTYPE_xxx flags..
 * \param [in] ethertype_max Max ethertype count.
 * \param [out] ethertype_array Ethertype array.
 * \param [out] ethertype_count Actual ethertype count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_egress_ecmp_ethertype_get(int unit, uint32_t *flags, int ethertype_max,
                                      int *ethertype_array, int *ethertype_count)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_ethertype_get(unit, ethertype_max, flags,
                                         ethertype_array, ethertype_count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [in] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_dlb_mon_config_set(int unit,
                                    bcm_if_t ecmp_intf,
                                    bcm_l3_ecmp_dlb_mon_cfg_t * dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_dlb_mon_config_set(unit, ecmp_intf, dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the ECMP dlb monitor configurations.
 *
 * \param [in] unit Unit Number.
 * \param [in] tid ECMP Identifier.
 * \param [out] dlb_mon_cfg ECMP DLB monitor configurations.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_dlb_mon_config_get(int unit,
                                    bcm_if_t intf,
                                    bcm_l3_ecmp_dlb_mon_cfg_t * dlb_mon_cfg)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_dlb_mon_config_get(unit, intf, dlb_mon_cfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [in] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_l3_ecmp_dlb_stat_set(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t value)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_dlb_stat_set(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB statistics based on the ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_l3_ecmp_dlb_stat_get(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_dlb_stat_get(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get the DLB statistics based on the ECMP interface in sync mode.
 *
 * \param [in] unit Unit number.
 * \param [in] ecmp_intf ECMP interface id.
 * \param [in] type L3 ECMP DLB statistics type..
 * \param [out] value 64-bit counter value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
int
bcm_ltsw_l3_ecmp_dlb_stat_sync_get(
    int unit,
    bcm_if_t ecmp_intf,
    bcm_l3_ecmp_dlb_stat_t type,
    uint64_t *value)
{
    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);
    }

    if (!ltsw_feature(unit, LTSW_FT_DLB)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_dlb_stat_sync_get(unit, ecmp_intf, type, value));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach counter entries to the given ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id ECMP interface ID.
 * \param [in] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_ecmp_stat_attach(
    int unit,
    bcm_if_t intf_id,
    uint32_t stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci, pre_ci;
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    sal_memset(&pre_ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_info_get(unit, stat_counter_id, &ci));

    if ((type == BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) &&
        (ci.source != bcmFlexctrSourceEcmp)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    pre_ci.direction = ci.direction;

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_info_get(unit, intf_id, &pre_ci));

    if (pre_ci.action_index != BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_BUSY);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_attach_counter_id_status_update(unit,
                                                           stat_counter_id));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_l3_ecmp_flexctr_attach(unit, intf_id, &ci));

    if (SHR_FUNC_ERR()) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                               stat_counter_id));
    }

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Detach counter entries of the given ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id ECMP interface.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_ecmp_stat_detach(
    int unit,
    bcm_if_t intf_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci;
    bcmi_ltsw_l3_egr_obj_type_t type;
    uint32_t ctr_id;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    sal_memset(&ci, 0, sizeof(bcmi_ltsw_flexctr_counter_info_t));
    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_info_get(unit, intf_id, &ci));

    if (ci.action_index == BCMI_LTSW_FLEXCTR_ACTION_INVALID) {
        SHR_EXIT();
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, &ctr_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_detach(unit, intf_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_detach_counter_id_status_update(unit,
                                                           ctr_id));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get stat counter id associated with the given ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id ECMP interface.
 * \param [out] stat_counter_id Stat counter ID.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm_ltsw_l3_ecmp_stat_id_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *stat_counter_id)
{
    bcmi_ltsw_flexctr_counter_info_t ci = {0};
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    ci.direction = BCMI_LTSW_FLEXCTR_DIR_INGRESS;

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_info_get(unit, intf_id, &ci));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_flexctr_counter_id_get(unit, &ci, stat_counter_id));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set flex counter object value for the given ECMP interface.
 *
 * \param [in] unit Unit number.
 * \param [in] intf_id ECMP interface.
 * \param [in] value The flex counter object value.
 *
 * The flex counter object value could be used to generate counter index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_flexctr_object_set(
    int unit,
    bcm_if_t intf_id,
    uint32_t value)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_object_set(unit,
                                              intf_id,
                                              value));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get flex counter object value for the given ECMP interface.
 *
 * \param [in] unit Unit Number.
 * \param [in] intf_id ECMP interface.
 * \param [out] value The flex counter object value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_l3_ecmp_flexctr_object_get(
    int unit,
    bcm_if_t intf_id,
    uint32_t *value)
{
    bcmi_ltsw_l3_egr_obj_type_t type;
    bool locked = false;

    SHR_FUNC_ENTER(unit);

    if (!L3_EGRESS_INITED(unit)) {
        SHR_IF_ERR_EXIT(SHR_E_INIT);
    }

    SHR_NULL_CHECK(value, SHR_E_PARAM);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_l3_egress_obj_id_resolve(unit, intf_id, NULL, &type));

    if ((type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_OL) &&
        (type != BCMI_LTSW_L3_EGR_OBJ_T_ECMP_UL)) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    L3_EGRESS_LOCK(unit);
    locked = true;

    SHR_IF_ERR_VERBOSE_EXIT
        (mbcm_ltsw_l3_ecmp_flexctr_object_get(unit,
                                              intf_id,
                                              value));

exit:
    if (locked) {
        L3_EGRESS_UNLOCK(unit);
    }
    SHR_FUNC_EXIT();
}

/*!
 * \brief clear all source ECMP table.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_clear(
    int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_clear(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Set source ECMP.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] ecmp_info ECMP group info.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_set(
    int unit,
    int idx,
    bcm_l3_egress_ecmp_t *ecmp_info,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_set(unit, idx, ecmp_info, member, count));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index of source ECMP group.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_del(
    int unit,
    int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_del(unit, idx));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Add a member to a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Pointer to ECMP member info.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_member_add(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_member_add(unit, idx, member));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete a member from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 * \param [in] member Array of ECMP member info.
 * \param [in] count Count of ECMP member.
 * \param [in] del_member Information to ECMP member to be deleted.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_member_del(
    int unit,
    int idx,
    bcmi_ltsw_l3_ecmp_member_info_t *member,
    int count,
    bcmi_ltsw_l3_ecmp_member_info_t *del_member)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_member_del(unit, idx, member, count, del_member));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Delete all members from a source ECMP group.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Entry index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmint_ltsw_l3_secmp_member_del_all(
    int unit,
    int idx)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_secmp_member_del_all(unit, idx));

exit:
    SHR_FUNC_EXIT();
}
