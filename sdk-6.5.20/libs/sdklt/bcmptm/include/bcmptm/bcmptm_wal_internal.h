/*! \file bcmptm_wal_internal.h
 *
 * This file contains APIs, defines for WAL sub-block
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_WAL_INTERNAL_H
#define BCMPTM_WAL_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
  Defines
 */


/*******************************************************************************
  Typedefs
 */


/*******************************************************************************
  Global variables
 */


/*******************************************************************************
  Function prototypes
 */
/*!
  \brief Enable/Disable atomic trans.
  - Can be called with enable = 1, only if atomic trans are disabled and
  autocommit mode is not active.
  - Can be called with enable = 0, only after it has been called earlier with
  enable = 1.
  \param [in] unit Logical device id
  \param [in] enable TRUE means override configured mode and setup WAL to work
  in mode where atomic transactions are enabled. FALSE, setup WAL to work in
  configured mode.

  \retval SHR_E_NONE Success
  */
extern int
bcmptm_wal_dyn_atomic_trans(int unit, bool enable);

/*!
  \brief Update HW table entry (through WAL)
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (TBD)
           \n TBD
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] misc_info Information needed for SLAM.
  \param [in] entry_words Array large enough to hold write_data
  \param [in] op_type See definition for bcmptm_rm_op_t
  \param [in] slice_mode_change_info Information needed to change slice_mode.

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available. Eventually timed out.
  */
extern int
bcmptm_wal_write(int unit,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 bcmptm_misc_info_t *misc_info,
                 uint32_t *entry_words,
                 bcmptm_rm_op_t op_type,
                 bcmptm_rm_slice_change_t *slice_mode_change_info);

/*!
  \brief Read table entry from HW (through WAL)
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
           \n BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM can be asserted to get raw
           data for TCAM type mems.
           \n BCMPTM_REQ_FLAGS_POSTED_HW_READ can be asserted to get control
           back in non-blocking fashion (without waiting for read data from HW)
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] misc_info Information needed for DMA..
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Array large enough to hold read data

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_read(int unit,
                uint64_t flags,
                bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *pt_dyn_info,
                bcmptm_misc_info_t *misc_info,
                size_t rsp_entry_wsize,

                uint32_t *rsp_entry_words);

/*!
  \brief Commit pending(uncommitted) ops
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
              \n Asserted BCMPTM_REQ_FLAGS_HW_SYNC will make WAL return ctl back
              to caller only when all ops have executed in HW (WAL is empty)
  \param [in] mreq_state_in_trans mreq_state
  \param [in] cseq_id Commit Sequence Id
  \param [in] notify_fn_ptr Notify TRM when committed ops are pushed to HW
  \param [in] notify_info Notify TRM when committed ops are pushed to HW

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_commit(int unit,
                  uint64_t flags,
                  bool mreq_state_in_trans,
                  uint32_t cseq_id,
                  bcmptm_trans_cb_f notify_fn_ptr,
                  void *notify_info);

/*!
  \brief Delete pending(uncommitted) ops
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior (TBD)
  \param [in] cseq_id Commit Sequence Id

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_abort(int unit,
                 uint64_t flags,
                 uint32_t cseq_id);

/*!
  \brief Search for SERC restore data in WAL
  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
           \n TBD. Pass all ZEROs for now.
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array
  \param [in] req_slice_num Tcam slice_num for corrupted entry. Must be -1 for non
  Tcam tables and for Tcam tables which don't have slices.
  \param [in] req_slice_mode Current slice_mode for slice specified by
  slice_num. Must be BCMPTM_TCAM_SLICE_INVALID for non Tcam tables and for Tcam
  tables which don't have slices.

  \param [out] rsp_entry_words Array large enough to hold read data

  \retval SHR_E_NONE Success
  \retval SHR_E_TIMEOUT Some WAL resource is not available or we hit timeout
  while waiting for committed ops to complete.
  */
extern int
bcmptm_wal_serc_data_find(int unit,
                          uint64_t flags,
                          bcmdrd_sid_t sid,
                          bcmbd_pt_dyn_info_t *pt_dyn_info,
                          size_t rsp_entry_wsize,
                          int req_slice_num,
                          bcmptm_rm_slice_mode_t req_slice_mode,

                          uint32_t *rsp_entry_words);

/*!
  \brief Drain all ops in WAL
  - Means: wait for all committed ops in WAL to finish executing in HW.
  - Optionally, stop WAL rdr after the drain is complete.

  \param [in] unit Logical device id
  \param [in] end_check TRUE means expect WAL c_trans_state to be IDLE when WAL
  is empty.

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_drain(int unit, bool end_check);

/*!
  \brief Alloc, init wal data structs
  - Writer side only

  \param [in] unit Logical device id
  \param [in] warm TRUE implies warmboot

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_init(int unit, bool warm);

/*!
  \brief De-alloc wal data structs
  - Writer side only

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_cleanup(int unit);

/*!
  \brief Enable, disable WAL auto-commit mode
  - All -ve numbers mean auto-commit mode
  - Must not provide Commit from LTM when in auto-commit mode
  - 0 means real-commit mode

  \param [in] unit Logical device id
  \param [in] wal_mode 0 => normal. Negative implies auto-commit

  \retval SHR_E_NONE Success
*/
extern int
bcmptm_wal_mode_sel(int unit, int wal_mode);

/*!
  \brief Apply wal_cfg
  - Mainly for debug - allows unit test to change wal_cfg vars in middle of
    test according to wal_mode selected using bcmptm_wal_mode_sel
  - There is no re-sizing of WAL buffers in HA space.

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success
*/
extern int
bcmptm_wal_cfg_set(int unit);

/*!
  \brief Check if WAL is in auto-commit mode

  \param [in] unit Logical device id

  \retval TRUE if WAL is in auto-commit mode
*/
extern bool
bcmptm_wal_autocommit_enabled(int unit);

/*! \brief Transaction cmd: commit, abort, cseq_id_check

  \param [in] unit Logical device id
  \param [in] flags For COMMIT, BCMPTM_REQ_FLAGS_HW_SYNC is meaningful
              \n Usage TBD for other commands
  \param [in] trans_cmd Refer to details of bcmptm_trans_cmd_t
  \param [in] cseq_id Commit Sequence Id
  \param [in] notify_fn_ptr Meaningful only for COMMIT
  \param [in] notify_info Meaningful only for COMMIT

  \retval SHR_E_NONE Success */
extern int
bcmptm_wal_trans_cmd(int unit, uint64_t flags,
                     bcmptm_trans_cmd_t trans_cmd,
                     uint32_t cseq_id, bcmptm_trans_cb_f notify_fn_ptr,
                     void* notify_info);

/*! \brief Record mreq_cseq_id, and set mreq_state_in_trans
    - Following must be called for every non-NOP request from LTM
    - It must be called even for READ, LOOKUP, GET_FIRST, GET_NEXT, etc ops
    - It changes mreq_state_in_trans to TRUE.

  \param [in] unit Logical device id
  \param [in] flags Usage TBD
  \param [in] cseq_id Commit Sequence Id

  \retval SHR_E_NONE Success */
int
bcmptm_wal_mreq_state_update(int unit, uint32_t cseq_id, uint64_t flags);

/*!
 * \brief Start WAL reader thread
 * - Includes alloc for msgq
 * - Includes WAL reader thread_create and ensuring that it is run state
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_init(int unit);

/*!
 * \brief Release WAL reader thread and msgq resources
 *
 * \param [in] unit Logical device id
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_walr_cleanup(int unit);

/*!
 * \brief Check wal DMA assist availability.
 *
 * \param [in] unit Logical device id
 * \param [in] read_op TRUE => Read operation. FALSE => Write operation.
 * \param [in] entry_count Number of entries.
 * \param [out] wal_dma_avail TRUE => DMA assist is available for WAL writes,
 * reads.
 */
extern void
bcmptm_wal_dma_avail(int unit, bool read_op, uint32_t entry_count,
                     bool *wal_dma_avail);

/*!
 * \brief Check if it is ok to give SERC a chance in middle of this transaction.
 *
 * \param [in] unit Logical device id
 * \param [out] allow_serc TRUE => Ok to give chance to SERC.
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_wal_allow_serc_check(int unit, bool *allow_serc);

#endif /* BCMPTM_WAL_INTERNAL_H */
