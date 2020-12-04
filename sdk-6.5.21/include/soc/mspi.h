/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        mspi.h
 * Purpose:     Base definitions for SPI Master mode routines
 * Requires:    
 */
#ifndef _SOC_MSPI_H
#define _SOC_MSPI_H

#ifdef BCM_CMICM_SUPPORT

#define MSPI_FLASH 0
#define MSPI_LIU   1
#define MSPI_DPLL  2
#define MSPI_EXTRA 3 /* Re-Use */

extern int soc_mspi_init(int unit);
extern int soc_mspi_config(int unit, int device, int cpol, int cpha);
extern int soc_mspi_writeread8(int unit, uint8 *wbuf, int wlen, uint8 *rbuf, int rlen);
extern int soc_mspi_read8(int unit, uint8 *buf, int len);
extern int soc_mspi_write8(int unit, uint8 *buf, int len);

#endif /* ifdef BCM_CMICM_SUPPORT */

#endif /* ifdef _SOC_SPIM_H */

