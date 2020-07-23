/*! \file bcmptm_uft.h
 *
 * APIs, defines for other modules to interface with UFT LT.
 *
 * This file contains APIs, defines for PTM-UFT interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMPTM_UFT_H
#define BCMPTM_UFT_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmptm/bcmptm_rm_hash_internal.h>

/*******************************************************************************
 * Typedefs
 */
/*! Maximum banks count of an enhanced memory bank group. */
#define UFT_VAL_BANKS_MAX DEVICE_EM_BANK_CNT_MAX

/*!
 * \brief Encode LTID and transaction ID into 64-bit value.
 *
 * The event notification callback signature provides a 64-bit value
 * to supply context for the receiving implementation.  This macro
 * combines the logical table ID and the transaction ID to provide
 * this context.
 *
 * \param [in] _ltid Logical table ID.
 * \param [in] _trans_id Transaction ID.
 *
 * \return 64-bit event information context value.
 */
#define BCMPTM_UFT_EV_INFO_CONSTRUCT(_ltid, _trans_id) \
    ((((uint64_t)(_trans_id)) << 32) | (_ltid))

/*!
 * \brief Retreive transaction ID from event context.
 *
 * The event notification callback signature provides a 64-bit value
 * to supply context for the receiving implementation.  This macro
 * extracts the transaction ID.
 *
 * \param [in] _ev_info Event notification context value.
 *
 * \return Transaction ID.
 */
#define BCMPTM_UFT_EV_INFO_TRANS_ID_GET(_ev_info)      \
    (((_ev_info) >> 32) & 0xffffffff)

/*!
 * \brief Retreive LTID from event context.
 *
 * The event notification callback signature provides a 64-bit value
 * to supply context for the receiving implementation.  This macro
 * extracts the logical table ID.
 *
 * \param [in] _ev_info Event notification context value.
 *
 * \return Logical table ID.
 */
#define BCMPTM_UFT_EV_INFO_LTID_GET(_ev_info)          \
    ((_ev_info) & 0xffffffff)

/*! Bank Attributes structure. */
typedef struct bcmptm_bank_attr_s {
    /*! Bank PT symbol ID.*/
    bcmdrd_sid_t bank_sid;
    /*! Bank ID. */
    uint8_t bank_id;
    /*! Offset for hash bank.*/
    uint8_t hash_offset;
    /*! Random seed. */
    uint32_t seed;
    /*! Enable/disable robust hash. */
    bool robust_en;
} bcmptm_bank_attr_t;

/*! Phyical bank information structure. */
typedef struct bcmptm_pt_banks_info_s {
    /*! Hash vector type. */
    bcmptm_rm_hash_vector_type_t  vector_type;
    /*! Bank list used for lookup0 of a logical table. */
    bcmptm_bank_attr_t bank[UFT_VAL_BANKS_MAX];
    /*!
     * User-configured paired bank used for lookup1 of the logical table.
     * Applicable if lookup0_bank[n] is used for lookup0 of a logical table
     * which requires 2 lookups.
     * bank[] array and paired_bank[] array have same index and bank_cnt.
     */
    bcmptm_bank_attr_t paired_bank[UFT_VAL_BANKS_MAX];
    /*! Count of banks used for lookup0. */
    uint8_t bank_cnt;
    /*!
     * Bank list used for lookup1 of a logical table.
     * This structure member is only applicable for tile mode.
     */
    bcmdrd_sid_t lookup1_bank_sid[UFT_VAL_BANKS_MAX];
    /*! Count of banks used for lookup1. */
    uint8_t lookup1_bank_cnt;
} bcmptm_pt_banks_info_t;

/*! Bank's hardware attribute. */
typedef struct bcmptm_bank_hw_attr_s {
    /*! SID of the bank to which a lookup of the logical table is mapped. */
    bcmdrd_sid_t bank_sid;
    /*! Hardware LT ID for the logical table. */
    int32_t hw_ltid;
} bcmptm_bank_hw_attr_t;

/*!
 * It provides mapped banks info for per lookup of a logical table
 * under a certain tile mode. It includes all the un-assgined banks
 * info for the lookup X of the logical table.
 */
typedef struct bcmptm_lt_lookup_banks_info_s {
    /*! Count of banks. */
    uint8_t bank_cnt;
    /*! Per bank's hardware attribute. */
    bcmptm_bank_hw_attr_t bank_attr[UFT_VAL_BANKS_MAX];
} bcmptm_lt_lookup_banks_info_t;

/*!
 * \brief Logical table lookup instance type definition.
 */
typedef enum bcmptm_uft_sbr_lt_lookup_e {
    /*! Invalid lookup instance. */
    BCMPTM_UFT_SBR_LOOKUP_INVALID   = 0,
    /*! LT lookup instance 0. */
    BCMPTM_UFT_SBR_LOOKUP0          = 1,
    /*! LT lookup instance 1. */
    BCMPTM_UFT_SBR_LOOKUP1          = 2,
    /*! LT lookup reserved instance for use by compiler. */
    BCMPTM_UFT_SBR_LOOKUP_RESERVED
} bcmptm_uft_sbr_lt_lookup_t;
/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Get move depth for a logical table.
 * \param [in] unit Logical device ID.
 * \param [in] ltid Logitcal table ID.
 * \param [out] move_depth Returned move depth.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_lt_move_depth_get(int unit, bcmltd_sid_t ltid,
                                 uint8_t *move_depth);

/*!
 * \brief Get bank group info for RM-ALPM.
 * \param [in] unit Logical device id.
 * \param [in] ptsid Sid of physical table.
 * \param [out] banks_info Banks info for the given pt.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_bank_info_get_from_rmalpm(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info);

/*!
 * \brief Get bank group info for RM-TCAM.
 * \param [in] unit Logical device id.
 * \param [in] ptsid Sid of physical table.
 * \param [out] banks_info Banks info for the given pt.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_bank_info_get_from_rmtcam(int unit, bcmdrd_sid_t ptsid,
                                         bcmptm_pt_banks_info_t *banks_info);

/*!
 * \brief Get bank group info for PTCache.
 * \param [in] unit Logical device id.
 * \param [in] ptsid Sid of physical table.
 * \param [out] banks_info Banks info for the given pt.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_bank_info_get_from_ptcache(int unit, bcmdrd_sid_t ptsid,
                                          bcmptm_pt_banks_info_t *banks_info);

/*!
 * \brief Get uft group info from LT ID.
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [in] lookup_num 0 represents lookup0, 1 represents lookup1.
 * \param [out] banks_info Banks info for the ltid's LOOKUP0/1.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_bank_hw_attr_get_from_lt(int unit, bcmltd_sid_t ltid,
                                    uint32_t lookup_num,
                                    bcmptm_lt_lookup_banks_info_t *banks_info);

/*!
 * \brief Get uft group info from LT ID.
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [in] alpm_level ALPM level2/3. For hash LT, set it to 0.
 * \param [out] banks_info Banks info for the given pt.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_bank_info_get_from_lt(int unit, bcmltd_sid_t ltid,
                                     int alpm_level,
                                     bcmptm_pt_banks_info_t *banks_info);


/*!
 * \brief Get LTS_TCAM sid list for EM_FT_1/2.
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [out] ptsid_array Returned ptsids.
 * \param [in] ptsid_array_size Size of ptsid_array.
 * \param [out] r_cnt Returned count of ptsids.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_lt_lts_tcam_get(int unit, bcmltd_sid_t ltid,
                               bcmdrd_sid_t *ptsid_array,
                               uint32_t ptsid_array_size,
                               uint32_t *r_cnt);

/*!
 * \brief Get maximum lookups supported by a logical table.
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [out] lookup_cnt Returned maximum lookups.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_lt_lookup_cnt_get(int unit, bcmltd_sid_t ltid,
                             uint32_t *lookup_cnt);

/*!
 * \brief Get uft group ID from LT ID.
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table id.
 * \param [in] alpm_level ALPM level value.
 * \param [out] grp_id Group ID of this LT.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_grp_id_get(int unit, bcmltd_sid_t ltid,
                          uint32_t alpm_level,
                          uint32_t *grp_id);

/*!
 * \brief Allocate an entry or an entry pair from a SBR profile table (SPT).
 *
 * This function can be used to allocate single entry or a pair of entries from
 * a SPT. For allocating an entry pair \c paired_lookups input parameter must
 * be set to TRUE by the callee.
 *
 * This function views the input \c spt_sid SPT entries to be organized as
 * buckets with each bucket supporting two entries. So, for an input \c spt_sid
 * SPT with 32 entries, the table would support 16 buckets and 2 entries (an
 * entry pair) per bucket. The paired entries in these buckets are separated by
 * an offset index value of SPT size divided by 2 i.e. 32 / 2 = 16. So, for the
 * example SPT with 32 entries, the bucket entries pairing is bkt0:{e0, e16},
 * bkt1:{e1, e17}, bkt2:{e2, e18}...bkt15:{e15, e31}.
 *
 * For single entry alloc call, this function will try to maximize the
 * utilization of buckets by giving preference to free entry in a partially
 * filled bucket over free entries in an empty bucket.
 *
 * For paired alloc, the first bucket that has both entries free will be
 * allocated and returned by this function.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] lt_id Lookup Logical Table ID.
 * \param [in] lookup_type Lookup instance for \c lt_id \ref
 *                         bcmptm_uft_sbr_lt_lookup_t.
 * \param [in] paired_lookups Paired lookup and entry alloc call.
 * \param [in] entry_count Size of input \c entry_index array.
 * \param [in] entry_index Array to set and return the allocated entry indices.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value \c lt_id or \c lookup_type.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 * \retval SHR_E_RESOURCE Free entries are not available to complete the alloc.
 * \retval SHR_E_INTERNAL Unanticipated internal error.
 */
extern int
bcmptm_uft_sbr_entry_alloc(int unit,
                           bcmdrd_sid_t spt_sid,
                           bcmltd_sid_t lt_id,
                           bcmptm_uft_sbr_lt_lookup_t lookup_type,
                           bool paired_lookups,
                           uint16_t entry_count,
                           uint16_t *entry_index);
/*!
 * \brief Free an entry or entry pair in a SBR profile table.
 *
 * This function can be used to free previously allocated SBR profile table
 * entry or entry pair by providing the \c entry_count and corresponding entry
 * index values in \c entry_index array.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] entry_count Size of input \c entry_index array.
 * \param [in] entry_index Pointer to entry or entries pair to be freed.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value.
 * \retval SHR_E_NOT_FOUND Entries in \c entry_index array are not found to be
 *                         in use to free them.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 */
extern int
bcmptm_uft_sbr_entry_free(int unit,
                          bcmdrd_sid_t spt_sid,
                          uint16_t entry_count,
                          uint16_t *entry_index);
/*!
 * \brief Allocate an entry or an entry pair in multiple SBR profile tables.
 *
 * The entry or entry pair will be allocated at the same index in all the
 * SBR profile tables given in \c spt_sid array. This function will return
 * success only when free entry or entry pair is found in all the SBR profile
 * tables or else it will return resource error.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid_count Size of input \c spt_sid array.
 * \param [in] spt_sid Array of SBR profile table IDs.
 * \param [in] lt_id Lookup Logical Table ID.
 * \param [in] lookup_type Lookup instance for \c lt_id \ref
 *             bcmptm_uft_sbr_lt_lookup_t.
 * \param [in] paired_lookups Paired lookup and entry alloc call.
 * \param [in] entry_count Size of \c entry_index array.
 * \param [in] entry_index Array to set and return the allocated entry indices.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value.
 * \retval SHR_E_RESOURCE Free entries are not available to complete the alloc.
 * \retval SHR_E_INTERNAL Unanticipated internal error.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 */
extern int
bcmptm_uft_sbr_multi_tables_entry_alloc(int unit,
                                        uint16_t spt_sid_count,
                                        bcmdrd_sid_t *spt_sid,
                                        bcmltd_sid_t lt_id,
                                        bcmptm_uft_sbr_lt_lookup_t lookup_type,
                                        bool paired_lookups,
                                        uint16_t entry_count,
                                        uint16_t *entry_index);
/*!
 * \brief Free an entry or entry pair in multiple SBR profile tables.
 *
 * The input entries provided in \c entry_index array must be in-use in all
 * the SBR profile tables listed in \c spt_sid array for this function call
 * to be successful. If the input \c entry_index value is not in use for one of
 * the SBR profile table, then this function will return entry not found error.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid_count Size of input \c spt_sid array.
 * \param [in] spt_sid Array of SBR profile table IDs.
 * \param [in] entry_count Size of \c entry_index array.
 * \param [in] entry_index Array of entry indices to be freed.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value.
 * \retval SHR_E_NOT_FOUND Entries in \c entry_index array are not found to be
 *                         in use to free them.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 */
extern int
bcmptm_uft_sbr_multi_tables_entry_free(int unit,
                                       uint16_t spt_sid_count,
                                       bcmdrd_sid_t *spt_sid,
                                       uint16_t entry_count,
                                       uint16_t *entry_index);

/*!
 * \brief Returns entry index into SBR profile table memory for given LT.LOOKUP.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] lt_id Enum for LT portion of LT.LOOKUP e.g. l2_narrow_host_table.
 * \param [in] lookup_type Specifies \ref bcmptm_uft_sbr_lt_lookup_t LT lookup
 *                         instance type.
 * \param [out] entry_index Index into physical SBR profile table memory, valid
 *                          when retval is SHR_E_NONE.
 * \retval SHR_E_NOT_FOUND If SBR entry corresponding to this lt_id and
 *                         lookup_type is not yet allocated. In this case,
 *                         returned entry_index value is dont_care.
 * \retval SHR_E_NONE On success.
 */
extern int
bcmptm_uft_sbr_entry_index_get(int unit,
                               bcmdrd_sid_t spt_sid,
                               bcmltd_sid_t lt_id,
                               bcmptm_uft_sbr_lt_lookup_t lookup_type,
                               uint16_t *entry_index);

/*!
 * \brief External API for committing SBR tile mode set changes to HA.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_uft_sbr_tile_mode_commit(int unit);

/*!
 * \brief External API for aborting SBR tile mode set changes to HA.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_uft_sbr_tile_mode_abort(int unit);

/*!
 * \brief Get configuration of a given tile.
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 * \param [out] tile_mode Returned tile mode.
 * \param [out] robust Returned whether robust is enabled.
 * \param [out] seed Returned seed of a tile.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_tile_cfg_get(int unit, uint32_t tile_id,
                        uint32_t *tile_mode, bool *robust, uint32_t *seed);

/*!
 * \brief Return ALPM PDD type for all IFTA90 tiles in order.
 * Note: It returns consecutive ALPM PDD types for all IFTA90 tiles.
 *       ALPM doesn't care about the tile to which this PDD type belongs.
 * \param [in] unit Unit number.
 * \param [in] pdd_type_size Size of pdd_type_array.
 * \param [in] mtop Set to return only MTOP PDD types.
 * \param [out] pdd_type_array Returned pdd type array.
 * \param [out] r_cnt Returned actual count of PDD type.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_alpm_pdd_type_get(int unit, uint32_t pdd_type_size,
                             int mtop,
                             uint32_t *pdd_type_array, uint32_t *r_cnt);

/*!
 * \brief Return ALPM key mux type for all IFTA80 tiles in order.
 * Note: It returns consecutive ALPM key mux types for all IFTA80 tiles.
 *       ALPM doesn't care about the tile to which this key mux type belongs.
 * \param [in] unit Unit number.
 * \param [in] key_mux_type_size Size of key_mux type_array.
 * \param [in] mtop Set to return only MTOP key mux types.
 * \param [out] key_mux_type_array Returned key mux type array.
 * \param [out] r_cnt Returned actual count of key mux type.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_alpm_key_mux_type_get(int unit, uint32_t key_mux_type_size,
                                 int mtop,
                                 uint32_t *key_mux_type_array,
                                 uint32_t *r_cnt);

/*!
 * \brief Provided API for RM-HASH module to set hit context for flex counter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg bcmltd_op_arg_t pointer.
 * \param [in] w_ptid The bank's ptsid in widest view.
 * \param [in] index Index of hit index profile table.
 * \param [in] hit_context Context value to be set.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_fc_hit_context_set_rm_hash(int unit,
                                      bcmltd_sid_t lt_id,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmdrd_sid_t w_ptid,
                                      uint32_t index,
                                      uint32_t hit_context);

/*!
 * \brief Provided API for RM-HASH module to set hit context for flex counter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg bcmltd_op_arg_t pointer.
 * \param [in] w_ptid The bank's ptsid in widest view.
 * \param [in] index Index of hit index profile table.
 * \param [in] hit_context Context value to be set.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_fc_hit_context_get_rm_hash(int unit,
                                      bcmltd_sid_t lt_id,
                                      const bcmltd_op_arg_t *op_arg,
                                      bcmdrd_sid_t w_ptid,
                                      uint32_t index,
                                      uint32_t *hit_context);

/*!
 * \brief Provided API for index tile RM module to set hit context.
 * for flex counter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg bcmltd_op_arg_t pointer.
 * \param [in] tile_id Tile ID.
 * \param [in] hit_context Returned context value.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_fc_hit_context_set_rm_index(int unit,
                                       bcmltd_sid_t lt_id,
                                       const bcmltd_op_arg_t *op_arg,
                                       uint32_t tile_id,
                                       uint32_t hit_context);

/*!
 * \brief Provided API for index tile RM module to get hit context.
 * for flex counter.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] op_arg bcmltd_op_arg_t pointer.
 * \param [in] tile_id Tile ID.
 * \param [in] hit_context Returned context value.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_fc_hit_context_get_rm_index(int unit,
                                       bcmltd_sid_t lt_id,
                                       const bcmltd_op_arg_t *op_arg,
                                       uint32_t tile_id,
                                       uint32_t *hit_context);

#endif /* BCMPTM_UFT_H */
