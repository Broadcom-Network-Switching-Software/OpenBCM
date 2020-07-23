/*! \file bcmbd_cmicx.h
 *
 * Definitions for CMICx.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMBD_CMICX_H
#define BCMBD_CMICX_H

#include <bcmdrd/bcmdrd_dev.h>
#include <bcmbd/bcmbd.h>

/*! Size of memory-mapped I/O window for CMICx. */
#define BCMBD_CMICX_IOMEM_SIZE          0x40000

/*! Number of 32-bit words in S-channel message buffer */
#define BCMBD_CMICX_SCHAN_WORDS         22

/*!
 * \name S-bus protocol v4 address encoding.
 * \anchor bcmbd_cmicx_sbus
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
 *   info   | MorClks  | SubPipe | AccType |   BlockTypes      |
 *          +----------+---------+---------+-------------------+
 *
 * Note that a (port) symbol can belong to more than one block type
 * within a single device.
 *
 * MorClks is the MOR (Multiple Outstanding Requests) clocks value
 * that should be used to optimize the CMIC S-bus DMA performance for
 * this symbol.
 *
 * \{
 */

/*! Extract block number from address extension. */
#define BCMBD_CMICX_ADEXT2BLOCK(_adext) \
        ((_adext) & 0x7f)

/*! Extract access type from address extension. */
#define BCMBD_CMICX_ADEXT2ACCTYPE(_adext) \
        ((((_adext) >> 8) & 0x20) ? -1 : (int)(((_adext) >> 8) & 0x1f))

/*! Modify block in existing address extension. */
#define BCMBD_CMICX_ADEXT_BLOCK_SET(_adext, _block) \
        (_adext = ((_adext) & ~0xff) | (_block))

/*! Modify access type in existing address extension */
#define BCMBD_CMICX_ADEXT_ACCTYPE_SET(_adext, _acctype) \
        if ((_acctype) < 0) { \
            (_adext = ((_adext) & ~0x3f00) | (0x20 << 8)); \
        } else { \
            (_adext = ((_adext) & ~0x3f00) | ((_acctype) << 8)); \
        }

/*! Extract access type from block/access information in symbol table. */
#define BCMBD_CMICX_BLKACC2ACCTYPE(_blkacc) \
        (((_blkacc) >> 16) & 0x1f)

/*! Extract MOR clocks value from block/access information in symbol table. */
#define BCMBD_CMICX_BLKACC2MORCLKS(_blkacc) \
        (((_blkacc) >> 24) & 0xff)

/*! Convert block/access information in symbol table to address extension. */
#define BCMBD_CMICX_BLKACC2ADEXT(_blkacc) \
        (((_blkacc) >> 8) & 0x1f00)

/*! \} */

/*! DMA slave offset for PCIe */
#define BCMBD_CMICX_PCIE_SLAVE_OFFSET (0x10000000)

/*! Read CMIC register. */
#define BCMBD_CMICX_READ(_u, _addr, _val) \
    BCMDRD_DEV_READ32(_u, _addr, _val)
/*! Write CMIC register. */
#define BCMBD_CMICX_WRITE(_u, _addr, _val) \
    BCMDRD_DEV_WRITE32(_u, _addr, _val)

/*! True if 64-bit CMIC access is supported. */
#define BCMBD_CMICX_64BIT_SUPPORT(_u)           \
    (bcmdrd_reg32_iomem[_u] != NULL && sizeof(long) == 8)

/*! True if 64-bit CMIC access may be used. */
extern bool bcmbd_cmicx_use_64bit;

/*! True if 64-bit CMIC access may be used. */
#define BCMBD_CMICX_USE_64BIT(_u)               \
    (bcmbd_cmicx_use_64bit)

/*!
 * \brief Encode two 32-bit words nito a 64-bit word.
 *
 * Used for 64-bit access to 32-bit CMIC registers.
 *
 * \param [out] _data64 64-bit destination.
 * \param [in] _buf32 Pointer to array of two 32-bit words.
 */
#define BCMBD_CMICX_DATA64_ENCODE(_data64, _buf32) do {   \
        uint32_t *_ptr32 = _buf32;                        \
        _data64 = _ptr32[1];                              \
        _data64 = (_data64 << 32) | _ptr32[0];            \
    } while (0)

/*!
 * \brief Broadcast indicator for port number.
 *
 * If this bit is set in a port number variable, then the variable
 * should be interpreted as an SBUS broadcast domain instead.
 *
 * See also \ref BCMBD_CMICX_BCAST_BLK.
 */
#define BCMBD_CMICX_BCAST_BIT (1U << 14)

/*!
 * \brief Test if port number is a broadcast domain.
 *
 * Use this macro to test whether a port unmber variable should be
 * interpreted as a physical port number or an SBUS broadcast domain.
 */
#define BCMBD_CMICX_BCAST_PORT(_port) \
    ((_port) >= 0 && ((_port) & BCMBD_CMICX_BCAST_BIT))

/*!
 * \brief Get SBUS broadcast block from port number.
 *
 * Use this macro to extract the SBUS broadcast block number if a port
 * variable is used for SBUS broadcast write.
 *
 * See also \ref BCMBD_CMICX_BCAST_PORT.
 */
#define BCMBD_CMICX_BCAST_BLK(_port) \
    ((_port) & (BCMBD_CMICX_BCAST_BIT - 1))

/*!
 * \brief Write 64-bit memory-mapped register.
 *
 * Based on the global 32-bit access pointer in the DRD HAL.
 *
 * \param [in] unit Unit number.
 * \param [in] addr Register offset in the CMIC register space.
 * \param [in] val 64-bit value to write.
 *
 * \retval SHR_E_NONE No errors.
 */
static inline int
bcmbd_cmicx_reg64_direct_write(int unit, uint32_t addr, uint64_t val)
{
    uint64_t *reg64_iomem = (uint64_t *)bcmdrd_reg32_iomem[unit];
    reg64_iomem[addr/8] = val;
    return SHR_E_NONE;
}

/*!
 * \brief Write two 32-bit registers as a 64-bit entity.
 *
 * This macro should be used only for 64-bit aligned registers and
 * only if \ref BCMBD_CMICX_USE_64BIT returns true.
 *
 * \param [in] _u Unit number.
 * \param [in] _addr Register offset in the CMIC register space.
 * \param [in] _val 64-bit value to write.
 */
#define BCMBD_CMICX_WRITE64(_u, _addr, _val)            \
    bcmbd_cmicx_reg64_direct_write(_u, _addr, _val)

/*!
 * Extract register base type from SOC register base address. The base
 * type is used for certain multi-pipe XGS designs.
 */
#define BCMBD_CMICX_ADDR2BASETYPE(_addr) (((_addr) >> 23) & 0x7)

/*! Maximum number of per-pipe register/memory instances. */
#define BCMBD_CMICX_PHYS_INST_MAX 16

/*!
 * \name Pipe information word encoding.
 * \anchor BCMBD_CMICX_PIPE_xxx
 *
 * The access macro of the 32-bit pipe information encoded word
 * returned from \ref bcmbd_cmicx_pipe_info.
 *
 * \{
 */

/*! Maximum bit length of PMASK in pipe information word. */
#define BCMBD_CMICX_PIPE_PMASK_BITS BCMBD_CMICX_PHYS_INST_MAX

/*! Maximum bit length of LINST in pipe information word. */
#define BCMBD_CMICX_PIPE_LINST_BITS 10

/*! Maximum bit length of SECT in pipe_information word. */
#define BCMBD_CMICX_PIPE_SECT_BITS \
    (32 - BCMBD_CMICX_PIPE_PMASK_BITS - BCMBD_CMICX_PIPE_LINST_BITS)

/*! Bit mask for PMASK in pipe_information word. */
#define BCMBD_CMICX_PIPE_PMASK_MASK \
    ((1U << BCMBD_CMICX_PIPE_PMASK_BITS) - 1)

/*! Bit mask for LINST in pipe information word. */
#define BCMBD_CMICX_PIPE_LINST_MASK \
    ((1U << BCMBD_CMICX_PIPE_LINST_BITS) - 1)

/*! Bit mask for SECT in pipe information word. */
#define BCMBD_CMICX_PIPE_SECT_MASK \
    ((1U << BCMBD_CMICX_PIPE_SECT_BITS) - 1)

/*! Bit shift for PMASK in pipe_information word. */
#define BCMBD_CMICX_PIPE_PMASK_SHIFT 0

/*! Bit shift for LINST in pipe information word. */
#define BCMBD_CMICX_PIPE_LINST_SHIFT BCMBD_CMICX_PIPE_PMASK_BITS

/*! Bit shift for SECT in pipe information word. */
#define BCMBD_CMICX_PIPE_SECT_SHIFT \
    (BCMBD_CMICX_PIPE_PMASK_BITS + BCMBD_CMICX_PIPE_LINST_BITS)

/*! Extract bit map of valid instances from pipe information word. */
#define BCMBD_CMICX_PIPE_PMASK(_pi) \
    (((_pi) >> BCMBD_CMICX_PIPE_PMASK_SHIFT) & BCMBD_CMICX_PIPE_PMASK_MASK)

/*! Extract number of logical instances from pipe information word. */
#define BCMBD_CMICX_PIPE_LINST(_pi) \
    (((_pi) >> BCMBD_CMICX_PIPE_LINST_SHIFT) & BCMBD_CMICX_PIPE_LINST_MASK)

/*! Extract section size encoding from pipe information word. */
#define BCMBD_CMICX_PIPE_SECT(_pi) \
    (((_pi) >> BCMBD_CMICX_PIPE_SECT_SHIFT) & BCMBD_CMICX_PIPE_SECT_MASK)

/*! Extract section size from pipe information word. */
#define BCMBD_CMICX_PIPE_SECT_SIZE(_pi) \
    ((BCMBD_CMICX_PIPE_SECT(_pi) == 0) ? \
     0 : (1 << (BCMBD_CMICX_PIPE_SECT(_pi))))

/*! Encode pipe information word from necessary information. */
#define BCMBD_CMICX_PIPE_INFO_SET(_pmask, _linst, _sect_shft) \
    ((((_pmask) & BCMBD_CMICX_PIPE_PMASK_MASK) << BCMBD_CMICX_PIPE_PMASK_SHIFT) | \
     (((_linst) & BCMBD_CMICX_PIPE_LINST_MASK) << BCMBD_CMICX_PIPE_LINST_SHIFT) | \
     (((_sect_shft) & BCMBD_CMICX_PIPE_SECT_MASK) << BCMBD_CMICX_PIPE_SECT_SHIFT))

/*! \} */

/*! CMICx base address within the AXI address space. */
#define BCMBD_CMICX_AXI_BASE_ADDR       0x3200000

/*!
 * \brief Get physical CMICx base address.
 *
 * This value is needed for various DMA operations.
 *
 * \param unit Unit number.
 *
 * \return CMICx base address within the AXI address space.
 */
static inline uint32_t
bcmbd_cmicx_base_addr(int unit)
{
    /* Same across all CMICx devices */
    return BCMBD_CMICX_AXI_BASE_ADDR;
}

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
bcmbd_cmicx_sym_addr_enable(int unit);

/*!
 * \brief Initialize DRD features for a device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_feature_init(int unit);

/*!
 * \brief Initialize CMICx base driver for a device.
 *
 * This function will initialize default function vectors for
 * register/memory access, etc.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_drv_init(int unit);

/*!
 * \brief Clean up CMICx base driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_drv_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_drv_cleanup(int unit);

/*!
 * \brief Disable CMICx table operations.
 *
 * This function will disable table operations 'insert', 'delete' and
 * 'lookup' in case these are not supported by hardware. Attempting
 * table operations on unsupported hardware may leave the CMIC in an
 * unusable state.
 *
 * This function should be called immediately after \ref
 * bcmbd_cmicx_drv_init if applicable.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_drv_tbl_ops_disable(int unit);

/*!
 * \brief Initialize CMICx S-channel driver for a device.
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
bcmbd_cmicx_schan_init(int unit);

/*!
 * \brief Clean up CMICx S-channel driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_schan_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmbd_cmicx_schan_cleanup(int unit);

/*!
 * \brief Perform CMICx S-channel raw operation.
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
bcmbd_cmicx_schan_raw_op(int unit, uint32_t *buf,
                         int dwc_write, int dwc_read, int *dwc_max);

/*!
 * \brief Initialize CMICx SBUSDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_sbusdma_init(int unit);

/*!
 * \brief Clean up CMICx SBUSDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_sbusdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_sbusdma_cleanup(int unit);

/*!
 * \brief Initialize CMICx FIFODMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_fifodma_init(int unit);

/*!
 * \brief Clean up CMICx FIFODMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_fifodma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_fifodma_cleanup(int unit);

/*!
 * \brief Initialize CMICx CCMDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_ccmdma_init(int unit);

/*!
 * \brief Initialize CMICx V2 CCMDMA driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx2_ccmdma_init(int unit);

/*!
 * \brief Clean up CMICx CCMDMA driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_ccmdma_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_ccmdma_cleanup(int unit);

/*!
 * \brief Initialize CMICx SCHAN FIFO driver for a device.
 *
 * This function will allocat synchronization objects, etc.
 * It will not perform any HW initialization.
 *
 * \param [in] unit Unit number.
 *
 * \return SHR_E_NONE if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_schanfifo_init(int unit);

/*!
 * \brief Clean up CMICx SCHAN FIFO driver for a device.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_schanfifo_init.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_schanfifo_cleanup(int unit);

/*!
 * \brief Initialize I2C driver with the default CMICx I2C driver.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmbd_cmicx_i2c_drv_init(int unit, uint8_t chan);

/*!
 * \brief Clean up CMICx I2C driver.
 *
 * This function will free any resources allocated by \ref
 * bcmbd_cmicx_i2c_drv_init.
 *
 * \param [in] unit Unit number.
 * \param [in] chan SMBus channel.
 *
 * \retval SHR_E_NONE no errors.
 */
extern int
bcmbd_cmicx_i2c_drv_cleanup(int unit, uint8_t chan);

/*!
 * \brief Initialize basic CMICx settings.
 *
 * Ensure that endian settings are correct and that all interrupts are
 * disabled.
 *
 * \param [in] unit Unit number.
 *
 * \return 0 if no errors, otherwise a negative value.
 */
extern int
bcmbd_cmicx_init(int unit);

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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_block_read(int unit, uint32_t blkacc, int blknum,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_block_write(int unit, uint32_t blkacc, int blknum,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_blocks_read(int unit, uint32_t blkacc, int port,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_blocks_write(int unit, uint32_t blkacc, int port,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_port_read(int unit, uint32_t blkacc, int port,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_port_write(int unit, uint32_t blkacc, int port,
                           uint32_t offset, int idx, uint32_t *data, int wsize);

/*!
 * \brief Override information for CMICx-based device.
 *
 * This structure defines the \c ovrr_info used in \ref bcmbd_cmicx_read
 * and \ref bcmbd_cmicx_write.
 *
 * For symbols with 'duplicate' attribute (multiple instances but accessed
 * in a single view), per-instance access is allowed by assigning the
 * \c ovrr_info for some particular reason, e.g., for SER purpose.
 */
typedef struct bcmbd_cmicx_ovrr_info_s {

    /*! Instance index to be overridden. */
    int inst;

} bcmbd_cmicx_ovrr_info_t;

/*!
 * \brief Read physical table entry.
 *
 * Read a register/memory entry on a CMICx-based device.
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_read(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data, size_t wsize);

/*!
 * \brief Write physical table entry.
 *
 * Write a register/memory entry on a CMICx-based device.
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_write(int unit, bcmdrd_sid_t sid,
                  bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                  uint32_t *data);

/*!
 * \brief Insert physical table entry.
 *
 * Insert a memory entry on a CMICx-based device.
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
bcmbd_cmicx_insert(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Delete physical table entry.
 *
 * Delete a memory entry on a CMICx-based device.
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
bcmbd_cmicx_delete(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Lookup physical table entry.
 *
 * Lookup a memory entry on a CMICx-based device.
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
bcmbd_cmicx_lookup(int unit, bcmdrd_sid_t sid,
                   bcmbd_pt_dyn_hash_info_t *dyn_hash_info,
                   void *ovrr_hash_info,
                   uint32_t *data, size_t wsize,
                   bcmbd_tbl_resp_info_t *resp_info, uint32_t *resp_data);

/*!
 * \brief Pop device table entry from FIFO.
 *
 * Pop an entry from a FIFO-enabled physical table on a
 * CMICx-based device.
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
bcmbd_cmicx_pop(int unit, bcmdrd_sid_t sid,
                bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                uint32_t *data, size_t wsize);

/*!
 * \brief Push device table entry to FIFO.
 *
 * Push an entry to a FIFO-enabled physical table on a
 * CMICx-based device.
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
bcmbd_cmicx_push(int unit, bcmdrd_sid_t sid,
                 bcmbd_pt_dyn_info_t *dyn_info, void *ovrr_info,
                 uint32_t *data);

/*!
 * \brief Decode a raw CMICx address to dynamic address information of PT.
 *
 * This function will decode the specified raw CMICx address and return
 * the corresponding symbol ID of a physical table (PT) and
 * the dynamic address information of the PT.
 *
 * Note that BCMDRD_SYMBOL_FLAG_xxx can be set to the \c drd_flags
 * to limit the decoding set of symbols and speed up the decoding process.
 *
 * \param [in] unit Unit number.
 * \param [in] adext CMICx SBUS address extension.
 * \param [in] addr CMICx address beat.
 * \param [in] drd_flags Flags of BCMDRD_SYMBOL_FLAG_xxx if non-zero.
 * \param [out] sid Decoded symbol ID.
 * \param [out] dyn_info Decoded dynamic address information.
 *
 * \return SHR_E_NONE on success, SHR_E_FAIL on failure.
 */
extern int
bcmbd_cmicx_addr_decode(int unit,
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
bcmbd_cmicx_cmd_hdr_get(int unit, bcmdrd_sid_t sid,
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
 * - bits [15:0] Bit map of valid unique access type values across all
 *               base indexes.
 * - bits [25:16] Number of base type instances.
 * - bits [31:26] Section size as log2(n), e.g. 3=>8, 5=>32.
 *
 * If a valid \c baseidx is specified, then information about access
 * constraints are returned as follows:
 *
 * - bits [32:0] Bit map of valid unique access type values for the
 *               specified base index.
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
bcmbd_cmicx_pipe_info(int unit, uint32_t addr, int acctype, int blktype,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_unique_block_read(int unit, uint32_t blkacc, int blknum,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_reg_unique_block_write(int unit, uint32_t blkacc, int blknum,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_unique_block_read(int unit, uint32_t blkacc, int blknum,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_unique_block_write(int unit, uint32_t blkacc, int blknum,
                                   int blkidx, int baseidx,
                                   uint32_t offset, int idx,
                                   uint32_t *data, int wsize);

/*!
 * \brief Read from specific block-based SOC memory.
 *
 * Read from a memory in a specific physical block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Physical block number.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [out] data Buffer for data entry.
 * \param [in] wsize Size of data entry buffer in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_block_read(int unit, uint32_t blkacc, int blknum,
                           uint32_t offset, int idx,
                           uint32_t *data, size_t wsize);

/*!
 * \brief Write to specific block-based SOC memory.
 *
 * Write to a memory in a specific physical block.
 *
 * \param [in] unit Unit number.
 * \param [in] blkacc Block/access type information (as in symbol table).
 * \param [in] blknum Physical block number.
 * \param [in] offset Base offset for this memory.
 * \param [in] idx Array entry.
 * \param [in] data Data entry to write.
 * \param [in] wsize Size of data entry in 32-bit words.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_TIMEOUT S-channel operation timed out.
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_block_write(int unit, uint32_t blkacc, int blknum,
                            uint32_t offset, int idx,
                            uint32_t *data, size_t wsize);

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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_blocks_read(int unit, uint32_t blkacc, int port,
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
 * \retval SHR_E_FAIL Invalid response from S-channel operation.
 */
extern int
bcmbd_cmicx_mem_blocks_write(int unit, uint32_t blkacc, int port,
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
bcmbd_cmicx_mem_blocks_clear(int unit, uint32_t blkacc, uint32_t offset,
                             uint32_t si, uint32_t ei, int wsize);

/*! Clear entire memory. */
#define BCMBD_CMICX_MEM_CLEAR(_u, _m) \
    bcmbd_cmicx_mem_blocks_clear(_u, _m##_BLKACC, _m, \
                                 _m##_MIN, _m##_MAX, (_m##_SIZE + 3) >> 2)

/*!
 * \brief Initialze Broadcom M0 CMICx-Linkscan firmware.
 *
 * This function will download CMICx-Linkscan firmware and then
 * let the uC start running.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 *
 * \retval SHR_E_NONE Intialization successed.
 * \retval SHR_E_FAIL Intialization failed.
 */
extern int
bcmbd_cmicx_m0ssq_fw_linkscan_init(int unit, int uc);

/*!
 * \brief Cleanup Broadcom M0 CMICx-Linkscan firmware.
 *
 * This function will try to stop the uC for CMICx-Linkscan firmware.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 *
 * \retval SHR_E_NONE Cleanup successed.
 * \retval SHR_E_FAIL Cleanup failed.
 */
extern int
bcmbd_cmicx_m0ssq_fw_linkscan_cleanup(int unit, int uc);

/*!
 * \brief Initialze Broadcom M0 CMICx-LED firmware.
 *
 * This function will download CMICx-LED firmware and then
 * let the uC start running.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 *
 * \retval SHR_E_NONE Intialization successed.
 * \retval SHR_E_FAIL Intialization failed.
 */
extern int
bcmbd_cmicx_m0ssq_fw_led_init(int unit, int uc);

/*!
 * \brief Cleanup Broadcom M0 CMICx-LED firmware.
 *
 *  This function will try to stop the uC for run CMICx-LED firmware.
 *
 * \param [in] unit Unit number.
 * \param [in] uc uC number.
 *
 * \retval SHR_E_NONE Cleanup successed.
 * \retval SHR_E_FAIL Cleanup failed.
 */
extern int
bcmbd_cmicx_m0ssq_fw_led_cleanup(int unit, int uc);

#endif /* BCMBD_CMICX_H */
