/*! \file bcmbd_cmicx_ts_drv_internal.h
 *
 * CMICx TimeSync driver functions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_TS_DRV_INTERNAL_H
#define BCMBD_CMICX_TS_DRV_INTERNAL_H

/*!
 * \brief Get TimeSync timestamp counter configuration (enabled/disabled).
 *
 * Get timestamp counter configuration from TimeSync hardware registers.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] enable True if timesatmp counter is enabled, otherwise false.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_enable_get_f)(int unit, int ts_inst, bool *enable);

/*!
 * \brief Configuration (enable/disable) the TimeSync timestamp counter.
 *
 * This function will enable or disable the timestamp counter from
 * TimeSync hardware registers.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] enable Enable timestamp counter if non-zero, otherwise disable.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_enable_set_f)(int unit, int ts_inst, bool enable);

/*!
 * \brief Capture immediate timestamp from TimeSync hardware registers.
 *
 * The timestamp value from CMICx TimeSync hardware is 52-bit composed of
 * 48-bit (51:4) nanoseconds and 4-bit (3:0) fraction of nanoseconds.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] timestamp Captured timestamp value.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_get_f)(int unit, int ts_inst, uint64_t *timestamp);

/*!
 * \brief Set immediate timestamp to TimeSync hardware registers.
 *
 * Set 52-bit timestamp value to TimeSync hardware registers.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] timestamp Timestamp value to set.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_set_f)(int unit, int ts_inst, uint64_t timestamp);

/*!
 * \brief Get timestamp offset from TimeSync hardware registers.
 *
 * The returned time offset \c ts_offset is in 52-bit timestamp format.
 *
 * \param [in] unit unit number.
 * \param [in] ts_inst zero-based timesync hardware instance.
 * \param [out] ts_offset Timestamp offset value.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_offset_get_f)(int unit, int ts_inst,
                                         uint64_t *ts_offset);

/*!
 * \brief Set timestamp offset to TimeSync hardware registers.
 *
 * Update timestamp offset \c ts_offset at time \c update_at_ns.
 * The offset is in 52-bit timestamp format and update time is in nanoseconds
 * based on hardware capability.
 *
 * \param [in] unit unit number.
 * \param [in] ts_inst zero-based TimeSync hardware instance.
 * \param [in] ts_offset Timestamp offset to set.
 * \param [in] update_at_ns Offset will be updated at the specified time.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
typedef int
(*bcmbd_cmicx_ts_timestamp_offset_set_f)(int unit, int ts_inst,
                                         uint64_t ts_offset,
                                         uint64_t update_at_ns);

/*! TimeSync hardware registers access functions. */
typedef struct bcmbd_cmicx_ts_drv_acc_s {

    /*!
     * Device-specific registers access to get timestamp counter enable status.
     */
    bcmbd_cmicx_ts_timestamp_enable_get_f timestamp_enable_get;

    /*!
     * Device-specific registers access to enable/disable timestamp counter.
     */
    bcmbd_cmicx_ts_timestamp_enable_set_f timestamp_enable_set;

    /*!
     * Device-specific registers access to get instantaneous timestamp value.
     */
    bcmbd_cmicx_ts_timestamp_get_f timestamp_get;

    /*!
     * Device-specific registers access to set instantaneous timestamp value.
     */
    bcmbd_cmicx_ts_timestamp_set_f timestamp_set;

    /*! Device-specific registers access to get timestamp offset. */
    bcmbd_cmicx_ts_timestamp_offset_get_f timestamp_offset_get;

    /*! Device-specific registers access to set timestamp offset. */
    bcmbd_cmicx_ts_timestamp_offset_set_f timestamp_offset_set;

} bcmbd_cmicx_ts_drv_acc_t;

/*!
 * \brief Install CMICx TimeSync driver.
 *
 * Install CMICx TimeSync driver into top-level TimeSync API.
 *
 * \param [in] unit Unit number.
 * \param [in] drv_acc CMICx TimeSync hardware access functions.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 */
extern int
bcmbd_cmicx_ts_drv_init(int unit, const bcmbd_cmicx_ts_drv_acc_t *drv_acc);

/*!
 * \brief Cleanup the installed CMICx TimeSync driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 */
extern int
bcmbd_cmicx_ts_drv_cleanup(int unit);

#endif /* BCMBD_CMICX_TS_DRV_INTERNAL_H */
