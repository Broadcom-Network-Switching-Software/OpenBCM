/*! \file bcmptm_types.h
 *
 * APIs, defines for top-level PTM interfaces
 *
 * This file contains APIs, defines for top-level PTM interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_TYPES_H
#define BCMPTM_TYPES_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <shr/shr_error.h>


/*******************************************************************************
 * Defines
 */
/*
 * Flags available to LTM for bcmptm_ireq, mreq.
 */

/*! Default flags */
#define BCMPTM_REQ_FLAGS_NO_FLAGS                      0x00000000ULL << 32



/* ======================== Public flags bits 31:0
 * - originate in BCMLT APIs
 */

/*!Meaningful only for Read request, Read portion of Read_Modify_Write requests.
 * \n 1 => Read from HW even if PT is cacheable
 * \n 0 => Read from Cache (if PT is cacheable)
 * \n This flag is not looked at for Write. Writes will always update the cache,
 if PT is cacheable,
 * \n Meaningful for both interactive (PTpassthru) path and Modeled path
 */
/* #define BCMPTM_REQ_FLAGS_DONT_USE_CACHE BCMLT_ENT_ATTR_GET_FROM_HW */

/*!
 * If set performs LPM lookup instead exact match.
 * Valid for ALPM only.
 */
/* #define BCMPTM_REQ_FLAGS_LPM_LOOKUP BCMLT_ENT_ATTR_LPM_LOOKUP */




/* ======================== API flags: bits 55:32 */

/*! 1 => Commit uncommitted WAL operations as a 'commit sequence' for HW
 * \n Return ctl back to LTM (without waiting for committed ops in WAL to
 * finish execution in HW)..
 * \n Meaningful only when req_op = BCMPTM_OP_NOP
 * \n Meaningful only for Modeled path.
 */
#define BCMPTM_REQ_FLAGS_COMMIT                        0x00000001ULL << 32

/*! 1 => Delete all uncommitted WAL operations and revert PTcache, SW state.
 * \n Meaningful only when req_op = BCMPTM_OP_NOP
 * \n Meaningful only for Modeled path.
 * \n notify_fn_ptr will NOT be called.
 */
#define BCMPTM_REQ_FLAGS_ABORT                         0x00000002ULL << 32

/*! 1 => Entry insert should be to all pipes individually
 * \n though the device is in global mode.
 * This is to override the behaviour of global and per pipe mode
 * for special handling in TCAMs.
 */
#define BCMPTM_REQ_FLAGS_WRITE_PER_PIPE_IN_GLOBAL      0x00000004ULL << 32

/*! 1 => Interactive request is for passthru (pt) table
 *  0 => Interactive request is for LT
 *  x for Modeled path */
#define BCMPTM_REQ_FLAGS_PASSTHRU                      0x00000008ULL << 32

/*! 1 => entry_words in hw_entry format
 *  0 => entry_words in ctr_cache format
 *  Same value as FLAGS_PASSTHRU on purpose */
#define BCMPTM_REQ_FLAGS_CCI_HW_ENTRY_FORMAT           0x00000008ULL << 32


/*! Meaningful only for Read requests, Read portion of Read_Modify_Write
  requests.
 * \n 1 => Return data as received from HW as is.
 * \n 0 => Attempt to convert XY format to Key, Mask format.
 * \n This flag is not looked at for Write, Read_Modify_Write requests.
 * \n Expected encoding of Key, Mask bits from LTM:
 * \n Key_bit, Mask_bit, Meaning:
 * \n 0, 1, Compare 0
 * \n 1, 1, Compare 1
 * \n 0, 0, Dont_Compare (key bit is dont_care)
 * \n 1, 0, Dont_Compare (key bit is dont_care)
 * \n Note: XY format can be different for different chips, TCAM types.
 */
#define BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM            0x00000010ULL << 32







/* ======================== PTM internal flags: bits 63:56 */

/*! Callers of bcmptm APIs must not assert any of these. */
#define BCMPTM_REQ_FLAGS_RSVD                          0xFF000000ULL << 32

/* Must list all internal flags usage here.
 * Must make sure there is no overlap with flags declared in bcmptm_types.h
 */
/*! 1 => Return pointer to entry_words in PTcache.
 *  0 => Copy entry_words from PTcache into rsp_entry_words buffer.
 */
#define BCMPTM_REQ_FLAGS_PTCACHE_RETURN_ENTRY_P        0x80000000ULL << 32





/*
 * Response flags
 */
/*! Default flags */
#define BCMPTM_RSP_FLAGS_NO_FLAGS                      0x00000000

/*!Meaningful only Insert request
 * \n 1 => Key already existed, so was replaced with new entry (data) */
#define BCMPTM_RSP_FLAGS_REPLACED                      0x00000001

/*! Maximum PT indices that can get updated during single operation */
#define BCMPTM_RSP_MAX_INDEX                           0x00000006

/*! Transaction ID to be used when using modeled path APIs for initializaiton */
#define BCMPTM_DIRECT_TRANS_ID                         0xFFFFFFFF

/*! Invalid hit index when the corresponding lookup is disabled. */
#define BCMPTM_INVALID_HIT_INDEX                       0xFFFFFFFF

/*! Lt entry move event. */
#define BCMPTM_LT_ENTRY_MOVE                           "bcmptmEvLtEntryMove"
/*******************************************************************************
 * Typedefs
 */
/*!
 * \brief Op types for bcmptm requests (modeled path)
 */
typedef enum {
    BCMPTM_OP_NOP = 1,
    BCMPTM_OP_LOOKUP,
    BCMPTM_OP_INSERT,
    BCMPTM_OP_DELETE,
    BCMPTM_OP_GET_FIRST,
    BCMPTM_OP_GET_NEXT,
    BCMPTM_OP_WRITE,
    BCMPTM_OP_READ,
    BCMPTM_OP_GET_TABLE_INFO
} bcmptm_op_type_t;

/*!
 * \brief Information needed from PTRM for TABLE_INFO LT.
 */
typedef struct bcmptm_table_info_s {
/*!
 * \brief Maximum number of entries for current configuration.
 */
    uint64_t entry_limit;
} bcmptm_table_info_t;

/*! \brief Information needed for TABLE_PT_OP_INFO.
  */
typedef struct bcmptm_pt_op_info_s {
/*! \brief count of valid entries in rsp_entry_index array */
    uint32_t rsp_entry_index_cnt;

/*! \brief Index at which entry was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded.  */
    uint32_t rsp_entry_index[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_sid array */
    uint32_t rsp_entry_sid_cnt;

/*! \brief SID of PT where entry was found (all ops)
  - For all ops, if entry was found, PTRM will return SID of found entry.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_sid_data array */
    uint32_t rsp_entry_sid_data_cnt;

/*! \brief SID of PT where entry data was found (all ops)
  - For all ops, if entry data was found, PTRM will return SID of found entry data.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT, PTRM will return SID of new entry if INSERT succeeded. */
    bcmdrd_sid_t rsp_entry_sid_data[BCMPTM_RSP_MAX_INDEX];

/*! \brief count of valid entries in rsp_entry_index_data array */
    uint32_t rsp_entry_index_data_cnt;

/*! \brief Index at which entry data was found (all ops)
  - For all ops, if entry was found, PTRM will return index of found entry data.
  - If entry was not found:
   -# For LOOKUP, DELETE ops, PTRM will not set this var
   -# For INSERT op, PTRM will return index of new entry if INSERT succeeded. */
    uint32_t rsp_entry_index_data[BCMPTM_RSP_MAX_INDEX];

} bcmptm_pt_op_info_t;

/*! \brief Information to be passed with keyed LT request - modeled path
  */
typedef struct bcmptm_keyed_lt_mreq_info_s {

/*! \brief Array of arrays holding key fields, mask fields of an entry
  - General
   -# Array of arrays is needed for key_width expansion - different key fields of
   an LTID may map to different rows of same phys table (SID). For eg: IPv6 key
   will map to 4 consecutive rows of L3_TUNNEL table.
   -# Value of KEY_TYPEf, BASE_VALIDf fields (as needed by HW) will be written
   by PTRM. 'Written' but NOT 'Inserted' - means LTM will have 'slots' for these
   fields in entry_key_words as needed, so PTM will never need to shift bits
   in this array - even for flex-keygen cases.

  - Hash type LTs
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Only single row of entry_key_words is needed.
   -# entry_key_words is needed for all ops

  - Aggr type Pri Tcam LTs
   -# 'Aggr type' refers to Tcam tables where TCAM+DATA view is defined in
   regsfile.
   -# For Aggr type LTs, L2P mapping L2P mapping must map user fields to
   TCAM+DATA view.
   -# entry_key_words format must match with key, mask portion in TCAM+DATA view
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Array of arrays is not needed.

  - Non_Aggr type Pri Tcam LTs
   -# 'Non_Aggr type' refers to Tcam tables where TCAM+DATA view is NOT
   available in regsfile.
   -# For such tables format of entry_key_words must match with format of
   TCAM_ONLY view in regsfile.
   -# Array of arrays can be used to support key_width expansion.

  - Prefix type Tcam LTs (unmapped)
   -# LTM will provide tcam mask as bit_mask in DM format (not HW specific XY
   format). PTM will convert DM format to HW-specific XY format.
   -# PTRM will compute prefix_length and also do checking of mask bits (for
   holes)
   -# entry_key_words is needed for all ops (need to know prefix_length for
   LOOKUP, DELETE)
   -# Array of arrays is not needed.
   */
    uint32_t **entry_key_words;

/*! \brief Array of arrays holding data fields (dfields) of an entry
  - General
   -# Array of arrays is needed for key_width expansion (different key fields of
   an LTID map to different rows of same phys table (SID). For eg: IPv6 key will
   map to 4 consecutive locations of L3_TUNNEL table.
   -# Value of DATA_TYPEf, BASE_VALIDf fields (as needed by HW) will be written/
   inserted by PTRM.

  - Hash type LTs
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Only single row of entry_data_words is needed..
   -# entry_data_words can be non_zero only for INSERT op
   -# Must be ALL_ZEROS for LOOKUP, DELETE ops (only entry_key_words are
   needed for these ops).

  - Aggr type Pri Tcam LTs
   -# entry_data_words format must match with data portion in TCAM+DATA view for
   simple tables (single fixed dfield format LTs)
   -# For simple tables, hw_entry_words = entry_key_words | entry_data_words;
   -# Array of arrays is not needed.
   -# entry_data_words must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# entry_data_words can be non-zero only for INSERT op.

  - Non_Aggr type Pri Tcam LTs
   -# For such tables format of entry_data_words must match with format of
   TCAM_DATA_ONLY view in regsfile.
   -# Array of arrays can be used to support width expansion of associated data
   (eg: fp_policy)
   -# Num of elements in entry_data_words need not be same as same as num
   elements in entry_key_words array. For example, if we have 3*160 bit key and
   2x fp_policy, we will have 3 rows in entry_key_words and 2 rows in
   entry_data_words.
   -# entry_data_words must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# entry_data_words can be non-zero only for INSERT op.

  - Prefix type Tcam LTs (unmapped)
   -# entry_data_words is needed only for INSERT op.
   -# Must be ALL_ZEROs for LOOKUP, DELETE ops.
   -# Array of arrays is not needed.
   */
    uint32_t **entry_data_words;

/*! \brief Array of logical_data_type presented by LTM
  - General
   -# Must be NULL for simple tables which do not have multiple dfield formats.
   -# Needed only if LT supports multiple dfield formats and, or entry widths.
   -# Needed because for same LT, we may want to insert entry with different set
   of dfields. Eg: we may want to INSERT an entry with or without flex_counter.
   -# Meaningful only for ops where entry_data_words are provided by LTM
   -# Array is needed because pdd_l_dtyp could be different for each row of
   entry_data_words
   -# Each row of pdd_l_dtyp provides information for corresponding row of
   entry_data_words.
   -# Because every row of entry_data_words may correspond to separate SID,
   value of 0 must be used if SID does not have multiple dfield formats.
   -# Meaningful only for INSERT op. Should be NULL for other ops.
   */
    uint16_t *pdd_l_dtyp;

/*! \brief entry_id (handle) needed for priority type Tcam LTs with entry_id
  - Dont care for Hash type LTs, Prefix type Tcam LTs
  - Needed for most (but not all) Pri Tcam LTs
  - For Pri Tcam LTs with entry_id:
   -# Search for LOOKUP, DELETE ops is based on entry_id (and not the key) and
   hence entry_key_words will not be needed for these ops.
  */
    uint32_t entry_id;

/*! \brief Needed for priority type TCAM LTs
  - Dont care for Hash type LTs, Prefix type Tcam LTs
  - For Pri Tcam LTs.
   -# Dont_care for LOOKUP, DELETE ops. If user-specified entry is found, it's
   entry_pri is returned as rsp_entry_pri.
   -# Needed only for INSERT op
    - entry_pri for Inserted/Replaced entry will be set/changed to entry_pri
 */
    uint32_t entry_pri;

/*! \brief LTM indicates to PTM that this is same key as previous LOOKUP op
  - Description below applies for all LTs on Modeled path.

  - Assumption for modeled path
   -# There is only one thread for modeled path and therefore only one uncommitted
   transaction in WAL per unit. PTM cannot assemble multiple concurrent transactions
   (even for different tables in WAL) for same unit.

  - After LOOKUP op, if LTM comes back with INSERT for same key, it must specify
  'same_key=TRUE'. This saves PTRM from having to redo the search (which may
  involve re-computing hash vector, bucket, index, lookup related state, etc for
  this key).
   -# For Hash type LTs, PTRM must save the hash_vector and bucket related information for LAST
   lookup operation from LTM for last LOOKUP op.
   -# For TCAM type LTs, PTRM can save info related to last LOOKUP op.

  - As per BCMLT op discussions::
   -# API req for INSERT must result in error if
   entry already exists. This requires LTM to first issue LOOKUP and only if
   entry does not exist it will issue INSERT with same_key=TRUE. Because previous
   LOOKUP failed, PTM will attempt to insert new entry when it gets INSERT. In
   this case, LTM must set same_key TRUE for INSERT op.
   -# API req for UPDATE must result in error if entry does not exist. This requires
   LTM to first issue LOOKUP and only if entry exists it will issue INSERT with
   same_key=TRUE. Because previous LOOKUP succeeded, PTM will either: 'replace
   entry' or 'insert new entry and delete old entry'.
   -# In both of above cases, when LOOKUP is followed by INSERT, LTM must
   provide same_key=TRUE with INSERT op to PTM.

  - Observation:
   -# same_key MUST always be FALSE for LOOKUP op
   -# same_key MUST always be FALSE for DELETE op
   -# same_key MUST always be 1 for for INSERT op.
   If LTM can guarantee above behavior we dont need 'same_key' param -
   PTRM can implicitly use the expected value. */
    bool same_key;

/*! \brief Num of words in rsp_entry_key_buf array
  - PTM will assume all rows of rsp_entry_key_words to be of same size */
    size_t rsp_entry_key_buf_wsize;

/*! \brief Num of words in rsp_entry_data_buf array
  - PTM will assume all rows of rsp_entry_data_words to be of same size */
    size_t rsp_entry_data_buf_wsize;

/*! \brief Attributes of entry needed for special type of TCAMs like FP.
 *   - PTRM will convert to required type and use the information. */
    void *entry_attrs;
} bcmptm_keyed_lt_mreq_info_t;

/*! \brief Information to be returned for keyed LT request - modeled path
  */
typedef struct bcmptm_keyed_lt_mrsp_info_s {

/*! \brief Key, mask fields of entry found (all ops)
  - General
   -# PTM will not touch this array if entry was not found (all ops).
   -# It is assumed that INSERT op will always be preceded by LOOKUP op, so
   'old_entry' was already returned to LTM and thus, there is no need for PTM to
   return this information again for subsequent INSERT-found case (for which
   PTRM may replace data or delete old entry).

  - Need to separate out rsp_entry_key_words, rsp_entry_data_words to support
  returning of data in following cases:
   -# data could have multiple dfield_formats
   -# data could be encoded in PDD
   -# data from Non_aggr type Tcam LTs

  - Hash type LTs, Aggr type Pri Tcam LTs, Prefix type Tcam LTs (unmapped)
   -# Only single row of entry_key_words is returned.
   -# For entry found case, rsp_entry_key_words may also contain data_fields.
   LTM must only pick 'key' fields (and ignore non-key fields).

  - Non_Aggr type Pri Tcam LTs
   -# Array of arrays can be used to return key, mask fields for Non_Aggr type
   Pri LTs with key expansion.
   -# rsp_entry_key_words will contain TCAM_ONLY fields
  */
    uint32_t **rsp_entry_key_words;

/*! \brief Data fields of entry found (all ops)
  - General
   -# PTM will not touch this array if entry was not found (all ops).
   -# It is assumed that INSERT op will always be preceded by LOOKUP op, so
   'old_entry' was already returned to LTM and thus, there is no need for PTM to
   return this information again for subsequent INSERT-found case (for which
   PTRM may replace data or delete old entry).

  - Hash type LTs, Aggr type Pri Tcam LTs, Prefix type Tcam LTs (unmapped)
   -# Only single row of entry_data_words is returned.
   -# For entry found case, rsp_entry_data_words may also contain non-data fields.
   LTM must only pick 'data' fields (and ignore non-data fields).

  - Non_Aggr type Pri Tcam LTs
   -# Array of arrays can be used to return key, mask fields for Non_Aggr type
   Pri LTs with data expansion.
   -# rsp_entry_key_words will contain TCAM_DATA_ONLY fields
  */
    uint32_t **rsp_entry_data_words;

/*! \brief Logical_data_type for entry found
  - General
   -# Meaningful only for cases where entry_data_words is returned by PTM
   -# Will be NULL for entry not found case.
   -# Will be NULL for INSERT op case (because INSERT was preceded by LOOKUP and
   so old_entry was already returned).
   -# Each row corresponds to row of rsp_entry_data_words
   -# Different data fields could be present/absent in every row of
   rsp_entry_data_words and hence we need corresponding row of pdd_l_dtyp
   -# Will be NULL for simple tables where data fields have only one format.
   -# Because every row of rsp_entry_data_words may correspond to separate SID,
   value of 0 must be used if SID does not have multiple data field formats.
   */
    uint16_t *rsp_pdd_l_dtyp;

/*! \brief Returned for priority type LTs
  - Hash type LTs
   -# Same value received with request (should be dont_care in LTM)

  - Prefix type LTs
   -# Same value received with request (should be dont_care in LTM)

  - Pri type LTs
   - Priority of entry found, deleted.
  */
    uint32_t rsp_entry_pri;

/*! \brief Pipe number of PT where entry was found, successfully inserted.
  - In case of LOOKUP, DELETE ops, if entry was found, PTRM will return
  pipe_number of PT where entry was found. If entry was not found, PTRM will
  not set this var or set it to 0.
  - For successful INSERT op, PTRM will return pipe_number of PT where entry
  was inserted. Else, it will not set the var or set it to 0.
  - For LTs in global mode, RM will return 0.
  - For LTs in pipe-unique mode, RM will return pipe_num where entry was found
  or successfully inserted. */
    uint32_t rsp_entry_pipe;

/*! \brief Pointer to TABLE_PT_OP_INFO */
    bcmptm_pt_op_info_t *pt_op_info;

/*! \brief Pointer to bcmptm_table_info_t, etc as determined by req_op. */
    void *rsp_misc;
} bcmptm_keyed_lt_mrsp_info_t;

/*!
 * \brief Define transaction operation status and callback.
 *
 * All logical table transaction operations result in a status
 * to be returned to the caller. This callback function is invoked
 * upon completion of a transaction.  The LTM module passes this
 * callback onward to the PTM so that module may notify when all portions
 * of a modelled transaction are complete.
 *
 * \param [in] trans_id Transaction ID of this notification.
 * \param [in] status Operation status indication.
 * \param [in] context Cookie supplied when callback registered.
 *
 * \return void None.
 */
typedef void (*bcmptm_trans_cb_f)(uint32_t trans_id,
                                  shr_error_t status,
                                  void *context);

/*! Tcam slice modes from serc perspective */
typedef enum {
    BCMPTM_TCAM_SLICE_INVALID = 0, /* Not applicable */
    BCMPTM_TCAM_SLICE_DISABLED = 1, /* Disabled */
    BCMPTM_TCAM_SLICE_NARROW = 2, /* Narrow */
    BCMPTM_TCAM_SLICE_WIDE = 3, /* Wide */
} bcmptm_rm_slice_mode_t;

/*! \brief Information passed by RM when changing slice_mode */
typedef struct bcmptm_rm_slice_change_s {
    /*! slice num. Use -1 to indicate invalid slice. */
    int slice_num0;

    /*! slice num. Use -1 to indicate invalid slice */
    int slice_num1;

    /*! New slice_mode */
    bcmptm_rm_slice_mode_t slice_mode;

    /*! Helps identify RM that manages these slices */
    uint32_t slice_group_id;
} bcmptm_rm_slice_change_t;

/*! Resource manager op types */
typedef enum {
    BCMPTM_RM_OP_NORMAL = 0, /* Nothing special */
    BCMPTM_RM_OP_EM_NARROW = 1, /* Entry mode narrow */
    BCMPTM_RM_OP_EM_WIDE = 2, /* Entry mode wide */
    BCMPTM_RM_OP_SLICE_MODE_CHANGE = 3, /* Slice mode change */
} bcmptm_rm_op_t;

/*! \brief Information to be passed with LT request.
  */
typedef struct bcmptm_misc_info_s {

/*! Enable DMA.
 * - Currently supported only for Indexed tables.
 */
    bool dma_enable;

/*! Number of entries for DMA.
 * - Meaningful only when dma_enable is TRUE.
 */
    uint32_t dma_entry_count;

/*! Allows CTH to specify slice_mode change.
 * See definition for \ref bcmptm_rm_slice_change_t.
 */
    bcmptm_rm_slice_change_t *slice_mode_change_info;
} bcmptm_misc_info_t;

/*! \brief Information to be passed with LT request.
  */
typedef struct bcmptm_wal_stats_info_s {
/*! Current state of stats_info collection. */
    bool stats_enabled;

/*! Maximum entries used from transaction_info buffer. */
    uint32_t max_trans_count;

/*! Maximum entries used from WAL message_info buffer. */
    uint32_t max_msg_count;

/*! Maximum entries used from WAL ops_info buffer. */
    uint32_t max_op_count;

/*! Maximum entries used from WAL words buffer. */
    uint32_t max_word_count;

/*! Maximum entries used from WAL undo_ops_info buffer. */
    uint32_t max_undo_op_count;

/*! Maximum entries used from WAL undo_words buffer. */
    uint32_t max_undo_word_count;

/*! Configured entries in WAL transaction_info buffer. */
    uint32_t cfg_trans_count;

/*! Configured entries in WAL message_info buffer. */
    uint32_t cfg_msg_count;

/*! Configured entries in WAL ops_info buffer. */
    uint32_t cfg_op_count;

/*! Configured entries in WAL words buffer. */
    uint32_t cfg_word_count;

/*! Configured entries in WAL undo_ops_info buffer. */
    uint32_t cfg_undo_op_count;

/*! Configured entries in WAL undo_words buffer. */
    uint32_t cfg_undo_word_count;

/*! Maximum number of control, data words used by single WAL message. */
    uint32_t max_words_in_msg;

/*! Maximum number of operations in single WAL message. */
    uint32_t max_ops_in_msg;

/*! Maximum number of messages in single WAL transaction.. */
    uint32_t max_msgs_in_trans;

/*! Maximum number of slam_messages in single WAL transaction.. */
    uint32_t max_slam_msgs_in_trans;
} bcmptm_wal_stats_info_t;

/*! \brief Hash LT entry width. */
typedef enum bcmptm_rm_hash_entry_width_e {
    /*! Single base entry sized entry. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_SINGLE = 0,

    /*! Double base entry sized entry. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_DOUBLE,

    /*! Quad base entry sized entry. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_QUAD,

    /*! Half base entry sized entry. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_HALF,

    /*! Third base entry sized entry. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_THIRD,

    /*! Count of all the supported entry widths. */
    BCMPTM_RM_HASH_ENTRY_WIDTH_COUNT,
} bcmptm_rm_hash_entry_width_t;

/*! \brief Hash table bank context info. */
typedef struct bcmptm_rm_hash_hit_context_s {
    /*! Tile ID.*/
    uint8_t tile_id;

    /*! Table ID. */
    uint8_t table_id;
} bcmptm_rm_hash_hit_context_t;

/*! Callback function to compute hit index for hash LTs. */
typedef int (*bcmptm_rm_hash_entry_index_cb_t)(int unit,
                                               bcmltd_sid_t ltid,
                                               bcmdrd_sid_t sid,
                                               bcmptm_rm_hash_hit_context_t *hit_context,
                                               int tbl_inst,
                                               uint32_t bkt_num,
                                               uint32_t bkt_entry,
                                               void *context,
                                               uint32_t *entry_idx);

#endif /* BCMPTM_TYPES_H */
