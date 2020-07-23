/*! \file lb_hash.h
 *
 * LB hash header file.
 * This file contains LB hash definitions internal to the BCM library.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XGS_LB_HASH_H
#define BCMINT_LTSW_XGS_LB_HASH_H

#include <bcm_int/ltsw/lb_hash.h>
#include <bcmlt/bcmlt.h>
#include <bcm_int/ltsw/lt_intf.h>


/*!
 * \brief De-init the LB hash module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_deinit(int unit);

/*!
 * \brief Initialize the LB hash module.
 *
 * \param [in] unit Unit Number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_init(int unit);

/*!
 * \brief Set the port based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The port based hash output selection type.
 * \param [in] port Port Number.
 * \param [in] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_os_port_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int value);

/*!
 * \brief Get the port based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The port based hash output selection type.
 * \param [in] port Port Number.
 * \param [out] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_os_port_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcm_port_t port,
    int *value);

/*!
 * \brief Set the flow based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The flow based hash output selection type.
 * \param [in] field The flow based hash output selection field.
 * \param [in] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_os_flow_control_set(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int value);

/*!
 * \brief Get the flow based hash output selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The flow based hash output selection type.
 * \param [in] field The flow based hash output selection field.
 * \param [out] value The hash output selection control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_os_flow_control_get(
    int unit,
    bcmi_ltsw_lb_hash_os_t type,
    bcmi_ltsw_lb_hash_flow_field_t field,
    int *value);

/*!
 * \brief Set the hash field selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The hash fields selection type.
 * \param [in] value The hash fields selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_fields_select_set(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int value);

/*!
 * \brief Set the hash field selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] type The hash fields selection type.
 * \param [out] value The hash fields selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_fields_select_get(
    int unit,
    bcmi_ltsw_lb_hash_fields_select_t type,
    int *value);

/*!
 * \brief Set the hash bin assignment.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash bin control.
 * \param [in] value The hash bin control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_bins_set(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int value);

/*!
 * \brief Get the hash bin assignment.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash bin control.
 * \param [out] value The hash bin control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_bins_get(
    int unit,
    bcmi_ltsw_lb_hash_bin_t control,
    int *value);

/*!
 * \brief Set the hash packet header selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash packet header selection.
 * \param [in] value The hash packet header selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_pkt_hdr_sel_set(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int value);

/*!
 * \brief Get the hash packet header selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash packet header selection.
 * \param [out] value The hash packet header selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_pkt_hdr_sel_get(
    int unit,
    bcmi_ltsw_lb_hash_phs_t control,
    int *value);

/*!
 * \brief Set the hash ip6 collapse selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash ip6 collapse selection.
 * \param [in] value The hash ip6 collapse selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_ip6_collapse_set(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int value);

/*!
 * \brief Get the hash ip6 collapse selection.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash ip6 collapse selection.
 * \param [out] value The hash ip6 collapse selection value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_ip6_collapse_get(
    int unit,
    bcmi_ltsw_lb_hash_ip6cs_t control,
    int *value);

/*!
 * \brief Set the hash algorithm.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash algorithm control.
 * \param [in] value The hash algorithm control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_algorithm_set(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int value);

/*!
 * \brief Get the hash algorithm.
 *
 * \param [in] unit Unit Number.
 * \param [in] control The hash algorithm control.
 * \param [out] value The hash algorithm control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_algorithm_get(
    int unit,
    bcmi_ltsw_lb_hash_algorithm_t control,
    int *value);

/*!
 * \brief Set the symmetric hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] value The symmetric hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_symmetric_control_set(
    int unit,
    int value);

/*!
 * \brief Get the symmetric hash control.
 *
 * \param [in] unit Unit Number.
 * \param [out] value The symmetric hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_symmetric_control_get(
    int unit,
    int *value);

/*!
 * \brief Set the hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Hash control type.
 * \param [in] value The hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_control_set(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int value);

/*!
 * \brief Get the hash control.
 *
 * \param [in] unit Unit Number.
 * \param [in] control Hash control type.
 * \param [out] value The hash control value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_control_get(
    int unit,
    bcmi_ltsw_hash_control_t control,
    int *value);

/*!
 * \brief Enable/Disable assignment of hash bins
 *        for flex hash.
 *
 * \param [in] unit Unit Number.
 * \param [in] enable Enable Flex Hash
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
xgs_ltsw_lb_hash_bins_flex_set(
    int unit,
    bool enable);

/*!
 * \brief Allocate flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] qset Field Qualifier set.
 * \param [out] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_alloc(
    int unit,
    int qual_count,
    int *qual_list,
    bcm_hash_entry_t *entry);

/*!
 * \brief Free flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_free(
    int unit,
    bcm_hash_entry_t entry);

/*!
 * \brief Install flex hash entry to hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] offset L4 offset.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_install(
    int unit,
    bcm_hash_entry_t entry,
    int r,
    uint32 offset);

/*!
 * \brief Remove flex hash entry from hardware.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_remove(
    int unit,
    bcm_hash_entry_t entry);

/*!
 * \brief Set flex hash entry.
 *
 * This sets up Hash Bin extraction configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] config Hash bin configurations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_set(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config);

/*!
 * \brief Get flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [out] config Hash bin configurations.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_switch_hash_entry_config_t *config);

/*!
 * \brief Traverse flex hash entries.
 *
 * \param [in] unit Unit number.
 * \param [in] flags Flags.
 * \param [in] cb_fn Callback function pointer.
 * \param [in] user_data cookie
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_entry_traverse(
    int unit,
    int flags,
    bcm_switch_hash_entry_traverse_cb cb_fn,
    void *user_data);

/*!
 * \brief Qualify UDF data to flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] udf_id UDF object Identifier.
 * \param [in] length Number of bytes in 'data'.
 * \param [in] data Data to match UDF extraction.
 * \param [in] mask Mask to be applied.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_qualify_udf(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int length,
    uint8 *data,
    uint8 *mask);

/*!
 * \brief Get UDF data qualified to flex hash entry.
 *
 * \param [in] unit Unit number.
 * \param [in] entry Entry Identifier.
 * \param [in] udf_id UDF object Identifier.
 * \param [in] max_length Number of bytes in 'data'.
 * \param [out] data Data.
 * \param [out] mask.
 * \param [out] actual_length Number of bytes valid in 'data'.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_flex_qualify_udf_get(
    int unit,
    bcm_hash_entry_t entry,
    bcm_udf_id_t udf_id,
    int max_length,
    uint8 *data,
    uint8 *mask,
    int *actual_length);

/*!
 * \brief Set lb hash control seed.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [in] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_control_seed_set(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int value);

/*!
 * \brief Get lb hash control seed.
 *
 * \param [in] unit Unit number.
 * \param [in] control Switch control.
 * \param [out] value control value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_control_seed_get(
    int unit,
    bcmi_ltsw_hash_control_seed_t control,
    int *value);

/*!
 * \brief Set macro flow hash seed.
 *
 * \param [in] unit Unit number.
 * \param [in] type Macro flow type.
 * \param [int] value Seed value.
 *
 * \retval SHR_E_NONE on success and error code otherwise.
 */
extern int
xgs_ltsw_lb_hash_macro_flow_seed_set(
    int unit,
    bcmi_ltsw_macro_flow_t type,
    int value);

#endif /* BCMINT_LTSW_XGS_LB_HASH_H */
