/*! \file bcmptm_rm_hash.h
 *
 * Utils, defines internal to RM Hash state
 *
 * This file defines data structures of hash resource manager, and declares
 * functions that operate on these data structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_RM_HASH_H
#define BCMPTM_RM_HASH_H

/*******************************************************************************
 * Includes
 */

#include <bcmptm/bcmptm_rm_hash_internal.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/generated/bcmptm_rm_hash_local.h>

/*******************************************************************************
 * Defines
 */
#define RM_HASH_SHR_MAX_BANK_COUNT                  8
#define RM_HASH_MAX_HW_ENTRY_INFO_COUNT             4
#define RM_HASH_MAX_KEY_TYPE_COUNT                 32
#define RM_HASH_MAX_SLB_SIZE                       16
#define RM_HASH_MAX_SRCH_BANK_COUNT                32
#define RM_HASH_MAX_GRP_FMT_COUNT                   4
#define RM_HASH_MAX_RSRC_GRP_COUNT                 24
#define RM_HASH_MAX_RSRC_MAP_COUNT                 24
#define RM_HASH_MAX_ENT_SIZE                        4
#define RM_HASH_WIDE_DUAL_HASH_BKT_SIZE            16
#define RM_HASH_MAX_GRP_INST_COUNT                  2
#define RM_HASH_LOOKUP_MAX_CNT                      2
#define RM_HASH_MAX_HASH_VECTOR_COUNT              12
#define RM_HASH_MAX_ENT_NUM_IN_NARROW_MODE         48
#define RM_HASH_MAX_NARROW_ENT_WORDS_SIZE           4
#define RM_HASH_HIT_CONTEXT_TILE_CNT               12
#define RM_HASH_HIT_PROFILE_ENTRY_CNT              32
#define RM_HASH_NARROW_ENTRY_LITD_NUM               3

/*
 *! \brief Number of base buckets for each bucket mode.
 */
#define RM_HASH_BMN_BASE_BKT_NUM                   1
#define RM_HASH_BM0_BASE_BKT_NUM                   1
#define RM_HASH_BM1_BASE_BKT_NUM                   2
#define RM_HASH_BM2_BASE_BKT_NUM                   4

#define RM_HASH_MULTI_INST_GRP               (1 << 7)
#define RM_HASH_MGRP_INST_MASK            (0x03 << 4)
#define RM_HASH_MGRP_NUM_MASK                   0x0F

#define RM_HASH_NODE_IN_USE                     0x01
#define RM_HASH_NODE_GLOBAL                     0x02
#define RM_HASH_NODE_PERPIPE                    0x04
#define RM_HASH_NODE_HASH_STORE                 0x08
#define RM_HASH_NODE_NARROWMODEHALF             0x10
#define RM_HASH_NODE_NARROWMODETHIRD            0x20


/*******************************************************************************
 * Typedefs
 */

/*!
 * \brief Information about a requested entry.
 *
 * This data structure describes the information about the request entry.
 */
typedef struct rm_hash_req_ent_info_s {
    /*! \brief Size of the requested entry in unit of base entry. */
    uint8_t e_size;

    /*! \brief Valid base bucket bitmap in SLB that the entry can reside. */
    uint8_t vbb_bmp;

    /*! \brief Bucket mode of the entry. */
    rm_hash_bm_t e_bm;
} rm_hash_req_ent_info_t;

/*
 *! \brief Bucket mode for a specific key type value
 *
 * This data structure describes the bucket mode for a specific key type.
 */
typedef struct rm_hash_key_type_attrib_s {
    /*! \brief Valid or not. */
    bool valid;

    /*! \brief Bucket mode for this key type. */
    rm_hash_bm_t bm;

    /*! \brief Narrow mode for this key type. */
    rm_hash_entry_narrow_mode_t enm;
} rm_hash_key_type_attrib_t;

/*
 *! \brief Bucket mode information for a bank.
 *
 * This data structure describes the bucket mode for a specific bank.
 */
typedef struct rm_hash_bank_key_type_attrib_s {
    /*! \brief Key attribute table SID. */
    const bcmdrd_sid_t *attr_sid;

    /*! \brief Bucket mode for a bank. */
    rm_hash_key_type_attrib_t attrib[RM_HASH_MAX_KEY_TYPE_COUNT];
} rm_hash_bank_key_type_attrib_t;

/*
 *! \brief SLB index information for a group of an entry.
 *
 * This data structure describes SLB index information for a group.
 */
typedef struct rm_hash_grp_slb_s {
    /*! \brief Number of SLB for a group. */
    uint8_t num_insts;

    /*! \brief SLB sequence number array for a group. */
    uint8_t slb_seq_list[RM_HASH_MAX_GRP_INST_COUNT];
} rm_hash_grp_slb_t;

/*
 * ! \brief Structure used for describing hash vector computation.
 *
 * This structure describe the hash vector information for a bank.
 */
typedef struct rm_hash_vector_info_s {
    /*! \brief Vector type. */
    bcmptm_rm_hash_vector_type_t type;

    /*! \brief Offset into hash vector. */
    uint16_t offset;

    /*! \brief Mask in the unit of hardware base bucket. */
    uint32_t mask;
} rm_hash_vector_info_t;

/*
 * ! \brief Structure used for describing valid SIDs for a bank.
 *
 * This structure is used for describing valid SIDs for a bank.
 */
typedef struct rm_hash_bank_sids_s {

    /*! \brief All the valid SIDs for a bank. */
    bcmdrd_sid_t view_sid[RM_HASH_MAX_ENT_SIZE + 1];
} rm_hash_bank_sids_t;

/*
 * ! \brief Enum used for describing the global or perpipe mode for a bank.
 *
 * This structure is used for describing the global or perpipe mode for a bank.
 */
typedef enum rm_hash_bank_mode_e{
    /*! \brief Uninitialized mode. */
    RM_HASH_BANK_MODE_NONE = 0,

    /*! \brief Global mode. */
    RM_HASH_BANK_MODE_GLOBAL = 1,

    /*! \brief Per pipe mode. */
    RM_HASH_BANK_MODE_PERPILE = 2,

    /*! \brief mixed mode. */
    RM_HASH_BANK_MODE_MIXED = 3,
} rm_hash_bank_mode_t;

/*
 * ! \brief Structure describing the attributes of a physical hash table.
 *
 * This data structure is defined on per physical hash table basis.
 * On some devices, each physical hash table has a separated structure
 * for it. Different table views of a hash table share the common structure
 * object.
 * On some devices, all the search banks (hash tables with different
 * views) share the common structure.
 */
typedef struct rm_hash_pt_info_s {
    /*! \brief All the supported bucket modes. */
    uint8_t bmf;

    /* \brief Flag indicating if pre-configured. */
    bool pre_config;

    /*! \brief Maximum bucket mode of the physical table. */
    rm_hash_bm_t t_bm;

    /* \brief Number of registered banks. */
    uint8_t num_rbanks;

    /* \brief Flag indicating if each bank has separated SID. */
    bool bank_separate_sid;

    /* \brief Flag indicating if there are some banks in mixed mode. */
    bool bank_mixed_mode;

    /* \brief Flag indicating if narrow mode is supported on this device.
     * Narrow mode is a per E-TILE attribute, and when it is enabled,
     * a base entry can accommodate two half width entries or three third
     * width entries.
     */
    bool narrow_mode_supported;

    /*! \brief Hash bank mode for a given physical hash table. */
    bcmptm_rm_hash_type_t hash_type[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Number of base entry in a base bucket. */
    uint8_t base_bkt_size[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Number of base entry in a base bucket. */
    uint8_t lbkt_size[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Physical table SID of the bank. */
    bcmdrd_sid_t rbank_sid[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief All the valid SIDs of the bank. */
    rm_hash_bank_sids_t sid_list[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Low power table SID of the bank. */
    const bcmdrd_sid_t *lp_sid[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Single view table SID of the bank. */
    const bcmdrd_sid_t *single_view_sid[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Narrow mode control information. */
    const bcmptm_rm_hash_narrow_mode_info_t *nm_info[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Flag indicating if bank in narrow mode. */
    bool bank_in_narrow_mode[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Number of base bucket for each bank. */
    uint32_t base_bkt_cnt[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Robust hash configuration for each bank. */
    void *rhash_ctrl[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Key attribute for each bank. */
    rm_hash_bank_key_type_attrib_t key_attrib[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Vector info of each bank. */
    rm_hash_vector_info_t vec_info[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Number of slbs for each bank. */
    uint32_t rbank_num_slbs[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Base slb of each bank. */
    uint32_t rbank_slb_base[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Size of the slb in the unit of base entry. */
    uint8_t slb_size;

    /*! \brief Maximum number of entries in an SLB. */
    uint8_t slb_num_entries;

    /*! \brief Number of slb for each entry. */
    uint8_t max_num_mapping_banks;

    /*! \brief Mode of the bank. */
    rm_hash_bank_mode_t bank_mode[RM_HASH_MAX_SRCH_BANK_COUNT];

    /*! \brief Pointer to the per bank bucket state. */
    rm_hash_slb_state_t *bkt_state[RM_HASH_MAX_SRCH_BANK_COUNT][RM_HASH_MAX_TBL_INST_COUNT];

    /*! \brief Pointer to the per bank entry node. */
    rm_hash_ent_node_t *ent_node[RM_HASH_MAX_SRCH_BANK_COUNT][RM_HASH_MAX_TBL_INST_COUNT];

    /*! \brief Pointer to the per bank entry SLB info. */
    rm_hash_ent_slb_info_t *slb_info[RM_HASH_MAX_SRCH_BANK_COUNT][RM_HASH_MAX_TBL_INST_COUNT];

    /*! \brief Pointer to the per bank narrow mode entry bmp. */
    rm_hash_nme_bmp_t *nme_bmp[RM_HASH_MAX_SRCH_BANK_COUNT][RM_HASH_MAX_TBL_INST_COUNT];
} rm_hash_pt_info_t;

/*
 * ! \brief Information about physical tables a LT can span horizontally.
 *
 * A LT can span one or multiple physical hash tables. On some older devices,
 * one LT can only map to one physical hash table which may consist of multiple
 * banks horizontally. On some newer devices, one LT can map to multiple
 * physical hash tables, each hash table only have one bank.
 */
typedef struct rm_hash_fmt_info_s {
    /* \brief Key format of the LT. */
    const bcmptm_rm_hash_key_format_t *key_format;

    /* \brief SID of a rsrc node this LT will span. */
    bcmdrd_sid_t sid;

    /* \brief pt_info used for describing the pt on which the LT
       is based. */
    rm_hash_pt_info_t *pt_info;

    /* \brief The number of enabled banks in the pt info which have labeled
       with sid. */
    uint8_t num_en_rbanks;

    /* \brief The list of enabled banks in the pt info which have labeled
       with sid. */
    uint8_t en_rbank_list[RM_HASH_SHR_MAX_BANK_COUNT];

    /* \brief The number of banks in the pt info which have labeled
       with sid. */
    uint8_t num_rbanks;

    /* \brief The list of banks in the pt info which have labeled
       with sid. */
    uint8_t rbank_list[RM_HASH_SHR_MAX_BANK_COUNT];
} rm_hash_fmt_info_t;

/*
 * ! \brief Information about physical tables a LT can span vertically.
 *
 * For some hash table base CTH LT, a LT can map onto multiple views of
 * a hash table.
 */
typedef struct rm_hash_grp_info_s {
    /* \brief Number of formats in a group. */
    uint8_t num_fmts;

    /* \brief List of all the valid formats in a group. */
    rm_hash_fmt_info_t fmt_info[RM_HASH_MAX_GRP_FMT_COUNT];
} rm_hash_grp_info_t;

/*
 * ! \brief Information about runtime enabled mapping for an LT.
 *
 * Some hash LTs may support multiple lookups, and thus have multiple
 * instances in a map.
 */
typedef struct rm_hash_map_info_s {
    /* \brief Number of instances in an enabled mapping. */
    uint8_t num_insts;

    /* \brief List of the valid groups for an enabled mapping. */
    uint8_t grp_idx[RM_HASH_MAX_GRP_FMT_COUNT];

    /* \brief List of the valid formats for an enabled mapping. */
    uint8_t fmt_idx[RM_HASH_MAX_GRP_FMT_COUNT];

    /* \brief List of key formats. */
    const bcmptm_rm_hash_key_format_t *key_format[RM_HASH_MAX_GRP_FMT_COUNT];

    /* \brief List of hit context for this mapping. */
    bcmptm_rm_hash_hit_context_t hit_context[RM_HASH_MAX_GRP_FMT_COUNT];
} rm_hash_map_info_t;

/*
 * ! \brief Information about hash LT flex counter related configuration.
 *
 * For LTs which support HIT bit or flex counter, this structure is used for
 * recording corresponding configuration registered by flex counter module,
 * for example whether the entry move event will be reported, which callback
 * function is used for compute the flex counter offset, etc.
 */
typedef struct rm_hash_lt_flex_ctr_info_s {
    /*! \brief Number of lookups. */
    uint8_t num_lookups;

    /*! \brief Indicating if entry move event reporting has been enabled. */
    bool lookup_entry_move_reporting[RM_HASH_LOOKUP_MAX_CNT];

    /*! \brief Callback function to calculate entry index. */
    bcmptm_rm_hash_entry_index_cb_t entry_index_cb;

    /*! \brief Indicating if move event will be reported for entry insert. */
    bool report_for_insert;

    /*! \brief Number of enabled hit context tile IDs for this LT. */
    uint8_t num_context;

    /*! \brief Array of enabled hit context tile id for this LT. */
    uint8_t tile_id_list[RM_HASH_HIT_CONTEXT_TILE_CNT];

    /*! \brief Flex counter context info. */
    void *context;
} rm_hash_lt_flex_ctr_info_t;

/*
 * ! \brief All the required information for a ltid for a hash operation.
 *
 * This structure is defined on per ltid basis, so for each ltid, there
 * will be a structure for it. In additional, its field value will be set
 * during hash resource manager initialization, and will not change during
 * runtime.
 */
typedef struct rm_hash_lt_ctrl_s {
    /*! \brief LTID used for a key for searching. */
    bcmltd_sid_t ltid;

    /*! \brief bcmptm_rm_hash_lt_info_t object for a ltid. */
    bcmptm_rm_hash_lt_info_t lt_info;

    /*! \brief If key field mapping is different between search banks. */
    bool key_field_exchange;

    /*! \brief If data field mapping is different between search banks. */
    bool data_field_exchange;

    /*! \brief Flex counter information for the LT. */
    rm_hash_lt_flex_ctr_info_t flex_ctr_info;

    /*! \brief Number of resource groups that the LT maps onto. */
    uint8_t num_grps;

    /*! \brief List of all the resource groups that the LT maps onto. */
    rm_hash_grp_info_t grp_info[RM_HASH_MAX_RSRC_GRP_COUNT];

    /*! \brief Number of runtime enabled mappings for the LT. */
    uint8_t num_maps;

    /*! \brief List of all the enabled mappings for the LT.*/
    rm_hash_map_info_t map_info[RM_HASH_MAX_RSRC_MAP_COUNT];

    /* \brief List of mapping index of the LT for free slot searching . */
    uint8_t srch_map_idx[RM_HASH_MAX_RSRC_MAP_COUNT];

    /*! \brief Number of all the valid formats for the LT. */
    uint8_t num_fmts;

    /* \brief List of all the valid formats for the LT. */
    rm_hash_fmt_info_t fmt_info[RM_HASH_MAX_RSRC_MAP_COUNT];

    /* \brief List of all the group index for each valid format. */
    uint8_t fmt_grp_idx[RM_HASH_MAX_RSRC_MAP_COUNT];

    /*! \brief Size of each group. */
    uint32_t grp_size[RM_HASH_MAX_RSRC_GRP_COUNT];

    /*! \brief Hash reordering move depth for a ltid. */
    uint8_t move_depth;

    /*! \brief If LT is operated in per pipe mode. */
    bool is_per_pipe;

    /*! \brief If LT is in narrow mode. */
    rm_hash_entry_narrow_mode_t e_nm;

    /*! \brief Traversal info for this LT */
    void *travs_info[RM_HASH_MAX_RSRC_GRP_COUNT];
} rm_hash_lt_ctrl_t;

/*
 *! \brief Information about hash vector for a robust hash K+N module.
 */
typedef struct rm_hash_glb_hash_vector_s {
    /*! Robust hash remap table SID. */
    bcmdrd_sid_t robust_remap_sid;

    /*! Flag indicating if hash vector has been available */
    bool valid;

    /*! Hash vector corresponding to this robust K+N module. */
    uint64_t vec;

    /*! Full key. */
    uint32_t full_key[RM_HASH_MAX_KEY_WORDS];
} rm_hash_glb_hash_vector_t;

/*
 * ! \brief Information about an entry search operation on an instance.
 *
 * This structure describes the information about an entry search operation.
 * For example the banks on which the search will be performed.
 */
typedef struct rm_hash_inst_srch_info_s {
    /*! \brief Format information. */
    rm_hash_fmt_info_t *fmt_info;

    /*! \brief Entry bucket mode. */
    rm_hash_bm_t e_bm;

    /*! \brief Entry narrow mode. */
    rm_hash_entry_narrow_mode_t e_nm;

    /*! \brief Hash vector of the node. */
    rm_hash_glb_hash_vector_t *hash_vector;

    /*! \brief Hardware logical bucket list. */
    uint32_t hlb_list[RM_HASH_SHR_MAX_BANK_COUNT];

    /*! \brief Software logical bucket list. */
    rm_hash_ent_slb_info_t slb_info_list[RM_HASH_SHR_MAX_BANK_COUNT];

    /*! \brief Enable/Disable hash store. */
    bool hash_store_en;

    /*! \brief Calculated hash store value. */
    uint32_t hash_store_val;

    /*! \brief Hash store key fields. */
    bcmlrd_hw_field_list_t *hash_store_key_fields;

    /*! LT ID. */
    bcmltd_sid_t ltid;
} rm_hash_inst_srch_info_t;

/*
 * ! \brief Information about an entry search operation in an enabled mapping.
 *
 * This structure describes the information about an entry search operation.
 * For example the number of instances to be searched in a mapping.
 */
typedef struct rm_hash_map_srch_info_s {
    /*! \brief Number of instances on which the search to be performed. */
    uint8_t num_insts;

    /*! \brief List of srch info for all the instances to be searched. */
    rm_hash_inst_srch_info_t inst_srch_info[RM_HASH_MAX_GRP_INST_COUNT];
} rm_hash_map_srch_info_t;

/*
 * ! \brief Information about an entry search operation on all the valid mapping.
 *
 * This structure describes the information about an entry search operation.
 * For example the number of all the enabled mappings for a LT.
 */
typedef struct rm_hash_glb_srch_info_s {
    /* \brief List of all the search info for all the mappings. */
    rm_hash_map_srch_info_t map_srch_info[RM_HASH_MAX_RSRC_GRP_COUNT];

    /*! \brief Number of hash vectors for this LT. */
    uint8_t num_vecs;

    /*! \brief Hash vector list for this LT. */
    rm_hash_glb_hash_vector_t vec_list[RM_HASH_MAX_HASH_VECTOR_COUNT];

    /* \brief Next object. */
    struct rm_hash_glb_srch_info_s *next;
} rm_hash_glb_srch_info_t;

/*
 *! \brief Information of the narrow mode entry.
 */
typedef struct rm_hash_nme_info_s {
    /*! \brief Narrow mode attribute of the entry. */
    rm_hash_entry_narrow_mode_t e_nm;

    /*! \brief Location within the base entry. */
    uint8_t nm_loc;

    /*! \brief Ltid of the entry. */
    uint32_t ltid;
} rm_hash_nme_info_t;

/*
 * ! \brief Information about an entry search result on an instance.
 *
 * This structure describes the result about an entry search operation.
 * For example the banks on which the search will be performed.
 */
typedef struct rm_hash_inst_srch_result_s {
    /*! \brief Flag indicating if entry has been found or not */
    bool e_exist;

    /*! \brief View info of the matched entry. */
    const bcmptm_rm_hash_view_info_t *view_info;

    /*! \brief Key format info of the matched entry. */
    const bcmptm_rm_hash_key_format_t *key_fmt;

    /*! \brief SID of the found entry */
    bcmdrd_sid_t sid;

    /*! \brief Index of the found entry */
    uint32_t e_idx;

    /*! \brief Bank number. */
    uint8_t rbank;

    /*! \brief SLB of the found entry */
    uint32_t slb;

    /*! \brief Location of the found entry */
    rm_hash_ent_loc_t e_loc;

    /*! \brief Location of the found narrow entry. */
    rm_hash_nme_info_t nme_info;

    /*! \brief Data format id from PTcache */
    uint16_t ldtype_val;

    /*! \brief Buffer containing the entry content */
    uint32_t e_words[BCMPTM_MAX_PT_ENTRY_WORDS];
} rm_hash_inst_srch_result_t;

/*
 *! \brief Information about an entry search result on a mapping.
 */
typedef struct rm_hash_map_srch_result_s {
    /*! \brief Flag indicating if the entry has been found or not */
    bool e_exist;

    /*! \brief Mapping on which the entry was found. */
    uint8_t map_idx;

    /*! \brief Number of instances of the mapping the search performed. */
    uint8_t num_insts;

    /*! \brief List of the results for each mapping instance. */
    rm_hash_inst_srch_result_t inst_srch_result[RM_HASH_MAX_GRP_INST_COUNT];

    /*! \brief Pointer to next element */
    struct rm_hash_map_srch_result_s *next;
} rm_hash_map_srch_result_t;

/*
 * ! \brief Information about an entry operation context.
 *
 * This structure describes the information about an entry operation context.
 */
typedef struct rm_hash_req_context_s {
    /*! \brief Flags passed from LTM & PTM. */
    uint64_t req_flags;

    /*! \brief Dynamic information of the request. */
    bcmbd_pt_dyn_info_t *pt_dyn_info;

    /*! \brief LTID information of the request. */
    bcmltd_sid_t req_ltid;

    /*! \brief Request information. */
    bcmptm_rm_hash_req_t *req_info;
} rm_hash_req_context_t;

/*
 *! \brief Attribute of the entry for which the free space to be searched.
 *
 * This data structure describes the attribute of the requested entry.
 */
typedef struct rm_hash_req_ent_attr_s {
    /*! \brief Size of the entry. */
    uint8_t e_size;

    /*! \brief Bucket mode of the entry. */
    rm_hash_bm_t e_bm;

    /*! \brief Narrow mode attribute of the entry. */
    rm_hash_entry_narrow_mode_t e_nm;
} rm_hash_req_ent_attr_t;

/*
 * ! \brief Information about free entry search.
 *
 * This structure describes the information about free entry search.
 */
typedef struct rm_hash_free_slot_info_s {
    /*! \brief Flag indicating if free space has been found. */
    bool found;

    /*! \brief Number of instances. */
    uint8_t num_inst;

    /*! \brief Location of the free space. */
    rm_hash_ent_loc_t e_loc_list[RM_HASH_MAX_GRP_INST_COUNT];

    /*! \brief List of slb info index. */
    uint8_t slb_info_idx_list[RM_HASH_MAX_GRP_INST_COUNT];

    /*! \brief The offset of free narrow entry slot within the base entry. */
    uint8_t nm_offset_list[RM_HASH_MAX_GRP_INST_COUNT];
} rm_hash_free_slot_info_t;

/*
 * ! \brief Structure to hold hash table entry.
 *
 * This structure describes the information about an entry buffer.
 */
typedef struct rm_hash_entry_buf_s {
    /*! \brief Buffer containing the entry content */
    uint32_t e_words[BCMPTM_MAX_PT_ENTRY_WORDS];

    struct rm_hash_entry_buf_s *next;
} rm_hash_entry_buf_t;

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Update transaction record when bcmptm_rm_hash_req is called.
 *
 * \param [in] unit Unit number.
 * \param [in] cseq_id Current transaction id.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_trans_ent_req_cmd(int unit, uint32_t cseq_id);

/*!
 * \brief Update low power table of a hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] pt_dyn_info Pointer to bcmbd_pt_dyn_info_t structure.
 * \param [in] single_sid Single width hash table view.
 * \param [in] key_attr_sid Key attribute table SID.
 * \param [in] lp_sid Low power table SID.
 * \param [in] bucket_mode Bucket mode of the entry.
 * \param [in] base_width Width of the entry.
 * \param [in] remote_bank_depth Base bucket count of the hash table.
 * \param [in] mem_offset Index of the entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_lp_ent_update(int unit,
                             bcmbd_pt_dyn_info_t *pt_dyn_info,
                             bcmdrd_sid_t single_sid,
                             bcmdrd_sid_t key_attr_sid,
                             bcmdrd_sid_t lp_sid,
                             rm_hash_bm_t bucket_mode,
                             uint8_t base_width,
                             uint32_t remote_bank_depth,
                             uint32_t mem_offset);

/*!
 * \brief Get flex counter offset value.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pionter to rm_hash_lt_ctrl_t structure.
 * \param [in] sid Physical table symbol id.
 * \param [in] tbl_inst Table instance.
 * \param [in] ent_index Entry index.
 * \param [in] ent_size Hash LT entry size.
 * \param [out] ctr_offset Buffer to hold the flex counter offset.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_flex_ctr_offset_get(int unit,
                                   rm_hash_lt_ctrl_t *lt_ctrl,
                                   bcmdrd_sid_t sid,
                                   int tbl_inst,
                                   uint32_t ent_index,
                                   uint8_t ent_size,
                                   uint32_t *ctr_offset);

/*!
 * \brief Set entry hit index information.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_ctrl Pointer to rm_hash_lt_ctrl_t structure.
 * \param [in] tbl_inst Table instance.
 * \param [in] map_sr Pointer to rm_hash_map_srch_result_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_ent_hit_index_info_set(int unit,
                                      rm_hash_lt_ctrl_t *lt_ctrl,
                                      int tbl_inst,
                                      rm_hash_map_srch_result_t *map_sr);

/*!
 * \brief Construct and report an entry move event.
 *
 * \param [in] unit Unit number.
 * \param [in] ltid Logical table ID.
 * \param [in] tbl_inst Physical table pipe instance.
 * \param [in] num_lookups Number of lookup.
 * \param [in] context Flex counter context info.
 * \param [in] dst_ctr_index Array containing hit index.
 * \param [in] src_ctr_index Array containing hit index.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_move_event_notify(int unit,
                                 bcmltd_sid_t ltid,
                                 int tbl_inst,
                                 uint8_t num_lookups,
                                 void *context,
                                 uint32_t *dst_ctr_index,
                                 uint32_t *src_ctr_index);

/*!
 * \brief Get the tile and bank hit information for a hash table.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Hash table symbol id.
 * \param [out] tbl_info Buffer to record the hit information.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_rm_hash_hit_tbl_info_get(int unit,
                                bcmdrd_sid_t sid,
                                const bcmptm_rm_hash_hit_tbl_info_t **tbl_info);

#endif /* BCMPTM_RM_HASH_H */
