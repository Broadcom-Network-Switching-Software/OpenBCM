/*! \file pci_common.c
 *
 * PCI abstraction common to all platforms.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>

#include <stdio.h>
#include <inttypes.h>

#include <sim/pli/pci.h>

#ifndef PCI_MAX_BUS
#define PCI_MAX_BUS             1
#endif

#ifndef PCI_MAX_DEV
#define PCI_MAX_DEV             32
#endif

/*
 * pci_device_iter
 *
 *   Scans all PCI busses and slots.
 *   Calls a user-supplied routine once per PCI device found.
 *   Passes the device location (dev) and device IDs to the routine.
 *   Continues as long as the user routine returns 0.
 *   Otherwise, stops and returns the value from the user routine.
 */

int
pci_device_iter(int (*rtn)(pci_dev_t *dev,
                           uint16_t pci_ven_id,
                           uint16_t pci_dev_id,
                           uint8_t pci_rev_id))
{
    uint32_t ven_id, dev_id, rev_id;
    int rv = 0;
    pci_dev_t dev;

    dev.func_no = 0;

    for (dev.bus_no = 0;
         dev.bus_no < PCI_MAX_BUS && rv == 0;
         dev.bus_no++) {

        for (dev.dev_no = 0;
             dev.dev_no < PCI_MAX_DEV && rv == 0;
             dev.dev_no++) {

            if (dev.dev_no == 12) {
                continue;
            }

            ven_id = pci_config_getw(&dev, PCI_CONF_ID) & 0xffff;

            if (ven_id == 0xffff) {
                continue;
            }

            dev_id = pci_config_getw(&dev, PCI_CONF_ID) >> 16;
            rev_id = pci_config_getw(&dev, PCI_CONF_REV) & 0xff;

            rv = (*rtn)(&dev, ven_id, dev_id, rev_id);
        }
    }

    return rv;
}
