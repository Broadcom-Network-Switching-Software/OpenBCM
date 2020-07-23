/*! \file issu.h
 *
 * BCM internal APIs for ISSU (a.k.a. In Service Software Upgrade).
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef BCMI_LTSW_ISSU_H
#define BCMI_LTSW_ISSU_H

#include <sal/sal_types.h>
#include <bcmissu/issu_api.h>

/*!
 * \brief ISSU upgrade start.
 *
 * This function initiates the ISSU process. It starts by loading the ISSU DLL.
 * It laters informs the system manager engine about the ongoing update by
 * assigning values to callback functions being executed during the pre config
 * stage.
 *
 * \param [in] from_ver Is the previous version of software that was running.
 * \param [in] to_ver Is the version of the current software.
  *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_issu_upgrade_start(const char *from_ver,  const char *to_ver);

/*!
 * \brief ISSU upgrade done.
 *
 * This function releases all the resources that were used by the ISSU. In
 * particular it unloads the ISSU DLL. It is assume that this function is being
 * called once all the units had completed their upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_issu_upgrade_done(void);

/*!
 * \brief Initialize the ISSU structure and update HA while warmboot enabled.
 *
 * \param [in] unit The unit associated with the HA memory.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int
bcmi_ltsw_issu_init(int unit);

/*!
 * \brief Detach the ISSU structure control DB.
 *
 * This function detaches the ISSU structure control DB by freeing some
 * resources that were used in the DB operations.
 *
 * \param [in] unit The unit associated with the structure control DB.
 *
 * \retval The calculated size of the structure.
 */
extern int
bcmi_ltsw_issu_detach(int unit);

/*!
 * \brief Inform the ISSU the location of a data structure within the HA block.
 *
 * Through this function a component can inform the ISSU manager about the
 * location of a known HA data structure within an HA memory block. This
 * information is being used during the ISSU process where the ISSU manager
 * determines if a particular data structure has changed and therefore needs
 * to be updated to the new structure while maintaining the previous values.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] comp_id Is the component ID that allocated the HA block.
 * \param [in] sub_id Is the sub module ID that together with the \c comp_id
 * identifies the HA block.
 * \param [in] offset The number of bytes from the beginning of the HA block
 * where the structure is being located.
 * \param [in] data_size the space occupied by the structure. If a structure
 * contains unbounded array this parameter should include the actual size of
 * the unbounded array.
 * \param [in] instances The number of reported structures in the array.
 * \param [in] id An automatically generated unique ID for the data structure.
 *
 * \return SHR_E_NONE success, otherwise SHR_E_PARAM due to wrong parameters.
 */
extern int
bcmi_ltsw_issu_struct_info_report(int unit,
                                  shr_ha_mod_id comp_id,
                                  shr_ha_sub_id sub_id,
                                  uint32_t offset,
                                  uint32_t data_size,
                                  size_t instances,
                                  bcmissu_struct_id_t id);

/*!
 * \brief Retrieve the location of a data structure within an HA memory block.
 *
 * The purpose of this function is to retrieve the location of an HA data
 * structure that was previously reported to the HA manager via
 * \ref shr_ha_struct_report() function.
 * This function should only be called during warm boot where the component
 * needs to find out the location of its own structures within HA memory. This
 * functionality is particularly important after software upgrade where the
 * locations of the structures can shift within the block due to the upgrade
 * migration process.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] id Unique generated structure ID.
 * \param [in] comp_id Is the component ID that allocated the HA block.
 * \param [in] sub_id Is the sub module ID that together with the \c comp_id
 * identifies the HA block.
 * \param [out] offset The offset from the beginning of the block were the
 *        data structure is located.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
extern int
bcmi_ltsw_issu_struct_info_get(int unit,
                               bcmissu_struct_id_t id,
                               shr_ha_mod_id comp_id,
                               shr_ha_sub_id sub_id,
                               uint32_t *offset);

/*!
 * \brief Clear the location of a structure in HA memory block.
 *
 * This function is the inverse of the function \ref shr_ha_struct_report().
 * It indicates that a specific data structure is no longer available within
 * the provided HA memory block.
 *
 * \param [in] unit The unit associated with the HA memory.
 * \param [in] id Unique generated structure ID.
 * \param [in] comp_id Is the component ID that allocated the HA block.
 * \param [in] sub_id Is the sub module ID that together with the \c comp_id
 * identifies the HA block.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
extern int
bcmi_ltsw_issu_struct_info_clear(int unit,
                                 bcmissu_struct_id_t id,
                                 shr_ha_mod_id comp_id,
                                 shr_ha_sub_id sub_id);

#endif /* BCMI_LTSW_ISSU_H */
