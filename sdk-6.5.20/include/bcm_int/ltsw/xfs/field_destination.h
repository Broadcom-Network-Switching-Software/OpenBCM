/*! \file field_destination.h
 *
 * Field destination headfiles to declare internal APIs for XFS devices.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XFS_FIELD_DESTINATION_H
#define BCMINT_LTSW_XFS_FIELD_DESTINATION_H
/*!
 * \brief XFS field destination init.
 *
 * \param [in]  unit       Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_init(int unit);

/*!
 * \brief Field destination deinit.
 *
 * \param [in]  unit       Unit number.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_deinit(int unit);

/*!
 * \brief Get the mirror index used by the Field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 * \param [out] mirror_index  Mirror index.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_mirror_index_get(
    int unit,
    bcm_field_destination_match_t  *match,
    int *mirror_index);

/*!
 * \brief Add a field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  options    Entry operation options.
 * \param [in]  match      Match info.
 * \param [in]  action     Action info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_entry_add(
    int unit,
    uint32   options,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action);

/*!
 * \brief Get a field destination entry.
 *
 * \param [in]  unit        Unit number.
 * \param [in/out]  match   Match info.
 * \param [out]  action     Action info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_entry_get(
    int unit,
    bcm_field_destination_match_t  *match,
    bcm_field_destination_action_t  *action);

/*!
 * \brief Traverse field destination entries.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  callback   User callback.
 * \param [in]  user_data  User cookie.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_entry_traverse(
    int unit,
    bcm_field_destination_entry_traverse_cb callback,
    void *user_data);

/*!
 * \brief Delete a field destination entry.
 *
 * \param [in]  unit       Unit number.
 * \param [in]  match      Match info.
 *
 * \retval SHR_E_NONE      No errors.
 * \retval !SHR_E_NONE     Failure.
 */
extern int
xfs_ltsw_field_destination_entry_delete(
    int unit,
    bcm_field_destination_match_t  *match);

/*!
 * \brief Attach a flex counter to the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_field_destination_flexctr_attach(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Get the info of a flex counter attached to
 *        the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [out] info             Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_field_destination_flexctr_info_get(
    int unit,
    bcm_field_destination_match_t *match,
    bcmi_ltsw_flexctr_counter_info_t *info);

/*!
 * \brief Detach a flex counter from the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] info              Counter information.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_field_destination_flexctr_detach(
    int unit,
    bcm_field_destination_match_t *match);

/*!
 * \brief Set the flexctr object for the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [in] value             Flexctr object value.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_field_destination_flexctr_object_set(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t value);

/*!
 * \brief Get the flexctr object of the given field destination entry.
 *
 * \param [in] unit              Unit number.
 * \param [in] match             Match info.
 * \param [out] value            Flexctr object value.
 *
 * \retval SHR_E_NONE            No errors.
 * \retval !SHR_E_NONE           Failure.
 */
extern int
xfs_ltsw_field_destination_flexctr_object_get(
    int unit,
    bcm_field_destination_match_t *match,
    uint32_t *value);

#endif /* BCMINT_LTSW_XFS_FIELD_DESTINATION_H */
