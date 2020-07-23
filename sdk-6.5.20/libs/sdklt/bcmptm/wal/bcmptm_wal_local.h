/*! \file bcmptm_wal_local.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_WAL_LOCAL_H
#define BCMPTM_WAL_LOCAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_mutex.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_internal.h>


/*******************************************************************************
 * Defines
 */


/*******************************************************************************
 * Typedefs
 */
/*! \brief wal_msg_cmd types */
typedef enum {
    BCMPTM_WAL_MSG_CMD_NOP = 0, /* can be used as marker to detect empty WAL, etc */
    BCMPTM_WAL_MSG_CMD_WRITE,
    BCMPTM_WAL_MSG_CMD_EMPTY_WRITE,
    BCMPTM_WAL_MSG_CMD_SLAM,
    BCMPTM_WAL_MSG_CMD_THREAD_EXIT       /* stop thread */
} bcmptm_wal_msg_cmd_t;

/*! \brief slice_mode encoding in uint32_t */
typedef union bcmptm_wal_slice_mode_info_s {
    struct {
        /*! 1st slice num */
        uint32_t slice_num0:10;

        /*! 2nd slice num */
        uint32_t slice_num1:10;

        /*! new slice_mode */
        uint32_t slice_mode:3;

        /*! 1 => This msg contains slice_mode_change op */
        uint32_t slice_change_en:1;

        /*! Up to 8 groups */
        uint32_t slice_group_id:3;

        /*! 1 => This msg contains entry_mode_change ops */
        uint32_t entry_change_en:1;

        /*! spare */
        uint32_t spare:4;
    } f;
    uint32_t entry;
} bcmptm_wal_slice_mode_info_t;


/*! \brief Info for individual ops to be sent to HW */
typedef struct bcmptm_wal_ops_info_s {
/*! \brief Number of words to be sent to HW for each op
 * - For Write - it includes opcode, addr, data beats
 * - When SCHAN_FIFO mode is disabled, BD will use SCHAN_PIO to drain ops. This
 * param helps BD to delineate individual ops in req_entry_words array.
 * - Dont_care for SLAM */
    uint16_t op_wsize;

/*! \brief Number of hdr words for each op
 * - For Write - it includes opcode, addr
 * - When SCHAN_FIFO mode is disabled, BD will use SCHAN_PIO to drain ops. This
 * param helps BD to delineate individual ops in req_entry_words array.
 * - Dont_care for SLAM */
    uint8_t op_ctrl_wsize;

/*! \brief SID for individual ops
 * - Specifies SID for SLAM cmd
 */
    bcmdrd_sid_t sid;

/*! \brief Dyn_info for individual ops
 * - Specifies index, tbl_inst (pipe) for each op
 * - Specifies Frist_index, tbl_inst (pipe) for SLAM operation
 */
    bcmbd_pt_dyn_info_t dyn_info;

/*! \brief Additional ops_info needed by cbf.
 * - To carry bcmptm_rm_op_t, etc.
 * - Not for SCF use.
 * - Dont_care for SLAM */
    bcmptm_rm_op_t op_type;
} bcmptm_wal_ops_info_t;

/*! \brief Format of msg (group of ops).
 *
 * - Will use offsets for pointers in HA space
 * - Must be able to recreate bcmptm_walr_msg in case of crash
 * - can have additional info to help with release of buffers
 */
typedef struct bcmptm_wal_msg_s {
/*! \brief Refer to defn for wal_slice_mode_info_t */
    bcmptm_wal_slice_mode_info_t smc_word;

/* \brief Index into Undo (prev_data) information */
    uint32_t undo_ops_info_idx;  /* 1st undo_op */

/* \brief 1st undo_word */
    uint32_t undo_words_buf_idx; /* 1st undo_word */

/* \brief Number of undo_words */
    uint32_t undo_num_words;

/* \brief Link to parent trans and thus the link to all msgs of this trans */
    uint32_t trans_idx;

/*! \brief Must be released when msg is sent to HW.
 * Can be really huge when SLAM entry count is large. */
    uint32_t num_words_skipped;

/*! \brief Unique msg ID to help with debug.
 * Non_zero implies msg contents are meaningful.
 */
    uint32_t umsg_idx;

/*! \brief Need for scf to call cbf() BEFORE any op for msg is sent to HW.
 * - Will be set by WAL writer.
 * - Will be cleared by cbf in WAL reader context */
    bool need_cbf_before_msg;

/*! \brief Refer to defn for bcmptm_walr_cmd_t */
    bcmptm_wal_msg_cmd_t cmd;

/*! \brief ptr to req_entry_words (uint32_t array)
 * - For WRITE: it contains {opcode, address, data_words} for multiple op packed
 * consecutively in single DMAable buffer.
 * - For SLAM: it contains {data_words} for multiple locations packed
 * consecutively in single DMAable buffer.
 */
    uint32_t words_buf_idx;

/*! \brief Num of words in req_entry_words array
 * - Specifies number of elements in req_entry_words array.
 * - Contains words_buf_count for SLAM. */
    uint32_t req_entry_wsize;

/*! \brief Num of individual ops in msg.
 * - Specifies number of elements in ops_info array
 *  -# WRITE: Number of elements in ops_info array
 *  -# SLAM: There is only 1 op - so use it to store entry_count. */
    uint32_t num_ops;

/*! \brief Info for ops in msg sent to WAL reader context */
    uint32_t ops_info_idx;

/*! \brief For error checking */
    bool committed;

/*! \brief True means this is last msg in transaction. */
    bool last_msg;
} bcmptm_wal_msg_t;

/*! \brief Format of status returned once ops for msg are sent to HW */
typedef struct bcmptm_walr_status_s {
/*! \brief rv */
    int rv;

/*! \brief Num of ops that went to HW without problem */
    uint32_t num_good_ops;

/*! \brief Num of req_words that went to HW without problem */
    uint32_t good_req_words_wsize;
} bcmptm_walr_status_t;


/*******************************************************************************
 * External global vars
 */
extern uint64_t bcmptm_wal_words_buf_bpa[BCMDRD_CONFIG_MAX_UNITS];
extern uint32_t *bcmptm_wal_words_buf[BCMDRD_CONFIG_MAX_UNITS];
extern bcmptm_wal_ops_info_t *bcmptm_wal_ops_info[BCMDRD_CONFIG_MAX_UNITS];

extern bcmptm_wal_msg_t *bcmptm_wal_msg_bp[BCMDRD_CONFIG_MAX_UNITS];

extern bool *bcmptm_wal_msg_dq_ready_bp[BCMDRD_CONFIG_MAX_UNITS];
extern bool *bcmptm_wal_msg_dq_ready_p[BCMDRD_CONFIG_MAX_UNITS];

extern uint32_t bcmptm_wal_msg_dq_idx[BCMDRD_CONFIG_MAX_UNITS];
extern bcmptm_wal_msg_t *bcmptm_wal_msg_dq_p[BCMDRD_CONFIG_MAX_UNITS];
extern int bcmptm_wal_scf_num_chans[BCMDRD_CONFIG_MAX_UNITS];


/*******************************************************************************
 * Function prototypes
 */
/*!
  \brief Function to be called when ops are sent to HW by WAL rdr thread.
  \param [in] unit Logical device id
  \param [in] wal_msg_ptr Message handle for group of ops sent to HW.
  \param [in] scf_status Status of sending ops to HW.

  \retval SHR_E_NONE Success in releasing of resources, calling of notification
  as needed..
  */
extern int
bcmptm_wal_rdr_msg_done(int unit, bcmptm_wal_msg_t *wal_msg_ptr,
                        bcmptm_walr_status_t *scf_status);

/*!
  \brief Function to be called by WAL rdr thread to know next msg.
  \param [in] unit Logical device id
  \param [in] c_msg_idx Current offset into message database.
  \param [in] adjust_dq_p Adjust bcmptm_wal_msg_dq_ready_p[u],
  bcmptm_wal_msg_dq_p[u].

  \retval Offset for next message to be processed by WAL rdr thread.
  */
extern uint32_t
bcmptm_wal_next_msg_idx(int unit, uint32_t c_msg_idx, bool adjust_dq_p);

#endif /* BCMPTM_WAL_LOCAL_H */
