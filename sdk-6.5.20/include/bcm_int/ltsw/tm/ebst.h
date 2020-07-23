/* \file ebst.h
 *
 * EBST headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_EBST_H
#define BCMINT_LTSW_TM_EBST_H

#include <bcm/cosq.h>

/*
 * \brief EBST index resolve function.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 * \param [out]  local_port   local_port.
 * \param [out]  start_index  start index.
 * \param [out]  end_index    end index.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
typedef int (*cosq_ebst_index_resolve_f)(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_port_t *local_port,
    int *start_index,
    int *end_index);

/*
 * \brief Validate input parameters for input BST.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   gport        GPORT ID.
 * \param [in]   cosq         COS queue.
 * \param [in]   bid          BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
typedef int (*cosq_ebst_bid_gport_cosq_param_check_f)(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid);

/*
 * \brief Cosq_ebst driver structure.
 */
typedef struct tm_ltsw_ebst_chip_driver_s {

    /* EBST index resolve function. */
    cosq_ebst_index_resolve_f cosq_ebst_index_resolve;

    /* Validate input parameters for input BST. */
    cosq_ebst_bid_gport_cosq_param_check_f cosq_ebst_bid_gport_cosq_param_check;

} tm_ltsw_ebst_chip_driver_t;

/*
 * \brief EBST Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_ebst_chip_driver_register(int unit, tm_ltsw_ebst_chip_driver_t* drv);

/*
 * \brief EBST Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_ebst_chip_driver_deregister(int unit);

/*
 * \brief EBST Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_ebst_chip_driver_get(int unit, tm_ltsw_ebst_chip_driver_t **drv);

/*
 * \brief Get a EBST threshold profile for a given BST stat Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [out] profile EBST profile configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_threshold_profile_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile);

/*
 * \brief Set a EBST threshold profile for a given BST stat Id.
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [in] profile EBST profile configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_threshold_profile_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_threshold_profile_t *profile);

/*
 * \brief To get EBST scan round scan mode and enable mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Ebst control type.
 * \param [out] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_control_get(
    int unit,
    bcm_cosq_ebst_control_t type,
    int *arg);

/*
 * \brief To set EBST scan round scan mode and enable mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] type Ebst control type.
 * \param [in] arg arg.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_control_set(
    int unit,
    bcm_cosq_ebst_control_t type,
    int arg);

/*
 * \brief To get monitor info for a given BST Stat ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [out] monitor EBST monitor info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_monitor_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor);

/*
 * \brief To set monitor info for a given BST Stat ID.
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [in] monitor EBST monitor info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_monitor_set(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    bcm_cosq_ebst_monitor_t *monitor);

/*
 * \brief To get max number entries and available number entries of  EBST data buffer.
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [out] info EBST data buffer info structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_data_stat_info_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_cosq_ebst_data_stat_info_t *info);

/*
 * \brief To get an identical EBST stat for a given BST Stat ID
 *
 * \param [in] unit Unit Number.
 * \param [in] object_id Integrated cosq index.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [in] array_size array size.
 * \param [out] entry_array entry array.
 * \param [out] count Count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_cosq_ebst_data_entry_t *entry_array,
    int *count);

/*
 * \brief To enable EBST tracking.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable EBST tracking.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_enable_set(
    int unit,
    int enable);

/*
 * \brief To get EBST feature enable status.
 *
 * \param [in] unit Unit Number.
 * \param [out] enable Enable EBST tracking status.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_enable_get(
    int unit,
    int *enable);

/*
 * \brief Initialize the ebst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_init(int unit);

/*
 * \brief Detach the ebst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_ebst_detach(int unit);

#endif /* BCMINT_LTSW_TM_EBST_H */
