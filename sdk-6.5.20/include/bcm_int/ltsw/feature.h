/*! \file feature.h
 *
 * LTSW feature header file.
 * This file contains the definitions of feature.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_FEATURE_H
#define BCMI_LTSW_FEATURE_H

#include <sal/sal_types.h>

/*!
 * \brief Device features which are shared across all device families.
 *
 * This data structure is used to define enums of the form ltsw_feature_t,
 * and a corresponding list of string name. New additions should be made
 * in the following section between the zero and count entries.
 */
#define LTSW_FEAT_ENUM_BEGIN(etype)  typedef enum etype##e {
#define LTSW_FEAT_ENUM_END(etype)    } etype##t
#define LTSW_FEAT_ENTRY(e)           LTSW_FEAT_MAKE_STR(e)
#define LTSW_FEAT_MAKE_ENUM(e)       \
        LTSW_FEAT_ENUM_BEGIN(e)      \
        LTSW_FEAT_ENTRIES(dont_care) \
        LTSW_FEAT_ENUM_END(e)

#define LTSW_FEAT_ENTRIES(DONT_CARE)                                           \
LTSW_FEAT_ENTRY(ZERO),              /*! ALWAYS FIRST PLEASE (DO NOT CHANGE) */ \
LTSW_FEAT_ENTRY(L3),                /*! L3 feature. */                         \
LTSW_FEAT_ENTRY(L3_FIB),            /*! L3 FIB lookup feature. */              \
LTSW_FEAT_ENTRY(MPLS),              /*! MPLS feature. */                       \
LTSW_FEAT_ENTRY(MPLS_VLAN_DOMAIN),  /*! Use vlan as MPLS forwarding domain. */ \
LTSW_FEAT_ENTRY(MPLS_SPECIAL_LABEL),/*! Special label control. */ \
LTSW_FEAT_ENTRY(RATE),              /*! Rate feature. */                       \
LTSW_FEAT_ENTRY(FLEX_FLOW),         /*! Flex flow feature. */                  \
LTSW_FEAT_ENTRY(FLEXCTR_GEN2),      /*! Flex counter gen2.0 feature. */        \
LTSW_FEAT_ENTRY(ASY_DUAL_MODID),    /*! Asymmetric dual module ID support. */   \
LTSW_FEAT_ENTRY(DLB),               /*! Dynamic Load Balance feature. */        \
LTSW_FEAT_ENTRY(DLB_DGM),           /*! Dynamic Group Multipath feature. */        \
LTSW_FEAT_ENTRY(IPMC),              /*! IP multicast routing feature. */       \
LTSW_FEAT_ENTRY(PIM_BIDIR),         /*! Bidirectional PIM feature. */          \
LTSW_FEAT_ENTRY(NAT),               /*! Network Adress Translation feature. */ \
LTSW_FEAT_ENTRY(L2_USER_ENTRY),     /*! L2 Caching of BPDU MAC addresses feature. */ \
LTSW_FEAT_ENTRY(OOB_FC),            /*! Out-of-band Flow Control feature.  */ \
LTSW_FEAT_ENTRY(FLEXDIGEST),        /*! Flex Digest.  */ \
LTSW_FEAT_ENTRY(SBR),               /*! SBR feature. */ \
LTSW_FEAT_ENTRY(STK),               /*! Stack feature. */ \
LTSW_FEAT_ENTRY(TIME),              /*! Time and SyncE feature. */ \
LTSW_FEAT_ENTRY(URPF),              /*! URPF.  */ \
LTSW_FEAT_ENTRY(HASH_OUTPUT_SELECTION_PROFILE), /*! Hash ouput selection profile.  */ \
LTSW_FEAT_ENTRY(TELEMETRY),         /*! Telemetry feature.  */ \
LTSW_FEAT_ENTRY(L3_EGRESS_DEFAULT_UNDERLAY), /*! Default layer for single level L3 egress objects.  */ \
LTSW_FEAT_ENTRY(VPLAG),             /*! Virtual Port Link Aggregation Group feature.  */ \
LTSW_FEAT_ENTRY(TRACE_DOP),         /*! Dop based packet trace.  */ \
LTSW_FEAT_ENTRY(FLEXSTATE),         /*! Flex state feature. */        \
LTSW_FEAT_ENTRY(TNL_SEQ_NUM_PRF),   /*! Tunnel sequence number profile. */        \
LTSW_FEAT_ENTRY(NO_HOST),           /*! No host table. */ \
LTSW_FEAT_ENTRY(NO_L2_MPLS),        /*! Not support VPLS and VPWS. */ \
LTSW_FEAT_ENTRY(MPLS_CONTROL_PKT),  /*! Support MPLS control packet policy. */ \
LTSW_FEAT_ENTRY(L2_TUNNEL_SINGLE_POINTER), /*! Feature WAR for specific handling in L2 tunnel single-pointer model. */ \
LTSW_FEAT_ENTRY(ECN_WRED),          /*! ECN WRED feature. */       \
LTSW_FEAT_ENTRY(FLEX_FLOW_VLAN_DOMAIN), /*! Use vlan as FLEX FLOW forwarding domain. */ \
LTSW_FEAT_ENTRY(L3_HIER),           /*! Hierarchical L3 feature. */       \
LTSW_FEAT_ENTRY(LB_HASH),           /*! Load balance hash. */       \
LTSW_FEAT_ENTRY(FLEXCTR_L2_HITBIT),   /*! Flexctr based L2 hit bit. */       \
LTSW_FEAT_ENTRY(FLEXCTR_IPMC_HITBIT), /*! Flexctr based IPMC hit bit. */       \
LTSW_FEAT_ENTRY(LDH),               /*! Latency distributuib histogram (latency monitor) */ \
LTSW_FEAT_ENTRY(MPLS_FLEXCTR_ACTION_PROFILE), /*! MPLS flex counter action profile support. */       \
LTSW_FEAT_ENTRY(IFA),               /*! Inband Flow Analyzer. */       \
LTSW_FEAT_ENTRY(L3_AACL),           /*! L3 AACL feature. */ \
LTSW_FEAT_ENTRY(FLEXCTR_EVICTION),  /*! Flex counter eviction. */       \
LTSW_FEAT_ENTRY(FLEXCTR_QUANT_32BIT), /*! Flex counter quantization 32-bit mode support. */ \
LTSW_FEAT_ENTRY(INT_IFA),           /*! Inband Flow Analyzer based on  INT module. */       \
LTSW_FEAT_ENTRY(INT_IOAM),          /*! In-site Operations, Administration, and Maintenance based on INT module. */       \
LTSW_FEAT_ENTRY(INT_DP),            /*! In-band Network Telemetry Data-plane Probe based on  INT module. */       \
LTSW_FEAT_ENTRY(VFI),               /*! VFI feature. */       \
LTSW_FEAT_ENTRY(L3_FIB_SINGLE_POINTER), /*! L3 forward with single-pointer model. */ \
LTSW_FEAT_ENTRY(TNL_TERM_FLEXCTR),  /*! Tunnel terminator flex counter. */ \
LTSW_FEAT_ENTRY(XFLOW_MACSEC), /*! XFLOW_MACSEC feature. */ \
LTSW_FEAT_ENTRY(NO_LPM_ROUTE), /*! No LPM route. */ \
LTSW_FEAT_ENTRY(PAYLOAD_TPID),      /*! PAYLOAD TPID. */ \
LTSW_FEAT_ENTRY(DEST_SYSPORT_TO_UL_NH), /*! Derive underlay nexthop from dest system port. */ \
LTSW_FEAT_ENTRY(NO_ROUTE_DATA_TYPE),      /*! No route data type. */ \
LTSW_FEAT_ENTRY(MMU_MOD_MC_COS_ONLY),      /*! MMU MOD enqueued into only multicast queue. */ \
LTSW_FEAT_ENTRY(COUNT)              /*! ALWAYS LAST PLEASE (DO NOT CHANGE)  */

/*! Make the enums for device features */
#undef  LTSW_FEAT_MAKE_STR
#define LTSW_FEAT_MAKE_STR_CONCAT(e, a)  e##a
#define LTSW_FEAT_MAKE_STR(a) LTSW_FEAT_MAKE_STR_CONCAT(LTSW_FT_, a)

LTSW_FEAT_MAKE_ENUM(ltsw_feature_);

#undef  LTSW_FEAT_MAKE_STR
#define LTSW_FEAT_MAKE_STR(a)        #a
#define LTSW_FEAT_NAME_BEGIN(etype)  {
#define LTSW_FEAT_NAME_END(etype)    }

#define LTSW_FEAT_NAME_LIST             \
        LTSW_FEAT_NAME_BEGIN(dont_care) \
        LTSW_FEAT_ENTRIES(dont_care)    \
        LTSW_FEAT_NAME_END(dont_care)

/*!
 * \brief Initialize features to be supported.
 *
 * \param [in] unit     Unit number.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval !SHR_E_NONE  Failure.
 */
extern int
bcmi_ltsw_feature_init(int unit);

/*!
 * \brief De-initialize the supported features.
 *
 * \param [in]  unit               Unit Number.
 *
 * \retval None.
 */
extern void
bcmi_ltsw_feature_detach(int unit);

/*!
 * \brief Enable the specific feature.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit.
 */
extern void
ltsw_feature_enable(int unit, ltsw_feature_t feature);

/*!
 * \brief Check if the feature is enabled.
 *
 * \param [in] unit Unit number.
 *
 * \retval TRUE  Feature is enabled.
 * \retval FALSE Feature is disabled.
 */
extern bool
ltsw_feature(int unit, ltsw_feature_t feature);

#endif  /* BCMI_LTSW_FEATURE_H */
