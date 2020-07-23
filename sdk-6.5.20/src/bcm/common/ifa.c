/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * IFA - In-band Flow Analyzer Monitoring Embedded Application APP interface
 * Purpose: API to Initialize IFA embedded app Structures.
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/ifa.h>

/* Initialize a ifa collector information structure. */
void bcm_ifa_collector_info_t_init(bcm_ifa_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_ifa_collector_info_t));
}

/* Initialize a ifa config information structure. */
void bcm_ifa_config_info_t_init(bcm_ifa_config_info_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_ifa_config_info_t));
}

/* Initialize a ifa cc config information structure. */
void bcm_ifa_cc_config_t_init(bcm_ifa_cc_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_ifa_cc_config_t));
}

/* Initialize a ifa leap config information structure. */
void bcm_ifa_template_transmit_config_t_init(bcm_ifa_template_transmit_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_ifa_template_transmit_config_t));
}

/* Initialize the bcm_ifa_header_t struct. */
void bcm_ifa_header_t_init(bcm_ifa_header_t *header)
{
    sal_memset(header, 0, sizeof(bcm_ifa_header_t));
}

