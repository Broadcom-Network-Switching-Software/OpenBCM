/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef PORTMOD_LEGACY_PHY_H
#define PORTMOD_LEGACY_PHY_H

int 
portmod_port_legacy_phy_probe(int unit, int port);
int
portmod_port_legacy_phy_init(int unit, int port);
int
portmod_port_legacy_phy_addr_get(int unit, int port);
int 
portmod_port_legacy_loopback_set(int unit, int port, phymod_loopback_mode_t lb, uint32_t enable);
int
portmod_port_legacy_loopback_get(int unit, int port, phymod_loopback_mode_t lb, uint32_t *enable);
int 
portmod_port_legacy_interface_config_set(int unit, int port, const phymod_phy_inf_config_t *config);
int 
portmod_port_legacy_interface_config_get(int unit, int port, phymod_phy_inf_config_t *config);
int 
portmod_port_legacy_power_set(int unit, int port, const phymod_phy_power_t *power);
int 
portmod_port_legacy_power_get(int unit, int port, phymod_phy_power_t *power); 
int 
portmod_port_legacy_tx_ln_ctrl_get(int unit, int port, phymod_phy_tx_lane_control_t* control); 
int 
portmod_port_legacy_tx_ln_ctrl_set(int unit, int port, const phymod_phy_tx_lane_control_t* control);
int 
portmod_port_legacy_rx_ln_ctrl_get(int unit, int port, phymod_phy_rx_lane_control_t* control);
int 
portmod_port_legacy_rx_ln_ctrl_set(int unit, int port, const phymod_phy_rx_lane_control_t* control);
int 
portmod_port_legacy_phy_control_set(int unit, int port, int phyn, int phy_lane, int sys_side,
                                    soc_phy_control_t control, uint32_t value);
int 
portmod_port_legacy_phy_control_get(int unit, int port, int phyn, int phy_lane, int sys_side,
                                        soc_phy_control_t control, uint32_t *value); 
int 
portmod_port_legacy_phy_link_get(int unit, int port, uint32_t *link);
int 
portmod_port_legacy_cl72_set(int unit, int port, uint32_t cl72_en);
int 
portmod_port_legacy_cl72_get(int unit, int port, uint32_t *cl72_en);
int 
portmod_port_legacy_fec_get(int unit, int port, uint32_t *fec_en);
int 
portmod_port_legacy_fec_set(int unit, int port, uint32_t fec_en);
int 
portmod_port_legacy_eee_set(int unit, int port, uint32_t eee_en);
int 
portmod_port_legacy_eee_get(int unit, int port, uint32_t *eee_en);
int 
portmod_port_legacy_advert_set(int unit, int port, const portmod_port_ability_t* portmod_ability);
int 
portmod_port_legacy_advert_get(int unit, int port, portmod_port_ability_t* portmod_ability);
int
portmod_port_legacy_ability_remote_get(int unit, int port, portmod_port_ability_t * portmod_ability);
int 
portmod_port_legacy_ability_local_get(int unit, int port, portmod_port_ability_t *portmod_ability);
int
portmod_port_legacy_duplex_get(int unit, int port, int* duplex);
int
portmod_port_legacy_duplex_set(int unit, int port,  int duplex);
int
portmod_port_legacy_an_get(int unit, int port, phymod_autoneg_control_t *an_config, uint32_t *an_done);
int
portmod_port_legacy_an_status_get(int unit, int port, phymod_autoneg_status_t *an_status);
int
portmod_port_legacy_an_set(int unit, int port,  const phymod_autoneg_control_t *an_config);
int portmod_port_legacy_tx_set(const phymod_phy_access_t* phy_access, const int chain_length, const phymod_tx_t* tx);
int
portmod_port_legacy_phy_mdix_set(int unit, int port, soc_port_mdix_t mode);
int
portmod_port_legacy_phy_mdix_get(int unit, int port, soc_port_mdix_t *mode);
int
portmod_port_legacy_phy_mdix_status_get(int unit, soc_port_t port,
                                             soc_port_mdix_status_t *status);
int
portmod_legacy_ext_phy_init(int unit, pbmp_t pbmp);
int
portmod_port_legacy_phy_reset_set(int unit, int port);

int portmod_port_legacy_timesync_config_set(int unit, int port,
                                            const portmod_phy_timesync_config_t* config);
int portmod_port_legacy_timesync_config_get(int unit, int port,
                                            portmod_phy_timesync_config_t* config);
int portmod_port_legacy_control_phy_timesync_set( int unit, int port,
                                                  const soc_port_control_phy_timesync_t type,
                                                  const uint64 value);

int portmod_port_legacy_control_phy_timesync_get( int unit, int port,
                                                  const soc_port_control_phy_timesync_t type,
                                                  const uint64* value);
int portmod_port_legacy_phy_link_up_event(int unit, int port);
int portmod_port_legacy_phy_link_down_event(int unit, int port);

int portmod_port_legacy_medium_config_set(int unit, int port, soc_port_medium_t medium,
                                          soc_phy_config_t *cfg);
int portmod_port_legacy_medium_config_get(int unit, int port, soc_port_medium_t medium,
                                          soc_phy_config_t *cfg);
int portmod_port_legacy_medium_get(int unit, int port, soc_port_medium_t* medium);

int portmod_port_legacy_reg_read(int unit, int port, uint32 flags, uint32 phy_addr, uint32 *phy_data);
int portmod_port_legacy_reg_write(int unit, int port, uint32 flags, uint32 phy_addr, uint32 phy_data);

int portmod_port_legacy_multi_get(int unit, soc_port_t port, uint32 flags,
                                  uint32 dev_addr, uint32 offset, int max_size, uint8 *data, int *actual_size);

int portmod_port_legacy_is_skip_spd_sync(int unit, int port);

int portmod_port_legacy_phy_drv_name_get(int unit, int port,
                                         char *name, int len);
int portmod_port_legacy_phy_flags_test(int unit, int port, int flag);
int portmod_port_legacy_master_set(int unit, int port, int master_mode);
int portmod_port_legacy_master_get(int unit, int port, int* master_mode);
int portmod_port_legacy_op_mode_get(int unit, int port, phymod_operation_mode_t* phy_op_mode);
#endif /* PORTMOD_LEGACY_PHY_H*/
