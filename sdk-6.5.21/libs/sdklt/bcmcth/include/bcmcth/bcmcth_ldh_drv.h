/*! \file bcmcth_ldh_drv.h
 *
 * BCMCTH Latency Distribution Histogram Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the base driver (BD) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_LDH_DRV_H
#define BCMCTH_LDH_DRV_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>

#include <bcmbd/bcmbd.h>
#include <bcmdrd/bcmdrd_pt.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmptm/bcmptm.h>
#include <bcmcth/bcmcth_ldh_device_info.h>

/*! Timesync instance. */
#define BCMLDH_TS_INST    1

/*!
 * \brief Define pt and lt mapping information.
 */
typedef struct bcmcth_ldh_pt_lt_mapping_s {

    /*! Pt symbol ID. */
    bcmdrd_sid_t pt_sid;

    /*! Lt symbol ID. */
    bcmltd_sid_t lt_sid;

} bcmcth_ldh_pt_lt_mapping_t;

/*!
 * \brief Get device-specific latency distribution histogram
 * device information.
 *
 * \param [in] unit Unit number.
 * \param [out] info Device-specific LDH information.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_device_info_get_f)(int unit,
                                            bcmcth_ldh_device_info_t *info);

/*!
 * \brief Get counter pool information.
 *
 * \param [in] unit Unit number.
 * \param [out] counter pool memory list.
 * \param [out] counter pool number.
 */
typedef int (*bcmcth_ldh_ctr_pool_info_get_f)(int unit,
                                              bcmcth_ldh_pt_lt_mapping_t **mem_list,
                                              uint32_t *num);

/*!
 * \brief Get maximum number of monitor.
 *
 * \param [in] unit Unit number.
 * \param [out] num Maximum number of monitor.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_monitor_max_get_f)(int unit, int *num);

/*!
 * \brief Get maximum number of counter pool.
 *
 * \param [in] unit Unit number.
 * \param [out] num Maximum number of counter pool.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_ctr_pool_max_get_f)(int unit, int *num);

/*!
 * \brief Get maximum entries of a counter pool.
 *
 * \param [in] unit Unit number.
 * \param [out] num Maximum entries of a counter pool.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_ctr_entry_max_get_f)(int unit, int *num);

/*!
 * \brief Get data of a counter entry.
 *
 * \param [in] unit Unit number.
 * \param [in] pool_id Counter pool ID.
 * \param [in] index Index of counter entry.
 * \param [out] packets Number of packets.
 * \param [out] bytes Number of bytes.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_ctr_data_get_f)(int unit,
                                         int pool_id,
                                         int index,
                                         uint32_t *packets,
                                         uint32_t *bytes);

/*!
 * \brief Get current working on histogram group of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [out] group Current working on histogram group.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_cur_histo_group_get_f)(int unit, int mon_id, int *group);

/*!
 * \brief Get last counter index of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [out] index Last counter index.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_last_ctr_idx_get_f)(int unit, int mon_id, int *indx);

/*!
 * \brief Get number of times of wrap around.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [out] num Number of times of wrap around.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_ctr_wrap_around_get_f)(int unit, int mon_id, int *num);

/*!
 * \brief Set current working on histogram group of monitor.
 *
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] group Current working on histogram group.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_cur_histo_group_set_f)(int unit, int mon_id, int group);

/*!
 * \brief Set last counter index of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] index Last counter index.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_last_ctr_idx_set_f)(int unit, int mon_id, int indx);

/*!
 * \brief Set number of times of counter wrapped around.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] num Number of times wrapped around.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_ctr_wrap_around_set_f)(int unit, int mon_id, int num);

/*!
 * \brief Enable or Disable monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] en Enable or Disable.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_enable_set_f)(int unit, int mon_id, bool en);

/*!
 * \brief Set count mode of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] mode Count mode of monitor.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_count_mode_set_f)(int unit, int mon_id, int mode);

/*!
 * \brief Set time interval of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] step Time interval of monitor.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_time_step_set_f)(int unit, int mon_id, int step);

/*!
 * \brief Set ctr pool id of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] pool_id Counter pool id of monitor.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_pool_id_set_f)(int unit, int mon_id, int pool_id);

/*!
 * \brief Set ctr pool mode of monitor.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] mode Counter pool mode of monitor.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_pool_mode_set_f)(int unit, int mon_id, int mode);

/*!
 * \brief Set offset of histogram group.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] offset Offset of histogram group.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_base_offset_set_f)(int unit, int mon_id, int offset);

/*!
 * \brief Set counter increment of histogram group.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] step Counter increment of histogram group.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_counter_incr_set_f)(int unit, int mon_id, int step);

/*!
 * \brief Set maximum number of histogram group.
 * \param [in] unit Unit number.
 * \param [in] mon_id Monitor ID.
 * \param [in] group_max Maximum number of histogram group.
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_ldh_mon_group_max_set_f)(int unit, int mon_id, int group_max);

/*!
 * \brief LDH driver object
 *
 * LDH driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH LDH module by
 * \ref bcmcth_ldh_drv_init() from the top layer. BCMCTH LDH internally
 * will use this interface to geti/set the corresponding information.
 */
typedef struct bcmcth_ldh_drv_s {

    /*! Get latency distribution histogram  device information. */
    bcmcth_ldh_device_info_get_f          device_info_get;

    /*! Get counter pool information. */
    bcmcth_ldh_ctr_pool_info_get_f        ctr_pool_info_get;

    /*! Get maximum number of monitors. */
    bcmcth_ldh_monitor_max_get_f          mon_max_get;

    /*! Get maximum number of counter pools. */
    bcmcth_ldh_ctr_pool_max_get_f         ctr_pool_max_get;

    /*! Get maximum entries of a counter pool. */
    bcmcth_ldh_ctr_entry_max_get_f        ctr_entry_max_get;

    /*! Get data of a counter entry. */
    bcmcth_ldh_ctr_data_get_f             ctr_data_get;

    /*! Get current working on histogram group of monitor. */
    bcmcth_ldh_cur_histo_group_get_f      cur_histo_group_get;

    /*! Get last counter index of monitor. */
    bcmcth_ldh_last_ctr_idx_get_f         last_ctr_idx_get;

    /*! Get number of times of wrap around. */
    bcmcth_ldh_ctr_wrap_around_get_f      ctr_wrap_around_get;

    /*! Set current working on histogram group of monitor. */
    bcmcth_ldh_cur_histo_group_set_f      cur_histo_group_set;

    /*! Set last counter index of monitor. */
    bcmcth_ldh_last_ctr_idx_set_f         last_ctr_idx_set;

    /*! Set number of times of counter wrapped around. */
    bcmcth_ldh_ctr_wrap_around_set_f      ctr_wrap_around_set;

    /*! Enable or disable monitor. */
    bcmcth_ldh_mon_enable_set_f           mon_enable_set;

    /*! Set count mode of monitor. */
    bcmcth_ldh_mon_count_mode_set_f       mon_count_mode_set;

    /*! Set time interval of monitor. */
    bcmcth_ldh_mon_time_step_set_f        mon_time_step_set;

    /*! Set counter pool id of monitor. */
    bcmcth_ldh_mon_pool_id_set_f          mon_pool_id_set;

    /*! Set counter pool mode of monitor. */
    bcmcth_ldh_mon_pool_mode_set_f        mon_pool_mode_set;

    /*! Set offset of monitor. */
    bcmcth_ldh_mon_base_offset_set_f      mon_base_offset_set;

    /*! Set counter increment of monitor. */
    bcmcth_ldh_mon_counter_incr_set_f     mon_ctr_incr_set;

    /*! Set maximum number of histogram group. */
    bcmcth_ldh_mon_group_max_set_f        mon_group_max_set;

} bcmcth_ldh_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_ldh_drv_t *_bc##_cth_ldh_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific LDH driver.
 * \param [in] unit Unit number.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_drv_init(int unit);

/*!
 * \brief Initialize device-specific LDH driver.
 * \param [in] unit Unit number.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_init(int unit);

/*!
 * \brief Clean up device-specific LDH driver.
 * \param [in] unit Unit number.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_cleanup(int unit);

/*!
 * \brief Get counter pool information.
 *
 * \param [in] unit Unit number.
 * \param [out] mems counter pool memory list.
 * \param [out] num counter pool number.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_ctr_pool_info_get(int unit,
                             bcmcth_ldh_pt_lt_mapping_t *mems,
                             uint32_t *num);

/*!
 * \brief Get the counter data of a entry of counter pool.
 *
 * \param [in] unit Unit number.
 * \param [in] pool_id The pool ID.
 * \param [in] index The index of counter pool.
 * \param [out] packets The number of packets.
 * \param [out] bytes The number of bytes.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_ctr_data_get(int unit,
                        int pool_id,
                        int index,
                        uint32_t *packets,
                        uint32_t *bytes);

/*!
 * \brief Initialize IMM callbacks.
 *
 * \param [in] unit Unit number.
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_ldh_imm_init(int unit);

/*!
 * \brief Cleanup of IMM.
 *
 * \param [in] unit Unit number.
 */
extern void
bcmcth_ldh_imm_cleanup(int unit);

#endif /* BCMCTH_LDH_DRV_H */
