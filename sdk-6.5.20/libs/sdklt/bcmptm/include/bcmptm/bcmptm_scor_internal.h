/*! \file bcmptm_scor_internal.h
 *
 * <description>
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SCOR_INTERNAL_H
#define BCMPTM_SCOR_INTERNAL_H

/*******************************************************************************
  Includes
 */
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmltd/bcmltd_lt_types.h>
#include <bcmptm/bcmptm_internal.h>

/*******************************************************************************
  Defines
 */
/*! Default flags */
#define BCMPTM_SCOR_REQ_FLAGS_NO_FLAGS                      0x00000000

/*! Read from HW (meaningful only for bcmptm_scor_read() */
#define BCMPTM_SCOR_REQ_FLAGS_DONT_USE_CACHE                0x00000001

/*! Dont convert xy formatted key, mask fields read from TCAM into data, mask
 * format */
#define BCMPTM_SCOR_REQ_FLAGS_DONT_CONVERT_XY2KM            0x00000002

/*! Key, mask fields inn wriie data are already in XY format */
#define BCMPTM_SCOR_REQ_FLAGS_KM_IN_XY_FORMAT               0x00000004

/*! Also write PTcache */
#define BCMPTM_SCOR_REQ_FLAGS_ALSO_WRITE_CACHE              0x00000008

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
  \brief Read entry from PTcache (or, if requested, from HW)

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior.
           \n Asserting of BCMLT_ENT_ATTR_GET_FROM_HW will result in read
           of entry from HW.
           \n Pass ZEROs for all other bits.
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
           \n Must specify index
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
           \n TBD. Pass NULL for now.
  \param [in] rsp_entry_wsize Num of words in rsp_entry_words array

  \param [out] rsp_entry_words Array large enough to hold read data

  \retval SHR_E_NONE Success.

  - Clarifications:
   -# Read will use SCHAN_PIO (and not go thro WAL)

   -# Will assume that PTcache is stable (and not being updated by another
      context like ireq, mreq, etc). Any locks, if needed, to ensure this must
      be taken by caller of this routine.

   -# Does not distinguish between counter-sid and non-counter-sid

  - Uses:
   -# Most of the time, SERC wants to read data from PTcache for SER correction.
     - Even for mems with SER_RESP_WRITE_CACHE_RESTORE, read for SER restore
       must come from PTcache (because dynamic fields like HIT, etc are don't
       care for.SER restore - they will be lost)

     - SERC must use bcmptm_scor_read() even for reads of registers - registers
       are cached in PTcache. If SERC wants to set only certain fields of
       registers (eg: to enable parity checking, etc) - it must use
       bcmptm_scor_read(), bcmptm_scor_write() routines for read-modify-write.

     - SERC has nothing to do with LTIDs, so rsp_ltid from PTcache will be
       ignored

     - Entries in PTcache are already in HW format - so rsp_dfid from PTcache
       will be ignored

     - if PTcache is not allocated at all for this SID - then this function
       will return SHR_E_UNAVAIL.

     - When reading data for TCAM tables, data from PTcache will return key,
       mask fields in DM format

   -# Sometimes, SERC may want to read data from HW and can set
      DONT_USE_CACHE flag. Example cases:
     - For error injection, SERC may want to read good data from HW (because
       entry in PTcache does not contain ECC/PARITY bits)
     - To detect hard=fault
     - Read corrupted entry for SER logging
     - Read data from good pipe and write it to bad pipe (HW-cache recovery)

     - When reading data for TCAM tables, data from HW will be in XY format and
       scor_read will, by default, convert the key, mask into DM format. If
       SERC wants xy format it can assert DONT_CONVERT_XY2DM flag

  - Implementation notes:
   - For non-overlay mems, if entry is not valid in Cache - scor_read must
     return 'null_entry'

   - For overlay mems, if 4x view was valid and SERC wants to read
     1x/2x/_ECC views - vbit in cache may be invalid - but cache_entry data
     is correct
     (also because _ECC view is never written - vbit in cache will always be
      invalid)

     - Because PTcache stores null value of entries during init, then scor_read
       can ignore vbit value and always return data from PTcache.
       This also implies that once SERC finds that restore data is not
       available in WAL - it can always use data from PTcache (does not have to
       know the 'null entry' like it did in SDK6)

   - Cache for IFP_TCAM, IFP_TCAM_WIDE is separate, so if we get read
     for IFP_TCAM view, we will provide data from IFP_TCAM view in cache. If at
     this time that slice was in wide mode then our rsp_entry_words will be
     incorrect. This means:
     - caller must be aware of current slice mode and issue appropriate read
     - Same comments for DEFIP, DEFIP_PAIR_128 views
 */
extern int
bcmptm_scor_read(int unit,
                 uint64_t flags,
                 bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *pt_dyn_info,
                 void *pt_ovrr_info,
                 size_t rsp_entry_wsize,

                 uint32_t *rsp_entry_words);

/*!
  \brief Update entry in HW (and, if requested, also in PTcache)
  - When writing entry in cache,
   -# 'cache_vbit' for this entry will be set to 1

  \param [in] unit Logical device id
  \param [in] flags Control to alter default behavior
  \n KM_IN_XY_FORMAT 1 => Key, mask fields are already in xy format
                  \n 0 => Key, mask fields are in DM format
  \n ALSO_WRITE_CACHE 1 => Also update cache entry with entry_words
                   \n 0 => Cache entry is not updated
  \param [in] sid Enum to specify reg, mem
  \param [in] pt_dyn_info Ptr to bcmbd_pt_dyn_info_t
  \param [in] pt_ovrr_info Ptr to bcmbd_pt_ovrr_info_t
           \n TBD. Pass NULL
  \param [in] entry_words Array large enough to hold write_data

  \retval SHR_E_NONE Success

  - Clarifications:
   -# Read will use SCHAN_PIO (and not go thro WAL)

   -# Will assume that PTcache is stable (and not being updated by another
     context like ireq, mreq, etc). Any locks, if needed, to ensure this must
     be taken by caller of this routine.

   -# Does not distinguish between counter-sid and non-counter-sid

  - Uses:
   -# Most of the time, SERC wants to write data only to HW for SER correction

   -# On exception basis (eg: during serc_init), write data must also be written
      to PTcache and such write will set vbit for this cache entry.

   -# ASSUMPTION: We don't expect target SIDs for such writes to have dfid, ltid
                  and so bcmptm_ptcache_update() will return SHR_E_INTERNAL if
                  SERC attempts to update cache for such SID.

   - For writes to TCAM based tables (which contain TCAM key, mask fields):
    -# If restore data is from WAL, it is already in XY format. Caller must
       assert km_in_xy_format in this case.
    -# If restore data is from PTcache, it is in DM format and needs to be
       converted to XY format
  */
extern int
bcmptm_scor_write(int unit,
                  uint64_t flags,
                  bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *pt_dyn_info,
                  void *pt_ovrr_info,
                  uint32_t *entry_words);

/*!
  \brief Threads which get stalled must wait for SERC to finish

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */
extern int
bcmptm_scor_wait(int unit);

/*!
  \brief SERC must call this after every SER correction

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */
extern int
bcmptm_scor_done(int unit);

/*!
  \brief Take ireq, locks as needed

  \param [in] unit Logical device id
  \param [in] sid Enum to specify reg, mem which encountered SER event
  \n Not used at present - but may help in deciding which locks to take.

  \retval SHR_E_NONE Success */
extern int
bcmptm_scor_locks_take(int unit,
                       bcmdrd_sid_t sid);

/*!
  \brief Take ireq, locks as needed

  \param [in] unit Logical device id
  \param [in] sid Enum to specify reg, mem which encountered SER event
  \n Not used at present - but may help in deciding which locks to take.

  \retval SHR_E_NONE Success */
extern int
bcmptm_scor_locks_give(int unit,
                       bcmdrd_sid_t sid);

/*!
  \brief Init scor related data structs
  - Alloc, init scor data structs

  \param [in] unit Logical device id
  \param [in] warm TRUE implies warmboot

  \retval SHR_E_NONE Success */

extern int
bcmptm_scor_init(int unit, bool warm);

/*!
  \brief Cleanup scor related data structs
  - De-alloc scor data structs

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_scor_cleanup(int unit);

/*!
  \brief ireq path req to take lock for interactive path

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_ireq_lock(int unit);

/*!
  \brief ireq path req to release lock for interactive path

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_ireq_unlock(int unit);

/*!
  \brief mreq path req to take lock for modeled path writer

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_lock(int unit);

/*!
  \brief mreq path req to release lock for modeled path writer

  \param [in] unit Logical device id

  \retval SHR_E_NONE Success */

extern int
bcmptm_mreq_unlock(int unit);

/*!
  \brief Return TRUE if SERC is waiting for mreq_lock

  \param [in] unit Logical device id

  \retval TRUE if SERC is waiting for mreq_lock. */
extern bool
bcmptm_scor_req_pending(int unit);

#endif /* BCMPTM_SCOR_INTERNAL_H */
