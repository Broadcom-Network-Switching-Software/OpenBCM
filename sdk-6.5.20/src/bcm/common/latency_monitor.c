/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * File:    latency_monitor.c
 * Purpose: Common latency_monitor APIs
 */

#include <soc/defs.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>

#include <bcm/latency_monitor.h>

/*
 * Function:
 *      bcm_latency_monitor_config_t_init
 * Purpose:
 *      Initialize a bcm_latency_monitor_config_t structure
 * Parameters:
 *      config - pointer to bcm_latency_monitor_config_t
 * Returns:
 *      NONE
 */

void
bcm_latency_monitor_config_t_init(bcm_latency_monitor_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof (*config));
    }
    return;
}

/*
 * Function:
 *      bcm_latency_monitor_cosq_info_t_init
 * Purpose:
 *      Initialize a bcm_latency_monitor_cosq_info_t structure
 * Parameters:
 *      config - pointer to bcm_latency_monitor_cosq_info_t
 * Returns:
 *      NONE
 */

void
bcm_latency_monitor_cosq_info_t_init(bcm_latency_monitor_cosq_info_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof (*config));
    }
    return;
}

/*
 * Function:
 *      bcm_latency_monitor_histogram_t_init
 * Purpose:
 *      Initialize a bcm_latency_monitor_histogram_t structure
 * Parameters:
 *      config - pointer to bcm_latency_monitor_histogram_t
 * Returns:
 *      NONE
 */

void
bcm_latency_monitor_histogram_t_init(bcm_latency_monitor_histogram_t *histogram)
{
    if (NULL != histogram) {
        sal_memset(histogram, 0, sizeof (*histogram));
    }
    return;
}


/*
 * Function:
 *      bcm_latency_monitor_info_t_init
 * Purpose:
 *      Initialize a bcm_latency_monitor_info_t_init structure
 * Parameters:
 *      config - pointer to bcm_latency_monitor_info_t
 * Returns:
 *      NONE
 */

void
bcm_latency_monitor_info_t_init(bcm_latency_monitor_info_t *monitor_info)
{
    if (NULL != monitor_info) {
        sal_memset(monitor_info, 0, sizeof (*monitor_info));
    }
    return;
}

