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
#include <bcm/flowtracker.h>


/* Initialize a flowtracker collector information structure. */
void bcm_flowtracker_collector_info_t_init(bcm_flowtracker_collector_info_t *collector_info)
{
    sal_memset(collector_info, 0, sizeof(bcm_flowtracker_collector_info_t));
}


/* Initialize a flowtracker flow group information structure. */
void bcm_flowtracker_group_info_t_init(
    bcm_flowtracker_group_info_t *flow_group_info)
{
    sal_memset(flow_group_info, 0, sizeof(bcm_flowtracker_group_info_t));
}

/* Initialize flowtracker template transmit configuration. */
void bcm_flowtracker_template_transmit_config_t_init(
                              bcm_flowtracker_template_transmit_config_t *config)
{
    sal_memset(config, 0, sizeof(bcm_flowtracker_template_transmit_config_t));
}

/* Initialize flow key. */
void bcm_flowtracker_flow_key_t_init(bcm_flowtracker_flow_key_t *flow_key)
{
    sal_memset(flow_key, 0, sizeof(bcm_flowtracker_flow_key_t));
}

/* Initialize a flowtracker group action info structure. */
void bcm_flowtracker_group_action_info_t_init(
                                bcm_flowtracker_group_action_info_t *action_info)
{
    sal_memset(action_info, 0, sizeof(bcm_flowtracker_group_action_info_t));
}

/* Initialize a flowtracker elephant profile info structure. */
void bcm_flowtracker_elephant_profile_info_t_init(
                               bcm_flowtracker_elephant_profile_info_t *profile)
{
    sal_memset(profile, 0, sizeof(bcm_flowtracker_elephant_profile_info_t));
}

/* Initialize a flowtracker check information structure. */
void bcm_flowtracker_check_info_t_init(
    bcm_flowtracker_check_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_check_info_t));
}

/* Initialize a flowtracker collector copy information structure. */
void bcm_flowtracker_collector_copy_info_t_init(
    bcm_flowtracker_collector_copy_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_collector_copy_info_t));
}

/* Initialize a flowtracker meter information structure. */
void bcm_flowtracker_meter_info_t_init(
    bcm_flowtracker_meter_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_meter_info_t));
}

/* Initialize a flowtracker check action information structure. */
void bcm_flowtracker_check_action_info_t_init(
    bcm_flowtracker_check_action_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_check_action_info_t));
    info->weight = bcmFlowtrackerTrackingParamWeight4;
}

/* Initialize a flowtracker check export information structure. */
void bcm_flowtracker_check_export_info_t_init(
    bcm_flowtracker_check_export_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_check_export_info_t));
}

/* Initialize a flowtracker check delay information structure. */
void bcm_flowtracker_check_delay_info_t_init(
    bcm_flowtracker_check_delay_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_check_delay_info_t));
}

/* Initialize a flowtracker tracking parameter information structure. */
void bcm_flowtracker_tracking_param_info_t_init(
    bcm_flowtracker_tracking_param_info_t *info)
{

    sal_memset(info, 0, sizeof(bcm_flowtracker_tracking_param_info_t));
    _shr_ip6_mask_create(info->mask, BCM_FLOWTRACKER_TRACKING_PARAM_MASK_LEN);

}

/*  Initialize a flowtracker user entry information structure. */
void bcm_flowtracker_user_entry_handle_t_init(
    bcm_flowtracker_user_entry_handle_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_user_entry_handle_t));
}

/* Initialize a flowtracker group statistics structure. */
void bcm_flowtracker_group_stat_t_init(
    bcm_flowtracker_group_stat_t *group_stats)
{
    sal_memset(group_stats, 0, sizeof(bcm_flowtracker_group_stat_t));
}

/* Initialize a flowtracker callback options structure. */
void bcm_flowtracker_collector_callback_options_t_init(
    bcm_flowtracker_collector_callback_options_t *callback_options)
{
    sal_memset(callback_options, 0, sizeof(bcm_flowtracker_collector_callback_options_t));
}

/* Initialize a flowtracker export record structure. */
void bcm_flowtracker_export_record_t_init(
    bcm_flowtracker_export_record_t *record)
{
    sal_memset(record, 0, sizeof(bcm_flowtracker_export_record_t));
}


/* Initialize a flowtracker export trigger information structure, */
void bcm_flowtracker_export_trigger_info_t_init(
    bcm_flowtracker_export_trigger_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_export_trigger_info_t));
}

/* Initialize a flowtracker export trigger information structure, */
void bcm_flowtracker_chip_debug_info_t_init(
    bcm_flowtracker_chip_debug_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_chip_debug_info_t));
}

/* Initialize elephant match data. */
void bcm_flowtracker_elephant_match_data_t_init(
                               bcm_flowtracker_elephant_match_data_t *match_data)
{
    sal_memset(match_data, 0, sizeof(bcm_flowtracker_elephant_match_data_t));
}

/* Initialize elephant stats */
void bcm_flowtracker_elephant_stats_t_init(bcm_flowtracker_elephant_stats_t *stats)
{
    sal_memset(stats, 0, sizeof(bcm_flowtracker_elephant_stats_t));
}

/* Initialize flowtracker aggregate class info. */
void bcm_flowtracker_aggregate_class_info_t_init(
        bcm_flowtracker_aggregate_class_info_t *aggregate_class_info)
{
    sal_memset(aggregate_class_info, 0,
            sizeof(bcm_flowtracker_aggregate_class_info_t));
}

/* Initialize flowtracker periodic export config. */
void bcm_flowtracker_periodic_export_config_t_init(
        bcm_flowtracker_periodic_export_config_t *config)
{
    sal_memset(config, 0,
            sizeof(bcm_flowtracker_periodic_export_config_t));
    config->flags = BCM_FLOWTRACKER_MICRO_PERIODIC_EXPORT_GUARANTEE;
}

/* Initialize flowtracker group record info. */
void bcm_flowtracker_group_record_info_t_init(
        bcm_flowtracker_group_record_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_group_record_info_t));
}

/* Initialize flowtracker export record data info. */
void bcm_flowtracker_export_record_data_info_t_init(
        bcm_flowtracker_export_record_data_info_t *data_info)
{
    sal_memset(data_info, 0,
        sizeof(bcm_flowtracker_export_record_data_info_t));
}

/* Initialize flowtracker udf info. */
void bcm_flowtracker_udf_info_t_init(
        bcm_flowtracker_udf_info_t *info)
{
    sal_memset(info, 0, sizeof(bcm_flowtracker_udf_info_t));
}
