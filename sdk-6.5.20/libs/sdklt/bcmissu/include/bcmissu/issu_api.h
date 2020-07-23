/*! \file issu_api.h
 *
 * ISSU API decleration.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef ISSU_API_H
#define ISSU_API_H

#include <shr/shr_ha.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief Invalid ISSU enum value.
 * This value will be used when an enum value existed in previous version is no
 * longer exist in the current version. The enum conversion process will
 * replace the old value with this value. This will enable the component
 * to identify fields that are no longer valid.
 */
#define BCMISSU_INVALID_ENUM BCMLTD_SID_INVALID

/*!
 * \brief ISSU upgrade start.
 *
 * This function initiates the ISSU process. It starts by loading the ISSU DLL.
 * It then registers a monitor function with the system manager engine. The
 * actual update occurs from within the monitor function.
 * Last it informs the DLL of the start and end versions.
 *
 * \param [in] from_ver Is the previous version of software that was running.
 * \param [in] to_ver Is the version of the current software.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_upgrade_start(const char *from_ver,  const char *to_ver);

/*!
 * \brief ISSU upgrade done.
 *
 * This function releases all the resources that were used by the ISSU. In
 * particular it unloads the ISSU DLL. It is assume that this function is being
 * called once all the units had completed their upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_upgrade_done(void);

/*!
  \brief HA structure ID type.
 */
typedef uint64_t bcmissu_struct_id_t;

/*!
 * \brief Inform the location of a data structure instance within an HA block.
 *
 * Through this function a component can inform the ISSU manager about the
 * location of a known HA data structure within an HA memory block.The
 * reported information is being used during the ISSU process where the ISSU
 * manager determines if a particular data structure has changed and
 * therefore needs to be updated to the new structure while maintaining the
 * previous values.
 * The instances parameter enables the caller to report an array of
 * structures at once. The caller should be aware that the number of instances
 * can not be changed during an ISSU even without special code by the
 * caller. Therefore if the caller determines that the number of instances may
 * change over time it should place a dedicated code to adjust the HA block size
 * to accommodate the number of instances. It
 * may also be easier to maintain the number of reported instances within
 * the same HA block so upon warm boot it is possible to determine if the
 * number of instances remains the same.
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
extern int bcmissu_struct_info_report(int unit,
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
 * \ref bcmissu_struct_info_report() function.
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
extern int bcmissu_struct_info_get(int unit,
                                   bcmissu_struct_id_t id,
                                   shr_ha_mod_id comp_id,
                                   shr_ha_sub_id sub_id,
                                   uint32_t *offset);

/*!
 * \brief Clear the location of a structure in HA memory block.
 *
 * This function is the inverse of the function
 * \ref bcmissu_struct_info_report() and
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
extern int bcmissu_struct_info_clear(int unit,
                                     bcmissu_struct_id_t id,
                                     shr_ha_mod_id comp_id,
                                     shr_ha_sub_id sub_id);

/*!
 * \brief Inform the location of a data structure within a generic HA block.
 *
 * This function is similar to the function \ref bcmissu_struct_info_report()
 * with the exception that the HA block is associated with the generic HA memory
 * pool.
 *
 * \param [in] comp_id Is the component ID that allocated the HA block.
 * \param [in] sub_id Is the sub module ID that together with the \c comp_id
 * identifies the HA block.
 * \param [in] offset The number of bytes from the beginning of the HA block
 * where the structure is being located.
 * \param [in] data_size The actual size of the structure.
 * \param [in] instances The number of reported structures in the array.
 * \param [in] id An automatically generated unique ID for the data structure.
 *
 * \return SHR_E_NONE success, otherwise SHR_E_PARAM due to wrong parameters.
 */
extern int bcmissu_struct_info_gen_report(shr_ha_mod_id comp_id,
                                          shr_ha_sub_id sub_id,
                                          uint32_t offset,
                                          uint32_t data_size,
                                          size_t instances,
                                          bcmissu_struct_id_t id);

/*!
 * \brief Retrieve the location of a data structure within a generic HA block.
 *
 * This function is similar to the function  \ref bcmissu_struct_info_get with
 * the exception that the HA block is associated with the generic HA memory
 * pool.
 *
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
extern int bcmissu_struct_info_gen_get(bcmissu_struct_id_t id,
                                       shr_ha_mod_id comp_id,
                                       shr_ha_sub_id sub_id,
                                       uint32_t *offset);

/*!
 * \brief Clear the location of a structure in a generic HA block.
 *
 * This function is similar to the function  \ref bcmissu_struct_info_clear with
 * the exception that the HA block is associated with the generic HA memory
 * pool.
 *
 * \param [in] id Unique generated structure ID.
 * \param [in] comp_id Is the component ID that allocated the HA block.
 * \param [in] sub_id Is the sub module ID that together with the \c comp_id
 * identifies the HA block.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_INTERNAL Internal data structure mismatch.
 * \retval SHR_E_NOT_FOUND The structure ID was not found.
 */
extern int bcmissu_struct_info_gen_clear(bcmissu_struct_id_t id,
                                         shr_ha_mod_id comp_id,
                                         shr_ha_sub_id sub_id);

/*!
 * \brief Convert old LTID into current LTID.
 *
 * This function receives the LTID of the older version and returns the
 * current LTID (i.e. the LTID of the current running version).
 * Typically, the ISSU will automatically update all the LTIDs of reported
 * data structures. However, there might be some components that desire
 * to perform the upgrade by themselves. These components can call this
 * function.
 *
 * \param [in] unit The unit containing the LT.
 * \param [in] old_ltid The LTID before the conversion.
 * \param [out] current_ltid The current LTID.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND The old LTID was not found.
 * \retval SHR_E_UNAVAIL The LT is no longer exist.
 */
extern int bcmissu_ltid_to_current(int unit,
                                   uint32_t old_ltid,
                                   uint32_t *current_ltid);

/*!
 * \brief Convert old field ID into current field ID.
 *
 * This function receives the LTID and field ID of the older version and
 * retrieves the field ID of the current running version.
 * This function can be used by components that maintain the field IDs of a LT
 * as part of their state.
 *
 * \param [in] unit The unit containing the LT.
 * \param [in] old_ltid The LTID before the conversion.
 * \param [in] old_flid The field ID before the conversion.
 * \param [out] current_flid The current field ID.
 *
 * \retval SHR_E_NONE on success.
 * \retval SHR_E_PARAM Invalid input parameter.
 * \retval SHR_E_NOT_FOUND The old LTID was not found.
 */
extern int bcmissu_fid_to_current(int unit,
                                  uint32_t old_ltid,
                                  uint32_t old_flid,
                                  uint32_t *current_flid);

/*!
 * \brief Check if an LT definition had change.
 *
 * This function checks if the LT definition had changed. Change includes
 * different fields or change in field ID. It doesn't include changes
 * to a field type.
 *
 * \param [in] unit The unit containing the LT.
 * \param [in] ltid The original LTID.
 *
 * \retval TRUE if LT definitions had changed, FALSE otherwise.
 */
extern bool bcmissu_lt_has_changed(int unit, uint32_t ltid);


/*!
 * \brief Check if enum definition had changed.
 *
 * This function check if an enum type definition had changed from the start
 * to current version.
 *
 * \param [in] unit The unit associated with the enum type.
 * \param [in] enum_type_name The enum type name to check.
 *
 * \retval true if enum definition had changed and false otherwise.
 */
extern bool bcmissu_enum_has_changed(int unit, const char *enum_type_name);

/*!
 * \brief Convert old enum value to current value.
 *
 * This function converts the enum value of a given enum type to its
 * current value.
 *
 * It is recommended that the caller will first call \ref
 * bcmissu_enum_has_changed() before calling this function for
 * multiple instances of the same enum definition.
 *
 * \param [in] unit The unit associated with this enum type.
 * \param [in] enum_type_name The enum type name.
 * \param [in] old_val The older value of the enum symbol.
 * \param [out] current_val The current value of the enum symbol.
 *
 * \return SHR_E_NONE Success.
 * \return SHR_E_NOT_FOUND The original enum record was not found. This
 * indicates that the enum value was probably deleted or changed.
 * \return SHR_E_PARAM Invalid input parameter.
 */
extern int bcmissu_enum_sym_to_current(int unit,
                                       const char *enum_type_name,
                                       uint32_t old_val,
                                       uint32_t *current_val);


/*!
 * \brief ISSU native unit init.
 *
 * This function initializes a unit to support ISSU in a native mode
 * that is not under the system manager. Since in native mode, the unit number
 * is not limited to the physical units within the system. Call this function
 * prior to upgrade procedure or prior to ISSU structure reports
 * (\ref bcmissu_struct_info_report)
 *
 * \param [in] unit The unit to initialize. Prior of calling this
 * \param [in] comp_id Is the component ID assigned to the ISSU component in
 * the native environment.
 * \param [in] warm Indicates if the initialization is warm or cold.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_native_unit_init(int unit, shr_ha_mod_id comp_id, bool warm);

/*!
 * \brief Shut down the ISSU native unit.
 *
 * This function shuts down the structure control DB by freeing some
 * resources that were used in a native mode.
 * This operation is implicitly done by the system manager but explicitly
 * required when running in native mode.
 *
 * \param [in] unit The unit associated with the structure control DB.
 * \param [in] graceful Indicating graceful or abrupt shutdown.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_native_unit_shutdown(int unit, bool graceful);

/*!
 * \brief ISSU native upgrade start.
 *
 * This function initiates the ISSU process for software that is running in
 * native mode and not under the system manager.
 * It starts by loading the ISSU DLL.
 * It then informs the DLL of the start and end versions.
 *
 * \param [in] from_ver Is the previous version of software that was running.
 * \param [in] to_ver Is the version of the current software.
 * \param [in] dll_name Is the name of the shared library that contains the ISSU
 * data base. It is assume that the library search path is properly set into
 * the OS environment.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_native_upgrade_start(const char *from_ver,
                                        const char *to_ver,
                                        const char *dll_name);

/*!
 * \brief ISSU native unit upgrade.
 *
 * This function performs the ISSU process for software that is running in
 * native mode and not under the system manager. Since in native mode, the
 * upgrade is done all at once. Note that in this mode only software states can
 * be updated. The ISSU will not be able to perform any writes into the device.
 *
 * \param [in] unit The unit to upgrade. Note that this unit must be first
 * initialized by the function \ref bcmissu_native_unit_init.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_native_unit_upgrade(int unit);

/*!
 * \brief ISSU native upgrade done.
 *
 * This function releases all the resources that were used by the ISSU in native
 * mode. In particular it unloads the ISSU DLL. It is assume that this function
 * is being called once all the units had completed their upgrade.
 *
 * \return SHR_E_NONE on success and error code otherwise.
 */
extern int bcmissu_native_upgrade_done(void);

/*!
 * \brief Indicate if ISSU is currently active.
 *
 * ISSU is considered to be active after successful call to
 * \ref bcmissu_upgrade_start() and before a call to
 * \ref bcmissu_upgrade_done().
 * This function can be used by components that uses enum fields within their
 * ISSU data structures. It is possible that an old enum value had been deleted
 * from the enum definition. In such a scenario the ISSU will replace the old
 * value with \ref BCMISSU_INVALID_ENUM value. During warm boot a component
 * may use this function to weed out all its invalid enum fields.
 *
 * \return True if ISSU is currently active and false otherwise.
 */
extern bool bcmissu_is_active(void);


#endif /* ISSU_API_H */

