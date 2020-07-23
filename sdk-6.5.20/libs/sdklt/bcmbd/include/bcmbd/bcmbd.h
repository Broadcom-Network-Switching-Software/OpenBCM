/*! \file bcmbd.h
 *
 * Top-level Base Driver (BD) APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_H
#define BCMBD_H

#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd_otp.h>

/*!
 * \name Range operation flags.
 *
 * These flags may be used for DMA operations on a range of table
 * entries.
 *
 * \anchor BCMBD_ROF_xxx
 */

/*! \{ */

/*! Use interrupt mode for memory DMA access. */
#define BCMBD_ROF_INTR          (1 << 0)

/*!
 * Prepare only a single entry data buffer for memory DMA access.
 * For reads, this can be used for test.
 * For writes, this can be used for clearing a memory.
 */
#define BCMBD_ROF_SGL_DATA      (1 << 1)

/*!
 * Use only a single HW entry address for memory DMA access.
 * This may be used to push/pop multiple FIFO memory entries.
 */
#define BCMBD_ROF_SGL_ADDR      (1 << 2)

/*!
 * Access from the highest entry to the lowest entry.
 */
#define BCMBD_ROF_INV_ADDR      (1 << 3)

/*! \} */

/*! \cond */
/*
 * Internal flag to indicates a DMA read operation (default is
 * write). This flag is normally set by read/write wrapper functions
 * to make the code more readable.
 */
#define BCMBD_ROF_READ          (1 << 31)
/*! \endcond */

/*!
 * \brief Get bypass state for register or memory address.
 *
 * The function will call a device-specific implementation which
 * determines if a particular register/memory address should be
 * bypassed.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Upper 32 bits of normalized register/memory address.
 * \param [in] addr Lower 32 bits of normalized register/memory address.
 *
 * \retval true This address should be bypassed.
 * \retval false This address should not be bypassed.
 */
typedef bool (*bcmbd_addr_bypass_f)(int unit, uint32_t adext, uint32_t addr);

/*!
 * \brief Dynamic address information for physical table.
 *
 * The dynamic address information is passed along the register/memory
 * read/write functions to carry table index, port number, etc.
 */
typedef struct bcmbd_pt_dyn_info_s {

    /*!
     * The index is the primary index if the table has more than a
     * single entry, i.e. memory-based tables and register arrays.
     */
    int index;

    /*!
     * The table instance is used if more than one copy of a table
     * exists, for example a copy per-port, per-block or per-pipeline.
     * Use -1 to write all instances of this table.
     *
     * For port-based registers this value is the port number as used
     * in the current block type, i.e. physical port number for port
     * blocks, MMU port number for MMU blocks and logical port number
     * for other switching blocks.
     */
    int tbl_inst;

} bcmbd_pt_dyn_info_t;

/*!
 * \brief Read device table entry.
 *
 * Common entry point for reading a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [out] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry read successfully.
 */
typedef int (*bcmbd_pt_read_f)(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                               uint32_t *data, size_t wsize);

/*!
 * \brief Write device table entry.
 *
 * Common entry point for writing a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry written successfully.
 */
typedef int (*bcmbd_pt_write_f)(int unit, bcmdrd_sid_t sid,
                                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                                uint32_t *data);

/*!
 * \brief Access a range of table entries via DMA.
 *
 * Common entry point for accessing a range of physical table entries.
 *
 * The provided data buffer must be a physically consecutive block of
 * memory suitable for DMA.
 *
 * For write operations, the caller is responsible for flushing the
 * memory cache before the DMA operation is started.
 *
 * For read operations, the caller is responsible for invalidating the
 * memory cache upon completion of the DMA operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address
 *                      components, typically the table index to read
 *                      from.
 * \param [in] ovrr_info Optional device-sepcific override structure,
 *                       which can be used to override otherwise fixed
 *                       address components, such as the access type.
 * \param [in] num_entries Number of (consecutive) table entries to
 *                         read.
 * \param [in] dma_addr Physical bus address of data buffer to read
 *                      to.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 *
 * \retval SHR_E_NONE Table entries read successfully.
 */
typedef int (*bcmbd_pt_range_op_f)(int unit, bcmdrd_sid_t sid,
                                   bcmbd_pt_dyn_info_t *dyn_info,
                                   void *ovrr_info, size_t num_entries,
                                   uint64_t dma_addr, size_t flags);

/*!
 * \brief Dynamic information for hashed physical table.
 *
 * The dynamic information is passed along the table insert/delete/loopkup
 * functions to carry bank information, etc.
 */
typedef struct bcmbd_pt_dyn_hash_info_s {

    /*!
     * The table instance is used if more than one copy of a table
     * exists, for example a copy per-port, per-block or per-pipeline.
     * Use -1 to update/search all instances of this table.
     */
    int tbl_inst;

    /*! Bank information specified in bitmap. Use 0 for all banks. */
    uint32_t bank;

} bcmbd_pt_dyn_hash_info_t;

/*!
 * \brief Device-specific response information for hash table operations.
 */
typedef struct bcmbd_tbl_resp_info_s {

    /*! Response word for CMIC table operation. */
    uint32_t resp_word;

} bcmbd_tbl_resp_info_t;

/*!
 * \brief Insert, delete or lookup device hash table entry.
 *
 * Common entry point for performing a physical hash table operation.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer where to use for key of hash table operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert to replace existing
 *              entry, or deleted entry data on table delete, or matched
 *              data entry on table lookup if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully on
 *                    insert operation, or table entry deleted successfully on
 *                    delete operation, or table entry found successfully on
 *                    lookup operation.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
typedef int (*bcmbd_pt_tblop_f)(int unit, bcmdrd_sid_t sid,
                                bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                                void *ovrr_hash_info,
                                uint32_t *data, size_t wsize,
                                bcmbd_tbl_resp_info_t *resp_info,
                                uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Common entry point for popping an entry from a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry popped successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
typedef int (*bcmbd_pt_pop_f)(int unit, bcmdrd_sid_t sid,
                              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                              uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Common entry point for pushing an entry to a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to push to the table entry.
 *
 * \retval SHR_E_NONE Table entry pushed successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
typedef int (*bcmbd_pt_push_f)(int unit, bcmdrd_sid_t sid,
                               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                               uint32_t *data);

/*!
 * \brief Decode a raw HMI address to dynamic address information of PT.
 *
 * This function will decode the specified raw HMI address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c sym_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decoded dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
typedef int (*bcmbd_pt_addr_decode_f)(int unit,
                                      uint32_t adext, uint32_t addr,
                                      uint32_t sym_flags,
                                      bcmdrd_sid_t *sid,
                                      bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief PT commands used by \ref bcmbd_pt_cmd_hdr_get.
 */
typedef enum bcmbd_pt_cmd_e {
    BCMBD_PT_CMD_NONE = 0,
    BCMBD_PT_CMD_READ,
    BCMBD_PT_CMD_WRITE,
    BCMBD_PT_CMD_POP,
    BCMBD_PT_CMD_PUSH,
    BCMBD_PT_CMD_COUNT
} bcmbd_pt_cmd_t;

/*!
 * \brief Get command header for PT operation.
 *
 * Common entry point for creating a command header for a physical
 * table (PT) operation (read, write, etc.).
 *
 * The command header is typically needed by bulk-operation interfaces
 * (DMA, FIFO) in order to execute multiple PT operations.
 *
 * The table entry may be a memory entry or a register.
 *
 * The data portion of write commands is not part of the output. Only
 * the header part is generated.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to access.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] cmd PT command for which to generate command header.
 * \param [in] wsize Size of \c data buffer in units of 32-bit words.
 * \param [out] data Buffer where to return the command header.
 * \param [out] hdr_wsize Actual size of command header in units of 32-bit words.
 *
 * \retval SHR_E_NONE Command header successfully created.
 * \retval SHR_E_UNAVAIL Unsupported command.
 */
typedef int (*bcmbd_pt_cmd_hdr_get_f)(int unit, bcmdrd_sid_t sid,
                                      bcmbd_pt_dyn_info_t *dyn_info,
                                      void *ovrr_info, bcmbd_pt_cmd_t cmd,
                                      uint32_t wsize, uint32_t *data,
                                      uint32_t *hdr_wsize);

/*!
 * \brief PT parameters supported by \ref bcmbd_pt_param_get.
 *
 * Typically a given device type will only support a subset of these
 * parameters.
 */
typedef enum bcmbd_pt_param_e {

    /*!
     * MOR stands for Multiple Outstanding Reqests, which is a
     * performance optimization technique for the CMIC S-bus.
     *
     * S-bus operations can be optimized by allowing a new request
     * onto the S-bus ring before the previous request is
     * complete. The MOR clocks value is the minimum number of clocks
     * required between such two S-bus requests.
     *
     * The MOR clocks value will be different for each request
     * depending on the S-bus slave and the register/memory being
     * accessed.
     *
     * This parameter can be used to obtain the MOR clocks value for a
     * read operation on a specific register/memory. If the
     * register/memory does not support MOR, a value of zero will be
     * returned.
     */
    BCMBD_PT_PARAM_MOR_CLKS_READ,

    /*!
     * This parameter can be used to obtain the MOR clocks value for a
     * write operation on a specific register/memory. If the
     * register/memory does not support MOR, a value of zero will be
     * returned.
     *
     * Please see \ref BCMBD_PT_PARAM_MOR_CLKS_READ for MOR details.
     */
    BCMBD_PT_PARAM_MOR_CLKS_WRITE,

    /* Must be last */
    BCMBD_PT_PARAM_COUNT

} bcmbd_pt_param_t;

/*!
 * \brief Get access parameter for a physical table.
 *
 * For optimized hardware access, special PT-specific settings can be
 * applied to each individual access. This function can be used to
 * obtain such parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] ptype Parameter type.
 * \param [out] pval Parameter value.
 *
 * \retval SHR_E_NONE Parameter successfully obtained.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_UNAVAIL Parameter type not supported.
 */
typedef int (*bcmbd_pt_param_get_f)(int unit, bcmdrd_sid_t sid,
                                    bcmbd_pt_param_t ptype, uint32_t *pval);

/*!
 * \brief Get OTP parameter value.
 *
 * \param [in] unit Unit number.
 * \param [in] otp_param Type of parameter to get.
 * \param [out] val OTP parameter value.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNAVAIL OTP parameter not supported for this device.
 */
typedef int (*bcmbd_otp_param_get_f)(int unit, bcmbd_otp_param_t otp_param,
                                     uint32_t *val);

/*!
 * \brief Perform device reset sequence.
 *
 * The device reset sequence will do the minimum amount of work to
 * enable all registers and memories for host CPU access.
 *
 * For example, an XGS device reset sequence will typically pull all
 * hardware blocks out of reset, configure the S-bus rings, and then
 * enable all clocks.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Device reset sequence successfully completed.
 * \retval SHR_E_FAIL Device reset sequence failed.
 */
typedef int (*bcmbd_dev_reset_f)(int unit);

/*!
 * \brief Perform device initialization sequence.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Device initialization sequence successfully completed.
 * \retval SHR_E_FAIL Device initialization sequence failed.
 */
typedef int (*bcmbd_dev_init_f)(int unit);

/*!
 * \brief Start HMI drivers and other device-specific drivers.
 *
 * Note that this function will also be called during warm boot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE All drivers started successfully.
 * \retval SHR_E_FAIL One or more drivers failed to start.
 */
typedef int (*bcmbd_dev_start_f)(int unit);

/*!
 * \brief Stop HMI drivers and other device-specific drivers.
 *
 * Stop drivers and free all resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE All drivers stopped successfully.
 * \retval SHR_E_FAIL One or more drivers failed to stop.
 */
typedef int (*bcmbd_dev_stop_f)(int unit);

/*!
 * \brief Enable or disable KNET mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
typedef int (*bcmbd_knet_enable_set_f)(int unit, bool enable);

/*!
 * \brief Get KNET mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
typedef int (*bcmbd_knet_enable_get_f)(int unit, bool *enabled);

/*! Base driver structure. */
typedef struct bcmbd_drv_s {

    /*! Device type. */
    bcmdrd_dev_type_t dev_type;

    /*! Address calculation function. */
    bcmdrd_block_port_addr_f block_port_addr;

    /*! Bypass check function. */
    bcmbd_addr_bypass_f addr_bypass;

    /*! Physical table (register/memory) read function. */
    bcmbd_pt_read_f pt_read;

    /*! Physical table (register/memory) write function. */
    bcmbd_pt_write_f pt_write;

    /*! Physical table (register/memory) range operation function. */
    bcmbd_pt_range_op_f pt_range_op;

    /*! Physical table (memory hash table) insert function. */
    bcmbd_pt_tblop_f pt_insert;

    /*! Physical table (memory hash table) delete function. */
    bcmbd_pt_tblop_f pt_delete;

    /*! Physical table (memory hash table) lookup function. */
    bcmbd_pt_tblop_f pt_lookup;

    /*! Physical table fifo pop function. */
    bcmbd_pt_pop_f pt_pop;

    /*! Physical table push function. */
    bcmbd_pt_push_f pt_push;

    /*! Physical table address decode function. */
    bcmbd_pt_addr_decode_f pt_addr_decode;

    /*! Get command header for a PT operation. */
    bcmbd_pt_cmd_hdr_get_f pt_cmd_hdr_get;

    /*! Get access parameter for a PT table. */
    bcmbd_pt_param_get_f pt_param_get;

    /*! Get OTP information. */
    bcmbd_otp_param_get_f otp_param_get;

    /*! Perform device reset sequence. */
    bcmbd_dev_reset_f dev_reset;

    /*! Perform device initialization sequence. */
    bcmbd_dev_init_f dev_init;

    /*! Start HMI drivers. */
    bcmbd_dev_start_f dev_start;

    /*! Stop HMI drivers. */
    bcmbd_dev_stop_f dev_stop;

    /*! Enable/disable KNET mode. */
    bcmbd_knet_enable_set_f knet_enable_set;

    /*! Get KNET enable status. */
    bcmbd_knet_enable_get_f knet_enable_get;

} bcmbd_drv_t;

/*!
 * \brief Application call-back for device reset sequence.
 *
 * A function of this type may optionally be invoked immediately
 * before the device reset sequence.
 *
 * Typically such a function will initialize various board-specific
 * components like voltage regulators.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Callback encountered errors.
 */
typedef int (*bcmbd_dev_reset_cb_f)(int unit);

/*!
 * \brief Install device reset callback function.
 *
 * Please refer to \ref bcmbd_dev_reset_cb_f for details.
 *
 * Only a single function can be installed.
 *
 * Set \c reset_cb to NULL to clear the callback.
 *
 * \param [in] unit Unit number.
 * \param [in] reset_cb Callback function to be installed.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_dev_reset_cb_set(int unit, bcmbd_dev_reset_cb_f reset_cb);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_bd_attach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmbd_attach(int unit);

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern int _bc##_bd_detach(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*! \cond  Externs for the required functions. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_bd_var_attach(int unit);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated by \ref bcmbd_attach.
 *
 * \param [in] unit Unit number.
 */
extern int
bcmbd_detach(int unit);

/*!
 * \brief Check if device driver is attached.
 *
 * Check if \ref bcmbd_attach has been called for this device.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device driver already attached.
 * \retval false Device driver not attached.
 */
extern bool
bcmbd_attached(int unit);

/*!
 * \brief Get pointer to base driver device structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to device structure, or NULL if not found.
 */
extern bcmbd_drv_t *
bcmbd_drv_get(int unit);

/*!
 * \brief Calculate regsiter/memory address.
 *
 * This function calculates the register/memory address based on a
 * fixed offset and a number of dynamic parameters. The address
 * calculation is typically specific to host management interface
 * (HMI), but some devices require special considerations beyond that.
 *
 * \param [in] unit Unit number.
 * \param [in] block Hardware block number.
 * \param [in] lane Lane/port within hardware block.
 * \param [in] offset Base offset of address.
 * \param [in] idx Array index (use -1 if not array).
 *
 * \return Regsiter/memory address.
 */
extern uint32_t
bcmbd_block_port_addr(int unit, int block, int lane, uint32_t offset, int idx);

/*!
 * \brief Get bypass state for register or memory address.
 *
 * The function will call a device-specific implementation which
 * determines if a particular register/memory address should be
 * bypassed.
 *
 * \param [in] unit Unit number.
 * \param [in] adext Upper 32 bits of normalized register/memory address.
 * \param [in] addr Lower 32 bits of normalized register/memory address.
 *
 * \retval true This address should be bypassed.
 * \retval false This address should not be bypassed.
 */
extern bool
bcmbd_addr_bypass(int unit, uint32_t adext, uint32_t addr);

/*!
 * \brief Read device table entry.
 *
 * Common entry point for reading a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry read successfully.
 */
extern int
bcmbd_pt_read(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data, size_t wsize);

/*!
 * \brief Write device table entry.
 *
 * Common entry point for writing a physical table entry.
 *
 * The table entry may be a memory entry or a register.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry written successfully.
 */
extern int
bcmbd_pt_write(int unit, bcmdrd_sid_t sid,
               bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
               uint32_t *data);

/*!
 * \brief Read range of device table entries.
 *
 * Common entry point for reading a range of physical table entries.
 *
 * The provided data buffer must be a physically consecutive block of
 * memory suitable for DMA.
 *
 * The caller is responsible for invalidating the memory cache upon
 * completion of the DMA operation.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address
 *                      components, typically the first table index to
 *                      read from.
 * \param [in] ovrr_info Optional device-sepcific override structure,
 *                       which can be used to override otherwise fixed
 *                       address components, such as the access type.
 * \param [in] num_entries Number of (consecutive) table entries to
 *                         read.
 * \param [in] dma_addr Physical bus address of data buffer to read
 *                      to.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 *
 * \retval SHR_E_NONE Table entries read successfully.
 */
extern int
bcmbd_pt_range_read(int unit, bcmdrd_sid_t sid,
                    bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                    size_t num_entries, uint64_t dma_addr, uint32_t flags);

/*!
 * \brief Write range of device table entries.
 *
 * Common entry point for writing a range of physical table entries.
 *
 * The provided data buffer must be a physically consecutive block of
 * memory suitable for DMA.
 *
 * The caller is responsible for flushing the memory cache before
 * the DMA operation is started.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address
 *                      components, typically the first table index to
 *                      write to.
 * \param [in] ovrr_info Optional device-sepcific override structure,
 *                       which can be used to override otherwise fixed
 *                       address components, such as the access type.
 * \param [in] num_entries Number of (consecutive) table entries to
 *                         write.
 * \param [in] dma_addr Physical bus address of data buffer to write
 *                      from.
 * \param [in] flags Please refer to \ref BCMBD_ROF_xxx.
 *
 * \retval SHR_E_NONE Table entries written successfully.
 */
extern int
bcmbd_pt_range_write(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                     size_t num_entries, uint64_t dma_addr, uint32_t flags);

/*!
 * \brief Insert device table entry.
 *
 * Common entry point for insert a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer to insert to table.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Old entry data on table insert if not NULL.
 *
 * \retval SHR_E_NONE New table entry inserted or replaced successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_insert(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);


/*!
 * \brief Delete device table entry.
 *
 * Common entry point for delete a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer contains table key for delete operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Deleted entry data if not NULL.
 *
 * \retval SHR_E_NONE table entry deleted successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_delete(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup device table entry.
 *
 * Common entry point for lookup a physical table entry.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_hash_info Device-specific structure of hash dynamic
 *             components, typically the bank information.
 * \param [in] ovrr_hash_info Optional device-sepcific hash override structure,
 *             which can be used to override otherwise fixed address components,
 *             such as bank ignore mask in address offset.
 * \param [in] data Buffer contains table key for table lookup operation.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 * \param [out] resp_info Table operation response information if not NULL.
 * \param [out] resp_data Matched data entry if not NULL.
 *
 * \retval SHR_E_NONE table entry found successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_INTERNAL Other failures.
 */
extern int
bcmbd_pt_lookup(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_hash_info_t *dyn_hash_info, void *ovrr_hash_info,
                uint32_t *data, size_t wsize,
                bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Common entry point for popping an entry from a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to read from.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Buffer where to return the table entry contents.
 * \param [in] wsize Size of buffer in units of 32-bit words.
 *
 * \retval SHR_E_NONE Table entry popped successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcmbd_pt_pop(int unit, bcmdrd_sid_t sid,
             bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
             uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Common entry point for pushing an entry to a FIFO-enabled physical table.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to write.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] data Data buffer to write to the table entry.
 *
 * \retval SHR_E_NONE Table entry pushed successfully.
 * \retval SHR_E_FAIL NACK bit is inserted in S-channel response.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_PARAM Invalid parameters.
 */
extern int
bcmbd_pt_push(int unit, bcmdrd_sid_t sid,
              bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
              uint32_t *data);

/*!
 * \brief Decode a raw HMI address to dynamic address information of PT.
 *
 * This function will decode the specified raw HMI address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c sym_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMIC-specific SBUS address extension.
 * \param [in] addr CMIC-specific address beat.
 * \param [in] sym_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decode dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
extern int
bcmbd_pt_addr_decode(int unit,
                     uint32_t adext, uint32_t addr, uint32_t sym_flags,
                     bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief Get command header for PT operation.
 *
 * Common entry point for creating a command header for a physical
 * table (PT) operation (read, write, etc.).
 *
 * The command header is typically needed by bulk-operation interfaces
 * (DMA, FIFO) in order to execute multiple PT operations.
 *
 * The table entry may be a memory entry or a register.
 *
 * The data portion of write commands is not part of the output. Only
 * the header part is generated.
 *
 * The function implementation relies on device-specific driver
 * functions for interpreting the address components.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] dyn_info Device-specific structure of dynamic address components,
 *             typically the table index to access.
 * \param [in] ovrr_info Optional device-sepcific override structure, which can
 *             be used to override otherwise fixed address components, such as
 *             the access type.
 * \param [in] cmd PT command for which to generate command header.
 * \param [in] wsize Size of \c data buffer in units of 32-bit words.
 * \param [out] data Buffer where to return the command header.
 * \param [out] hdr_wsize Actual size of command header in units of 32-bit words.
 *
 * \retval SHR_E_NONE Command header successfully created.
 * \retval SHR_E_UNAVAIL Unsupported command.
 */
extern int
bcmbd_pt_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                     bcmbd_pt_dyn_info_t *dyn_info,
                     void *ovrr_info, bcmbd_pt_cmd_t cmd,
                     uint32_t wsize, uint32_t *data,
                     uint32_t *hdr_wsize);

/*!
 * \brief Get access parameter for a physical table.
 *
 * For optimized hardware access, special PT-specific settings can be
 * applied to each individual access. This function can be used to
 * obtain such parameters.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 * \param [in] ptype Parameter type.
 * \param [out] pval Parameter value.
 *
 * \retval SHR_E_NONE Parameter successfully obtained.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_UNAVAIL Parameter type not supported.
 */
extern int
bcmbd_pt_param_get(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_param_t ptype, uint32_t *pval);

/*!
 * \brief Get MOR clocks for read operation.
 *
 * If MOR is not applicable, then a value of zero is returned.
 *
 * Please see \ref BCMBD_PT_PARAM_MOR_CLKS_READ for MOR details.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 *
 * \return Number of MOR clocks.
 */
extern uint32_t
bcmbd_mor_clks_read_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get MOR clocks for write operation.
 *
 * If MOR is not applicable, then a value of zero is returned.
 *
 * Please see \ref BCMBD_PT_PARAM_MOR_CLKS_READ for MOR details.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID for physical table.
 *
 * \return Number of MOR clocks.
 */
extern uint32_t
bcmbd_mor_clks_write_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Perform device reset sequence.
 *
 * The device reset sequence will do the minimum amount of work to
 * enable all registers and memories for host CPU access.
 *
 * For example, an XGS device reset sequence will typically pull all
 * hardware blocks out of reset, configure the S-bus rings, and then
 * enable all clocks.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device reset sequence successfully completed.
 * \retval SHR_E_FAIL Device reset sequence failed.
 */
extern int
bcmbd_dev_reset(int unit);

/*!
 * \brief Perform device initialization sequence.
 *
 * \param [in] unit Unit number
 *
 * \retval SHR_E_NONE Device initialization sequence successfully completed.
 * \retval SHR_E_FAIL Device initialization sequence failed.
 */
extern int
bcmbd_dev_init(int unit);

/*!
 * \brief Start HMI drivers and other device-specific drivers.
 *
 * Note that this function will also be called during warm boot.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE All drivers started successfully.
 * \retval SHR_E_FAIL One or more drivers failed to start.
 */
extern int
bcmbd_dev_start(int unit);

/*!
 * \brief Stop HMI drivers and other device-specific drivers.
 *
 * Stop drivers and free all resources.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE All drivers stopped successfully.
 * \retval SHR_E_FAIL One or more drivers failed to stop.
 */
extern int
bcmbd_dev_stop(int unit);

/*!
 * \brief Check if HMI drivers were started.
 *
 * Main purpose is to avoid detaching a running device.
 *
 * \param [in] unit Unit number.
 *
 * \retval true Device is running.
 * \retval false Device is stopped.
 */
bool
bcmbd_dev_running(int unit);

/*!
 * \brief Enable or disable KNET mode.
 *
 * Low-level packet I/O (interrupts and DMA) can be handled either in
 * user space or kernel space (if applicable to the operating system).
 *
 * This function will configure interrupt handling according to the
 * selected mode of operation (KNET or non-KNET).
 *
 * \param [in] unit Unit number.
 * \param [in] enable Set to true for KNET mode.
 *
 * \retval SHR_E_NONE Operating mode successfully updated.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_knet_enable_set(int unit, bool enable);

/*!
 * \brief Get KNET mode enable status.
 *
 * Retrieve the mode that was currently selected using \ref
 * bcmbd_knet_enable_set.
 *
 * \param [in] unit Unit number.
 * \param [out] enabled Set to true if KNET mode is enabled.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Invalid unit number or not supported.
 */
extern int
bcmbd_knet_enable_get(int unit, bool *enabled);

#endif /* BCMBD_H */
