/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    lb.c
 * Purpose: Manages common LB functions
 */

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/debug.h>

#include <bcm/lb.h>


/* Initialize structure bcm_lb_rx_modem_map_index_t */
void bcm_lb_rx_modem_map_index_t_init(
    bcm_lb_rx_modem_map_index_t *map_index)
{
    if (map_index != NULL) {
        sal_memset(map_index, 0, sizeof(*map_index));
    }
}

/* Initialize structure bcm_lb_rx_modem_map_config_t */
void bcm_lb_rx_modem_map_config_t_init(
    bcm_lb_rx_modem_map_config_t *map_config)
{
    if (map_config != NULL) {
        sal_memset(map_config, 0, sizeof(*map_config));
    }
}

/* Initialize structure bcm_lb_packet_config_t */
void bcm_lb_packet_config_t_init(
    bcm_lb_packet_config_t *packet_config)
{
    if (packet_config != NULL) {
        sal_memset(packet_config, 0, sizeof(*packet_config));
    }
}

/* Initialize structure bcm_lb_modem_packet_config_t */
void bcm_lb_modem_packet_config_t_init(
    bcm_lb_modem_packet_config_t *packet_config)
{
    if (packet_config != NULL) {
        sal_memset(packet_config, 0, sizeof(*packet_config));
    }
}

/* Initialize structure bcm_lb_tc_dp_t */
void bcm_lb_tc_dp_t_init(
    bcm_lb_tc_dp_t *tc_dp)
{
    if (tc_dp != NULL) {
        sal_memset(tc_dp, 0, sizeof(*tc_dp));
    }
}

/* Initialize structure bcm_lb_pkt_pri_t */
void bcm_lb_pkt_pri_t_init(
    bcm_lb_pkt_pri_t *pkt_pri)
{
    if (pkt_pri != NULL) {
        sal_memset(pkt_pri, 0, sizeof(*pkt_pri));
    }
}

/* Initialize structure bcm_lb_lbg_weight_t */
void bcm_lb_lbg_weight_t_init(
    bcm_lb_lbg_weight_t *lbg_weights)
{
    if (lbg_weights != NULL) {
        sal_memset(lbg_weights, 0, sizeof(*lbg_weights));
    }
}

/* Initialize structure bcm_lb_modem_shaper_config_t */
void bcm_lb_modem_shaper_config_t_init(
    bcm_lb_modem_shaper_config_t *shaper)
{
    if (shaper != NULL) {
        sal_memset(shaper, 0, sizeof(*shaper));
    }
}

