/*! \file bcmdrd_dev.h
 *
 * DRD device APIs.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMDRD_DEV_H
#define BCMDRD_DEV_H

#include <shr/shr_bitop.h>
#include <shr/shr_error.h>

#include <bcmdrd/bcmdrd.h>
#include <bcmdrd/bcmdrd_chip.h>
#include <bcmdrd/bcmdrd_hal.h>

/*! Global pointers for direct access to memory-mapped registers. */
extern volatile uint32_t *bcmdrd_reg32_iomem[BCMDRD_CONFIG_MAX_UNITS];

#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1

static inline int
bcmdrd_hal_reg32_direct_read(int unit, uint32_t addr, uint32_t *val)
{
    *val = bcmdrd_reg32_iomem[unit][addr/4];
    return SHR_E_NONE;
}

static inline int
bcmdrd_hal_reg32_direct_write(int unit, uint32_t addr, uint32_t val)
{
    bcmdrd_reg32_iomem[unit][addr/4] = val;
    return SHR_E_NONE;
}

#endif

/*!
 * \brief Read 32-bit device register.
 *
 * Wrapper for \ref bcmdrd_hal_reg32_read, which may be redefined to a
 * direct memory-mapped access for maximum performance.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Register offset relative to register base.
 * \param [out] _val Data read from register.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL I/O read returned an error.
 */
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#define BCMDRD_DEV_READ32(_unit, _addr, _val)   \
    bcmdrd_hal_reg32_direct_read(_unit, _addr, _val)
#else
#define BCMDRD_DEV_READ32(_unit, _addr, _val)   \
    bcmdrd_hal_reg32_read(_unit, _addr, _val)
#endif

/*!
 * \brief Write 32-bit device register.
 *
 * Wrapper for \ref bcmdrd_hal_reg32_write, which may be redefined to
 * a direct memory-mapped access for maximum performance.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Register offset relative to register base.
 * \param [in] _val Data to write to register.
 */
#if BCMDRD_CONFIG_MEMMAP_DIRECT == 1
#define BCMDRD_DEV_WRITE32(_unit, _addr, _val) \
    bcmdrd_hal_reg32_direct_write(_unit, _addr, _val)
#else
#define BCMDRD_DEV_WRITE32(_unit, _addr, _val) \
    bcmdrd_hal_reg32_write(_unit, _addr, _val)
#endif

/*!
 * \brief Read iProc register.
 *
 * The register address can be specified anywhere in the 32-bit AXI
 * address space.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Absolute register offset in AXI address space.
 * \param [out] _val Data read from register.
 */
#define BCMDRD_IPROC_READ(_unit, _addr, _val)   \
    bcmdrd_hal_iproc_read(_unit, _addr, _val)

/*!
 * \brief Write iProc register.
 *
 * The register address can be specified anywhere in the 32-bit AXI
 * address space.
 *
 * \param [in] _unit Unit number.
 * \param [in] _addr Absolute register offset in AXI address space.
 * \param [in] _val Data to write to register.
 */
#define BCMDRD_IPROC_WRITE(_unit, _addr, _val)   \
    bcmdrd_hal_iproc_write(_unit, _addr, _val)

/*!
 * \brief Check if multi-byte register access needs byte-swapping.
 *
 * If the endianness of the host CPU and the switch device differ,
 * then it may be necessary to perform a byte-swap for each register
 * access.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * Byte-swapping may be implemented in either hardware or software
 * depdending on the device capabilities.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise false.
 */
extern bool
bcmdrd_dev_byte_swap_pio_get(int unit);

/*!
 * \brief Check if packet DMA needs byte-swapping.
 *
 * Packet data is represented as an array of bytes irrespective of the
 * CPU endianness, however packet data may pass through a generic
 * byte-swapping bridge during a DMA operation, in which case the
 * packet data must be swapped back to its original format.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise
 * false.
 */
extern bool
bcmdrd_dev_byte_swap_packet_dma_get(int unit);

/*!
 * \brief Check if non-packet DMA needs byte-swapping.
 *
 * If the endianness of the host CPU and the switch device differ,
 * then it may be necessary to perform a byte-swap on data transferred
 * between host memory and switch device memory via DMA.
 *
 * The endianness information must be provided by the system when I/O
 * resources are assigned. See also \ref bcmdrd_dev_hal_io_init.
 *
 * \param [in] unit Unit number.
 *
 * \return true if byte-swapping is required, otherwise
 * false.
 */
extern bool
bcmdrd_dev_byte_swap_non_packet_dma_get(int unit);

/*!
 * \brief Get device bus type.
 *
 * Get the bus type by which the device os connected to the host CPU.
 *
 * The bus type is normally PCI if an external host CPU is used, and
 * AXI (ARM interconnect) is an embedded CPU is used.
 *
 * The bus type may affect both DMA access and register access
 * configurations.
 *
 * \param [in] unit Unit number.
 *
 * \return Device bus type.
 */
extern bcmdrd_hal_bus_type_t
bcmdrd_dev_bus_type_get(int unit);

/*!
 * \brief Get device IO flags.
 *
 * Get device IO flags, (BCMDRD_HAL_IO_F_XXX).
 *
 * \param [in] unit Unit number.
 *
 * \retval Device IO flags.
 */
extern uint32_t
bcmdrd_dev_io_flags_get(int unit);

/*!
 * \brief Provide block of high-availability DMA memory.
 *
 * This function is used by the application to supply the SDK with a
 * fixed-size block of high-availability DMA memory.
 *
 * The SDK will expect to get the same block of memory assigned after
 * a warm-boot or a crash.
 *
 * The memory block must be physically contiguous and cache-coherent.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes).
 * \param [in] ha_dma_mem Logical address of DMA memory block.
 * \param [out] dma_addr Physical address of DMA memory block.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmdrd_dev_ha_dma_mem_set(int unit, size_t size,
                          void *ha_dma_mem, uint64_t dma_addr);

/*!
 * \brief Get pointer to high-availability DMA memory.
 *
 * The SDK will use this API to access the HA DMA memory block
 * provided by the application.
 *
 * The application may call this API with \c dma_addr as a NULL
 * pointer to get the desired size of the HA DMA memory blcok.
 *
 * \param [in] unit Unit number.
 * \param [in] size Size of DMA memory block (in bytes).
 * \param [out] dma_addr Physical address of DMA memory block.
 *
 * \return Pointer to DMA memory block or NULL if an error occurred.
 */
extern void *
bcmdrd_dev_ha_dma_mem_get(int unit, size_t *size, uint64_t *dma_addr);

/*!
 * \brief Set chip information.
 *
 * The chip information consiste mainly of block and port information,
 * but it may also provide functions to retrieve special register and
 * memory attributes, etc.
 *
 * Note that only a pointer to the chip information structure will be
 * stored in the DRD device strucutre.
 *
 * \param [in] unit Unit number.
 * \param [in] chip_info Pointer to chip information structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_chip_info_set(int unit, const bcmdrd_chip_info_t *chip_info);

/*!
 * \brief Get a pointer to the chip information structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Pointer to chip information, or NULL if not found.
 */
extern const bcmdrd_chip_info_t *
bcmdrd_dev_chip_info_get(int unit);

/*!
 * \brief Set the bitmap of valid ports for this unit.
 *
 * Ports not supported by the underlying device will be ignored.
 *
 * \param [in] unit Unit number.
 * \param [in] pbmp Port bitmap of valid ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_valid_ports_set(int unit, const bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get the bitmap of valid ports for this unit.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Port bitmap of valid ports.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_valid_ports_get(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Set the bitmap of valid pipes for this unit.
 *
 * Override the default set of valid pipes for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] pipe_map Bitmap of valid pipes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_valid_pipes_set(int unit, uint32_t pipe_map);

/*!
 * \brief Get the bitmap of valid pipes for this unit.
 *
 * Pipes without any ports in the underlying device will be ignored.
 *
 * Ports that do not belong to a valid pipe will be removed from the
 * bitmap of valid ports for this unit.
 *
 * \param [in] unit Unit number.
 * \param [out] pipe_map Bitmap of valid pipes.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmdrd_dev_valid_pipes_get(int unit, uint32_t *pipe_map);

/*!
 * \brief Get the bitmap of valid pipes for a device-specific block type.
 *
 * This function transforms the bitmap of pipes from device basis to
 * block basis. If \c dev_pipe_map is 0, the valid pipes bitmap of the device
 * will be used for the transformation.
 *
 * \param [in] unit Unit number.
 * \param [in] dev_pipe_map Bitmap of pipes on device basis.
 * \param [in] blktype Device-specific block type.
 * \param [out] blk_pipe_map Bitmap of valid pipes for the specified block.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 * \retval SHR_E_PARAM Invalid parameter.
 */
extern int
bcmdrd_dev_valid_blk_pipes_get(int unit, uint32_t dev_pipe_map,
                               int blktype, uint32_t *blk_pipe_map);

/*!
 * \brief Get pointer to device symbol table.
 *
 * \param [in] unit Unit number.
 * \param [in] idx Symbol table index (currently only 0 is valid).
 *
 * \return Pointer to symbol table, or NULL if not found.
 */
extern const bcmdrd_symbols_t *
bcmdrd_dev_symbols_get(int unit, int idx);

/*!
 * \brief Get device-specific flags.
 *
 * Get the flags defined in the chip information structure.
 *
 * \param [in] unit Unit number.
 *
 * \return Device-specific flags.
 */
extern uint32_t
bcmdrd_dev_flags_get(int unit);

/*!
 * \brief Set pipeline bypass mode.
 *
 * This setting may have different meanings on different devices all
 * depending on the bypass capabilities of a particular device type.
 *
 * The base driver will use this setting to silently ignore access to
 * bypassed registers and memories. Other driver components may use
 * this setting to modify their internal behavior.
 *
 * \param [in] unit Unit number.
 * \param [in] bypass_mode Device-specific bypass mode.
 *
 * \return Nothing.
 */
extern void
bcmdrd_dev_bypass_mode_set(int unit, uint32_t bypass_mode);

/*!
 * \brief Get pipeline bypass mode.
 *
 * See \ref bcmdrd_dev_bypass_mode_set for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Device-specific bypass mode.
 */
extern uint32_t
bcmdrd_dev_bypass_mode_get(int unit);

/*!
 * \brief Get pipe information for multi-pipe device.
 *
 * It is assumed that the caller of this function is familiar with the
 * S-channel access mechanism for multi-pipe XGS designs, including
 * the concepts of access type and base type.
 *
 * This function will return different information based on different
 * query key type \c pi_type(\ref bcmdrd_pipe_info_type_t).
 *
 * \param [in] unit Unit number.
 * \param [in] pi Device-specific parameters to query related pipe information.
 * \param [in] pi_type Query key for specific pipe information.
 *
 * \return Specific pipe information based on query key.
 */
extern uint32_t
bcmdrd_dev_pipe_info(int unit, bcmdrd_pipe_info_t *pi,
                     bcmdrd_pipe_info_type_t pi_type);

/*!
 * \brief Get special address calculation function.
 *
 * Used for non-standard address calculations. See \ref
 * bcmdrd_block_port_addr_f for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Function pointer, or NULL if standard address calculation.
 */
extern bcmdrd_block_port_addr_f
bcmdrd_dev_block_port_addr_func(int unit);

/*!
 * \brief Get address decode function.
 *
 * Used for non-standard address decoding. See \ref
 * bcmdrd_addr_decode_f for details.
 *
 * \param [in] unit Unit number.
 *
 * \return Function pointer, or NULL if standard address decoding.
 */
extern bcmdrd_addr_decode_f
bcmdrd_dev_addr_decode_func(int unit);

/*!
 * \brief Get pipeline index of a given physical port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port Physical port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_phys_port_pipe(int unit, int port);

/*!
 * \brief Get pipeline index of a given logical port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_logic_port_pipe(int unit, int port);

/*!
 * \brief Get pipeline index of a given MMU port for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] port MMU port number.
 *
 * \return Pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_mmu_port_pipe(int unit, int port);

/*!
 * \brief Get device-specific block-based pipeline index of a given port.
 *
 * The port number domain of the specified port must match
 * the port number domain of the specified block type.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Device-specific block type.
 * \param [in] port Port number.
 *
 * \return block-based pipeline index, or -1 on error.
 */
extern int
bcmdrd_dev_blk_port_pipe(int unit, int blktype, int port);

/*!
 * \brief Get bitmap of valid physical ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid physical ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_phys_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of valid logical ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid logical ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_logic_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of valid MMU ports for this unit.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit Unit number.
 * \param [out] pbmp Bitmap of valid MMU ports.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_mmu_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of ports associated with a given block type.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * The block type enumeration is specific to the base device type.
 *
 * \param [in] unit Unit number.
 * \param [in] blktype Device-specific block type.
 * \param [out] pbmp Bitmap of ports associated with the specified block type.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_blktype_pbmp(int unit, int blktype, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of physical ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of physical ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_phys_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of logical ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of logical ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_logic_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of MMU ports associated with a given switch pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 * \param [out] pbmp Bitmap of MMU ports associated with the specified
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_pipe_mmu_pbmp(int unit, int pipe_no, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get bitmap of ports associated with a given block-based pipeline.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] blktype Device-specific block type.
 * \param [in] blk_pipe_no Device-specific block-based pipeline index.
 * \param [out] pbmp Bitmap of ports associated with the specified block-based
 *                   pipeline.
 *
 * \retval 0 No errors.
 * \retval -1 Invalid parameter.
 */
extern int
bcmdrd_dev_blk_pipe_pbmp(int unit, int blktype, int blk_pipe_no,
                         bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get block number from type and instance.
 *
 * Given a block type and an instance number of that block type, this
 * function will return the corresponding physical block number.
 *
 * \param [in] unit unit number.
 * \param [in] blktype Device-specific block type.
 * \param [in] blkinst Instance number for block type.
 *
 * \return Physical block number, or -1 if no matching block is found.
 */
extern int
bcmdrd_dev_block_number(int unit, int blktype, int blkinst);

/*!
 * \brief Get bitmap of physical loopback ports.
 *
 * The returned port bitmap is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [out] pbmp Bitmap of physical loopback ports.
 *
 * \retval 0 No errors.
 * \retval -1 Feature is not supported for the specified device.
 */
extern int
bcmdrd_dev_lb_pbmp(int unit, bcmdrd_pbmp_t *pbmp);

/*!
 * \brief Get number of physical ports associated with a given switch pipeline.
 *
 * The returned number of ports is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 *
 * \return Number of physical ports, or -1 on error.
 */
extern int
bcmdrd_dev_pipe_num_phys_ports(int unit, int pipe_no);

/*!
 * \brief Get number of logical ports associated with a given switch pipeline.
 *
 * The returned number of ports is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 *
 * \return Number of logical ports, or -1 on error.
 */
extern int
bcmdrd_dev_pipe_num_logic_ports(int unit, int pipe_no);

/*!
 * \brief Get number of MMU ports associated with a given switch pipeline.
 *
 * The returned number of ports is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] pipe_no Switch pipeline index.
 *
 * \return Number of MMU ports, or -1 on error.
 */
extern int
bcmdrd_dev_pipe_num_mmu_ports(int unit, int pipe_no);

/*!
 * \brief Get number of ports associated with a given block-based pipeline.
 *
 * The returned number of ports is defined by the switch device type only,
 * i.e. it is independent of the current switch configuration.
 *
 * \param [in] unit unit number.
 * \param [in] blktype Device-specific block type.
 * \param [in] blk_pipe_no Device-specific block-based pipeline index.
 *
 * \return Number of ports, or -1 on error.
 */
extern int
bcmdrd_dev_blk_pipe_num_ports(int unit, int blktype, int blk_pipe_no);

/*!
 * \brief Set the tainted state for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] tainted Tainted state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_tainted_set(int unit, bool tainted);

/*!
 * \brief Get the tainted state for this unit.
 *
 * \param [in] unit Unit number.
 * \param [in] tainted Tainted state.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_UNIT Invalid unit number.
 */
extern int
bcmdrd_dev_tainted_get(int unit, bool *tainted);

/*!
 * \brief Get chip modifier object for a given symbol.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Device-specific symbol ID
 *
 * \return Pointer to chip modifier object or NULL if not found.
 *
 * \param [in] unit
 * \param [in] sid
 *
 * \retval 0 No errors
 */
extern bcmdrd_chip_mod_t *
bcmdrd_dev_mod_get(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get adjusted maximum index of a specific memory.
 *
 * This function returns adjusted maximum index of a specific memory.
 * Some device variant have smaller memories than the base device, and
 * this function can be used to retrieve such override values. If no
 * override information is found, then the input \c maxidx is
 * returned.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol index in DRD of the specific memory.
 * \param [in] maxidx Default maximum index of the specific memory.
 *
 * \return Maximum index of a specific memory.
 */
extern uint32_t
bcmdrd_dev_mem_maxidx(int unit, bcmdrd_sid_t sid, uint32_t maxidx);

/*!
 * \brief Check if symbol is marked as invalid.
 *
 * For some device variants (SKUs) some registers and memories may
 * have been disabled.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID to check.
 *
 * \retval true Symbol is not marked as invalid.
 * \retval false Symbol is marked as invalid.
 */
bool
bcmdrd_dev_sym_valid(int unit, bcmdrd_sid_t sid);

/*!
 * \brief Get chip parameter.
 *
 * Get device-specific device property value.
 *
 * \param [in] unit Unit number.
 * \param [in] prm_name Parameter name.
 * \param [out] prm_val Parameter value.
 *
 * \retval 0 No errors.
 * \retval -1 Not found.
 */
extern int
bcmdrd_dev_param_get(int unit, const char *prm_name, uint32_t *prm_val);

/*!
 * \brief Get next supported chip parameter.
 *
 * Device variants may implement device-specific chip parameters
 * whenever a device variant property differs from the base device.
 *
 * Pass in NULL to get the first supported parameter. Pass in the name
 * of the first supported parameter to get the second parameter and so
 * forth.
 *
 * \param [in] unit Unit number.
 * \param [in] prm_name Parameter name.
 *
 * \return Name of next parameter or NULL if no more parameters.
 */
extern const char *
bcmdrd_dev_param_next(int unit, const char *prm_name);


/*!
 * \brief Get device signature.
 *
 * Get pointer to a string containing a message digest generated
 * across the symbol table. This string will change if there has been
 * any changes to the register and memory definitions.
 *
 * \param [in] unit Unit number.
 *
 * \return Signature string or NULL if invalid unit number.
 */
extern const char *
bcmdrd_dev_signature_get(int unit);

/*!
 * \brief Device-specific function for registers/memories index validity.
 *
 * If there are exceptions of registers/memories that some indices or instances
 * can not be accessed in the indices or instances range, this function can be
 * implemented as device-specific to reflect the exceptions for table indices
 * or instances.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID to check.
 * \param [in] tbl_inst Table instance to check.
 * \param [in] tbl_idx Table index to check.
 * \param [out] True if the \c idx for the \c sid is valid, otherwise false.
 *
 * \retval SHR_E_NONE The returned \c valid can be referenced as validity.
 * \retval SHR_E_UNIT The Unit number \c unit is invalid.
 * \retval SHR_E_UNAVAIL No exceptions check support for the \c unit or \c sid.
 */
typedef int (*bcmdrd_dev_pt_index_valid_f)(int unit, bcmdrd_sid_t sid,
                                           int tbl_inst, int tbl_idx,
                                           bool *valid);

/*!
 * \brief Set device-specific function of index validity.
 *
 * Set device-specific index validity function to the device resource database.
 * Set \c func to NULL to clean up the database.
 *
 * \param [in] unit Unit number.
 * \param [in] func Device-specific index validity function.
 *
 * \retval SHR_E_NONE The returned \c valid can be referenced as validity.
 * \retval SHR_E_UNIT The Unit number \c unit is invalid.
 */
extern int
bcmdrd_dev_pt_index_valid_func_set(int unit, bcmdrd_dev_pt_index_valid_f func);

/*!
 * \brief Check exceptions for registers/memories index or instance validity.
 *
 * This function can be used to check if there is any exception index or
 * instance that can not be accessed in the indices or instances range of
 * registers/memories.
 *
 * \param [in] unit Unit number.
 * \param [in] sid Symbol ID to check.
 * \param [in] tbl_inst Table instance to check.
 * \param [in] tbl_idx Table index to check.
 * \param [out] valid True if the \c tbl_inst and \c tbl_idx for the \c sid
 *                    is valid, otherwise false.
 *
 * \retval SHR_E_NONE The returned \c valid can be referenced as validity.
 * \retval SHR_E_UNIT The Unit number \c unit is invalid.
 * \retval SHR_E_UNAVAIL No exceptions check support for the \c unit or \c sid.
 */
extern int
bcmdrd_dev_pt_index_valid(int unit, bcmdrd_sid_t sid, int tbl_inst, int tbl_idx,
                          bool *valid);

#endif /* BCMDRD_DEV_H */
