/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SUM - Switch Utilization Monitor Embedded Application APP interface
 * Purpose: API to Initialize SUM embedded app Structures.
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/sum.h>

/* Initialize a sum config information structure. */
void bcm_sum_config_t_init(bcm_sum_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_sum_config_t));
}

/* Initialize a sum stats information structure. */
void bcm_sum_stat_info_t_init(bcm_sum_stat_info_t *stat_info)
{
    sal_memset(stat_info, 0, sizeof(bcm_sum_stat_info_t));
}
