/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    oob.c
 * Purpose: Manages common OOB functions
 */

#include <sal/core/libc.h>
#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/oob.h>

/*
 * Function:
 *      bcm_oob_fc_tx_config_t_init
 * Purpose:
 *      Initialize the OOB FC Tx Global Config Structure
 * Parameters:
 *      config - OOB FC Tx Global Config Structure to be init'ed
 */

void
bcm_oob_fc_tx_config_t_init(bcm_oob_fc_tx_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *      bcm_oob_fc_tx_info_t_init
 * Purpose:
 *      Initialize the OOB FC Tx Global Info Structure
 * Parameters:
 *      info - OOB FC Tx Global Info Structure to be init'ed
 */

void
bcm_oob_fc_tx_info_t_init(bcm_oob_fc_tx_info_t *info)
{
    if (info != NULL) {
        sal_memset(info, 0, sizeof(*info));
    }
}

/*
 * Function:
 *      bcm_oob_fc_rx_config_t_init
 * Purpose:
 *      Initialize the OOB FC Rx Global Config Structure
 * Parameters:
 *      config - OOB FC Rx Global Config Structure to be init'ed
 */

void
bcm_oob_fc_rx_config_t_init(bcm_oob_fc_rx_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *      bcm_oob_stats_config_t_init
 * Purpose:
 *      Initialize the OOB Stats Global Config Structure
 * Parameters:
 *      config - OOB Stats Global Config Structure to be init'ed
 */

void
bcm_oob_stats_config_t_init(bcm_oob_stats_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *      bcm_oob_fc_tx_queue_config_t_init
 * Purpose:
 *      Initialize the OOB Queue config structure.
 * Parameters:
 *      config - OOB Stats Queue Config Structure to be init'ed
 */

void bcm_oob_fc_tx_queue_config_t_init(
        bcm_oob_fc_tx_queue_config_t *config)
{
    if (config != NULL) {
        sal_memset(config, 0, sizeof(bcm_oob_fc_tx_queue_config_t));
    }
}

