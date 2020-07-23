/*! \file bcmptm_chip_internal.h
 *
 * Chip-specific utils, defines shared across BCMPTM sub-modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CHIP_INTERNAL_H
#define BCMPTM_CHIP_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */


/*******************************************************************************
 * Typedefs
 */
/*! Information on device-specific default WAL buffer depth.
 */
typedef struct bcmptm_wal_info_s {
    /*! Maximum number of transactions. */
    uint32_t trans_max_count;
    /*! Maximum number of messages. */
    uint32_t msg_max_count;
    /*! Maximum number of words in words_buf array. */
    uint32_t words_buf_max_count;
    /*! Maximum number of ops_info elements. */
    uint32_t ops_info_max_count;
    /*! Maximum number of ops_info elements in undo array. */
    uint32_t undo_ops_info_max_count;
    /*! Maximum number of words in undo_words_buf array. */
    uint32_t undo_words_buf_max_count;

    /*! Maximum number of words in schan_fifo. */
    uint32_t max_words_in_msg;
} bcmptm_wal_info_t;

/*! Device-specific miscellaneous information.
 */
typedef struct bcmptm_dev_info_s {
    /*! RM needs to send slice_mode_change messages to SER logic. */
    uint32_t need_smc_msg;

    /*! RM needs to send entry_mode_change messages to SER logic. */
    uint32_t need_emc_msg;

    /*! Number of control words in sbus cmd.
     *  2 (opcode beat + address beat) for XGS devices)
     */
    uint8_t num_cwords;
} bcmptm_dev_info_t;

/*! Additional information related to setting of index_count.
 */
typedef struct bcmptm_index_count_set_info_s {
    /*! init = True means:
     * Initialize bank_count and pt_size to default HW values.
     * Return default HW bank_count,
     *
     * init = False means:
     * Use bank_count, start_bank as input and appropriately initialize
     * index_min, and index_count for user-specified pt_group.
     */
    bool init;

    /*! True means: update non-ser related index_counts. */
    bool for_non_ser;

    /*! True means: update ser related index_counts
     * (set ser related index_counts same as non_ser related index_counts.)
     */
    bool for_ser;

    /* Truth Table:
     * for_ser  for_non_ser     Means
     * 0        1               Only update non-ser values.
     *                          SER logic is working on old index_counts.
     *
     * 1        1               Normal mode.
     *                          Update both ser, non_ser related counts.
     *
     * 0        0               Illegal.
     *
     * 1        0               Only update ser values (copy non_ser value to ser).
     */
} bcmptm_index_count_set_info_t;

/*! Decoded response info for HW_lookup, insert, delete operations.
 */
typedef struct bcmptm_rsp_info_s {
    /*! response entry index */
    uint32_t entry_index;

    /*! key for entry to be inserted existed, so data replaced */
    bool replaced;

    /*! entry_index corresponds to 2x view */
    bool entry_index_view_2x;

    /*! entry_index corresponds to 4x view */
    bool entry_index_view_4x;

    /*! error in lp_word */
    bool error_in_lp_word;

    /*! error in action_table_a */
    bool error_in_action_table_a;

    /*! error in remap_table_a */
    bool error_in_remap_table_a;

    /*! error in action_table_b */
    bool error_in_action_table_b;

    /*! error in remap_table_b */
    bool error_in_remap_table_b;

    /*! Must do ser correction and retry */
    bool ser_retry;

} bcmptm_rsp_info_t;

/*!
 * \brief Format of {key, mask} stored in TCAM.
 * - During reads, TCAM will return key, mask in this format.
 * - During writes, either SW or HW needs to convert standard (key, mask) into
 * tcam-specific format
 */
typedef enum {
    BCMPTM_TCAM_KM_FORMAT_IS_KM = 0, /* tcam format is KM - meaning no conv needed */
    BCMPTM_TCAM_KM_FORMAT_ONLY_CLEAR_DCD, /* only clear dont_care D bits */
    BCMPTM_TCAM_KM_FORMAT_40NM_XY,
    BCMPTM_TCAM_KM_FORMAT_X_YBAR,
    BCMPTM_TCAM_KM_FORMAT_LPT,
    BCMPTM_TCAM_KM_FORMAT_LPT_7NM
} bcmptm_pt_tcam_km_format_t;

/*! Raw field info (used in constant init struct) */
typedef struct bcmptm_field_info_raw_s {
    /*! Start bit for the field */
    uint16_t start_bit;

    /*! width of the field */
    uint16_t width;
} bcmptm_field_info_raw_t;

/*!
 * \brief Types of overlay mems in PTcache. */
typedef enum {
    BCMPTM_OINFO_MODE_NONE = 0,
        /* Not an overlay */

    BCMPTM_OINFO_MODE_HASH = 1,
        /* multiple NEs in one WE,
         * All NEs of same width, start_bits of NEs are implicit.
         * Note: wide_entry corresponding to narrow entry may have a fixed
         * offset (as specified by start_bit_ne_in_we) in wide view. */

    BCMPTM_OINFO_MODE_TCAM_DATA = 2,
        /* NE is TCAM_DATA_ONLY view of TCAM_AGGR view */

    /* Following must always be the last entry in enum list */
    BCMPTM_OINFO_MODE_CACHE0
        /* Cacheable but without resource in PTcache. */
} bcmptm_oinfo_mode_t;

/*! Information for overlay (narrow) mems */
typedef struct bcmptm_overlay_info_s {
    /*! Overlay info mode. See defn for bcmptm_oinfo_mode_t */
    bcmptm_oinfo_mode_t mode;

    /*! TRUE for alpm type tables */
    bool is_alpm;

    /*! narrow view */
    bcmdrd_sid_t n_sid;

    /*! widest view */
    bcmdrd_sid_t w_sid;

    /*! width of narrow entries
     * - Also see defn of num_we_field below */
    uint16_t width_ne;

    /*! num narrow entries in widest view */
    uint8_t num_ne;

    /*! Start bit for narrow entry in widest view
     * Meaningful only in following cases:
     *      - when (mode == TCAM_DATA) && (num_we_field == 1)
     *      - when (mode == HASH):
     *          - Value N for this field implies offset of N*1024 in
     *            wide_entry view.
     */
    uint16_t start_bit_ne_in_we;

    /*! Number of wide_entry_fields to be extracted to construct narrow_entry
     * If num_we_field = 0, This cannot be TCAM_DATA type overlay.
     *                      'width_ne' provides width of narrow_entry
     *                      we_field_array will be NULL
     *
     * If num_we_field = 1, For TCAM_DATA type overlay:
     *                      'width_ne' provides width of 1st wef(wide_entry_field),
     *                      'start_bit_ne_we' provides start_bit of 1st wef
     *                      we_field_array will be NULL
     *                      This covers most TCAM_DATA type overlays.
     *
     * If num_we_field > 1, For TCAM_DATA type overlay:
     *                      'width_ne' provides width of 1st wef(wide_entry_field),
     *                      'start_bit_ne_we' provides start_bit of 1st wef
     *                      we_field_array will be non-NULL and provides
     *                      width, start_bit for 2nd, 3rd, and so on wefs
     *                      This mode is intended for cases like L3_DEFIP
     *                      where several wefs must be extracted to assemble 1 ne
     */
    uint8_t num_we_field;

    /*! Points to array that holds start_bit, width info for fields in Wide
     * entry that need to be accessed when accessing narrow entry.
     * When !NULL, num_we_field must be > 1 */
    bcmptm_field_info_raw_t *we_field_array;
} bcmptm_overlay_info_t;

/*! Information for write-only aggr mems */
typedef struct bcmptm_wo_aggr_info_s {
    /*! Pointer to list of member sid (n_sid) */
    bcmdrd_sid_t *n_sid_list;

    /*! Number of elements in n_sid_list */
    int n_sid_count;

    /*! TRUE means underlying overlay(narrow) sid can be part of multiple
     * wide (aggregate) views */
    bool w_mult;

} bcmptm_wo_aggr_info_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate and initialize 'function pointers' for a unit
 * \n Must be called every time a new unit is attached.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_fnptr_init(int unit);

/*!
 * \brief Deallocate 'function pointer' space allocated  for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_fnptr_cleanup(int unit);

/*!
 * \brief Initialize variant dependent data.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_var_data_init(int unit);

/*!
 * \brief Cleanup variant dependent data.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_var_data_cleanup(int unit);

/*!
 * \brief Return index_count for a PT
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval index_count (index_max - index_min + 1) if no errors
 * \retval 0 if errors (eg: sid is illegal, etc)
 */
extern uint32_t
bcmptm_pt_index_count(int unit,
                      bcmdrd_sid_t sid);

/*!
 * \brief Return list of PTs in a PT group.
 *
 * \param [in] unit Logical device id
 * \param [in] group_id Enum for PT group. When group_id is
 * BCMPTM_PT_GROUP_DEFAULT, pt_count is used to return total number of PT
 * groups supported by the device (group_count includes DEFAULT PT group - so
 * min returned group_count can be 1).
 *
 * \param [out] pt_count Num of PTs in PT group. Will be used to return
 * group_count when caller sets group_id to BCMPTM_PT_GROUP_DEFAULT.
 *
 * \param [out] bank_start_en TRUE means bank_start is meaningful for this PT
 * group. This value is common for all PTs in a group. This variable will always
 * be FALSE for BCMPTM_PT_GROUP_DEFAULT.
 *
 * \param [out] pt_list List of enums for PTs in PT group. Enum for widest PT
 * in the group will be the first element in the group. Will be NULL when
 * group_id is specified as BCMPTM_PT_GROUP_DEFAULT.
 *
 * Returns:
 * \retval SHR_E_NONE when no errors.
 */
extern int
bcmptm_pt_group_info_get(int unit, uint32_t group_id,
                         uint32_t *pt_count, bool *bank_start_en,
                         const bcmdrd_sid_t **pt_list);

/*!
 * \brief Compute and set index_count for a PT based on num of banks.
 *
 * \param [in] unit Logical device id.
 * \param [in] group_id Enum for PT group.
 * \param [in] index_count_set_info See definition of \ref
 * bcmptm_index_count_set_info_t.
 *
 * \param [in, out] bank_count Total number of banks (dedicated, uft) for this PT
 * group. During WB ('init' is FALSE) this is an input. During coldboot ('init'
 * is TRUE) this is an output.
 *
 * \param [in, out] start_bank_enum First bank for this PT group. During WB
 * ('init' is FALSE) this is an input. During coldboot ('init' is TRUE) this is
 * an output.
 *
 * Returns:
 * \retval SHR_E_NONE when no errors.
 */
extern int
bcmptm_pt_group_index_count_set(int unit, uint32_t group_id,
                                bcmptm_index_count_set_info_t
                                *index_count_set_info,
                                uint8_t *bank_count, uint8_t *start_bank_enum);

/*!
 * \brief Initialize mreq_info corresponding to an LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] trans_id transaction id.
 * \param [in] ltid Logical table ID
 * \param [in] warm  WB flag
 *
 * Returns:
 * \retval SHR_E_NONE when no errors.
 * \retval SHR_E_FAIL if mreq info is NULL.
 * \retval SHR_E_INTERNAL if a previously allocated hw resource is unavailable.
 */
extern int
bcmptm_rm_tcam_dyn_lt_mreq_info_init(int unit,
                                     uint32_t trans_id,
                                     bcmltd_sid_t ltid,
                                     bool warm);

/*!
 * \brief To validate pt banks info of an LT.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table ID
 *
 * Returns:
 * \retval SHR_E_NONE when no errors.
 * \retval SHR_E_FAIL if mreq info is NULL.
 */
extern int
bcmptm_rm_tcam_dynamic_lt_validate(int unit, bcmltd_sid_t ltid);

/*!
 * \brief Return index_count for a PT for SER logic.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval index_count (index_max - index_min + 1) if no errors
 * \retval 0 if errors (eg: sid is illegal, etc)
 */
extern uint32_t
bcmptm_scor_pt_index_count(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Return index_min for a PT for SER logic.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval First index for PT.
 * \retval Negative if errors (eg: sid is illegal, etc)
 */
extern int
bcmptm_scor_pt_index_min(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Return index_max for a PT for SER logic.
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval Last index for PT.
 * \retval Negative if errors (eg: sid is illegal, etc)
 */
extern int
bcmptm_scor_pt_index_max(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Deallocate 'function pointer' space allocated  for a unit
 * \n Must be called every time a unit is detached.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \retval ser_resp type. Refer to definition of bcmdrd_ser_resp_t
 */
extern bcmdrd_ser_resp_t bcmptm_pt_ser_resp(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Decode rsp_info for HW ops: lookup, insert, delete
 *
 * \param [in] unit Logical device id
 * \param [in] req_op Only LOOKUP, INSERT, DELETE supported - see defn for
 * bcmptm_op_type_t
 * \param [in] bd_rsp_info Response info from HW - see defn for
 * bcmbd_tbl_resp_info_t
 *
 * \param [out] rsp_info HW independent response info - see defn for
 * bcmptm_rsp_info_t
 *
 * \retval SHR_E_NONE - Interpretation successful. Nothing unexpected.
 *                 \n - May need SER correction
 * \retval Else       - Error/Unexpected
 */
extern int
bcmptm_rsp_info_get(int unit,
                    bcmptm_op_type_t req_op,
                    bcmbd_tbl_resp_info_t *bd_rsp_info,
                    bcmptm_rsp_info_t *rsp_info); /* out */

/*!
 * \brief Get device-specific WAL buffer, operation related information.
 *
 * \param [in] unit Logical device id
 *
 * \param [out] wal_info Device dependent WAL buffer, operation information.
 * Refer to definition for \ref bcmptm_wal_info_t
 *
 * \retval SHR_E_NONE - Success.
 * \retval Else       - Error/Unexpected
 */
extern int
bcmptm_wal_info_get(int unit,
                    bcmptm_wal_info_t *wal_info); /* out */

/*!
 * \brief Get general device-specific information.
 *
 * \param [in] unit Logical device id
 *
 * \param [out] dev_info Device dependent information. Refer to definition of
 * bcmptm_dev_info_t
 *
 * \retval SHR_E_NONE - Success.
 * \retval Else       - Error/Unexpected
 */
extern int
bcmptm_dev_info_get(int unit,
                    const bcmptm_dev_info_t **dev_info); /* out */

/*!
 * \brief Find if PT is ALPM view
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] is_alpm TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_attr_is_alpm(int unit, bcmdrd_sid_t sid, bool *is_alpm);

/*!
 * \brief Find overlay properties for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] oinfo Overlay info. See defn of bcmptm_overlay_info_t
 *
 * \retval SHR_E_NONE Success
 */
extern int bcmptm_pt_overlay_info_get(int unit, bcmdrd_sid_t sid,
                                      const bcmptm_overlay_info_t **oinfo);

/*!
 * \brief Find overlay properties for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] n_sid Enum to specify reg, mem that is member of aggregate view.
 * \param [in] w_sid Enum to specify aggregate view reg, mem.
 *
 * \param [out] oinfo Overlay info. See defn of bcmptm_overlay_info_t
 *
 * \retval SHR_E_NONE Success
 */
extern int bcmptm_pt_overlay_mult_info_get(int unit, bcmdrd_sid_t n_sid,
                                           bcmdrd_sid_t w_sid,
                                           const bcmptm_overlay_info_t **oinfo);

/*!
 * \brief Find properties for a Write Only Aggregate PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] wo_aggr_info Write-Only Aggr info (optional). See defn of
 * bcmptm_wo_aggr_info_t
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Table indicated by sid is NOT wo_aggr type mem.
 */
extern int bcmptm_pt_wo_aggr_info_get(int unit, bcmdrd_sid_t sid,
                                      bcmptm_wo_aggr_info_t *wo_aggr_info);
/*!
 * \brief Find key,mask format used by PT
 * - can be different for different PTs on same chip
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] km_format See enums for bcmptm_pt_tcam_km_format_t
 *
 * \retval SHR_E_NONE
 */
extern int
bcmptm_pt_tcam_km_format(int unit, bcmdrd_sid_t sid,
                         bcmptm_pt_tcam_km_format_t *km_format);

/*!
 * \brief Find if HW converts key,mask to HW specific xy format
 * - In future devices HW can convert key,mask to HW specific xy format during
 * writes because parity generation, checking are done in TCAM wrapper. This was
 * not possible in older devices where parity gen, check logic was centralized
 * in CMIC block.
 *
 * \param [in] unit Logical device id
 * \param [out] hw_does_km_to_xy TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_hw_does_km_to_xy(int unit, bool *hw_does_km_to_xy);

/*!
 * \brief Find if SW needs to store tcam_corrupt_bits
 * - TRUE for devices where TCAM parity error reporting is not via SER_FIFO
 *
 * \param [in] unit Logical device id
 * \param [out] tc_en TRUE or FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_tcam_corrupt_bits_enable(int unit, bool *tc_en);

/*!
 * \brief Find if PT needs to store logical_table_id for every entry
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] ltid_en TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_needs_ltid(int unit, bcmdrd_sid_t sid, bool *ltid_en);

/*!
 * \brief Find if PT needs to store dfield_format_id for every entry
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] dfid_bytes Only one byte or two byte dfid supported. Legal
 * values are thus: 0, 1, 2
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_needs_dfid(int unit, bcmdrd_sid_t sid, uint8_t *dfid_bytes);

/*!
 * \brief Find if PT is in 'must_cache' exception list
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] must_cache TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_in_must_cache_list(int unit, bcmdrd_sid_t sid,
                                        bool *must_cache);

/*!
 * \brief Find if PT is in 'dont_cache' exception list
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] dont_cache TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_in_dont_cache_list(int unit, bcmdrd_sid_t sid,
                                        bool *dont_cache);

/*!
 * \brief Find if PT is cacheable
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [out] cache_en TRUE, FALSE
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_pt_attr_is_cacheable(int unit, bcmdrd_sid_t sid,
                                       bool *cache_en);

/*!
 * \brief Enable, Disable PTM to process mreq, ireq API calls.
 *
 * \param [in] unit Logical device id
 * \param [in] ready TRUE implies allow PTM to process mreq, ireq API calls.
 *
 * \retval SHR_E_NONE
 */
extern void bcmptm_ptm_ready_set(int unit, bool ready);

/*!
 * \brief Check if PTM is ready to accept mreq, ireq API calls.
 *
 * \param [in] unit Logical device id
 *
 * \retval TRUE - Yes PTM is ready to accept mreq, ireq API calls.
 */
extern bool bcmptm_ptm_is_ready(int unit);

/*!
 * \brief Allocate block in HA mem.
 * For coldboot: Allocated mem will be zeroed out, signature added.
 * For warmboot: Signature word will be verified.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE, FALSE implies warmboot, coldboot respectively.
 * \param [in] in_req_size Number of requested bytes.
 * \param [in] submod_name Name of requestor submodule.
 * \param [in] submod_id ID of requestor submodule.
 * \param [in] ref_sign Signature.
 *
 * \param [out] alloc_ptr Pointer to allocated mem. NULL implies allocation
 * failure.
 *
 * \retval SHR_E_NONE
 */
extern int bcmptm_do_ha_alloc(int unit, bool warm, uint32_t in_req_size,
                              const char *submod_name, uint8_t submod_id,
                              uint32_t ref_sign, uint32_t **alloc_ptr);

/*!
 * \brief Mutex take wrapper
 * - Wrapper around sal_mutex_take.
 * - Will increase wait time if not real HW system.
 *
 * \param [in] unit Logical device id
 * \param [in] mtx Mutex handle from sal_mutex_create.
 * \param [in] usec Maximum number of microseconds to wait.
 *
 * \retval 0 Mutex successfully acquired.
 * \retval -1 -1 Failed to acquire mutex.
 */
extern int bcmptm_mutex_take(int unit, sal_mutex_t mtx, int usec);

#endif /* BCMPTM_CHIP_INTERNAL_H */
