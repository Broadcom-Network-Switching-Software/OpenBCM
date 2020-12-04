/*! \file bcmtm_ts_tod.h
 *
 * File containing TS ToD PT parameters for
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMTM_TS_TOD_H
#define BCMTM_TS_TOD_H

#include <shr/shr_types.h>
#include <bcmltd/bcmltd_types.h>

/*! BCMTM TS ToD entry. */
typedef struct bcmtm_ts_tod_entry_s {
    /*! Enable hardware update. */
    bool hw_update;
    /*! ToD second. */
    uint64_t sec;
    /*! ToD nanosecond. */
    uint32_t nsec;
    /*! ToD adjustment. */
    uint64_t adjust;
} bcmtm_ts_tod_entry_t;


/*!
 * \brief Update ToD config
 *
 * This device specific function is called by the IMM callback handler
 * to update hardware configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] entry ToD configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*bcmtm_ts_tod_update) (int unit,
                                    bcmltd_sid_t ltid,
                                    bcmtm_ts_tod_entry_t *entry);

/*!
 * \brief Update ToD config
 *
 * This device specific function is called by the IMM callback handler
 * to update hardware configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] entry ToD configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*bcmtm_ts_tod_auto) (int unit,
                                  bcmltd_sid_t ltid,
                                  bcmtm_ts_tod_entry_t *entry);

/*!
 * \brief Update ToD get.
 *
 * This device specific function is called by the IMM callback handler
 * to update hardware configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lt_id Logical table ID.
 * \param [in] entry ToD configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Invalid parameter.
 */
typedef int (*bcmtm_ts_tod_get) (int unit,
                                 bcmltd_sid_t ltid,
                                 bcmtm_ts_tod_entry_t *entry);

/*! BCMTM TS ToD driver. */
typedef struct bcmtm_ts_tod_drv_s {
    /*! Update ToD config */
    bcmtm_ts_tod_update tod_update;

    /*! Enable ToD Hardware update. */
    bcmtm_ts_tod_auto tod_auto;

    /*! Enable ToD Hardware update. */
    bcmtm_ts_tod_get tod_get;
} bcmtm_ts_tod_drv_t;


/*!
 * \brief BCMTM TS ToD imm register.
 *
 * \param [in] unit Logical unit number.
 *
 * \return SHR_E_NONE No errors.
 * \return !SHR_E_NONE Error code.
 */
extern int
bcmtm_ts_tod_imm_register(int unit);

#endif /* BCMTM_TS_TOD_H */
