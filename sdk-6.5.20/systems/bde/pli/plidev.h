/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PLISIM device API
 */

#ifndef __PLIDEV_H__
#define __PLIDEV_H__

#include <sal/types.h>

extern int
plidev_supported(uint16 pciVenID, uint16 pciDevID, uint8 pciRevID);

#endif /* __PLIDEV_H__ */
