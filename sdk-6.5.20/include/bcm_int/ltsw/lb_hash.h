/*! \file lb_hash.h
 *
 * LB hash header file.
 * This file contains LB hash definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMI_LTSW_LB_HASH_H
#define BCMI_LTSW_LB_HASH_H

#include <bcm/types.h>
#include <bcm/field.h>
#include <bcm/switch.h>

/*!
 * \brief The Hash Output Selection Resolution type.
 */
typedef enum bcmi_ltsw_lb_hash_os_s {

    /*! For DLB ECMP packet resolution. */
    bcmiLbHosPortTypeDlbEcmp = 0,

    /*! For ECMP packet resolution. */
    bcmiLbHosPortTypeEcmp = 1,

    /*! For LBID or Entropy label resolution. */
    bcmiLbHosPortTypeEntropy = 2,

    /*! For unicast trunk resolution. */
    bcmiLbHosPortTypeUcTrunk = 3,

    /*! For non unicast trunk resolution. */
    bcmiLbHosPortTypeNonUcTrunk = 4,

    /*! For MPLS ECMP resolution. */
    bcmiLbHosPortTypeMplsEcmp = 5,

    /*! For Port LAG Failover resolution. */
    bcmiLbHosPortTypePlfs = 6,

    /*! For resilient ECMP resolution. */
    bcmiLbHosPortTypeRhEcmp = 7,

    /*! For Weighted ECMP resolution. */
    bcmiLbHosPortTypeWEcmp = 8,

    /*! For overlay ECMP Resilient hashing. */
    bcmiLbHosPortTypeOverlayRhEcmp = 9,

    /*! For MPLS overlay ECMP hashing. */
    bcmiLbHosPortTypeMplsOverlayEcmp = 10,

    /*! For overlay ECMP hashing. */
    bcmiLbHosPortTypeOverlayEcmp = 11,

    /*! For overlay weighted ECMP hashing. */
    bcmiLbHosPortTypeOverlayWEcmp = 12,

    /*! For L2 ECMP hashing. */
    bcmiLbHosPortTypeL2Ecmp = 13,

    /*! For overlay L2 ECMP hashing. */
    bcmiLbHosPortTypeL2OverlayEcmp = 14,

    /*! For DLB ECMP packet resolution. */
    bcmiLbHosFlowTypeDlbEcmp = 15,

    /*! For ECMP packet resolution. */
    bcmiLbHosFlowTypeEcmp = 16,

    /*! For LBID or Entropy label resolution. */
    bcmiLbHosFlowTypeEntropy = 17,

    /*! For trunk resolution. */
    bcmiLbHosFlowTypeTrunk = 18,

    /*! For Weighted ECMP packet resolution. */
    bcmiLbHosFlowTypeWECMP = 19,

    /*! For uc trunk resolution. */
    bcmiLbHosFlowTypeUcTrunk = 20,

    /*! For non uc trunk resolution. */
    bcmiLbHosFlowTypeNonUcTrunk = 21,

    /*! For resilient ECMP resolution. */
    bcmiLbHosFlowTypeRhEcmp = 22,

    /*! For resilient overlay ECMP resolution. */
    bcmiLbHosFlowTypeOverlayRhEcmp = 23,

    /*! For MPLS ECMP resolution. */
    bcmiLbHosFlowTypeMplsEcmp = 24,

    /*! For MPLS overlay ECMP resolution. */
    bcmiLbHosFlowTypeMplsOverlayEcmp = 25,

    /*! For LAG Failover resolution. */
    bcmiLbHosFlowTypePlfs = 26,

    /*! For L2 ECMP hashing. */
    bcmiLbHosFlowTypeL2Ecmp = 27,

    /*! For overlay L2 ECMP hashing. */
    bcmiLbHosFlowTypeL2OverlayEcmp = 28,

    /*! For Overlay Weighted ECMP packet resolution. */
    bcmiLbHosFlowTypeOverlayWECMP = 29,

    /*! For Overlay ECMP packet resolution. */
    bcmiLbHosFlowTypeOverlayECMP = 30,

    /*! Flow HOS resolution type count. */
    bcmiLbHosTypeCount

} bcmi_ltsw_lb_hash_os_t;

/*!
 * \brief The Flow Based Hash Output Selection Field type.
 */
typedef enum bcmi_ltsw_lb_hash_flow_field_s {

    /*! Min field. */
    bcmiLbHosFlowScMin = 0,

    /*! Max field. */
    bcmiLbHosFlowScMax = 1,

    /*! Stride field. */
    bcmiLbHosFlowScStride = 2,

    /*! Concat field. */
    bcmiLbHosFlowScConcat = 3,

    /*! Field count. */
    bcmiLbHosFlowFieldCount

} bcmi_ltsw_lb_hash_flow_field_t;


/*!
 * \brief The hash fields select type.
 */
typedef enum bcmi_ltsw_lb_hash_fields_select_s {

    /*! Hash 0 IPV4 fields. */
    bcmiHash0FieldSelectTypeIP = 0,

    /*! Hash 0 V4 TCP/UDP fields. */
    bcmiHash0FieldSelectTypeTcpUdp = 1,

    /*! Hash 0 V4 L4 source port and destination port fields. */
    bcmiHash0FieldSelectTypeL4Port = 2,

    /*! Hash 0 IPV6 fields. */
    bcmiHash0FieldSelectTypeIP6 = 3,

    /*! Hash 0 V6 TCP/UDP fields. */
    bcmiHash0FieldSelectTypeTcpUdpIp6 = 4,

    /*! Hash 0 V6 L4 source port and destination port fields. */
    bcmiHash0FieldSelectTypeL4PortIp6 = 5,

    /*! Hash 0 L2 fields. */
    bcmiHash0FieldSelectTypeL2 = 6,

    /*! Hash 0 L3 MPLS IP payload fields. */
    bcmiHash0FieldSelectTypeL3Mpls = 7,

    /*! Hash 0 MPLS transit fields. */
    bcmiHash0FieldSelectTypeMplsTransit = 8,

    /*! Hash 1 IPV4 fields. */
    bcmiHash1FieldSelectTypeIP = 9,

    /*! Hash 1 V4 TCP/UDP fields. */
    bcmiHash1FieldSelectTypeTcpUdp = 10,

    /*! Hash 1 V4 L4 source port and destination port fields. */
    bcmiHash1FieldSelectTypeL4Port = 11,

    /*! Hash 1 IPV6 fields. */
    bcmiHash1FieldSelectTypeIP6 = 12,

    /*! Hash 1 V6 TCP/UDP fields. */
    bcmiHash1FieldSelectTypeTcpUdpIp6 = 13,

    /*! Hash 1 V6 L4 source port and destination port fields. */
    bcmiHash1FieldSelectTypeL4PortIp6 = 14,

    /*! Hash 1 L2 fields. */
    bcmiHash1FieldSelectTypeL2 = 15,

    /*! Hash 1 L3 MPLS IP payload fields. */
    bcmiHash1FieldSelectTypeL3Mpls = 16,

    /*! Hash 1 MPLS transit fields. */
    bcmiHash1FieldSelectTypeMplsTransit = 17,

    /*! Hash fields select type count. */
    bcmiFieldSelectTypeCount

} bcmi_ltsw_lb_hash_fields_select_t;

/*!
 * \brief The hash bins assignment type.
 */
typedef enum bcmi_ltsw_lb_hash_bin_s {

    /* Enable pre-processing for enhanced hash mode 0. */
    bcmiHashBinField0PreProcessEnable = 0,

    /* Enable pre-processing for enhanced hash mode 1. */
    bcmiHashBinField1PreProcessEnable = 1,

    /* Selections for IPV6 packets. */
    bcmiHashBinField0Ip6FlowLabel = 2,

    /* Selections for IPV6 packets. */
    bcmiHashBinField1Ip6FlowLabel = 3,

    /*! Hash 0 V6 TCP/UDP fields. */
    bcmiHashBinUdfHashEnable = 4,

    /*! Enable using the Tunnel endpoint id for hashing, for GTP pkts. */
    bcmiHashBinGtpTeidEnable0 = 5,

    /*! Enable using the Tunnel endpoint id for hashing, for GTP pkts. */
    bcmiHashBinGtpTeidEnable1 = 6,

    /*! Hash fields select type count. */
    bcmiHashBinCount

} bcmi_ltsw_lb_hash_bin_t;

/*!
 * \brief The hash packet header selection.
 */
typedef enum bcmi_ltsw_lb_hash_phs_s {

    /*! Packet header field selection control. */
    bcmiPktHdrSelControl = 0,

    /*!
         * If there are more than 3 labels,
         * label stack (up to 5 labels) is used
         *  for hashing else Top 3 MPLS labels +
         *  payload IPV4/IPv6 fields will be
         *  selected for hashing .
         */
    bcmiPktHdrSelMplsUseLabelStack = 1,

    /*! Hash Control to select the IPv6 Next Header field for hash Block A. */
    bcmiPktHdrSelIp6ExtensionHeader = 2,

    /*! To exclude all reserved labels from hash computation */
    bcmiPktHdrSelplsExcludeReservedLabel = 3,

    /*! Hash fields select type count. */
    bcmiPktHdrSelCount

} bcmi_ltsw_lb_hash_phs_t;

/*!
 * \brief The hash ip6 collapse selection.
 */
typedef enum bcmi_ltsw_lb_hash_ip6cs_s {

    /*! Hash Control to collapse IPV6 addresses for hash block A. */
    bcmiIp6CollapseField0 = 0,

    /*! Hash Control to collapse IPV6 addresses for hash block B. */
    bcmiIp6CollapseField1 = 1,


    /*! Hash Control to collapse IPV6 addresses count. */
    bcmiIp6CollapseFieldCount

} bcmi_ltsw_lb_hash_ip6cs_t;

/*!
 * \brief The hash ip6 collapse selection.
 */
typedef enum bcmi_ltsw_lb_hash_algorithm_s {

    /*! Algorithm to be used for Hash 0 Instance 0 computation. */
    bcmiAlgorithmField0Inst0 = 0,

    /*! Algorithm to be used for Hash 0 Instance 1 computation. */
    bcmiAlgorithmField0Inst1 = 1,

    /*! Algorithm to be used for Hash 1 Instance 0 computation. */
    bcmiAlgorithmField1Inst0 = 2,

    /*! Algorithm to be used for Hash 1 Instance 1 computation. */
    bcmiAlgorithmField1Inst1 = 3,

    /*! Algorithm used for generation of RTAG7 Hash Macro Flow Identifier. */
    bcmiAlgorithmMacroFlow = 4,

    /*! Hash Algorithm count. */
    bcmiAlgorithmCount

} bcmi_ltsw_lb_hash_algorithm_t;

/*!
 * \brief Application switch control type of L3 for a specific device.
 */
typedef enum bcmi_ltsw_hash_control_s {

    /*! Non-IP packets w/DA of my router MAC to CPU. */
    bcmiHashControlEcmp = 0,

    /*! DIP not found in L3/LPM tables. */
    bcmiHashControlEntroyLabelFlowBased = 1,

    /*! Set Lookup status for Flex Hash fields selection */
    bcmiHashControlFlexHashLookupStatus = 2,

    /*! Hash Control count. */
    bcmiHashControlCount
} bcmi_ltsw_hash_control_t;

/*!
 * \brief Application switch control type of L3 for a specific device.
 */
typedef enum bcmi_ltsw_hash_versatile_control_s {

    /*! Initial value 0 for both versatile hash block A and B. */
    bcmiHashVersatileControlInitValue0 = 0,

    /*! Initial value 1 for both versatile hash block A and B. */
    bcmiHashVersatileControlInitValue1 = 1,

    /*! Initial value 0 for versatile hash block A. */
    bcmiHashVersatileControlBlockAInitValue0 = 2,

    /*! Initial value 1 for versatile hash block A. */
    bcmiHashVersatileControlBlockAInitValue1 = 3,

    /*! Initial value 0 for versatile hash block B. */
    bcmiHashVersatileControlBlockBInitValue0 = 4,

    /*! Initial value 1 for versatile hash block B. */
    bcmiHashVersatileControlBlockBInitValue1 = 5,

    /*! Hash Versatile Control count. */
    bcmiHashVersatileControlCount
} bcmi_ltsw_hash_versatile_control_t;

/*!
 * \brief Application switch control type of lb hash seed for a specific device.
 */
typedef enum bcmi_ltsw_hash_control_seed_s {

    /*! Hash seed input for hash 0. */
    bcmiHashControlSeed0 = 0,

    /*! Hash seed input for hash 1. */
    bcmiHashControlSeed1 = 1,

    /*! Hash seed input count. */
    bcmiHashControlSeedCount
} bcmi_ltsw_hash_control_seed_t;

/*!
 * \brief Macro flow type.
 */
typedef enum bcmi_ltsw_macro_flow_s {

    /*! ECMP macro flow. */
    bcmiMacroFlowEcmp = 0,

    /*! Overlay ECMP macro flow. */
    bcmiMacroFlowEcmpOverlay = 1,

    /*! Load balance macro  flow. */
    bcmiMacroFlowLoadBalance = 2,

    /*! Underlay ECMP macro flow. */
    bcmiMacroFlowECMPUnderlay = 3,

    /*! Trunk macro flow. */
    bcmiMacroFlowTrunk = 4,

    /*! Macro flow count. */
    bcmiMacroFlowCount
} bcmi_ltsw_macro_flow_t;

/*!
 * \brief De-init the LB hash module.
 *
 * \param [int] unit Unit Number.
 */
extern int
bcmi_ltsw_lb_hash_deinit(int unit);

/*!
 * \brief Initialize the LB hash module.
 *
 * \param [int] unit Unit Number.
 */
extern int
bcmi_ltsw_lb_hash_init(int unit);

/*!
 * \brief Set the port based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The port based hash output selection type.
 * \param [in] port Port Number.
 * \param [in] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value);

/*!
 * \brief Get the port based hash output selection configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The port based hash output selection type.
 * \param [in] port Port Number.
 * \param [out] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value);

/*!
 * \brief Set the macro flow based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The flow based hash output selection type.
 * \param [in] field The flow based hash output field.
 * \param [in] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value);

/*!
 * \brief Get the macro flow based hash output selection configuration.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The flow based hash output selection type.
 * \param [in] field The flow based hash output field.
 * \param [out] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value);

/*!
 * \brief Set the fields selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The hash fields selection type.
 * \param [in] value The hash fields selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value);

/*!
 * \brief Get the fields selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The hash fields selection type.
 * \param [out] value The hash fields selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value);

/*!
 * \brief Set the hash bin assignment.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash bin control.
 * \param [in] value The hash bin control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value);

/*!
 * \brief Get the hash bin assignment.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash bin control.
 * \param [out] value The hash bin control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value);

/*!
 * \brief Set the hash packet header selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash packet header selection.
 * \param [in] value The hash packet header selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value);

/*!
 * \brief Get the hash packet header selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash packet header selection.
 * \param [out] value The hash packet header selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value);

/*!
 * \brief Set the hash ip6 collapse selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash ip6 collapse selection.
 * \param [in] value The hash ip6 collapse selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value);

/*!
 * \brief Get the hash ip6 collapse selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash ip6 collapse selection.
 * \param [out] value The hash ip6 collapse selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value);

/*!
 * \brief Set the hash algorithm.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash algorithm control.
 * \param [in] value The hash algorithm control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value);

/*!
 * \brief Get the hash algorithm.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash algorithm control.
 * \param [out] value The hash algorithm control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value);

/*!
 * \brief Set the symmetric hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] value The symmetric hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value);

/*!
 * \brief Get the symmetric hash control.
 *
 * \param [in] unit Unit Number.
 * \param [out] value The symmetric hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value);

/*!
 * \brief Set the hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Hash contro type.
 * \param [in] value The hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value);

/*!
 * \brief Get the hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Hash contro type.
 * \param [out] value The hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value);

/*!
 * \brief Enable/Disable assignment of hash bins
 *        for flex hash.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable Flex Hash
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_lb_hash_bins_flex_set(
    int unit,
    int enable);

/*!
 * \brief Create flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] qset Field Qualifier set.
 * \param [out] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_create(
    int unit,
    bcm_field_qset_t qset,
    bcm_hash_entry_t *entry);

/*!
 * \brief Destroy flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_destroy(
    int unit,
    bcm_hash_entry_t entry);

/*!
 * \brief Install flex hash entry to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] offset L4 offset.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset);

/*!
 * \brief Reinstall flex hash entry to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] offset L4 offset.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_reinstall(
    int unit,
    bcm_hash_entry_t entry,
    uint32 offset);

/*!
 * \brief Remove flex hash entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry);

/*!
 * \brief Set flex hash entry.
 *
 * This sets up Hash Bin extraction configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] config Hash bin configurations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config);

/*!
 * \brief Get flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [out] config Hash bin configurations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config);

/*!
 * \brief Traverse flex hash entries.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags.
 * \param [in] cb_fn Callback function pointer.
 * \param [in] user_data cookie
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data);

/*!
 * \brief Qualify UDF data to flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] udf_id UDF object Identifier.
 * \param [in] length Number of bytes in 'data'.
 * \param [in] data Data to match UDF extraction.
 * \param [in] mask Mask to be applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask);

/*!
 * \brief Get UDF data qualified to flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] udf_id UDF object Identifier.
 * \param [in] max_length Number of bytes in 'data'.
 * \param [out] data Data.
 * \param [out] mask.
 * \param [out] actual_length Number of bytes valid in 'data'.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length);

/*!
 * \brief Set lb hash versatile control.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [in] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_versatile_control_set(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int value);

/*!
 * \brief Get lb hash versatile control.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [out] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_versatile_control_get(
    int unit,
    bcmi_ltsw_hash_versatile_control_t control,
    int *value);

/*!
 * \brief Set lb hash control seed.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [in] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value);

/*!
 * \brief Get lb hash control seed.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [out] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value);

/*!
 * \brief Set macro flow hash seed.
 *
 * \param [in] unit Unit number.
 * \param [in] type Macro flow type.
 * \param [int] value Seed value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value);

#endif /* BCMI_LTSW_LB_HASH_H */
