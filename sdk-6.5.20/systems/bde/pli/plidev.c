/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * PLISIM device API
 */

#include "plidev.h"

#include <soc/devids.h>
#include <sal/core/libc.h>

#ifdef BCM_LTSW_SUPPORT
#include <bcmdrd/bcmdrd_dev.h>
#endif

/*!
 * \brief Check whether the probed PLI device is supported or not.
 *
 * Use this function to check whether the probed PLI device is supported
 * or not.
 *
 * Note:
 * In HSDK infrastructure, the PCI device will be probed twice. The first round
 * is executed by SDKLT for probing the SDKLT-based devices. The second round
 * is executed by SDK6 for probing other devices. In some scenarios, one device
 * could be supported on both SDK6 and SDKLT. In order to prevent one device
 * from being probed twice, it is necessary to skip SDKLT-based device during
 * the second round SDK6 probe.
 *
 * \param [in] pciVenID The vendor identifier in PCI configuration space.
 * \param [in] pciDevID The device identifier in PCI configuration space.
 * \param [in] pciRevID The revision identifier in PCI configuration space.
 *
 * \retval TRUE Supported.
 * \retval FALSE Unsupported.
 */
int
plidev_supported(uint16 pciVenID, uint16 pciDevID, uint8 pciRevID)
{
#ifdef BCM_LTSW_SUPPORT
    bcmdrd_dev_id_t id;
#endif

    if((pciVenID != BROADCOM_VENDOR_ID)) {
        return FALSE;
    }

    /* don't want to expose non 56XX devices */
    if (pciVenID == BROADCOM_VENDOR_ID) {
        if(((pciDevID & 0xFF00) != 0x5600) &&
           ((pciDevID & 0xF000) != 0xc000) &&
           ((pciDevID & 0xFFF0) != 0x0230) &&
           ((pciDevID & 0xF000) != 0xb000) &&
           (pciDevID != BCM83207_DEVICE_ID) &&
           (pciDevID != BCM83208_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88650_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88675_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88375_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88680_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88690_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88800_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88850_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88470_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM88270_DEVICE_ID) &&
           ((pciDevID & 0xFFFF) != BCM8206_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88750_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88950_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88790_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM88660_DEVICE_ID) &&
           ((pciDevID & 0xFFF2) != BCM88202_DEVICE_ID) &&
           ((pciDevID & 0xFF00) != BCM53400_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM53570_DEVICE_ID) &&
           ((pciDevID & 0xFFF0) != BCM53540_DEVICE_ID))
           {
            return FALSE;
        }
    }

#ifdef BCM_LTSW_SUPPORT
    /* Set up ID structure */
    sal_memset(&id, 0, sizeof(id));
    id.vendor_id = pciVenID;
    id.device_id = pciDevID;
    id.revision = pciRevID;

    /* Skip SDKLT supported devices */
    if (bcmdrd_dev_supported(&id)) {
        return FALSE;
    }
#endif

    return TRUE;
}
