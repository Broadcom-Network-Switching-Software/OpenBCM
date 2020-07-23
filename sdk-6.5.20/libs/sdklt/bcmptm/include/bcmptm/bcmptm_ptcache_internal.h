/*! \file bcmptm_ptcache_internal.h
 *
 * Utils, defines in ptcache
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_PTCACHE_INTERNAL_H
#define BCMPTM_PTCACHE_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_ser.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm_types.h>
#include <bcmltd/bcmltd_types.h>


/*******************************************************************************
 * Defines
 */
/*! Specify 'get TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_GET 0

/*! Specify 'set TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_SET 1

/*! Specify 'clear TCAM CORRUPT bit' operation.
 *  \n tc_cmd for use in bcmptm_ptcache_tc_cmd()
 */
#define BCMPTM_TCAM_CORRUPT_CMD_CLR 2

/*! Identifies 'ctl' type tcam_info_word
 */
#define BCMPTM_TCAM_IW_TYPE_CW 0

/*! Max value of 'minbit' field in tcam_info_word of type 'field'
 *  \n (encoded with 14 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_MINBIT 16383

/*! Max value of 'maxbit' field in tcam_info_word of type 'field'
 *  \n (encoded with 14 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_MAXBIT 16383

/*! Max value of 'width_word_count' field in tcam_info_word of type 'field'
 *  \n (encoded with 14 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_WIDTH_WC 16383

/*! Max value of 'width_bit_count' field in tcam_info_word of type 'field'
 *  \n (encoded with 14 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_WIDTH_BC 16383

/*! Max value of 'word_count' field in tcam_info_word of type 'ctl'
 *  \n (encoded with 11 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_WORD_COUNT 2047

/*! Max value of 'kmf_count' field in tcam_info_word of type 'ctl'
 *  \n (encoded with 8 bits)
 */
#define BCMPTM_TCAM_IW_LIMIT_KMF_COUNT 255

/*! value of LTID when entry is read from HW
 * - never written in cache */
#define BCMPTM_LTID_RSP_HW                             0xFFFFFFFF

/*! value of LTID when cache is not allocated for this PT
 * - never written in cache */
#define BCMPTM_LTID_RSP_NOCACHE                        0xFFFFFFFE

/*! value of LTID when LTID is not supported for this PT
 * - never written in cache */
#define BCMPTM_LTID_RSP_NOT_SUPPORTED                  0xFFFFFFFD

/*! value of LTID when entry is not valid in cache
 * - never written in cache */
#define BCMPTM_LTID_RSP_INVALID                        0xFFFFFFFC

/*! value of LTID used as Sentinel
 * - can be written into, read from cache
 */
#define BCMPTM_LTID_SENTINEL                           0xFFFFFFFB

/*! 16 bit version of LTID_SENTINEL
 */
#define BCMPTM_LTID_SENTINEL16                         0xFFFB

/*! value of LTID used by interactive path
 * - can be written into, read from cache
 */
#define BCMPTM_LTID_IREQ                               0xFFFFFFFA

/*! 16 bit version of LTID_IREQ
 */
#define BCMPTM_LTID_IREQ16                             0xFFFA

/*! Reserved value of LTID
 * - never written in cache
 * - rsrc_mgr must consider values >= LTID_RSVD as invalid entries even if
 * rsp_entry_cache_vbit = TRUE */
#define BCMPTM_LTID_RSVD                               BCMPTM_LTID_IREQ

/*! value of DFID when entry is read from HW
 * - never written in cache */
#define BCMPTM_DFID_RSP_HW                             0xFFFF

/*! value of DFID when cache is not allocated for this PT
 * - PTCACHE_TYPE is NO_CACHE or CCI_ONLY
 * - never written in cache */
#define BCMPTM_DFID_RSP_NOCACHE                        0 /* 0xFFFE */

/*! value of DFID when DFID is not supported for this PT
 * - dfid_seg_en = 0 for this PT or this PT is of type SE (eg: Register)
 * - never written in cache */
#define BCMPTM_DFID_RSP_NOT_SUPPORTED                  0 /* 0xFFFD */

/*! value of DFID when entry is not valid in cache
 * - never written in cache and is of type ME (multiple entries)  */
#define BCMPTM_DFID_RSP_INVALID                        0 /* 0xFFFC */

/*! value of DFID from modelled path
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_MREQ                               0xFFFB

/*! value of DFID from interactive path
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_IREQ                               0xFFFA

/*! value of DFID from modeled path - 8b version
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_MREQ8                              (BCMPTM_DFID_MREQ & 0xFF)

/*! value of DFID from interactive path - 8b version
 * - can be written in cache, read from cache */
#define BCMPTM_DFID_IREQ8                              (BCMPTM_DFID_IREQ & 0xFF)


/*! Reserved value of DFID
 * - rsrc_mgr must consider values >= DFID_RSVD as illegal */
#define BCMPTM_DFID_RSVD                               BCMPTM_DFID_IREQ

/*! Reserved value of DFID - 8b version
 * - only used by ptcache */
#define BCMPTM_DFID_RSVD8                              (BCMPTM_DFID_IREQ & 0xFF)


/*******************************************************************************
 * Typedefs
 */
/*! Definition of bits in info_words for tcam type pt
 * \n Needed when converting (key, mask) to XY format and vice-versa.
 */
typedef union bcmptm_tcam_iw_fi_s {
    /* usage: store field info */

    /*! Format for field info word0, word1 */
    struct fi01_s {
        /*! spare */
        uint32_t spare:4;

        /*! first bit for this field in entry */
        uint32_t minbit:14;

        /*! last bit for this field in entry */
        uint32_t maxbit:14;
    } fi01; /*!< field info word0, word1 */

    /*! Format for field info word2 */
    struct fi2_s {
        /*! spare */
        uint32_t spare:4;

        /*! width of this field in 32 bit words */
        uint32_t width_word_count:14;

        /*! width of this field in bits */
        uint32_t width_bit_count:14;
    } fi2; /*!< field info word2 */


    /*! Format for ctl info word */
    struct cw_s {
        /*! type of info in this word */
        uint32_t type:3;

        /*! spare */
        uint32_t spare:10;

        /*! num of key, mask field pairs in info_array */
        uint32_t kmf_count:8;

        /*! num of words in info_array - only for DEBUG */
        uint32_t word_count:11;
    } cw; /*!< ctl info word */

    /*! Format for raw entry word */
    uint32_t entry;
} bcmptm_tcam_iw_fi_t;

/*! Information for counter mems from ptcache */
typedef struct bcmptm_ptcache_ctr_info_s {
    /*! TRUE if sid is counter */
    bool pt_is_ctr;

    /*!  TRUE if counter sid is CCI_ONLY */
    bool ctr_is_cci_only;

    /*! cci_map_id */
    uint32_t cci_map_id;
} bcmptm_ptcache_ctr_info_t;


/*******************************************************************************
 * Global variables
 */


/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Allocate and initialize PTcache for a unit
 * \n Must be called every time a new unit is attached and table sizes have been
 * determined, configured.
 *
 * \param [in] unit Logical device id
 * \param [in] warm TRUE => warmboot, FALSE => coldboot
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_init(int unit, bool warm);

/*!
 * \brief Deallocate PTcache for a unit
 *
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_ptcache_cleanup(int unit);

/*!
 * \brief Determine if PT is ctr, whether it is completely managed by CCI
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
 * \param [in] sid Enum to specify reg, mem
 * \param [out] ctr_info Refer to bcmptm_ptcache_ctr_info_t defn
 *
 * \retval SHR_E_NONE Success. May or may not be counter.
 */
extern int
bcmptm_ptcache_ctr_info_get(int unit, bool acc_for_ser,
                            bcmdrd_sid_t sid,
                            bcmptm_ptcache_ctr_info_t *ctr_info);

/*!
 * \brief Read a mem or reg from PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
 * \param [in] flags Additional controls to modify behavior.
 * \n When BCMPTM_REQ_FLAGS_PTCACHE_RETURN_ENTRY_P is asserted, pointer to entry
 * words in PTcache will be returned instead of copying entry words into
 * rsp_entry_words. In such case, rsp_entry_words is expected to
 * point to (pointer to uint32_t). This feature is supported only for SIDs that
 * are not overlays (narrow) views of wider PTs.
 * \n When this flag is not asserted, caller is requesting entry_read_data from
 * PTcache to be copied into rsp_entry_words buffer. In this case,
 * rsp_entry_words is expected to point to 1st element in uint32_t array.
 *
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is returned from one of the instances
 * \param [in] entry_count Number of consecutive entries to fetch.
 * \n rsp_cache_vbit is returned only for first entry in the list.
 * \n Must be 1 for tables for which have single entry / LTID / DFID or are
 * narrow overlay views of wider tables.
 * \param [in] rsp_entry_wsize Num of words in rsp_entry_words array.
 *
 * \param [out] rsp_entry_words Array large enough to hold read data. See
 * description of flags above.
 * \param [out] rsp_entry_cache_vbit 'cache_vbit (vbit)' state of entry in cache
 * \param [out] rsp_entry_dfid dfield_format_id of entry in cache. Meaningful
 * only if rsp_entry_cache_vbit is TRUE.
 * \param [out] rsp_entry_ltid LTid of previous writer. Meaningful only if
 * rsp_entry_cache_vbit is TRUE.
 *
 * \retval SHR_E_NONE Success (entry may be valid or not valid in cache)
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT. rsp_entry_words are
 * not correct.
 */
extern int
bcmptm_ptcache_read(int unit,
                    bool acc_for_ser,
                    uint64_t flags,
                    bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info,
                    uint32_t entry_count,
                    size_t rsp_entry_wsize,
                    void *rsp_entry_words,
                    bool *rsp_entry_cache_vbit,
                    uint16_t *rsp_entry_dfid,
                    bcmltd_sid_t *rsp_entry_ltid);

/*!
 * \brief Write a mem or reg in PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is written to ALL instances
 * \param [in] entry_count Number of consecutive entries to update.
 * set_cache_vbit applies to all consecutive entries. User must ensure it has
 * filled entry words correctly.
 * \n Must be 1 for tables for which have single entry / LTID / DFID or are
 * narrow overlay views of wider tables.
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] set_cache_vbit 1 => Make cache_vbit TRUE for this cache_entry
 *                         \n 0 => Make cache_vbit FALSE for this cache_entry
 * \param [in] req_ltid LTid of Current writer
 * \param [in] req_dfid Data field format id
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT
 */
extern int
bcmptm_ptcache_update(int unit,
                      bool acc_for_ser,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      uint32_t entry_count,
                      uint32_t *entry_words,
                      bool set_cache_vbit,
                      bcmltd_sid_t req_ltid,
                      uint16_t req_dfid);

/*!
 * \brief Write a mem or reg in PTcache on Interactive path
 * Will first read entry in cache. If entry is valid in cache, then it will
 * preserve LTID,DFID. If entry is not valid in cache, it will write req_ltid,
 * req_dfid in cache.
 * Is needed to allow modification of modeled SIDs on interactive path (for
 * debug).
 *
 * \param [in] unit Logical device id
 * \param [in] acc_for_ser TRUE means access is by SER logic. FALSE means access
 * is by non-SER logic (RMs, WAL, CCI, etc)..
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n If (tbl_inst < 0) data is written to ALL instances
 * \param [in] entry_words Array large enough to hold write_data
 * \param [in] set_cache_vbit 1 => Make cache_vbit TRUE for this cache_entry
 *                         \n 0 => Make cache_vbit FALSE for this cache_entry
 * \param [in] req_ltid LTid of Current writer
 * \param [in] req_dfid Data field format id
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache is not allocated for this PT
 */
extern int
bcmptm_ptcache_update_ireq(int unit,
                           bool acc_for_ser,
                           bcmdrd_sid_t sid,
                           bcmbd_pt_dyn_info_t *dyn_info,
                           uint32_t *entry_words,
                           bool set_cache_vbit,
                           bcmltd_sid_t req_ltid,
                           uint16_t req_dfid);

/*!
 * \brief Access TCAM CORRUPT BIT for a mem in PTcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 * \param [in] dyn_info Ptr to bcmbd_pt_dyn_info_t
 *             \n tbl_inst is 0-based instance (copy) number
 *             \n For Writes, (tbl_inst < 0) => bit is updated for ALL instances
 *             \n For Reads, (tbl_inst < 0) => bit is read from last instance
 * \param [in] tc_cmd Refer to encoding of BCMPTM_TCAM_CORRUPT_CMD_XXX
 *
 * \param [out] prev_tc_rdata TCAM CORRUPT BIT data (before update, if any)
 *                            \n NonZero => 1, Zero => 0
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL Cache or TCAM CORRUPT bits is not allocated for this PT
 */
extern int
bcmptm_ptcache_tc_cmd(int unit,
                      bcmdrd_sid_t sid,
                      bcmbd_pt_dyn_info_t *dyn_info,
                      uint32_t tc_cmd,
                      uint32_t *prev_tc_rdata);

/*!
 * \brief Fetch base_index for info_seg in ptcache
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum to specify reg, mem
 *
 * \param [out] iw_base_ptr Pointer to first info_word in vinfo.info_seg
 *
 * \retval SHR_E_NONE Success
 * \retval SHR_E_UNAVAIL info_words are not allocated for this sid
 */
extern int
bcmptm_ptcache_iw_base(int unit,
                       bcmdrd_sid_t sid,
                       uint32_t **iw_base_ptr);

/*!
 * \brief Convert (key, mask) fields of TCAM table to HW specific XY format.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] entry_count Num of consecutive entries to converted
 * \param [in] km_entry Ptr to 1st entry in (key, mask) format
 *
 * \param [out] xy_entry Ptr to 1st entry in (x, y) format
 *              \n xy_entry can be same as km_entry (REPLACE)
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_pt_tcam_km_to_xy(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *km_entry,
                        uint32_t *xy_entry);

/*!
 * \brief Convert (key, mask) fields of TCAM from HW specific XY to (key, mask) format.
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] entry_count Num of consecutive entries to converted
 * \param [in] xy_entry Ptr to 1st entry in (x, y) format
 *
 * \param [out] km_entry Ptr to 1st entry in (key, mask) format
 *              \n km_entry can be same as xy_entry (REPLACE)
 *
 * Returns:
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_pt_tcam_xy_to_km(int unit,
                        bcmdrd_sid_t sid,
                        uint32_t entry_count,
                        uint32_t *xy_entry,
                        uint32_t *km_entry);


/*!
 * \brief Find info_word count required for a PT
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [out] iw_count Non-zero value means info_words needed.
 *
 * Returns:
 * \retval SHR_E_NONE
 */
extern int
bcmptm_pt_iw_count(int unit,
                   bcmdrd_sid_t sid, uint32_t *iw_count);

/*!
 * \brief Fill info_seg with info_words
 * \n
 *
 * \param [in] unit Logical device id
 * \param [in] sid Enum for PT
 * \param [in] iw_base_ptr Pointer to first info_word in vinfo.info_seg
 *
 * Returns:
 * \retval SHR_E_NONE Success.
 */
extern int
bcmptm_pt_iw_fill(int unit,
                  bcmdrd_sid_t sid,
                  uint32_t *iw_base_ptr);

#endif /* BCMPTM_PTCACHE_INTERNAL_H */
