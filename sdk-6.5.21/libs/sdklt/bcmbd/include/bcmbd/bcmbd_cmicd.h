/*! \file bcmbd_cmicd.h
 *
 * Definitions for CMICd v2.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICD_H
#define BCMBD_CMICD_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>

/*! Size of memory-mapped I/O window for CMICd. */
#define BCMBD_CMICD_IOMEM_SIZE          0x40000

/*!
 * \name S-bus protocol v4 address encoding.
 * \anchor bcmbd_cmicd_sbus
 *
 * The SOC register/memory address information for S-bus protocol v4
 * is carried in two 32-bit words, addr (address) and adext (address
 * extension).
 *
 *           31                                               0
 *          +--------------------------------------------------+
 *   addr   |                      Offset                      |
 *          +--------------------------------------------------+
 *
 *           31                    16 15         8 7          0
 *          +------------------------+------------+------------+
 *   adext  |        (unused)        |  AccType   |    Block   |
 *          +------------------------+------------+------------+
 *
 *
 * In the symbol table, the device-specific field 'info' is encoded as
 * follows:
 *
 *           31      24 23     22 21     16 15                0
 *          +----------+---------+---------+-------------------+
 *   info   | (unused) | SubPipe | AccType |   BlockTypes      |
 *          +----------+---------+---------+-------------------+
 *
 * Note that a (port) symbol can belong to more than one block type
 * within a single device.
 *
 * \{
 */

/*! Extract block number from address extension. */
#define BCMBD_CMICD_ADEXT2BLOCK(_adext) \
        ((_adext) & 0x7f)

/*! Extract access type from address extension. */
#define BCMBD_CMICD_ADEXT2ACCTYPE(_adext) \
        ((((_adext) >> 8) & 0x20) ? -1 : (int)(((_adext) >> 8) & 0x1f))

/*! Modify block in existing address extension. */
#define BCMBD_CMICD_ADEXT_BLOCK_SET(_adext, _block) \
        (_adext = ((_adext) & ~0xff) | (_block))

/*! Modify access type in existing address extension */
#define BCMBD_CMICD_ADEXT_ACCTYPE_SET(_adext, _acctype) \
        if ((_acctype) < 0) { \
            (_adext = ((_adext) & ~0x3f00) | (0x20 << 8)); \
        } else { \
            (_adext = ((_adext) & ~0x3f00) | ((_acctype) << 8)); \
        }

/*! Extract access type from block/access information in symbol table. */
#define BCMBD_CMICD_BLKACC2ACCTYPE(_blkacc) \
        (((_blkacc) >> 16) & 0x1f)

/*! Convert block/access information in symbol table to address extension. */
#define BCMBD_CMICD_BLKACC2ADEXT(_blkacc) \
        (((_blkacc) >> 8) & 0x1f00)

/*! \} */



/*! Default CMC to use for CMICd-based devices. */
#define BCMBD_CMICD_CMC 0

/*
 * CMIC CMC selection for PCI interface.
 *
 * Use dynamic CMC selection by default, but allow override by
 * hard-coded CMC selection.
 */
#if defined(BCMBD_CMICD_CMC)
/*! Get CMC to use for host CPU access. */
#define BCMBD_CMICD_CMC_GET(_u)    BCMBD_CMICD_CMC
/*! Set CMC to use for host CPU access. */
#define BCMBD_CMICD_CMC_SET(_u,_c)
#else
/*! Get CMC to use for host CPU access. */
#define BCMBD_CMICD_CMC_GET(_u)    BCMBD_CMICD_CMIC_CMC(_u)
/*! Set CMC to use for host CPU access. */
#define BCMBD_CMICD_CMC_SET(_u,_c) BCMBD_CMICD_CMIC_CMC(_u) = _c;
#endif

/*! CMC base address offset. */
#define BCMBD_CMICD_CMC_OFFSET(_u) (BCMBD_CMICD_CMC_GET(_u) * 0x1000)

/*! Read CMC-based CMIC register. */
#define BCMBD_CMICD_CMC_READ(_u, _addr, _val) \
    BCMDRD_DEV_READ32(_u, (_addr) + BCMBD_CMICD_CMC_OFFSET(_u), _val)
/*! Write CMC-based CMIC register. */
#define BCMBD_CMICD_CMC_WRITE(_u, _addr, _val) \
    BCMDRD_DEV_WRITE32(_u, (_addr) + BCMBD_CMICD_CMC_OFFSET(_u), _val)

/*!
 * Extract register base type from SOC register base address. The base
 * type is used for certain multi-pipe XGS designs.
 */
#define BCMBD_CMICD_ADDR2BASETYPE(_addr) (((_addr) >> 23) & 0x7)

/*! Maximum number of per-pipe register/memory instances. */
#define BCMBD_CMICD_PHYS_INST_MAX  8

/*! Extract number of logical instances from pipe information word. */
#define BCMBD_CMICD_PIPE_LINST(_pi) (((_pi) >> 8) & 0xff)

/*! Extract bit map of valid instances from pipe information word. */
#define BCMBD_CMICD_PIPE_PMASK(_pi) (((_pi) >> 0) & 0xff)

/*! Extract section size encoding from pipe information word. */
#define BCMBD_CMICD_PIPE_SECT(_pi) (((_pi) >> 16) & 0xff)

/*! Extract section size from pipe information word. */
#define BCMBD_CMICD_PIPE_SECT_SIZE(_pi) \
    ((BCMBD_CMICD_PIPE_SECT(_pi) == 0) ? \
     0 : (1 << (BCMBD_CMICD_PIPE_SECT(_pi))))

/*! Extract duplicate access type from pipe information word. */
#define BCMBD_CMICD_PIPE_AT_DUPLICATE(_pi) (((_pi) >> 24) & 0x1f)

/*! Encode pipe information word from necessary information. */
#define BCMBD_CMICD_PIPE_INFO_SET(_pmask, _linst, _sect_shft, _dup_acc) \
    (((_pmask) & 0xff) | (((_linst) & 0xff) << 8) | \
     (((_sect_shft) & 0xff) << 16) | (((_dup_acc) & 0x1f) << 24))

/*!
 * \brief Enable symbolic address decoding in debug messages.
 *
 * If this function is not called, register and memory addresses in
 * verbose/debug messages will be shown in raw hex format.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicd_sym_addr_enable(int unit);

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_feature_init(int unit);

/*!
 * \brief Initialize CMICd base driver for a device.
 *
 * This function will initialize default function vectors for
 * register/memory access, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_drv_init(int unit);

/*!
 * \brief Clean up CMICd base driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicd_drv_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_drv_cleanup(int unit);

/*!
 * \brief Initialize CMICd S-channel driver for a device.
 *
 * This function will allocated synchronization objects, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_INTERNAL Already initialized.
 * \retval SHR_E_RESOURCE Failed to allocate syncronization objects.
 */
extern int
bcmbd_cmicd_schan_init(int unit);

/*!
 * \brief Clean up CMICd S-channel driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicd_schan_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicd_schan_cleanup(int unit);

/*!
 * \brief Perform CMICd S-channel raw operation.
 *
 * This function will return the maximum output buffer size required in
 * \c dwc_max if \c buf is set to NULL. The caller then can reserve a buffer
 * of the size that is previously returned via \c dwc_max. The \c buf carries
 * the data to write to S-channel and the data read from S-channel will also
 * be copied to the \c buf. The size of \c dwc_write and \c dwc_read must not
 * exceed the returned \c dwc_max.
 *
 * \param [in] unit Unit number.
 * \param [in, out] buf The raw data for S-channel operation.
 * \param [in] dwc_write Number of 32-bit words to write to S-channel.
 * \param [in] dwc_read Number of 32-bit words to read from S-channel.
 * \param [out] dwc_max Maximum output buffer size (in 32-bit words)
 *                      when \c buf is NULL.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_schan_raw_op(int unit, uint32_t *buf,
                         int dwc_write, int dwc_read, int *dwc_max);

/*!
 * \brief Initialize CMICd SBUSDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_sbusdma_init(int unit);

/*!
 * \brief Clean up CMICd SBUSDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicd_sbusdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicd_sbusdma_cleanup(int unit);

/*!
 * \brief Initialize CMICd FIFODMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_fifodma_init(int unit);

/*!
 * \brief Clean up CMICd FIFODMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicd_fifodma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicd_fifodma_cleanup(int unit);

/*!
 * \brief Initialize CMICd CCMDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_ccmdma_init(int unit);

/*!
 * \brief Clean up CMICd CCMDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicd_ccmdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicd_ccmdma_cleanup(int unit);

/*!
 * \brief Initialize basic CMICd settings.
 *
 * Ensure that endian settings are correct and that all interrupts are
 * disabled.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicd_init(int unit);

/*!
 * \brief Read from SOC register in a specific block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_block_read(int unit, uint32_t blkacc, int blknum,
                           uint32_t offset, int idx,
                           uint32_t *data, size_t wsize);

/*!
 * \brief Write to SOC register in a specific block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_block_write(int unit, uint32_t blkacc, int blknum,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Read from block-based SOC register.
 *
 * If port is specified as -1, the function will read from the first
 * block of the specified block type(s).
 *
 * If port is a valid port number, the function will read from the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Write to block-based SOC register.
 *
 * If port is specified as -1, the function will write to all blocks
 * of the specified block type(s).
 *
 * If port is a valid port number, the function will write to the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize);

/*!
 * \brief Read from port-based SOC register.
 *
 * The function will read from the port block which contains the
 * specified port.
 *
 * If port is an invalid port number, the result is undefined.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_port_read(int unit, uint32_t blkacc, int port,
                          uint32_t offset, int idx, uint32_t *data, int wsize);

/*!
 * \brief Write to port-based SOC register.
 *
 * The function will write to the port block which contains the
 * specified port.
 *
 * If port is an invalid port number, the result is undefined.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_port_write(int unit, uint32_t blkacc, int port,
                           uint32_t offset, int idx, uint32_t *data, int wsize);

/*!
 * \brief Override information for CMICd-based (v2) device.
 *
 * This structure defines the \c ovrr_info used in \ref bcmbd_cmicd_read
 * and \ref bcmbd_cmicd_write.
 *
 * For symbols with 'duplicate' attribute (multiple instances but accessed
 * in a single view), per-instance access is allowed by assigning the
 * \c ovrr_info for some particular reason, e.g., for SER purpose.
 */
typedef struct bcmbd_cmicd_ovrr_info_s {

    /*! Instance index to be overridden. */
    int inst;

} bcmbd_cmicd_ovrr_info_t;

/*!
 * \brief Read physical table entry.
 *
 * Read a register/memory entry on a CMICd-based (v2) device.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize);

/*!
 * \brief Write physical table entry.
 *
 * Write a register/memory entry on a CMICd-based (v2) device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_info Dynamic access information (table index, etc.).
 * \param [in] ovrr_info Optional override control (for debug).
 * \param [in] data Data entry to write.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data);

/*!
 * \brief Insert physical table entry.
 *
 * Insert a memory entry on a CMICd-based (v2) device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
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
bcmbd_cmicd_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Delete physical table entry.
 *
 * Delete a memory entry on a CMICd-based (v2) device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to delete from table.
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
bcmbd_cmicd_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup physical table entry.
 *
 * Lookup a memory entry on a CMICd-based (v2) device.
 *
 * The size of the data entry is derived from the symbol information.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID.
 * \param [in] dyn_hash_info Dynamic access information (bank, etc.).
 * \param [in] ovrr_hash_info Optional override control (for debug).
 * \param [in] data Buffer contains key to lookup in table.
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
bcmbd_cmicd_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Pop an entry from a FIFO-enabled physical table on a
 * CMICd-based (v2) device.
 *
 * The table entry should be able to perform FIFO pop operations,
 * otherwise SHR_E_PARAM will be returned.
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
bcmbd_cmicd_pop(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Push an entry to a FIFO-enabled physical table on a
 * CMICd-based (v2) device.
 *
 * The table entry should be able to perform FIFO push operations,
 * otherwise SHR_E_PARAM will be returned.
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
bcmbd_cmicd_push(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data);

/*!
 * \brief Decode a raw CMICd address to dynamic address information of PT.
 *
 * This function will decode the specified raw CMICd address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c drd_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMICd SBUS address extension.
 * \param [in] addr CMICd address beat.
 * \param [in] drd_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decoded dynamic address information.
 *
 * \retval SHR_E_NONE Address successfuly decoded.
 * \retval SHR_E_FAIL Error decoding address.
 */
extern int
bcmbd_cmicd_addr_decode(int unit,
                        uint32_t adext, uint32_t addr, uint32_t drd_flags,
                        bcmdrd_sid_t *sid, bcmbd_pt_dyn_info_t *dyn_info);

/*!
 * \brief Get command header for PT operation.
 *
 * See \ref bcmbd_pt_cmd_hdr_get for details.
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
bcmbd_cmicd_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
                        bcmbd_pt_dyn_info_t *dyn_info,
                        void *ovrr_info, bcmbd_pt_cmd_t cmd,
                        uint32_t wsize, uint32_t *data,
                        uint32_t *hdr_wsize);

/*!
 * \brief Get pipe information for multi-pipe device.
 *
 * It is assumed that the caller of this function is familiar with the
 * S-channel access mechanism for multi-pipe XGS designs, including
 * the concepts of access type and base type.
 *
 * This function will operate in two different modes depending on the
 * value of the \c baseidx parameter.
 *
 * If \c baseidx is -1, then information about access constraints are
 * returned as follows:
 *
 * - bits [7:0] Bit map of valid unique access type values across all
 *              base indexes.
 * - bits [15:8] Number of base type instances.
 * - bits [23:16] Section size as log2(n), e.g. 3=>8, 5=>32.
 * - bits [28:24] Global access type value for pipes with
 *                unique access type.
 *
 * If a valid \c baseidx is specified, then information about access
 * constraints are returned as follows:
 *
 * - bits [7:0] Bit map of valid unique access type values for the
 *              specified base index.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Base address (base type is extracted from this).
 * \param [in] acctype Access type (device-specific).
 * \param [in] blktype Block type.
 * \param [in] baseidx Base index (interpreted according to base type).
 *
 * \return Encoded pipe information.
 */
extern uint32_t
bcmbd_cmicd_pipe_info(int unit, uint32_t addr, int acctype, int blktype,
                      int baseidx);

/*!
 * \brief Read from unique port-based register.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, size_t wsize);

/*!
 * \brief Write to unique port-based register.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry (if register array).
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_reg_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, size_t wsize);

/*!
 * \brief Read from unique memory.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_mem_unique_block_read(int unit, uint32_t blkacc, int blknum,
                                  int blkidx, int baseidx,
                                  uint32_t offset, int idx,
                                  uint32_t *data, int wsize);

/*!
 * \brief Write to unique memory.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Block number.
 * \param [in] blkidx Unique block instance (within block \c blknum).
 * \param [in] baseidx Base type index (port, pipe, etc.).
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_mem_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, int wsize);

/*!
 * \brief Read from block-based SOC memory.
 *
 * If port is specified as -1, the function will read from the first
 * block of the specified block type(s).
 *
 * If port is a valid port number, the function will read from the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_mem_blocks_read(int unit, uint32_t blkacc, int port,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

/*!
 * \brief Write to block-based SOC memory.
 *
 * If port is specified as -1, the function will write to all blocks
 * of the specified block type(s).
 *
 * If port is a valid port number, the function will write to the
 * block which contains the specified port number.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] port Top-level port number for this block type.
 * \param [in] offset Base offset for this register.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid reponse from S-channel operation.
 */
extern int
bcmbd_cmicd_mem_blocks_write(int unit, uint32_t blkacc, int port,
                             uint32_t offset, int idx,
                             uint32_t *data, size_t wsize);

/*!
 * \brief Clear SOC memory partially or in full.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] offset Base offset for this memory.
 * \param [in] si Start index, i.e. first memory entry to clear.
 * \param [in] ei End index, i.e. last memory entry to clear.
 * \param [in] wsize Size of memory entries in 32-bit words.
 *
 * \retval 0 No errors
 */
extern int
bcmbd_cmicd_mem_blocks_clear(int unit, uint32_t blkacc, uint32_t offset,
                             uint32_t si, uint32_t ei, int wsize);

/*! Clear entire memory. */
#define BCMBD_CMICD_MEM_CLEAR(_u, _m) \
    bcmbd_cmicd_mem_blocks_clear(_u, _m##_BLKACC, _m, \
                                 _m##_MIN, _m##_MAX, (_m##_SIZE + 3) >> 2)

#endif /* BCMBD_CMICD_H */
