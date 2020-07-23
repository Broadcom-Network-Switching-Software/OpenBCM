/*! \file ha.h
 *
 * Sample HA management definitions.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef DIAG_LTSW_HA_H
#define DIAG_LTSW_HA_H

#include <sal/sal_types.h>
#include <shr/shr_pb.h>

/*! The default HA file size. */
#define DEFAULT_HA_FILE_SIZE 0x400000

/*! The default HA generic file size. */
#define DEFAULT_HA_GEN_FILE_SIZE 0x2800  /* 10 KB */

/*!
 * \brief Initialize HA for a unit.
 * This function must be called prior to device attach.
 *
 * \param [in] unit The unit number.
 * \param [in] size is the total HA memory that is expected to be used for
 * this unit. If the number is too small the system will automatically
 * adjust to the proper size. However, it will be slightly more efficient
 * to provide sufficient size that doesn't require any expantions. Note that
 * significantly larger size than necessary simply consumes more memory.
 * \param [in] enable If true enable using HA memory, otherwise implement via
 * \c SAL alloc and free functions.
 * \param [in] warm Indicates if this is a warm or cold boot.
 *
 * \return SHR_E_NONE success, otherwise failure.
 */
extern int
appl_ltsw_ha_unit_open(int unit, int size, int enabled, int warm);

/*!
 * \brief Close HA files for a unit.
 * This function closes the HA file for a given unit. It should be called
 * post the detach functions of all modules.
 *
 * \param [in] unit The unit number to close.
 * \param [in] enable Indicates if HA share memory is enabled.
 * \param [in] warm_exit If the value of this parameter is true, it indicates
 * that the current HA file should be kept for next (warm) boot. Otherwise,
 * the current HA file will be erased.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
appl_ltsw_ha_unit_close(int unit, int enabled, int warm_exit);

/*!
 * \brief Initialize HA file for generic category.
 * This function must be called prior to device attach.
 *
 * \param [in] size is the total HA memory that is expected to be used for
 * this unit. If the number is too small the system will automatically
 * adjust to the proper size. However, it will be slightly more efficient
 * to provide sufficient size that doesn't require any expantions. Note that
 * significantly larger size than necessary simply consumes more memory.
 * \param [in] enable If true enable using HA memory, otherwise implement via
 * \c SAL alloc and free functions.
 * \param [in] warm Indicates if this is a warm or cold boot.
 *
 * \return SHR_E_NONE success, otherwise failure.
 */
extern int
appl_ltsw_ha_gen_open(int size, int enabled, int warm);

/*!
 * \brief Close HA files for generic category.
 * This function closes the HA generic file. It should be called
 * post the detach functions of all modules.
 *
 * \param [in] enable Indicates if HA share memory is enabled.
 * \param [in] warm_exit If the value of this parameter is true, it indicates
 * that the current HA file should be kept for next (warm) boot. Otherwise,
 * the current HA file will be erased.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
appl_ltsw_ha_gen_close(int enabled, int warm_exit);

/*!
 * \brief Get HA file name of the specified unit.
 *
 * This function can provide the HA file name in full path for the specified
 * instance.
 *
 * \param [in] inst Instance number.
 * \param [in] buf Buffer to hold the HA file path/name.
 * \param [in] buf_size The size of \c buf.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
appl_ltsw_ha_file_name_get(int inst, char *buf, int buf_size);

/*!
 * \brief Get generic HA file name.
 *
 * This function can provide the HA generic file name in full path.
 *
 * \param [in] buf Buffer to hold the HA file path/name.
 * \param [in] buf_size The size of \c buf.
 *
 * \return 0 if no errors, otherwise error code.
 */
extern int
appl_ltsw_ha_gen_file_name_get(char *buf, int buf_size);

/*!
 * \brief Dump the system state with respect to a single unit.
 *
 * This function saves the current HA file content for the associated given unit
 * into external storage. The saved content can later be compared with the
 * running HA content using the function \ref bcma_ha_unit_state_comp().
 *
 * \param [in] unit Associated with the HA state to save.
 * \param [in] state_path The path where the state file had been stored.
 *
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_IO Failed to create output storage.
 */
extern int
appl_ltsw_ha_unit_state_dump(int unit, const char *state_path);


/*!
 * \brief Compare the current system state to its store state for a single unit.
 *
 * This function compares the content of the system state stored in HA memory
 * to the content that was previously saved
 * (see \ref bcma_ha_unit_state_dump()).
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
appl_ltsw_ha_unit_state_comp(int unit, const char *state_path);

/*!
 * \brief Get the HA memory usage for the specified component name.
 *
 * The HA component memory usage includes per-unit memory usage and
 * generic memory usage. The argument \c gen_mem_inc can be used to decide
 * whether to include generic memory usage to the HA memory usage result.
 *
 * This function will also format the component memory usage
 * (including per sub-id of the specified component) to print buffer
 * if \c pb is not NULL. If \c fmt_hdr is TRUE, a header information will
 * be first added to print buffer \c pb.
 *
 * \param [in] unit Unit number.
 * \param [in] comp_name Component name.
 * \param [in] gen_mem_inc Include HA generic memory usage if TURE.
 * \param [in] pb Print buffer.
 * \param [in] fmt_hdr Whether to format header to print buffer.
 *
 * \return The HA memory usage in bytes for component \c comp_name.
 */
extern uint32_t
appl_ltsw_ha_comp_mem_usage(int unit, const char *comp_name, bool gen_mem_inc,
                            shr_pb_t *pb, bool fmt_hdr);

/*!
 * \brief Get the HA memory usage for the current device.
 *
 * The HA memory usage includes per-unit memory usage and generic memory usage.
 * The argument \c gen_mem_inc can be used to decide whether to include
 * generic memory usage to the HA memory usage result.
 *
 * This function will also format the HA memory usage (including per-component
 * usage) to print buffer if \c pb is not NULL. If \c fmt_hdr is TRUE,
 * a header information will be first added to print buffer \c pb.
 *
 * \param [in] unit Unit number.
 * \param [in] gen_mem_inc Include HA generic memory usage if TURE.
 * \param [in] pb Print buffer.
 * \param [in] fmt_hdr Whether to format header to print buffer.
 *
 * \return The HA memory usage in bytes for the current device.
 */
extern uint32_t
appl_ltsw_ha_mem_usage(int unit, bool gen_mem_inc, shr_pb_t *pb, bool fmt_hdr);

#endif /* DIAG_LTSW_HA_H */
