/*! \file bcmptm_rm_hash_internal.h
 *
 * APIs, defines for PTRM-Hash interfaces
 *
 * This file contains APIs, defines for PTRM-Hash interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_INTERNAL_H
#define BCMPTM_RM_HASH_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmptm/bcmlrd_ptrm.h>
#include <bcmptm/bcmptm_cmdproc_internal.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm.h>

/*******************************************************************************
 * Defines
 */
/*! Max move depth for hash reordering.  */
#define BCMPTM_RM_HASH_MAX_HASH_REORDERING_DEPTH   6

/*! Default move depth for hash reordering.  */
#define BCMPTM_RM_HASH_DEF_HASH_REORDERING_DEPTH   4

/*! Max hash bank count for a physical table. */
#define BCMPTM_RM_HASH_MAX_BANK_COUNT              8

/*! Bitsize of robust hash added for an entry key. */
#define BCMPTM_RM_HASH_RHASH_KEY_SPACE_WIDTH      16

/*! Maximum response instance count for SID and entry index. */
#define BCMPTM_RM_HASH_RSP_MAX_INST_CNT            2

/*! HA block id for transaction record. */
#define BCMPTM_HA_SUBID_RMHASH_TRANS_REC           0

/*! HA block id for hash bucket state control block. */
#define BCMPTM_HA_SUBID_RMHASH_BKT_STATE_CTRL      1

/*! HA block id for hash entry node control block. */
#define BCMPTM_HA_SUBID_RMHASH_ENT_NODE_CTRL       2

/*! HA block id for software logical bucket control block. */
#define BCMPTM_HA_SUBID_RMHASH_SLB_INFO_CTRL       3

/*! HA block id for transaction undo node counter. */
#define BCMPTM_HA_SUBID_RMHASH_UNDO_CTRL           4

/*! HA block id for transaction undo list. */
#define BCMPTM_HA_SUBID_RMHASH_UNDO_CHUNKID_BASE   5

/*! HA block id for narrow mode entry bitmap control block. */
#define BCMPTM_HA_SUBID_RMHASH_NME_BMP_CTRL        6

/*! HA block id for the first dynamic block. */
#define BCMPTM_HA_SUBID_RMHASH_BLK_ID_FIRST      128

/*! HA block id for the first narrow mode block. */
#define BCMPTM_HA_SUBID_RMHASH_NMBLK_ID_FIRST     (1536 + 128)

/*! HA block id for the DT_EM_FP_ENTRY hash store header block. */
#define BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_HDR_ID   2000

/*! HA block id for the DT_EM_FP_ENTRY hash store entry block. */
#define BCMPTM_HA_SUBID_RMHASH_FP_HASH_STORE_ENT_ID   2001

/*! Bit flags indicating bucket mode none. */
#define RM_HASH_BMN                          (1 << 0)

/*! Bit flags indicating bucket mode 0. */
#define RM_HASH_BM0                          (1 << 1)

/*! Bit flags indicating bucket mode 1. */
#define RM_HASH_BM1                          (1 << 2)

/*! Bit flags indicating bucket mode 2. */
#define RM_HASH_BM2                          (1 << 3)

/*******************************************************************************
 * Typedefs
 */
/*! \brief All the possible hash algorithms. */
typedef enum {
    BCMPTM_RM_HASH_VEC_ZERO,
    BCMPTM_RM_HASH_VEC_CRC32,
    BCMPTM_RM_HASH_VEC_CRC16,
    BCMPTM_RM_HASH_VEC_LSB,
    BCMPTM_RM_HASH_VEC_CRC32_CRC16_LSB,
    BCMPTM_RM_HASH_VEC_CRC32_CRC16_ZERO,
    BCMPTM_RM_HASH_VEC_CRC32A_CRC32B,
    BCMPTM_RM_HASH_VEC_CRC32A_LSB_ZERO
} bcmptm_rm_hash_vector_type_t;

/*! \brief All the possible hash modes
  - Dual hash and shared hash have different entry index calculating mechanism
  w.r.t hash bucket. */
typedef enum {
    BCMPTM_RM_HASH_TYPE_DUAL,
    BCMPTM_RM_HASH_TYPE_SHARED
} bcmptm_rm_hash_type_t;

/*! Group Modes supported */
typedef enum  bcmptm_rm_hash_entry_mode_e {
    /*! Entry Mode none. */
    BCMPTM_RM_HASH_ENTRY_MODE_NONE = 0,

    /*! Exact Match Mode 128. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_128,

    /*! Exact Match Mode 160. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_160,

    /*! Exact Match Mode 320. */
    BCMPTM_RM_HASH_ENTRY_MODE_EM_320,

    /*! Using MPLS_ENTRY table. */
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_MPLS,

    /*! Using L3_TUNNEL_SINGLE table */
    BCMPTM_RM_HASH_ENTRY_MODE_TNL_DECAP_L3_TNL,

    /*! Using flexible key format */
    BCMPTM_RM_HASH_ENTRY_MODE_FLEX,

    /*! Entry Mode Count. */
    BCMPTM_RM_HASH_ENTRY_MODE_COUNT,
} bcmptm_rm_hash_entry_mode_t;

/*! Hash store 16 bit selection type. */
typedef enum bcmptm_rm_hash_store_16bit_sel_e {
    /*! Select every 4th bit starting from bit 0 of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_16BIT_SELECTION_EVERY_FOURTH_BIT_0 = 0,

    /*! Select every 4th bit starting from bit 1 of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_16BIT_SELECTION_EVERY_FOURTH_BIT_1 = 1,

    /*! Select every 4th bit starting from bit 2 of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_16BIT_SELECTION_EVERY_FOURTH_BIT_2 = 2,

    /*! Select every 4th bit starting from bit 3 of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_16BIT_SELECTION_EVERY_FOURTH_BIT_3 = 3,

    /*! 16bit selection count. */
    BCMPTM_RM_HASH_STORE_16BIT_SELECTION_COUNT = 4,
} bcmptm_rm_hash_store_16bit_sel_t;

/*! Hash store 32 bit selection type. */
typedef enum bcmptm_rm_hash_store_32bit_sel_e {
    /*! Select all even bits of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_32BIT_SELECTION_EVEN_BITS = 0,

    /*! Select all odd bits of hash_vector(64). */
    BCMPTM_RM_HASH_STORE_32BIT_SELECTION_ODD_BITS = 1,

    /*! 32bit selection count. */
    BCMPTM_RM_HASH_STORE_32BIT_SELECTION_COUNT = 2,
} bcmptm_rm_hash_store_32bit_sel_t;

/*! Hash store key length type. */
typedef enum bcmptm_rm_hash_store_type_e {
    /*! Store 16bit key length. */
    BCMPTM_RM_HASH_STORE_TYPE_16BIT_KEY_LENGTH = 0,

    /*! Store 32bit key length. */
    BCMPTM_RM_HASH_STORE_TYPE_32BIT_KEY_LENGTH = 1,

    /*! Hash story type. */
    BCMPTM_RM_HASH_STORE_TYPE_COUNT = 2,
} bcmptm_rm_hash_store_type_t;

/*! Hash store attribute. */
typedef struct bcmptm_rm_hash_store_attr_s {
    /*! Enable/disable hash store function.*/
    bool enable;

    /*! Hash store key length type. */
    bcmptm_rm_hash_store_type_t store_type;

    /*! Bit selection type from hash_vector. */
    uint8_t bit_sel_type;

    /*! new key fields for hash store. */
    bcmlrd_hw_field_list_t *hash_store_key_fields;

    /*! Entry words to store new entry in hash store mode. */
    uint32_t *hash_store_ent_words;

} bcmptm_rm_hash_store_attr_t;

/*! \brief Attributes for FP flexible exact match hash entry key.
  - This structure will be provided by rm hash caller when the LT is a dynamic LT.
 */
typedef struct bcmptm_rm_hash_dynamic_entry_attrs_s {
    /*! Key field information will be provided for dynamic table. */
    bcmlrd_hw_field_list_t *hw_key_fields;

    /*! Bucket mode of the entry. */
    uint8_t bucket_mode;

    /*! Size of the entry in the unit of base entry. */
    uint8_t entry_size;

    /*! Hash store attribute. */
    bcmptm_rm_hash_store_attr_t hstore_attr;
} bcmptm_rm_hash_dynamic_entry_attrs_t;

/*! This structure is passed from FP to PTM on
 * every INSERT/DELETE/UPDATE operations of
 * entries in FP_EM_ENTRY table. FP will
 * pass this as a void pointer to PTM and FP
 * subcomponent in PTM module will typecast
 * the void pointer to this structure type.
 */
typedef struct bcmptm_rm_hash_entry_attrs_s {

    /*! Mode(EM_128/EM_160/EM_320) of this etnry.
     * This information is required for every operation.
     */
    bcmptm_rm_hash_entry_mode_t entry_mode;

    /*! Dynamic entry attribute. Null for static hash LTs. */
    bcmptm_rm_hash_dynamic_entry_attrs_t *d_entry_attrs;
} bcmptm_rm_hash_entry_attrs_t;

/*! \brief Attributes for hardware hash bucket computing on per hank basis. */
typedef struct bcmptm_rm_hash_vec_bank_attr_s {

/*! \brief Mask to the hash vector.
  - The mask applied to the hash vector for a given physical bank to get the
    final bucket pointer in that bank.
  - Mostly this is calculated by bucket_count - 1. */
    uint32_t mask;
} bcmptm_rm_hash_vec_bank_attr_t;

/*! \brief Attributes for hash vector on per logical table basis. */
typedef struct bcmptm_rm_hash_vec_attr_s {

/*! \brief hash vector algorithm used to do hash calculation.
  - This member describes the hash vector generation algorithm.
  On some device, all the hash tables use the common CRC32A + CRC32B
  generation algorithm. */
    bcmptm_rm_hash_vector_type_t type;

/*! \brief Key length used to do hash calculation.
  - This is the maximal key length among all the key types supported by
  the underlying physical hash table. It is used to calculate the hash vector.
  Also, it is used to pad zeros if the actual (compare) key length is less,
  because for a same SID, the hash calculation assumes key_length is always
  the maximal one. */
    uint16_t key_length;

/*! \brief width of KEY_LSBf for a specific LTID.
  - The LSB of a key can be directly used as a hash vector,
    this is mainly for debug purpose. For example, on some devices,
    for SID L3_ENTRY_IPV4_UNICASTm, there exists LSB fields like
    IPV4UC__HASH_LSBf, IPV4UC__HASH_LSBf. For a given LTID, the LSB is fixed.
  - Same for all banks. */
    uint16_t hw_lsb_field_width;

/*! \brief start_bit position of KEY_LSBf.
  - MUST be same for all banks. */
    uint16_t hw_lsb_field_start_bit;

/*! \brief Number of vector bank attribute for a hash table. */
    uint8_t bank_attr_count;

/*! \brief Array of bank attributes for a given LTID.
  - Number of elements = num_banks (as declared above)
  - Separate hash_vector info is needed for each bank (irrespective of whether
  they map to same / different SID values). */
    const bcmptm_rm_hash_vec_bank_attr_t * bank_attr;
} bcmptm_rm_hash_vec_attr_t;

/*! \brief Hit table mapping for this LT.
  - A hash LT entry may consist of key + data + hit. And a LT may have
  multiple physical hit tables. For each hit table, it can be described
  by this structure. */
typedef struct bcmptm_rm_hash_hit_map_s {
    /*! \brief SID for the hit table. */
    const bcmdrd_sid_t *sid;

    /*! \brief logical hit bit position in physical table entry. */
    uint16_t bit;
} bcmptm_rm_hash_hit_map_t;

/*! \brief Hit table information for this LT.
  - A hash LT entry may consist of key + data + hit. On some devices,
  there is a combined physical view containing both functional table and
  hit tables. On some devices, the functional tables and hit tables are separated,
  i.e. there is no combined physical view. */
typedef struct bcmptm_rm_hash_hit_info_s {
    /*! \brief Number of hit tables. */
    uint8_t hit_map_count;

    /*! \brief Array of hit table mapping for a LT. */
    const bcmptm_rm_hash_hit_map_t *hit_map;
} bcmptm_rm_hash_hit_info_t;

/*! \brief Information specific to hw_entry for this LT.
  - Generally, a LT entry consist of key portion + data portion. On some devices,
  a LT has single KEY TYPE value, and thus single key field list. This key portion
  is appended with single data field list. The key + data combination corresponds
  to a specific width (view) on the hash table.
  This structure describes the information of this specific view.
  - For some LTs on some devices, they have multiple KEY TYPE values, and thus
  multiple KEY field list, for each KEY field list, there is a specific width (view)
  on the hash table.
  - On some newer devices, a LT has single KEY TYPE on each search bank. A LT
  can map to multiple search banks. In addition, for the given KEY field
  list on each search bank, it can be appended by multiple data field lists. Each
  key + data combination correspond to a specific width on the search bank. Each
  valid width of the search bank for this LT is described by a object of this
  structure.
  - This is a per SID structure. */
typedef struct bcmptm_rm_hash_view_info_s {

/*! \brief SID for the LT.
  - On some devices, a LT can only map to one physical hash table spanning
  multiple banks. All the banks have same SID.
  - On some devices, a LT can map to 1 or multiple search banks. Each bank
  has a separated SID.
  - NOT an array, and it is a pointer to the buffer maintained by LRD that holds
  the SID value. */
    const bcmdrd_sid_t *sid;

/*! \brief HW details of KEY_TYPEf in the entry on this SID.
  - This member describes the information of KEY_TYPE fields for a specific
  width entry.
  - On some devices, key type information is same across all its banks.
  - NOT an array - but pointer to structure maintained by LRD
  - NULL means underlying PTs don't have KEY_TYPEf */
    const bcmlrd_hw_rfield_info_t *hw_ktype_field_info;

/*! \brief HW details of DATA_TYPEf in the entry on this SID.
  - This member describes the information of DATA_TYPE fields for a specific
  width entry.
  - On some devices, data type information is same across all its banks.
  - NOT an array - but pointer to structure maintained by LRD
  - NULL means underlying PTs don't have DATA_TYPEf */
    const bcmlrd_hw_rfield_info_t *hw_dtype_field_info;

/*! \brief Number of base entries in the entry on this SID.
  - num_base_entries describes how many base entries are occupied for an
  entry in this table view. On all devices, for a given logical key fields +
  data fields combination, the width for this case will be determined. Thus
  the number of base entries will be determed as well. */
    uint8_t  num_base_entries;

/*! \brief Corresponding LP table SID for the functional SID.
  - On some devices, low power tables for the functional hash tables are
  managed by hardware. This member will be NULL.
  - On some devices, low power tables for the functional hash tables
  are managed by software. In addition, each search bank has separated SID. */
    const bcmdrd_sid_t *lp_sid;

/*! \brief Corresponding hit table information.
  - On some devices, there is no separated HIT table. This member will be NULL.
  - On some devices, the HIT table is separated with functional hash table.
  This member will point to the hit table info. */
    const bcmptm_rm_hash_hit_info_t *hit_info;
} bcmptm_rm_hash_view_info_t;

/*! \brief Structure describing all the views on a physical table for a LT. */
typedef struct bcmptm_rm_hash_grp_view_info_s {

/*! \brief Number of all the valid views on a physical hash table for a LT.
  - On some devices, the value is 1 for a LT.
  - On some devices, the value is the number of valid views on a search bank
  this LT can map. */
    uint8_t view_info_count;

/*! \brief Array of bcmptm_rm_hash_view_info_t. Each element corresponds
  to one view on physical hash table. */
    const bcmptm_rm_hash_view_info_t *view_info;
} bcmptm_rm_hash_grp_view_info_t;

/*! \brief Structure describing all the table view information of a LT. */
typedef struct bcmptm_rm_hash_lt_view_info_s {

/*! \brief Number of all the hash tables.
  - On some devices, the member is always 1.
  - On some devices, the value of this member is the number of search banks
  (physical tables) this LT maps onto. */
    uint8_t grp_view_info_count;

/*! \brief Array of bcmptm_rm_hash_grp_view_info_t. Each element corresponds
  to view information on a phsyical hash table. */
    const bcmptm_rm_hash_grp_view_info_t *grp_view_info;
} bcmptm_rm_hash_lt_view_info_t;

/*! \brief Valid view information on a physical hash table for a LT. */
typedef struct bcmptm_rm_hash_valid_view_info_s {

/*! \brief Number of valid views for a LT on this physical hash table. */
    uint8_t view_index_count;

/*! \brief Array of index into array of valid views this LT maps onto. */
    const uint8_t *view_index;
} bcmptm_rm_hash_valid_view_info_t;

/*! \brief Additional info for HASH based LTs accessed on modeled path.
  - Generally, a LT entry consist of key portion + data portion. This data
  structure is used for describing the KEY TYPE value and the key field list
  mapping on this hash table. For some LTs, they have multiple KEY TYPE values
  and thus multiple key field lists, for each KEY TYPE value and key field list,
  it has a separated object of this structure.
  - On some devices, a LT has single KEY TYPE on each search banks
  (hash tables). This member describes each KEY TYPE and key field list on
  each separated hash tables. */
typedef struct bcmptm_rm_hash_key_format_s {

/*! \brief KEY_TYPE value on this table for the LT.
  - On some devices, a LT (KEY + DATA) can map to 1 or multiple
  table views of a hash table with multiple banks. For each valid view,
  this LT has a deterministic KEY_TYPE value.
  - On some devices, for a LT the same KEY portion can be appended by
  different DATA portion, and thus corresponds to multiple table widths (views).
  The KEY TYPE value is same for all the table views of a specific bank (table),
  - NULL means underlying PTs don't have KEY_TYPE. */
    const uint16_t *key_type_val;

/*! \brief List of key fields in entry on this table.
  - Used for hash_vector extraction, so ORDER of the fields MUST match the order
  in which hardware uses these fields for hash_vector extraction. */
    const bcmlrd_hw_field_list_t *hw_key_fields;

/*! \brief Entry view info for this key type and key field list.
  - This member describes the information about all the valid view(s) for
  a key format. */
    const bcmptm_rm_hash_valid_view_info_t *valid_view_info;

/*! \brief Hash bank mode for a given LTID.
  - The addressing schemes for dual hash and shared hash are different.
  - hash_type is used to determine how to get the entry index based on
  bucket value.
  - On some devices, there are two schemes of how table index is generated:
  dual or shared.
  - On some devices, only the shared index generation scheme is supported.
  In other words, the value of this member is fixed. */
    bcmptm_rm_hash_type_t hash_type;

/*! \brief mode for a entry.
  - It is just used for hash tabled based LT implemented thru CTH.
  - On some devices, for some LTs, this member can be ignored.
  - On some devices, this member can be ignored. */
    bcmptm_rm_hash_entry_mode_t entry_mode;

/*! \brief Specify whether each bank has separated SID.
  - On some devices, all the banks in a hash table has common table SID.
  This member is FALSE.
  - On some devices, each search bank has separated SID.
  This member is TRUE. */
    bool bank_separate_sid;

/*! \brief Attributes needed to compute hash_vector.
  - See defn for bcmptm_rm_hash_vec_attr_t. */
    const bcmptm_rm_hash_vec_attr_t *hash_vector_attr;

/*! \brief Number of banks for the SID this LT maps onto.
  - On some devices where all banks appear as one SID, this member include
  sum of dedicated banks and shared UFT banks.
  - On some devices where each bank has separated SID, this member has constant
  value 1. */
    uint8_t num_banks;

/*! \brief Bitmap to indicate valid physical banks for this LT.
  - On some devices where all banks appear as one SID, this member indicates
  valid banks on this SID for this LT.
  - On some device, as each SID only has single bank, this member has constant
  value 1. */
    uint16_t valid_bank_bitmap;

/*! \brief Number of base entries in a hash bucket.
  - num_base_entries describes how many base entries are contained in a hash
  bucket. This information is required for calculating entry index based on
  hash bucket.
  - On some devices, for dual hash table, usually there are 8 base entries
  in a bucket. For shared hash table (for example L3_ENTRY), there are 4 base
  entries in a bucket.
  - On some devices, there is no dual hash mechanism. This member will have
  constant value of 4. */
    uint8_t bucket_size;

/*! \brief Bit size of a base entry. */
    uint16_t base_entry_size;

/*! \brief Flag indicating if word alignment is required for robust hash. */
    bool alignment;

/*! \brief KEY attribute table sid of the underlying physical hash table.
  - This member is used for describing the corresponding KEY TYPE ATTRIBUTE
  table for a hash table. If a hash table has no KEY TYPE ATTRIBUTE table,
  the value for this member will be NULL.
  - It is assumed that the field format for all the KEY TYPE ATTRIBUTE tables
  across all the devices will be SAME. */
    const bcmdrd_sid_t *key_attr_sid;

/*! \brief SID of the robust hash remap table.
  - This member is used for describing the REMAP_A table of the robust hash
  KEY+N module for a hash table. If there is no robust hash support for a hash
  table, its value will be NULL.
  - It is assumed that the format for all the REMAP_A tables across all
  the devices will be SAME. */
    const bcmdrd_sid_t *rh_remap_a_sid;

/*! \brief SID of the robust hash remap table
  - This member is used for describing the REMAP_B table of the robust hash
  KEY+N module for a hash table. If there is no robust hash support for a hash
  table, its value will be NULL.
  - It is assumed that the format for all the REMAP_B tables across all
  the devices will be SAME. */
    const bcmdrd_sid_t *rh_remap_b_sid;

/*! \brief SID of the robust hash action table
  - This member is used for describing the ACTION_A table of the robust hash
  KEY+N module for a hash table. If there is no robust hash support for a hash
  table, its value will be NULL.
  - It is assumed that the format for all the ACTION_A tables across all
  the devices will be SAME. */
    const bcmdrd_sid_t *rh_act_a_sid;

/*! \brief SID of the robust hash action table
  - This member is used for describing the ACTION_B table of the robust hash
  KEY+N module for a hash table. If there is no robust hash support for a hash
  table, its value will be NULL.
  - It is assumed that the format for all the ACTION_B tables across all
  the devices will be SAME. */
    const bcmdrd_sid_t *rh_act_b_sid;

/*! \brief Corresponding base entry table SID for the functional SID.
  - On some devices, low power tables for the functional hash tables are
  managed by hardware. This member will be NULL.
  - On some devices, this will be used when calculating LP words. */
    const bcmdrd_sid_t *single_view_sid;
} bcmptm_rm_hash_key_format_t;

/*! \brief Information about all the key formats for a LT on a hash table. */
typedef struct bcmptm_rm_hash_grp_key_info_s {

/*! \brief Number of valid key formats for this LT on a physical hash table.
  - On some devices, for some LTs, usually there is only one key type value
  and key field list info on a hash table for all the valid views. In this case,
  this member has the value of 1.
  - On some devices, each LT will have one KEY TYPE value on each bank(physical
  hash table) it maps onto. The value of this member will be 1. */
    uint8_t key_format_count;

/*! \brief Array of bcmptm_rm_hash_key_format_t for this LT on a hash table.
  - Size of the array is specified by 'key_format_count'. */
    const bcmptm_rm_hash_key_format_t *key_format;
} bcmptm_rm_hash_grp_key_info_t;

/*! \brief Structure describing all the key information of a LT. */
typedef struct bcmptm_rm_hash_lt_key_info_s {

/*! \brief Number of all the hash tables.
  - On some devices, the value of this member is 1.
  - On some devices, the value of this member is the number of search banks
  (physical tables) this LT maps onto. */
    uint8_t grp_key_info_count;

/*! \brief Array of bcmptm_rm_hash_grp_key_info_t. Each element corresponds to key
  info on physical hash table. */
    const bcmptm_rm_hash_grp_key_info_t *grp_key_info;
} bcmptm_rm_hash_lt_key_info_t;

/*! \brief Information about all the data formats for a LT on a hash table. */
typedef struct bcmptm_rm_hash_grp_data_info_s {

/*! \brief Number of logical data type values of this LT on this hash table.
  - On some devices, this member is not applicable.
  - On some devices, the value of this member is the number of valid
  logical data type values. */
    uint8_t num_ldtype;

/*! \brief Array of logical data type values of this LT on this hash table.
  - On some devices, this member is not applicable.
  - On some devices, this member lists each valid logical data type for
  a LT. */
    uint16_t *ldtype_val;

/*! \brief Array of physical data type values of this LT on this hash table.
  - On some devices, this member is not applicable.
  - On some devices, this member lists each valid physical data type for
  a LT. */
    uint16_t *pdtype_val;

/*! \brief Array of physical data field lists of this LT on this hash table.
  - On some devices, this member is not applicable.
  - On some devices, this member lists each valid physical data field
  list mapping for each logical data type value for a LT on this hash table. */
    const bcmlrd_hw_field_list_t *hw_data_fields;

/*! \brief Array of view index for this LT on this hash table.
  - On some devices, this member is not applicable.
  - On some devices, this member lists corresponding view for each logical
  data type on this hash table. */
    uint8_t *entry_size;
} bcmptm_rm_hash_grp_data_info_t;

/*! \brief Structure describing all the data information of a LT. */
typedef struct bcmptm_rm_hash_lt_data_info_s {

/*! \brief Number of all the hash tables.
  - On some devices, the member is not applicable.
  - On some devices, the value of this member is the number of search banks
  (physical tables) this LT maps onto. */
    uint8_t grp_data_info_count;

/*! \brief Array of bcmptm_rm_hash_grp_data_info_t. Each element corresponds to data
  info on physical hash table. */
    const bcmptm_rm_hash_grp_data_info_t *grp_data_info;
} bcmptm_rm_hash_lt_data_info_t;

/*! \brief Structure describing all the view, key and data information
  of a LT. */
typedef struct bcmptm_rm_hash_enh_more_info_s {

/*! \brief Pointer to bcmptm_rm_hash_lt_view_info_t structure of the LT.
  - This member describes the view information of the LT. */
    const bcmptm_rm_hash_lt_view_info_t *lt_view_info;

/*! \brief Pointer to bcmptm_rm_hash_lt_key_info_t structure of the LT.
  - This member describes the key information of the LT. */
    const bcmptm_rm_hash_lt_key_info_t *lt_key_info;

/*! \brief Pointer to data info structure of the LT.
  - This member describes the data information of the LT.
  - On some devices, this is NULL.
  - On some devices, this member points to the data information of this LT. */
    const bcmptm_rm_hash_lt_data_info_t *lt_data_info;

/*! \brief Configurable per pipe.
  - This member is true if the LT has a conditional PIPE field. */
    uint32_t conditional_pipe;

/*! \brief Configuration LT table.
  - This member contains the LTID of the per-pipe config table. */
    uint32_t conditional_ltid;

/*! \brief Configuration LT field.
  - This member  contains the LFID of the per-pipe config table. */
    uint32_t conditional_lfid;
} bcmptm_rm_hash_enh_more_info_t;

/*! \brief Information specific to LT needed by PTRM. */
typedef struct bcmptm_rm_hash_lt_info_s {
/*! \brief Num of all the info besides entry format, including key and data
  mapping */
    uint32_t enh_more_info_count;

/*! \brief Pointer to structure object that describe all the info about info
  besides entry info for a LT. */
    const bcmptm_rm_hash_enh_more_info_t *enh_more_info;

/*! \brief Capability of dynamic bank assignment.
  - TRUE/1 Application can dynamically assign banks to the HASH LT.
  - FALSE/0 Application cannot assign banks to the HASH LT dynamically. */
    bool dynamic_banks;
} bcmptm_rm_hash_lt_info_t;

/*! \brief Information to be passed to PTRM for each LTM op */
typedef struct bcmptm_rm_hash_req_s {

/*! \brief Ptr to array holding entry words
  - Format of fields in this array will match regsfile key, data fields format
  of underlying PT
  - Value of KEY_TYPEf, DATA_TYPEf fields (as needed by HW) must be written by PTRM
  - For LOOKUP, DELETE ops
   - Only key portion of the entry will be provided
   - KEY_TYPE, BASE_VALID values must be filled by PTRM
  - For INSERT op
   - key, data portion of the entry will be provided
   - KEY_TYPE, DATA_TYPE, BASE_VALID values must be filled by PTRM */
    uint32_t *entry_words;

/*! \brief Tells PTRM that this is same key as previous op
  - PTRM must save the hash_vector and bucket related information for LAST
   lookup operation from LTM. If LTM comes back with Insert for same key, it must
   specify 'same_key=TRUE'. This saves PTM from having to re-compute the hash_vector,
   bucket, index, lookup related state, etc for this key.
   -# same_key MUST always be FALSE for LOOKUP op
   -# same_key MUST always be FALSE for DELETE op
   -# same_key MUST always be TRUE for for INSERT op. We don't need it if LTM can
   guarantee that INSERT to PTM will always be preceded by LOOKUP for same key.
   In short, if LTM can guarantee above behavior we dont need 'same_key' param -
   PTRM can implicitly use the expected value.
   -# As per BCMLT op discussions, API req for INSERT must result in error if
   entry already exists. This requires LTM to first issue LOOKUP and (only if
   entry does not exist) then issue INSERT with same_key=TRUE. Because previous
   LOOKUP failed, PTM will attempt to insert new entry when it gets INSERT. Thus
   LTM must always set same_key TRUE for INSERT op.
   -# As per BCMLT op discussions, API req for UPDATE must result in error if
   entry does not exist. This requires LTM to first issue LOOKUP and (only if
   entry exists) then issue INSERT with same_key=TRUE. Because previous LOOKUP
   succeeded, PTM will either: 'replace entry' or 'insert new entry and delete
   old entry'.
   -# ASSUME: We have only one thread for modelled path and one uncommitted
   transaction in WAL. We cannot assemble multiple concurrent transactions (even
   for different tables in WAL) */
    bool same_key;

/*! \brief id for logical dfield_format
  - PTM (LTM) will provide this info ONLY for INSERT ops.
   -# PTM does not have this info for LOOKUP, DELETE commands.

  - When writing the entry (INSERT)
   -# PTRM must provide dfield_format_id with Writes to CmdProc.

  - When searching for entry (for any op)
   -# PTRM must ignore this value
   -# Will be returned by CmdProc during reads of found entry. PTRM must return
   this as part of rsp.
 */
    uint16_t dfield_format_id;

/*! \brief Atrributes of entry needed for special type of HASHs like FP_EM.
  - PTRM will convert to required type and use the information. */
    void *entry_attrs;

} bcmptm_rm_hash_req_t;

/*! \brief Counter index for the entry returned from rm hash. */
typedef struct bcmptm_rm_hash_ctr_index_s {
    /*! Flex counter index for the entry. */
    uint32_t ctr_offset;

} bcmptm_rm_hash_ctr_index_t;

/*! \brief Info returned for hash table request - modelled path */
typedef struct bcmptm_rm_hash_rsp_s {

/*! \brief Index at which entry was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded. */
    uint32_t rsp_entry_index[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

/*! \brief SID of PT where entry was found (all ops)
  - For all ops, if entry was found, PTRM will return SID of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

/*! \brief Flex counter offset for the returned entry. */
    uint32_t rsp_entry_ctr_offset[BCMPTM_RM_HASH_RSP_MAX_INST_CNT];

/*! \brief Count of entry index and sids. */
    uint8_t rsp_entry_count;

/*! \brief Logical ID for dfield_format of found entry (all ops).
  - If entry was found, PTRM will return logical dfield_format.
  - If entry was not found, PTRM will set this var to 0 */
    uint16_t rsp_dfield_format_id;

/*! \brief Pipe of the physical table. */
    int rsp_entry_pipe;

/*! \brief Pointer to info needed for TABLE_PT_OP_INFO */
    bcmptm_pt_op_info_t *rsp_pt_op_info;

/*! \brief Pointer to misc_info (eg: bcmptm_table_info_t). */
    void *rsp_misc;
} bcmptm_rm_hash_rsp_t;

/*! \brief Describe default bank information.
  - This structure is provied for specifying default bank information. */
typedef struct bcmptm_rm_hash_def_bank_info_s {
    /*! Hash vector type. */
    bcmptm_rm_hash_vector_type_t vector_type;

    /*! Count of banks. */
    uint8_t bank_cnt;

    /*! hash offset.*/
    uint8_t hash_offset[BCMPTM_RM_HASH_MAX_BANK_COUNT];

    /*! Enable/disable robust hash. */
    bool robust_en[BCMPTM_RM_HASH_MAX_BANK_COUNT];
} bcmptm_rm_hash_def_bank_info_t;

/*! \brief Hash table bank narrow mode information on a device. */
typedef struct bcmptm_rm_hash_narrow_mode_info_s {
    /*! Hash table SID.*/
    bcmdrd_sid_t sid;

    /*! Narrow table SID.*/
    bcmdrd_sid_t nm_ctrl_sid;

    /*! Width of field in bits */
    uint16_t nm_field_width;

    /*! Start position of the field */
    uint16_t nm_field_start_bit;

    /*! Bit position for each type of narrow mode entry. */
    const bcmlrd_hw_rfield_info_t **nme_field_list;
} bcmptm_rm_hash_narrow_mode_info_t;

/*! \brief Hash table bank context info. */
typedef struct bcmptm_rm_hash_hit_tbl_info_s {
    /*! Tile ID.*/
    uint8_t tile_id;

    /*! Table ID. */
    uint8_t table_id;

    /*! Hit context bit offset. */
    uint8_t offset;
} bcmptm_rm_hash_hit_tbl_info_t;

/*! \brief Hash table bank information on a device. */
typedef struct bcmptm_rm_hash_table_info_s {
    /*! Bank has separated SID. */
    bool separated_sid;

    /*! Hash type. */
    bcmptm_rm_hash_type_t type;

    /*! Hash table SID.*/
    bcmdrd_sid_t sid;

    /*! Narrow table SID.*/
    bcmdrd_sid_t single_view_sid;

    /*! Key attribute table SID.*/
    bcmdrd_sid_t key_attr_sid;

    /*! Low power table SID.*/
    bcmdrd_sid_t lp_sid;

    /*! Number of banks on this SID.*/
    uint8_t num_banks;

    /*! Array of base bucket count for this SID.*/
    const uint32_t *base_bkt_cnt;

    /*! Size of the bucket.*/
    uint8_t bkt_size;

    /*! Bucket modes on this SID.*/
    uint8_t bm_flags;

    /*! Narrow mode control information.*/
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info;

    /*! Hit context information.*/
    const bcmptm_rm_hash_hit_tbl_info_t *hit_tbl_info;
} bcmptm_rm_hash_table_info_t;

/*! Callback function to write robust hash tables. */
typedef int (*bcmptm_rm_hash_robust_hash_write)(int unit,
                                                bcmltd_sid_t ltid,
                                                const bcmltd_op_arg_t *op_arg,
                                                bcmdrd_sid_t sid,
                                                int index,
                                                void *entry_data);
/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
  \brief Perform Lookup, Insert, Delete for Hash Table based LT
  \param [in] unit Logical device id
  \param [in] req_flags TBD. Ignore for now.
           \n (??? eg: final read from HW instead of cache?)
  \param [in] cseq_id Transaction id. Must be passed with every cmdproc_write, read
  \param [in] req_ltid Logical Table enum for current req_op
           \n Must be provided with Writes to CmdProc
           \n Will be provided by CmdProc during reads.
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  - For Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  - tbl_inst member of pt_dyn_info may indicate pipe num.
  - Must pass tbl_inst received as part of pt_dyn_info for all cmdproc write,
  read requests.
  - Need pipe num to support unique mode for EXACT_MATCH tables.
  - Value of -1 means Global mode.
  - pipe_num for all underlying SIDs will be same for a given LT
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] req_info See defn for bcmptm_rm_hash_req_t
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Contents of found entry (all ops)
        \n PTRM will not touch this array if entry was not found (all ops)
        \n PTM will separate out key, data portions for LTM (to support
        returning of data for pdd type LTs)
        \n Is dont_care for INSERT
  \param [out] rsp_info See defn for bcmptm_rm_hash_rsp_t
  \param [out] rsp_ltid LTID of found entry
            \n If entry was found, for all ops, PTRM will return ltid of found
               entry from PTcache (from cmdproc read).
            \n If entry was not found, PTRM will return ALL_ZEROs for LOOKUP,
               DELETE ops.
            \n If entry was not found, PTRM will return req_ltid for INSERT op.
  \param [out] rsp_flags Return ZEROs for LOOKUP, DELETE ops
                      \n For Insert operation, PTRM must provide additional info
                      as below:
                       - OLD_ENTRY_DELETED
                       - REPLACED
                       - ??? Anything else?

  \retval SHR_E_NONE Success
  \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
  \retval SHR_E_FULL Could not perform requested Insert due to Hash Collisions
  \retval SHR_E_MEMORY Could not perform requested Insert due to Table Full
  \retval SHR_E_UNAVAIL Requested op is not supported
 */
extern int
bcmptm_rm_hash_req(int unit,
                   uint64_t req_flags,
                   uint32_t cseq_id,
                   bcmltd_sid_t req_ltid,
                   bcmbd_pt_dyn_info_t *pt_dyn_info,
                   bcmptm_op_type_t req_op,
                   bcmptm_rm_hash_req_t *req_info,
                   size_t rsp_entry_buf_wsize,

                   uint32_t *rsp_entry_words,
                   bcmptm_rm_hash_rsp_t *rsp_info,
                   bcmltd_sid_t *rsp_ltid,
                   uint32_t *rsp_flags);

/*!
 * \brief Initialize PTRM hash (alloc bucket state)
 * \n Must be called every time a new unit is attached and hash table sizes
 * have been determined, configured.
 *
 * \param [in] unit Logical device id
 * \param [in] warm Flag indicating if it is warmboot.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_init(int unit, bool warm);

/*!
 * \brief PTRM hash cleanup (de-alloc bucket state, etc)
 * \n Will release SW data-structures associated with this
 * unit.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_cleanup(int unit);

/*!
 * \brief Component config phase of init sequence
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 * \param [in] phase 1,2,3
 * \n 1 => 1st scan of all LTIDs. Should see calls to bcmptm_rm_hash_lt_reg
 * \n 2 => 2nd scan of all LTIDs. Should see calls to bcmptm_rm_hash_lt_reg
 * \n 3 => No scan of LTIDs. No calls to bcmptm_rm_hash_lt_reg
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_comp_config(int unit, bool warm, bcmptm_sub_phase_t phase);

/*!
 * \brief Registration of LTID with rm_hash
 *
 * \param [in] unit Logical device id
 * \param [in] ltid LTID that needs to be managed by rsrc_mgr
 * \param [in] phase 1,2,3
 * \param [in] lt_state_offset Offset into LT state mem (in rsrc_mgr)
 * \n Note: lt_state_offset will be stored by caller only in phase 2
 * (comp_config)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_lt_reg(int unit, bcmltd_sid_t ltid,
                      bcmptm_sub_phase_t phase, uint32_t *lt_state_offset);

/*!
 * \brief Commit, Abort, Idle_Check
 *
 * \param [in] unit Logical device id
 * \param [in] flags usage TBD
 * \param [in] cseq_id Transaction id
 * \param [in] trans_cmd Commit, Abort, Idle_Check (See bcmptm_trans_cmd_t)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_trans_cmd(int unit, uint64_t flags, uint32_t cseq_id,
                         bcmptm_trans_cmd_t trans_cmd);

/*!
 * \brief Update hash move_depth for a hash typed LT.
 * \n Interface provided for configuration of TABLE_EM_CONTROL LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] lt_id Logical talbe id.
 * \param [in] move_depth New move_depth to be set.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_lt_move_depth_update(int unit, bcmdrd_sid_t lt_id,
                                    int move_depth);

/*!
 * \brief Iterate through all LTIDs and notify RM about LTs that need to be
 * be managed by RM..
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot. Passed to RMs - no
 * other use.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_ltid_identify(int unit, bool warm);

/*!
 * \brief Get default bank info.
 *
 * \param [in] unit Logical device id
 * \param [in] mask Bank mask for a hash table.
 * \param [out] banks_info Pointer to bcmptm_rm_hash_def_bank_info_t structure.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_hash_def_bank_info_get(int unit,
                              uint32_t mask,
                              bcmptm_rm_hash_def_bank_info_t *banks_info);

/*!
 * \brief Get entry hit index.
 *
 * \param [in] unit Logical device id.
 * \param [in] lookup Physical lookup index.
 * \param [out] hit_index Buffer returning the index.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_ent_hit_index_info_get(int unit,
                                      uint32_t lookup,
                                      uint32_t *hit_index);

/*!
 * \brief Update robust hash remap and action table.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [in] trans_id Transaction id.
 * \param [in] write_cb Callback function to program the remap and action table.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_robust_hash_update(int unit,
                                  bcmltd_sid_t ltid,
                                  uint32_t trans_id,
                                  bcmptm_rm_hash_robust_hash_write write_cb);

/*!
 * \brief Get the per unit hash table information.
 *
 * \param [in] unit Logical device id.
 * \param [out] tbl_info Pointer to bcmptm_rm_hash_table_info_t structure.
 * \param [out] num_tbl_info Pointer to buffer saving the result.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_hash_table_info_get(int unit,
                           const bcmptm_rm_hash_table_info_t **tbl_info,
                           uint8_t *num_tbl_info);
#endif /* BCMPTM_RM_HASH_INTERNAL_H */

