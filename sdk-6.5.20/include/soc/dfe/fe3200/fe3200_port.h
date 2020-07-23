/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FE3200 PORT H
 */
 
#ifndef _SOC_FE3200_PORT_H_
#define _SOC_FE3200_PORT_H_





#include <soc/error.h>
#include <soc/types.h>

#include <soc/dcmn/dcmn_defs.h>
#include <soc/dcmn/dcmn_port.h>

#include <soc/dfe/cmn/dfe_defs.h>






#define SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE1          (12)
#define SOC_FE3200_PORT_COMMA_BURST_SIZE_FE1            (2)
#define SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE2          (12)
#define SOC_FE3200_PORT_COMMA_BURST_SIZE_FE2            (1)
#define SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE3          (13)
#define SOC_FE3200_PORT_COMMA_BURST_SIZE_FE3            (1)
#define SOC_FE3200_PORT_COMMA_BURST_PERIOD_REPEATER     (SOC_FE3200_PORT_COMMA_BURST_PERIOD_FE2)
#define SOC_FE3200_PORT_COMMA_BURST_SIZE_REPEATER       (SOC_FE3200_PORT_COMMA_BURST_SIZE_FE2)
#define SOC_FE3200_PORT_COMMA_BURST_PERIOD_MAC_LOOPBACK (8)
#define SOC_FE3200_PORT_COMMA_BURST_SIZE_MAC_LOOPBACK   (3)


#define SOC_FE3200_PORT_SYNC_E_MIN_DIVIDER              (2)
#define SOC_FE3200_PORT_SYNC_E_MAX_DIVIDER              (16)


#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_2               (0)
#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_3               (0x40200000)
#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_4               (0x01020030)
#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE1        (0x8)
#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE2        (0x48)
#define SOC_FE3200_PORT_PLL_CONFIG_DEFAULT_WORD_5_STAGE3        (0x68)

#define SOC_FE3200_PORT_PLL_CONFIG_OUT_125_MHZ_WORD_0           (0x51419)

#define SOC_FE3200_PORT_PLL_CONFIG_IN_125_MHZ_WORD_1            (0xc8041A40)

#define SOC_FE3200_PORT_PLL_CONFIG_OUT_156_25_MHZ_WORD_0        (0x51414)

#define SOC_FE3200_PORT_PLL_CONFIG_IN_156_25_MHZ_WORD_1         (0xA0041A40)


#define SOC_FE3200_PORT_ECI_POWER_UP_CONFIG_STATIC_PLL_BIT      (8)







soc_error_t soc_fe3200_port_soc_init(int unit);
soc_error_t soc_fe3200_port_speed_max(int unit, soc_port_t port, int *speed);
soc_error_t soc_fe3200_port_init(int unit);
soc_error_t soc_fe3200_port_deinit(int unit);
soc_error_t soc_fe3200_port_detach(int unit, soc_port_t port);
soc_error_t soc_fe3200_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
soc_error_t soc_fe3200_port_phy_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe3200_port_phy_enable_get(int unit, soc_port_t port, int *enable);
soc_error_t soc_fe3200_port_burst_control_set(int unit, soc_port_t port, soc_dcmn_loopback_mode_t loopback);
soc_error_t soc_fe3200_port_serdes_power_disable(int unit , soc_pbmp_t disable_pbmp);
soc_error_t soc_fe3200_port_link_status_get(int unit , soc_port_t port, int *link_up);
soc_error_t soc_fe3200_port_speed_set(int unit, soc_port_t port, int speed);
soc_error_t soc_fe3200_port_interface_set(int unit, soc_port_t port, soc_port_if_t intf);
soc_error_t soc_fe3200_port_interface_get(int unit, soc_port_t port, soc_port_if_t* intf);
soc_error_t soc_fe3200_port_speed_get(int unit, soc_port_t port, int *speed);
soc_error_t soc_fe3200_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link, int enable);
soc_error_t soc_fe3200_port_fsrd_block_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe3200_port_update_valid_block_database(int unit, int block_type, int block_number, int enable);
soc_error_t soc_fe3200_port_pump_enable_set(int unit, soc_port_t port, int enable);
soc_error_t soc_fe3200_port_dynamic_soc_init(int unit, soc_port_t port, int is_first_link);
soc_error_t soc_fe3200_port_sync_e_link_set(int unit, int is_master, int port);
soc_error_t soc_fe3200_port_sync_e_divider_set(int unit, int divider);
soc_error_t soc_fe3200_port_sync_e_link_get(int unit, int is_master, int *port);
soc_error_t soc_fe3200_port_sync_e_divider_get(int unit, int *divider);
soc_error_t soc_fe3200_port_quad_disabled(int unit, int quad, int *disabled);


#endif

