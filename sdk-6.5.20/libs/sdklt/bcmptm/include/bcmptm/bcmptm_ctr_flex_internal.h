/*! \file bcmptm_ctr_flex_internal.h
 *
 * Interface for CTR_FLEX driver APIs defined in ctr_flex_common.c
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CTR_FLEX_INTERNAL_H
#define BCMPTM_CTR_FLEX_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <bcmdrd/bcmdrd_types.h>
#include <sal/sal_types.h>
#include <sal/sal_time.h>

/*!
 * \brief packet_resolution values
 */
typedef struct ing_pkt_types_s {
    uint8_t    unknown_pkt;
    uint8_t    control_pkt;
    uint8_t    oam_pkt;
    uint8_t    bfd_pkt;
    uint8_t    bpdu_pkt;
    uint8_t    icnm_pkt;
    uint8_t    n1588_pkt;
    uint8_t    known_l2uc_pkt;
    uint8_t    unknown_l2uc_pkt;
    uint8_t    known_l2mc_pkt;
    uint8_t    unknown_l2mc_pkt;
    uint8_t    l2bc_pkt;
    uint8_t    known_l3uc_pkt;
    uint8_t    unknown_l3uc_pkt;
    uint8_t    known_ipmc_pkt;
    uint8_t    unknown_ipmc_pkt;
    uint8_t    known_mpls_l2_pkt;
    uint8_t    unknown_mpls_pkt;
    uint8_t    known_mpls_l3_pkt;
    uint8_t    known_mpls_pkt;
    uint8_t    known_mpls_mc_pkt;
    uint8_t    known_mim_pkt;
    uint8_t    unknown_mim_pkt;
    uint8_t    known_trill_pkt;
    uint8_t    unknown_trill_pkt;
    uint8_t    known_niv_pkt;
    uint8_t    unknown_niv_pkt;
    uint8_t    known_l2gre_pkt;
    uint8_t    known_vxlan_pkt;
    uint8_t    known_fcoe_pkt;
    uint8_t    unknown_fcoe_pkt;
} ing_pkt_types_t;

typedef struct egr_pkt_types_s {
    uint8_t    multicast_pkt;
    uint8_t    unicast_pkt;
} egr_pkt_types_t;

/*!\brief Offset Table fields */
typedef struct offset_table_fields_s {
    bcmdrd_fid_t count_enable;
    bcmdrd_fid_t offset;
} offset_table_fields_t;

/*!\brief Packet Selector Table fields */
typedef struct pkt_key_selector_udf_fields_s {
    bcmdrd_fid_t user_specified_udf_valid;
    bcmdrd_fid_t use_udf_key;
    bcmdrd_fid_t use_compressed_pkt_key;
} pkt_key_selector_udf_fields_t;

/*!
 * \brief Flex packet attribute bits
 */
typedef struct flex_pkt_attribute_bits_s {

    uint8_t    ip_bits;
    uint8_t    ip_pos;

    uint8_t    drop_bits;
    uint8_t    drop_pos;

    uint8_t    svp_bits;
    uint8_t    svp_pos;

    uint8_t    dvp_bits;
    uint8_t    dvp_pos;

    uint8_t    pkt_type_bits;
    uint8_t    pkt_type_pos;

    uint8_t    tos_ecn_bits;
    uint8_t    tos_ecn_pos;

    uint8_t    tos_dscp_bits;
    uint8_t    tos_dscp_pos;

    uint8_t    port_bits;
    uint8_t    port_pos;

    uint8_t    int_cn_bits;
    uint8_t    int_cn_pos;

    uint8_t    inner_dot1p_bits;
    uint8_t    inner_dot1p_pos;

    uint8_t    outer_dot1p_bits;
    uint8_t    outer_dot1p_pos;

    uint8_t    vlan_bits;
    uint8_t    vlan_pos;

    uint8_t    int_pri_bits;
    uint8_t    int_pri_pos;

    uint8_t    fp_color_bits;
    uint8_t    fp_color_pos;

    uint8_t    pre_fp_color_bits;
    uint8_t    pre_fp_color_pos;

    uint8_t    elephant_pkt_bits;
    uint8_t    elephant_pkt_pos;

    uint8_t    tcp_flag_bits;
    uint8_t    tcp_flag_pos;

    uint8_t    congestion_bits;
    uint8_t    congestion_pos;

    uint8_t    unicast_queueing_bits;
    uint8_t    unicast_queueing_pos;

    uint8_t    mmu_queue_bits;
    uint8_t    mmu_queue_pos;

} flex_pkt_attribute_bits_t;

/*!
 * \brief Flex counter device specific information
 */
typedef struct ctr_flex_device_info_s {
    /*! Number of ingress pools */
    size_t num_ingress_pools;

    /*! Number of egress pools */
    size_t num_egress_pools;

    /*! Start of pools reserved for FP */
    size_t fp_pool_start;

    /*! Number of pipes */
    size_t num_pipes;

    /*! Ingress packet attribute bits */
    flex_pkt_attribute_bits_t *ing_pkt_attr_bits;

    /*! Egress packet attribute bits */
    flex_pkt_attribute_bits_t *egr_pkt_attr_bits;

    /*! Ingress packet resolution types */
    ing_pkt_types_t *ing_pkt_types;

    /*! Egress packet resolution types */
    egr_pkt_types_t *egr_pkt_types;
} ctr_flex_device_info_t;

/*!\brief Virtual functions for ctr_flex routines. */

/*!
 * \brief Get Pkt selector registers and field list.
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [out] selector_list  List of key selectors
 * \param [out] num_selectors  Number of key selectors
 * \param [out] for_bit_field_list  List of FOR_BIT controls in key selector.
 * \param [out] bit_en_field_list  List of EN controls in key selector.
 * \param [out] num_fields Number of bit controls.
 * \param [out] udf_field_list List of UDF controls.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_pkt_selector_get_t) (int unit,
                                  bool ingress,
                                  bcmdrd_sid_t **selector_list,
                                  uint32_t *num_selectors,
                                  bcmdrd_fid_t **for_bit_field_list,
                                  bcmdrd_fid_t **bit_en_field_list,
                                  uint32_t *num_fields,
                                  pkt_key_selector_udf_fields_t **udf_fields);
/*!
 * \brief Get Offset tables and field list.
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [out] offset_table_list  List of offset tables.
 * \param [out] num_selectors  Number of offset tables.
 * \param [out] max_counters_per_mode  Max counters supported per mode.
 * \param [out] offset_table_flds Offset table field list.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_offset_table_get_t) (int unit,
                                  bool ingress,
                                  bcmdrd_sid_t **offset_table_list,
                                  uint32_t *num,
                                  uint32_t *max_counters_per_mode,
                                  uint32_t *entry_wsize,
                                  offset_table_fields_t **offset_table_flds);
/*!
 * \brief Set the Offset tables entry width
 *
 * \param [in] unit       Unit number.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_offset_table_init_t) (int unit);

/*!
 * \brief Get update controls list and field list.
 *
 * \param [in] unit       Unit number.
 * \param [in] ingress    Ingress or Egress direction.
 * \param [out] selector_list  List of update controls registers.
 * \param [out] num_selectors  Number of update controls.
 * \param [out] counter_pool_enable_fid Counter pool enable field ID.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_update_control_get_t)(int unit,
                                  bool ingress,
                                  bcmdrd_sid_t **update_control_list,
                                  uint32_t *num,
                                  bcmdrd_fid_t *counter_pool_enable_fid);
/*!
 * \brief Get counter tables list and field list.
 *
 * \param [in] unit    Unit number.
 * \param [in] ingress Ingress or Egress direction.
 * \param [out] counter_table_list  List of counter tables.
 * \param [out] num  Number of counter tables.
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_counter_table_get_t)(int unit,
                                  bool ingress,
                                  bcmdrd_sid_t **counter_table_list,
                                  uint32_t *num);
/*!
 * \brief Get ingress packet types
 *
 * \param [in] unit    Unit number.
 * \param [out] ing_pkt_types All ingress packet types
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_ing_pkt_types_get_t)(int unit,
                                  ing_pkt_types_t **ing_pkt_types);
/*!
 * \brief Get egress packet types
 *
 * \param [in] unit    Unit number.
 * \param [out] egr_pkt_types All egress packet types
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_egr_pkt_types_get_t)(int unit,
                                  egr_pkt_types_t **egr_pkt_types);
/*!
 * \brief Get ingress packet attribute bits
 *
 * \param [in] unit    Unit number.
 * \param [out] ing_pkt_attr_bits Packet attributes position and size
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_ing_pkt_attr_bits_get_t)(int unit,
                                  flex_pkt_attribute_bits_t **ing_pkt_attr_bits);
/*!
 * \brief Get egress packet attribute bits
 *
 * \param [in] unit    Unit number.
 * \param [out] egr_pkt_attr_bits Packet attributes position and sizetypes
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_egr_pkt_attr_bits_get_t)(int unit,
                                  flex_pkt_attribute_bits_t **egr_pkt_attr_bits);

/*!
 * \brief Get device info
 *
 * \param [in] unit    Unit number.
 * \param [out] dev_info Pool and pipe info
 *
 * \return SHR_E_NONE Success.
 */
typedef int
(*bcmptm_ctr_flex_dev_info_get_t)(int unit,
                                  ctr_flex_device_info_t **dev_info);

/*!\brief Virtual table for ct_flex data driver. */
typedef struct bcmptm_ctr_flex_data_driver_s {
    bcmptm_ctr_flex_pkt_selector_get_t pkt_selector_get_fn;
    bcmptm_ctr_flex_offset_table_get_t  offset_table_get_fn;
    bcmptm_ctr_flex_offset_table_init_t  offset_table_init_fn;
    bcmptm_ctr_flex_update_control_get_t update_control_get_fn;
    bcmptm_ctr_flex_counter_table_get_t counter_table_get_fn;
    bcmptm_ctr_flex_ing_pkt_types_get_t ing_pkt_types_get_fn;
    bcmptm_ctr_flex_egr_pkt_types_get_t egr_pkt_types_get_fn;
    bcmptm_ctr_flex_ing_pkt_attr_bits_get_t ing_pkt_attr_bits_get_fn;
    bcmptm_ctr_flex_egr_pkt_attr_bits_get_t egr_pkt_attr_bits_get_fn;
    bcmptm_ctr_flex_dev_info_get_t dev_info_get_fn;
} bcmptm_ctr_flex_data_driver_t;

/*!
* \brief Get data driver from chip specific code.
*
* \param [in] unit     Logical device id.
*
* \return Data driver routines.
*/
extern const bcmptm_ctr_flex_data_driver_t *
bcmptm_ctr_flex_data_driver_register(int unit);

#endif /* BCMPTM_CTR_FLEX_INTERNAL_H */
