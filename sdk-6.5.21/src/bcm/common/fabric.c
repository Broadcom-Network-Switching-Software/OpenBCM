/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 *  Fabric functions common to all platforms.
 */

#include <sal/core/libc.h>
#include <bcm/fabric.h>

/* Initialize a fabric predicate info structure. */
void bcm_fabric_predicate_info_t_init(int unit, 
                                      bcm_fabric_predicate_info_t *pred_info)
{
    sal_memset(pred_info, 0x00, sizeof(*pred_info));
}

/* Initialize a fabric action info structure. */
void bcm_fabric_action_info_t_init(int unit, 
                                   bcm_fabric_action_info_t *action_info)
{
    sal_memset(action_info, 0x00, sizeof(*action_info));
    action_info->override_dp = -1;
    action_info->override_ecn = -1;
    action_info->clear_ehv_bit = -1;
    action_info->clear_mp_bit = -1;
    action_info->four_byte_remove_offset = -1;
}

/* Initialize a fabric predicate action info structure */
void bcm_fabric_predicate_action_info_t_init(int unit, 
                     bcm_fabric_predicate_action_info_t *predicate_action_info)
{
    sal_memset(predicate_action_info, 0x00, sizeof(*predicate_action_info));
}

/* Initialize a fabric tdm editing structure. */
void bcm_fabric_tdm_editing_t_init(bcm_fabric_tdm_editing_t *editing)
{
    sal_memset(editing, 0x00, sizeof(bcm_fabric_tdm_editing_t));
}

/* Initialize a fabric route structure. */
void bcm_fabric_route_t_init(bcm_fabric_route_t *fabric_route)
{
    sal_memset(fabric_route, 0x00, sizeof(bcm_fabric_route_t));
    fabric_route->pipe_id = -1;
}

/* Initialize a fabric remote pipe mapping structure. */
void bcm_fabric_link_remote_pipe_mapping_t_init(bcm_fabric_link_remote_pipe_mapping_t *pipe_mapping)
{
    sal_memset(pipe_mapping, 0x00, sizeof(bcm_fabric_link_remote_pipe_mapping_t));
}

/* Initialize a fabric rci config structure. */
void bcm_fabric_rci_config_t_init(bcm_fabric_rci_config_t *rci_config)
{
    int i;
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_LEVELS; ++i) {
        rci_config->rci_core_level_thresholds[i] = -1; 
        rci_config->rci_device_level_thresholds[i] = -1;
    }
    for (i = 0; i < BCM_FABRIC_NUM_OF_RCI_SEVERITIES; ++i) {
        rci_config->rci_severity_factors[i] = -1;
    }
    rci_config->rci_high_score_fabric_rx_queue = -1;
    rci_config->rci_high_score_fabric_rx_local_queue = -1;
    rci_config->rci_threshold_num_of_congested_links = -1;
    rci_config->rci_high_score_congested_links = -1;
}

