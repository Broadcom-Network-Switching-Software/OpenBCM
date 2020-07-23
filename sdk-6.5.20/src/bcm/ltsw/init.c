/*! \file init.c
 *
 * BCM Library Initialization
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/core/boot.h>

#include <bcm/init.h>
#include <bcm/module.h>
#include <shared/shr_bprof.h>

#include <bcm_int/control.h>
#include <bcm_int/ltsw_dispatch.h>
#include <bcm_int/ltsw/issu.h>
#include <bcm_int/ltsw/dev.h>
#include <bcm_int/ltsw/dev_misc.h>
#include <bcm_int/ltsw/ha.h>
#include <bcm_int/ltsw/mbcm/mbcm.h>
#include <bcm_int/ltsw/profile_mgmt.h>
#include <bcm_int/ltsw/feature.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/stg.h>
#include <bcm_int/ltsw/vlan.h>
#include <bcm_int/ltsw/range_int.h>
#include <bcm_int/ltsw/rate.h>
#include <bcm_int/ltsw/stat.h>
#include <bcm_int/ltsw/event_mgr.h>
#include <bcm_int/ltsw/virtual.h>
#include <bcm_int/ltsw/flexctr.h>
#include <bcm_int/ltsw/flexstate.h>
#include <bcm_int/ltsw/failover.h>
#include <bcm_int/ltsw/mirror.h>
#include <bcm_int/ltsw/index_table_mgmt.h>
#include <bcm_int/ltsw/stack.h>
#include <bcm_int/ltsw/dlb.h>
#include <bcm_int/ltsw/hash_output_selection.h>
#include <bcm_int/ltsw/switch.h>
#include <bcm_int/ltsw/sbr.h>
#include <bcm_int/ltsw/property.h>
#include <bcm_int/ltsw/qos.h>
#include <bcm_int/ltsw/ser.h>
#include <bcm_int/ltsw/uft.h>
#include <bcm_int/ltsw/lb_hash.h>
#include <bcm_int/ltsw/rx.h>
#include <bcm_int/ltsw/policer.h>
#include <bcm_int/ltsw/tunnel.h>
#if defined(INCLUDE_XFLOW_MACSEC)
#include <bcm_int/ltsw/xflow_macsec.h>
#endif
#include <shr/shr_debug.h>

/******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCM_INIT

#define BCM_CHECK_ERROR_RETURN(unit, rv, module)                           \
    if (SHR_FAILURE(rv) && (rv != SHR_E_UNAVAIL)) {                        \
        LOG_WARN(BSL_LOG_MODULE,                                           \
                 (BSL_META_U(unit, "bcm_init failed in %s\n"),             \
                  bcm_module_name(unit, module)));                         \
        ltsw_init_flag[unit] = 0;                                          \
        return(rv);                                                        \
    }

static sal_usecs_t bcm_modules_init_time;

#define BCM_MOD_INIT_CER(unit, rtn, module, dispname, init) {              \
    if (init) {                                                            \
        sal_usecs_t stime, etime;                                          \
        int bcm_rv;                                                        \
        LOG_VERBOSE(BSL_LOG_MODULE,                                        \
                    (BSL_META_U(unit, "bcm_init: (%s)\n"),                 \
                     bcm_module_name(unit, module)));                      \
        stime = sal_time_usecs();                                          \
        SHR_BPROF_STATS_TIME(dispname) {                                   \
            bcm_rv = rtn(unit);                                            \
            BCM_CHECK_ERROR_RETURN(unit, bcm_rv, module);                  \
        }                                                                  \
        etime = sal_time_usecs();                                          \
        LOG_VERBOSE(BSL_LOG_MODULE,                                        \
                    (BSL_META_U(unit,                                      \
                                "bcm_init: %8s   took %10d usec\n"),       \
                                bcm_module_name(unit, module),             \
                                SAL_USECS_SUB(etime, stime)));             \
        bcm_modules_init_time += SAL_USECS_SUB(etime, stime);              \
    } else {                                                               \
        LOG_WARN(BSL_LOG_MODULE,                                           \
                 (BSL_META_U(unit, "bcm_init: skipped %s init\n"),         \
                  bcm_module_name(unit, module)));                         \
    }                                                                      \
}

#define BCM_MOD_SEL_INIT_CER(unit, module, dispname, init) {               \
    if (init) {                                                            \
        sal_usecs_t stime, etime;                                          \
        int bcm_rv;                                                        \
        LOG_VERBOSE(BSL_LOG_MODULE,                                        \
                    (BSL_META_U(unit, "bcm_init: (%s)\n"),                 \
                     bcm_module_name(unit, module)));                      \
        stime = sal_time_usecs();                                          \
        SHR_BPROF_STATS_TIME(dispname) {                                   \
            bcm_rv = bcm_ltsw_init_selective(unit, module);                \
            BCM_CHECK_ERROR_RETURN(unit, bcm_rv, module);                  \
        }                                                                  \
        etime = sal_time_usecs();                                          \
        LOG_VERBOSE(BSL_LOG_MODULE,                                        \
                    (BSL_META_U(unit,                                      \
                                "bcm_init: %8s   took %10d usec\n"),       \
                                bcm_module_name(unit, module),             \
                                SAL_USECS_SUB(etime, stime)));             \
        bcm_modules_init_time += SAL_USECS_SUB(etime, stime);              \
    } else {                                                               \
        LOG_WARN(BSL_LOG_MODULE,                                           \
                 (BSL_META_U(unit, "bcm_init: skipped %s init\n"),         \
                  bcm_module_name(unit, module)));                         \
    }                                                                      \
}

/* LTSW init flag */
static int ltsw_init_flag[BCM_MAX_NUM_UNITS] = { 0 };

/******************************************************************************
 * Private functions
 */

/******************************************************************************
 * Public Functions
 */

/*!
 * \brief Initialize common resource for BCM API layer.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
bcm_ltsw_init_common(int unit)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_uft_init(unit));

    if (ltsw_feature(unit, LTSW_FT_HASH_OUTPUT_SELECTION_PROFILE)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_hos_init(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_LB_HASH)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_lb_hash_init(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_SBR)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_sbr_init(unit));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmi_ltsw_dev_misc_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_ser_init(unit));

    if (ltsw_feature(unit, LTSW_FT_TNL_SEQ_NUM_PRF)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmi_ltsw_tunnel_encap_seq_profile_init(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize BCM API layer.
 *
 * Initialize the BCM API layer only for LTSW devices, without resetting the
 * switch chip.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE  No errors.
 * \retval !SHR_E_NONE Failure.
 */
int
bcm_ltsw_init(int unit)
{
    SHR_BPROF_STATS_DECL;

    bcm_modules_init_time = 0;

    ltsw_init_flag[unit] = 1;

    BCM_MOD_INIT_CER(unit, bcm_ltsw_init_common, BCM_MODULE_COMMON,
                     SHR_BPROF_BCM_COMMON, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_PORT,
                         SHR_BPROF_BCM_PORT, true);

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW) ||
        (ltsw_feature(unit, LTSW_FT_MPLS) &&
         !ltsw_feature(unit, LTSW_FT_NO_L2_MPLS))) {
        BCM_MOD_INIT_CER(unit, bcmi_ltsw_virtual_init, BCM_MODULE_VIRTUAL,
                         SHR_BPROF_BCM_VIRTUAL, true);
    }

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FLEXCTR,
                         SHR_BPROF_BCM_FLEXCTR, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FLEXSTATE,
                         SHR_BPROF_BCM_FLEXSTATE, true);

    if (ltsw_feature(unit, LTSW_FT_FLEXDIGEST)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FLEXDIGEST,
                             SHR_BPROF_BCM_FLEXDIGEST, true);
    }

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_L2,
                         SHR_BPROF_BCM_L2, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_STG,
                         SHR_BPROF_BCM_STG, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_VLAN,
                         SHR_BPROF_BCM_VLAN, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_RATE,
                         SHR_BPROF_BCM_RATE, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_TRUNK,
                         SHR_BPROF_BCM_TRUNK, true);

    if (ltsw_feature(unit, LTSW_FT_STK)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_STACK,
                             SHR_BPROF_BCM_STK, true);
    }

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_COSQ,
                         SHR_BPROF_BCM_COSQ, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_RX,
                         SHR_BPROF_BCM_RX, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_LINKSCAN,
                         SHR_BPROF_BCM_LINKSCAN, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_PKTIO,
                         SHR_BPROF_BCM_PKTIO, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_MIRROR,
                         SHR_BPROF_BCM_MIRROR, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_QOS,
                         SHR_BPROF_BCM_QOS, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_L3,
                         SHR_BPROF_BCM_L3, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FAILOVER,
                         SHR_BPROF_BCM_FAILOVER, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FLOW,
                         SHR_BPROF_BCM_FLOW, true);

    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_MPLS,
                             SHR_BPROF_BCM_MPLS, true);
    }

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_STAT,
                         SHR_BPROF_BCM_STAT, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_MULTICAST,
                         SHR_BPROF_BCM_MULTICAST, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FIELD,
                         SHR_BPROF_BCM_FIELD, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_RANGE,
                         SHR_BPROF_BCM_RANGE, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_SWITCH,
                         SHR_BPROF_BCM_SWITCH, true);

    if (ltsw_feature(unit, LTSW_FT_TIME)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_TIME,
                             SHR_BPROF_BCM_TIME, true);
    }

    if (ltsw_feature(unit, LTSW_FT_IPMC)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_IPMC,
                             SHR_BPROF_BCM_IPMC, true);
    }

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_FLOWTRACKER,
                         SHR_BPROF_BCM_FT, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_COLLECTOR,
                         SHR_BPROF_BCM_COLLECTOR, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_TELEMETRY,
                         SHR_BPROF_BCM_TELEMETRY, true);
#if defined(INCLUDE_BFD)
    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_BFD,
                         SHR_BPROF_BCM_BFD, true);
#endif

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_UDF,
                         SHR_BPROF_BCM_UDF, true);

    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_POLICER,
                         SHR_BPROF_BCM_POLICER, true);

    if (ltsw_feature(unit, LTSW_FT_IFA)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_IFA,
                             SHR_BPROF_BCM_IFA, true);
    }

#if defined(INCLUDE_INT)
    BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_INT,
                         SHR_BPROF_BCM_INT, true);
#endif

    if (ltsw_feature(unit, LTSW_FT_LDH)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_LATENCY_MONITOR,
                             SHR_BPROF_BCM_LATENCY_MONITOR, true);
    }

#if defined(INCLUDE_XFLOW_MACSEC)
    if (ltsw_feature(unit, LTSW_FT_XFLOW_MACSEC)) {
        BCM_MOD_SEL_INIT_CER(unit, BCM_MODULE_XFLOW_MACSEC,
                             SHR_BPROF_BCM_XFLOW_MACSEC, true);
    }
#endif

    LOG_VERBOSE(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "bcm_init all modules took %10d usec\n"),
                            bcm_modules_init_time));

    ltsw_init_flag[unit] = 0;

    return BCM_E_NONE;
}

/*!
 * \brief Match BCM control subtype strings for LTSW types
 *
 * \param [in] unit Unit number
 * \param [in] subtype_a Pointer to subtype string a
 * \param [in] subtype_p Pointer to subtype string p
 *
 * \return 0 if match, otherwise not match
 */
int
_bcm_ltsw_match(int unit, char *subtype_a, char *subtype_b)
{
    COMPILER_REFERENCE(unit);
    return sal_strcmp(subtype_a, subtype_b);
}

/*!
 * \brief Attach and initialize LTSW device
 *
 * \param [in] unit Unit number
 * \param [in] subtype Pointer to subtype string
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
int
_bcm_ltsw_attach(int unit, char *subtype)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dev_init(unit));

    BCM_CONTROL(unit)->capability |= BCM_CAPA_LOCAL;
    BCM_CONTROL(unit)->chip_vendor = bcmi_ltsw_dev_vendor_id(unit);
    BCM_CONTROL(unit)->chip_device = bcmi_ltsw_dev_device_id(unit);
    BCM_CONTROL(unit)->chip_revision = bcmi_ltsw_dev_revision_id(unit);

    SHR_IF_ERR_EXIT
        (mbcm_ltsw_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_property_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_dev_info_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_feature_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_event_mgr_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_idx_tbl_mgmt_init(unit));

    SHR_IF_ERR_EXIT
        (bcmi_ltsw_issu_init(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_profile_init(unit));

    SHR_IF_ERR_EXIT
        (bcm_ltsw_init(unit));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Initialize specific bcm modules as desired.
 *
 * \param [in] unit Unit number.
 * \param [in] module_number Indicate module number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Not availale.
 */

int
bcm_ltsw_init_selective(int unit, uint32 module_number)
{
    SHR_FUNC_ENTER(unit);

    switch (module_number) {
    case BCM_MODULE_PORT:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_port_init(unit));
        break;
    case BCM_MODULE_L2:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_l2_init(unit));
        break;
    case BCM_MODULE_STG:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stg_init(unit));
        break;
    case BCM_MODULE_VLAN:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_vlan_init(unit));
        break;
    case BCM_MODULE_TRUNK:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_trunk_init(unit));
        break;
    case BCM_MODULE_STACK:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stk_init(unit));
        break;
    case BCM_MODULE_COSQ:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_cosq_init(unit));
        break;
    case BCM_MODULE_RX:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_rx_init(unit));
        break;
    case BCM_MODULE_LINKSCAN:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_linkscan_init(unit));
        break;
    case BCM_MODULE_MPLS:
        SHR_IF_ERR_EXIT
             (bcm_ltsw_mpls_init(unit));
        break;
    case BCM_MODULE_FLOW:
        if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_flow_init(unit));
        }
        break;
    case BCM_MODULE_STAT:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_stat_init(unit));
        break;
    case BCM_MODULE_FLEXCTR:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_flexctr_init(unit));
        break;
    case BCM_MODULE_FLEXSTATE:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_flexstate_init(unit));
        break;
    case BCM_MODULE_FLEXDIGEST:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_flexdigest_init(unit));
        break;
    case BCM_MODULE_L3:
        if (ltsw_feature(unit, LTSW_FT_L3) &&
                bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_l3_init(unit));
        }
        break;
    case BCM_MODULE_FAILOVER:
        if (ltsw_feature(unit, LTSW_FT_L3) &&
                bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_failover_init(unit));
        }
        break;
    case BCM_MODULE_RATE:
        if (ltsw_feature(unit, LTSW_FT_RATE)) {
            SHR_IF_ERR_EXIT
                (bcmi_ltsw_rate_init(unit));
        }
        break;
    case BCM_MODULE_PKTIO:
        if (SAL_BOOT_XGSSIM) {
            SHR_ERR_EXIT(SHR_E_UNAVAIL);
        } else {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_pktio_init(unit));
        }
        break;
    case BCM_MODULE_MIRROR:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_mirror_init(unit));
        break;
    case BCM_MODULE_MULTICAST:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_multicast_init(unit));
        break;
    case BCM_MODULE_FIELD:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_field_init(unit));
        break;
    case BCM_MODULE_RANGE:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_range_init(unit));
        break;
    case BCM_MODULE_SWITCH:
        SHR_IF_ERR_EXIT
            (bcmi_ltsw_switch_init(unit));
        break;
    case BCM_MODULE_TIME:
        if (ltsw_feature(unit, LTSW_FT_TIME)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_time_init(unit));
        }
        break;
    case BCM_MODULE_IPMC:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_ipmc_init(unit));
        break;
    case BCM_MODULE_QOS:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_qos_init(unit));
        break;
    case BCM_MODULE_FLOWTRACKER:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_flowtracker_init(unit));
        break;
    case BCM_MODULE_COLLECTOR:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_collector_init(unit));
        break;
    case BCM_MODULE_TELEMETRY:
        if (ltsw_feature(unit, LTSW_FT_TELEMETRY)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_telemetry_init(unit));
        }
        break;
    case BCM_MODULE_UDF:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_udf_init(unit));
        break;
    case BCM_MODULE_POLICER:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_policer_init(unit));
        break;
#if defined(INCLUDE_BFD)
    case BCM_MODULE_BFD:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_bfd_init(unit));
        break;
#endif
    case BCM_MODULE_IFA:
        if (ltsw_feature(unit, LTSW_FT_IFA)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_ifa_init(unit));
        }
        break;
#if defined(INCLUDE_INT)
    case BCM_MODULE_INT:
        SHR_IF_ERR_EXIT
            (bcm_ltsw_int_init(unit));
        break;
#endif
    case BCM_MODULE_LATENCY_MONITOR:
        if (ltsw_feature(unit, LTSW_FT_LDH)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_latency_monitor_init(unit));
        }
        break;
#if defined(INCLUDE_XFLOW_MACSEC)
    case BCM_MODULE_XFLOW_MACSEC:
        if (ltsw_feature(unit, LTSW_FT_XFLOW_MACSEC)) {
            SHR_IF_ERR_EXIT
                (bcm_ltsw_xflow_macsec_init(unit));
        }
        break;
#endif
    default:
        SHR_IF_ERR_EXIT(SHR_E_UNAVAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Return value from system_init() operation.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL Not availale.
 */
int
bcm_ltsw_init_check(int unit)
{
    if (!bcmi_ltsw_dev_exists(unit)) {
        return BCM_E_UNIT;
    }

    return BCM_CONTROL(unit)->system_initialized_rv;
}

/*!
 * \brief Detach LTSW device
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE No errors.
 */
int
_bcm_ltsw_detach(int unit)
{
    SHR_FUNC_ENTER(unit);


    if (ltsw_feature(unit, LTSW_FT_LDH)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_latency_monitor_detach(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_IPMC)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_ipmc_detach(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_STK)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_stk_deinit(unit));
    }

    SHR_IF_ERR_CONT
        (bcm_ltsw_field_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_range_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_udf_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_multicast_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_qos_detach(unit));

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_flow_cleanup(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_MPLS)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_mpls_cleanup(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_L3) &&
            bcmi_ltsw_property_get(unit, BCMI_CPN_L3_ENABLE, 1)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_l3_cleanup(unit));
        SHR_IF_ERR_CONT
            (bcm_ltsw_failover_cleanup(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_RATE)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_rate_deinit(unit));
    }

    SHR_IF_ERR_CONT
        (bcmi_ltsw_mirror_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_linkscan_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_cosq_detach(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_rx_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_trunk_detach(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_vlan_detach(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_stg_detach(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_l2_detach(unit));

    if (ltsw_feature(unit, LTSW_FT_IFA)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_ifa_detach(unit));
    }

    SHR_IF_ERR_CONT
        (bcmi_ltsw_stat_detach(unit));

    if (ltsw_feature(unit, LTSW_FT_FLEX_FLOW) ||
        (ltsw_feature(unit, LTSW_FT_MPLS) &&
         !ltsw_feature(unit, LTSW_FT_NO_L2_MPLS))) {
        bcmi_ltsw_virtual_cleanup(unit);
    }

    if (ltsw_feature(unit, LTSW_FT_FLEXDIGEST)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_flexdigest_detach(unit));
    }

    SHR_IF_ERR_CONT
        (bcm_ltsw_flexctr_cleanup(unit));

    SHR_IF_ERR_CONT
        (bcm_ltsw_flexstate_cleanup(unit));

    if (!SAL_BOOT_XGSSIM) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_pktio_cleanup(unit));
    }

    if (ltsw_feature(unit, LTSW_FT_TNL_SEQ_NUM_PRF)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_tunnel_encap_seq_profile_deinit(unit));
    }

#if defined(INCLUDE_INT)
    SHR_IF_ERR_CONT
        (bcm_ltsw_int_detach(unit));
#endif

    SHR_IF_ERR_CONT
        (bcmi_ltsw_dev_misc_cleanup(unit));

    if (ltsw_feature(unit, LTSW_FT_SBR)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_sbr_detach(unit));
    }
    SHR_IF_ERR_CONT
        (bcmi_ltsw_dlb_deinit(unit));

    if (ltsw_feature(unit, LTSW_FT_HASH_OUTPUT_SELECTION_PROFILE)) {
        SHR_IF_ERR_CONT
            (bcmi_ltsw_hos_deinit(unit));
    }

    SHR_IF_ERR_CONT
        (bcmi_ltsw_lb_hash_deinit(unit));

    bcmi_ltsw_idx_tbl_mgmt_deinit(unit);

    bcmi_ltsw_profile_cleanup(unit);

    bcmi_ltsw_feature_detach(unit);

    SHR_IF_ERR_CONT
        (bcmi_ltsw_switch_detach(unit));

    if (ltsw_feature(unit, LTSW_FT_TIME)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_time_deinit(unit));
    }

    SHR_IF_ERR_CONT
        (bcm_ltsw_flowtracker_detach(unit));

#if defined(INCLUDE_BFD)
    SHR_IF_ERR_CONT
        (bcm_ltsw_bfd_detach(unit));
#endif

    SHR_IF_ERR_CONT
        (bcmi_ltsw_uft_detach(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_policer_deinit(unit));

    SHR_IF_ERR_CONT
        (mbcm_ltsw_deinit(unit));

    SHR_IF_ERR_CONT
        (bcmi_ltsw_ser_detach(unit));

    SHR_IF_ERR_CONT
        (bcmi_event_mgr_deinit(unit));

#if defined(INCLUDE_XFLOW_MACSEC)
    if (ltsw_feature(unit, LTSW_FT_XFLOW_MACSEC)) {
        SHR_IF_ERR_CONT
            (bcm_ltsw_xflow_macsec_detach(unit));
    }
#endif

    SHR_FUNC_EXIT();
}

/*!
 * \brief Fetch LTSW device info
 *
 * \param [in]  unit   Unit number
 * \param [out] info   Device info structure
 *
 * \retval SHR_E_NONE  No errors.
 */
int
bcm_ltsw_info_get(int unit, bcm_info_t *info)
{
    info->vendor = BCM_CONTROL(unit)->chip_vendor;
    info->device = BCM_CONTROL(unit)->chip_device;
    info->revision = BCM_CONTROL(unit)->chip_revision;
    info->capability = 0;
    
    info->capability = BCM_INFO_SWITCH;
    if (ltsw_feature(unit, LTSW_FT_L3)) {
        info->capability |= BCM_INFO_L3;
    }
    if (ltsw_feature(unit, LTSW_FT_IPMC)) {
        info->capability |= BCM_INFO_IPMC;
    }
    info->num_pipes = bcmi_ltsw_dev_max_pipe_num(unit);
    info->num_pp_pipes = bcmi_ltsw_dev_max_pp_pipe_num(unit);
    info->num_buffers = bcmi_ltsw_dev_max_packet_buffer_num(unit);

    return SHR_E_NONE;
}

/*!
 * \brief Indicate if the device is in bcm init stage or not.
 *
 * \param [in] unit Unit number
 *
 * \retval In bcm init stage or not.
 */
int bcmi_ltsw_init_state_get(int unit)
{
    return ltsw_init_flag[unit];
}
