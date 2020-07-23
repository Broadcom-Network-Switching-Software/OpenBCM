/*! \file bcmptm_internal.h
 *
 * Utils, defines shared across BCMPTM sub-modules
 *
 * This file contains utils, defines shared across PTM sub-modules
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_INTERNAL_H
#define BCMPTM_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_chip_internal.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */
/*! Size of buffer allocated to store entry data words for any PT.
 */
#define BCMPTM_MAX_PT_ENTRY_WORDS 40     /* 1280 bit wide mem */

/*! Size of buffer allocated to store control words (opcode beat, address beat,
 * etc.)
 */
#define BCMPTM_MAX_BD_CWORDS 4

/*! Size of buffer allocated to store field words of an entry for any PT
 */
#define BCMPTM_MAX_PT_FIELD_WORDS BCMPTM_MAX_PT_ENTRY_WORDS

/*! Size of buffer allocated to store words received from LTM for modeled req
 */
#define BCMPTM_MAX_MREQ_ENTRY_WORDS 40     /* 1280 bit wide mem */


/*! Convert number of bits to number of 32 bit words.
 */
#define BCMPTM_BITS2WORDS(x) (((x) + 31) / 32)

/*! Convert number of bytes to number of 32 bit words.
 * - Example usage: determine num of 32 bit words required to store elements of
 * size uint8_t - DFID, etc
 */
#define BCMPTM_BYTES2WORDS(x) (((x) + 3) / 4)

/*! Convert number of double_bytes to number of 32 bit words.
 * - Example usage: determine num of 32 bit words required to store elements of
 * size uint16_t - LTID, DFID, etc
 */
#define BCMPTM_DBYTES2WORDS(x) (((x) + 1) / 2)

/*! Debug macro to allow force display of verbose msgs from PTM.
 * \n For debug, we can change RHS to LOG_ERROR.
 */
#define BCMPTM_LOG_VERBOSE LOG_VERBOSE

/*! Macro to set, clr, test a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_OP(vmap, index, BOOL_OP) \
        ((vmap) BOOL_OP (1 << ((index) % 32)))

/*! Macro to set a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_SET(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, |=)

/*! Macro to clr a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_CLR(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, &= ~)

/*! Macro to test a bit in 32 bit word.
 */
#define BCMPTM_WORD_BMAP_TST(vmap, index) BCMPTM_WORD_BMAP_OP(vmap, index, &)

/*! Macro to test if SHR error code implies SER event
 */
#define BCMPTM_SHR_FAILURE_SER(_expr) ((_expr) == SHR_E_FAIL)

/*! Max number of Mode_Change groups supported per unit
 */
#define BCMPTM_RM_MC_GROUP_COUNT_MAX 4

/*! Group of PTs for which size (number of entries) is fixed.
 * All devices have this group - so min group_count for all devices is 1. */
#define BCMPTM_PT_GROUP_DEFAULT 0

/*! Max number of groups that PTcache can manage per unit.
 * Limited by number of HA_SUBID supported per unit */
#define BCMPTM_PT_GROUP_COUNT_MAX 96

/*! Define for UFT managed EM group change event. */
#define BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT "bcmptmEvUftGrpChgCommit"
/*! Define for UFT bank change event. */
#define BCMPTM_EVENT_UFT_BANK_CHANGE_COMMIT "bcmptmEvUftBankChgCommit"
/*! Define for UFT tile change event. */
#define BCMPTM_EVENT_UFT_TILE_CHANGE_COMMIT "bcmptmEvUftTileChgCommit"
/*! Define for UFT LT move depth event. */
#define BCMPTM_EVENT_UFT_LT_MOVEDEPTH_COMMIT "bcmptmEvUftLtMoveDepthChg"

/*! HA signatures for PTCACHE_SINFO */
#define BCMPTM_HA_SIGN_PTCACHE_SINFO         0xABCA0001
/*! HA signatures for PTCACHE_VINFO */
#define BCMPTM_HA_SIGN_PTCACHE_VINFO         0xABCA0002
/*! HA signatures for PTCACHE_LTID_SEG */
#define BCMPTM_HA_SIGN_PTCACHE_LTID_SEG      0xABCA0003
/*! HA signatures for PTCACHE_CWME_SEG */
#define BCMPTM_HA_SIGN_PTCACHE_CWME_SEG      0xABCA0004
/*! HA signatures for PTCACHE_PT_GROUP_INFO */
#define BCMPTM_HA_SIGN_PTCACHE_PT_GROUP_INFO 0xABCA0005
/*! HA signatures for PTCACHE_SEDS */
#define BCMPTM_HA_SIGN_PTCACHE_SEDS          0xABCA0006


/*! HA signatures for WAL_TRANS */
#define BCMPTM_HA_SIGN_WAL_TRANS             0xABCB1231
/*! HA signatures for WAL_MSG */
#define BCMPTM_HA_SIGN_WAL_MSG               0xABCB1232
/*! HA signatures for WAL_OPS_INFO */
#define BCMPTM_HA_SIGN_WAL_OPS_INFO          0xABCB1233
/*! HA signatures for WAL_UNDO_OPS_INFO */
#define BCMPTM_HA_SIGN_WAL_UNDO_OPS_INFO     0xABCB1234
/*! HA signatures for WAL_UNDO_WORDS_BUF */
#define BCMPTM_HA_SIGN_WAL_UNDO_WORDS_BUF    0xABCB1235
/*! HA signatures for WAL_WSTATE */
#define BCMPTM_HA_SIGN_WAL_WSTATE            0xABCB1236
/*! HA signatures for WAL_WORDS_BUF */
#define BCMPTM_HA_SIGN_WAL_WORDS_BUF         0xABCB1237

/*! HA signatures for LT_OFFSET_INFO */
#define BCMPTM_HA_SIGN_LT_OFFSET_INFO        0xABCD0001


/*******************************************************************************
 * Typedefs
 */
/*! Sub-phases */
typedef enum {
    BCMPTM_SUB_PHASE_1 = 1, /* 1st scan of LTID/SID. Notify RM */
    BCMPTM_SUB_PHASE_2 = 2, /* 2nd scan of LTID/SID. RM will return offset */
    BCMPTM_SUB_PHASE_3 = 3  /* No scan of LTID/SID. RM can do extra init */
} bcmptm_sub_phase_t;

/*! HA sub-module IDs for PTM component */
typedef enum {
    BCMPTM_HA_SUBID_WAL_TRANS      = 1,
    BCMPTM_HA_SUBID_WAL_MSG        = 2,
    BCMPTM_HA_SUBID_WAL_WSTATE     = 3,
    BCMPTM_HA_SUBID_WAL_OPS_INFO   = 4,
    BCMPTM_HA_SUBID_WAL_WORDS_BUF  = 5,
    BCMPTM_HA_SUBID_UNDO_WORDS_BUF = 6,
    BCMPTM_HA_SUBID_UNDO_OPS_INFO  = 7,

    BCMPTM_HA_SUBID_CCI_INFO = 16,
    BCMPTM_HA_SUBID_CCI_MAPID = 17,
    BCMPTM_HA_SUBID_CCI_CTR_CACHE = 18,

    BCMPTM_HA_SUBID_PTCACHE_SINFO = 60,
    BCMPTM_HA_SUBID_PTCACHE_PT_GROUP_INFO = 61,
    BCMPTM_HA_SUBID_PTCACHE_SEDS = 62,
    BCMPTM_HA_SUBID_PTCACHE_CWME_SEG = 63,

    /* 96 SUBIDs (64 - 160) for VINFO segments */
    BCMPTM_HA_SUBID_PTCACHE_VINFO = 64,

    /* 96 SUBIDs (160 - 255) for LTID segments */
    BCMPTM_HA_SUBID_PTCACHE_LTID_SEG = 160
} bcmptm_ha_sub_id_t;

/*! HA sub-module IDs for SER component */
typedef enum {
    BCMPTM_HA_SUBID_SER_SLICES_INFO = 1,
    BCMPTM_HA_SUBID_SER_OVERLAY_BITMAP,
    BCMPTM_HA_SUBID_SER_MISC_INFO
} bcmptm_ha_sub_id_ser_t;

/*! HA sub-module IDs for UFT component */
typedef enum {
    BCMPMT_HA_SUBID_UFT_GRP_BANK_BMP = 1,
    BCMPMT_HA_SUBID_UFT_FREE_BANK_BMP,
    BCMPMT_HA_SUBID_UFT_BANK_OFFSET,
    /*! UFT-MGR SBR profile table state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_PT_STATE,
    /*! UFT-MGR SBR profile table entries state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_PT_ESTATE,
    /*! UFT-MGR SBR tiles state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_TILE_STATE,
    /*! UFT-MGR SBR tiles LT lookup entries state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_TILE_LT_LKUP_STATE,
    /*! UFT-MGR SBR variant SBR profile table state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_VAR_SPT_STATE,
    /*! UFT-MGR SBR variant tile ID state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_VAR_TILE_ID_STATE,
    /*! UFT-MGR SBR variant tile LT.LOOKUP state HA block. */
    BCMPMT_HA_SUBID_UFT_SBR_VAR_TILE_LT_LOOKUP_STATE
} bcmptm_ha_sub_id_uft_t;

/*! HA sub-module IDs for RM_ALPM component */
typedef enum {
    BCMPTM_HA_SUBID_RM_ALPM = 1,
    BCMPTM_HA_SUBID_RM_ALPM_L1,
    BCMPTM_HA_SUBID_RM_ALPM_LN,
    BCMPTM_HA_SUBID_M_RM_ALPM,
    BCMPTM_HA_SUBID_M_RM_ALPM_L1,
    BCMPTM_HA_SUBID_M_RM_ALPM_LN
} bcmptm_ha_sub_id_rm_alpm_t;

/*! Transaction commands for rsrc_mgr */
typedef enum {
    BCMPTM_TRANS_CMD_IDLE_CHECK = 1,
    BCMPTM_TRANS_CMD_COMMIT,
    BCMPTM_TRANS_CMD_ABORT,
    BCMPTM_TRANS_CMD_ATOMIC_TRANS_ENABLE,
    BCMPTM_TRANS_CMD_ATOMIC_TRANS_DISABLE,
} bcmptm_trans_cmd_t;

/*! \brief Information passed to SER logic when changing entry mode */
typedef struct bcmptm_rm_entry_mode_change_s {
    /*! Index into narrow, wide view */
    uint32_t index;

    /*! Helps identify narrow, wide view.
     * When WAL reader calls entry_mode_change() API, it will NOT provide
     * op_type = NORMAL or SLICE_MODE_CHANGE.
     * If it does, then SERC logic must return SHR_E_UNAVAIL to caller. */
    bcmptm_rm_op_t op_type;
} bcmptm_rm_entry_mode_change_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Write a mem or reg internal to SOC - Interactive path.
 * - This API is for use by CCI ONLY.
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
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
bcmptm_ireq_write_cci(int unit,
                      bool acc_for_ser,
                      uint64_t flags,
                      bcmdrd_sid_t sid,
                      void *pt_dyn_info,
                      void *pt_ovrr_info,
                      uint32_t *entry_words,
                      bcmltd_sid_t req_ltid,

                      bcmltd_sid_t *rsp_ltid,
                      uint32_t *rsp_flags);

/*!
 * \brief Read a mem or reg internal to SOC - Interactive path.
 * \n Intended for CCI only.
 * \n By default data is read from PTCache for cacheable PTs.
 * \n Will first try reading from PTcache and then from HW.
 * \n Will NOT do TCAM(x,y) to (k,m) conversion.
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
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
bcmptm_ireq_read_cci(int unit,
                     bool acc_for_ser,
                     uint64_t flags,
                     bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *pt_dyn_info,
                     void *pt_ovrr_info,
                     size_t rsp_entry_wsize,
                     bcmltd_sid_t req_ltid,

                     uint32_t *rsp_entry_words,
                     bcmltd_sid_t *rsp_ltid,
                     uint32_t *rsp_flags);

/*!
  \brief Init modeled path data structs
  - Alloc, init modeled path data structs

  \param [in] unit Logical device id
  \param [in] warm TRUE implies warmboot

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_init(int unit, bool warm);

/*!
  \brief Stop modeled path
  - Will wait for all msgs in WAL to drain, and then disable WAL rdr thread

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_stop(int unit);

/*!
  \brief Cleanup modeled path
  - De-alloc modeled path data structs

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_cleanup(int unit);

/*!
  \brief Alloc mem for rsrc_mgr traverse

  \param [in] unit Logical device id
  \param [in] sz Number of bytes to alloc
  \param [in] s Text string for tracking purposes
  \param [in] ptr Pointer to allocated mem or NULL on error

  \retval SHR_E_NONE Success */
extern int
bcmptm_rm_traverse_alloc(int unit, size_t sz, char *s, void **ptr);

/*!
  \brief Free mem for rsrc_mgr traverse

  \param [in] unit Logical device id
  \param [in] ptr Pointer to mem block to be freed

  \retval SHR_E_NONE Success */
extern int
bcmptm_rm_traverse_free(int unit, void *ptr);

/*!
  \brief Read entry_data for write_only aggregate type mem/reg from HW.
  This can be called from both modeled, interactive path (simultaneously).

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
           \n BCMPTM_REQ_FLAGS_DONT_CONVERT_XY2KM can be asserted to get raw
           data for TCAM type mems.
  \param [in] sid Enum to specify writer-only aggr reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
  \param [in] wo_aggr_info Ptr to bcmptm_wo_aggr_info_t
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Array large enough to hold read data
  \param [out] ser_failure TRUE means read encountered SER event.
  \param [out] retry_needed TRUE means should retry the read for which SER event
  was encountered.

  \retval SHR_E_NONE Success
  */
extern int
bcmptm_wo_aggr_read_hw(int unit, uint64_t flags,
                       bcmdrd_sid_t sid,
                       bcmbd_pt_dyn_info_t *pt_dyn_info,
                       void *pt_ovrr_info,
                       bcmptm_wo_aggr_info_t *wo_aggr_info,
                       size_t rsp_entry_wsize,

                       uint32_t *rsp_entry_words,
                       bool *ser_failure,
                       bool *retry_needed);

/*!
  \brief Query DRD and return number of SIDs relevant for PTcache, SER.

  \param [in] unit Logical device id

  \param [out] sid_count Number of PTcache relevant SIDs.

  \retval SHR_E_NONE Success
  */
extern int
bcmptm_pt_sid_count_get(int unit, size_t *sid_count);

#endif /* BCMPTM_INTERNAL_H */
