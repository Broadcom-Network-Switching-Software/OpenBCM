/*! \file mbcm.c
 *
 * BCM multiplexing.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bcm_int/control.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/mbcm/feature.h>
#include <bcm_int/ltsw/mbcm/dev.h>
#include <bcm_int/ltsw/mbcm/dev_misc.h>
#include <bcm_int/ltsw/mbcm/ecn.h>
#include <bcm_int/ltsw/mbcm/l2.h>
#include <bcm_int/ltsw/mbcm/l3.h>
#include <bcm_int/ltsw/mbcm/l3_intf.h>
#include <bcm_int/ltsw/mbcm/l3_egress.h>
#include <bcm_int/ltsw/mbcm/l3_fib.h>
#include <bcm_int/ltsw/mbcm/port.h>
#include <bcm_int/ltsw/mbcm/stg.h>
#include <bcm_int/ltsw/mbcm/vlan.h>
#include <bcm_int/ltsw/mbcm/rate.h>
#include <bcm_int/ltsw/mbcm/trunk.h>
#include <bcm_int/ltsw/mbcm/cosq.h>
#include <bcm_int/ltsw/mbcm/cosq_thd.h>
#include <bcm_int/ltsw/mbcm/cosq_stat.h>
#include <bcm_int/ltsw/mbcm/bst.h>
#include <bcm_int/ltsw/mbcm/ebst.h>
#include <bcm_int/ltsw/mbcm/obm.h>
#include <bcm_int/ltsw/mbcm/pfc.h>
#include <bcm_int/ltsw/mbcm/flow.h>
#include <bcm_int/ltsw/mbcm/stat.h>
#include <bcm_int/ltsw/mbcm/virtual.h>
#include <bcm_int/ltsw/mbcm/flexctr.h>
#include <bcm_int/ltsw/mbcm/flexstate.h>
#include <bcm_int/ltsw/mbcm/switch.h>
#include <bcm_int/ltsw/mbcm/time.h>
#include <bcm_int/ltsw/mbcm/tunnel.h>
#include <bcm_int/ltsw/mbcm/failover.h>
#include <bcm_int/ltsw/mbcm/mirror.h>
#include <bcm_int/ltsw/mbcm/multicast.h>
#include <bcm_int/ltsw/mbcm/mpls.h>
#include <bcm_int/ltsw/mbcm/field.h>
#include <bcm_int/ltsw/mbcm/range.h>
#include <bcm_int/ltsw/mbcm/stack.h>
#include <bcm_int/ltsw/mbcm/dlb.h>
#include <bcm_int/ltsw/mbcm/ipmc.h>
#include <bcm_int/ltsw/mbcm/sbr.h>
#include <bcm_int/ltsw/mbcm/qos.h>
#include <bcm_int/ltsw/mbcm/nat.h>
#include <bcm_int/ltsw/mbcm/flowtracker.h>
#include <bcm_int/ltsw/mbcm/oob.h>
#include <bcm_int/ltsw/mbcm/flexdigest.h>
#include <bcm_int/ltsw/mbcm/mod.h>
#include <bcm_int/ltsw/mbcm/uft.h>
#include <bcm_int/ltsw/mbcm/rx.h>
#include <bcm_int/ltsw/mbcm/hash_output_selection.h>
#include <bcm_int/ltsw/mbcm/udf.h>
#include <bcm_int/ltsw/mbcm/lb_hash.h>
#include <bcm_int/ltsw/mbcm/policer.h>
#include <bcm_int/ltsw/mbcm/pktio.h>
#include <bcm_int/ltsw/mbcm/ldh.h>
#include <bcm_int/ltsw/mbcm/uft.h>
#include <bcm_int/ltsw/mbcm/ifa.h>
#include <bcm_int/ltsw/mbcm/int.h>
#include <bcm_int/ltsw/mbcm/l3_aacl.h>

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>

/******************************************************************************
 * Local definitions
 */

#define BSL_LOG_MODULE BSL_LS_BCM_INIT

typedef enum mbcm_modules_e {
    MBCM_FEATURE,
    MBCM_FLOW,
    MBCM_DEV,
    MBCM_DEV_MISC,
    MBCM_ECN,
    MBCM_L2,
    MBCM_L3,
    MBCM_L3_INTF,
    MBCM_L3_EGRESS,
    MBCM_L3_FIB,
    MBCM_PORT,
    MBCM_STG,
    MBCM_VLAN,
    MBCM_RATE,
    MBCM_TRUNK,
    MBCM_STAT,
    MBCM_VIRTUAL,
    MBCM_FLEXCTR,
    MBCM_SC,
    MBCM_TIME,
    MBCM_COSQ,
    MBCM_COSQ_THD,
    MBCM_COSQ_STAT,
    MBCM_COSQ_BST,
    MBCM_COSQ_EBST,
    MBCM_TUNNEL,
    MBCM_FAILOVER,
    MBCM_MIRROR,
    MBCM_MULTICAST,
    MBCM_MPLS,
    MBCM_FIELD,
    MBCM_STACK,
    MBCM_DLB,
    MBCM_IPMC,
    MBCM_SBR,
    MBCM_PFC,
    MBCM_OBM,
    MBCM_QOS,
    MBCM_NAT,
    MBCM_FLOWTRACKER,
    MBCM_OOB,
    MBCM_FLEXDIGEST,
    MBCM_COSQ_MOD,
    MBCM_HASH,
    MBCM_RX,
    MBCM_HASH_OUTPUT_SELECTION,
    MBCM_UDF,
    MBCM_RANGE,
    MBCM_LB_HASH,
    MBCM_POLICER,
    MBCM_FLEXSTATE,
    MBCM_PKTIO,
    MBCM_LDH,
    MBCM_IFA,
    MBCM_INT,
    MBCM_L3_AACL,
    MBCM_MODULE_MAX
} mbcm_modules_t;

/*! Declare chip specific attach functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bd##_ltsw_feature_drv_attach(int unit); \
extern int _bd##_ltsw_dev_drv_attach(int unit);      \
extern int _bd##_ltsw_dev_misc_drv_attach(int unit); \
extern int _bd##_ltsw_ecn_drv_attach(int unit);      \
extern int _bd##_ltsw_l2_drv_attach(int unit);      \
extern int _bd##_ltsw_l3_drv_attach(int unit);      \
extern int _bd##_ltsw_l3_intf_drv_attach(int unit); \
extern int _bd##_ltsw_l3_egress_drv_attach(int unit); \
extern int _bd##_ltsw_l3_fib_drv_attach(int unit); \
extern int _bd##_ltsw_port_drv_attach(int unit);    \
extern int _bd##_ltsw_stg_drv_attach(int unit);     \
extern int _bd##_ltsw_vlan_drv_attach(int unit);    \
extern int _bd##_ltsw_rate_drv_attach(int unit);    \
extern int _bd##_ltsw_trunk_drv_attach(int unit);   \
extern int _bd##_ltsw_flow_drv_attach(int unit);    \
extern int _bd##_ltsw_stat_drv_attach(int unit);    \
extern int _bd##_ltsw_virtual_drv_attach(int unit); \
extern int _bd##_ltsw_flexctr_drv_attach(int unit); \
extern int _bd##_ltsw_sc_drv_attach(int unit);    \
extern int _bd##_ltsw_time_drv_attach(int unit);    \
extern int _bd##_ltsw_tunnel_drv_attach(int unit);    \
extern int _bd##_ltsw_cosq_drv_attach(int unit);    \
extern int _bd##_ltsw_cosq_thd_drv_attach(int unit); \
extern int _bd##_ltsw_cosq_stat_drv_attach(int unit); \
extern int _bd##_ltsw_cosq_bst_drv_attach(int unit); \
extern int _bd##_ltsw_cosq_ebst_drv_attach(int unit); \
extern int _bd##_ltsw_failover_drv_attach(int unit); \
extern int _bd##_ltsw_mirror_drv_attach(int unit);   \
extern int _bd##_ltsw_multicast_drv_attach(int unit);\
extern int _bd##_ltsw_mpls_drv_attach(int unit);     \
extern int _bd##_ltsw_field_drv_attach(int unit);    \
extern int _bd##_ltsw_stack_drv_attach(int unit);    \
extern int _bd##_ltsw_dlb_drv_attach(int unit);      \
extern int _bd##_ltsw_ipmc_drv_attach(int unit);     \
extern int _bd##_ltsw_sbr_drv_attach(int unit);      \
extern int _bd##_ltsw_pfc_drv_attach(int unit);      \
extern int _bd##_ltsw_obm_drv_attach(int unit);      \
extern int _bd##_ltsw_qos_drv_attach(int unit);      \
extern int _bd##_ltsw_nat_drv_attach(int unit);      \
extern int _bd##_ltsw_flowtracker_drv_attach(int unit); \
extern int _bd##_ltsw_oob_drv_attach(int unit); \
extern int _bd##_ltsw_flexdigest_drv_attach(int unit); \
extern int _bd##_ltsw_cosq_mod_drv_attach(int unit); \
extern int _bd##_ltsw_uft_drv_attach(int unit); \
extern int _bd##_ltsw_rx_drv_attach(int unit); \
extern int _bd##_ltsw_hos_drv_attach(int unit); \
extern int _bd##_ltsw_udf_drv_attach(int unit); \
extern int _bd##_ltsw_range_drv_attach(int unit); \
extern int _bd##_ltsw_lb_hash_drv_attach(int unit); \
extern int _bd##_ltsw_policer_drv_attach(int unit); \
extern int _bd##_ltsw_flexstate_drv_attach(int unit); \
extern int _bd##_ltsw_pktio_drv_attach(int unit); \
extern int _bd##_ltsw_ldh_drv_attach(int unit); \
extern int _bd##_ltsw_ifa_drv_attach(int unit); \
extern int _bd##_ltsw_int_drv_attach(int unit); \
extern int _bd##_ltsw_l3_aacl_drv_attach(int unit); \

#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>

/*! Attach function types for each module. */
typedef int (*drv_attach_f)(int unit);

/*! Array of chip specific attach functions */
#undef BCMDRD_DEVLIST_OVERRIDE
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    {{   \
        _bd##_ltsw_feature_drv_attach,  \
        _bd##_ltsw_dev_drv_attach,    \
        _bd##_ltsw_dev_misc_drv_attach,       \
        _bd##_ltsw_ecn_drv_attach,    \
        _bd##_ltsw_l2_drv_attach,    \
        _bd##_ltsw_l3_drv_attach,    \
        _bd##_ltsw_l3_intf_drv_attach,      \
        _bd##_ltsw_l3_egress_drv_attach,    \
        _bd##_ltsw_l3_fib_drv_attach,    \
        _bd##_ltsw_port_drv_attach,  \
        _bd##_ltsw_stg_drv_attach,   \
        _bd##_ltsw_vlan_drv_attach,  \
        _bd##_ltsw_rate_drv_attach,  \
        _bd##_ltsw_trunk_drv_attach, \
        _bd##_ltsw_flow_drv_attach,  \
        _bd##_ltsw_stat_drv_attach,  \
        _bd##_ltsw_virtual_drv_attach, \
        _bd##_ltsw_flexctr_drv_attach, \
        _bd##_ltsw_sc_drv_attach,   \
        _bd##_ltsw_time_drv_attach,   \
        _bd##_ltsw_tunnel_drv_attach,   \
        _bd##_ltsw_cosq_drv_attach,   \
        _bd##_ltsw_cosq_thd_drv_attach, \
        _bd##_ltsw_cosq_stat_drv_attach, \
        _bd##_ltsw_cosq_bst_drv_attach, \
        _bd##_ltsw_cosq_ebst_drv_attach, \
        _bd##_ltsw_failover_drv_attach, \
        _bd##_ltsw_mirror_drv_attach,   \
        _bd##_ltsw_multicast_drv_attach,\
        _bd##_ltsw_mpls_drv_attach,     \
        _bd##_ltsw_field_drv_attach,    \
        _bd##_ltsw_stack_drv_attach,    \
        _bd##_ltsw_dlb_drv_attach,      \
        _bd##_ltsw_ipmc_drv_attach,     \
        _bd##_ltsw_sbr_drv_attach,      \
        _bd##_ltsw_pfc_drv_attach,      \
        _bd##_ltsw_obm_drv_attach,      \
        _bd##_ltsw_qos_drv_attach,      \
        _bd##_ltsw_nat_drv_attach,      \
        _bd##_ltsw_flowtracker_drv_attach,       \
        _bd##_ltsw_oob_drv_attach,      \
        _bd##_ltsw_flexdigest_drv_attach, \
        _bd##_ltsw_cosq_mod_drv_attach, \
        _bd##_ltsw_uft_drv_attach,     \
        _bd##_ltsw_rx_drv_attach,  \
        _bd##_ltsw_hos_drv_attach,     \
        _bd##_ltsw_udf_drv_attach,     \
        _bd##_ltsw_range_drv_attach,    \
        _bd##_ltsw_lb_hash_drv_attach,  \
        _bd##_ltsw_policer_drv_attach,  \
        _bd##_ltsw_flexstate_drv_attach, \
        _bd##_ltsw_pktio_drv_attach, \
        _bd##_ltsw_ldh_drv_attach, \
        _bd##_ltsw_ifa_drv_attach, \
        _bd##_ltsw_int_drv_attach, \
        _bd##_ltsw_l3_aacl_drv_attach \
     }},
static struct {
    drv_attach_f drv_attach[MBCM_MODULE_MAX];
} list_dev_attach[] = {
    {{ 0 }}, /* dummy entry for type "none" */
#include <bcmdrd/bcmdrd_devlist.h>
    {{ 0 }} /* end-of-list */
};

static int fn_unavail(int unit, ...)
{
    return SHR_E_UNAVAIL;
}

static mbcm_ltsw_fnptr_t fnptr_unavail = &fn_unavail;

/******************************************************************************
 * Public functions
 */

int
mbcm_ltsw_init(int unit)
{
    int dunit, m;
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dunit = bcmi_ltsw_dev_dunit(unit);

    dev_type = bcmdrd_dev_type(dunit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    for (m = 0; m < MBCM_MODULE_MAX; m++) {
        SHR_IF_ERR_EXIT
            (list_dev_attach[dev_type].drv_attach[m](unit));
    }

exit:
    SHR_FUNC_EXIT();
}

int
mbcm_ltsw_deinit(int unit)
{
    mbcm_ltsw_feature_drv_set(unit, NULL);

    mbcm_ltsw_dev_drv_set(unit, NULL);

    mbcm_ltsw_dev_misc_drv_set(unit, NULL);

    mbcm_ltsw_l2_drv_set(unit, NULL);

    mbcm_ltsw_l3_drv_set(unit, NULL);

    mbcm_ltsw_l3_intf_drv_set(unit, NULL);

    mbcm_ltsw_l3_egress_drv_set(unit, NULL);

    mbcm_ltsw_l3_fib_drv_set(unit, NULL);

    mbcm_ltsw_port_drv_set(unit, NULL);

    mbcm_ltsw_stg_drv_set(unit, NULL);

    mbcm_ltsw_vlan_drv_set(unit, NULL);

    mbcm_ltsw_rate_drv_set(unit, NULL);

    mbcm_ltsw_trunk_drv_set(unit, NULL);

    mbcm_ltsw_flow_drv_set(unit, NULL);

    mbcm_ltsw_stat_drv_set(unit, NULL);

    mbcm_ltsw_virtual_drv_set(unit, NULL);

    mbcm_ltsw_flexctr_drv_set(unit, NULL);

    mbcm_ltsw_sc_drv_set(unit, NULL);

    mbcm_ltsw_time_drv_set(unit, NULL);

    mbcm_ltsw_cosq_drv_set(unit, NULL);

    mbcm_ltsw_cosq_thd_drv_set(unit, NULL);

    mbcm_ltsw_cosq_stat_drv_set(unit, NULL);

    mbcm_ltsw_cosq_bst_drv_set(unit, NULL);

    mbcm_ltsw_cosq_ebst_drv_set(unit, NULL);

    mbcm_ltsw_tunnel_drv_set(unit, NULL);

    mbcm_ltsw_failover_drv_set(unit, NULL);

    mbcm_ltsw_mirror_drv_set(unit, NULL);

    mbcm_ltsw_multicast_drv_set(unit, NULL);

    mbcm_ltsw_mpls_drv_set(unit, NULL);

    mbcm_ltsw_field_drv_set(unit, NULL);

    mbcm_ltsw_range_drv_set(unit, NULL);

    mbcm_ltsw_stack_drv_set(unit, NULL);

    mbcm_ltsw_dlb_drv_set(unit, NULL);

    mbcm_ltsw_ipmc_drv_set(unit, NULL);

    mbcm_ltsw_sbr_drv_set(unit, NULL);

    mbcm_ltsw_pfc_drv_set(unit, NULL);

    mbcm_ltsw_obm_drv_set(unit, NULL);

    mbcm_ltsw_qos_drv_set(unit, NULL);

    mbcm_ltsw_nat_drv_set(unit, NULL);

    mbcm_ltsw_flowtracker_drv_set(unit, NULL);

    mbcm_ltsw_oob_drv_set(unit, NULL);

    mbcm_ltsw_flexdigest_drv_set(unit, NULL);

    mbcm_ltsw_cosq_mod_drv_set(unit, NULL);

    mbcm_ltsw_uft_drv_set(unit, NULL);

    mbcm_ltsw_rx_drv_set(unit, NULL);

    mbcm_ltsw_hos_drv_set(unit, NULL);

    mbcm_ltsw_udf_drv_set(unit, NULL);

    mbcm_ltsw_lb_hash_drv_set(unit, NULL);

    mbcm_ltsw_policer_drv_set(unit, NULL);

    mbcm_ltsw_flexstate_drv_set(unit, NULL);

    mbcm_ltsw_pktio_drv_set(unit, NULL);

    mbcm_ltsw_ldh_drv_set(unit, NULL);

    mbcm_ltsw_uft_drv_set(unit, NULL);

    mbcm_ltsw_ifa_drv_set(unit, NULL);

#if defined(INCLUDE_INT)
    mbcm_ltsw_int_drv_set(unit, NULL);
#endif

    mbcm_ltsw_l3_aacl_drv_set(unit, NULL);

    return SHR_E_NONE;
}

void
mbcm_ltsw_drv_init(mbcm_ltsw_drv_t *dst,
                   mbcm_ltsw_drv_t *src,
                   int cnt)
{
    const uint8_t *s = (const uint8_t *)src;
    uint8_t *d = (uint8_t *)dst;
    int i, j;

    for (i = 0; i < cnt; i++) {
        for (j = 0; j < MBCM_LTSW_FNPTR_SZ; j++) {
            if (s && s[j] != 0)  {
                sal_memcpy(d, s, MBCM_LTSW_FNPTR_SZ);
                break;
            }
        }

        if (j == MBCM_LTSW_FNPTR_SZ) {
            for (j = 0; j < MBCM_LTSW_FNPTR_SZ; j++) {
                if (d[j] != 0) {
                    break;
                }
            }
        }

        if ((!s) || (j == MBCM_LTSW_FNPTR_SZ)) {
            sal_memcpy(d, &fnptr_unavail, MBCM_LTSW_FNPTR_SZ);
        }

        d += MBCM_LTSW_FNPTR_SZ;
        if (s) {
            s += MBCM_LTSW_FNPTR_SZ;
        }
    }
}
