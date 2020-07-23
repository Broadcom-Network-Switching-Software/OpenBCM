/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        spi.h
 * Purpose:     Base definitions for SPI routines
 * Requires:    
 */

#ifndef _SOC_SPI_H
#define _SOC_SPI_H


#include <soc/defs.h>
#include <soc/field.h>


/* SOC SPI Routines */
extern int soc_spi_read(int unit, uint32 addr, uint8 *buf, int len);
extern int soc_spi_write(int unit, uint32 addr, uint8 *buf, int len);


#endif	/* !_SOC_SPI_H */
