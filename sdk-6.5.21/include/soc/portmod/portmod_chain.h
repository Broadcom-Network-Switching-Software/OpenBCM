/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef _PORTMOD_CHAIN_H_
#define _PORTMOD_CHAIN_H_

#define MAX_VARS_PER_XPHY_BUFFER 13

int portmod_phychain_ext_phy_info_set(int unit, int phyn, int core_index, const portmod_ext_phy_core_info_t* core_info);
int portmod_phychain_ext_phy_info_get(int unit, int phyn, int core_index, portmod_ext_phy_core_info_t* core_info);


int portmod_ext_phy_core_info_t_init(int unit, portmod_ext_phy_core_info_t* portmod_ext_phy_core_info);

int portmod_chain_xphy_add_by_index(int unit, int xphy_idx, int xphy_addr, const phymod_core_access_t* core_access);
int portmod_chain_xphy_add(int unit, int xphy_addr, const phymod_core_access_t* core_access, int *xphy_idx_out);
int portmod_chain_xphy_user_access_store(int unit, int xphy_idx, void* user_acc);
void portmod_chain_xphy_user_access_release (int unit);

int portmod_chain_xphy_delete(int unit, int xphy_addr);
int portmod_chain_xphy_delete_all(int unit);
int portmod_xphy_core_access_set(int unit, uint32 xphy_addr, phymod_core_access_t *core_access, int is_legacy_phy);
int portmod_xphy_core_access_get(int unit, uint32 xphy_addr, phymod_core_access_t *core_access, int *is_legacy_phy);
int portmod_xphy_core_config_set(int unit, uint32 xphy_addr, phymod_core_init_config_t* core_config);
int portmod_xphy_core_config_get(int unit, uint32 xphy_addr, phymod_core_init_config_t* core_config);
int portmod_xphy_is_initialized_set(int unit, uint32 xphy_addr, int is_initialized);
int portmod_xphy_is_initialized_get(int unit, uint32 xphy_addr, int* is_initialized);
int portmod_xphy_phy_type_set(int unit, uint32 xphy_addr, phymod_dispatch_type_t phy_type);
int portmod_xphy_phy_type_get(int unit, uint32 xphy_addr, phymod_dispatch_type_t* phy_type);
int portmod_xphy_is_legacy_phy_set(int unit, uint32 xphy_addr, int is_legacy_phy);
int portmod_xphy_is_legacy_phy_get(int unit, uint32 xphy_addr, int* is_legacy_phy);
int portmod_xphy_ref_clk_set(int unit, uint32 xphy_addr, phymod_ref_clk_t ref_clk);
int portmod_xphy_ref_clk_get(int unit, uint32 xphy_addr, phymod_ref_clk_t* ref_clk);
int portmod_xphy_fw_load_method_set(int unit, uint32 xphy_addr, phymod_firmware_load_method_t  fw_load_method);
int portmod_xphy_fw_load_method_get(int unit, uint32 xphy_addr, phymod_firmware_load_method_t* fw_load_method);
int portmod_xphy_force_fw_load_set(int unit, uint32 xphy_addr, int force_fw_load);
int portmod_xphy_force_fw_load_get(int unit, uint32 xphy_addr, int* force_fw_load);
int portmod_xphy_polarity_set(int unit, uint32 xphy_addr, phymod_polarity_t polarity);
int portmod_xphy_polarity_get(int unit, uint32 xphy_addr, phymod_polarity_t* polarity);
int portmod_xphy_lane_map_set(int unit, uint32 xphy_addr, phymod_lane_map_t lane_map);
int portmod_xphy_lane_map_get(int unit, uint32 xphy_addr, phymod_lane_map_t* lane_map);
int portmod_xphy_gearbox_enable_set(int unit, uint32 xphy_addr, uint8 gearbox_enable);
int portmod_xphy_gearbox_enable_get(int unit, uint32 xphy_addr, uint8* gearbox_enable);
int portmod_xphy_pin_compatibility_enable_set(int unit, uint32 xphy_addr, uint8 pin_compatibility_enable);
int portmod_xphy_pin_compatibility_enable_get(int unit, uint32 xphy_addr, uint8* pin_compatibility_enable);
int portmod_xphy_phy_mode_reverse_set(int unit, uint32 xphy_addr, uint8 phy_mode_reverse);
int portmod_xphy_phy_mode_reverse_get(int unit, uint32 xphy_addr, uint8* phy_mode_reverse);
int portmod_xphy_primary_core_num_set(int unit, uint32 xphy_addr, uint32 primary_core_num);
int portmod_xphy_primary_core_num_get(int unit, uint32 xphy_addr, uint32* primary_core_num);
int portmod_xphy_core_probed_set(int unit, uint32 xphy_addr, int core_probed);
int portmod_xphy_core_probed_get(int unit, uint32 xphy_addr, int* core_probed);
int portmod_xphy_active_lane_set(int unit, uint32 xphy_addr, uint32 active_lanes);
int portmod_xphy_active_lane_get(int unit, uint32 xphy_addr, uint32 *active_lanes);


int portmod_port_phychain_core_access_set(int unit, int phyn, int core_index, const phymod_core_access_t* core_access);
int portmod_port_phychain_core_access_get(int unit, int port, const int chain_length, int max_cores, phymod_core_access_t* core_access);
int portmod_port_phychain_core_probe (int unit, int port, phymod_core_access_t* core,
                                      const int chain_length);
int portmod_port_phychain_core_identify (int unit, phymod_core_access_t* core,
                                          const int chain_length,
                                          uint32_t core_id,
                                          uint32_t* identified);
int portmod_port_phychain_core_lane_map_get(int unit, const phymod_core_access_t* core,
                                            const int chain_length,
                                            phymod_lane_map_t* lane_map);
int portmod_port_phychain_core_reset_set(int unit, const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t direction);
int portmod_port_phychain_core_reset_get(int unit, const phymod_core_access_t* core,
                                         const int chain_length,
                                         phymod_reset_mode_t reset_mode,
                                         phymod_reset_direction_t* direction );
int portmod_port_phychain_core_firmware_info_get(int unit, const phymod_core_access_t* core,
                                                 const int chain_length,
                                                 phymod_core_firmware_info_t* fw_info); 
int portmod_port_phychain_loopback_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_loopback_mode_t loopback,
                                       uint32_t *enable);
int portmod_port_phychain_loopback_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_loopback_mode_t loopback,
                                       uint32_t enable);
int portmod_port_phychain_firmware_core_config_set(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_core_config_t fw_core_config);
int phymod_chain_phy_firmware_core_config_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_firmware_core_config_t *fw_core_config);
int portmod_port_phychain_firmware_lane_config_set(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t fw_lane_config);
int portmod_port_phychain_firmware_lane_config_get(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length,
                                                   phymod_firmware_lane_config_t *fw_lane_config);
int portmod_port_phychain_core_pll_sequencer_restart(int unit, const phymod_core_access_t* core,
                                                     const int chain_length,
                                                     uint32_t flags,
                                                     phymod_sequencer_operation_t operation);
int phymod_chain_core_wait_event(int unit, const phymod_core_access_t* phy_access,
                                 const int chain_length,
                                 phymod_core_event_t event,
                                 uint32_t timeout);
int portmod_port_phychain_rx_restart(int unit, const phymod_phy_access_t* phy_access,
                                     const int chain_length);
int portmod_port_phychain_polarity_set(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       const phymod_polarity_t* polarity);
int portmod_port_phychain_polarity_get(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       phymod_polarity_t* polarity);
int portmod_port_phychain_tx_set(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_tx_t* tx);
int portmod_port_phychain_tx_get(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_tx_t* tx);
int portmod_port_phychain_media_type_tx_get(int unit, const phymod_phy_access_t* phy_access,
                                           const int chain_length,
                                           phymod_media_typed_t media,
                                           phymod_tx_t* tx);
int portmod_port_phychain_tx_override_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_tx_override_t* tx_override);
int portmod_port_phychain_tx_override_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_tx_override_t* tx_override);
int portmod_port_phychain_txpi_config_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          const phymod_txpi_config_t *config);
int portmod_port_phychain_txpi_config_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_txpi_config_t *config);
int portmod_port_phychain_rx_set(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 const phymod_rx_t* rx);
int portmod_port_phychain_rx_get(int unit, const phymod_phy_access_t* phy_access,
                                 const int chain_length,
                                 phymod_rx_t* rx);
int portmod_port_phychain_reset_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    const phymod_phy_reset_t* reset);
int portmod_port_phychain_reset_get(int unit, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    phymod_phy_reset_t* reset);
int portmod_port_phychain_power_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length, uint32 flags,
                                    const phymod_phy_power_t* power);
int portmod_port_phychain_power_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length, uint32 flags,
                                    phymod_phy_power_t* power);
int portmod_port_phychain_tx_lane_control_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_tx_lane_control_t tx_control);
int portmod_port_phychain_tx_lane_control_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_tx_lane_control_t *tx_control);
int portmod_port_phychain_rx_lane_control_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_rx_lane_control_t rx_control);
int portmod_port_phychain_rx_lane_control_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_phy_rx_lane_control_t *rx_control);
int portmod_port_phychain_fec_enable_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t enable);
int portmod_port_phychain_fec_enable_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t *enable);
int portmod_port_phychain_eee_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t enable);
int portmod_port_phychain_eee_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                  const int chain_length,
                                  uint32_t *enable);
int portmod_port_phychain_interface_config_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t flags,
                                               const phymod_phy_inf_config_t* config,
                                               phymod_interface_t line_intf,
                                               int ref_clk,
                                               const uint32 phy_init_flags);
int portmod_port_phychain_interface_config_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint32_t flags,
                                              phymod_ref_clk_t ref_clock,
                                              phymod_phy_inf_config_t* config,
                                              const uint32 phy_init_flags);
int portmod_port_phychain_duplex_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         phymod_duplex_mode_t *full_duplex);
int portmod_port_phychain_cl72_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t cl72_en);
int portmod_port_phychain_cl72_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *cl72_en);
int portmod_port_phychain_cl72_status_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_cl72_status_t *status);
int portmod_port_phychain_autoneg_ability_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              const phymod_autoneg_ability_t* an_ability,
                                              const portmod_port_ability_t * port_ability);
int portmod_port_phychain_autoneg_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, uint32 flags,
                                              phymod_autoneg_ability_t* an_ability,
                                              portmod_port_ability_t * port_ability);
int portmod_port_phychain_local_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            portmod_port_ability_t* port_ability);
int portmod_port_phychain_autoneg_restart_set(int unit, const phymod_phy_access_t* phy_access,
                                                              const int chain_length);

int portmod_port_phychain_autoneg_remote_ability_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                                     const int chain_length, uint32 flags,
                                                     phymod_autoneg_ability_t* an_ability,
                                                     portmod_port_ability_t * port_ability);
int portmod_port_phychain_autoneg_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                      const int chain_length, uint32 flags,
                                      const phymod_autoneg_control_t *an_config);
int portmod_port_phychain_autoneg_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                      const int chain_length, uint32 flags,
                                      phymod_autoneg_control_t *an_config,
                                      uint32_t * an_done);
int portmod_port_phychain_autoneg_status_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_autoneg_status_t *status);
int portmod_port_phychain_core_init(int unit, const phymod_core_access_t* core,
                                    const int chain_length,
                                    const phymod_core_init_config_t* init_config,
                                    const phymod_core_status_t* core_status);
int portmod_port_phychain_pll_multiplier_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *core_vco_pll_multiplier);
int portmod_port_phychain_phy_init(int unit, const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   const phymod_phy_init_config_t* init_config);
int portmod_port_phychain_rx_pmd_locked_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         uint32_t *rx_pmd_locked);
int portmod_port_phychain_rx_pmd_lock_counter_get(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length, uint32_t *rx_pmd_lock_count);
int portmod_port_phychain_link_status_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                         const int chain_length, uint32 flags,
                                         uint32_t *link_status);
int portmod_port_phychain_pcs_userspeed_set(int unit, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            const phymod_pcs_userspeed_config_t* config);
int portmod_port_phychain_pcs_userspeed_get(int unit, const phymod_phy_access_t* phy_access,
                                            const int chain_length,
                                            phymod_pcs_userspeed_config_t* config);
int portmod_port_phychain_reg_write(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,
                                    int lane,
                                    uint32_t flags,
                                    uint32_t reg_addr,
                                    uint32_t reg_val);
int portmod_port_phychain_reg_read(int unit, int port, const phymod_phy_access_t* phy_access,
                                   const int chain_length,
                                   int lane,
                                   uint32_t flags,
                                   uint32_t reg_addr,
                                   uint32_t *reg_val);

int portmod_port_phychain_rx_slicer_position_set(int unit, const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 const phymod_slicer_position_t* position);
int portmod_port_phychain_rx_slicer_position_get(int unit, const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position);
int portmod_port_phychain_rx_slicer_position_max_get(int unit, const phymod_phy_access_t* phy_access,
                                                 const int chain_length,
                                                 uint32_t flags,
                                                 phymod_slicer_position_t* position_min,
                                                 phymod_slicer_position_t* position_max);
int portmod_port_phychain_prbs_config_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          const phymod_prbs_t* prbs);
int portmod_port_phychain_prbs_config_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_t* prbs);
int portmod_port_phychain_prbs_enable_set(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t enable);
int portmod_port_phychain_prbs_enable_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          uint32_t *enable);
int portmod_port_phychain_prbs_status_get(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint32_t flags,
                                          phymod_prbs_status_t *prbs_status);
int portmod_port_phychain_pattern_config_set(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             const phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_config_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_enable_set(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t enable,
                                             phymod_pattern_t* pattern);
int portmod_port_phychain_pattern_enable_get(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             uint32_t *enable);
int portmod_port_phychain_core_diagnostics_get(int unit, const phymod_core_access_t* core,
                                               const int chain_length,
                                               phymod_core_diagnostics_t* diag);
int portmod_port_phychain_phy_diagnostics_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              phymod_phy_diagnostics_t *diag);
int portmod_port_phychain_pmd_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type);
int portmod_port_phychain_pcs_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                        const int chain_length,
                                        char *type);
int portmod_port_phychain_meas_lowber_eye(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          phymod_phy_eyescan_options_t eyescan_options,
                                          uint32_t *buffer);
int portmod_port_phychain_display_lowber_eye(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length,
                                             phymod_phy_eyescan_options_t eyescan_options,
                                             uint32_t *buffer);
int portmod_port_phychain_pmd_ber_end_cmd(int unit, const phymod_phy_access_t* phy_access,
                                          const int chain_length,
                                          uint8_t supp_info,
                                          uint32_t timeout_ms);
int portmod_port_phychain_meas_eye_scan_start(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint8_t direction);
int portmod_port_phychain_meas_eye_scan_done(int unit, const phymod_phy_access_t* phy_access,
                                             const int chain_length);
int portmod_port_phychain_read_eye_scan_stripe(int unit, const phymod_phy_access_t* phy_access,
                                               const int chain_length,
                                               uint32_t *buffer,
                                               uint16_t *status);
int portmod_port_phychain_display_eye_scan_header(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index);
int portmod_port_phychain_display_eye_scan_footer(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index);
int portmod_port_phychain_display_eye_scan_stripe(int unit, const phymod_phy_access_t* phy_access,
                                                  const int chain_length,
                                                  uint8_t index,
                                                  uint32_t *buffer);
int portmod_port_phychain_eye_scan_debug_info_dump(int unit, const phymod_phy_access_t* phy_access,
                                                   const int chain_length);

int portmod_port_phychain_timesync_config_set(int unit, int port, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              const portmod_phy_timesync_config_t* config);

int portmod_port_phychain_timesync_config_get(int unit, int port, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, 
                                              portmod_phy_timesync_config_t* config);

int portmod_port_phychain_timesync_enable_set(int unit, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32 enable);
int portmod_port_phychain_timesync_enable_get(int unit, const phymod_phy_access_t* phy_access, 
                                              const int chain_length, uint32* enable);
int portmod_port_phychain_timesync_nco_addend_set(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, uint32 freq_step);
int portmod_port_phychain_timesync_nco_addend_get(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint32* freq_step);
int portmod_port_phychain_timesync_framesync_mode_set(int unit, const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      const phymod_timesync_framesync_t* framesync);
int portmod_port_phychain_timesync_framesync_mode_get(int unit, const phymod_phy_access_t* phy_access, 
                                                      const int chain_length, 
                                                      phymod_timesync_framesync_t* framesync);
int portmod_port_phychain_timesync_local_time_set(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  const uint64 local_time);
int portmod_port_phychain_timesync_local_time_get(int unit, const phymod_phy_access_t* phy_access, 
                                                  const int chain_length, 
                                                  uint64* local_time);
int portmod_port_phychain_timesync_load_ctrl_set(int unit, const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32 load_once, 
                                                 uint32 load_always);

int portmod_port_phychain_timesync_load_ctrl_get(int unit, const phymod_phy_access_t* phy_access, 
                                                 const int chain_length, 
                                                 uint32* load_once, 
                                                 uint32* load_always);

int portmod_port_phychain_timesync_tx_timestamp_offset_set(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset);
int portmod_port_phychain_timesync_tx_timestamp_offset_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset);
int portmod_port_phychain_timesync_rx_timestamp_offset_set(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32 ts_offset);
int portmod_port_phychain_timesync_rx_timestamp_offset_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, 
                                                           uint32* ts_offset);
int portmod_port_phychain_phy_intr_enable_set(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length,
                                       uint32_t enable);
int portmod_port_phychain_phy_intr_enable_get(int unit, const phymod_phy_access_t* phy_access,
                                       const int chain_length, 
                                       uint32_t* enable);
int portmod_port_phychain_phy_intr_status_get(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length,
                                              uint32_t* enable);

int portmod_port_phychain_phy_intr_status_clear(int unit, const phymod_phy_access_t* phy_access,
                                              const int chain_length);
int portmod_port_phychain_phy_timesync_do_sync(int unit, const phymod_phy_access_t* phy_access,
                                               const int chain_length );
int portmod_port_phychain_timesync_capture_timestamp_get(int unit, const phymod_phy_access_t* phy_access, 
                                                         const int chain_length, uint64* cap_ts);
int portmod_port_phychain_timesync_heartbeat_timestamp_get(int unit, const phymod_phy_access_t* phy_access, 
                                                           const int chain_length, uint64* hb_ts);
int portmod_port_phychain_edc_config_set(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         const phymod_edc_config_t* config);
int portmod_port_phychain_edc_config_get(int unit, const phymod_phy_access_t* phy_access,
                                         const int chain_length,
                                         phymod_edc_config_t* config);

int portmod_port_phychain_failover_mode_get(int unit, const phymod_phy_access_t *phy_access, 
                                  int nof_phys, phymod_failover_mode_t *failover);
int portmod_port_phychain_failover_mode_set(int unit, const phymod_phy_access_t *phy_access, 
                                  int nof_phys, phymod_failover_mode_t failover);

int portmod_port_phychain_multi_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                    const int chain_length,  
                                    portmod_multi_get_t* multi_get);

/* common routines between pm4x10 and pm4x25 */
int portmod_phy_port_diag_ctrl (int unit, soc_port_t port, 
                                phymod_phy_access_t *phy_access, 
                                int nof_phys, uint32 inst, int op_type, 
                                int op_cmd, const void *arg);

int portmod_phy_port_reset_get (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int* direction);

int portmod_phy_port_reset_set (int unit, int port, pm_info_t pm_info,
                           int reset_mode, int opcode, int direction);

int portmod_phy_port_prbs_config_set (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      const phymod_prbs_t* config);

int portmod_phy_port_prbs_config_get (int unit, int port, pm_info_t pm_info, 
                                      portmod_prbs_mode_t mode, int flags, 
                                      phymod_prbs_t* config);

int portmod_phy_port_prbs_enable_set(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int enable);

int portmod_phy_port_prbs_enable_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, int* enable);

int portmod_phy_port_prbs_status_get(int unit, int port, pm_info_t pm_info, 
                                     portmod_prbs_mode_t mode, int flags, 
                                     phymod_prbs_status_t* status);

int portmod_phy_port_link_get(int unit, int port, pm_info_t pm_info, int* link);
int portmod_port_phychain_phy_link_up_event(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length);
int portmod_port_phychain_phy_link_down_event(int unit, int port, const phymod_phy_access_t* phy_access,
                                            const int chain_length);

int portmod_phy_port_autoneg_status_get (int unit, int port, pm_info_t pm_info,
                                    phymod_autoneg_status_t* an_status);

int portmod_port_phychain_control_phy_timesync_set(int unit, int port, const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t value);

int portmod_port_phychain_control_phy_timesync_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                                    const int chain_length,
                                                    const portmod_port_control_phy_timesync_t type,
                                                    uint64_t* value);

int
portmod_port_line_to_sys_intf_map(int unit, const phymod_phy_access_t* line_phy_acc, const phymod_phy_access_t* sys_phy_acc,
                                  soc_port_t port, const phymod_phy_inf_config_t *config, int ref_clk,
                                  const uint32 phy_init_flags, phymod_interface_t line_intf,
                                  phymod_interface_t *sys_intf);
int
portmod_port_line_interface_is_valid(int unit, const phymod_phy_access_t* phy_acc, soc_port_t port, int data_rate,
                                  phymod_interface_t line_intf, int num_lanes, uint32_t intf_mode, int *is_valid);

int
portmod_port_line_interface_map_for_xphy(int unit, const phymod_phy_access_t* phy_acc, soc_port_t port,
                                  soc_port_if_t line_intf, soc_port_if_t *mapped_line_intf);

int portmod_port_phymod_xphy_default_interface_get(int unit, const phymod_phy_access_t* phy_acc,
                                                   const portmod_port_interface_config_t* config,
                                                   soc_port_if_t* interface);

int portmod_port_phychain_medium_config_set(int unit, int port, soc_port_medium_t medium,
                                            soc_phy_config_t* config);
int portmod_port_phychain_medium_config_get(int unit, int port, soc_port_medium_t medium,
                                            soc_phy_config_t* config);
int portmod_port_phychain_medium_get(int unit, int port, soc_port_medium_t* medium);
int portmod_port_phychain_master_set(int unit, int port, pm_info_t pm_info, int master_mode);
int portmod_port_phychain_master_get(int unit, int port, pm_info_t pm_info, int* master_mode);
int portmod_port_phychain_operation_mode_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                             const int chain_length, phymod_operation_mode_t* phy_op_mode);
int portmod_port_phychain_timesync_tx_info_get(int unit, int port, const phymod_phy_access_t* phy_access,
                                              const int chain_length, phymod_ts_fifo_status_t* ts_tx_info);

#endif /*_PORTMOD_CHAIN_H_*/
