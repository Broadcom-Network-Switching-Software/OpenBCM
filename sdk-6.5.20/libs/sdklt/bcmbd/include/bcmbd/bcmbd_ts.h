/*! \file bcmbd_ts.h
 *
 * BD TimeSync driver API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_TS_H
#define BCMBD_TS_H

/*!
 * \brief Get enable status of TimeSync timestamp counter.
 *
 * This function gets the enable status of the timestamp counter in
 * TimeSync hardware.
 * The \c ts_inst is used to specify the zero-based TimeSync hardware instance.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] enable Enable status for the timestamper.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMBD TS driver not initialized yet.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid parameter for \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_enable_get(int unit, int ts_inst, bool *enable);

/*!
 * \brief Enable/Disable TimeSync timestamp counter.
 *
 * This function enables status of the timestamp counter in TimeSync hardware.
 * The \c ts_inst is used to specify the zero-based TimeSync hardware instance.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] enable TRUE - enable TS counter; FALSE - disable TS counter.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INIT BCMBD TS driver not initialized yet.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid parameter for \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_enable_set(int unit, int ts_inst, bool enable);

/*!
 * \brief Get instantaneous TS counter value.
 *
 * This function gets the current timestamp counter from TimeSync hardware.
 * The format of the returned timestamp \c ts_raw is normally depends on
 * TimeSync hardware and is usually combined with nanoseconds in higher bits
 * and fraction of nanoseconds in lower bits.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] ts_raw Raw timestamp value from TS hardware if not NULL.
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
extern int
bcmbd_ts_timestamp_raw_get(int unit, int ts_inst, uint64_t *ts_raw,
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
extern int
bcmbd_ts_timestamp_raw_set(int unit, int ts_inst, uint64_t ts_raw,
                           uint32_t valid_bits, uint32_t frac_bits);

/*!
 * \brief Get timestamp offset of TS counter value.
 *
 * This functions gets the current timestamp offset value in TimeSync hardware.
 *
 * Note that the format of \c ts_offset is identical to the timestamp format
 * in TimeSync hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] ts_offset Returned Timestamp offset if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_raw_offset_get(int unit, int ts_inst, uint64_t *ts_offset);

/*!
 * \brief Set timestamp offset to instantaneous TS counter value.
 *
 * This function sets the timestamp offset \c ts_offset to timestamp counter in
 * TimeSync hardware and the offset will be loaded after the specified
 * time interval \c ts_update_interval.
 *
 * Note that the format of \c ts_offset and \c ts_update_interval is
 * identical to the timestamp format in TimeSync hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] ts_offset Timestamp offset to set.
 * \param [in] ts_update_interval Timestamp interval for timestamp offset
 *                                to be applied.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_DISABLE Timestamp counter of TimeSync hardware is disabled.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_raw_offset_set(int unit, int ts_inst,
                                  uint64_t ts_offset,
                                  uint64_t ts_update_interval);

/*!
 * \brief Get instantaneous TS counter value in nanoseconds.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [out] ts_ns Timestamp in nanoseconds from TS hardware if not NULL.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_DISABLE Timestamp counter of TimeSync hardware is disabled.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_nanosec_get(int unit, int ts_inst, uint64_t *ts_ns);

/*!
 * \brief Set instantaneous TS counter value in nanoseconds.
 *
 * \param [in] unit Unit number.
 * \param [in] ts_inst Zero-based TimeSync hardware instance.
 * \param [in] ts_ns Timestamp in nanoseconds to be set to TS hardware.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Device \c unit is not existing.
 * \retval SHR_E_PARAM Invalid TimeSync hardware instance \c ts_inst.
 * \retval SHR_E_UNAVAIL Not support in current device.
 * \retval SHR_E_IO Failed to access registers.
 */
extern int
bcmbd_ts_timestamp_nanosec_set(int unit, int ts_inst, uint64_t ts_ns);

#endif /* BCMBD_TS_H */
