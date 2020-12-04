/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose:     Header for CMICX QSPI driver
 */
#ifndef _CMICX_QSPI_H_
#define _CMICX_QSPI_H_

/*!
 * Initialize QSPI. software and hardware resources
 *
.* unit: Device ID.
 * max_hz:  Maximum SCK rate in Hz.
 * mode:    Clock polarity, clock phase and other parameters.
 *
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_init(int unit,
                    uint32 max_hz,
                    uint32 mode);

/*!
 * Cleanup QSPI resources
 *
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_cleanup(int unit);

/*!
 * Claim the bus and prepare it for communication.
 * This must be called before doing any transfers
 *
.* unit: Device ID.
 * Returns: SOC_E_XXX
 */
extern int
cmicx_qspi_claim_bus(int unit);

/*!
 * Release the SPI bus
 *
 * This must be called once all transfers have finished
 *
.* unit: Device ID.
 *   slave:	The SPI slave
 */
extern int
cmicx_qspi_release_bus(int unit);

/*!
 * QSPI Read Write
 *
 * This writes  the SPI MOSI port and simultaneously clocks
 * read from the MISO port
.*
.* unit: Device ID.
 * en: Bytes to write and read.
 * dout: Data Out buffer.
 * din: Data in buffer.
 * flags: A bitwise combination of SPI_XFER_* flags.
 *
 *   Returns: 0 on success, not 0 on failure
 */
extern int
cmicx_qspi_rw(int unit,
                  uint32 len,
                  const void *dout,
                  void *din,
                  uint32 flags);

/* Possible values for access_method below */
#define QSPI_ACCESS_METHOD_PCIE 0 /* Access using the 1st PCIe BAR (default) */
#ifdef INCLUDE_CPU_I2C
#define QSPI_ACCESS_METHOD_I2C 1 /* Access by I2C using specified IDs */
#define QSPI_ACCESS_METHOD_I2C_BDE 2 /* by I2C using the BDE */
#endif

/* Set the current QSPI access method */
extern void
iproc_qspi_set_access_method(int unit, uint8 access_method, uint8 i2c_bus, uint8 i2c_dev);

#ifdef INCLUDE_CPU_I2C
/* Read a uint32 from the AXI address space using the current QSPI access method */
extern int
iproc_qspi_read(int unit, uint32 addr, uint32 *read_value);
/* write a uint32 from the AXI address space using the current QSPI access method */
extern int
iproc_qspi_write(int unit, uint32 addr, uint32 value);
#endif /* INCLUDE_CPU_I2C */

#endif /* _CMICX_QSPI_H_ */
