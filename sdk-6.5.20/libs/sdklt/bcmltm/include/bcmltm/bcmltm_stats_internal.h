/*! \file bcmltm_stats_internal.h
 *
 * Logical Table Manager Statistics Internal Definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_STATS_INTERNAL_H
#define BCMLTM_STATS_INTERNAL_H

#include <bcmdrd_config.h>

#include <bcmltm/bcmltm_types.h>

/*!
 * \brief Define format to retrieve statistics array of a LT
 *
 * This is the function prototype for fetching the statistics array
 * of a particular Logical Table, which is managed within the high
 * level LTM submodule.  Some  operations and tables require access
 * to the statistics of a table other than the access LT.
 * This callback permits access to another LT's metadata structure.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the pointer for statistics array
 *                         for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 typedef int (*bcmltm_stats_lt_get_f)(int unit,
                                      uint32_t ltid,
                                      uint32_t **lt_stats_p);

/*!
 * \brief LTM statistics management information per-unit.
 */
typedef struct bcmltm_stats_mgmt_s {
    /*!
     * Size of statistic types indexed arrays on this unit.
     */
    uint32_t stats_array_size;

    /*!
     * The registered callback from a higher level submodule to retrieve
     * the LT info for a given Logical Table.
     */
    bcmltm_stats_lt_get_f stats_lt_get_cb;
} bcmltm_stats_mgmt_t;

/*!
 * \brief Register a callback for retrieving LT statistics array.
 *
 * The TABLE_STATS node functions require access to the LT statistics array
 * of a LT other than the LTID specified in the entry structure.  To preserve
 * the layering hierarchy of the LTM submodules, a higher-level
 * function to provide a specific LTs statistics array is registered
 * for use as a callback to the lower layers.
 *
 * \param [in] unit Logical device id.
 * \param [in] stats_lt_get_cb Callback function to retrieve LT
 *                             statistics array.
 *
 * \retval None
 */
extern void
bcmltm_stats_lt_get_register(int unit,
                             bcmltm_stats_lt_get_f stats_lt_get_cb);

/*!
 * \brief Retrieve statistics array of a LT.
 *
 * Fetch the statistics array of a particular Logical Table.
 * This function provides an interface to the registered statistics
 * array retrieval callback which may be used by other LTM submodules.
 *
 * \param [in] unit Logical device id.
 * \param [in] ltid Logical Table ID.
 * \param [out] lt_stats_p Pointer to the statistics array for this LT.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM LTID outside the range of valid LTIDs.
 * \retval SHR_E_NOT_FOUND No state for this LTID.
 */
 extern int
 bcmltm_stats_lt_get(int unit,
                     uint32_t ltid,
                     uint32_t **lt_stats_p);

/*!
 * \brief Increment LT_STATS field
 *
 * Wrapper for auto-incrementing various LT_STATS values.
 *
 * \param [in] lt_stats Pointer to the statistics array for this LT.
 * \param [in] stat_field FID indicating which stat is to increment.
 */
extern void
bcmltm_stats_increment(uint32_t *lt_stats,
                       uint32_t stat_field);

#endif /* BCMLTM_STATS_INTERNAL_H */
