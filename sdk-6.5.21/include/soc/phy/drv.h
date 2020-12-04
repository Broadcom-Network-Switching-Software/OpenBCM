/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        drv.h
 * Purpose:     Link Layer interface (PHY)
 */

#ifndef _PHY_DRV_H
#define _PHY_DRV_H

#include <shared/port.h>
#include <shared/phyconfig.h>

#include <soc/defs.h>
#include <soc/macipadr.h>
#include <soc/portmode.h>
#include <soc/phyctrl.h>
#include <soc/phy/phyctrl.h>
#include <soc/port_ability.h>
#include <soc/timesync.h>
#include <soc/oam.h>

/*
 * Typedef:
 *      phy_driver_t
 * Purpose:
 *      Maps out entry points into PHY drivers.
 * Notes:
 *      Supplied functions are:
 *
 *      pd_init         - Initialize the PHY to a known good state.
 *      pd_reset        - Reset and setup the PHY (used by pd_init, after
 *                        link_down, etc.)
 *      pd_link_get     - Retrieve current link status from PHY.
 *      pd_enable_set   - Enable/Disable the PHY physical interface.
 *      pd_enable_get   - Retrieve current PHY Enable/disable mode.
 *      pd_duplex_set   - Set the non-negotiated duplex mode (does not
 *                        disable autonegotiation at this layer).
 *      pd_duplex_get   - Get the current operating duplicity (if
 *                        auto-negotiation is enabled, negotiated speed
 *                        returned, otherwise fixed speed returned).
 *      pd_speed_set    - Set the non-negotiated speed in the PHY (does
 *                        not disable auto-negotiation at this layer).
 *      pd_speed_get    - Get the current operating speed (if
 *                        auto-negotiation is enabled, negotiated speed
 *                        returned, otherwise fixed speed returned).
 *      pd_master_set   - Set the master/slave mode for PHYs that need it
 *                        (does not disable auto-negotiation at this layer).
 *      pd_master_get   - Get the current master/slave mode for the PHY
 *                        (if auto-negotiation is enabled, negotiated master
 *                        returned, otherwise fixed master returned).
 *      pd_an_set       - Enable/Disable autonegotiation
 *      pd_an_get       - Retrieve current autonegotiation settings.
 *      pd_adv_local_set- Set local advertisement (does not enable
 *                        autonegotiation).
 *      pd_adv_local_get- Get local advertisement for auto-negotiation.
 *      pd_adv_remote_get- Retrieve remote advertisement if auto-negotiation
 *                        is enabled.
 *      pd_lb_set       - Enable/Disable PHY loopback.
 *      pd_lb_get       - Retrieve current PHY loopback mode.
 *      pd_interface_set- Set MAC-PHY interface type (SOC_PORT_IF_*)
 *      pd_interface_get- Get MAC-PHY interface type
 *      pd_ability      - Retrieve PHY abilities.
 *      pd_linkup_evt   - Call back for link up event
 *      pd_linkdn_evt   - Call back for link down event
 *      pd_mdix_set          - Set MDIX mode for the port
 *      pd_mdix_get          - Get the MDIX mode for the port
 *      pd_mdix_status_get   - Get the MDI crossover status for the port
 *      pd_medium_config_set - Set medium-dependent configuration
 *      pd_medium_config_get - Get medium-dependent configuration
 *      pd_medium_get        - Get active medium
 *      pd_cable_diag   - Run Cable Diagnostics
 *      pd_control_set  - Set PHY specific control settings
 *      pd_control_get  - Get PHY specific control settings
 *
 * All routines return SOC_E_XXX values.
 * Null pointers for routines will cause SOC_E_UNAVAIL to be returned.
 *
 * Macros are provided to access the fields.
 */
typedef struct phy_driver_s {
    char *drv_name;
    int  (*pd_init)(int, soc_port_t);
    int  (*pd_reset)(int, soc_port_t, void *);
    int  (*pd_link_get)(int, soc_port_t, int *);
    int  (*pd_enable_set)(int, soc_port_t, int);
    int  (*pd_enable_get)(int, soc_port_t, int *);
    int  (*pd_duplex_set)(int, soc_port_t, int);
    int  (*pd_duplex_get)(int, soc_port_t, int *);
    int  (*pd_speed_set)(int, soc_port_t, int);
    int  (*pd_speed_get)(int, soc_port_t, int *);
    int  (*pd_master_set)(int, soc_port_t, int);
    int  (*pd_master_get)(int, soc_port_t, int *);
    int  (*pd_an_set)(int, soc_port_t, int);
    int  (*pd_an_get)(int, soc_port_t, int *, int *);
    int  (*pd_adv_local_set)(int, soc_port_t, soc_port_mode_t);
    int  (*pd_adv_local_get)(int, soc_port_t, soc_port_mode_t *);
    int  (*pd_adv_remote_get)(int, soc_port_t, soc_port_mode_t *);
    int  (*pd_lb_set)(int, soc_port_t, int);
    int  (*pd_lb_get)(int, soc_port_t, int *);
    int  (*pd_interface_set)(int, soc_port_t, soc_port_if_t);
    int  (*pd_interface_get)(int, soc_port_t, soc_port_if_t *);
    int  (*pd_ability)(int, soc_port_t, soc_port_mode_t *);
    int  (*pd_linkup_evt)(int, soc_port_t);
    int  (*pd_linkdn_evt)(int, soc_port_t);
    int  (*pd_mdix_set)(int, soc_port_t, soc_port_mdix_t);
    int  (*pd_mdix_get)(int, soc_port_t, soc_port_mdix_t *);
    int  (*pd_mdix_status_get)(int, soc_port_t,
                               soc_port_mdix_status_t *);
    int  (*pd_medium_config_set)(int, soc_port_t, soc_port_medium_t,
                                 soc_phy_config_t *);
    int  (*pd_medium_config_get)(int, soc_port_t, soc_port_medium_t,
                                 soc_phy_config_t *);
    int  (*pd_medium_get)(int, soc_port_t, soc_port_medium_t *);
    int  (*pd_cable_diag)(int, soc_port_t, soc_port_cable_diag_t *);
    int  (*pd_link_change)(int, soc_port_t, int *);
    int  (*pd_control_set)(int, soc_port_t, soc_phy_control_t, uint32); 
    int  (*pd_control_get)(int, soc_port_t, soc_phy_control_t, uint32 *); 
    int  (*pd_reg_read)(int, soc_port_t, uint32, uint32, uint32 *);
    int  (*pd_reg_write)(int, soc_port_t, uint32, uint32, uint32);
    int  (*pd_reg_modify)(int, soc_port_t, uint32, uint32, uint32, uint32);
    int  (*pd_notify)(int, soc_port_t, soc_phy_event_t, uint32);
    int  (*pd_probe)(int, phy_ctrl_t *);
    int  (*pd_ability_advert_set)(int, soc_port_t, soc_port_ability_t *);
    int  (*pd_ability_advert_get)(int, soc_port_t, soc_port_ability_t *);
    int  (*pd_ability_remote_get)(int, soc_port_t, soc_port_ability_t *);
    int  (*pd_ability_local_get)(int, soc_port_t, soc_port_ability_t *);
    int  (*pd_firmware_set)(int,soc_port_t,int,uint8 *,int);
    int  (*pd_timesync_config_set)(int, soc_port_t, soc_port_phy_timesync_config_t *);
    int  (*pd_timesync_config_get)(int, soc_port_t, soc_port_phy_timesync_config_t *);
    int  (*pd_timesync_control_set)(int, soc_port_t, soc_port_control_phy_timesync_t, uint64);
    int  (*pd_timesync_control_get)(int, soc_port_t, soc_port_control_phy_timesync_t, uint64 *);
    int  (*pd_diag_ctrl)(int,soc_port_t,uint32,int,int,void *);
    int  (*pd_lane_control_set)(int, soc_port_t, int, soc_phy_control_t, uint32); 
    int  (*pd_lane_control_get)(int, soc_port_t, int, soc_phy_control_t, uint32 *); 
    int  (*pd_lane_reg_read)(int, soc_port_t, int, uint32, uint32, uint32 *);
    int  (*pd_lane_reg_write)(int, soc_port_t, int, uint32, uint32, uint32);
    int  (*pd_oam_config_set)(int, soc_port_t, soc_port_config_phy_oam_t *);
    int  (*pd_oam_config_get)(int, soc_port_t, soc_port_config_phy_oam_t *);
    int  (*pd_oam_control_set)(int, soc_port_t, soc_port_control_phy_oam_t, uint64);
    int  (*pd_oam_control_get)(int, soc_port_t, soc_port_control_phy_oam_t, uint64 *);
    int  (*pd_timesync_enhanced_capture_get)(int, soc_port_t, soc_port_phy_timesync_enhanced_capture_t *);
    int  (*pd_multi_get)(int, soc_port_t, uint32, uint32, uint32, int, uint8 *, int *);
    int  (*pd_precondition_before_probe)(int, phy_ctrl_t *);
    int  (*pd_linkfault_get)(int, soc_port_t, int *);
    int  (*pd_synce_clk_ctrl_set)(int, soc_port_t, uint32, uint32, uint32);
    int  (*pd_synce_clk_ctrl_get)(int, soc_port_t, uint32 *, uint32 *, uint32 *);
    int  (*pd_fec_error_inject_set)(int, soc_port_t, uint16, soc_phy_control_fec_error_mask_t);
    int  (*pd_fec_error_inject_get)(int, soc_port_t, uint16 *, soc_phy_control_fec_error_mask_t *);
} phy_driver_t;

#define _PHY_CALL(_pd, _pf, _pa) \
        ((_pd) == 0 ? SOC_E_PARAM : \
         ((_pd)->_pf == 0 ? SOC_E_UNAVAIL : (_pd)->_pf _pa))

#define PHY_INIT(_pd, _u, _p) \
        _PHY_CALL((_pd), pd_init, ((_u), (_p)))

#define PHY_RESET(_pd, _u, _p) \
        _PHY_CALL((_pd), pd_reset, ((_u), (_p), NULL))

#define PHY_LINK_GET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_link_get, ((_u), (_p), (_l)))

#define PHY_LINKFAULT_GET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_linkfault_get, ((_u), (_p), (_l)))

#define PHY_ENABLE_SET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_enable_set, ((_u), (_p), (_l)))

#define PHY_ENABLE_GET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_enable_get, ((_u), (_p), (_l)))

#define PHY_DUPLEX_SET(_pd, _u, _p, _d) \
        _PHY_CALL((_pd), pd_duplex_set, ((_u), (_p), (_d)))

#define PHY_DUPLEX_GET(_pd, _u, _p, _d) \
        _PHY_CALL((_pd), pd_duplex_get, ((_u), (_p), (_d)))

#define PHY_SPEED_SET(_pd, _u, _p, _s) \
        _PHY_CALL((_pd), pd_speed_set, ((_u), (_p), (_s)))

#define PHY_SPEED_GET(_pd, _u, _p, _s) \
        _PHY_CALL((_pd), pd_speed_get, ((_u), (_p), (_s)))

#define PHY_MASTER_SET(_pd, _u, _p, _s) \
        _PHY_CALL((_pd), pd_master_set, ((_u), (_p), (_s)))

#define PHY_MASTER_GET(_pd, _u, _p, _s) \
        _PHY_CALL((_pd), pd_master_get, ((_u), (_p), (_s)))

#define PHY_AUTO_NEGOTIATE_SET(_pd, _u, _p, _a) \
        _PHY_CALL((_pd), pd_an_set, ((_u), (_p), (_a)))

#define PHY_AUTO_NEGOTIATE_GET(_pd, _u, _p, _a, _b) \
        _PHY_CALL((_pd), pd_an_get, ((_u), (_p), (_a), (_b)))

#define PHY_ADV_SET(_pd, _u, _p, _a) \
        _PHY_CALL((_pd), pd_adv_local_set, ((_u), (_p), (_a)))

#define PHY_ADV_GET(_pd, _u, _p, _a) \
        _PHY_CALL((_pd), pd_adv_local_get, ((_u), (_p), (_a)))

#define PHY_LOOPBACK_SET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_lb_set, ((_u), (_p), (_l)))

#define PHY_LOOPBACK_GET(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_lb_get, ((_u), (_p), (_l)))

#define PHY_INTERFACE_SET(_pd, _u, _p, _i) \
        _PHY_CALL((_pd), pd_interface_set, ((_u), (_p), (_i)))

#define PHY_INTERFACE_GET(_pd, _u, _p, _i) \
        _PHY_CALL((_pd), pd_interface_get, ((_u), (_p), (_i)))

#define PHY_ABILITY_GET(_pd, _u, _p, _m) \
        _PHY_CALL((_pd), pd_ability, ((_u), (_p), (_m)))

#define PHY_LINKUP_EVT(_pd, _u, _p) \
        _PHY_CALL((_pd), pd_linkup_evt, ((_u), (_p)))

#define PHY_LINKDN_EVT(_pd, _u, _p) \
        _PHY_CALL((_pd), pd_linkdn_evt, ((_u), (_p)))

#define PHY_ADV_REMOTE_GET(_pd, _u, _p, _m) \
        _PHY_CALL((_pd), pd_adv_remote_get, ((_u), (_p), (_m)))

#define PHY_MDIX_SET(_pd, _u, _p, _m) \
        _PHY_CALL((_pd), pd_mdix_set, ((_u), (_p), (_m)))

#define PHY_MDIX_GET(_pd, _u, _p, _m) \
        _PHY_CALL((_pd), pd_mdix_get, ((_u), (_p), (_m)))

#define PHY_MDIX_STATUS_GET(_pd, _u, _p, _s) \
        _PHY_CALL((_pd), pd_mdix_status_get, ((_u), (_p), (_s)))

#define PHY_MEDIUM_CONFIG_SET(_pd, _u, _p, _m, _c) \
        _PHY_CALL((_pd), pd_medium_config_set, ((_u), (_p), (_m), (_c)))

#define PHY_MEDIUM_CONFIG_GET(_pd, _u, _p, _m, _c) \
        _PHY_CALL((_pd), pd_medium_config_get, ((_u), (_p), (_m), (_c)))

#define PHY_MEDIUM_GET(_pd, _u, _p, _m) \
        _PHY_CALL((_pd), pd_medium_get, ((_u), (_p), (_m)))

#define PHY_CABLE_DIAG(_pd, _u, _p, _status) \
        _PHY_CALL((_pd), pd_cable_diag, ((_u), (_p), (_status)))

#define PHY_LINK_CHANGE(_pd, _u, _p, _l) \
        _PHY_CALL((_pd), pd_link_change, ((_u), (_p), (_l)))

#define PHY_CONTROL_SET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_control_set, ((_u), (_p), (_t), (_v)))

#define PHY_CONTROL_GET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_control_get, ((_u), (_p), (_t), (_v)))

#define PHY_REG_READ(_pd, _u, _p, _f, _a, _v) \
        _PHY_CALL((_pd), pd_reg_read, ((_u), (_p), (_f), (_a), (_v)))

#define PHY_REG_WRITE(_pd, _u, _p, _f, _a, _v) \
        _PHY_CALL((_pd), pd_reg_write, ((_u), (_p), (_f), (_a), (_v)))

#define PHY_REG_MODIFY(_pd, _u, _p, _f, _a, _v, _m) \
        _PHY_CALL((_pd), pd_reg_modify, ((_u), (_p), (_f), (_a), (_v), (_m)))

#define PHY_NOTIFY(_pd, _u, _p, _e, _v) \
        _PHY_CALL((_pd), pd_notify, ((_u), (_p), (_e), (_v)))

#define PHY_PROBE(_pd, _u, _pc) \
        _PHY_CALL((_pd), pd_probe, ((_u), (_pc)))

#define PHY_PRECONDITION_BEFORE_PROBE(_pd, _u, _pc) \
        _PHY_CALL((_pd), pd_precondition_before_probe, ((_u), (_pc)))

#define PHY_ABILITY_ADVERT_SET(_pd, _u, _p, _pa) \
        _PHY_CALL((_pd), pd_ability_advert_set, ((_u), (_p), (_pa)))

#define PHY_ABILITY_ADVERT_GET(_pd, _u, _p, _pa) \
        _PHY_CALL((_pd), pd_ability_advert_get, ((_u), (_p), (_pa)))

#define PHY_ABILITY_REMOTE_GET(_pd, _u, _p, _pa) \
        _PHY_CALL((_pd), pd_ability_remote_get, ((_u), (_p), (_pa)))
 
#define PHY_ABILITY_LOCAL_GET(_pd, _u, _p, _pa) \
        _PHY_CALL((_pd), pd_ability_local_get, ((_u), (_p), (_pa)))

#define PHY_FIRMWARE_SET(_pd, _u, _p, _o, _a, _l) \
        _PHY_CALL((_pd), pd_firmware_set, ((_u),(_p),(_o),(_a),(_l)))

#define PHY_TIMESYNC_CONFIG_SET(_pd, _u, _p, _c) \
        _PHY_CALL((_pd), pd_timesync_config_set, ((_u),(_p),(_c)))

#define PHY_TIMESYNC_CONFIG_GET(_pd, _u, _p, _c) \
        _PHY_CALL((_pd), pd_timesync_config_get, ((_u),(_p),(_c)))

#define PHY_TIMESYNC_CONTROL_SET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_timesync_control_set, ((_u),(_p),(_t),(_v)))

#define PHY_TIMESYNC_CONTROL_GET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_timesync_control_get, ((_u),(_p),(_t),(_v)))

#define PHY_TIMESYNC_ENHANCED_CAPTURE_GET(_pd, _u, _p, _c) \
        _PHY_CALL((_pd), pd_timesync_enhanced_capture_get, ((_u),(_p),(_c)))

#define PHY_DIAG_CTRL(_pd, _u, _p, _i, _o, _c,_a) \
        _PHY_CALL((_pd), pd_diag_ctrl, ((_u),(_p),(_i),(_o),(_c),(_a)))

#define PHY_LANE_CONTROL_SET(_pd, _u, _p, _l, _t, _v) \
        _PHY_CALL((_pd), pd_lane_control_set, ((_u), (_p), (_l), (_t), (_v)))

#define PHY_LANE_CONTROL_GET(_pd, _u, _p, _l, _t, _v) \
        _PHY_CALL((_pd), pd_lane_control_get, ((_u), (_p), (_l), (_t), (_v)))

#define PHY_LANE_REG_READ(_pd, _u, _p, _l, _f, _a, _v) \
        _PHY_CALL((_pd), pd_lane_reg_read, \
                                ((_u), (_p), (_l), (_f), (_a), (_v)))

#define PHY_LANE_REG_WRITE(_pd, _u, _p, _l, _f, _a, _v) \
        _PHY_CALL((_pd), pd_lane_reg_write, \
                                ((_u), (_p), (_l), (_f), (_a), (_v)))
#define PHY_OAM_CONFIG_SET(_pd, _u, _p, _c) \
        _PHY_CALL((_pd), pd_oam_config_set, ((_u),(_p),(_c)))

#define PHY_OAM_CONFIG_GET(_pd, _u, _p, _c) \
        _PHY_CALL((_pd), pd_oam_config_get, ((_u),(_p),(_c)))

#define PHY_OAM_CONTROL_SET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_oam_control_set, ((_u),(_p),(_t),(_v)))

#define PHY_OAM_CONTROL_GET(_pd, _u, _p, _t, _v) \
        _PHY_CALL((_pd), pd_oam_control_get, ((_u),(_p),(_t),(_v)))

#define PHY_MULTI_GET(_pd, _u, _p, _f, _a, _o, _m, _d, _s) \
        _PHY_CALL((_pd), pd_multi_get, ((_u),(_p), (_f), (_a), (_o), (_m), (_d), (_s)))

#define PHY_SYNCE_CLK_SET(_pd, _u, _p, _m0, _m1, _v) \
        _PHY_CALL((_pd), pd_synce_clk_ctrl_set, ((_u),(_p),(_m0),(_m1),(_v)))

#define PHY_SYNCE_CLK_GET(_pd, _u, _p, _m0, _m1, _v) \
        _PHY_CALL((_pd), pd_synce_clk_ctrl_get, ((_u),(_p),(_m0),(_m1),(_v)))

#define PHY_FEC_ERROR_INJECT_SET(_pd, _u, _p, _e, _b) \
        _PHY_CALL((_pd), pd_fec_error_inject_set, ((_u),(_p),(_e), (_b)))

#define PHY_FEC_ERROR_INJECT_GET(_pd, _u, _p, _e, _b) \
        _PHY_CALL((_pd), pd_fec_error_inject_get, ((_u),(_p),(_e), (_b)))

extern  int soc_phy_nocxn(int unit, phy_driver_t **phyd_ptr);

#endif  /* _PHY_DRV_H */
