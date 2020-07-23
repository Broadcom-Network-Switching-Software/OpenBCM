/*! \file bcmltm_lta_intf_internal.h
 *
 * Interfaces to call initialize and cleanup routines for custom table
 * handlers that have requested such service.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMLTM_LTA_INTF_INTERNAL_H
#define BCMLTM_LTA_INTF_INTERNAL_H

#include <bcmltd/bcmltd_handler.h>
#include <bcmlrd/bcmlrd_types.h>

/*!
 * \brief Device configuration structure.
 *
 * Per-device configuration set.
 */
typedef struct bcmltm_lta_device_conf_s {
    /*! LRD device configuration. */
    bcmlrd_device_conf_t    *lrd_device_config;

    /*! LTD LTM_INTF configuration. */
    const bcmltd_ltm_intf_t **ltm_lta_intf_config;
} bcmltm_lta_device_conf_t;

/*! LTM-LTA device configuration. */
extern bcmltm_lta_device_conf_t const *bcmltm_lta_device_conf;

/*! LTM-LTA default device configuration. */
extern const bcmltm_lta_device_conf_t bcmltm_default_lta_device_conf;

/*!
 * \brief Set LTM LTA interface configuration.
 *
 * Set the desired device configuration to the given device
 * configuration, returning the current device configuration.
 *
 * This is a test interface only - not for production.
 *
 * \param [in] conf             Desired configuration.
 *
 * \retval Previous device configuration
 * \retval NULL ERROR
 */
extern const bcmltm_lta_device_conf_t*
bcmltm_lta_device_config_set(const bcmltm_lta_device_conf_t *conf);

/*!
 * \brief Get transform handler.
 *
 * Get the transform handler for this unit by handler_id.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  handler_id      Handler ID.
 *
 * \retval Handler.
 * \retval NULL ERROR.
 */
extern const bcmltd_xfrm_handler_t *
bcmltm_lta_intf_xfrm_handler_get(int unit, uint32_t handler_id);

/*!
 * \brief Get validation handler.
 *
 * Get the validation handler for this unit by handler_id.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  handler_id      Handler ID.
 *
 * \retval Handler.
 * \retval NULL ERROR.
 */
extern const bcmltd_field_val_handler_t *
bcmltm_lta_intf_val_handler_get(int unit, uint32_t handler_id);

/*!
 * \brief Get table commit handler.
 *
 * Get the table commit handler for this unit by handler_id.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  handler_id      Handler ID.
 *
 * \retval Handler.
 * \retval NULL ERROR.
 */
extern const bcmltd_table_commit_handler_t *
bcmltm_lta_intf_table_commit_handler_get(int unit, uint32_t handler_id);

/*!
 * \brief Get table entry limit handler.
 *
 * Get the table entry limit handler for this unit by table ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 *
 * \retval Handler.
 * \retval NULL, if not available.
 */
extern const bcmltd_table_entry_limit_handler_t *
bcmltm_lta_intf_table_entry_limit_handler_get(int unit, bcmltd_sid_t sid);

/*!
 * \brief Get table entry usage handler.
 *
 * Get the table entry usage handler for this unit by table ID.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  sid             Table ID.
 *
 * \retval Handler.
 * \retval NULL, if not available.
 */
extern const bcmltd_table_entry_usage_handler_t *
bcmltm_lta_intf_table_entry_usage_handler_get(int unit, bcmltd_sid_t sid);

/*!
 * \brief Get custom table handler handler.
 *
 * Get the custom table handler for this unit by handler_id.
 *
 * \param [in]  unit            Unit number.
 * \param [in]  handler_id      Handler ID.
 *
 * \retval Handler.
 * \retval NULL ERROR.
 */
extern const bcmltd_table_handler_t *
bcmltm_lta_intf_table_handler_get(int unit, uint32_t handler_id);

/*!
 * \brief Get table lifecycle interface.
 *
 * Get the custom table lifecycle interface for this unit.
 *
 * \param [in]  unit            Unit number.
 *
 * \retval Handler
 * \retval NULL ERROR
 */
extern const bcmltd_table_lifecycle_intf_t *
bcmltm_lta_intf_table_lifecycle_get(int unit);

/*!
 * \brief Set LTM LTA interface configuration.
 *
 * Set the LTM LTA interface configuration to the given interface
 * configuration, returning the current interface configuration.
 *
 * This is a test interface only - not for production.
 *
 * \param [in] lta_conf         Desired interface configuration.
 *
 * \retval Previous interface configuration
 * \retval NULL ERROR
 */
extern const bcmltd_ltm_intf_t **
bcmltm_lta_intf_set(const bcmltd_ltm_intf_t **lta_conf);


/*!
 * \brief Check if table is an itile index LT.
 *
 * This routine checks if given table ID is an itile index LT.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Logical table ID.
 *
 * \retval TRUE Table is an itile index LT.
 * \retval FALSE Table is a not an itile index LT or failure.
 */
extern bool
table_is_itile_index(int unit, bcmlrd_sid_t sid);

#endif /* BCMLTM_LTA_INTF_INTERNAL_H */
