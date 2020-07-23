/*! \file bcmha_mem_diag.h
 *
 * HA memory diagnostics API.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMHA_MEM_DIAG_H
#define BCMHA_MEM_DIAG_H

#include <shr/shr_ha_types.h>

/*!
 * \brief Dump the contents of an HA memory instance.
 *
 * This function traverses all active HA memory blocks associated with
 * a given unit and writes the contents using the provided write
 * function.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] write_f I/O write function that receives a pointer to a
 *                     buffer and a buffer length. The I/O function
 *                     saves the content of the buffer so it can later
 *                     be retrieved.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The I/O function failed.
 */
extern int
bcmha_mem_content_dump(int unit, int (*write_f)(void *buf, size_t len));

/*!
 * \brief Compare HA memory block contents with saved contents.
 *
 * This function reads saved HA memory blocks via a provided read
 * function and compares the contents to the current HA memory blocks
 * with the same component and sub-component.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] read_f I/O read function that receives a pointer to a
 *                    buffer and a buffer length as input. The I/O
 *                    function reads \c len bytes into the provided
 *                    buffer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_IO The I/O function failed.
 * \retval SHR_E_MEMORY Failed to allocate working buffer.
 * \retval SHR_E_FAIL The content comparison failed.
 */
extern int
bcmha_mem_content_comp(int unit, int (*read_f)(void *buf, size_t len));

/*!
 * \brief Retrive per-component HA memory usage.
 *
 * For a given HA memory instance, this function provides a report on
 * the HA memory size allocated by each component. The report is
 * provided for each sub-component ID where the sub-component ID used
 * as the index in the \c reports array.
 *
 * \param [in] unit The unit number associated with this HA instance
 *                  or -1 for the generic instance.
 * \param [in] comp_id The memory usage will be provided for this component.
 * \param [in] report_size Indicates the \c reports array size. The
 *                         maximum size of the array can be \c
 *                         SHR_HA_SUB_IDS. If it is smaller the HA
 *                         will only report for sub-id that is smaller
 *                         then the report_size.
 * \param [out] reports Array containing the number of bytes allocated per
 *                      sub-component ID.
 *
 * \retval SHR_E_NONE success.
 * \retval SHR_E_NOT_FOUND When the component is not allocating any HA
 * resources.
 */
extern int
bcmha_mem_usage(int unit, shr_ha_mod_id comp_id,
                uint32_t report_size, shr_ha_usage_report_t *reports);

#endif /* BCMHA_MEM_DIAG_H */
