/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <soc/mem.h>
#include <bcm/tsn.h>

void bcm_tsn_sr_port_config_t_init(bcm_tsn_sr_port_config_t *port_config)
{
    if (NULL != port_config) {
        sal_memset(port_config, 0, sizeof(*port_config));
    }
}

void bcm_tsn_pri_map_entry_t_init(bcm_tsn_pri_map_entry_t *entry)
{
    if (NULL != entry) {
        sal_memset(entry, 0, sizeof(*entry));
    }
}

void bcm_tsn_pri_map_config_t_init(bcm_tsn_pri_map_config_t *config)
{
    int ent_idx;
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));

        config->map_type = bcmTsnPriMapTypeCount;
        config->num_map_entries = 0;
        for (ent_idx = 0;
             ent_idx <
             sizeof(config->map_entries) / sizeof(bcm_tsn_pri_map_entry_t);
             ent_idx++) {
            bcm_tsn_pri_map_entry_t_init(&config->map_entries[ent_idx]);
            config->map_entries[ent_idx].new_int_pri = ent_idx;
            config->map_entries[ent_idx].flow_offset = 0;
            config->map_entries[ent_idx].profile_id = -1;
            config->map_entries[ent_idx].taf_gate_express = -1;
            config->map_entries[ent_idx].taf_gate_preempt = -1;
        }
    }
}

void bcm_tsn_sr_tx_flow_config_t_init(bcm_tsn_sr_tx_flow_config_t *flow_config)
{
    if (NULL != flow_config) {
        sal_memset(flow_config, 0, sizeof(*flow_config));
    }
}

void bcm_tsn_sr_rx_flow_config_t_init(bcm_tsn_sr_rx_flow_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_sr_flowset_status_t_init(bcm_tsn_sr_flowset_status_t *status)
{
    if (NULL != status) {
        sal_memset(status, 0, sizeof(*status));
    }
}

void bcm_tsn_sr_tx_flow_status_t_init(bcm_tsn_sr_tx_flow_status_t *status)
{
    if (NULL != status) {
        sal_memset(status, 0, sizeof(*status));
    }
}

void bcm_tsn_sr_rx_flow_status_t_init(bcm_tsn_sr_rx_flow_status_t *status)
{
    if (NULL != status) {
        sal_memset(status, 0, sizeof(*status));
    }
}

void bcm_tsn_flow_config_t_init(bcm_tsn_flow_config_t *flow_config)
{
    if (NULL != flow_config) {
        sal_memset(flow_config, 0, sizeof(*flow_config));
    }
}

void bcm_tsn_flowset_status_t_init(bcm_tsn_flowset_status_t *status)
{
    if (NULL != status) {
        sal_memset(status, 0, sizeof(*status));
    }
}

void bcm_tsn_sr_auto_learn_group_config_t_init(bcm_tsn_sr_auto_learn_group_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_sr_auto_learn_config_t_init(bcm_tsn_sr_auto_learn_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *    bcm_tsn_stat_threshold_config_t_init
 * Purpose:
 *    Intitialize an TSN stat threshold configuration structure
 * Parameters:
 *    config - (OUT) Pointer to TSN stat threshold configuration structure.
 * Returns:
 *    nothing
 */
void
bcm_tsn_stat_threshold_config_t_init(bcm_tsn_stat_threshold_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_event_source_t_init(bcm_tsn_event_source_t *source)
{
    if (NULL != source) {
        sal_memset(source, 0, sizeof(*source));
    }
}

/*
 * Function:
 *      bcm_tsn_mtu_config_t_init
 * Purpose:
 *      Initialize a tsn_mtu_config structure
 * Parameters:
 *      config - pointer to the bcm_tsn_mtu_config_t structure.
 * Returns:
 *      NONE
 */
void bcm_tsn_mtu_config_t_init(bcm_tsn_mtu_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

/*
 * Function:
 *      bcm_tsn_stu_config_t_init
 * Purpose:
 *      Initialize a tsn_stu_config structure
 * Parameters:
 *      config - pointer to the bcm_tsn_stu_config_t structure.
 * Returns:
 *      NONE
 */
void bcm_tsn_stu_config_t_init(bcm_tsn_stu_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_ingress_mtu_config_t_init(bcm_tsn_ingress_mtu_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_ingress_stu_config_t_init(bcm_tsn_ingress_stu_config_t *config)
{
    if (NULL != config) {
        sal_memset(config, 0, sizeof(*config));
    }
}

void bcm_tsn_taf_profile_status_t_init(
         bcm_tsn_taf_profile_status_t *profile_status)
{
    if (profile_status) {
        sal_memset(profile_status, 0, sizeof(bcm_tsn_taf_profile_status_t));
    }
}

void bcm_tsn_taf_profile_t_init(
         bcm_tsn_taf_profile_t *profile)
{
    if (profile) {
        sal_memset(profile, 0, sizeof(bcm_tsn_taf_profile_t));
    }
}
