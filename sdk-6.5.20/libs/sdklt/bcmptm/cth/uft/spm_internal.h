/*! \file spm_internal.h
 *
 * Defines internal to SPM
 *
 * This file defines data structures of Strength Profile Manager CTH.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SPM_INTERNAL_H
#define SPM_INTERNAL_H

#include <shr/shr_error.h>
#include <shr/shr_debug.h>

/*!
 * \brief State of SPM entry logical index.
 */
typedef enum bcmptm_spm_entry_usage_e {
    /*! SPM profile entry is free to use. */
    BCMPTM_SPM_ENTRY_FREE = 0,

    /*! SPM profile entry is reserved by compiler. */
    BCMPTM_SPM_ENTRY_RESERVED,

    /*! SPLT has no active SP PTs mapped. */
    BCMPTM_SPM_ENTRY_UNMAPPED,

    /*! SPM profile entry was allocated by SDKLT internals. */
    BCMPTM_SPM_ENTRY_INUSE,

    /*! SPM profile entry is currently in use by a different SPLT. */
    BCMPTM_SPM_ENTRY_ALLOCATED_OTHER,

    /*! SPM profile entry is currently in use by this logical table. */
    BCMPTM_SPM_ENTRY_ALLOCATED_SELF,
} bcmptm_spm_entry_state_t;

/*!
 * \brief SPM PT entry list.
 *
 * Device resource parameters and entry buffer pointer to permit
 * physical operations on a specific PT entry.
 *
 * An array of these structures is used as working records during
 * SPM ops.  The entry_data buffers are maximum-size PTM entries,
 * each entry pointing to a section of a combined allocation block.
 */
typedef struct bcmptm_spm_pt_entry_s {
    /*! Physical Table id. */
    bcmdrd_sid_t pt_id;

    /*! Physical Table index for this entry. */
    uint32_t index;

    /*! This buffer contains the current entry for (pt_id, index). */
    bool entry_data_valid;

    /*!
     * The data in this buffer was modified during this SPM op.
     * It requires commit to HW.
     */
    bool entry_data_changed;

    /* Pointer to buffer to read/write PTM entry */
    uint32_t entry_data[BCMDRD_MAX_PT_WSIZE];
} bcmptm_spm_pt_entry_t;

/*!
 * \brief SPM LT information.
 *
 * Each logical table implemented with this CTH needs a record
 * of the fields and operating model.  This structure stores the
 * information on these elements during initialization.
 */
typedef struct bcmptm_spm_lt_fields_s {
    /*! Logical Table id. */
    bcmdrd_sid_t ltid;

    /*! Lookup Logical Table for this strength. */
    bcmdrd_sid_t ltid_lookup;

    /*! First encountered PT size. */
    uint32_t pt_size_1;

    /*! Second encountered PT size. */
    uint32_t pt_size_2;

    /*! PT offset for SBR multiple lookups. */
    uint32_t pt_offset;

    /*! PT effective index maximum for all mapped PTs. */
    uint32_t pt_index_max;

    /*! LT index maximum for LT allocation. */
    uint32_t lt_index_max;

    /*! LT entry_limit for LT allocation. */
    uint32_t lt_entry_limit;

    /*! Number of key fields. NB:  Must be 1 or 0. */
    uint32_t key_num;

    /*! Field ID for key field, if any. */
    uint32_t key_fid;

    /*! Number of value fields. */
    uint32_t value_num;

    /*! Field IDs for value fields. */
    uint32_t *value_fids;

    /*! Core of the L2P map for this LT. */
    const bcmlrd_hmf_instance_t *hmf_instance;
} bcmptm_spm_lt_fields_t;

/*!
 * \brief SPM all LT records.
 *
 * Top-level info for all LTs managed by SPM CTH.
 */
typedef struct bcmptm_spm_lt_record_s {
    /*
     * The root structure for all LRD info describing SPM L2P maps
     * in this variant and configuration.
     */
    const bcmptm_sbr_lt_str_pt_list_t *lt_str_pt_list;

    /*! Number of SPM PT entry records allocated in OP info. */
    uint32_t spm_pt_num;

    /*! Number of API field records allocated in OP info. */
    uint32_t api_field_num;

    /*! Number of logical tables managed. */
    uint32_t lt_num;

    /*! Field IDs for value fields. */
    bcmptm_spm_lt_fields_t lt_fields_info[];
} bcmptm_spm_lt_record_t;

/*!
 * \brief SPM PT op working information.
 *
 * PT entries with attributes + API fields for modification during
 * a SPM operation.
 *
 * These structures are working records during SPM ops, used to
 * assemble device table modifications and API field coherence.
 */
typedef struct bcmptm_spm_op_info_s {
    /*!
     * The LT fields information for the LT of this op.
     */
    bcmptm_spm_lt_fields_t *lt_fields;

    /*! Number of SPM PT entry records in use for this op. */
    uint32_t spm_pt_used;

    /*! Array of spm_pt_num SPM PT entries. */
    bcmptm_spm_pt_entry_t *spm_pt_entries;

    /*! Number of API field records used for this op. */
    uint32_t api_field_used;

    /*! Array of spm_pt_num SPM PT entries. */
    bcmltd_field_t *api_fields;

    /*! LTA intput list for retrieving sole no-key entry for UPDATE. */
    bcmltd_fields_t imm_lta_no_keys;

    /*! LTA intput list for retrieving current entry for UPDATE. */
    bcmltd_fields_t imm_lta_key_field;

    /*! LTA "array" of field pointers for key field. */
    bcmltd_field_t *key_field_ptr;

    /*! LTA output list for retrieving current entry for UPDATE. */
    bcmltd_fields_t imm_lta_out_fields;

    /*!
     * Chunk of memory for constructing LTA output field list.
     * Sized for the number of value fields in api_field_num.
     */
    void *imm_lta_fields_space;
} bcmptm_spm_op_info_t;

/*!
 * \brief SPM per-resource LTs information.
 *
 * SP resource LT information structure.
 */
typedef struct bcmptm_spm_lt_resource_info_s {
    /*! SP resource Logical Table id. */
    bcmdrd_sid_t ltid;

    /*! Corresponding SP Logical Table id. */
    bcmdrd_sid_t sp_ltid;

    /*! Field ID for key field. */
    uint32_t key_fid;

    /*! Field ID for value field. */
    uint32_t value_fid;
} bcmptm_spm_lt_resource_info_t;

/*!
 * \brief SPM all resource LTs information.
 *
 * SP resource LTs root information for a single unit.
 */
typedef struct bcmptm_spm_resource_root_s {
    /*! Pointer to the root SP LT records structure. */
    bcmptm_spm_lt_record_t *spm_lt_record;

    /*! Number of strength profile resource logical tables managed. */
    uint32_t resource_lt_count;

    /*! Strength profile resource logical table information list. */
    bcmptm_spm_lt_resource_info_t resource_lt[];
} bcmptm_spm_resource_root_t;

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize SPM IMM interface on a specific unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_spm_imm_init(int unit);

/*!
 * \brief  Cleanup SPM IMM interface on a specific unit.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_spm_imm_cleanup(int unit);

/*
 * \brief Initialize SPM resource info CTH handler on this BCM unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Inidicates cold or warm start status.
 * \param [in] spm_lt_record Strength Profole SPLT records structure.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_INIT On invalid device variant information.
 * \retval SHR_E_MEMORY On memory allocation failure.
 */
extern int
bcmptm_spm_resource_info_init(int unit, bool warm,
                              bcmptm_spm_lt_record_t *spm_lt_record);

/*
 * \brief Free all runtime resources allocated by SPM resource info
 *        CTH implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_spm_resource_info_cleanup(int unit, bool warm);

#endif /* SPM_INTERNAL_H */
