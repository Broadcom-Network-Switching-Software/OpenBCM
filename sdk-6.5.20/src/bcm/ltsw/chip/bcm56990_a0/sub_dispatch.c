/*! \file sub_dispatch.c
 *
 * BCM56990_A0 subordinate dispatch functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include "sub_dispatch.h"

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_COMMON

/* Subordinate attach function per module. */
#define BCM56990_A0_SUB_DEV_ENTRY(_dn,_vn,_pf,_pd,_r0) \
    extern int _vn##_dev_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_uft_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_port_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l3_egress_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l2_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_mpls_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l3_intf_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_lb_hash_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l3_fib_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l3_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_vlan_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_mirror_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_flexctr_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_stat_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_virtual_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_flow_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_tunnel_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_field_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_int_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_l3_aacl_sub_drv_attach(int unit, void *hdl); \
    extern int _vn##_stg_sub_drv_attach(int unit, void *hdl);
#include "sub_devlist.h"

/* Subordinate attach function type. */
typedef int (*sub_drv_attach_f)(int unit, void *hdl);

/* Supported subordinate devices with attach functions. */
#define BCM56990_A0_SUB_DEV_ENTRY(_dn,_vn,_pf,_pd,_r0) \
    { \
        #_dn, \
        { \
            NULL, \
            _vn##_dev_sub_drv_attach, \
            _vn##_uft_sub_drv_attach, \
            _vn##_port_sub_drv_attach, \
            _vn##_l3_egress_sub_drv_attach, \
            _vn##_l2_sub_drv_attach, \
            _vn##_mpls_sub_drv_attach, \
            _vn##_l3_intf_sub_drv_attach, \
            _vn##_lb_hash_sub_drv_attach, \
            _vn##_l3_fib_sub_drv_attach, \
            _vn##_l3_sub_drv_attach, \
            _vn##_vlan_sub_drv_attach, \
            _vn##_mirror_sub_drv_attach, \
            _vn##_flexctr_sub_drv_attach, \
            _vn##_stat_sub_drv_attach, \
            _vn##_virtual_sub_drv_attach, \
            _vn##_flow_sub_drv_attach, \
            _vn##_tunnel_sub_drv_attach, \
            _vn##_field_sub_drv_attach, \
            _vn##_int_sub_drv_attach, \
            _vn##_l3_aacl_sub_drv_attach, \
            _vn##_stg_sub_drv_attach, \
        } \
    },
static struct {
    const char *dev_desc;
    sub_drv_attach_f drv_attachs[BCM56990_A0_SUB_MOD_CNT];
} sub_devs[] = {
    #include "sub_devlist.h"
    {NULL, {0}}
};
static int subdev_num = sizeof(sub_devs) / sizeof(sub_devs[0]);

/* All subordinate drivers. */
#define BCM56990_A0_SUB_DRV_ENTRY(_dn,_bd) \
    { \
        #_dn, \
    },
static struct {
    const char *drv_desc;
} sub_drvs[] = {
    {NULL},
    #include "sub_devlist.h"
    {NULL}
};

/******************************************************************************
 * Public functions
 */

int
bcm56990_a0_sub_drv_attach(int unit, void *hdl,
                           bcm56990_a0_sub_drv_t drv,
                           bcm56990_a0_sub_mod_t mod)
{
    int dev;

    SHR_FUNC_ENTER(unit);

    if (drv <= BCM56990_A0_SUB_DRV_NONE ||
        drv >= BCM56990_A0_SUB_DRV_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    if (mod <= BCM56990_A0_SUB_MOD_INVAL ||
        mod >= BCM56990_A0_SUB_MOD_CNT) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_PARAM);
    }

    for (dev = 0; dev < subdev_num; dev++) {
        if (!sub_devs[dev].dev_desc || !sub_devs[dev].drv_attachs[mod]) {
            continue;
        }
        if (!sal_strcmp(sub_drvs[drv].drv_desc, sub_devs[dev].dev_desc)) {
            break;
        }
    }
    if (dev == subdev_num) {
        SHR_IF_ERR_VERBOSE_EXIT(SHR_E_UNAVAIL);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (sub_devs[dev].drv_attachs[mod](unit, hdl));

exit:
    SHR_FUNC_EXIT();
}
