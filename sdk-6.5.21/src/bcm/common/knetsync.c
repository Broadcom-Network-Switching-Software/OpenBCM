/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Time - Broadcom KNETSync API - shared functionality
 */

#include <shared/bsl.h>
#include <soc/drv.h>
#include <soc/knet.h>
#include <soc/property.h>
#include <include/soc/uc.h>
#include <bcm/error.h>

#ifdef NO_SAL_APPL
#include <string.h>
#else
#include <sal/appl/sal.h>
#endif


/*
 * Function:
 *      _bcm_common_mirror_mtp_update
 * Purpose:
 *      Initialize knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_common_knetsync_mtp_update(int unit, int mtp_idx, int start)
{
#ifndef INCLUDE_KNETSYNC
    /* function called regardless knetsync is insmoded or not. */
    return BCM_E_NONE;
#else
    int rv;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = ((start == 1) ? 
                                KSYNC_M_MTP_TS_UPDATE_ENABLE:
                                KSYNC_M_MTP_TS_UPDATE_DISABLE);
    clock_cmd.clock_info.data[0] = mtp_idx;

    rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                          sizeof(clock_cmd), sizeof(clock_cmd));

    /* Return OK even if knet module is not loaded */
    if (SOC_E_UNAVAIL == rv) {
        rv = SOC_E_NONE;
    }

    /* Return OK even if clock driver kernel module not loaded */
    if (SOC_E_NOT_FOUND == rv) {
        rv = SOC_E_NONE;
    }

    return rv;
#endif
}

/*
 * Function:
 *      bcm_common_knetsync_init
 * Purpose:
 *      Initialize knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_common_knetsync_init(int unit)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    unsigned knetsync_pci_cosq;
    int uc;
    char *version = NULL;
    int knetsync_fw = 0;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = KSYNC_M_HW_INIT;
    knetsync_pci_cosq = soc_property_get(unit, spn_KNETSYNC_PCI_COSQ, 0);  /* 0->Default */
    clock_cmd.clock_info.data[0] = knetsync_pci_cosq; /* CosQ Property */;

    for(uc=0; uc<SOC_INFO(unit).num_ucs; uc++) {
        if (!soc_uc_in_reset(unit, uc)) {
            version = soc_uc_firmware_version(unit, uc);
            if (version) {
#ifdef NO_SAL_APPL
                if (strstr((char *)version,(char *)"KNETSYNC")) {
#else
                if (sal_strcasestr((char *)version,(char *)"KNETSYNC")) {
#endif
                    knetsync_fw = 1;
                }
                soc_cm_sfree(unit, version);
                if (1 == knetsync_fw) {
                    break;
                }
            }
        }
    }

    if (knetsync_fw == 0) {
        LOG_INFO(BSL_LS_BCM_KNETSYNC,
                 (BSL_META_U(0, "KNETSync Application is not loaded\n")));
        rv = BCM_E_NOT_FOUND;
        return rv;
    }
    clock_cmd.clock_info.data[1] = uc; /* FW Core */;

    rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                          sizeof(clock_cmd), sizeof(clock_cmd));
    return rv;
#endif
}

/*
 * Function:
 *      bcm_common_knetsync_deinit
 * Purpose:
 *      Cleanup knetsync interface.
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
bcm_common_knetsync_deinit(int unit)
{
#ifndef INCLUDE_KNETSYNC
    return BCM_E_UNAVAIL;
#else
    int rv;
    kcom_msg_clock_cmd_t clock_cmd;

    sal_memset(&clock_cmd, 0, sizeof(clock_cmd));
    clock_cmd.hdr.opcode = KCOM_M_CLOCK_CMD;
    clock_cmd.hdr.unit = unit;
    clock_cmd.clock_info.cmd = KSYNC_M_HW_DEINIT;
    rv = soc_knet_cmd_req((kcom_msg_t *)&clock_cmd,
                          sizeof(clock_cmd), sizeof(clock_cmd));
    return rv;
#endif
}

