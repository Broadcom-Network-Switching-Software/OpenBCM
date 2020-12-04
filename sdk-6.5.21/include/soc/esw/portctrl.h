/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        portctrl.h
 * Purpose:     SDK SOC Port Control Glue Layer
 */

#ifndef   _SOC_ESW_PORTCTRL_H_
#define   _SOC_ESW_PORTCTRL_H_

#include <soc/feature.h>
#include <soc/property.h>
#include <soc/phyctrl.h>
#include <soc/error.h>
#include <soc/drv.h>
#include <soc/esw/port.h>

#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod.h>
#endif

#ifdef PORTMOD_SUPPORT
#define SOC_PORTCTRL_NOT_INITIALIZED   (0)
#define SOC_PORTCTRL_INITIALIZED       (1)

/* PORT MACRO PM */
#define SOC_PM12X10_PM4X10_COUNT      (3)
#define SOC_PM4X10_NUM_LANES          (4)
#define SOC_PM4X10_LANE_MASK          (0xF)
#define SOC_PM12X10_NUM_LANES         (10)
#define SOC_PM12X10_LANE_MASK         (0x3FF)
#define SOC_PM4X25_NUM_LANES          (4)
#define SOC_PM4X25_LANE_MASK          (0xF)
#define SOC_PM_QTC_NUM_LANES          (16)
#define SOC_PM_QTC_LANE_MASK          (0xFFFF)
#define SOC_PM_GPHY_NUM_LANES          (8)
#define SOC_PM_GPHY_LANE_MASK          (0xFF)
#define SOC_PM8X50_NUM_LANES          (8)
#define SOC_PM8X50_LANE_MASK          (0xFF)
#define SOC_PM4X10_QTC_NUM_LANES      (4)
#endif

#define SOC_PM_PHY_NAME_MAX_LEN        32

#define SOC_PORTCTRL_HG2_TO_IEEE_BW_GET(_speed_) \
            (_speed_ ==  11000 ?  10000 :       \
            (_speed_ ==  21000 ?  20000 :       \
            (_speed_ ==  27000 ?  25000 :       \
            (_speed_ ==  42000 ?  40000 :       \
            (_speed_ ==  53000 ?  50000 :       \
            (_speed_ == 106000 ? 100000 :       \
            (_speed_ == 127000 ? 120000 : (_speed_))))))))

#define SOC_PORTCTRL_IS_HG2_SPEED(_speed_) \
            (_speed_ ==  11000 ?  1 :       \
            (_speed_ ==  21000 ?  1 :       \
            (_speed_ ==  27000 ?  1 :       \
            (_speed_ ==  42000 ?  1 :       \
            (_speed_ ==  53000 ?  1 :       \
            (_speed_ == 106000 ?  1 :       \
            (_speed_ == 127000 ?  1 : 0)))))))

/* device-specific function vectors */
extern soc_portctrl_functions_t soc_td3_portctrl_func;
extern soc_portctrl_functions_t soc_hr4_portctrl_func;
extern soc_portctrl_functions_t soc_hx5_portctrl_func;
extern soc_portctrl_functions_t soc_mv2_portctrl_func;
extern soc_portctrl_functions_t soc_fb6_portctrl_func;
extern soc_portctrl_functions_t soc_td2p_portctrl_func;
extern soc_portctrl_functions_t soc_th_portctrl_func;
extern soc_portctrl_functions_t soc_gh_portctrl_func;
extern soc_portctrl_functions_t soc_fl_portctrl_func;


/*
 * Define:
 *      PORTMOD_SUCCESS
 *      PORTMOD_FAILURE
 *      PORTMOD_IF_ERROR_RETURN
 * Purpose:
 *      Checks PortMod and PhyMod library error return code.
 *      PORTMOD_IF_ERROR_RETURN() causes routine to return on error.
 * Note:
 *      Currently, the PortMod functions return error codes are the same as
 *      the SHARED error codes, which are used by the BCM and SOC layers.
 *      Phymod functions error codes are also the same as the
 *      SHARED error codes.
 */
#define PORTMOD_SUCCESS(_rv)           _SHR_E_SUCCESS(_rv)
#define PORTMOD_FAILURE(_rv)           _SHR_E_FAILURE(_rv)
#define PORTMOD_IF_ERROR_RETURN(_op)   _SHR_E_IF_ERROR_RETURN(_op)

#define SOC_PORT_RESOURCE_CONFIGURE_FLEX       (0)
#define SOC_PORT_RESOURCE_CONFIGURE_SPEED_ONLY (1)
#define SOC_PORT_RESOURCE_CONFIGURE_LANES_ONLY (2)

/* This structrure is used to pass information to Portmod API during Flexport
 * reconfiguration of PGW. NOte it is used for TD2+ only
 */
typedef struct soc_esw_portctrl_pgw_s {
    int mode;
    int lanes;
    int port_index;
    int flags;
} soc_esw_portctrl_pgw_t;

/* Port type used in PortMod functions */
typedef int portctrl_pport_t;

extern int
soc_esw_portctrl_init(int unit);
extern int
soc_esw_portctrl_deinit(int unit);

extern int
soc_portctrl_software_deinit(int unit);

extern int
soc_esw_portctrl_add(int unit, soc_port_t port, int init_flag,
                     void *add_info);

extern int
soc_esw_portctrl_delete(int unit, soc_port_t port);

extern int
soc_esw_portctrl_pgw_reconfigure(int unit, soc_port_t port,
                                 int phy_port, soc_esw_portctrl_pgw_t *data);

extern int
soc_esw_portctrl_duplex_get(int unit, soc_port_t port, int *duplex);

extern int
soc_esw_portctrl_duplex_set(int unit, soc_port_t port, int duplex);

extern int
soc_esw_portctrl_port_fc_config_init(int unit, soc_port_t port);

extern int
soc_esw_portctrl_frame_max_set(int unit, soc_port_t port, int size);

extern int
soc_esw_portctrl_frame_max_get(int unit, soc_port_t port, int *size);

extern int
soc_esw_portctrl_pause_set(int unit, soc_port_t port, int pause_tx, int pause_rx);

extern int
soc_esw_portctrl_pause_get(int unit, soc_port_t port, int *pause_tx, int *pause_rx);

extern int
soc_esw_portctrl_ifg_set(int unit, soc_port_t port, int speed,
                         soc_port_duplex_t duplex, int ifg);

extern int
soc_esw_portctrl_ifg_get(int unit, soc_port_t port, int speed,
                         soc_port_duplex_t duplex, int *ifg);

extern int
soc_esw_portctrl_mac_reset_set(int unit, soc_port_t port, int val);

extern int
soc_portctrl_led_chain_config(int unit, int port, int value);

extern int
soc_portctrl_phy_name_get(int unit, int port, char *phy_name);

extern int
soc_portctrl_port_mode_set(int unit, int port, int mode);
extern int
soc_portctrl_port_mode_get(int unit, int port, int *mode, int *lanes);

extern int
soc_portctrl_phy_control_set(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 value);
extern int
soc_portctrl_phy_control_get(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 *value);
extern int
soc_portctrl_phy_timesync_config_set(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf);
extern int
soc_portctrl_phy_timesync_config_get(int unit, soc_port_t port,
                                     soc_port_phy_timesync_config_t *conf);
extern int
soc_portctrl_control_phy_timesync_set(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 value);
extern int
soc_portctrl_control_phy_timesync_get(int unit, soc_port_t port,
                                      soc_port_control_phy_timesync_t type,
                                      uint64 *value);

extern int
soc_esw_portctrl_encap_get(int unit, soc_port_t port, int *mode);

extern int
soc_esw_portctrl_port_to_phyaddr(int unit, soc_port_t port, uint8* phy_addr);

extern int
soc_portctrl_pm_type_get(int unit, soc_port_t phy_port, int* pm_type);

extern int
soc_esw_portctrl_functions_register(int unit, soc_driver_t *drv);

extern int
soc_esw_portctrl_speed_get(int unit, soc_port_t port, int *speed);

int
soc_esw_portctrl_is_same_speed_class(int unit, int pre_speed, int post_speed);

extern soc_error_t
soc_esw_portctrl_port_ability_update(int unit, soc_port_t port, soc_port_ability_t *ability);

#ifdef PORTMOD_SUPPORT
extern int
soc_esw_portctrl_init_check(int unit);

/* utility functions called from device specific portmod code */
extern int
soc_esw_portctrl_pm_user_access_alloc(int unit, int num,
                   portmod_default_user_access_t **user_acc);
extern void
soc_esw_portctrl_dump_txrx_lane_map(int unit, int first_port, int logical_port,
                   int core_num, uint32 txlane_map_b, uint32 rxlane_map_b,
                   uint32 txlane_map, uint32 rxlane_map);

extern int soc_esw_portctrl_xphy_wb_db_restore(int unit);
extern int soc_esw_portctrl_setup_ext_phy_add(int unit, int port, portmod_pbmp_t *phy_ports);
extern int soc_esw_portctrl_reset_tsc0_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc1_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc2_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_tsc3_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_qtsce_cb(int unit, int port, uint32 in_reset);
extern int soc_esw_portctrl_reset_gphy_cb(int unit, int port, uint32 in_reset);

extern int
soc_esw_portctrl_config_get(int unit, soc_port_t port, 
                            portmod_port_interface_config_t* interface_config,
                            portmod_port_init_config_t* init_config, 
                            phymod_operation_mode_t *phy_op_mode);

extern int
soc_esw_portctrl_pm_update_vco_soc_info(int unit, const int *phy_ports,
                                   int num_ports,
                                   int pm_id,
                                   portmod_dispatch_type_t pm_type,
                                   uint32 flags);
extern int
soc_esw_portctrl_update_vco_soc_info(int unit);

extern int
soc_esw_portctrl_pm_vco_store(int unit, int index, const void *pm_vco_info);


extern int
soc_esw_portctrl_speed_config_fill(int unit, soc_port_t port,
                int speed,
                int num_lane,
                int fec,
                int link_training,
                int lane_config,
                void *speed_config);

extern int
soc_esw_portmod_port_vcos_get(int unit, soc_port_t phy_port ,
                              portmod_dispatch_type_t pm_type, int* lanes,
                              int* speed , int* encap ,int size, int flex, portmod_pbmp_t flexing_ports);
#endif /* PORTMOD_SUPPORT */
extern int
soc_esw_portctrl_flex_vco_validation(int unit,
                soc_port_schedule_state_t *port_schedule_state);

extern int
soc_esw_portctrl_pm_vco_fetch(int unit, int index, void *pm_vco_info);

extern int
soc_esw_portctrl_pm_vco_store_clear(int unit);

extern int
soc_esw_portctrl_pm_vco_reconfigure(int unit);

extern int
soc_esw_portctrl_sbus_bcast_setup(int unit);

extern int
soc_esw_portctrl_pll_div_get(int unit, soc_port_t logic_port, soc_port_t phy_port,
                             int speed, int num_lane, int encap_mode,
                             uint32 *pll_div);

extern int
soc_esw_portctrl_pm_ports_delete(int unit,
                               int nport,
                               soc_port_resource_t *resource);

extern int
soc_esw_portctrl_pm_ports_add(int unit,
                               int nport,
                               soc_port_resource_t *resource);
extern int
soc_esw_portctrl_phy_tx_set(int unit, soc_port_t port,
                            int phyn, int phy_lane,
                            int sys_side, soc_port_phy_tx_t *tx);

extern int
soc_esw_portctrl_phy_tx_get(int unit, soc_port_t port,
                            int phyn, int phy_lane,
                            int sys_side, soc_port_phy_tx_t *tx);

extern int 
soc_esw_portctrl_portmod_callback_unregister(int unit, soc_port_t port);

#endif /* _SOC_ESW_PORTCTRL_H_ */

