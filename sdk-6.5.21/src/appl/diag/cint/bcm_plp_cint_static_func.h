/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sdk_config.h>

#if defined(INCLUDE_LIB_CINT)
 
#include <cint_config.h>
#include <cint_types.h>
#include <cint_porting.h>
#include <bcm/types.h>

#if defined(PHYMOD_EPDM_APIS_SUPPORT)

#include <phymod/phymod.h>
#include <phymod/phymod_types.h>
#include "epdm.h"
#include "epdm_sec.h"
#include "epdm_bcm_common_defines.h"
#include "epdm_bcm_common_sec_defines.h"

static int __macro__bcm_plp_static_config_set(char* chip_name, bcm_plp_access_t phy_info, void* bcm_static_config)
{
       return PLP_CALL((chip_name), __static_config_set, ((phy_info), (bcm_static_config)));
}

static int __macro__bcm_plp_static_config_get(char* chip_name, bcm_plp_access_t phy_info, void* bcm_static_config)
{
       return PLP_CALL((chip_name), __static_config_get, ((phy_info), (bcm_static_config)));
}

static int __macro__bcm_plp_init(char* chip_name, bcm_plp_access_t phy_info, phymod_bus_read_f read, phymod_bus_write_f write,
                                 bcm_plp_pm_firmware_load_method_t firmware_load_method)
{
        return PLP_CALL((chip_name), __init, ((phy_info), (read), (write), (firmware_load_method)));
}

static int __macro__bcm_plp_init_fw_bcast(char* chip_name, bcm_plp_access_t phy_info, phymod_bus_read_f read, phymod_bus_write_f write,
                                 bcm_plp_firmware_load_type_t *firmware_load_type, bcm_plp_firmware_broadcast_method_t broadcast_method)
{
        return PLP_CALL((chip_name), __init_fw_bcast, ((phy_info), (read), (write), (firmware_load_type), (broadcast_method)));
}

static int __macro__bcm_plp_cleanup(char* chip_name, bcm_plp_access_t phy_info)
{
       return PLP_CALL((chip_name), __cleanup, ((phy_info)));
}

static int __macro__bcm_plp_link_status_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* link_status)
{
       return PLP_CALL((chip_name), __link_status_get, ((phy_info), (link_status)));
}

static int __macro__bcm_plp_mode_config_set(char* chip_name, bcm_plp_access_t phy_info, int speed, int if_type,\
                                            int ref_clk, int interface_mode, void* device_aux_modes) 
{
       return PLP_CALL((chip_name), __mode_config_set, ((phy_info), (speed), (if_type), (ref_clk), (interface_mode), (device_aux_modes)));
}

static int __macro__bcm_plp_mode_config_get(char* chip_name, bcm_plp_access_t phy_info, int* speed, int* if_type,\
                                            int* ref_clk, int* interface_mode, void* device_aux_modes)
{
       return PLP_CALL((chip_name), __mode_config_get, ((phy_info), (speed), (if_type), (ref_clk), (interface_mode), (device_aux_modes)));
}

static int __macro__bcm_plp_driver_version_get(char* chip_name, bcm_plp_access_t phy_info, char *phy_chip_name,\
                                               unsigned short *major_ver, unsigned short *minor_ver)
{
       return PLP_CALL((chip_name), __driver_version_get, ((phy_info), (phy_chip_name), (major_ver), (minor_ver)));
}

static int __macro__bcm_plp_prbs_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int tx_rx, unsigned int poly,\
                                     unsigned int invert, unsigned int loopback, unsigned int ena_dis)
{
       return PLP_CALL((chip_name), __prbs_set, ((phy_info), (tx_rx), (poly), (invert), (loopback), (ena_dis)));
}

static int __macro__bcm_plp_prbs_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int tx_rx, unsigned int* poly,\
                                     unsigned int* invert, unsigned int* loopback, unsigned int* ena_dis)
{
       return PLP_CALL((chip_name), __prbs_get, ((phy_info), (tx_rx), (poly), (invert), (loopback), (ena_dis)));
}

static int __macro__bcm_plp_prbs_rx_stat(char* chip_name, bcm_plp_access_t phy_info, unsigned int time)
{
       return PLP_CALL((chip_name), __prbs_rx_stat, ((phy_info), (time)));
}

static int __macro__bcm_plp_prbs_clear(char* chip_name, bcm_plp_access_t phy_info, unsigned int tx_rx)
{
       return PLP_CALL((chip_name), __prbs_clear, ((phy_info), (tx_rx)));
}

static int __macro__bcm_plp_prbs_config_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int tx_rx,\
                                            unsigned int *poly, unsigned int *invert) 
{
       return PLP_CALL((chip_name), __prbs_config_get, ((phy_info), (tx_rx), (poly), (invert)));
}

static int __macro__bcm_plp_prbs_status_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int *prbs_lock,\
                                            unsigned int *prbs_lock_loss, unsigned int *error_count)
{
       return PLP_CALL((chip_name), __prbs_status_get, ((phy_info), (prbs_lock), (prbs_lock_loss), (error_count)));
}

static int __macro__bcm_plp_reg_value_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int devaddr,\
                                            unsigned int regaddr, unsigned int data)
{
       return PLP_CALL((chip_name), __reg_value_set, ((phy_info), (devaddr), (regaddr), (data)));
}

static int __macro__bcm_plp_reg_value_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int devaddr,\
                                            unsigned int regaddr, unsigned int* data)
{
       return PLP_CALL((chip_name), __reg_value_get, ((phy_info), (devaddr), (regaddr), (data)));
}

static int __macro__bcm_plp_polarity_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int tx_pol,\
                                         unsigned int rx_pol)
{
       return PLP_CALL((chip_name), __polarity_set, ((phy_info), (tx_pol), (rx_pol)));
}

static int __macro__bcm_plp_polarity_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* tx_pol,\
                                         unsigned int* rx_pol)
{
       return PLP_CALL((chip_name), __polarity_get, ((phy_info), (tx_pol), (rx_pol)));
}

static int __macro__bcm_plp_rx_pmd_lock_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* rx_pmd_lock) 
{
       return PLP_CALL((chip_name), __rx_pmd_lock_get, ((phy_info), (rx_pmd_lock)));
}

static int __macro__bcm_plp_rev_id(char* chip_name, bcm_plp_access_t phy_info, unsigned int* rev_id)
{
       return PLP_CALL((chip_name), __rev_id, ((phy_info), (rev_id)));
}

static int __macro__bcm_plp_loopback_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int lb_mode,\
                                         unsigned int enable)
{
       return PLP_CALL((chip_name), __loopback_set, ((phy_info), (lb_mode), (enable)));
}

static int __macro__bcm_plp_loopback_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int lb_mode,\
                                         unsigned int* enable)
{
       return PLP_CALL((chip_name), __loopback_get, ((phy_info), (lb_mode), (enable)));
}

static int __macro__bcm_plp_tx_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_tx_t* tx)
{
       return PLP_CALL((chip_name), __tx_set, ((phy_info), (tx)));
}

static int __macro__bcm_plp_tx_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_tx_t* tx)
{
       return PLP_CALL((chip_name), __tx_get, ((phy_info), (tx)));
}

static int __macro__bcm_plp_rx_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_rx_t* rx)
{
       return PLP_CALL((chip_name), __rx_set, ((phy_info), (rx)));
}

static int __macro__bcm_plp_rx_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_rx_t* rx)
{
       return PLP_CALL((chip_name), __rx_get, ((phy_info), (rx)));
}

static int __macro__bcm_plp_reset_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int reset_mode,\
                                         unsigned int reset_val)
{
       return PLP_CALL((chip_name), __reset_set, ((phy_info), (reset_mode), (reset_val)));
}

static int __macro__bcm_plp_phy_lane_reset_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_reset_t* reset)
{
       return PLP_CALL((chip_name), __phy_lane_reset_set, ((phy_info), (reset)));
}

static int __macro__bcm_plp_phy_lane_reset_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_reset_t* reset)
{
       return PLP_CALL((chip_name), __phy_lane_reset_get, ((phy_info), (reset)));
}

static int __macro__bcm_plp_tx_lane_control_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_tx_lane_control_t tx_control)
{
       return PLP_CALL((chip_name), __tx_lane_control_set, ((phy_info), (tx_control)));
}

static int __macro__bcm_plp_tx_lane_control_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_tx_lane_control_t* tx_control)
{
       return PLP_CALL((chip_name), __tx_lane_control_get, ((phy_info), (tx_control)));
}

static int __macro__bcm_plp_rx_lane_control_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_rx_lane_control_t rx_control)
{
       return PLP_CALL((chip_name), __rx_lane_control_set, ((phy_info), (rx_control)));
}

static int __macro__bcm_plp_rx_lane_control_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_rx_lane_control_t* rx_control)
{
       return PLP_CALL((chip_name), __rx_lane_control_get, ((phy_info), (rx_control)));
}

static int __macro__bcm_plp_force_tx_training_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int enable)
{
       return PLP_CALL((chip_name), __force_tx_training_set, ((phy_info), (enable)));
}

static int __macro__bcm_plp_force_tx_training_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* enable)
{
       return PLP_CALL((chip_name), __force_tx_training_get, ((phy_info), (enable)));
}

static int __macro__bcm_plp_force_tx_training_status_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* enabled,\
                                                         unsigned int *training_failure, unsigned int *trained)
{
       return PLP_CALL((chip_name), __force_tx_training_status_get, ((phy_info), (enabled), (training_failure), (trained)));
}

static int __macro__bcm_plp_cl73_ability_set(char* chip_name, bcm_plp_access_t phy_info, unsigned short tech_ability,\
                                             unsigned short fec_ability, unsigned short pause_ability, bcm_plp_an_config_t an_config)
{
       return PLP_CALL((chip_name), __cl73_ability_set, ((phy_info), (tech_ability), (fec_ability), (pause_ability), (an_config)));
}

static int __macro__bcm_plp_cl73_ability_get(char* chip_name, bcm_plp_access_t phy_info, unsigned short* tech_ability,\
                                             unsigned short* fec_ability, unsigned short* pause_ability, bcm_plp_an_config_t* an_config)
{
       return PLP_CALL((chip_name), __cl73_ability_get, ((phy_info), (tech_ability), (fec_ability), (pause_ability), (an_config)));
}

static int __macro__bcm_plp_cl73_set(char* chip_name, bcm_plp_access_t phy_info, unsigned short ena_dis)
{
       return PLP_CALL((chip_name), __cl73_set, ((phy_info), (ena_dis)));
}

static int __macro__bcm_plp_cl73_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* an,\
                                         unsigned int* an_done)
{
       return PLP_CALL((chip_name), __cl73_get, ((phy_info), (an), (an_done)));
}

static int __macro__bcm_plp_display_eye_scan(char* chip_name, bcm_plp_access_t phy_info)
{
       return PLP_CALL((chip_name), __display_eye_scan, ((phy_info)));
}

static int __macro__bcm_plp_firmware_info_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int *fw_version, unsigned int *fw_crc)
{
       return PLP_CALL((chip_name), __firmware_info_get, ((phy_info), (fw_version), (fw_crc)));
}

static int __macro__bcm_plp_fec_enable_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int enable)
{
       return PLP_CALL((chip_name), __fec_enable_set, ((phy_info), (enable)));
}

static int __macro__bcm_plp_fec_enable_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* enable)
{
       return PLP_CALL((chip_name), __fec_enable_get, ((phy_info), (enable)));
}

static int __macro__bcm_plp_phy_status_dump(char* chip_name, bcm_plp_access_t phy_info)
{
       return PLP_CALL((chip_name), __phy_status_dump, ((phy_info)));
}

static int __macro__bcm_plp_phy_diagnostics_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pm_phy_diagnostics_t* diag)
{
       return PLP_CALL((chip_name), __phy_diagnostics_get, ((phy_info), (diag)));
}

static int __macro__bcm_plp_intr_status_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int intr_type, unsigned int* intr_status)
{
       return PLP_CALL((chip_name), __intr_status_get, ((phy_info), (intr_type), (intr_status)));
}

static int __macro__bcm_plp_intr_enable_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int intr_type, unsigned int enable)
{
       return PLP_CALL((chip_name), __intr_enable_set, ((phy_info), (intr_type), (enable)));
}

static int __macro__bcm_plp_intr_enable_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int intr_type, unsigned int* enable)
{
       return PLP_CALL((chip_name), __intr_enable_get, ((phy_info), (intr_type), (enable)));
}

static int __macro__bcm_plp_intr_status_clear(char* chip_name, bcm_plp_access_t phy_info, unsigned int intr_type)
{
       return PLP_CALL((chip_name), __intr_status_clear, ((phy_info), (intr_type)));
}
#ifdef SERDES_API_FLOATING_POINT
static int __macro__bcm_plp_eye_margin_proj(char* chip_name, bcm_plp_access_t phy_info, double rate,\
                                           unsigned char ber_scan_mode, unsigned char timer_control, unsigned char max_error_control)
{
       return PLP_CALL((chip_name), __eye_margin_proj, ((phy_info), (rate), (ber_scan_mode), (timer_control), (max_error_control)));
}
#else 
static int __macro__bcm_plp_eye_margin_proj(char* chip_name, bcm_plp_access_t phy_info, int rate,\
                                           unsigned char ber_scan_mode, unsigned char timer_control, unsigned char max_error_control)
{
       return PLP_CALL((chip_name), __eye_margin_proj, ((phy_info), (rate), (ber_scan_mode), (timer_control), (max_error_control)));
}
#endif
static int __macro__bcm_plp_module_read(char* chip_name, bcm_plp_access_t phy_info, unsigned int slv_addr,\
                                        unsigned int start_addr, unsigned int no_of_bytes, unsigned char *read_data)
{
       return PLP_CALL((chip_name), __module_read, ((phy_info), (slv_addr), (start_addr), (no_of_bytes), (read_data)));
}

static int __macro__bcm_plp_module_write(char* chip_name, bcm_plp_access_t phy_info, unsigned int slv_addr,\
                                        unsigned int start_addr, unsigned int no_of_bytes, unsigned char *write_data)
{
       return PLP_CALL((chip_name), __module_write, ((phy_info), (slv_addr), (start_addr), (no_of_bytes), (write_data)));
}

static int __macro__bcm_plp_cfg_gpio_pin_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int gpio_pin_number,\
                                        unsigned int cfg_direction, unsigned int cfg_pull, unsigned int pin_value)
{
       return PLP_CALL((chip_name), __cfg_gpio_pin_set, ((phy_info), (gpio_pin_number), (cfg_direction), (cfg_pull), (pin_value)));
}

static int __macro__bcm_plp_cfg_gpio_pin_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int gpio_pin_number,\
                                        unsigned int* cfg_direction, unsigned int* cfg_pull, unsigned int* pin_value)
{
       return PLP_CALL((chip_name), __cfg_gpio_pin_get, ((phy_info), (gpio_pin_number), (cfg_direction), (cfg_pull), (pin_value)));
}

static int __macro__bcm_plp_power_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int power_rx, unsigned int power_tx)
{
       return PLP_CALL((chip_name), __power_set, ((phy_info), (power_rx), (power_tx)));
}

static int __macro__bcm_plp_power_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int* power_rx, unsigned int* power_tx)
{
       return PLP_CALL((chip_name), __power_get, ((phy_info), (power_rx), (power_tx)));
}

static int __macro__bcm_plp_fec_corrected_error_counter(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_fec_type_t fec_type,\
                                                            unsigned int* count)
{
       return PLP_CALL((chip_name), __fec_corrected_counter_get, ((phy_info), (fec_type), (count)));
}

static int __macro__bcm_plp_fec_uncorrected_error_counter(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_fec_type_t fec_type,\
                                                            unsigned int* count)
{
       return PLP_CALL((chip_name), __fec_uncorrected_counter_get, ((phy_info), (fec_type), (count)));
}

static int __macro__bcm_plp_event_status_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int event_type,\
                                                            unsigned int* event_status)
{
       return PLP_CALL((chip_name), __event_status_get, ((phy_info), (event_type), (event_status)));
}

static int __macro__bcm_plp_rxtx_laneswap_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_laneswap_map_t* laneswap_map)  
{
       return PLP_CALL((chip_name), __rxtx_laneswap_set, ((phy_info), (laneswap_map)));
}

static int __macro__bcm_plp_rxtx_laneswap_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_laneswap_map_t* laneswap_map)
{
       return PLP_CALL((chip_name), __rxtx_laneswap_get, ((phy_info), (laneswap_map)));
}

static int __macro__bcm_plp_firmware_lane_config_set(char* chip_name, bcm_plp_access_t phy_info,\
                                                     bcm_plp_pm_firmware_lane_config_t* firmware_lane_config)
{
       return PLP_CALL((chip_name), __firmware_lane_config_set, ((phy_info), (firmware_lane_config)));
}

static int __macro__bcm_plp_firmware_lane_config_get(char* chip_name, bcm_plp_access_t phy_info,\
                                                     bcm_plp_pm_firmware_lane_config_t* firmware_lane_config)
{
       return PLP_CALL((chip_name), __firmware_lane_config_get, ((phy_info), (firmware_lane_config)));
}

#ifdef PLP_MACSEC_SUPPORT

static int __macro__bcm_plp_cfye_device_init(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_init_t* init_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_device_init, ((pa), (init_p)));
}

static int __macro__bcm_plp_cfye_device_uninit(char* chip_name, bcm_plp_sec_phy_access_t* pa)
{
       return PLP_SEC_CALL((chip_name), __cfye_device_uninit, ((pa)));
}

static int __macro__bcm_plp_cfye_vport_add(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t* vport_handle_p,\
                                       bcm_plp_cfye_vport_t *vport_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_add, ((pa), (vport_handle_p), (vport_p)));
}

static int __macro__bcm_plp_cfye_vport_remove(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t vport_handle)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_remove, ((pa), (vport_handle)));
}

static int __macro__bcm_plp_cfye_vport_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t vport_handle,\
                                       bcm_plp_cfye_vport_t *vport_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_update, ((pa), (vport_handle), (vport_p)));
}

static int __macro__bcm_plp_cfye_rule_add(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t vport_handle,\
                                       bcm_plp_cfye_rule_handle_t* rule_handle_p, bcm_plp_cfye_rule_t* rule_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_add, ((pa), (vport_handle), (rule_handle_p), (rule_p)));
}

static int __macro__bcm_plp_cfye_rule_remove(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                                       bcm_plp_cfye_rule_handle_t rule_handle)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_remove, ((pa), (rule_handle)));
}

static int __macro__bcm_plp_cfye_rule_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t rule_handle,\
                                         bcm_plp_cfye_rule_t* rule_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_update, ((pa), (rule_handle), (rule_p)));
}

static int __macro__bcm_plp_cfye_rule_enable(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t rule_handle,\
                                         unsigned char fsync)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_enable, ((pa), (rule_handle), (fsync)));
}

static int __macro__bcm_plp_cfye_rule_disable(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t rule_handle,\
                                         unsigned char fsync)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_disable, ((pa), (rule_handle), (fsync)));
}

static int __macro__bcm_plp_cfye_rule_enable_disable(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t rule_handle_disable,\
     bcm_plp_cfye_rule_handle_t rule_handle_enable, unsigned char enable_all, unsigned char disable_all, unsigned char fsync)
{
return PLP_SEC_CALL((chip_name), __cfye_rule_enable_disable, ((pa), (rule_handle_disable), (rule_handle_enable), (enable_all), (disable_all), (fsync)));
}

static int __macro__bcm_plp_cfye_vport_id_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t vport_handle)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_id_get, ((pa), (vport_handle)));
}

static int __macro__bcm_plp_cfye_device_limits(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int* max_channel_count_p,\
                                           unsigned int *maxv_port_count_p, unsigned int *max_rule_count_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_device_limits, ((pa), (max_channel_count_p), (maxv_port_count_p), (max_rule_count_p)));
}

static int __macro__bcm_plp_cfye_device_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_device_t* control_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_device_update, ((pa), (control_p)));
}

static int __macro__bcm_plp_cfye_statistics_read(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int stat_index,\
                                             bcm_plp_cfye_statistics_t *stat_p, unsigned char fsync)
{
       return PLP_SEC_CALL((chip_name), __cfye_statistics_read, ((pa), (stat_index), (stat_p), (fsync)));
}

static int __macro__bcm_plp_cfye_statistics_summary_read(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int start_offset,\
                                             unsigned int *summary_p, unsigned int Count)
{
       return PLP_SEC_CALL((chip_name), __cfye_statistics_summary_read, ((pa), (start_offset), (summary_p), (Count)));
}

static int __macro__bcm_plp_cfye_statistics_summary_channel_read(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                                             bcm_plp_cfye_statistics_channel_summary_t *ch_summary_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_statistics_summary_channel_read, ((pa), (ch_summary_p)));
}

static int __macro__bcm_plp_cfye_vport_handle_issame(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t handle1_p,\
                                             bcm_plp_cfye_vport_handle_t handle2_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_handle_issame, ((pa), (handle1_p), (handle2_p)));
}

static int __macro__bcm_plp_cfye_rule_handle_issame(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t handle1_p,\
                                             bcm_plp_cfye_rule_handle_t handle2_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_handle_issame, ((pa), (handle1_p), (handle2_p)));
}

static int __macro__bcm_plp_cfye_intr_enable_set(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_intr_t *cfye_intr)
{
       return PLP_SEC_CALL((chip_name), __cfye_intr_enable_set, ((pa), (cfye_intr)));
}

static int __macro__bcm_plp_cfye_intr_enable_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_intr_t *cfye_intr)
{
       return PLP_SEC_CALL((chip_name), __cfye_intr_enable_get, ((pa), (cfye_intr)));
}

static int __macro__bcm_plp_cfye_intr_status_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_intr_t *cfye_intr_sts)
{
       return PLP_SEC_CALL((chip_name), __cfye_intr_status_get, ((pa), (cfye_intr_sts)));
}

static int __macro__bcm_plp_cfye_rule_handle_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int rule_index,\
                                                 bcm_plp_cfye_rule_handle_t* rule_handle_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_handle_get, ((pa), (rule_index), (rule_handle_p)));
}

static int __macro__bcm_plp_cfye_rule_index_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_rule_handle_t rule_handle,\
                                                unsigned int* rule_index_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_rule_index_get, ((pa), (rule_handle), (rule_index_p)));
}

static int __macro__bcm_plp_cfye_vport_handle_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport_index,\
                                                bcm_plp_cfye_vport_handle_t* vport_handle_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_handle_get, ((pa), (vport_index), (vport_handle_p)));
}

static int __macro__bcm_plp_cfye_vport_index_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_cfye_vport_handle_t vport_handle,\
                                                unsigned int* vport_index_p)
{
       return PLP_SEC_CALL((chip_name), __cfye_vport_index_get, ((pa), (vport_handle), (vport_index_p)));
}

static int __macro__bcm_plp_secy_device_init(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_settings_t* settings_p)
{
       return PLP_SEC_CALL((chip_name), __secy_device_init, ((pa), (settings_p)));
}

static int __macro__bcm_plp_secy_device_uninit(char* chip_name, bcm_plp_sec_phy_access_t* pa)
{
       return PLP_SEC_CALL((chip_name), __secy_device_uninit, ((pa)));
}

static int __macro__bcm_plp_secy_sa_add(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport,\
                                    bcm_plp_secy_sa_handle_t* sa_handle_p, bcm_plp_secy_sa_t* sa_p)
{
       return PLP_SEC_CALL((chip_name), __secy_sa_add, ((pa), (vport), (sa_handle_p), (sa_p)));
}

static int __macro__bcm_plp_secy_sa_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                       bcm_plp_secy_sa_t* sa_p)
{
       return PLP_SEC_CALL((chip_name), __secy_sa_update, ((pa), (sa_handle), (sa_p)));
}

static int __macro__bcm_plp_secy_sa_read(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                     unsigned int word_offset, unsigned int word_count, unsigned int * transform_p)
{
       return PLP_SEC_CALL((chip_name), __secy_sa_read, ((pa), (sa_handle), (word_offset), (word_count), (transform_p)));
}

static int __macro__bcm_plp_secy_sa_remove(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle)
{
       return PLP_SEC_CALL((chip_name), __secy_sa_remove, ((pa), (sa_handle)));
}

static int __macro__bcm_plp_secy_rules_sectag_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_rule_sectag_t * sec_tag_rules_p)
{
       return PLP_SEC_CALL((chip_name), __secy_rules_sectag_update, ((pa), (sec_tag_rules_p)));
}

static int __macro__bcm_plp_secy_bypass_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned char* fbypass)
{
       return PLP_SEC_CALL((chip_name), __secy_bypass_get, ((pa), (fbypass)));
}

static int __macro__bcm_plp_secy_bypass_set(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned char fbypass)
{
       return PLP_SEC_CALL((chip_name), __secy_bypass_set, ((pa), (fbypass)));
}

static int __macro__bcm_plp_secy_config_set(char* chip_name, bcm_plp_sec_phy_access_t* pa)
{
       return PLP_SEC_CALL((chip_name), __secy_config_set, ((pa)));
}

/* Hoang
static int __macro__bcm_plp_secy_config_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int* mode)
{
       return PLP_SEC_CALL((chip_name), __secy_config_get, ((pa), (mode)));
}
*/

static int __macro__bcm_plp_secy_device_limits(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int* max_channel_count_p,\
                                           unsigned int* maxv_port_count_p, unsigned int* max_sa_count_p, unsigned int* max_sc_count_p)
{
return PLP_SEC_CALL((chip_name), __secy_device_limits, ((pa), (max_channel_count_p), (maxv_port_count_p), (max_sa_count_p), (max_sc_count_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_secy_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int secy_index,\
                                           unsigned int* count_summary_p)
{
return PLP_SEC_CALL((chip_name), __secy_device_count_summary_secy_checkandclear, ((pa), (secy_index), (count_summary_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_pifc_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int** ifc_indexes_pp,unsigned int* num_ifc_indexes_p) 
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_pifc_checkandclear, ((pa), (ifc_indexes_pp), (num_ifc_indexes_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_ifc_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int ifc_index, unsigned int* count_summary_ifc_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_ifc_checkandclear, ((pa), (ifc_index), (count_summary_ifc_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_pifc1_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int ** ifc_indexes_pp, unsigned int * num_ifc_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_pifc1_checkandclear, ((pa), (ifc_indexes_pp), (num_ifc_indexes_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_ifc1_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int ifc_index, unsigned int* count_summary_ifc_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_ifc1_checkandclear, ((pa), (ifc_index), (count_summary_ifc_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_prxcam_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int ** rxcam_indexes_pp, unsigned int* num_rxcam_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_prxcam_checkandclear, ((pa), (rxcam_indexes_pp), (num_rxcam_indexes_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_psa_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int ** sa_indexes_pp, unsigned int* num_sa_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_psa_checkandclear, ((pa), (sa_indexes_pp), (num_sa_indexes_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_sa_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int  sa_index, unsigned int* count_summary_sa_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_sa_checkandclear, ((pa), (sa_index), (count_summary_sa_p)));
}

static int __macro__bcm_plp_secy_device_count_summary_channel_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                          unsigned int** channel_summary_pp , unsigned int* num_channel_summaries_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_channel_checkandclear, ((pa), (channel_summary_pp), (num_channel_summaries_p)));
}

static int __macro__bcm_plp_secy_sa_handle_issame(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                                              bcm_plp_secy_sa_handle_t* handle1_p, bcm_plp_secy_sa_handle_t* handle2_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_handle_issame, ((pa), (handle1_p), (handle2_p)));
}

static int __macro__bcm_plp_secy_sa_handle_sa_index_issame(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                                              bcm_plp_secy_sa_handle_t sa_handle, unsigned int sa_index)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_handle_sa_index_issame, ((pa), (sa_handle), (sa_index)));
}

static int __macro__bcm_plp_secy_crypt_auth_bypass_len_update(char* chip_name, bcm_plp_sec_phy_access_t* pa,\
                                                          unsigned int bypass_length)
{
    return PLP_SEC_CALL((chip_name), __secy_crypt_auth_bypass_len_update, ((pa), (bypass_length)));
}

static int __macro__bcm_plp_secy_sa_nextpn_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                               unsigned int next_pn_lo,  unsigned int next_pn_hi, unsigned char* fnext_pn_written_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_nextpn_update, ((pa), (sa_handle), (next_pn_lo), (next_pn_hi), (fnext_pn_written_p)));
}

static int __macro__bcm_plp_secy_sa_chain(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t active_sa_handle,\
                                      bcm_plp_secy_sa_handle_t* new_sa_handle_p, bcm_plp_secy_sa_t* new_sa_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_chain, ((pa), (active_sa_handle), (new_sa_handle_p), (new_sa_p)));
}

static int __macro__bcm_plp_secy_sa_switch(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t active_sa_handle,\
                                      bcm_plp_secy_sa_handle_t new_sa_handle, bcm_plp_secy_sa_t* new_sa_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_switch, ((pa), (active_sa_handle), (new_sa_handle), (new_sa_p)));
}

static int __macro__bcm_plp_secy_sa_statistics_e_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                      bcm_plp_secy_sa_stat_e_t* stat_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_statistics_e_get, ((pa), (sa_handle), (stat_p), (fsync)));
}

static int __macro__bcm_plp_secy_sa_statistics_i_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                      bcm_plp_secy_sa_stat_i_t* stat_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_statistics_i_get, ((pa), (sa_handle), (stat_p), (fsync)));
}

static int __macro__bcm_plp_secy_ifc_statistics_e_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport,\
                                      bcm_plp_secy_ifc_stat_e_t* stat_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_ifc_statistics_e_get, ((pa), (vport), (stat_p), (fsync)));
}

static int __macro__bcm_plp_secy_ifc_statistics_i_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport,\
                                      bcm_plp_secy_ifc_stat_i_t* stat_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_ifc_statistics_i_get, ((pa), (vport), (stat_p), (fsync)));
}

static int __macro__bcm_plp_secy_rxcam_statistics_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int scindex,\
                                      bcm_plp_secy_rxcam_stat_t* stat_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_rxcam_statistics_get, ((pa), (scindex), (stat_p), (fsync)));
}

static int __macro__bcm_plp_secy_sa_pnthr_summary_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int** sa_indexes_pp,\
                                                            unsigned int* num_sa_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_pnthr_summary_checkandclear, ((pa), (sa_indexes_pp), (num_sa_indexes_p)));
}

static int __macro__bcm_plp_secy_sa_expired_summary_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int** sa_indexes_pp,\
                                                            unsigned int* num_sa_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_expired_summary_checkandclear, ((pa), (sa_indexes_pp), (num_sa_indexes_p)));
}

static int __macro__bcm_plp_secy_rules_mtucheck_update(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int scindex,\
                                                   bcm_plp_secy_sc_rule_mtu_check_t* mtu_check_rule_p)
{
    return PLP_SEC_CALL((chip_name), __secy_rules_mtucheck_update, ((pa), (scindex), (mtu_check_rule_p)));
}

static int __macro__bcm_plp_secy_channel_statistics_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_channel_stat_t* stats_p,\
                                                   unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_channel_statistics_get, ((pa), (stats_p), (fsync)));
}

static int __macro__bcm_plp_secy_channel_packets_inflight_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int* in_flight_p)
{
    return PLP_SEC_CALL((chip_name), __secy_channel_packets_inflight_get, ((pa), (in_flight_p)));
}

static int __macro__bcm_plp_secy_secy_statistics_e_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport,\
                                                   bcm_plp_secy_secy_stat_e_t *stats_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_secy_statistics_e_get, ((pa), (vport), (stats_p), (fsync)));
}

static int __macro__bcm_plp_secy_secy_statistics_i_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int vport,\
                                                   bcm_plp_secy_secy_stat_i_t *stats_p, unsigned char fsync)
{
    return PLP_SEC_CALL((chip_name), __secy_secy_statistics_i_get, ((pa), (vport), (stats_p), (fsync)));
}

static int __macro__bcm_plp_secy_device_count_summary_psecy_checkandclear(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int** secy_indexes_pp,\
                                                   unsigned int* num_secy_indexes_p)
{
    return PLP_SEC_CALL((chip_name), __secy_device_count_summary_psecy_checkandclear, ((pa), (secy_indexes_pp), (num_secy_indexes_p)));
}

static int __macro__bcm_plp_secy_intr_enable_set(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_intr_t* secy_intr)
{
    return PLP_SEC_CALL((chip_name), __secy_intr_enable_set, ((pa), (secy_intr)));
}

static int __macro__bcm_plp_secy_intr_enable_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_intr_t* secy_intr)
{
    return PLP_SEC_CALL((chip_name), __secy_intr_enable_get, ((pa), (secy_intr)));
}

static int __macro__bcm_plp_secy_intr_status_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_intr_t* secy_intr)
{
    return PLP_SEC_CALL((chip_name), __secy_intr_status_get, ((pa), (secy_intr)));
}

static int __macro__bcm_plp_secy_sa_handle_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, unsigned int sa_index,\
                                               bcm_plp_secy_sa_handle_t* sa_handle_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_handle_get, ((pa), (sa_index), (sa_handle_p)));
}

static int __macro__bcm_plp_secy_sa_index_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_secy_sa_handle_t sa_handle,\
                                               unsigned int * sa_index_p, unsigned int * sc_index_p)
{
    return PLP_SEC_CALL((chip_name), __secy_sa_index_get, ((pa), (sa_handle), (sa_index_p), (sc_index_p)));
}

static int __macro__bcm_plp_mac_sec_burst_reg_value_get(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_sec_reg_access_t *reg_acc) 
{
    return PLP_SEC_CALL((chip_name), __mac_sec_burst_reg_value_get, ((pa), (reg_acc)));
}

static int __macro__bcm_plp_mac_sec_burst_reg_value_set(char* chip_name, bcm_plp_sec_phy_access_t* pa, bcm_plp_sec_reg_access_t *reg_acc)
{
    return PLP_SEC_CALL((chip_name), __mac_sec_burst_reg_value_set, ((pa), (reg_acc)));
}

static int __macro__bcm_plp_mac_loopback_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_lb_type_t lb_type, unsigned int enable)
{
    return PLP_SEC_CALL((chip_name), __mac_loopback_set, ((mac_info), (lb_type), (enable)));
}

static int __macro__bcm_plp_mac_loopback_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_lb_type_t lb_type, unsigned int* enable)
{
    return PLP_SEC_CALL((chip_name), __mac_loopback_get, ((mac_info), (lb_type), (enable)));
}

static int __macro__bcm_plp_mac_fault_option_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_fault_option_t fault_option)
{
    return PLP_SEC_CALL((chip_name), __mac_fault_option_set, ((mac_info), (fault_option)));
}

static int __macro__bcm_plp_mac_fault_option_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_fault_option_t* fault_option)
{
    return PLP_SEC_CALL((chip_name), __mac_fault_option_get, ((mac_info), (fault_option)));
}

static int __macro__bcm_plp_mac_flow_control_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_flow_control_t flow_option)
{
    return PLP_SEC_CALL((chip_name), __mac_flow_control_set, ((mac_info), (flow_option)));
}

static int __macro__bcm_plp_mac_flow_control_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_flow_control_t* flow_option)
{
    return PLP_SEC_CALL((chip_name), __mac_flow_control_get, ((mac_info), (flow_option)));
}

static int __macro__bcm_plp_mac_store_and_forward_mode_set(char* chip_name, bcm_plp_mac_access_t mac_info, int enable)
{
    return PLP_SEC_CALL((chip_name), __mac_store_and_forward_mode_set, ((mac_info), (enable)));
}

static int __macro__bcm_plp_mac_store_and_forward_mode_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* is_enable)
{
    return PLP_SEC_CALL((chip_name), __mac_store_and_forward_mode_get, ((mac_info), (is_enable)));
}

static int __macro__bcm_plp_reg64_value_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int devaddr,\
                                            unsigned int regaddr, unsigned long long data)
{
       return PLP_SEC_CALL((chip_name), __reg64_value_set, ((phy_info), (devaddr), (regaddr), (data)));
}

static int __macro__bcm_plp_reg64_value_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int devaddr,\
                                            unsigned int regaddr, unsigned long long* data)
{
       return PLP_SEC_CALL((chip_name), __reg64_value_get, ((phy_info), (devaddr), (regaddr), (data)));
}

static int __macro__bcm_plp_mac_cleanup(char* chip_name, bcm_plp_mac_access_t mac_info)
{
    return PLP_SEC_CALL((chip_name), __mac_cleanup, ((mac_info)));
}

static int __macro__bcm_plp_mac_max_packet_size_set(char* chip_name, bcm_plp_mac_access_t mac_info, int pkt_size)
{
    return PLP_SEC_CALL((chip_name), __mac_max_packet_size_set, ((mac_info), (pkt_size)));
}

static int __macro__bcm_plp_mac_max_packet_size_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* pkt_size)
{
    return PLP_SEC_CALL((chip_name), __mac_max_packet_size_get, ((mac_info), (pkt_size)));
}

static int __macro__bcm_plp_mac_runt_threshold_set(char* chip_name, bcm_plp_mac_access_t mac_info, int threshold)
{
    return PLP_SEC_CALL((chip_name), __mac_runt_threshold_set, ((mac_info), (threshold)));
}

static int __macro__bcm_plp_mac_runt_threshold_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* threshold)
{
    return PLP_SEC_CALL((chip_name), __mac_runt_threshold_get, ((mac_info), (threshold)));
}

static int __macro__bcm_plp_mac_pad_size_set(char* chip_name, bcm_plp_mac_access_t mac_info, int pad_size)
{
    return PLP_SEC_CALL((chip_name), __mac_pad_size_set, ((mac_info), (pad_size)));
}

static int __macro__bcm_plp_mac_pad_size_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* pad_size)
{
    return PLP_SEC_CALL((chip_name), __mac_pad_size_get, ((mac_info), (pad_size)));
}

static int __macro__bcm_plp_tx_mac_sa_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_mac_t mac_sa)
{
    return PLP_SEC_CALL((chip_name), __tx_mac_sa_set, ((mac_info), (mac_sa)));
}

static int __macro__bcm_plp_tx_mac_sa_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_mac_t mac_sa)
{
    return PLP_SEC_CALL((chip_name), __tx_mac_sa_get, ((mac_info), (mac_sa)));
}

static int __macro__bcm_plp_rx_mac_sa_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_mac_t mac_sa)
{
    return PLP_SEC_CALL((chip_name), __rx_mac_sa_set, ((mac_info), (mac_sa)));
}

static int __macro__bcm_plp_rx_mac_sa_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_mac_t mac_sa)
{
    return PLP_SEC_CALL((chip_name), __rx_mac_sa_get, ((mac_info), (mac_sa)));
}

static int __macro__bcm_plp_mac_tx_avg_ipg_set(char* chip_name, bcm_plp_mac_access_t mac_info, int avg_ipg)
{
    return PLP_SEC_CALL((chip_name), __tx_avg_ipg_set, ((mac_info), (avg_ipg)));
}

static int __macro__bcm_plp_mac_tx_avg_ipg_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* avg_ipg)
{
    return PLP_SEC_CALL((chip_name), __tx_avg_ipg_get, ((mac_info), (avg_ipg)));
}

static int __macro__bcm_plp_mac_tx_preamble_length_set(char* chip_name, bcm_plp_mac_access_t mac_info, int preamble_length)
{
    return PLP_SEC_CALL((chip_name), __mac_tx_preamble_length_set, ((mac_info), (preamble_length)));
}

static int __macro__bcm_plp_mac_tx_preamble_length_get(char* chip_name, bcm_plp_mac_access_t mac_info, int* preamble_length)
{
    return PLP_SEC_CALL((chip_name), __mac_tx_preamble_length_get, ((mac_info), (preamble_length)));
}

static int __macro__bcm_plp_mac_pause_control_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_pause_control_t pause_control)
{
    return PLP_SEC_CALL((chip_name), __mac_pause_control_set, ((mac_info), (pause_control)));
}

static int __macro__bcm_plp_mac_pause_control_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_pause_control_t* pause_control)
{
    return PLP_SEC_CALL((chip_name), __mac_pause_control_get, ((mac_info), (pause_control)));
}

static int __macro__bcm_plp_mac_pfc_control_set(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_pfc_control_t pfc_ctrl)
{
    return PLP_SEC_CALL((chip_name), __mac_pfc_control_set, ((mac_info), (pfc_ctrl)));
}

static int __macro__bcm_plp_mac_pfc_control_get(char* chip_name, bcm_plp_mac_access_t mac_info, bcm_plp_mac_pfc_control_t* pfc_ctrl)
{
    return PLP_SEC_CALL((chip_name), __mac_pfc_control_get, ((mac_info), (pfc_ctrl)));
}

static int __macro__bcm_plp_mac_diagnostic_dump(char* chip_name, bcm_plp_mac_access_t mac_info)
{
    return PLP_SEC_CALL((chip_name), __mac_diag_dump, ((mac_info)));
}
#endif 

static int __macro__bcm_plp_timesync_config_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_timesync_config_t* config)
{
    return PLP_CALL((chip_name), __timesync_config_set, ((phy_info), (config)));
}

static int __macro__bcm_plp_timesync_config_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_timesync_config_t* config)
{
    return PLP_CALL((chip_name), __timesync_config_get, ((phy_info), (config)));
}

static int __macro__bcm_plp_timesync_enable_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags, unsigned int enable)
{
    return PLP_CALL((chip_name), __timesync_enable_set, ((phy_info), (flags), (enable)));
}

static int __macro__bcm_plp_timesync_enable_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags, unsigned int* enable)
{
    return PLP_CALL((chip_name), __timesync_enable_get, ((phy_info), (flags), (enable)));
}

static int __macro__bcm_plp_timesync_nco_addend_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags, unsigned int freq_step)
{
    return PLP_CALL((chip_name), __timesync_nco_addend_set, ((phy_info), (flags), (freq_step)));
}

static int __macro__bcm_plp_timesync_nco_addend_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags, unsigned int* freq_step)
{
    return PLP_CALL((chip_name), __timesync_nco_addend_get, ((phy_info), (flags), (freq_step)));
}

static int __macro__bcm_plp_timesync_framesync_mode_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                        bcm_plp_timesync_framesync_t* framesync)
{
    return PLP_CALL((chip_name), __timesync_framesync_mode_set, ((phy_info), (flags), (framesync)));
}

static int __macro__bcm_plp_timesync_framesync_mode_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                        bcm_plp_timesync_framesync_t* framesync)
{
    return PLP_CALL((chip_name), __timesync_framesync_mode_get, ((phy_info), (flags), (framesync)));
}

static int __macro__bcm_plp_timesync_local_time_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                    unsigned long long local_time)
{
    return PLP_CALL((chip_name), __timesync_local_time_set, ((phy_info), (flags), (local_time)));
}

static int __macro__bcm_plp_timesync_local_time_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                    unsigned long long* local_time)
{
    return PLP_CALL((chip_name), __timesync_local_time_get, ((phy_info), (flags), (local_time)));
}

static int __macro__bcm_plp_timesync_load_ctrl_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                    unsigned int load_once, unsigned int load_always)
{
    return PLP_CALL((chip_name), __timesync_load_ctrl_set, ((phy_info), (flags), (load_once), (load_always)));
}

static int __macro__bcm_plp_timesync_load_ctrl_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                    unsigned int* load_once, unsigned int* load_always)
{
    return PLP_CALL((chip_name), __timesync_load_ctrl_get, ((phy_info), (flags), (load_once), (load_always)));
}

static int __macro__bcm_plp_timesync_tx_timestamp_offset_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned int ts_offset)
{
    return PLP_CALL((chip_name), __timesync_tx_timestamp_offset_set, ((phy_info), (flags), (ts_offset)));
}

static int __macro__bcm_plp_timesync_tx_timestamp_offset_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned int* ts_offset)
{
    return PLP_CALL((chip_name), __timesync_tx_timestamp_offset_get, ((phy_info), (flags), (ts_offset)));
}

static int __macro__bcm_plp_timesync_rx_timestamp_offset_set(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned int ts_offset)
{
    return PLP_CALL((chip_name), __timesync_rx_timestamp_offset_set, ((phy_info), (flags), (ts_offset)));
}

static int __macro__bcm_plp_timesync_rx_timestamp_offset_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned int* ts_offset)
{
    return PLP_CALL((chip_name), __timesync_rx_timestamp_offset_get, ((phy_info), (flags), (ts_offset)));
}

static int __macro__bcm_plp_timesync_capture_timestamp_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned long long* cap_ts)
{
    return PLP_CALL((chip_name), __timesync_capture_timestamp_get, ((phy_info), (flags), (cap_ts)));
}

static int __macro__bcm_plp_timesync_heartbeat_timestamp_get(char* chip_name, bcm_plp_access_t phy_info, unsigned int flags,\
                                                             unsigned long long* hb_ts)
{
    return PLP_CALL((chip_name), __timesync_heartbeat_timestamp_get, ((phy_info), (flags), (hb_ts)));
}

static int __macro__bcm_plp_timesync_do_sync(char* chip_name, bcm_plp_access_t phy_info)
{
    return PLP_CALL((chip_name), __timesync_do_sync, ((phy_info)));
}

#ifdef CINT_BCM_PLP_BASE_T_PHY
static int __macro__bcm_plp_base_t_autoneg_ability_set (char *chip_name, bcm_plp_access_t phy_info, bcm_plp_base_t_ability_t *ability) {
    return PLP_CALL((chip_name),  __base_t_autoneg_ability_set, ((phy_info), (ability)));
}
static int __macro__bcm_plp_base_t_autoneg_set (char *chip_name, bcm_plp_access_t phy_info, int enable) {
    return PLP_CALL((chip_name),  __base_t_autoneg_set, ((phy_info), (enable)));
}
static int __macro__bcm_plp_base_t_eee_set (char *chip_name, bcm_plp_access_t phy_info, bcm_plp_base_t_eee_t *eee_conf) {
    return PLP_CALL((chip_name),  __base_t_eee_set, ((phy_info), (eee_conf)));
}
static int __macro__bcm_plp_base_t_eee_get (char *chip_name, bcm_plp_access_t phy_info, bcm_plp_base_t_eee_t *eee_conf) {
    return PLP_CALL((chip_name),  __base_t_eee_get, ((phy_info), (eee_conf)));
}
#endif /* CINT_BCM_PLP_BASE_T_PHY */

static int __macro__bcm_plp_pam4_tx_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pam4_tx_t* tx)
{
       return PLP_CALL((chip_name), __pam4_tx_get, ((phy_info), (tx)));
}

static int __macro__bcm_plp_pam4_tx_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_pam4_tx_t* tx)
{
       return PLP_CALL((chip_name), __pam4_tx_set, ((phy_info), (tx)));
}

static int __macro__bcm_plp_dsrds_firmware_lane_config_get(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_dsrds_firmware_lane_config_t* firmware_lane_config)
{
       return PLP_CALL((chip_name), __dsrds_firmware_lane_config_get, ((phy_info), (firmware_lane_config)));
}

static int __macro__bcm_plp_dsrds_firmware_lane_config_set(char* chip_name, bcm_plp_access_t phy_info, bcm_plp_dsrds_firmware_lane_config_t firmware_lane_config)
{
       return PLP_CALL((chip_name), __dsrds_firmware_lane_config_set, ((phy_info), (firmware_lane_config)));
}

#endif /* INCLUDE_LIB_CINT */
#endif /* PHYMOD_EPDM_APIS_SUPPORT */

