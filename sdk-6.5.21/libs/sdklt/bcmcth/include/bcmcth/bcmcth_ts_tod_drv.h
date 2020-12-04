/*! \file bcmcth_ts_tod_drv.h
 *
 * BCMCTH TS TOD driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_TS_TOD_DRV_H
#define BCMCTH_TS_TOD_DRV_H

#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmltd/chip/bcmltd_common_enum_ctype.h>
#include <shr/shr_bitop.h>

/*! ToD lt entry operational value */
typedef struct ts_tod_entry_oper_s {
    /*! Oper value of ToD second */
    uint64_t sec;
    /*! Oper value of ToD nanosecond */
    uint32_t nsec;
    /*! Oper value of ToD adjustment */
    uint64_t adjust;
    /*! HW_UPDATE status */
    bool update_fail;
} ts_tod_entry_oper_t;

/*! ToD lt entry */
typedef struct ts_tod_entry_s {
    /*! The logical table ID */
    bcmltd_sid_t ltid;
    /*! The operation arguments */
    const bcmltd_op_arg_t *op_arg;
    /*! Bitmap of Fields to be operated. */
    SHR_BITDCLNAME(
        fbmp,
        DEVICE_TS_TODt_FIELD_COUNT);
    /*! Enable hw update. */
    bool hw_update;
    /*! ToD second */
    uint64_t sec;
    /*! ToD nanosecond */
    uint32_t nsec;
    /*! ToD adjustment */
    uint64_t adjust;
} ts_tod_entry_t;

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
typedef int (*ts_tod_update) (
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_t *entry);

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
typedef int (*ts_tod_auto) (
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_t *entry);

/*!
 * \brief Get ToD value
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
typedef int (*ts_tod_get) (
    int unit,
    bcmltd_sid_t ltid,
    ts_tod_entry_oper_t *entry);

/*!
 * \brief TS ToD driver object
 *
 * TS ToD driver is used to provide the chip specific information.
 */
typedef struct bcmcth_ts_tod_drv_s {
    /*! Update ToD config */
    ts_tod_update tod_update;
    /*! Enable ToD hw_update */
    ts_tod_auto tod_auto;
    /*! Get ToD value */
    ts_tod_get tod_get;
} bcmcth_ts_tod_drv_t;

/*!
 * \brief Get ToD driver APIs.
 *
 * This function returns a structure which contains the device specific APIs
 * for SyncE.
 *
 * \param [in] unit Unit number.
 *
 * \return ToD driver API structure
 */
extern bcmcth_ts_tod_drv_t *
bcmcth_ts_tod_drv_get(int unit);

/*!
 * \brief Register IMM callbacks for ToD.
 *
 * The TS ToD custom table handling is done via the IMM component.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ts_tod_imm_register(int unit);

/*!
 * \brief ToD LT operation function
 *
 * This function handles the operations for DEVICE_TS_TOD
 * to update HW according to the content of entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Configuration values of the entry.
 */
extern int
bcmcth_ts_tod_update(int unit,
                     ts_tod_entry_t *entry);

/*!
 * \brief ToD LT lookup handler.
 *
 * The method lookups the configuration for getting ToD operational values.
 *
 * \param [in]  unit          Unit Number.
 * \param [in]  ltid          Logical table ID
 * \param [in]  op_arg        Operation arguments.
 * \param [out] oper_entry    Operational status of the entry.
 *
 * \retval SHR_E_NONE         No errors.
 */
int
bcmcth_ts_tod_lookup(int unit,
                     bcmltd_sid_t ltid,
                     const bcmltd_op_arg_t *op_arg,
                     ts_tod_entry_oper_t *oper_entry);
#endif /* BCMCTH_TS_TOD_DRV_H */

