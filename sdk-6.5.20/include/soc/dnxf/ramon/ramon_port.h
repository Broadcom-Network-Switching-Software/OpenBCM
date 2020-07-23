/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RAMON PORT H
 */
 
#ifndef _SOC_RAMON_PORT_H_
#define _SOC_RAMON_PORT_H_

#ifndef BCM_DNXF_SUPPORT 
#error "This file is for use by DNXF (Ramon) family only!" 
#endif 





#include <soc/error.h>
#include <soc/types.h>

#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxf/cmn/dnxf_port.h>
#include <soc/dnxf/cmn/dnxf_defs.h>






#define SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE1          (12)
#define SOC_RAMON_PORT_COMMA_BURST_SIZE_FE1            (2)
#define SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE2          (12)
#define SOC_RAMON_PORT_COMMA_BURST_SIZE_FE2            (1)
#define SOC_RAMON_PORT_COMMA_BURST_PERIOD_FE3          (13)
#define SOC_RAMON_PORT_COMMA_BURST_SIZE_FE3            (1)
#define SOC_RAMON_PORT_COMMA_BURST_PERIOD_MAC_LOOPBACK (8)
#define SOC_RAMON_PORT_COMMA_BURST_SIZE_MAC_LOOPBACK   (3)


#define SOC_RAMON_PORT_SYNC_E_MIN_DIVIDER              (2)
#define SOC_RAMON_PORT_SYNC_E_MAX_DIVIDER              (16)


#define SOC_RAMON_PORT_PUC_LCPLL_IN_SEL     (8)
#define SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL  (10)
#define SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL  (12)
#define SOC_RAMON_PORT_PUC_CORE_PLL_N_DIV   (14)

#define SOC_RAMON_PORT_PUC_DEF_CORE_CLOCK_OVRD_SEL          (2)
#define SOC_RAMON_PORT_PUC_DEF_CORE_CLOCK_OVRD_NOF_BITS     (1)

#define SOC_RAMON_PORT_PUC_CORE_PLL_N_DIVIDER_SEL           (14)
#define SOC_RAMON_PORT_PUC_CORE_PLL_N_DIVIDER_NOF_BITS      (10)

#define SOC_RAMON_PORT_PUC_CORE_PLL_M_DIVIDER_SEL           (24)
#define SOC_RAMON_PORT_PUC_CORE_PLL_M_DIVIDER_NOF_BITS      (10)

#define SOC_RAMON_PORT_PUC_LCPLL_IN_SEL_NOF_BITS    (SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL - SOC_RAMON_PORT_PUC_LCPLL_IN_SEL)
#define SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL_NOF_BITS (SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL - SOC_RAMON_PORT_PUC_LCPLL01_OUT_SEL)
#define SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL_NOF_BITS (SOC_RAMON_PORT_PUC_CORE_PLL_N_DIV - SOC_RAMON_PORT_PUC_LCPLL23_OUT_SEL)


#define SOC_RAMON_PORT_PUC_LCPLL_IN_125_MHZ         (0)
#define SOC_RAMON_PORT_PUC_LCPLL_IN_156_25_MHZ      (1)
#define SOC_RAMON_PORT_PUC_LCPLL_IN_312_5_MHZ       (2)
#define SOC_RAMON_PORT_PUC_LCPLL_IN_DISABLE_POWERUP (3)


#define SOC_RAMON_PORT_PUC_LCPLL_OUT_312_5_MHZ     (0)
#define SOC_RAMON_PORT_PUC_LCPLL_OUT_156_25_MHZ    (1)
#define SOC_RAMON_PORT_PUC_LCPLL_OUT_125_MHZ       (2)
#define SOC_RAMON_PORT_PUC_LCPLL_OUT_BYPASS        (3)

#define SOC_RAMON_PORT_PAM4_NOF_LINK_UP_CHECK_ITERATIONS       (18)
#define SOC_RAMON_PORT_LINK_UP_CHECK_INTERVAL_SEC              (1)





shr_error_e soc_ramon_port_soc_init(int unit);
shr_error_e soc_ramon_port_speed_max(int unit, soc_port_t port, int *speed);
shr_error_e soc_ramon_port_init(int unit);
shr_error_e soc_ramon_port_deinit(int unit);
shr_error_e soc_ramon_port_detach(int unit, soc_port_t port);
shr_error_e soc_ramon_port_probe(int unit, pbmp_t pbmp, pbmp_t *okay_pbmp, int is_init_sequence);
shr_error_e soc_ramon_port_burst_control_set(int unit, soc_port_t port, portmod_loopback_mode_t loopback);
shr_error_e soc_ramon_port_serdes_power_disable(int unit , soc_pbmp_t disable_pbmp);
shr_error_e soc_ramon_port_link_status_get(int unit, soc_port_t port, int *link_up);
shr_error_e soc_ramon_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link);
shr_error_e soc_ramon_port_fsrd_block_power_set(int unit, soc_port_t port, int power_up);
shr_error_e soc_ramon_port_update_valid_block_database(int unit, int block_type, int block_number, int enable);
shr_error_e soc_ramon_port_pump_enable_set(int unit, soc_port_t port, int enable);
shr_error_e soc_ramon_port_dynamic_soc_init(int unit, soc_port_t port, int is_first_link);
shr_error_e soc_ramon_port_sync_e_link_set(int unit, int is_master, int port);
shr_error_e soc_ramon_port_sync_e_divider_set(int unit, int is_master, int divider);
shr_error_e soc_ramon_port_sync_e_link_get(int unit, int is_master, int *port);
shr_error_e soc_ramon_port_sync_e_divider_get(int unit, int is_master, int *divider);
shr_error_e soc_ramon_port_fault_get(int unit, soc_port_t port, uint32* flags);
shr_error_e soc_ramon_port_bucket_fill_rate_validate(int unit, uint32 bucket_fill_rate);
shr_error_e soc_ramon_port_dynamic_port_update(int unit, int port, int enable, soc_dnxf_port_update_type_t type);
shr_error_e soc_ramon_port_enable_set(int unit, soc_port_t port, int enable);
shr_error_e soc_ramon_port_stat_gtimer_config_set(int unit, soc_port_t port, soc_dnxc_port_stat_gtimer_config_t *gtimer_config);
shr_error_e soc_ramon_port_stat_gtimer_config_get(int unit, soc_port_t port, soc_dnxc_port_stat_gtimer_config_t *gtimer_config);
shr_error_e soc_ramon_port_link_up_check(int unit, soc_pbmp_t port_bmp);
shr_error_e soc_ramon_port_enabled_fmac_blocks_get(int unit, soc_pbmp_t * active_fmacs_bmp);
#endif

