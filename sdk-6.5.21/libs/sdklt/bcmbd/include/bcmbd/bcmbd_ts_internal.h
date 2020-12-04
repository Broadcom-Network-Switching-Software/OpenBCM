/*! \file bcmbd_ts_internal.h
 *
 * BD TimeSync driver internal API and structures.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TS_INTERNAL_H
#define BCMBD_TS_INTERNAL_H

#include <bcmbd/bcmbd_ts.h>

/*!
 * \brief Device-specific TimeSync driver initialization.
 *
 * This function is used to initialize the chip-specific TimeSync
 * configuration, which may include both software and hardware
 * operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL TimeSync driver initialization failed.
 */
typedef int
(*bcmbd_ts_init_f)(int unit);

/*!
 * \brief Device-specific TimeSync driver cleanup.
 *
 * This function is used to clean up the TimeSync driver resources
 * allocated by \ref bcmbd_ts_init_f.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL TimeSync driver cleanup failed.
 */
typedef int
(*bcmbd_ts_cleanup_f)(int unit);

/*!
 * \brief Get enable status of TimeSync timestamp counter.
 *
 * This function enables the timestamp counter in TimeSync hardware.
 * The \c ts_inst is used to specify the zero-based TimeSync hardware instance.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] enable enable status of the timestamper.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_enable_get_f)(int unit, int ts_inst, bool *enable);

/*!
 * \brief Get enable status of TimeSync timestamp counter.
 *
 * This function gets the enable status of the timestamp counter in TimeSync hardware.
 * The \c ts_inst is used to specify the zero-based TimeSync hardware instance.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] enable Enable status for the timestamper.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_enable_set_f)(int unit, int ts_inst, bool enable);

/*!
 * \brief Get instantaneous TS counter value.
 *
 * This function gets the current timestamp counter from TimeSync hardware.
 * The returned timestamp format is normally depends on TimeSync hardware
 * and is usually combined with nanoseconds in higher bits and
 * fraction of nanoseconds in lower bits.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] timestamp Raw timestamp value from TS hardware if not NULL.
 * \param [out] valid_bits Number of valid bits of timestamp if not NULL.
 * \param [out] frac_bits Number of fraction bits of timestamp if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_DISABLE Timestamp counter of TimeSync hardware is disabled.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_get_f)(int unit, int ts_inst, uint64_t *timestamp,
                            uint32_t *valid_bits, uint32_t *frac_bits);

/*!
 * \brief Set instantaneous TS counter value.
 *
 * This function sets the current timestamp counter to TimeSync hardware.
 * The timestamp format is normally depends on TimeSync hardware and is
 * usually combined with nanoseconds in higher bits and fraction of nanoseconds
 * in lower bits.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] ts_raw Raw timestamp value to be set to TS hardware.
 * \param [in] valid_bits Number of valid bits of raw timestamp \c ts_raw.
 * \param [in] frac_bits Number of fraction bits of raw timestamp \c ts_raw.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_set_f)(int unit, int ts_inst, uint64_t timestamp,
                            uint32_t valid_bits, uint32_t frac_bits);

/*!
 * \brief Get timestamp offset of TS counter value.
 *
 * This function gets the current timestamp offset value from TimeSync hardware.
 *
 * Note that the format of \c offset is identical to the timestamp format
 * in TimeSync hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] offset Returned Timestamp offset if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_offset_get_f)(int unit, int ts_inst, uint64_t *offset);

/*!
 * \brief Set timestamp offset to instantaneous TS counter value.
 *
 * This function sets the timestamp offset \c offset to timestamp counter in
 * TimeSync hardware and the offset will be loaded after the specified
 * time interval \c update_interval.
 *
 * Note that the format of \c offset and \c update_interval is identical to
 * the timestamp format in TimeSync hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] offset Timestamp offset to set.
 * \param [in] update_interval Timestamp interval for timestamp offset
 *                             to be applied.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_DISABLE Timestamp counter of TimeSync hardware is disabled.
 * \retval SHR_E_IO Failed to access registers.
 */
typedef int
(*bcmbd_ts_timestamp_offset_set_f)(int unit, int ts_inst,
                                   uint64_t offset, uint64_t update_interval);

/*! TimeSync driver object. */
typedef struct bcmbd_ts_drv_s {

    /*! Initialize TimeSync driver. */
    bcmbd_ts_init_f init;

    /*! Clean up TimeSync driver. */
    bcmbd_ts_cleanup_f cleanup;

    /*! Get enable status of the timestamp counter. */
    bcmbd_ts_timestamp_enable_get_f timestamp_enable_get;

    /*! Enable/Disable timestamp counter. */
    bcmbd_ts_timestamp_enable_set_f timestamp_enable_set;

    /*! Get instantaneous timestamp counter value. */
    bcmbd_ts_timestamp_get_f timestamp_get;

    /*! Set instantaneous timestamp counter value. */
    bcmbd_ts_timestamp_set_f timestamp_set;

    /*! Get offset of timestamp counter value. */
    bcmbd_ts_timestamp_offset_get_f timestamp_offset_get;

    /*! Set offset of timestamp counter value. */
    bcmbd_ts_timestamp_offset_set_f timestamp_offset_set;

} bcmbd_ts_drv_t;

/*!
 * \brief Install device-specific TimeSync driver.
 *
 * Install device-specific TimeSync driver into top-level TimeSync API.
 *
 * Use \c ts_drv = NULL to uninstall a driver.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_drv TimeSync driver object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 */
extern int
bcmbd_ts_drv_init(int unit, const bcmbd_ts_drv_t *ts_drv);

/*!
 * \brief Device-specific TimeSync driver initialization.
 *
 * This function is used to initialize the chip-specific TimeSync
 * configuration, which may include both software and hardware
 * operations.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL TimeSync driver initialization failed.
 */
extern int
bcmbd_ts_init(int unit);

/*!
 * \brief Device-specific TimeSync driver cleanup.
 *
 * This function is used to clean up the TimeSync driver resources
 * allocated by \ref bcmbd_ts_init_f.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL TimeSync driver cleanup failed.
 */
extern int
bcmbd_ts_cleanup(int unit);

#endif /* BCMBD_TS_INTERNAL_H */
