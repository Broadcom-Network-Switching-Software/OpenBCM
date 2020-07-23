/*! \file l3_egress.c
 *
 * L3 egress management.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/xgs/l3_egress.h>
#include "sub_dispatch.h"

#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_L3

/******************************************************************************
 * Private functions
 */

/*!
 * \brief L3_egress driver function variable for bcm56990_a0 device.
 */
static mbcm_ltsw_l3_egress_drv_t bcm56990_a0_ltsw_l3_egress_drv = {
    .l3_egress_init = xgs_ltsw_l3_egress_init,
    .l3_egress_deinit = xgs_ltsw_l3_egress_deinit,
    .l3_egress_id_range_get = xgs_ltsw_l3_egress_id_range_get,
    .l3_egress_create = xgs_ltsw_l3_egress_create,
    .l3_egress_destroy = xgs_ltsw_l3_egress_destroy,
    .l3_egress_get = xgs_ltsw_l3_egress_get,
    .l3_egress_find = xgs_ltsw_l3_egress_find,
    .l3_egress_traverse = xgs_ltsw_l3_egress_traverse,
    .l3_egress_obj_id_resolve = xgs_ltsw_l3_egress_obj_id_resolve,
    .l3_egress_obj_id_construct = xgs_ltsw_l3_egress_obj_id_construct,
    .l3_egress_dvp_set = xgs_ltsw_l3_egress_dvp_set,
    .l3_egress_obj_cnt_get = xgs_ltsw_l3_egress_obj_cnt_get,
    .l3_egress_flexctr_attach = xgs_ltsw_l3_egress_flexctr_attach,
    .l3_egress_flexctr_detach = xgs_ltsw_l3_egress_flexctr_detach,
    .l3_egress_flexctr_info_get = xgs_ltsw_l3_egress_flexctr_info_get,
    .l3_egress_flexctr_object_set = xgs_ltsw_l3_egress_flexctr_object_set,
    .l3_egress_flexctr_object_get = xgs_ltsw_l3_egress_flexctr_object_get,
    .l3_ecmp_create = xgs_ltsw_l3_ecmp_create,
    .l3_ecmp_destroy = xgs_ltsw_l3_ecmp_destroy,
    .l3_ecmp_get = xgs_ltsw_l3_ecmp_get,
    .l3_ecmp_member_add = xgs_ltsw_l3_ecmp_member_add,
    .l3_ecmp_member_del = xgs_ltsw_l3_ecmp_member_del,
    .l3_ecmp_member_del_all = xgs_ltsw_l3_ecmp_member_del_all,
    .l3_ecmp_find = xgs_ltsw_l3_ecmp_find,
    .l3_ecmp_trav = xgs_ltsw_l3_ecmp_trav,
    .l3_ecmp_max_paths_get = xgs_ltsw_l3_ecmp_max_paths_get,
    .l3_ecmp_hash_ctrl_set = BCM56990_A0_SUB_DRV,
    .l3_ecmp_hash_ctrl_get = BCM56990_A0_SUB_DRV,
    .l3_ecmp_member_status_set = xgs_ltsw_l3_ecmp_member_status_set,
    .l3_ecmp_member_status_get = xgs_ltsw_l3_ecmp_member_status_get,
    .l3_ecmp_ethertype_set = xgs_ltsw_l3_ecmp_ethertype_set,
    .l3_ecmp_ethertype_get = xgs_ltsw_l3_ecmp_ethertype_get,
    .l3_ecmp_dlb_mon_config_set = xgs_ltsw_l3_ecmp_dlb_mon_config_set,
    .l3_ecmp_dlb_mon_config_get = xgs_ltsw_l3_ecmp_dlb_mon_config_get,
    .l3_egress_size_get = xgs_ltsw_l3_egress_size_get,
    .l3_egress_count_get = xgs_ltsw_l3_egress_count_get,
    .l3_ecmp_size_get = xgs_ltsw_l3_ecmp_size_get,
    .l3_ecmp_count_get = xgs_ltsw_l3_ecmp_count_get,
    .l3_ecmp_flexctr_attach = xgs_ltsw_l3_ecmp_flexctr_attach,
    .l3_ecmp_flexctr_detach = xgs_ltsw_l3_ecmp_flexctr_detach,
    .l3_ecmp_flexctr_info_get = xgs_ltsw_l3_ecmp_flexctr_info_get,
    .l3_ecmp_flexctr_object_set = xgs_ltsw_l3_ecmp_flexctr_object_set,
    .l3_ecmp_flexctr_object_get = xgs_ltsw_l3_ecmp_flexctr_object_get,
    .l3_egress_tbl_db_get = BCM56990_A0_SUB_DRV,
    .l3_ecmp_tbl_db_get = BCM56990_A0_SUB_DRV,
    .l3_ecmp_flexctr_action_tbl_db_get = BCM56990_A0_SUB_DRV,
    .l3_overlay_table_init = BCM56990_A0_SUB_DRV,
    .l3_ecmp_dlb_stat_set = xgs_ltsw_l3_ecmp_dlb_stat_set,
    .l3_ecmp_dlb_stat_get = xgs_ltsw_l3_ecmp_dlb_stat_get,
    .l3_ecmp_dlb_stat_sync_get = xgs_ltsw_l3_ecmp_dlb_stat_sync_get,
    .l3_egress_add_l2tag = xgs_ltsw_l3_egress_add_l2tag,
    .l3_egress_get_l2tag = xgs_ltsw_l3_egress_get_l2tag,
    .l3_egress_delete_l2tag = xgs_ltsw_l3_egress_delete_l2tag,
    .l3_egress_php_action_add = xgs_ltsw_l3_egress_php_action_add,
    .l3_egress_php_action_del = xgs_ltsw_l3_egress_php_action_del,
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_ltsw_l3_egress_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_l3_egress_drv_set(unit, &bcm56990_a0_ltsw_l3_egress_drv));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_l3_egress_drv,
                                    BCM56990_A0_SUB_DRV_BCM56990_Ax,
                                    BCM56990_A0_SUB_MOD_L3_EGRESS));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Attach functions for other members.
 */
#define BCM56990_A0_DRV_ATTACH_ENTRY(_dn,_vn,_pf,_pd,_r0) \
int _vn##_ltsw_l3_egress_drv_attach(int unit) \
{ \
    SHR_FUNC_ENTER(unit); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (mbcm_ltsw_l3_egress_drv_set(unit, &bcm56990_a0_ltsw_l3_egress_drv)); \
    SHR_IF_ERR_VERBOSE_EXIT \
        (bcm56990_a0_sub_drv_attach(unit, &bcm56990_a0_ltsw_l3_egress_drv, \
                                    BCM56990_A0_SUB_DRV_BCM5699x, \
                                    BCM56990_A0_SUB_MOD_L3_EGRESS)); \
exit: \
    SHR_FUNC_EXIT(); \
}
#include "sub_devlist.h"
