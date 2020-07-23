/*! \file bcmfp_tbl_internal.h
 *
 * BCMFP Logical table IDs and field IDs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_TBL_INTERNAL_H
#define BCMFP_TBL_INTERNAL_H

#include <bcmltd/bcmltd_types.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/generated/bcmfp_ha.h>
#include <bcmfp/bcmfp_qual_internal.h>

#define BCMFP_MAX_IPBM_PARTS 4
#define BCMFP_MAX_IPBM_PIPES 4

/*! Structure to map the offset and width of qualfier fid. */
typedef struct bcmfp_tbl_fid_remote_map_s {

    /*! Remote LT FID. */
    bcmltd_fid_t fid;

    /*! Offset in remote LT FID. */
    uint16_t offset;

    /*! Width in remote LT FID. */
    uint16_t width;

} bcmfp_tbl_fid_remote_map_t;

/*! Structure to map fields in one LT to fields in other LT. */
typedef struct bcmfp_tbl_fid_remote_maps_s {

    /*! Source LT fid. */
    bcmltd_fid_t fid;

    /*! Number of source LT FID to remote LT fid */
    uint16_t num_remote_map;

    /*! Source LT FID to remote LT FID maps */
    bcmfp_tbl_fid_remote_map_t const *remote_map;

} bcmfp_tbl_fid_remote_maps_t;

/*! Structure to remote FID maps of an LT. */
typedef struct bcmfp_tbl_fid_remote_info_s {

    /*! Source LTID. */
    bcmltd_sid_t sid;

    /*! Pointer to Source LTID fids to remote LT fids. */
    bcmfp_tbl_fid_remote_maps_t const *remote_maps;

    /*! Number of elements in remote_maps. */
    uint16_t num_remote_maps;

} bcmfp_tbl_fid_remote_info_t;

/*! Type of logical table field. */
typedef enum bcmfp_tbl_fid_type_e {
    /*! LT fid is part LT key fids. */
    BCMFP_TBL_FID_TYPE_KEY,

    /*! LT fid is part LT mask fids. */
    BCMFP_TBL_FID_TYPE_MASK,

    /*! LT fid is part LT data fids. */
    BCMFP_TBL_FID_TYPE_DATA,

    /*! Constant type(fid is invalid). */
    BCMFP_TBL_FID_TYPE_CONSTANT,

    /*! Invalid fid type. */
    BCMFP_TBL_FID_TYPE_COUNT,
} bcmfp_tbl_fid_type_t;

/*!
 * Field offsets and widths information in the physical
 * entry of an LT
 */
typedef struct bcmfp_tbl_fid_hw_info_s {
   /*! Type of LT field(key/mask/data/constant) */
   bcmfp_tbl_fid_type_t type;

   /*!
    * LT field identifier. In case of constant
    * type field fid is invalid.
    */
   bcmltd_fid_t fid;

   /*! Offset of FID partition. */
   uint16_t offset;

   /*! Width of FID partition. */
   uint16_t width;

   /*!
    * Value of FID partition. This is valid only
    * for BCMFP_TBL_FID_TYPE_CONSTANT type FIDs.
    */
   uint16_t value;

   /*! Offset in hardware entry. */
   uint16_t hw_offset;

} bcmfp_tbl_fid_hw_info_t;

typedef struct bcmfp_tbl_lrd_info_s {
    /*
     * Number of fids that are active in this table
     * for the specific device.This may or may not be
     * equal to fid_count of the table.
     */
    size_t num_fid;
} bcmfp_tbl_lrd_info_t;

typedef struct bcmfp_tbl_qualifier_fid_info_s {
    bcmltd_fid_t fid;

    bcmfp_qualifier_t qualifier;

    bcmfp_qualifier_fid_flags_t flags;

} bcmfp_tbl_qualifier_fid_info_t;

typedef struct bcmfp_tbl_offset_info_s {

     bcmltd_fid_t offset_fid;

     bcmltd_fid_t width_fid;

     bcmltd_fid_t order_fid;

     bcmltd_fid_t count_fid;

} bcmfp_tbl_offset_info_t;

/*
 * Filed identifiers information that are part of any type
 * of compression key.
 */
typedef struct bcmfp_tbl_compress_fid_info_s {
    /*!
     * Unique type to identify different compressions supported
     * in the device.
     */
    uint8_t type;

    /*!
     * BITMAP FIDs contibuting to compression key in the order of
     * MSB to LSB.
     */
    bcmltd_fid_t *bitmap_fids;

    /*!
     * Qualifier key FIDs contibuting to compression key in the
     * order of MSB to LSB. Relavent only for group template LT.
     */
    bcmltd_fid_t *key_fids;

    /*!
     * Qualifier mask FIDs contibuting to compression key in the
     * order of MSB to LSB. Relavent only for group template LT.
     */
    bcmltd_fid_t *mask_fids;

    /*! Start bit in FID data to use to build compression key. */
    uint16_t *fid_offsets;

    /*!
     * Number of bits in FID data, from start bit specified in
     * fid_offsets array, to use to build compression key.
     */
    uint16_t *fid_widths;

    /*! Number of FIDs in fids array. */
    uint8_t num_fids;

    /*! Compress class ID bitmap FID. */
    bcmltd_fid_t cid_bitmap_fid;

    /*! Compress class ID key FID. */
    bcmltd_fid_t cid_key_fid;

    /*! Compress class ID mask FID. */
    bcmltd_fid_t cid_mask_fid;

    /*! Compression ID size. */
    uint8_t cid_size;
} bcmfp_tbl_compress_fid_info_t;

/*! Logical Table details(SID and some FIDs) for group. */
typedef struct bcmfp_tbl_group_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field Identifier for policy Id. */
    uint32_t default_policy_id_fid;

    /*! Field Identifier for policy Id. */
    uint32_t default_pdd_id_fid;

    /*! Field Identifier for default SBR Id. */
    uint32_t default_sbr_id_fid;

    /*! Field Identifier for default policy meter ID */
    uint32_t default_meter_id_fid;

    /*! Field Identifier for default policy flex ctr action index */
    uint32_t default_flex_ctr_action_fid;

    /*! Field Identifier for Pipe. */
    uint32_t pipe_id_fid;

    /*! Field Identifier for Group Priority. */
    uint32_t priority_fid;

    /*! Field Identifier for Group User Configured Mode. */
    uint32_t mode_fid;

    /*!
     * Field Identifier for Group Mode Auto. This indicates
     * if algorithm need to use user given \c mode_fid or
     * use auto mode.
     */
    uint32_t mode_auto_fid;

    /*! Field Identifier for Group operating mode. */
    uint32_t mode_oper_fid;

    /*! Field Identifier for INPORTS qualifier. */
    uint32_t qual_inports_fid;

    /*! Field Identifier for SYSTEM_PORTS qualifier. */
    uint32_t qual_system_ports_fid;

    /*! Field Identifier for DEVICE_PORTS qualifier. */
    uint32_t qual_device_ports_fid;

    /*! Field Identifier for Virtual Slice Group. */
    uint32_t vslice_group_fid;

    /*! Field Identifier for Group's Port Packet Type. */
    uint32_t group_type_fid;

    /*! Field Identifier for Group's slice_mode. */
    uint32_t slice_mode_fid;

    /*!
     * Field Identifier for number of presel IDs assigned
     * to the group.
     */
    uint32_t presel_ids_count_fid;

    /*! Field Identifier for Presel IDs array. */
    uint32_t presel_ids_fid;

    /*!
     * Table Identifier for Presel for which \c presel_ids_fid
     * is given.
     */
    uint32_t presel_sid;

    /*! Field Identifier for Presel Id. */
    uint32_t presel_key_fid;

    /*!
     * Table Identifier for hit index template table.
     */
    uint32_t hit_index_sid;

    /*!
     * Field Identifier for hit index field.
     */
    uint32_t hit_index_fid;

    /*!
     * Field Identifier for mixed source class mask qualifier in
     * \c presel_sid.
     */
    uint32_t presel_mixed_src_class_mask_fid;

    /*!
     * Field Identifier for EM group lookup id.
     */
    uint32_t group_lookup_id_fid;

    /*!
     * Field Identifier for LTID in EM group.
     */
    uint32_t group_ltid_fid;

    /*!
     * Field Identifier for auto LTID in EM group. This determine if
     * the algorithm has to use the auto LTID or user given LTID.
     */
    uint32_t group_ltid_auto_fid;

    /*!
     * Field Identifier for operational LTID in EM group.
     */
    uint32_t group_ltid_oper_fid;

    /*! Field Identifier for PDD. */
    uint32_t pdd_id_fid;

    /*! Field Identifier for SBR. */
    uint32_t sbr_id_fid;

    /*! Field Identifier for presel group. */
    uint32_t psg_id_fid;

    /* Logical Table ID for source class mode. */
    uint32_t src_class_sid;

    /*! Field identifier for Compression types array  */
    uint32_t compression_type_array_fid;

    /*!
     * Field identifier for number of elements compression
     * types array.
     */
    uint32_t num_compression_type_fid;

    /*! Array to map qualifiers LT fields in group table. */
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info;

    /*! HASH_A0 value. */
    uint32_t hash_a0_fid;

    /*! HASH_A1 value. */
    uint32_t hash_a1_fid;

    /*! HASH_B0 value. */
    uint32_t hash_b0_fid;

    /*! HASH_B1 value. */
    uint32_t hash_b1_fid;

    /*! HASH_C0 value. */
    uint32_t hash_c0_fid;

    /*! HASH_C10 value. */
    uint32_t hash_c1_fid;

    /*! Drop data. */
    uint32_t drop_data_fid;

    /*! Drop decision. */
    uint32_t drop_decision_fid;

    /*! Trace vector. */
    uint32_t trace_vector_fid;

    /*! Field identifier to enable/disable group. */
    uint32_t enable_fid;

    /*! Field identifier to enable/disable group auto-expansion. */
    uint32_t auto_expand_fid;

    /*! Field Identifier for key type. */
    uint32_t key_type_fid;

    /*! Field Identifier for group slice type. */
    uint32_t group_slice_type_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_group_t;

/*! Logical Table details(SID and some FIDs) for rule. */
typedef struct bcmfp_tbl_rule_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field Identifier for INPORTS qualifier. */
    uint32_t inports_key_fid;

    /*! Field Identifier for INPORTS qualifier. */
    uint32_t inports_mask_fid;

    /*! Field Identifier for SYSTEM_PORTS qualifier. */
    uint32_t system_ports_key_fid;

    /*! Field Identifier for SYSTEM_PORTS qualifier. */
    uint32_t system_ports_mask_fid;

    /*! Field Identifier for DEVICE_PORTS qualifier. */
    uint32_t device_ports_key_fid;

    /*! Field Identifier for DEVICE_PORTS qualifier. */
    uint32_t device_ports_mask_fid;

    /*! Field Identifier for INPORTS qualifier with parts. */
    uint32_t ipbm_key_fid[BCMFP_MAX_IPBM_PARTS][BCMFP_MAX_IPBM_PIPES];

    /*! Field Identifier for INPORTS qualifier. */
    uint32_t ipbm_mask_fid[BCMFP_MAX_IPBM_PARTS][BCMFP_MAX_IPBM_PIPES];

    /*! Field Identifier for number of parts of IPBM qualifier. */
    uint8_t num_ipbm_parts;

    /*! Field identifier for Compression types array  */
    uint32_t compression_type_array_fid;

    /*!
     * Field identifier for number of elements compression
     * types array.
     */
    uint32_t num_compression_type_fid;

    /*! Field Identifier for key type. */
    uint32_t key_type_fid;

    /*! Field Identifier for key type mask. */
    uint32_t key_type_mask_fid;

    /*!
     * Array to map qualifiers LT fields in rule table to
     * corresponding qualifier LT fields in group table.
     */
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info;

    /*! HASH_A0 value. */
    uint32_t hash_a0_fid;

    /*! HASH_A0_MASK value. */
    uint32_t hash_a0_mask_fid;

    /*! HASH_A1 value. */
    uint32_t hash_a1_fid;

    /*! HASH_A1_MASK value. */
    uint32_t hash_a1_mask_fid;

    /*! HASH_B0 value. */
    uint32_t hash_b0_fid;

    /*! HASH_B0_MASK value. */
    uint32_t hash_b0_mask_fid;

    /*! HASH_B1 value. */
    uint32_t hash_b1_fid;

    /*! HASH_B1_MASK value. */
    uint32_t hash_b1_mask_fid;

    /*! HASH_C0 value. */
    uint32_t hash_c0_fid;

    /*! HASH_C0_MASK value. */
    uint32_t hash_c0_mask_fid;

    /*! HASH_C1 value. */
    uint32_t hash_c1_fid;

    /*! HASH_C1_MASK value. */
    uint32_t hash_c1_mask_fid;

    /*! Drop data. */
    uint32_t drop_data_fid;

    /*! Drop data mask. */
    uint32_t drop_data_mask_fid;

    /*! Drop decision. */
    uint32_t drop_decision_fid;

    /*! Drop decision mask. */
    uint32_t drop_decision_mask_fid;

    /*! Trace vector. */
    uint32_t trace_vector_fid;

    /*! Trace vector mask. */
    uint32_t trace_vector_mask_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_rule_t;

typedef struct bcmfp_tbl_action_fid_info_s {

    uint32_t action;

} bcmfp_tbl_action_fid_info_t;

/*! Logical Table details(SID and some FIDs) for policy. */
typedef struct bcmfp_tbl_policy_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Flex counter red packet count field. */
    uint32_t flex_ctr_r_count_fid;

    /*! Flex counter yellow packet count field. */
    uint32_t flex_ctr_y_count_fid;

    /*! Flex counter green packet count field. */
    uint32_t flex_ctr_g_count_fid;

    /*! Flex counter object field. */
    uint32_t flex_ctr_object_fid;

    /*! Flex counter container ID field. */
    uint32_t flex_ctr_container_id_fid;

    /*! Array to map action LT fields in policy table. */
    bcmfp_tbl_action_fid_info_t *actions_fid_info;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_policy_t;

/*! Logical Table details(SID and some FIDs) for entry. */
typedef struct bcmfp_tbl_entry_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field identifier for operation state. */
    uint32_t oprtnl_state_fid;

    /*! Field identifier for entry priority. */
    uint32_t priority_fid;

    /*! Field identifier for group ID. */
    uint32_t group_id_fid;

    /*! Field identifier for rule ID. */
    uint32_t rule_id_fid;

    /*! Field identifier for policy ID. */
    uint32_t policy_id_fid;

    /*! Field identifier for meter ID. */
    uint32_t meter_id_fid;

    /*! Field identifier for counter ID. */
    uint32_t ctr_id_fid;

    /*! Field identifier for flex counter base index. */
    uint32_t flex_ctr_base_idx_fid;

    /*! Field identifier for flex counter offset mode. */
    uint32_t flex_ctr_offset_mode_fid;

    /*! Field identifier for flex counter pool index. */
    uint32_t flex_ctr_pool_id_fid;

    /*! Field identifier for flex ctr action index. */
    uint32_t flex_ctr_action_fid;

    /*! Field Identifier for enable/disable. */
    uint32_t enable_fid;

    /*! Field identifier for SBR ID. */
    uint32_t sbr_id_fid;

    /*! Field identifier for PDD ID. */
    uint32_t pdd_id_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_entry_t;

/*! Logical Table details(SID and some FIDs) for preselection entry. */
typedef struct bcmfp_tbl_pse_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field Identifier for Presel Entry Priority. */
    uint32_t priority_fid;

    /*! Drop decision field ID. */
    uint32_t drop_decision_fid;

    /*! Drop decision mask field ID. */
    uint32_t drop_decision_mask_fid;

    /*!
     * Array to map qualifiers LT fields in pse table to
     * corresponding qualifier LT fields in psg table.
     */
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info;

    /*! Field Identifier for enable/disable. */
    uint32_t enable_fid;

    /*! Field Identifier for key type. */
    uint32_t key_type_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_pse_t;

/*! Logical Table details(SID and some FIDs) for preselection group. */
typedef struct bcmfp_tbl_psg_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Drop decision field ID. */
    uint32_t drop_decision_fid;

    /*! Array to map qualifiers LT fields in psg table. */
    bcmfp_tbl_qualifier_fid_info_t *qualifiers_fid_info;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_psg_t;

/*! Logical Table details(SID and some FIDs) for PDD. */
typedef struct bcmfp_tbl_pdd_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*!
     * Array to map action LT fields in PDD table to
     * corresponding action LT fields in policy table.
     */
    bcmfp_tbl_action_fid_info_t *actions_fid_info;

    /*!
     * Auxiliary drop code action fid, which is part of aux_data
     * in pdd profile entry.
     */
    uint32_t aux_drop_code_fid;

    /*!
     * Auxiliary drop priority action fid, which is part of aux_data
     * in pdd profile entry.
     */
    uint32_t aux_drop_priority_fid;

    /*!
     * Auxiliary trace id fid, which is part of aux_data
     * in pdd profile entry.
     */
    uint32_t aux_trace_id_fid;

    /*!
     * Auxiliary flex ctr valid, which is part of aux_data
     * in pdd profile entry.
     */
    uint32_t aux_flex_ctr_valid_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_pdd_t;

/*! Logical Table details(SID and some FIDs) for SBR. */
typedef struct bcmfp_tbl_sbr_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*!
     * Array to map action LT fields in SBR table to
     * corresponding action LT fields in policy table.
     */
    bcmfp_tbl_action_fid_info_t *actions_fid_info;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_sbr_t;

/*! Logical Table details(SID and some FIDs) for counter entry. */
typedef struct bcmfp_tbl_ctr_entry_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field identifier for pipe ID. */
    uint32_t pipe_id_fid;

    /*! Field identifier for pool ID. */
    uint32_t pool_id_fid;

    /*! Field identifier for counter color. */
    uint32_t color_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_ctr_entry_t;

/*! Logical Table details(SID and some FIDs) for counter stat. */
typedef struct bcmfp_tbl_ctr_stat_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field identifier for packet count. */
    uint32_t pkt_count_fid;

    /*! Field identifier for byte count. */
    uint32_t byte_count_fid;

    /*
     * HW table where packet and byte count are
     * collected.
     */
    bcmdrd_sid_t counter_sid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_ctr_stat_t;

/*! Logical Table details(SID and some FIDs) for meter. */
typedef struct bcmfp_tbl_meter_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_meter_t;

/*! Logical Table details(SID and some FIDs) for source class mode. */
typedef struct bcmfp_tbl_src_class_s {
    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field Identifier for pipe_id */
    uint32_t pipe_id_fid;

    /*! Field Identifier for src_class. */
    uint32_t mode_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_src_class_t;

/*!
 * Logical Table details(SID and some FIDs) for ALPM LTs used
 * for field compression.
 */
typedef struct bcmfp_tbl_compress_alpm_s {
    /*! compress alpm SID. */
    bcmltd_sid_t sid;

    /*! Array of key FIDs in the order of LSB to MSB. */
    bcmltd_fid_t *key_fids;

    /*! Number of key FIDs. */
    uint8_t num_key_fids;

    /*! Array of mask FIDs in the order of LSB to MSB. */
    bcmltd_fid_t *mask_fids;

    /*! Number of mask FIDs. */
    uint8_t num_mask_fids;

    /*! Compression ID FID. */
    bcmltd_fid_t cid_fid;

    /*! Data type FID. */
    bcmltd_fid_t data_type_fid;

    /*!
     * All table fid offsets and widths inofmration in
     * physical entry.
     */
    bcmfp_tbl_fid_hw_info_t *fid_hw_info;

    /*! Number of elements in fid_hw_info array. */
    uint16_t num_fid_hw_info;
} bcmfp_tbl_compress_alpm_t;

typedef struct bcmfp_tbl_range_check_group_s {
    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Physical pipe line FID. */
    int pipe_id_fid;

    /*! Range check ID FID */
    uint32_t rcid_fid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_range_check_group_t;

/*! Logical Table details(SID and some FIDs) for group info. */
typedef struct bcmfp_tbl_group_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Field Identifier for hardware ltid used in action resolution
     *  or partition resolution.
     */
    uint32_t hw_ltid;

    /*! Field Identifier for number of entries created.*/
    uint32_t num_entries_created;

    /*! Field Identifier for number of entries which can be created.*/
    uint32_t num_entries_tentative;

    /*! Field Identifier for number of partitions.*/
    uint32_t num_partition_id;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_group_info_t;

/*! Logical Table details(SID and some FIDs) for slice info. */
typedef struct bcmfp_tbl_slice_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID */
    uint32_t key_fid;

    /*! Logical table pipe FID */
    uint32_t pipe_fid;

    /*! Field Identifier for number of entries created.*/
    uint32_t num_entries_created;

    /*! Field Identifier for total number of entries which can be created.*/
    uint32_t num_entries_total;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_slice_info_t;

/*! Logical Table details(SID and some FIDs) for group info. */
typedef struct bcmfp_tbl_stage_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Field Identifier for number of slices.*/
    uint32_t num_slices;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_stage_info_t;

/*! Logical Table details(SID and some FIDs) for group partition info. */
typedef struct bcmfp_tbl_group_partition_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID 1 */
    uint32_t group_key_fid;

    /*! Logical table key FID 2*/
    uint32_t part_key_fid;

    /*! FID for number of auto expanded slices in this partition. */
    uint32_t num_slice_fid;

    /*! FID for list of slice numbers. */
    uint32_t slice_fid;

    /*! FID for list of virual slice numbers. */
    uint32_t virtual_slice_fid;

    /*! Field Identifiers for offsets, widths, partition ID fid arrays
     * for every qualifier.
     */
    bcmfp_tbl_offset_info_t *qual_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_A0 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_a0_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_A1 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_a1_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_B0 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_b0_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_B1 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_b1_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_C0 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_c0_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for HASH_C1 qualifier.
     */
    bcmfp_tbl_offset_info_t hash_c1_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for DROP_DATA qualifier.
     */
    bcmfp_tbl_offset_info_t drop_data_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for DROP_DECISION qualifier.
     */
    bcmfp_tbl_offset_info_t drop_decision_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for TRACE_VECTOR qualifier.
     */
    bcmfp_tbl_offset_info_t trace_vector_fid_info;

    /*! FID for em key attributes index info. */
    uint32_t em_key_attrib_sid;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_group_partition_info_t;

/*! Logical Table details(SID and some FIDs) for PDD info. */
typedef struct bcmfp_tbl_pdd_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID 1 */
    uint32_t pdd_key_fid;

    /*! Logical table key FID 2*/
    uint32_t part_key_fid;

    /*! Field Identifiers for offsets, widths, order ID fid arrays
     * for every action.
     */
    bcmfp_tbl_offset_info_t *action_fid_info;

    /*! Field Identifiers for offsets, widths, order ID fid arrays
     * for flex ctr valid action.
     */
    bcmfp_tbl_offset_info_t action_flex_ctr_valid_fid_info;

    /*! Field Identifiers for offsets, widths, order ID fid arrays
     * for drop code action.
     */
    bcmfp_tbl_offset_info_t action_drop_code_fid_info;

    /*! Field Identifiers for offsets, widths, order ID fid arrays
     * for drop priority action.
     */
    bcmfp_tbl_offset_info_t action_drop_priority_fid_info;

    /*! Field Identifiers for offsets, widths, order ID fid arrays
     * for trace id action.
     */
    bcmfp_tbl_offset_info_t action_trace_id_fid_info;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_pdd_info_t;

/*! Logical Table details(SID and some FIDs) for presel group info. */
typedef struct bcmfp_tbl_presel_group_info_s {

    /*! Logical table ID */
    uint32_t sid;

    /*! Logical table key FID 1 */
    uint32_t psg_key_fid;

    /*! Logical table key FID 2*/
    uint32_t part_key_fid;

    /*! Field Identifiers for offsets, widths, partition ID fid arrays
     * for every qualifier.
     */
    bcmfp_tbl_offset_info_t *qual_fid_info;

    /*!
     * Field Identifiers for offsets, widths, order ID fid arrays
     * for DROP_DECISION qualifier.
     */
    bcmfp_tbl_offset_info_t drop_decision_fid_info;

    /*!
     * This is the total number of FID count int this LT.
     * This is equivalent to FIELD_COUNT FID generated
     * the LT.
     */
    uint16_t fid_count;

    /*!
     * LRD info of table. Information that is accessed very
     * frequently and takes time to retrive from LRD is
     * maintained in this structure for faster access.
     */
    bcmfp_tbl_lrd_info_t lrd_info;
} bcmfp_tbl_presel_group_info_t;

/*!
 * Differnt field IDs information of LT that contains packet
 * or metadat fields eligible for remapping or range checking.
 */
typedef struct bcmfp_tbl_compression_info_s {
    /*! Field compression info LTID. */
    uint32_t sid;

    /*! Key field ID */
    uint32_t key_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using first match and remap LT
     */
    uint32_t match_remap_0_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using second match and remap LT
     */
    uint32_t match_remap_1_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using first index and remap LT
     */
    uint32_t index_remap_0_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using second index and remap LT
     */
    uint32_t index_remap_1_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using third index and remap LT
     */
    uint32_t index_remap_2_fid;

    /*!
     * Field ID of field holding the info about which packet or
     * metadat field is compressed using fourth index and remap LT
     */
    uint32_t index_remap_3_fid;

    /*!
     * Field ID of field holding the info about one of the packet or
     * metadat field which can be range checked using range check LT.
     */
    uint32_t range_check_0_fid;

    /*!
     * Field ID of field holding the info about one of the packet or
     * metadat field which can be range checked using range check LT.
     */
    uint32_t range_check_1_fid;

    /*!
     * Field ID of field holding the info about one of the packet or
     * metadat field which can be range checked using range check LT.
     */
    uint32_t range_check_2_fid;

    /*!
     * Field ID of field holding the info about one of the packet or
     * metadat field which can be range checked using range check LT.
     */
    uint32_t range_check_3_fid;
} bcmfp_tbl_compression_info_t;

/*!
 * Function pointer to get varient specific compression info
 * LT field value get.
 */
typedef int(*bcmfp_fn_compression_info_get_t)(int unit,
                                              uint32_t key,
                                              bcmltd_fid_t fid,
                                              uint32_t *fid_value);
/*!
 * Each stage have a reference to this structure. It contains
 * all valid logical tables information(SID and some FIDs).
 */
typedef struct bcmfp_tbls_s {

    /*! Logical Table information for group */
    bcmfp_tbl_group_t *group_tbl;

    /*! Logical Table information for rule */
    bcmfp_tbl_rule_t *rule_tbl;

    /*! Logical Table information for policy */

    bcmfp_tbl_policy_t *policy_tbl;

    /*! Logical Table information for entry */
    bcmfp_tbl_entry_t *entry_tbl;

    /*! Logical Table information for PDD */
    bcmfp_tbl_pdd_t *pdd_tbl;

    /*! Logical Table information for SBR */
    bcmfp_tbl_sbr_t *sbr_tbl;

    /*! Logical Table information for preselection group */
    bcmfp_tbl_pse_t *pse_tbl;

    /*! Logical Table information for preselection entry */
    bcmfp_tbl_psg_t *psg_tbl;

    /*! Logical Table information for counter entry */
    bcmfp_tbl_ctr_entry_t *ctr_entry_tbl;

    /*! Logical Table information for counter stat */
    bcmfp_tbl_ctr_stat_t *ctr_stat_tbl;

    /*! Logical Table information for meters */
    bcmfp_tbl_meter_t *meter_tbl;

    /*! Logical Table information for mixed source class mode */
    bcmfp_tbl_src_class_t *src_class_tbl;

    /*! Logical Table information for range check group */
    bcmfp_tbl_range_check_group_t *range_check_group_tbl;

    /*!
     * Information of FIDs eligible for ALPM compression that
     * are present in group and rule logical tables.
     */
    bcmfp_tbl_compress_fid_info_t *compress_fid_info[BCMFP_COMPRESS_TYPES_MAX];

    /* Logical Table information for ALPM compression. */
    bcmfp_tbl_compress_alpm_t *compress_alpm_tbl[BCMFP_COMPRESS_TYPES_MAX];

    /*! Logical Table information for group info. */
    bcmfp_tbl_group_info_t *group_info_tbl;

    /*! Logical Table information for group info. */
    bcmfp_tbl_group_partition_info_t *group_part_tbl;

    /*! Logical Table information for group info. */
    bcmfp_tbl_pdd_info_t *pdd_info_tbl;

    /*! Logical Table information for group info. */
    bcmfp_tbl_presel_group_info_t *psg_info_tbl;

    /* Logical Table information for Stage info. */
    bcmfp_tbl_stage_info_t *stage_info_tbl;

    /* Logical Table information for slice info. */
    bcmfp_tbl_slice_info_t *slice_info_tbl;

    /*! Number of LT FID to other LT FID mappings. */
    uint32_t num_compress_fid_remote_info;

    /*! Pointer to one LT FID to other LT FID mappings. */
    bcmfp_tbl_fid_remote_info_t const *compress_fid_remote_info;

    /*! Logical table field information of compression info LT. */
    bcmfp_tbl_compression_info_t *compression_info_tbl;

    /*!
     * Function pointer to get varient specific compression info
     * LT field value get.
     */
    bcmfp_fn_compression_info_get_t compression_info_get;
} bcmfp_tbls_t;

#endif /* BCMFP_TBL_INTERNAL_H */
