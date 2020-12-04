/*! \file bcma_ha.h
 *
 * Application High Availability API.
 *
 * Application functions require to initialize the HA component
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMA_HA_H
#define BCMA_HA_H

/*!
 * \brief Invalid HA file instance number
 *
 * Instance number can be assigned to each HA file when running at multiple
 * instance environment (mostly used by QA).
 */
#define BCMA_HA_INVALID_INSTANCE        ((uint32_t)-1)

/*!
 * \brief Default HA file size.
 *
 * The default HA file size to be used with \ref bcma_ha_mem_init when
 * called with a valid unit number.
 */
#define DEFAULT_HA_FILE_SIZE            (4 * 1024 * 1024)

/*!
 * \brief Default HA generic file size.
 *
 * The default HA file size to be used with \ref bcma_ha_mem_init when
 * called with the generic unit number (\c BCMHA_GEN_UNIT).
 */
#define DEFAULT_HA_GEN_FILE_SIZE        (10 * 1024)

/*!
 * \brief Initialize HA memory instance.
 *
 * This function allocates and initializes resources for a given HA
 * memory instance.
 *
 * If \c warm is false, then the HA memory instance is initialized
 * from scratch. If \c warm is true, then the function will attempt to
 * use an existing HA file to restore the HA memory.
 *
 * If the provided \c size turns out to be too small, the HA memory
 * manager will automatically attempt to increase the amount of HA
 * memory.
 *
 * \param [in] unit Unit number or -1 for generic HA memory.
 * \param [in] size Initial size of the HA memory.
 * \param [in] warm True if warm-boot, otherwise cold-boot.
 * \param [in] inst SDK instance number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient memory.
 */
extern int
bcma_ha_mem_init(int unit, int size, bool warm, int inst);

/*!
 * \brief Clean up HA memory instance.
 *
 * This function releases all resources allocated for a given HA
 * memory instance.
 *
 * If \c warm_next is false, then any associated HA files will be
 * deleted as well. If \c warm_next is true, then all associated HA
 * files will be left untouched, and these files can then be used for
 * a subsequent warm-boot.
 *
 * \param [in] unit Unit number or -1 for generic HA memory.
 * \param [in] warm_next Do not delete HA files if true.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_MEMORY Insufficient memory.
 */
extern int
bcma_ha_mem_cleanup(int unit, bool warm_next);

/*!
 * \brief Get HA memory name of the specified unit.
 *
 * This function provides the HA memory name for the specified
 * unit. Typically this will be the name of a shared file.
 *
 * This function should be called after \ref bcma_ha_mem_init and
 * before \ref bcma_ha_mem_cleanup.
 *
 * \param [in] unit Unit number or -1 for generic HA memory.
 * \param [in] buf_size The size of \c buf.
 * \param [out] buf Buffer to hold the HA memory name.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcma_ha_mem_name_get(int unit, int buf_size, char *buf);

/*!
 * \brief Initialize all HA memory instances.
 *
 * This function initializes all the HA memory instances that are
 * associated with units that were discovered but not attached yet. It
 * also initializes the generic HA memory instance.
 *
 * \param [in] warm_boot Indicates if this is cold or warm boot.
 * \param [in] instance Indicates the instance of the shared file. The instance
 * being used as the file name suffix.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_MEMORY Insufficient memory to complete this task.
 */
extern int
bcma_ha_mem_init_all(bool warm_boot, uint32_t instance);

/*!
 * \brief Clean up all HA memory instance.
 *
 * This function cleans up all the HA memory instances that are
 * associated with attached units as well as the generic HA memory
 * instance.
 *
 * \param [in] keep_ha_file True to keep the HA file for next warm boot.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcma_ha_mem_cleanup_all(bool keep_ha_file);

/*!
 * \brief Open unit based HA file.
 *
 * Note: This API is deprecated.
 *
 * This function must be called prior to the device attachment call
 * (\c bcmlt_device_attach).
 *
 * \param [in] unit The newly discovered unit number.
 * \param [in] size The total HA memory that is expected to be used for
 *                  this unit. If the number is too small the system will
 *                  automatically adjust to the proper size.
 *                  However, it will be slightly more efficient to provide
 *                  sufficient size that doesn't require any expantions.
 *                  Note that significantly larger size than necessary
 *                  simply consumes more memory.
 * \param [in] enabled Indicates if HA memory should be enabled or regular
 *                     memory can be used instead.
 * \param [in] warm Indicates if this is a warm or cold boot.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_unit_open(int unit, int size, bool enabled, bool warm);

/*!
 * \brief Extended open unit based HA file.
 *
 * Note: This API is deprecated.
 *
 * This function must be called prior to the device attachment call
 * (\c bcmlt_device_attach).
 *
 * \param [in] unit The newly discovered unit number.
 * \param [in] size The total HA memory that is expected to be used for
 *                  this unit. If the number is too small the system will
 *                  automatically adjust to the proper size.
 *                  However, it will be slightly more efficient to provide
 *                  sufficient size that doesn't require any expantions.
 *                  Note that significantly larger size than necessary
 *                  simply consumes more memory.
 * \param [in] enabled Indicates if HA memory should be enabled or regular
 *                     memory can be used instead.
 * \param [in] warm Indicates if this is a warm or cold boot.
 * \param [in] instance Indicates the instance of the shared file. The instance
 * being used as the file name suffix.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_unit_open_ext(int unit, int size, bool enabled,
                      bool warm, uint32_t instance);

/*!
 * \brief Close HA file for a unit.
 *
 * Note: This API is deprecated.
 *
 * This function closes the HA file for a given unit. It should be called
 * post the detach function (\c bcmlt_device_detach).
 *
 * \param [in] unit The unit number to close.
 * \param [in] warm_next If the value of this parameter is true, it indicates
 *                       that the current HA file should be kept for
 *                       next (warm) boot. Otherwise, the current HA file
 *                       will be erased.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_unit_close(int unit, bool warm_next);

/*!
 * \brief Open generic HA file.
 *
 * Note: This API is deprecated.
 *
 * This function is similar to the function \ref bcma_ha_unit_open with the
 * exception that it is not unit specific. Typically the generic memory should
 * be initialized first.
 *
 * \param [in] size The total HA memory that is expected to be used for
 *                  this unit. If the number is too small the system will
 *                  automatically adjust to the proper size.
 *                  However, it will be slightly more efficient to provide
 *                  sufficient size that doesn't require any expantions.
 *                  Note that significantly larger size than necessary
 *                  simply consumes more memory.
 * \param [in] enabled Indicates if HA memory should be enabled or regular
 *                     memory can be used instead.
 * \param [in] warm Indicates if this is a warm or cold boot.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_gen_open(int size, bool enabled, bool warm);

/*!
 * \brief Extended open generic HA file.
 *
 * Note: This API is deprecated.
 *
 * This function is similar to the function \ref bcma_ha_unit_open_ext with the
 * exception that it is not unit specific. Typically the generic memory should
 * be initialized first.
 *
 * \param [in] size The total HA memory that is expected to be used for
 *                  this unit. If the number is too small the system will
 *                  automatically adjust to the proper size.
 *                  However, it will be slightly more efficient to provide
 *                  sufficient size that doesn't require any expantions.
 *                  Note that significantly larger size than necessary
 *                  simply consumes more memory.
 * \param [in] enabled Indicates if HA memory should be enabled or regular
 *                     memory can be used instead.
 * \param [in] warm Indicates if this is a warm or cold boot.
 * \param [in] instance Indicates the instance of the shared file. The instance
 * being used as the file name suffix.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_gen_open_ext(int size, bool enabled, bool warm, uint32_t instance);

/*!
 * \brief Close generic HA file.
 *
 * Note: This API is deprecated.
 *
 * This function closes the generic HA file. It operates similarly to the
 * function \ref bcma_ha_unit_close with the exception that it doesn't operates
 * on a specific unit.
 *
 * \param [in] warm_next If the value of this parameter is true, it indicates
 *                       that the current HA file should be kept for
 *                       next (warm) boot. Otherwise, the current HA file
 *                       will be erased.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
bcma_ha_gen_close(bool warm_next);

/*!
 * \brief Open all HA files.
 *
 * Note: This API is deprecated.
 *
 * This function opens all the HA files that are associated with units that were
 * discovered but not attached yet. It also open the generic HA file.
 *
 * \param [in] enable Set to true for HA functionality.
 * \param [in] warm_boot Indicates if this is cold or warm boot.
 * \param [in] instance Indicates the instance of the shared file. The instance
 * being used as the file name suffix.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_MEMORY Insufficient memory to complete this task.
 */
extern int
bcma_ha_open_all(bool enable, bool warm_boot, uint32_t instance);

/*!
 * \brief Close all opened HA files.
 *
 * Note: This API is deprecated.
 *
 * This function closes all the HA files that are associated with attached units
 * as well as the generic HA file.
 *
 * \param [in] keep_ha_file True to keep the HA file for next warm boot.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcma_ha_close_all(bool keep_ha_file);

/*!
 * \brief Dump the content of all HA files into external storage.
 *
 * This function takes a snapshot of the system state by dumping the
 * content of all active HA blocks into a file. The file can later be
 * used to compare a particular state with the dumped state (see \ref
 * bcma_ha_mem_state_comp_all and \ref bcma_ha_mem_state_comp).
 *
 * \param [in] state_path The path where to store the state file.
 *
 * \retval SHR_E_NONE Success.
 */
extern int
bcma_ha_mem_state_dump_all(const char *state_path);

/*!
 * \brief Compare all active HA states to stored HA states.
 *
 * This function compares each open HA file to its corresponding stored file. It
 * reports for every mismatched files via BSL LOG message.
 *
 * \param [in] state_path The path where the state file can be retrieved from.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_IO Failed to access the input storage.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL Comparison failed.
 */
extern int
bcma_ha_mem_state_comp_all(const char *state_path);

/*!
 * \brief Dump the system state with respect to a single unit.
 *
 * This function saves the current HA file content for the associated given unit
 * into external storage. The saved content can later be compared with the
 * running HA content using the function \ref bcma_ha_mem_state_comp.
 *
 * \param [in] unit Associated with the HA state to save.
 * \param [in] state_path The path where the state file had been stored.
 *
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_IO Failed to create output storage.
 */
extern int
bcma_ha_mem_state_dump(int unit, const char *state_path);

/*!
 * \brief Compare the current system state to its store state for a single unit.
 *
 * This function compares the content of the system state stored in HA memory
 * to the content that was previously saved
 * (see \ref bcma_ha_mem_state_dump).
 *
 * \param [in] unit Associated with the HA state to compare.
 * \param [in] state_path The path where the state file had been stored.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_IO Failed to access the output storage.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL Comparison failed.
 */
extern int
bcma_ha_mem_state_comp(int unit, const char *state_path);

#endif /* BCMA_HA_H */
