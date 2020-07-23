/*! \file bcmfp_stage_internal.h
 *
 * Defines, Enums and Structures to represent FP stages
 * (IFP/EFP/VFP/EM) related information.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMFP_STAGE_INTERNAL_H
#define BCMFP_STAGE_INTERNAL_H

#include <bcmdrd/bcmdrd_types.h>
#include <bcmfp/bcmfp_group_internal.h>
#include <bcmfp/bcmfp_types_internal.h>
#include <bcmfp/bcmfp_qual_cfg_internal.h>
#include <bcmfp/bcmfp_presel_qual_cfg_internal.h>
#include <bcmfp/bcmfp_action_cfg_internal.h>
#include <bcmfp/bcmfp_presel_action_cfg_internal.h>
#include <bcmfp/bcmfp_em_internal.h>
#include <bcmfp/bcmfp_keygen_api.h>
#include <bcmfp/bcmfp_pdd.h>
#include <bcmfp/bcmfp_cth_pdd.h>
#include <bcmcth/bcmcth_meter_util.h>
#include <bcmimm/bcmimm_int_comp.h>
#include <bcmfp/bcmfp_tbl_internal.h>
#include <bcmfp/bcmfp_cth_range_check.h>
#include <bcmfp/bcmfp_cth_ctr.h>
#include <bcmfp/bcmfp_cth_common.h>
#include <bcmfp/bcmfp_trie_mgmt.h>
#include <bcmfp/bcmfp_compression_internal.h>

typedef enum bcmfp_stage_flag_s {

    BCMFP_STAGE_POLICY_SPAN_SINGLE_SLICE,

    BCMFP_STAGE_KEY_TYPE_PRESEL,

    BCMFP_STAGE_KEY_TYPE_SELCODE,

    BCMFP_STAGE_KEY_TYPE_FIXED,

    BCMFP_STAGE_POLICY_TYPE_PDD,

    /* PDD profile compes from main TCAM policy. */
    BCMFP_STAGE_PDD_PER_ENTRY,

    BCMFP_STAGE_KEYGEN_PROFILE,

    BCMFP_STAGE_ACTION_PROFILE,

    BCMFP_STAGE_ACTION_QOS_PROFILE,

    BCMFP_STAGE_ENTRY_TYPE_HASH,

    BCMFP_STAGE_DEFAULT_POLICY,

    BCMFP_STAGE_MODE_HALF_NOT_SUPPORTED,

    BCMFP_STAGE_INPORTS_SUPPORT,

    BCMFP_STAGE_ACTION_RESOLUTION_SBR,

    /* SBR profile compes from main TCAM policy. */
    BCMFP_STAGE_SBR_PER_ENTRY,
    /*
     * Presel TCAM key format is not fixed
     * and user has to choose the qualifiers
     * to be part of key.
     */
    BCMFP_STAGE_PRESEL_KEY_DYNAMIC,

    BCMFP_STAGE_NO_METER_SUPPORT,

    /*
     * Flag to indicate in which direction
     * the PDD format is read  by decoder.
     * Encoding in BCMFP needs to be done
     * accordingly.
     */
    BCMFP_STAGE_PDD_READ_FROM_MSB,

    BCMFP_STAGE_PDD_READ_FROM_LSB,

    BCMFP_STAGE_LEGACY_CTR_SUPPORT,

    /* Flag to indicate flex counter support. */
    BCMFP_STAGE_FLEX_CTR_SUPPORT,

    /*
     * FP stage suppors colored ations.
     * Colored action will be applied on
     * packet only if packets color matches
     * with configured color. Color of the
     * color of incoming packet or color
     * assigned by meters if meters are
     * attached to FP entry.
     */
    BCMFP_STAGE_SUPPORTS_COLORED_ACTIONS,

    /*
     * FP stage has PDD profiles common to
     * all slices.
     */
    BCMFP_STAGE_PDD_PROFILES_GLOBAL,

    /*
     * Policy and PDD bitmap information
     * needs to be set in last slice of
     * wider groups.
     */
    BCMFP_STAGE_PDD_INFO_ONLY_IN_LAST_SLICE,

    /*
     * Policy and PDD bitmap information
     * needs to be set in primary slice
     * of wider groups.
     */
    BCMFP_STAGE_PDD_INFO_ONLY_IN_FIRST_SLICE,

    /*
     * Flag to indicate enhanced flex counter(FC_2)
     * support.
     */
    BCMFP_STAGE_EFLEX_CTR_SUPPORT,

    /*!
     * Different map segments maintained in
     * stage operational info structure are
     * indexed by hash value.
     */
    BCMFP_STAGE_IDP_MAP_TYPE_HASH,

    /*!
     * Different map segments maintained in
     * stage operational info structure are
     * indexed by hash value of FP template
     * LT IDs.
     */
    BCMFP_STAGE_IDP_MAP_TYPE_ARRAY,

    /*
     * Flag to indicate flexible key support.
     */
    BCMFP_STAGE_HASH_FLEX_KEY_DATA,

    /*
     * FP stage has SBR profiles common to
     * all slices.
     */
    BCMFP_STAGE_SBR_PROFILES_GLOBAL,

    /*
     * FP stage has PSG  profiles common to
     * all slices.
     */
    BCMFP_STAGE_PSG_PROFILES_GLOBAL,

    /*
     * UFT banks can be dynamicaly assigned
     * for this FP stage.
     */
    BCMFP_STAGE_UFT_BANKS_DYNAMIC,

    /*! List compression is supported. */
    BCMFP_STAGE_LIST_COMPRESSION,

    /* Flow tracker stage. */
    BCMFP_STAGE_HW_FLOW_TRACKER,

    /*! IPBM support for HW bug. */
    BCMFP_STAGE_IPBM_SUPPORT,

    /*! IPBM parts support. */
    BCMFP_STAGE_IPBM_PARTS_SUPPORT,

    /*! UFT MGR Support for SBR entry allocation */
    BCMFP_STAGE_SBR_UFTMGR_SUPPORT,

    /*!
     * Set if the Policy table entries
     * support multiple views
     */
    BCMFP_STAGE_POLICY_MULTI_VIEW,

    BCMFP_STAGE_FLAGS_COUNT,

} bcmfp_stage_flag_t;

typedef struct bcmfp_stage_flags_s {
    SHR_BITDCL w[SHRi_BITDCLSIZE(BCMFP_STAGE_FLAGS_COUNT)];
} bcmfp_stage_flags_t;

#define BCMFP_STAGE_FLAGS_INIT(stage)  \
    sal_memset(stage->flags.w, 0, sizeof(bcmfp_stage_flags_t))

#define BCMFP_STAGE_FLAGS_ADD(stage, flag) \
    SHR_BITSET((stage->flags.w), (flag))

#define BCMFP_STAGE_FLAGS_REMOVE(stage, flag) \
    SHR_BITCLR((stage->flags.w), (flag))

#define BCMFP_STAGE_FLAGS_TEST(stage, flag) \
    SHR_BITGET((stage->flags.w), (flag))

/*
 * Maximum number of Groups supported in IFP
 * across all supported devices.
 */
#define BCMFP_GROUP_MAX_IFP (128)

/*
 * Maximum number of PDD sections a stage supported
 * across all supported devices.
 */
#define BCMFP_PDD_MAX_SECTIONS  (4)

/* BCMFP stage ID String. */
#define BCMFP_STAGE_STRINGS \
{ \
    "BCMFP_STAGE_INGRESS", \
    "BCMFP_STAGE_EGRESS", \
    "BCMFP_STAGE_LOOKUP", \
    "BCMFP_STAGE_EXACT_MATCH", \
    "BCMFP_STAGE_FLOW_TRACKER", \
}

typedef struct bcmfp_qset_aset_size_info_s {

  uint32_t data_size[BCMFP_GROUP_MODE_COUNT];

  /*
   * the index at which MSB of the data portion of the entry
   * starts in MAIN TCAM/Hash BANKs for each of the modes.
   * Used mainly for EM
   */
  uint32_t data_start_offset[BCMFP_GROUP_MODE_COUNT];

  uint32_t key_size[BCMFP_GROUP_MODE_COUNT];

  uint32_t key_data_size[BCMFP_GROUP_MODE_COUNT];

  uint32_t key_part_size[BCMFP_GROUP_MODE_COUNT][4];

  uint32_t em_mode[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t policy_data_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t action_prof_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t qos_prof_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t em_class_id_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t data_fid[BCMFP_GROUP_MODE_COUNT];

  bcmdrd_fid_t key_parts_fid[BCMFP_GROUP_MODE_COUNT][4];

} bcmfp_qset_aset_size_info_t;

typedef struct bcmfp_stage_hw_entry_info_s {

    bool aggr_view;

    bool presel_aggr_view;

    bcmdrd_sid_t sid;

    bcmdrd_sid_t sid_data_only;

    bcmdrd_sid_t presel_sid;

    bcmdrd_sid_t presel_sid_data_only;

    bcmdrd_fid_t key_fid;

    bcmdrd_fid_t mask_fid;

    bcmdrd_fid_t valid_fid;

    uint32_t max_entries;

    /*
     * Max number of groups that can be created in
     * a given mode with different priorities.
     */
    uint8_t max_non_conflicting_groups;

} bcmfp_stage_hw_entry_info_t;

typedef struct bcmfp_stage_hw_field_info_s {

   uint8_t num_offsets;

   uint16_t *offset;

   uint16_t *width;

} bcmfp_stage_hw_field_info_t;

typedef struct bcmfp_stage_misc_info_s {

    uint8_t num_group_action_res_ids;

    uint16_t num_groups;

    uint16_t num_keygen_prof;

    uint16_t num_qos_prof;

    bcmfp_group_mode_bmp_t group_mode_bmp;

    /* Maximum number of presel groups a stage supports. */
    uint16_t num_presel_groups;

    /* Total number of PDD profiles supported per stage. */
    uint16_t num_pdd_profiles;

    /* Total number of SBR profiles supported per stage. */
    uint16_t num_sbr_profiles;

    /* Total number of exact match key type profiles. */
    uint16_t num_em_key_attrib_profile;

    /* Maximum number of banks that can be assigned to FP EM. */
    uint16_t max_em_banks;

    /* Default Policy data width */
    uint8_t default_policy_data_width;
    /*
     * Raw policy offset for PDD data when
     * PDD profile is attached to FP entry and
     * SBR profile attached to FP Group
     */
    uint8_t per_entry_pdd_raw_data_offset;

    /*
     * Raw policy offset for PDD data when
     * SBR profile is attached to FP entry and
     * PDD profile attached to FP Group
     */
    uint8_t per_entry_sbr_raw_data_offset;

    /*
     * Raw policy offset for PDD data when
     * SBR profile and PDD profile are attached
     * to FP entry
     */
    uint8_t per_entry_pdd_sbr_raw_data_offset;

    /*
     * Raw policy offset for PDD data when
     * SBR profile and PDD profile are attached
     * to FP Group
     */
    uint8_t per_group_pdd_sbr_raw_data_offset;

    /* Size of the SBR physical table index. */
    uint16_t sbr_index_size;

    /* Size of the PDD physical table index. */
    uint16_t pdd_index_size;

    bcmdrd_sid_t egress_fp_counter_sid;

    bcmdrd_sid_t em_key_attribute_sid;

    /* HW field information. */
    bcmfp_stage_hw_field_info_t *colored_actions_enable;

    /*!
     * Bitmap of stage ids with which this stage id is sharing
     * action resoultion id space.
     */
    uint16_t shared_action_res_id_stage_id_bmp;

} bcmfp_stage_misc_info_t;

/* SBR HW information. */
typedef struct bcmfp_sbr_hw_info_s {

    /*
     * Number of SBR profile SIDs used to save
     * priorities of all actions in a group/entry.
     */
    uint8_t sbr_profile_sid_columns;

    /*
     * Offset and widths of priority field in each
     * SBR profile SID column.
     */
    bcmfp_stage_hw_field_info_t *priority_field_info;

    /*
     * Number of SBR table SIDs.
     */
    uint8_t num_sbr_sids;

    /*
     * List of SBR table SIDs.
     */
    bcmdrd_sid_t *sbr_sid;

    /*
     * The max sbr index that can be used for a stage
     * This is used for EM and EM_FT where only half of
     * the sbr entries can be used. The upper half and
     * lower half of the sbr profile table for EM and
     * EM_FT need to be mirrored.
     */
    int max_sbr_idx;
    /*
     *The min sbr index that can be used for a stage
     */
    int min_sbr_idx;

} bcmfp_sbr_hw_info_t;

/*!
 * Maximum possible number of HIT index profiles per stage across all
 * FP stages of all devices.
 */
#define BCMFP_HIT_IDX_PROFILES_MAX 0x8

/* HIT Context HW Info structre */
typedef struct bcmfp_hit_context_s {

    /* bit position of the <stage>_hw_index logical field in PDD Bus */
    int8_t botp_value;

    /* Number of HIT_INDEX_PROFILE tbls to be programmed for a Stage */
    uint8_t hit_idx_profiles;

    /* The SIDs of the corresponding HIT_INDEX_PROFILE tbls */
    bcmdrd_sid_t hit_idx_profile_tbls[BCMFP_HIT_IDX_PROFILES_MAX];
} bcmfp_hit_context_t;

/*!
 * Maximum number of SID rows required to repeat a single logical
 * entry across all devices.
 */
#define BCMFP_SID_ROW_MAX 0x4

/*!
 * Maximum number of SIDs required to write a single logical
 * entry across all devices.
 */
#define BCMFP_SID_COL_MAX 0x2

/*!
 * Information to manage a particular resource as a profile table
 * inorder to abstarct the hardware differences.
 */
typedef struct bcmfp_profile_hw_info_s {

    /*
     * Number of physical SIDs required to program a
     * single profile entry.
     */
    uint8_t columns;

    /*
     * Number of physical SID rows. SIDs in all these
     * rows will have same data.
     */
    uint8_t rows;

    /* Physical SIDs of all rows and columns. */
    bcmdrd_sid_t sids[BCMFP_SID_ROW_MAX][BCMFP_SID_COL_MAX];

} bcmfp_profile_hw_info_t;

/* All possible resources to be managed as profile tables. */
typedef struct bcmfp_profiles_hw_info_s {

    /* Hardware information of SBR profile. */
    bcmfp_profile_hw_info_t *sbr_profile_hw_info;

} bcmfp_profiles_hw_info_t;

typedef struct bcmfp_stage_s {

    /*! Unique BCMFP stage ID */
    bcmfp_stage_id_t stage_id;

    bcmfp_stage_flags_t flags;

    uint8_t num_group_action_res_ids;

    uint8_t num_group_sbr_ids;

    /*!
     * Stage operational information has many map segements.
     * Those map segments can be indexed by either LT IDs
     * directly or hash values.If they are indexed by hash
     * values this will be set to BCMFP_IDP_MAP_MADULO.
     * Otherwise it is set to 1.
     */
    uint8_t idp_maps_modulo;

    bcmfp_group_mode_bmp_t group_mode_bmp;

    bcmfp_tbls_t tbls;

    bcmfp_qual_cfg_info_db_t *qual_cfg_info_db;

    bcmfp_qual_cfg_info_db_t *presel_qual_cfg_info_db;

    bcmfp_action_cfg_db_t *action_cfg_db;

    bcmfp_action_cfg_db_t *presel_action_cfg_db;

    bcmfp_stage_hw_entry_info_t *hw_entry_info[BCMFP_GROUP_MODE_COUNT];

    bcmfp_keygen_qual_cfg_info_db_t *kgn_qual_cfg_info_db;

    bcmfp_keygen_ext_cfg_db_t *kgn_ext_cfg_db_arr[BCMFP_GROUP_MODE_COUNT];

    bcmfp_keygen_qual_cfg_info_db_t *kgn_presel_qual_cfg_info_db;

    bcmfp_keygen_ext_cfg_db_t *kgn_presel_ext_cfg_db_arr[BCMFP_GROUP_MODE_COUNT];

    bcmfp_qset_aset_size_info_t *key_data_size_info;

    bcmfp_stage_misc_info_t *misc_info;

    bcmfp_pdd_hw_info_t *pdd_hw_info;

    bcmfp_sbr_hw_info_t *sbr_hw_info;

    bcmfp_shr_trie_md_t *compress_tries[BCMFP_COMPRESS_TYPES_MAX];

    bcmfp_compress_key_info_t *compress_key_info[BCMFP_COMPRESS_TYPES_MAX];

    /*!
     * IP address only compression status(TRUE/FALSE).
     * TRUE  - ALPM IP address only compression is enabled.
     *         i.e FP_CONFIG->FP_ING_COMP_XXX_ONLY is 1
     * FALSE - ALPM IP address only compression is disabled.
     *         i.e FP_CONFIG->FP_ING_COMP_XXX_ONLY is 0
     */
    bool compress_ip_addr_only[BCMFP_COMPRESS_TYPES_MAX];

    /*!
     * ALPM Compression status(TRUE/FALSE).
     * TRUE  - Auto ALPM compression is enabled.
     *         i.e FP_CONFIG->FP_ING_MANUAL_COMP is 0
     * FALSE - Auto ALPM compression is enabled.
     *         i.e FP_CONFIG->FP_ING_MANUAL_COMP is 1
     */
    bool compress_enabled;

    /*! LTD buffers used in IMM backend operations. */
    bcmfp_ltd_buffers_t imm_buffers;

    /* HIT Context HW Info - used in configuring the HIT_INDEX_PROFILE tbls */
    bcmfp_hit_context_t *hit_context;

    /*! List of resources needs to be managed as profiles. */
    bcmfp_profiles_hw_info_t profiles_hw_info;
} bcmfp_stage_t;

typedef int(*bcmfp_fn_qual_cfg_db_init_t)(int unit,
                                          bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_presel_qual_cfg_db_init_t)(int unit,
                                          bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_action_cfg_db_init_t)(int unit,
                                            bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_action_conflict_check_t)(int unit,
                                             bcmfp_action_t action1,
                                             bcmfp_action_t action2);
typedef int(*bcmfp_fn_presel_action_cfg_db_init_t)(int unit,
                                            bcmfp_stage_t *stage);
typedef int(*bcmfp_fn_ext_cfg_db_init_t)(int unit,
                                         bcmfp_stage_t *stage);

/*!
 * \brief Function pointer to build chip specific presel
 *  extractor database.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage pointer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM If stage is NULL.
 */
typedef int(*bcmfp_fn_presel_ext_cfg_db_init_t)(int unit,
                                                bcmfp_stage_t *stage);
/*!
 * \brief Build the presel policy to be written to HW.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] group_id BCMFP group identifier.
 * \param [in] opinfo Group operational information.
 * \param [out] edw Presel entry data words buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
typedef int(*bcmfp_fn_presel_edw_get_t)(int unit,
                                        bcmfp_stage_id_t stage_id,
                                        bcmfp_group_id_t group_id,
                                        bcmfp_group_oper_info_t *opinfo,
                                        uint32_t **edw);

/*!
 * \brief Build the presel groups key.
 *
 * \param [in] unit Logical device id
 * \param [in] stage_id BCMFP stage identifier.
 * \param [in] group_id BCMFP presel group identifier.
 * \param [out] presel_group_info Presel group key info buffer.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM if edw is NULL
 */
typedef int(*bcmfp_fn_presel_group_info_get_t)(int unit,
                                        bcmfp_stage_id_t stage_id,
                                        bcmfp_group_id_t group_id,
                                        uint32_t **presel_group_info);

typedef int(*bcmfp_fn_stage_init_t)(int unit,
                                    bcmfp_stage_t *stage);

typedef int(*bcmfp_fn_em_bank_sids_get_t)(int unit,
                                          uint32_t bank_sid,
                                          uint32_t *pt_info);

typedef int(*bcmfp_fn_qualifier_enum_set_t)(int unit,
                                            bcmfp_qualifier_t qaul,
                                            bool is_presel,
                                            int value,
                                            uint32_t *data,
                                            uint32_t *mask);

typedef int(*bcmfp_fn_src_class_mode_set_t)(int unit,
                                            uint32_t trans_id,
                                            bcmltd_sid_t sid,
                                            int pipe_id,
                                            uint32_t mode);

typedef int(*bcmfp_fn_qualifier_enum_get_t)(int unit,
                                            bcmfp_qualifier_t qaul,
                                            bool is_presel,
                                            uint32_t data,
                                            uint32_t mask,
                                            int *value);

typedef int(*bcmfp_fn_profiles_install_t)(int unit,
                                          uint32_t stage_id,
                                          uint32_t trans_id,
                                          uint32_t flags,
                                          bcmfp_group_id_t group_id,
                                          bcmfp_group_oper_info_t *opinfo,
                                          bcmfp_buffers_t *buffers);

typedef int(*bcmfp_fn_profiles_uninstall_t)(int unit,
                                            uint32_t stage_id,
                                            uint32_t trans_id,
                                            uint32_t flags,
                                            bcmfp_group_id_t group_id,
                                            bcmfp_group_oper_info_t *opinfo);

typedef int(*bcmfp_fn_profiles_copy_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_stage_t *stage);

typedef int(*bcmfp_fn_udf_qual_id_get_t)(int unit,
                                         uint8_t udf_chunk_id,
                                         bcmfp_qualifier_t *qual_id);
typedef int(*bcmfp_fn_group_selcode_key_get_t)(int unit,
                                               bcmfp_group_t *fg);
typedef int(*bcmfp_fn_entry_key_match_type_set_t)(int unit,
                                  bcmfp_stage_t *stage,
                                  bcmfp_group_id_t group_id,
                                  bcmfp_group_oper_info_t *opinfo,
                                  bcmfp_group_slice_mode_t slice_mode,
                                  bcmfp_group_type_t  port_pkt_type,
                                  uint32_t **ekw);
typedef int(*bcmfp_fn_group_action_profile_install_t)(int unit,
                                       uint32_t trans_id,
                                       bool default_entry,
                                       uint32_t *cont_bitmap,
                                       bcmfp_group_t *fg);
typedef int(*bcmfp_fn_group_action_profile_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bool default_entry,
                                       bcmfp_group_id_t group_id);
typedef int(*bcmfp_fn_entry_qos_profile_install_t)(int unit,
                                       uint32_t trans_id,
                                       uint32_t stage_id,
                                       bcmltd_sid_t ltid,
                                       int pipe_id,
                                       bool default_entry,
                                       bcmfp_group_id_t group_id,
                                       uint32_t *flat_policy_data,
                                       int *qos_prof_idx);
typedef int(*bcmfp_fn_entry_qos_profile_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       int qos_prof_idx);
typedef int(*bcmfp_fn_entry_qos_profile_id_get_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       uint32_t **edw,
                                       int *qos_prof_idx);

typedef int(*bcmfp_fn_buffers_customize_t)(int unit,
                                           uint32_t trans_id,
                                           uint32_t flags,
                                           bcmfp_stage_t *stage,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmfp_pdd_config_t *pdd_config,
                                           bcmfp_key_ids_t *key_ids,
                                           bcmfp_buffers_t *buffers);

typedef int(*bcmfp_fn_default_policy_install_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_t *fg,
                                       int qos_prof_idx,
                                       int em_class_id,
                                       uint32_t **edw);
typedef int(*bcmfp_fn_default_policy_uninstall_t)(int unit,
                                       uint32_t trans_id,
                                       bcmfp_group_id_t group_id,
                                       int default_policy_idx);
typedef int(*bcmfp_fn_pdd_profile_entry_get_t)(int unit,
                                       uint32_t trans_id,
                                       uint32_t stage_id,
                                       bcmfp_pdd_id_t pdd_id,
                                       bcmfp_group_id_t group_id,
                                       bcmfp_group_oper_info_t *opinfo,
                                       bool pdd_per_group,
                                       uint32_t **pdd_profile,
                                       bcmfp_pdd_config_t *pdd_config);
typedef int(*bcmfp_fn_range_check_grp_set_t)(int unit,
                                             uint32_t trans_id,
                                             bcmltd_sid_t sid,
                                             int pipe_id,
                                             int range_check_grp_id,
                                             bcmfp_range_check_id_bmp_t bmp);
typedef int(*bcmfp_fn_meter_actions_set_t)(int unit,
                                           bcmfp_group_id_t group_id,
                                           bcmfp_pdd_id_t pdd_id,
                                           bcmfp_group_oper_info_t *opinfo,
                                           bcmcth_meter_fp_policy_fields_t *meter_entry,
                                           uint32_t **edw);
typedef int(*bcmfp_fn_counter_actions_set_t)(int unit,
                                             bcmfp_stage_id_t stage_id,
                                             bcmfp_group_id_t group_id,
                                             bcmfp_pdd_id_t pdd_id,
                                             bcmfp_group_oper_info_t *opinfo,
                                             bcmfp_ctr_params_t *ctr_params,
                                             uint32_t **edw);
typedef int(*bcmfp_fn_counter_hw_idx_set_t)(int unit,
                                            bcmfp_stage_id_t stage_id,
                                            uint32_t ctr_id,
                                            uint32_t pipe_id,
                                            uint32_t pool_id,
                                            int color,
                                            bcmimm_entry_event_t event_reason);

/* Function Pointers */
typedef struct bcmfp_fn_ptrs_s {
    bcmfp_fn_qual_cfg_db_init_t           bcmfp_fn_qual_cfg_db_init;
    bcmfp_fn_presel_qual_cfg_db_init_t    bcmfp_fn_presel_qual_cfg_db_init;
    bcmfp_fn_action_cfg_db_init_t         bcmfp_fn_action_cfg_db_init;
    bcmfp_fn_action_conflict_check_t      bcmfp_fn_action_conflict_check;
    bcmfp_fn_presel_action_cfg_db_init_t  bcmfp_fn_presel_action_cfg_db_init;
    bcmfp_fn_ext_cfg_db_init_t            bcmfp_fn_ext_cfg_db_init;
    bcmfp_fn_presel_ext_cfg_db_init_t     bcmfp_fn_presel_ext_cfg_db_init;
    bcmfp_fn_presel_group_info_get_t      bcmfp_fn_presel_group_info_get;
    bcmfp_fn_stage_init_t                 bcmfp_fn_stage_init;
    bcmfp_fn_em_bank_sids_get_t           bcmfp_fn_em_bank_sids_get;
    bcmfp_fn_qualifier_enum_set_t         bcmfp_fn_qualifier_enum_set;
    bcmfp_fn_src_class_mode_set_t         bcmfp_fn_src_class_mode_set;
    bcmfp_fn_qualifier_enum_get_t         bcmfp_fn_qualifier_enum_get;
    bcmfp_fn_profiles_install_t           bcmfp_fn_profiles_install;
    bcmfp_fn_profiles_uninstall_t         bcmfp_fn_profiles_uninstall;
    bcmfp_fn_udf_qual_id_get_t            bcmfp_fn_udf_qual_id_get;
    bcmfp_fn_group_selcode_key_get_t      bcmfp_fn_group_selcode_key_get;
    bcmfp_fn_entry_key_match_type_set_t   bcmfp_fn_entry_key_match_type_set;
    bcmfp_fn_group_action_profile_install_t     bcmfp_fn_group_action_profile_install;
    bcmfp_fn_group_action_profile_uninstall_t   bcmfp_fn_group_action_profile_uninstall;
    bcmfp_fn_entry_qos_profile_install_t        bcmfp_fn_entry_qos_profile_install;
    bcmfp_fn_entry_qos_profile_uninstall_t      bcmfp_fn_entry_qos_profile_uninstall;
    bcmfp_fn_entry_qos_profile_id_get_t         bcmfp_fn_entry_qos_profile_id_get;
    bcmfp_fn_buffers_customize_t                bcmfp_fn_buffers_customize;
    bcmfp_fn_default_policy_install_t           bcmfp_fn_default_policy_install;
    bcmfp_fn_default_policy_uninstall_t         bcmfp_fn_default_policy_uninstall;
    bcmfp_fn_presel_edw_get_t                   bcmfp_fn_presel_edw_get;
    bcmfp_fn_pdd_profile_entry_get_t            bcmfp_fn_pdd_profile_entry_get;
    bcmfp_fn_range_check_grp_set_t              bcmfp_fn_range_check_grp_set;
    bcmfp_fn_meter_actions_set_t                bcmfp_fn_meter_actions_set;
    bcmfp_fn_counter_actions_set_t              bcmfp_fn_counter_actions_set;
    bcmfp_fn_counter_hw_idx_set_t               bcmfp_fn_counter_hw_idx_set;
    bcmfp_fn_profiles_copy_t           bcmfp_fn_profiles_copy;
} bcmfp_fn_ptrs_t;
/* BCMFP stage ID String. */
extern char* bcmfp_stage[BCMFP_STAGE_ID_COUNT];

#define BCMFP_STAGE_ID_TO_BCMPTM_STAGE_ID(bcmfp_stage_id, bcmptm_stage_id) \
    do {                                                                   \
        if (bcmfp_stage_id == BCMFP_STAGE_ID_INGRESS) {                    \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_INGRESS;          \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_EGRESS) {               \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_EGRESS;           \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_LOOKUP) {               \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_LOOKUP;           \
        } else if(bcmfp_stage_id == BCMFP_STAGE_ID_EXACT_MATCH) {          \
            bcmptm_stage_id = BCMPTM_RM_TCAM_FP_STAGE_ID_EXACT_MATCH;      \
        }                                                                  \
    } while(0)

/*!
 * \brief Set the idp map modulo to right value based on
 * stage idp map flag.
 *
 * \param [in] unit Logical device id
 * \param [in] stage BCMFP stage structure.
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 * \retval SHR_E_INTERNAL Stage has unknown IDP map flag
 */
extern int
bcmfp_stage_idp_maps_modulo_set(int unit,
                                bcmfp_stage_t *stage);

/*!
 * \brief get the entry limit
 *
 * \param [in] stage ltid
 * \param [in] transaction ID
 * \param [out] count Max entry limit
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_PARAM Invalid parameters
 */
extern int
bcmfp_ptm_stage_entry_limit_get(int unit,
                            uint32_t trans_id,
                            bcmltd_sid_t sid,
                            uint32_t *count);

#endif /* BCMFP_STAGE_INTERNAL_H */
