/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/collector.h>

/* Initialize a collector information structure. */
void bcm_collector_info_t_init(bcm_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_collector_info_t));
    collector_info->max_records_reserve = 16;
}

/* Initialize a collector export profile information structure. */
void bcm_collector_export_profile_t_init(bcm_collector_export_profile_t *export_profile_info)
{
    sal_memset(export_profile_info, 0, sizeof(bcm_collector_export_profile_t));
    /* 
     * Value 0 is valid from now onwards for export interval. 
     * So update to a new invalid value. 
     */
    export_profile_info->export_interval = BCM_COLLECTOR_EXPORT_INTERVAL_INVALID;
}

/* Initialize a collector callback options structure. */
void bcm_collector_callback_options_t_init(
    bcm_collector_callback_options_t *callback_options)
{
    sal_memset(callback_options, 0, sizeof(bcm_collector_callback_options_t));
}

/* Initialize a export record structure. */
void bcm_collector_export_record_t_init(
    bcm_collector_export_record_t *record)
{
    sal_memset(record, 0, sizeof(bcm_collector_export_record_t));
}


