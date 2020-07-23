/* \file bst.h
 *
 * BST headfiles to declare internal APIs for TM devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_TM_BST_H
#define BCMINT_LTSW_TM_BST_H

#include <bcm/types.h>
#include <bcm/cosq.h>

#include <sal/sal_types.h>
/*
 * \brief BST index resolve function.
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
typedef int (*cosq_bst_index_resolve_f)(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_port_t *local_port,
    int *start_index,
    int *end_index);

/*
 * \brief Retrieve gport/cosq from BST SW index for a resource.
 *        Used for decoding BST trigger information for notifying users.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          BST stat id.
 * \param [in]   port         Unused (retained to maintain consistency).
 * \param [in]   index        SW index for which stat > 0.
 * \param [in]   itm          buffer id.
 * \param [in]   idx          index of sw.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
typedef int (*cosq_bst_resolve_index_f)(
    int unit,
    bcm_bst_stat_id_t bid,
    int port,
    int index,
    int *idx);

/*
 * \brief Retrieve max index for a resource.
 *
 * \param [in]   unit         Unit number.
 * \param [in]   bid          BST stat id.
 * \param [in]   max_idx      index of sw.
 *
 * \retval SHR_E_NONE         No errors.
 * \retval !SHR_E_NONE        Failure.
 */
typedef int (*cosq_bst_resolve_max_index_f)(
    int unit,
    bcm_bst_stat_id_t bid,
    int *max_idx);

/*
 * \brief Cosq_bst driver structure.
 */
typedef struct tm_ltsw_bst_chip_driver_s {

    /* BST index resolve function. */
    cosq_bst_index_resolve_f cosq_bst_index_resolve;

    /* Retrieve gport/cosq from BST SW index for a resource. */
    cosq_bst_resolve_index_f cosq_bst_resolve_index;

    /* Retrieve max index for a resource. */
    cosq_bst_resolve_max_index_f cosq_bst_resolve_max_index;

} tm_ltsw_bst_chip_driver_t;

/*
 * \brief BST Driver callback registration
 *
 * \param [in] unit Unit number.
 * \param [in] drv  Driver pointer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_bst_chip_driver_register(int unit, tm_ltsw_bst_chip_driver_t* drv);

/*
 * \brief BST Driver callback deregistration
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_bst_chip_driver_deregister(int unit);

/*
 * \brief BST Driver get.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
tm_ltsw_bst_chip_driver_get(int unit, tm_ltsw_bst_chip_driver_t **drv);

/*
 * \brief Get the BST profile for CosQ objects
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [out] profile BST profile configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile);

/*
 * \brief Set the BST profile for CosQ objects
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [in] profile BST profile configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_cosq_bst_profile_t *profile);

/*
 * \brief Get the BST profile for CosQ objects
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer BST stat ID to identify the COSQ resource/object.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 * \param [in] options BST stat read options.
 * \param [out] pvalue BST stat count.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_stat_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid,
    uint32_t options,
    uint64_t *pvalue);

/*
 * \brief Clear speficied BST stat
 *
 * \param [in] unit Unit Number.
 * \param [in] gport Gport Number.
 * \param [in] cosq COS queue.
 * \param [in] buffer BST stat ID to identify the COSQ resource/object.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_stat_clear(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_buffer_id_t buffer,
    bcm_bst_stat_id_t bid);

/*
 * \brief Sync the HW stats value to SW copy for all or given BST resource
 *
 * \param [in] unit Unit Number.
 * \param [in] bid BST stat ID to identify the COSQ resource/object.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_stat_sync(
    int unit,
    bcm_bst_stat_id_t bid);

/*
 * \brief Initialize the bst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_init(int unit);

/*
 * \brief Detach the bst module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_detach(int unit);

/*
 * \brief Enable BST tracking.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_enable_set(
    int unit,
    int arg);

/*
 * \brief Get BST enable value.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST enable value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_enable_get(
    int unit,
    int *arg);

/*
 * \brief Set BST resource usage tracking mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST resource usage tracking mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_tracking_mode_set(
    int unit,
    int arg);

/*
 * \brief Get BST resource usage tracking mode.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST resource usage tracking mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_tracking_mode_get(
    int unit,
    int *arg);

/*
 * \brief Set BST resource usage SnapShot mode.
 *
 * \param [in] unit Unit Number.
 * \param [in] arg BST resource usage SnapShot mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_snapshot_enable_set(
    int unit,
    int arg);

/*
 * \brief Get BST resource usage SnapShot mode.
 *
 * \param [in] unit Unit Number.
 * \param [out] arg BST resource usage SnapShot mode value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
tm_ltsw_cosq_bst_snapshot_enable_get(
    int unit,
    int *arg);

#endif /* BCMINT_LTSW_TM_BST_H */
