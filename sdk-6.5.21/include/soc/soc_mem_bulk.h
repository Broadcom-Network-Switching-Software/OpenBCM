/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *
 * Header file for SOC memory Bulk API.
 */

#ifndef _SOC_MEM_BULK_H
#define _SOC_MEM_BULK_H

#include <soc/mem.h>

/*!
 * Handle for SOC mem bulk  operation.
 */

typedef uint32 soc_mem_blk_handle_t;

/*!
 * SOC mem bulk operation flags
 */

typedef enum {
    /*! Init opeartion creates the handle and return as out parameter */
    SOC_MEM_BULK_WRITE_INIT = 1,

    /*!
     * Stage opeartion stages the schan commands in local queue
     * In case handle is null PIO is used to commit immediately
     */
    SOC_MEM_BULK_WRITE_STAGE,

    /* Commit opeartion will execute the commnand sequence */
    SOC_MEM_BULK_WRITE_COMMIT
} soc_mem_bulk_write_flags_t;

/*!
 * SOC bulk memory opeartion
 * NO memory read on same index between multiple STAGEs.
 * Memory cache will only be updated after a successful COMMIT.
 * Entry_data can be valid or NULL when flags OP is either INIT and COMMIT.
 * When flags OP is COMMIT: the handle would be deallocated.
 * When flags OP is COMMIT: is a blocked call till the operation is complete.
 * Entry_data: size should be less than or equal to 80 words.
 * No data will be staged during INIT and COMMIT Operation
 * handle  should only be used with the unit for which it is initialized.
 *
 * \param [in] unit, Logical device id.
 * \param [in] flags, Bulk memory op flags.
 * \param [in, out] handle, handle of soc mem bulk.
 * \param [in] mem, memory table type.
 * \param [in] index, index of memory table.
 * \param [in] copyno, block for memory table.
 * \param [in] entry_data, buffer pointer of entry data.
 *
 * \retval SHR_E_XXX
 */

extern int
soc_mem_bulk_op(int unit,
                int flags,
                soc_mem_blk_handle_t *handle,
                soc_mem_t mem,
                int index,
                int copyno,
                uint32 *entry_data);


/*!
 * Bulk write for memory entry array.
 * The call will be blocked till all the commands in array are  executed.
 * Same usage as soc_mem_write
 *
 * \param [in] unit, Logical device id.
 * \param [in] mem, array of memory table type.
 * \param [in] index, array of table index of memory table.
 * \param [in] copyno, array of block for memory table.
 * \param [in] entry_data, buffer pointer of entry data.
 * \param [in] count, count of entry data.
 *
 * \retval SHR_E_XXX
 */
extern int
soc_mem_bulk_write(int unit,
                   soc_mem_t *mem,
                   int *index,
                   int *copyno,
                   uint32 **entry_data,
                   int count);


/*!
 * Bulk mem module init
 * Initialize the software resources.
 *
 * \param [in] unit, Logical device id.
 *
 * \retval SHR_E_XXX
 */
extern int
soc_mem_bulk_init(int unit);

/*!
 * Bulk mem module cleanup
 * Cleanup the software resources.
 *
 * \param [in] unit, Logical device id.
 *
 * \retval SHR_E_XXX
 */
extern int
soc_mem_bulk_cleanup(int unit);


#endif
