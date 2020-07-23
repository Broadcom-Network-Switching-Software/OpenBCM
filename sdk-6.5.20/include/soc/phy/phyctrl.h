/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phyctrl.h
 * Purpose:     Data structures and function prototypes for PHY configuration.
 * Note:        This header file is for PHY driver module. Should not include
 *              this header file in SOC or BCM layers except
 *              soc/common/phyctrl.c
 */

#ifndef _PHY_PHYCTRL_H_
#define _PHY_PHYCTRL_H_

#include <sal/core/time.h>
#include <soc/types.h>
#include <soc/portmode.h>
#ifdef PHYMOD_SUPPORT
#include <soc/phy/phymod_ctrl.h>
#endif

/* Data Structures */
typedef struct phy_ctrl_s {
    int                 unit;               /* BCM unit number */
    soc_port_t          port;               /* BCM port number */
    uint16              phy_id0;            /* PHY ID0 register */
    uint16              phy_id1;            /* PHY ID1 register */
    uint16              phy_model;          /* PHY model from ID reg */
    uint16              phy_rev;            /* PHY revision from ID reg */
    uint32              phy_oui;            /* PHY oui from ID reg */
    uint16              phy_id;             /* PHY ID on MDIO bus */
    uint8               block_init_flag;    /* Block Init Flag */
    uint8               lane_num;           /* Lane number in a device */ 
    uint8               phy_mode;           /* configuration mode */
    uint16              chip_num;           /* chip number */ 
    int                 speed_max;          /* Restrict support to speed max */ 
    uint8               ledmode[4];         /* LED1..4 selectors */
    uint16              ledctrl;            /* LED control */
    uint16              ledselect;          /* LED select */
    int                 automedium;         /* Medium auto-select active */
    int                 ext_phy_autodetect_en; /* Enable/disable autodetect */
    int                 ext_phy_fiber_iface; /* Fiber Iface when Autodetect is disabled*/
    int                 tbi_capable;        /* PHY & SOC both support TBI */
    int                 medium;             /* Last active medium type */
    int                 fiber_detect;       /* Fiber Signal Detection */
    int                 halfout;            /* Transmit half amplitude */
    int                 fiber_plug_dely;    /* De-glitch time from property */
    sal_usecs_t         fiber_plug_time;    /* Time de-glitch started */
    int                 fiber_plug_wait;    /* De-glitch timer is running */
    int                 fiber_plug_done;    /* De-glitch period satisfied */
    sal_usecs_t         an_state_time;
    int                 an_state_valid;
    uint16              sgmii_an_state;
    uint16              mii_stat;
    soc_port_if_t       interface;          /* Operating mode of PHY */
    soc_phy_config_t    copper;             /* Current copper config */
    soc_phy_config_t    fiber;              /* Current fiber config */
    uint32              stop;               /* Power up/down state */
    uint32              flags;              /* PHY_STOP flags */ 
    uint32              power_mode;         /* various power saving modes */ 
    char                *dev_name;          /* device name */
    struct phy_driver_s *pd;                /* PHY driver functions */
#if defined (INCLUDE_MACSEC) || defined(INCLUDE_PLP_IMACSEC)
    int                 macsec_dev_addr;    /* MACSEC PHY MDIO address */
    int                 macsec_dev_port;    /* MACSEC PHY Port Number */
    int                 macsec_mac_policy;  /* MACSEC PHY Switch MAC Policy */
    int                 macsec_enable;      /* MACSEC Enable/Disabled */
    int                 macsec_switch_fixed;       /* Switch Fixed Policy */
    int                 macsec_switch_fixed_speed; /* Switch Fixed Speed */
    int                 macsec_switch_fixed_duplex;/* Switch Fixed Duplex */
    int                 macsec_switch_fixed_pause; /* Switch Fixed Pause */
    int                 macsec_pause_rx_fwd;  /* RX pause Forward or consume */
    int                 macsec_pause_tx_fwd;  /* TX pause Forward or consume */
    int                 macsec_line_ipg;      /* Line side IPG */
    int                 macsec_switch_ipg;      /* Switch side IPG */
#endif
#ifdef INCLUDE_FCMAP
    int                 fcmap_dev_addr;    /* FCMAP PHY MDIO address */
    int                 fcmap_uc_dev_addr; /* FCMAP MDIO address for uc */
    int                 fcmap_dev_port;    /* FCMAP PHY Port Number */
    int                 fcmap_mac_policy;  /* FCMAP PHY Switch MAC Policy */
    int                 fcmap_enable;      /* FCMAP Enable/Disabled */
    int                 fcmap_switch_fixed;       /* Switch Fixed Policy */
    int                 fcmap_switch_fixed_speed; /* Switch Fixed Speed */
    int                 fcmap_switch_fixed_duplex;/* Switch Fixed Duplex */
    int                 fcmap_switch_fixed_pause; /* Switch Fixed Pause */
    int                 fcmap_pause_rx_fwd;  /* RX pause Forward or consume */
    int                 fcmap_pause_tx_fwd;  /* TX pause Forward or consume */
    int                 fcmap_line_ipg;      /* Line side IPG */
    int                 fcmap_switch_ipg;      /* Switch side IPG */
    void                *fcmap_port_cfg;     /* Port Config */ 
#endif
#ifdef PHYMOD_SUPPORT
    soc_phymod_ctrl_t   phymod_ctrl;
#endif
    int                 (*read)(int, uint32, uint32, uint16*); /* HW read */
    int                 (*write)(int, uint32, uint32, uint16); /* HW write */
    int                 (*wb_write)(int, uint32, uint32, uint16); /* HW write */
    int                 (*data_write)(int, uint32, uint8, uint16); /* HW write */
    int                 (*addr_write)(int, uint32, uint8, uint16); /* HW write */
    int                 (*wrmask)(int, uint32, uint32, uint16, uint16);
    void                *driver_data;       /* driver private data */
    int                 size;   /* additional memory space requested by driver */
} phy_ctrl_t; 

extern phy_ctrl_t **int_phy_ctrl[SOC_MAX_NUM_DEVICES];
extern phy_ctrl_t **ext_phy_ctrl[SOC_MAX_NUM_DEVICES];

#define INT_PHY_SW_STATE(unit, port)  (int_phy_ctrl[unit][port])
#define EXT_PHY_SW_STATE(unit, port)  (ext_phy_ctrl[unit][port])

/* Param check macros */
#define EXT_PHY_INIT_CHECK(unit, port) \
    if (EXT_PHY_SW_STATE(unit, port) == NULL) { return SOC_E_INIT; }
#define INT_PHY_INIT_CHECK(unit, port) \
    if (INT_PHY_SW_STATE(unit, port) == NULL) { return SOC_E_INIT; }
#define PHY_CONTROL_TYPE_CHECK(type) \
if ((((int)(type)) < 0) || (((int)(type)) >= ((int)(SOC_PHY_CONTROL_COUNT)))){\
        return SOC_E_PARAM; \
}

/* phy bus read/write function type */
typedef int (*soc_phy_bus_rd_t)(int,uint32,uint32,uint16 *);
typedef int (*soc_phy_bus_wr_t)(int,uint32,uint32,uint16);

extern  int
soc_phy_probe(int unit, soc_port_t port, phy_ctrl_t *ext_pc,
                              phy_ctrl_t *int_pc);

extern int
soc_phy_check_sim(int unit, int port, phy_ctrl_t *pc);

typedef enum phyident_core_type_e {
    phyident_core_type_wc = 0,
    phyident_core_type_mld,
    phyident_core_type_qsmii,
    phyident_core_types_count
} phyident_core_type_t;

typedef struct phyident_core_info_s {
    uint16 mdio_addr;
    phyident_core_type_t core_type;
    uint32 mld_index; 
    uint32 index_in_mld;
    uint32 qsmii_lane; 
    uint32 first_phy_in_core;
    uint32 nof_lanes_in_core;
} phyident_core_info_t;

#define PHYIDENT_INFO_NOT_SET 0xFFFFFFFF

extern void phyident_core_info_t_init(phyident_core_info_t* core_info);

#ifdef BCM_PETRA_SUPPORT
extern int
_dpp_phy_addr_multi_get(int unit, soc_port_t port, int is_logical, int array_max,
                       int *array_size, phyident_core_info_t *core_info);
#endif

extern int
soc_phy_addr_multi_get(int unit, soc_port_t port, int array_size,
                       int *addr_num, phyident_core_info_t *core_info);

extern  int
soc_phy_list_get(char *phy_list[],int phy_list_size,int *phys_in_list);

#ifdef INCLUDE_I2C
extern int
phy_i2c_miireg_write (int unit,uint32 phy_id,uint32 phy_reg_addr,
                                                uint16 phy_wr_data);
extern int
phy_i2c_miireg_read (int unit,uint32 phy_id,uint32 phy_reg_addr,
                                             uint16 *phy_rd_data);
extern int
phy_i2c_bus_func_hook_set (int unit, soc_phy_bus_rd_t rd, 
                    soc_phy_bus_wr_t wr);
#endif

#define _PHYCTRL_CALL(_pc, _pf, _pa) \
        ((_pc) == 0 ? SOC_E_INIT : \
         (((_pc)->pd)->_pf == 0 ? SOC_E_UNAVAIL : ((_pc)->pd)->_pf _pa))

#define PHYCTRL_INIT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_init, ((_u), (_p)))

#define PHYCTRL_RESET(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_reset, ((_u), (_p), NULL))

#define PHYCTRL_LINK_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_link_get, ((_u), (_p), (_l)))

#define PHYCTRL_ENABLE_SET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_enable_set, ((_u), (_p), (_l)))

#define PHYCTRL_ENABLE_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_enable_get, ((_u), (_p), (_l)))

#define PHYCTRL_DUPLEX_SET(_pc, _u, _p, _d) \
        _PHYCTRL_CALL((_pc), pd_duplex_set, ((_u), (_p), (_d)))

#define PHYCTRL_DUPLEX_GET(_pc, _u, _p, _d) \
        _PHYCTRL_CALL((_pc), pd_duplex_get, ((_u), (_p), (_d)))

#define PHYCTRL_SPEED_SET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_speed_set, ((_u), (_p), (_s)))

#define PHYCTRL_SPEED_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_speed_get, ((_u), (_p), (_s)))

#define PHYCTRL_MASTER_SET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_master_set, ((_u), (_p), (_s)))

#define PHYCTRL_MASTER_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_master_get, ((_u), (_p), (_s)))

#define PHYCTRL_AUTO_NEGOTIATE_SET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_an_set, ((_u), (_p), (_a)))

#define PHYCTRL_AUTO_NEGOTIATE_GET(_pc, _u, _p, _a, _b) \
        _PHYCTRL_CALL((_pc), pd_an_get, ((_u), (_p), (_a), (_b)))

#define PHYCTRL_ADV_SET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_adv_local_set, ((_u), (_p), (_a)))

#define PHYCTRL_ADV_GET(_pc, _u, _p, _a) \
        _PHYCTRL_CALL((_pc), pd_adv_local_get, ((_u), (_p), (_a)))

#define PHYCTRL_LOOPBACK_SET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_lb_set, ((_u), (_p), (_l)))

#define PHYCTRL_LOOPBACK_GET(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_lb_get, ((_u), (_p), (_l)))

#define PHYCTRL_INTERFACE_SET(_pc, _u, _p, _i) \
        _PHYCTRL_CALL((_pc), pd_interface_set, ((_u), (_p), (_i)))

#define PHYCTRL_INTERFACE_GET(_pc, _u, _p, _i) \
        _PHYCTRL_CALL((_pc), pd_interface_get, ((_u), (_p), (_i)))

#define PHYCTRL_ABILITY_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_ability, ((_u), (_p), (_m)))

#define PHYCTRL_LINKUP_EVT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_linkup_evt, ((_u), (_p)))

#define PHYCTRL_LINKDN_EVT(_pc, _u, _p) \
        _PHYCTRL_CALL((_pc), pd_linkdn_evt, ((_u), (_p)))

#define PHYCTRL_ADV_REMOTE_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_adv_remote_get, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_SET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_mdix_set, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_mdix_get, ((_u), (_p), (_m)))

#define PHYCTRL_MDIX_STATUS_GET(_pc, _u, _p, _s) \
        _PHYCTRL_CALL((_pc), pd_mdix_status_get, ((_u), (_p), (_s)))

#define PHYCTRL_MEDIUM_CONFIG_SET(_pc, _u, _p, _m, _c) \
        _PHYCTRL_CALL((_pc), pd_medium_config_set, ((_u), (_p), (_m), (_c)))

#define PHYCTRL_MEDIUM_CONFIG_GET(_pc, _u, _p, _m, _c) \
        _PHYCTRL_CALL((_pc), pd_medium_config_get, ((_u), (_p), (_m), (_c)))

#define PHYCTRL_MEDIUM_GET(_pc, _u, _p, _m) \
        _PHYCTRL_CALL((_pc), pd_medium_get, ((_u), (_p), (_m)))

#define PHYCTRL_CABLE_DIAG(_pc, _u, _p, _status) \
        _PHYCTRL_CALL((_pc), pd_cable_diag, ((_u), (_p), (_status)))

#define PHYCTRL_LINK_CHANGE(_pc, _u, _p, _l) \
        _PHYCTRL_CALL((_pc), pd_link_change, ((_u), (_p), (_l)))

#define PHYCTRL_CONTROL_SET(_pc, _u, _p, _t, _v) \
        _PHYCTRL_CALL((_pc), pd_control_set, ((_u), (_p), (_t), (_v)))

#define PHYCTRL_CONTROL_GET(_pc, _u, _p, _t, _v) \
        _PHYCTRL_CALL((_pc), pd_control_get, ((_u), (_p), (_t), (_v)))

#define PHYCTRL_REG_READ(_pc, _u, _p, _f, _a, _v) \
        _PHYCTRL_CALL((_pc), pd_reg_read, ((_u), (_p), (_f), (_a), (_v)))

#define PHYCTRL_REG_WRITE(_pc, _u, _p, _f, _a, _v) \
        _PHYCTRL_CALL((_pc), pd_reg_write, ((_u), (_p), (_f), (_a), (_v)))

#define PHYCTRL_REG_MODIFY(_pc, _u, _p, _f, _a, _v, _m) \
        _PHYCTRL_CALL((_pc), pd_reg_modify, ((_u), (_p), (_f), (_a), (_v), (_m)))

#define PHYCTRL_NOTIFY(_pc, _u, _p, _e, _v) \
        _PHYCTRL_CALL((_pc), pd_notify, ((_u), (_p), (_e), (_v)))

#endif /* _PHY_PHYCTRL_H_ */
