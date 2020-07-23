/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * The PCID application handler.
 *
 * Requires:
 *     All modules
 *     
 * Provides:
 *     Initialization
 */

#ifdef SINGLE_MODE

#include <shared/bsl.h>
#include <shared/bslext.h>

#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <soc/mem.h>
#include <soc/hash.h>
#include <soc/cmext.h>

#include <soc/cmic.h>
#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif
#include <soc/drv.h>
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DFE_SUPPORT)
#include <soc/dpp/drv.h>
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_PETRA_SUPPORT
#include <soc/dpp/mbcm.h>
#endif
#include <soc/devids.h>
#include <soc/chip.h>
#include <soc/mcm/driver.h>    /* soc_base_driver_table */
#include <soc/debug.h>
#include <sal/appl/io.h>

#include <bde/pli/verinet.h>
#include <bde/pli/plibde.h>

#include <sal/core/boot.h>
#include <sal/appl/config.h>

#include <sim/pcid/pcid.h>
#include <sim/pcid/mem.h>
#include <sim/pcid/cmicsim.h>
#include <sim/pcid/dma.h>
#include <sim/pcid/pli.h>
#include <sim/pcid/socintf.h>
#include <sim/pcid/chipsim.h>

pcid_info_t pcid_info;

void
local_pcid_info_init(pcid_info_t *pcid_info)
{
    sal_memset(pcid_info, 0, sizeof(pcid_info_t));
    pcid_info->opt_gbp_banks = 2;
    pcid_info->opt_gbp_mb = 32;
    pcid_info->opt_gbp_wid = 128;

    pcid_info->single_mode = 1;
    
    if ((pcid_info->pkt_mutex = sal_mutex_create("pkt_mutex")) == NULL) {
    printf("sal_mutex_create -pkt_mutex failed\n");
    exit(1);
    }
    soc_cm_init();
}

int
_local_setup(pcid_info_t* pcid_info, char *chip_name, uint16 *dev_id, uint8 *rev_id)
{
    int             i, chip, found;
#ifndef __STRICT_ANSI__
    extern char    *optarg;
#endif
    uint16          pci_dev_id, pci_ven_id;
    uint8           pci_rev_id;

    local_pcid_info_init(pcid_info);

    pcid_info->opt_chip_name = chip_name;
    pcid_info->sbus_addr_version = SBUS_ADDR_VERSION_2;

    found = 0;

    for (i = 0; i < SOC_CHIP_TYPES_COUNT; i++) {
        if (sal_strcasecmp(pcid_info->opt_chip_name, soc_chip_type_names[i]) == 0) {
            found = 1;
            pcid_info->opt_chip = i;
            chip = soc_chip_type_to_index(pcid_info->opt_chip);
            break;
        }
        /* Allow specifying chip by number only, e.g. 5690 or 56504 */
        if (isdigit((unsigned)pcid_info->opt_chip_name[0]) &&
            atoi(pcid_info->opt_chip_name) ==
            atoi(&soc_chip_type_names[i][3])) {
            found = 1;
            pcid_info->opt_chip = i;
            chip = soc_chip_type_to_index(pcid_info->opt_chip);
            break;
        }
    }

    if (!found) {
        fprintf(stderr,
                "pcid: Unrecognized chip type '%s'\n",
                pcid_info->opt_chip_name);
        fprintf(stderr, "pcid: Valid chip types are:\n");
        for (i = 0; i < SOC_CHIP_TYPES_COUNT; i++) {
            fprintf(stderr, "pcid:  %s\n", soc_chip_type_names[i]);
        }
        exit(1);
    }

    /* 
     * Set initial contents of PCI configuration space.
     */
    chip = soc_chip_type_to_index(pcid_info->opt_chip);
    pci_ven_id = soc_base_driver_table[chip]->pci_vendor;
    pci_dev_id = soc_base_driver_table[chip]->pci_device;
    pci_rev_id = soc_base_driver_table[chip]->pci_revision;
    

    if (pci_dev_id == 0x0) {
        fprintf(stderr,
                "pcid: Not compiled with support for '%s'\n",
                pcid_info->opt_chip_name);
        exit(1);
    }

    /* From here on, we want the effective ID's */
    if (pcid_info->opt_override_devid) {
        pci_dev_id = pcid_info->opt_override_devid;
    }
    if (pcid_info->opt_override_revid) {
        pci_rev_id = pcid_info->opt_override_revid;
    }

    /* Set device and revision */
    *dev_id = pci_dev_id;
    *rev_id = pci_rev_id;

    soc_internal_pcic_init(pcid_info, pci_dev_id, 
                           pci_ven_id, pci_rev_id, 0xcd600000);

    pli_reset_service(pcid_info);

    return 0;
}
#else
void single_mode_dummy_function(void)
{
    /* ISO C forbids an empty source file */
    /* This function is here to handle cases when SINGLE_MODE is not defined */
}
#endif /* !SINGLE_MODE */
