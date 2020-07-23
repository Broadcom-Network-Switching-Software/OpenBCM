/*! \file bcmbd_cmicx_init.c
 *
 * Basic CMICx initialization.
 *
 * PCI PIO endianness is controlled in the iProc AXI/PCI bridge (PAXB)
 * and DMA endianness is controlled by the individual drivers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <bcmbd/bcmbd_cmicx.h>

/* Log source for this component */
#define BSL_LOG_MODULE  BSL_LS_BCMBD_DEV

/*******************************************************************************
 * Local definitions
 */

/* Global variable for optimized access */
bool bcmbd_cmicx_use_64bit;

/*******************************************************************************
 * Local functions
 */

/*!
 * \brief Initialize 64-bit CMIC access.
 *
 * 64-bit CMIC write access is currenlty supported for 64-bit little
 * endian host CPUs.
 *
 * \param [in] unit Unit number.
 */
static void
cmicx_64bit_init(int unit)
{
    bcmbd_cmicx_use_64bit = false;

    if (BCMBD_CMICX_64BIT_SUPPORT(unit)) {
        int big_endian = 1;
        char *little_endian = (char *)&big_endian;
        if (*little_endian) {
            bcmbd_cmicx_use_64bit = true;
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Enable 64-bit CMIC writes\n")));
        }
    }
}

/*******************************************************************************
 * Public functions
 */

int
bcmbd_cmicx_init(int unit)
{
    cmicx_64bit_init(unit);

    return 0;
}
