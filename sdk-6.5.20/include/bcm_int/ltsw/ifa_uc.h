/*! \file ifa_uc.h
 *
 * IFA module internal header file.
 * This file contains definitions internal to IFA module to interface with the
 * IFA embedded app.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_IFA_UC_H
#define BCMINT_LTSW_IFA_UC_H

#include <bcm/types.h>

#define BSL_LOG_MODULE BSL_LS_BCM_IFA

/*!
 * \brief Set the IFA EApp config info.
 *
 * Set the IFA EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [in] cfg_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_config_info_set(int unit, bcm_ifa_config_info_t *cfg_info);

/*!
 * \brief Get the IFA EApp config info.
 *
 * Get the IFA EApp config info.
 *
 * \param [in] unit Unit number.
 * \param [out] cfg_info Configuration info.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_config_info_get(int unit, bcm_ifa_config_info_t *cfg_info);

/*!
 * \brief Set the IFA EApp stats.
 *
 * Set the IFA EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [in] stats Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_stats_set(int unit, bcm_ifa_stat_info_t *stats);

/*!
 * \brief Get the IFA EApp stats.
 *
 * Get the IFA EApp stats.
 *
 * \param [in] unit Unit number.
 * \param [out] stats Statistics.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_stats_get(int unit, bcm_ifa_stat_info_t *stats);

/*!
 * \brief Create the IFA IPFIX template.
 *
 * Create the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] options Options.
 * \param [inout] Id Template Id.
 * \param [in] set_id IPFIX Set ID.
 * \param [in] num_export_elements Number of export elements.
 * \param [in] export_elements List of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_template_create(int unit, uint32 options,
                                 bcm_ifa_export_template_t *id, uint16_t set_id,
                                 int num_export_elements,
                                 bcm_ifa_export_element_info_t *export_elements);

/*!
 * \brief Get the IFA IPFIX template.
 *
 * Get the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 * \param [out] set_id IPFIX Set ID.
 * \param [in] max_size Size of the export_elements array.
 * \param [out] export_elements List of export elements.
 * \param [out] num_export_elements Number of export elements.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_template_get(int unit, bcm_ifa_export_template_t id,
                              uint16_t *set_id, int max_export_elements,
                              bcm_ifa_export_element_info_t *export_elements,
                              int *num_export_elements);

/*!
 * \brief Destroy the IFA IPFIX template.
 *
 * Destroy the IFA IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [in] Id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_template_destroy(int unit, bcm_ifa_export_template_t id);

/*!
 * \brief Attach the IFA collector.
 *
 * Attach the IFA IPFIX collector.
 *
 * \param [in] unit Unit number.
 * \param [in] collector_id Collector ID
 * \param [in] export_profile_id Export profile ID
 * \param [in] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_collector_attach(int unit,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_ifa_export_template_t template_id);

/*!
 * \brief Get the IFA collector details.
 *
 * Get the attached IFA collector, export profile and IPFIX template.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_collector_attach_get(int unit,
                                      bcm_collector_t *collector_id,
                                      int *export_profile_id,
                                      bcm_ifa_export_template_t *template_id);

/*!
 * \brief Detach the IFA collector.
 *
 * Detach the IFA collector.
 *
 * \param [in] unit Unit number.
 * \param [out] collector_id Collector ID
 * \param [out] export_profile_id Export profile ID
 * \param [out] template_id Template Id.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_collector_detach(int unit,
                                  bcm_collector_t collector_id,
                                  int export_profile_id,
                                  bcm_ifa_export_template_t template_id);

/*!
 * \brief Set the template transmit configuration.
 *
 * Set the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [in] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_template_transmit_config_set(int unit,
                                              bcm_ifa_export_template_t template_id,
                                              bcm_collector_t collector_id,
                                              bcm_ifa_template_transmit_config_t *config);

/*!
 * \brief Get the template transmit configuration.
 *
 * Get the template transmit configuration.
 *
 * \param [in] unit Unit number.
 * \param [in] template_id Template ID
 * \param [in] collector_id Collector ID
 * \param [out] config Template transmit config.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_template_transmit_config_get(int unit,
                                              bcm_ifa_export_template_t template_id,
                                              bcm_collector_t collector_id,
                                              bcm_ifa_template_transmit_config_t *config);

/*!
 * \brief Detach the IFA EApp.
 *
 * Detach the  IFA EApp related logical tables if it is already initalized.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_detach(int unit);

/*!
 * \brief Initialize the IFA EApp.
 *
 * Initialize IFA EApp related logical tables.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmi_ltsw_ifa_uc_init(int unit);

#endif /* BCMINT_LTSW_IFA_UC_H */
