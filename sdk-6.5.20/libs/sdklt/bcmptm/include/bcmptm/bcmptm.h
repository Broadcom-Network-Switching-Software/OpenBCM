/*! \file bcmptm.h
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

#ifndef BCMPTM_H
#define BCMPTM_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_ltm_types.h>
#include <shr/shr_pb.h>
/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Read a mem or reg internal to SOC - Interactive path.
 * \n This API must NOT be used by CTH. It is meant to be used by LTM only.
 * \n By default data is read from PTCache for cacheable PTs..
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ireq_read(int unit,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 void *pt_dyn_info,
                 void *pt_ovrr_info,
                 size_t rsp_entry_wsize,
                 bcmltd_sid_t req_ltid,

                 uint32_t *rsp_entry_words,
                 bcmltd_sid_t *rsp_ltid,
                 uint32_t *rsp_flags);

/*!
 * \brief Write a mem or reg internal to SOC - Interactive path.
 * - This API must NOT be used by CTH. It is meant to be used by LTM only.
 * - We have provided a debug option to allow Ireq path to modify modeled table.
 * If the entry was already valid in cache and not written by ireq_path, we will
 * preserved the LTID, DFID. Else, we will allow ireq path to over-write LTID,
 * DFID for this entry in cache.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ireq_write(int unit,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  void *pt_dyn_info,
                  void *pt_ovrr_info,
                  uint32_t *entry_words,
                  bcmltd_sid_t req_ltid,

                  bcmltd_sid_t *rsp_ltid,
                  uint32_t *rsp_flags);

/*!
 * \brief Verify sentinel words, invalid entries in ptcache
 * \n Can be used as end-check in ALL tests to make sure that sentinel words in
 * PTcache did not corrupted.
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE => verify PTcache for SER logic. FALSE => verify
 * PTcache for non-SER logic.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_verify(int unit, bool acc_for_ser);

/*!
 * \brief Check if PTcache is enabled for user-specified physical SID.
 * - Access to physical SIDs on modeled path is allowed only if PTcache is
 * enabled.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] enabled TRUE means PTcache enabled.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmptm_ptcache_check(int unit,
                     bcmdrd_sid_t sid,

                     bool *enabled);

/*!
  \brief Perform Lookup, Insert, Delete for hash type PTs - Interactive path.
  - This API must NOT be used by CTH. It is meant to be used by LTM only.
  - WILL NOT USE SW_CACHE
  - For simple tables, PTM will:
   -# Use (entry_key_words | entry_data_words) as entry_words and forward op to HW.
   -# Copy rsp_entry_words as received from HW into entry_key_words,
   entry_data_words arrays.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_hash_info_t (for XGS devices)
           \n Specifies the bank information.
  \param [in] pt_ovrr_info Ptr to override info (TBD)
  \param [in] entry_key_words Array large enough to hold key, mask fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] entry_data_words Array large enough to hold data fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_key_words,
  rsp_entry_data_words arrays
  \param [in] req_ltid Logical Table enum that is accessing the table

  \param [out] rsp_entry_key_words As received from HW
  \param [out] rsp_entry_data_words As received from HW
  \param [out] rsp_entry_index As received from HW
  \param [out] rsp_ltid Same as req_ltid (TBD)..
  \param [out] rsp_flags More detailed status
  \n For successful insert, BCMPTM_RSP_FLAGS_REPLACED may be asserted.
  \retval SHR_E_NONE Table entry found, deleted, inserted successfully.
  \retval SHR_E_NOT_FOUND Table entry not found (for lookup, delete ops)
  \retval SHR_E_FULL Table full (for Insert op only).
  \retval SHR_E_TIMEOUT S-channel operation timed out.
  \retval SHR_E_INTERNAL Invalid S-channel NACK response type.
  \retval SHR_E_FAIL Other failures.
 */
extern int
bcmptm_ireq_hash_lt(int unit,
                    uint64_t flags,
                    bcmdrd_sid_t sid,
                    void *pt_dyn_info,
                    void *pt_ovrr_info,
                    uint32_t *entry_key_words,
                    uint32_t *entry_data_words,
                    bcmptm_op_type_t req_op,
                    size_t rsp_entry_buf_wsize,
                    bcmltd_sid_t req_ltid,

                    uint32_t *rsp_entry_key_words,
                    uint32_t *rsp_entry_data_words,
                    uint32_t *rsp_entry_index,
                    bcmltd_sid_t *rsp_ltid,
                    uint32_t *rsp_flags);

/*!
  \brief Perform Write, Read for Index based LTs (without Key) - Modeled path.
  - This API must NOT be used by CTH. It is meant to be used by LTM only.
  - This API allows LTM to manage direct type LTs (eg: profile tables) that
  may map to several physical tables (SIDs). LTM decides the order in which
  phys tables are accessed. PTM does not do LTID to SID mapping for ops
  presented by this API. There is no rsrc_mgr or SW state for these in PTM.
  - Only WRITE, READ, NOP type opcodes supported
  - Hash type LTs, TCAM type LTs should not be accessed with
  bcmptm_mreq_indexed_lt()
  - NOPs allow LTM to issue ABORT, COMMIT without any table access.
  Only unit, flags, req_op, cseq_id, notify_fn_ptr, notify_info,
  rsp_flags, ret_val are meaningful for NOP request.
  - READs allow LTM to perform read_modify_write.
  - WRITEs:
   - req_ltid is stored in PTcache along with entry
  - READs from HW:
   - LTM can force PTM to read from HW by setting
   BCMLT_ENT_ATTR_GET_FROM_HW along with READ op.
  - READs from PTcache:
   - rsp_ltid is provided from PTcache along with entry
   - All physical tables in modeled path are expected to be Cacheable
    -# If table is not cacheable, READ req will return SHR_E_UNAVAIL
    -# If entry is not valid in cache (was never written before by SW), data in
    cache is still valid because during initialization, cache is filled with
    'default' values. Return SHR_E_NONE and entry value from cache.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Write, Read requests, index member of pt_dyn_info is necessary
  \n for Write, Read requests, tbl_inst member of pt_dyn_info may indicate pipe
  num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Additional information that can be specified to request
  use of DMA. Refer to definition of bcmptm_misc_info_t for details.
  \param [in] req_op Only WRITE, READ, NOP ops are valid
  \param [in] entry_words Array large enough to hold write data
  \n Dont care for READ, NOP requests.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \n Dont care for WRITE, NOP requests.
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] cseq_id Commit Sequence ID.
  \n For every mreq, LTM must also provide a cseq_id - even for mreq for which
  COMMIT is not asserted.
  \n Commit sequence contains multiple mreq. Each mreq submitted to PTM could
  result in N physical table ops where N could > size of schan_fifo. Having
  cseq_id for each mreq allows PTM to track each 'bunch of pt ops' for
  every mreq.
  \n For committed sequences which requested notification, this ID will be returned
  along with notification msg (TBD).
  \n PTM cmdproc will maintain overall state 'IDLE', so we do not need to
  reserve any value (like 0, 0xFFFF) as 'reserved'.
  \n This ID will also help with crash recovery (details TBD).

  \param [in] notify_fn_ptr Will be called when committed transaction is
  pushed to HW.
  \n Meaningful ONLY for COMMIT (and not for ABORT)
  \n NULL means notification is not needed.

  \param [in] notify_info Will be returned as param with notify_fn_ptr call
  \n Meaningful ONLY when notify_fn_ptr is not NULL

  \param [out] rsp_entry_words Array large enough to hold read data
  \n Dont care for WRITE, NOP requests.
  \param [out] rsp_ltid For READs from PTcache, rsp_ltid is ltid of entry in
  PTcache. For all other cases, rsp_ltid = req_ltid. (TBD)
  \param [out] rsp_flags More detailed status (TBD)

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL Table is not cacheable.
 */
extern int
bcmptm_mreq_indexed_lt(int unit,
                       uint64_t flags,
                       bcmdrd_sid_t sid,
                       void *pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_misc_info_t *misc_info,
                       bcmptm_op_type_t req_op,
                       uint32_t *entry_words,
                       size_t rsp_entry_wsize,
                       bcmltd_sid_t req_ltid,
                       uint32_t cseq_id,
                       bcmptm_trans_cb_f notify_fn_ptr,
                       void *notify_info,

                       uint32_t *rsp_entry_words,
                       bcmltd_sid_t *rsp_ltid,
                       uint32_t *rsp_flags);

/*!
  \brief Perform Lookup, Insert, Delete for LTs with Key - Modeled path.
  - This API must NOT be used by CTH. It is meant to be used by LTM only.
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  \n tbl_inst member of pt_dyn_info may indicate pipe num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] req_op LOOKUP, DELETE, INSERT ops are valid for tables
  with key.
  \n LTM can force PTM to read final entry from HW by setting
   BCMLT_ENT_ATTR_GET_FROM_HW along with LOOKUP op. This can be used to
   read Hit bits.
  \param [in] req_info See the definition for bcmptm_keyed_lt_mreq_info_t
  \param [in] cseq_id Commit Sequence ID.
  \n See earlier description for details.

  \param [out] rsp_info See the definition for bcmptm_keyed_lt_mrsp_info_t
  \param [out] rsp_ltid Previous owner of the entry
  \param [out] rsp_flags More detailed status (TBD)
  \n For Insert operation, PTM can provide additional information as below:
   - REPLACED
   - OLD_ENTRY_DELETED

  \retval SHR_E_NONE Success
  \retval SHR_E_UNAVAIL Requested op is not supported
  \retval SHR_E_NOT_FOUND Entry not found for Lookup, Delete ops
  \retval SHR_E_FULL Could not perform requested Insert for Tcam Tables
  \retval SHR_E_FULL Could not perform requested Insert due to Hash Collisions
  (Hash Tables)
  \retval SHR_E_MEMORY Could not perform requested Insert due to Table Full
  (Hash Tables).
 */
extern int
bcmptm_mreq_keyed_lt(int unit,
                     uint64_t flags,
                     bcmltd_sid_t req_ltid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     bcmptm_op_type_t req_op,
                     bcmptm_keyed_lt_mreq_info_t *req_info,
                     uint32_t cseq_id,

                     bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/*!
  \brief Dynamically enable atomic transaction.
  - dynamic_atomic_trans will be set only if feature.enable_atomic_trans is 0.
  - dynamic_atomic_trans will be disabled upon next commit, abort.

  \retval SHR_E_NONE Success
 */
extern int
bcmptm_mreq_atomic_trans_enable(int unit);

/* LTM callbacks for LT statistics and PT status tracking */

/*!
 * \brief Register a callback for incrementing LTM statistics.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT ops related statistic values.  This callback permits the PTM
 * implementation to update these statistics.
 *
 * \param [in] unit Logical device id
 * \param [in] lt_stat_cb Callback function to increment LT statistics
 *             values for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_lt_stat_increment_register(int unit,
                                  bcmptm_lt_stats_increment_cb lt_stat_cb);

/* LTM callbacks for PT status recording. */

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for modeled keyed operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers modeled keyed PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_mreq_keyed_lt_register(int unit,
                      bcmptm_pt_status_mreq_keyed_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for modeled index operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers modeled index PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_mreq_indexed_lt_register(int unit,
                      bcmptm_pt_status_mreq_indexed_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for interactive hash operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers interactive hash PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_ireq_hash_lt_register(int unit,
                           bcmptm_pt_status_ireq_hash_lt_cb pt_status_cb);

/*!
 * \brief Register a callback for LTM function to record PT status
 *        for interactive index operations.
 *
 * CTH implementations of Logical Tables require a mechanism to update
 * the PT status for each PTM op.  This callback permits the PTM
 * implementation to update this record.
 *
 * This case covers interactive index PT operations.
 *
 * \param [in] unit Logical device id
 * \param [in] pt_status_cb Callback function to increment PT status
 *             record for PT ops.
 *
 * \retval None
 */
extern void
bcmptm_pt_status_ireq_op_register(int unit,
                                  bcmptm_pt_status_ireq_op_cb pt_status_cb);

/* LTM wrappers for PTM interfaces */

/*!
 * \brief LTM-callbacks wrapper for bcmptm_ireq_read
 * - This API is meant to be used by CTH only.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_entry_words Array large enough to hold read data
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ltm_ireq_read(int unit,
                     uint64_t flags,
                     bcmdrd_sid_t sid,
                     void *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,
                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/*!
 * \brief LTM-callbacks wrapper for bcmptm_ireq_write
 * - This API is meant to be used by CTH only.
 *
 * \param [in] unit Logical device id
 * \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
 * \param [in] sid Enum to specify reg, mem
 * \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
 * \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] req_ltid Logical Table enum that is accessing the table
 *
 * \param [out] rsp_ltid Previous owner of the entry (TBD)
 * \param [out] rsp_flags More detailed status (TBD)
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ltm_ireq_write(int unit,
                      uint64_t flags,
                      bcmdrd_sid_t sid,
                      void *pt_dyn_info,
                      void *pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,
                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags);

/*!
  \brief  LTM-callbacks wrapper for bcmptm_ireq_hash_lt
  - This API is meant to be used by CTH only.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_hash_info_t (for XGS devices)
           \n Specifies the bank information.
  \param [in] pt_ovrr_info Ptr to override info (TBD)
  \param [in] entry_key_words Array large enough to hold key, mask fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] entry_data_words Array large enough to hold data fields
           \n For simple tables, fields must be formatted exactly as required by
           sid
  \param [in] req_op Only LOOKUP, DELETE, INSERT ops are valid for hash tables
  \param [in] rsp_entry_buf_wsize Num of words in rsp_entry_key_words,
  rsp_entry_data_words arrays
  \param [in] req_ltid Logical Table enum that is accessing the table

  \param [out] rsp_entry_key_words As received from HW
  \param [out] rsp_entry_data_words As received from HW
  \param [out] rsp_entry_index As received from HW
  \param [out] rsp_ltid Same as req_ltid (TBD)..
  \param [out] rsp_flags More detailed status
  \n For successful insert, BCMPTM_RSP_FLAGS_REPLACED may be asserted.
 */
extern int
bcmptm_ltm_ireq_hash_lt(int unit,
                        uint64_t flags,
                        bcmdrd_sid_t sid,
                        void *pt_dyn_info,
                        void *pt_ovrr_info,
                        uint32_t *entry_key_words,
                        uint32_t *entry_data_words,
                        bcmptm_op_type_t req_op,
                        size_t rsp_entry_buf_wsize,
                        bcmltd_sid_t req_ltid,
                        uint32_t *rsp_entry_key_words,
                        uint32_t *rsp_entry_data_words,
                        uint32_t *rsp_entry_index,
                        bcmltd_sid_t *rsp_ltid,
                        uint32_t *rsp_flags);

/*!
  \brief LTM-callbacks wrapper for bcmptm_mreq_indexed_lt
  - This API is meant to be used by CTH only.

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Write, Read requests, index member of pt_dyn_info is necessary
  \n for Write, Read requests, tbl_inst member of pt_dyn_info may indicate pipe
  num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] misc_info Additional information that can be specified to request
  use of DMA. Refer to definition of bcmptm_misc_info_t for details.
  \param [in] req_op Only WRITE, READ, NOP ops are valid
  \param [in] entry_words Array large enough to hold write data
  \n Dont care for READ, NOP requests.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \n Dont care for WRITE, NOP requests.
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] cseq_id Commit Sequence ID.
  \n For every mreq, LTM must also provide a cseq_id - even for mreq for which
  COMMIT is not asserted.
  \n Commit sequence contains multiple mreq. Each mreq submitted to PTM could
  result in N physical table ops where N could > size of schan_fifo. Having
  cseq_id for each mreq allows PTM to track each 'bunch of pt ops' for
  every mreq.
  \n For committed sequences which requested notification, this ID will be returned
  along with notification msg (TBD).
  \n PTM cmdproc will maintain overall state 'IDLE', so we do not need to
  reserve any value (like 0, 0xFFFF) as 'reserved'.
  \n This ID will also help with crash recovery (details TBD).

  \param [in] notify_fn_ptr Will be called when committed transaction is
  pushed to HW.
  \n Meaningful ONLY for COMMIT (and not for ABORT)
  \n NULL means notification is not needed.

  \param [in] notify_info Will be returned as param with notify_fn_ptr call
  \n Meaningful ONLY when notify_fn_ptr is not NULL

  \param [out] rsp_entry_words Array large enough to hold read data
  \n Dont care for WRITE, NOP requests.
  \param [out] rsp_ltid For READs from PTcache, rsp_ltid is ltid of entry in
  PTcache. For all other cases, rsp_ltid = req_ltid. (TBD)
  \param [out] rsp_flags More detailed status (TBD)
 */
extern int
bcmptm_ltm_mreq_indexed_lt(int unit,
                           uint64_t flags,
                           bcmdrd_sid_t sid,
                           void *pt_dyn_info,
                           void *pt_ovrr_info,
                           bcmptm_misc_info_t *misc_info,
                           bcmptm_op_type_t req_op,
                           uint32_t *entry_words,
                           size_t rsp_entry_wsize,
                           bcmltd_sid_t req_ltid,
                           uint32_t cseq_id,
                           bcmptm_trans_cb_f notify_fn_ptr,
                           void *notify_info,
                           uint32_t *rsp_entry_words,
                           bcmltd_sid_t *rsp_ltid,
                           uint32_t *rsp_flags);

/*!
  \brief LTM-callbacks wrapper for bcmptm_mreq_keyed_lt
  - This API is meant to be used by CTH only.
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (see BCMPTM_REQ_FLAGS_XXX)
  \param [in] req_ltid Logical Table enum that is accessing the table
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t (for XGS devices)
  \n for Lookup, Insert, Delete ops, index member of pt_dyn_info is dont_care
  \n tbl_inst member of pt_dyn_info may indicate pipe num
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] req_op LOOKUP, DELETE, INSERT ops are valid for tables
  with key.
  \n LTM can force PTM to read final entry from HW by setting
   BCMLT_ENT_ATTR_GET_FROM_HW along with LOOKUP op. This can be used to
   read Hit bits.
  \param [in] req_info See the definition for bcmptm_keyed_lt_mreq_info_t
  \param [in] cseq_id Commit Sequence ID.
  \n See earlier description for details.

  \param [out] rsp_info See the definition for bcmptm_keyed_lt_mrsp_info_t
  \param [out] rsp_ltid Previous owner of the entry
  \param [out] rsp_flags More detailed status (TBD)
  \n For Insert operation, PTM can provide additional information as below:
   - REPLACED
   - OLD_ENTRY_DELETED
 */
extern int
bcmptm_ltm_mreq_keyed_lt(int unit,
                         uint64_t flags,
                         bcmltd_sid_t req_ltid,
                         void *pt_dyn_info,
                         void *pt_ovrr_info,
                         bcmptm_op_type_t req_op,
                         bcmptm_keyed_lt_mreq_info_t *req_info,
                         uint32_t cseq_id,
                         bcmptm_keyed_lt_mrsp_info_t *rsp_info,
                         bcmltd_sid_t *rsp_ltid,
                         uint32_t *rsp_flags);

/*!
  \brief Enable, Disable WAL usage statistics.
  \param [in] unit Logical device id
  \param [in] enable TRUE, FALSE enables, disables update to wal_stats_info.

  \retval SHR_E_NONE When no error.
 */
extern int
bcmptm_wal_stats_info_enable(int unit, bool enable);


/*!
  \brief Get WAL usage statistics.
  \param [in] unit Logical device id
  \param [in] info_p Pointer to wal statistics info structure. Refer to details
  of \ref bcmptm_wal_stats_info_t.

  \retval SHR_E_NONE When no error.
 */
extern int
bcmptm_wal_stats_info_get(int unit, bcmptm_wal_stats_info_t *info_p);

/*!
  \brief Clear WAL usage statistics.
  \param [in] unit Logical device id

  \retval SHR_E_NONE When no error.
 */
extern int
bcmptm_wal_stats_info_clear(int unit);

/*!
 * \brief Change LTID stored in PTcache.
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_ltid_change(int unit);

/*!
 * \brief Disable WAL abort error checking for one commit sequence.
 * \n Disabling of abort_error_check is effective only for the case where
 * enable_atomic_trans = 0 and commit sequence is aborted when WAL is not IDLE.
 * \n abort_error_check will automatically be re-enabled when WAL sees a commit
 * or abort.
 *
 * \param [in] unit Logical device id
 * \param [in] enable TRUE => Suppress error checking for one commit sequence.
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_wal_abort_error_check(int unit, bool enable);

/*!
 * \brief Register a callback function to compute hit index.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical table ID.
 * \param [in] index_cb Callback function.
 * \param [in] report_for_insert Flag indicating if reporting event for insert.
 * \param [in] num_lookups Number of lookups.
 * \param [in] lookup_reporting_enable Enable status for specific lookup.
 * \param [in] context Flex counter context info.
 * \param [out] entry_width Entry width.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_rm_hash_hit_index_callback_register(int unit,
                                           bcmltd_sid_t ltid,
                                           bcmptm_rm_hash_entry_index_cb_t index_cb,
                                           bool report_for_insert,
                                           uint8_t num_lookups,
                                           bool *lookup_reporting_enable,
                                           void *context,
                                           bcmptm_rm_hash_entry_width_t *entry_width);

/*!
 * \brief Show SBR-RM state in current software DB.
 * \param [in] unit Unit number.
 * \param [in] pb Print buffer to populate output data.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM When print buffer pointer in null.
 * \retval SHR_E_INIT When this feature has not completed initialization.
 */
extern int
bcmptm_sbr_dump_sw_state(int unit, shr_pb_t *pb);

/*!
 * \brief Show SBR-RM state in HA software DB.
 * \param [in] unit Unit number.
 * \param [in] pb Print buffer to populate output data.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM When print buffer pointer in null.
 * \retval SHR_E_INIT When this feature has not completed initialization.
 */
extern int
bcmptm_sbr_dump_ha_sw_state(int unit, shr_pb_t *pb);

/*!
 * \brief Convert LT entry attributes to PTM request flags.
 *
 * This routine returns the corresponding PTM request flags for
 * the specified LT entry attributes BCMLT_ENT_ATTR_xxx.
 *
 * \param [in] attrib LT entry attributes.
 *
 * \retval PTM request flags.
 */
static inline uint64_t
bcmptm_lt_entry_attrib_to_ptm_req_flags(uint32_t attrib)
{
    return ((uint64_t) attrib);
}

/*!
 * \brief Convert PTM request flags to  LT entry attributes.
 *
 * This routine returns the corresponding LT entry attributes
 * BCMLT_ENT_ATTR_xxx for the specified PTM request flags.
 *
 * \param [in] req_flags PTM request flags.
 *
 * \retval LT entry attributes.
 */
static inline uint32_t
bcmptm_req_flags_to_lt_entry_attrib(uint64_t req_flags)
{
    return ((uint32_t)(req_flags & 0xffffffff));
}

/*!
 * \brief Return index_min for a PT
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval First index for PT.
 * \retval Negative if errors (eg: sid is illegal, etc)
 */
extern int
bcmptm_pt_index_min(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Return index_max for a PT
 *
 * \param [in] unit Logical device id.
 * \param [in] sid Enum for PT.
 *
 * Returns:
 * \retval Last index for PT.
 * \retval Negative if errors (eg: sid is illegal, etc)
 */
extern int
bcmptm_pt_index_max(int unit, bcmdrd_sid_t sid);

#endif /* BCMPTM_H */
