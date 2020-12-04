/*! \file bcm56780_a0_tm_ts_tod.c
 *
 * File containing timestamp ToD related defines and internal function for
 * bcm56780_a0.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <bcmtm/bcmtm_ts_tod.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmtm/bcmtm_pt_internal.h>
#include <bcmdrd/chip/bcm56780_a0_enum.h>
#include <bcmbd/bcmbd_ts.h>
#include "bcm56780_a0_tm_ts_tod.h"

/******************************************************************************
 * Local definitions
 */
#define BSL_LOG_MODULE  BSL_LS_BCMTM_CHIP

/******************************************************************************
 * Private functions
 */

/*! \brief TM TS ToD auto updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logiacl table ID.
 * \param [in] entry TS ToD entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_ts_tod_auto(int unit,
                           bcmltd_sid_t ltid,
                           bcmtm_ts_tod_entry_t *entry)
{
    uint32_t ltmbuf[3] = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint64_t timestamp = 0;
    bool enable = entry->hw_update;
    bcmtm_pt_info_t pt_info = {0};
    uint32_t fval[2]; /* time stamp are 48 bits */
    uint32_t val;
    int rv;

    SHR_FUNC_ENTER(unit);

    if (enable) {
        /* Get TS1 counter value. */
        rv = bcmbd_ts_timestamp_nanosec_get(unit, 1, &timestamp);
        if (rv == SHR_E_DISABLED) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Timestamp counter is not enabled \n")));
        }

        /* Assign current Timestamp value. */
        sid = MMU_GLBCFG_TOD_TIMESTAMPm;
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

        fval[1] = timestamp >> 32;
        fval[0] = timestamp & 0xffffffff;

        fid = TIMESTAMP_TA_NSECf;
        SHR_IF_ERR_EXIT
            (bcmtm_field_set(unit, sid, fid, ltmbuf, fval));


        /* Update seconds and nano seconds from user configured values. */
        if (entry->sec != 0xffffffff) {
            fid = TIMESTAMP_TOD_SECf;
            val = entry->sec;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));
        }

        if (entry->nsec != 0xffffffff) {
            fid = TIMESTAMP_TOD_NSECf;
            val = entry->nsec;
            SHR_IF_ERR_EXIT
                (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));
        }
        /* Update the physical table. */
        SHR_IF_ERR_EXIT
            (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
    }
    /* Enable/disable Hardware update. */
    sal_memset(ltmbuf, 0, COUNTOF(ltmbuf) * sizeof(uint32_t));
    sid = MMU_GLBCFG_MISCCONFIGr;
    fid = TOD_HW_UPDATE_ENf;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    val = entry->hw_update;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}


/*! \brief TM TS ToD updates.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logiacl table ID.
 * \param [in] entry TS ToD entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_ts_tod_update(int unit,
                             bcmltd_sid_t ltid,
                             bcmtm_ts_tod_entry_t *entry)
{
    uint32_t ltmbuf[3] = {0};
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t fval[2] = {0};
    uint32_t val;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    if (entry->hw_update) {
        SHR_EXIT();
    }

    sid = MMU_GLBCFG_TOD_TIMESTAMPm;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));

    fid = TIMESTAMP_TOD_NSECf;
    val = entry->nsec;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = TIMESTAMP_TOD_SECf;
    val = entry->sec;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, &val));

    fid = TIMESTAMP_TA_NSECf;
    fval[1] = entry->adjust >> 32;
    fval[0] = entry->adjust & 0xffffffff;
    SHR_IF_ERR_EXIT
        (bcmtm_field_set(unit, sid, fid, ltmbuf, fval));

    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_write(unit, sid, ltid, &pt_info, ltmbuf));
exit:
    SHR_FUNC_EXIT();
}

/*! \brief TM TS ToD lookup.
 *
 * \param [in] unit Logical unit number.
 * \param [in] ltid Logiacl table ID.
 * \param [in] entry TS ToD entry.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_tm_ts_tod_get(int unit,
                          bcmltd_sid_t ltid,
                          bcmtm_ts_tod_entry_t *entry)
{
    bcmdrd_sid_t sid;
    bcmdrd_fid_t fid;
    uint32_t ltmbuf[3] = {0};
    uint32_t fval[2] = {0}, val = 0;
    bcmtm_pt_info_t pt_info = {0};

    SHR_FUNC_ENTER(unit);

    sid = MMU_GLBCFG_MISCCONFIGr;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    fid = TOD_HW_UPDATE_ENf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &val));

    /* No need to populate in case of hardware update disable. */
    if (val == 0) {
        SHR_EXIT();
    }

    entry->hw_update = val;
    sid = MMU_GLBCFG_TOD_TIMESTAMPm;
    SHR_IF_ERR_EXIT
        (bcmtm_pt_indexed_read(unit, sid, ltid, &pt_info, ltmbuf));
    fid = TIMESTAMP_TA_NSECf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, fval));
    entry->adjust = (uint64_t)fval[1] << 32 | fval[0];


    sid = MMU_GLBCFG_UTC_TIMESTAMPr;
    fid = UTC_TIMESTAMP_NSECf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &val));
    entry->nsec = val;

    fid = UTC_TIMESTAMP_SECf;
    SHR_IF_ERR_EXIT
        (bcmtm_field_get(unit, sid, fid, ltmbuf, &val));
    entry->sec = val;

exit:
    SHR_FUNC_EXIT();
}


/******************************************************************************
 * Public functions
 */
int
bcm56780_a0_tm_ts_tod_drv_get(int unit, void *ts_tod_drv)
{
    bcmtm_ts_tod_drv_t bcm56780_a0_ts_tod_drv = {
        .tod_update = bcm56780_a0_tm_ts_tod_update,
        .tod_auto = bcm56780_a0_tm_ts_tod_auto,
        .tod_get = bcm56780_a0_tm_ts_tod_get
    };

    SHR_FUNC_ENTER(unit);

    SHR_NULL_CHECK(ts_tod_drv, SHR_E_INTERNAL);
    *((bcmtm_ts_tod_drv_t *)ts_tod_drv) = bcm56780_a0_ts_tod_drv;
exit:
    SHR_FUNC_EXIT();
}
