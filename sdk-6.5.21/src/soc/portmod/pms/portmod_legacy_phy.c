/*
 *         
 * 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/* This file holds the calls to all the legacy phy drivers
 */

#include <soc/phyctrl.h>
#include <soc/drv.h>
#include <soc/phy/phyctrl.h>
#ifdef PORTMOD_SUPPORT
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/error.h>

/* Broadcast Support */
extern int soc_phyctrl_mdio_ucode_bcst(int unit, pbmp_t pbmap);

/* Probes for legacy phy using soc ctrl layer
 * if it finds the legacy phy returns 1 else 0
 */
int 
portmod_port_legacy_phy_probe(int unit, int port) {
    phy_ctrl_t *ext_pc = NULL;
    ext_pc = EXT_PHY_SW_STATE(unit, port);

#if 0
    /*check if extern legacy phy initialized or not*/
    if(ext_pc && ext_pc->pd != NULL) {
        return 1;
    }
#endif

    if (soc_phyctrl_probe(unit, port) >= 0) {
        ext_pc = EXT_PHY_SW_STATE(unit, port);

        if (NULL == ext_pc) {
            /* return 0 here to support systems with no sdk */
            /* something wrong with the init */
            return 0;
        }

        if (ext_pc->pd != NULL) {
            /* legacy External Phy is detected */
            /* Keep the return values as int instead of SOC_E_TYPE 
             * for clarity */
            return 1;
        } else {
            return 0;
        }
        
    } else {
        /* phy probe failed */
        return SOC_E_INTERNAL;
    }
}

int
portmod_port_legacy_phy_init(int unit, int port) {
    
    pbmp_t l_port, okay_port;

    SOC_PBMP_CLEAR(okay_port);
    SOC_PBMP_CLEAR(l_port);
    
    SOC_PBMP_PORT_ADD(l_port, port);
  
    if (soc_phyctrl_pbm_probe_init(unit, l_port, &okay_port) >= 0) {
        return SOC_E_NONE;
    } else {
        return SOC_E_FAIL;
    }
}

int portmod_port_legacy_phy_addr_get(int unit, int port) {
    return PORT_TO_PHY_ADDR(unit, port);
}

int portmod_port_legacy_core_identify(int unit, int port) {
    return SOC_E_NONE;
}

int portmod_port_legacy_lanemap_set(int unit, int port, const phymod_lane_map_t *lane_map) {
    return SOC_E_NONE;
}
 
soc_phy_control_t _portmod_phymod_to_soc_lb_control(phymod_loopback_mode_t lb, soc_phy_control_t *control) {

    switch(lb) {
        case phymodLoopbackGlobal:
            *control = SOC_PHY_CONTROL_LOOPBACK_INTERNAL; 
            break;
        case phymodLoopbackGlobalPMD:
            *control = SOC_PHY_CONTROL_LOOPBACK_PMD;
            break;
        case phymodLoopbackRemotePMD:
            *control = SOC_PHY_CONTROL_LOOPBACK_REMOTE_PCS_BYPASS;
            break;
        case phymodLoopbackRemotePCS:
            *control = SOC_PHY_CONTROL_LOOPBACK_REMOTE;
            break;    
        default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE; 
}

int portmod_port_legacy_loopback_set(int unit, int port, phymod_loopback_mode_t lb, uint32_t enable) {
    soc_phy_control_t control;    
    int rv = SOC_E_NONE;

    rv = _portmod_phymod_to_soc_lb_control(lb, &control);
    if (rv != SOC_E_PARAM) {
        rv =  soc_phyctrl_control_set(unit, port, control, enable);

        /* 
         * Need to adjust portmod_port_legacy_loopback_get() when retrieving the loopback
         * status for the unsupported loopback types.
         */
        if ((rv == SOC_E_UNAVAIL) && ((control == SOC_PHY_CONTROL_LOOPBACK_INTERNAL) ||
                                      (control == SOC_PHY_CONTROL_LOOPBACK_PMD))) {
            /* SOC_PHY_CONTROL_LB is not supported fall back to pd_lb_set */
            rv = soc_phyctrl_loopback_set(unit, port, enable, 0);
        }
    } else  {
        return SOC_E_PARAM;
    }
   
    return rv; 
}

int portmod_port_legacy_loopback_get(int unit, int port, phymod_loopback_mode_t lb, uint32_t *enable) {
    soc_phy_control_t control;    
    int rv = SOC_E_NONE;
   
    rv = _portmod_phymod_to_soc_lb_control(lb, &control);
    if (rv != SOC_E_PARAM) {
        rv = soc_phyctrl_control_get(unit, port, control, enable);  

        /* 
         * Some legacy ext PHYs do not support certain loopback types. Among these unsupported loopback
         * types, instead of calling soc_phyctrl_control_set() to set up the specific loopback type,
         * portmod_port_legacy_loopback_set() calls soc_phyctrl_loopback_set() to set up a "general"
         * loopback without specifying the loopback type. Below these special loopback types need to
         * use soc_phyctrl_loopback_get() to retrieve the "general" loopback status.
         */
        if ((rv == SOC_E_UNAVAIL) && ((control == SOC_PHY_CONTROL_LOOPBACK_INTERNAL) ||
                                      (control == SOC_PHY_CONTROL_LOOPBACK_PMD))) {
            rv = soc_phyctrl_loopback_get(unit, port, (int *)enable);
        }
    } else {
        return SOC_E_PARAM;
    }
    return rv;
}

int portmod_port_legacy_interface_config_set(int unit, int port, const phymod_phy_inf_config_t *config) {
    int rv = SOC_E_NONE;
    soc_port_if_t port_if;

    rv = portmod_intf_from_phymod_intf(unit, config->interface_type, &port_if);
    if (rv) return (rv); 

    rv = soc_phyctrl_interface_set(unit, port, port_if);
    if (rv) return (rv); 

    /* Speed et takes care of interface based on the speed config */
    rv = soc_phyctrl_speed_set(unit, port, config->data_rate);
    return(rv);
}

int portmod_port_legacy_interface_config_get(int unit, int port, phymod_phy_inf_config_t *config) {
    int rv = SOC_E_NONE;
    int speed;
    soc_port_if_t interface;
    rv = soc_phyctrl_speed_get(unit, port, &speed);
    config->data_rate = speed;
    if (rv < SOC_E_NONE)
        return rv;
    rv = soc_phyctrl_interface_get(unit, port, &interface);
    if (rv < SOC_E_NONE)
        return rv;
    rv = portmod_intf_to_phymod_intf(unit, speed, interface, &config->interface_type);
    return rv;
}

int portmod_port_legacy_power_set(int unit, int port, const phymod_phy_power_t *power) {
    int enable = 0;
    int rv = SOC_E_NONE;
    if ((power->tx == phymodPowerOn) &&(power->rx == phymodPowerOn)) {
        enable = 1;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else if ((power->tx == phymodPowerOff) &&(power->rx == phymodPowerOff)) {
        enable = 0;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else {
        
    }
    return rv;
}

int portmod_port_legacy_power_get(int unit, int port, phymod_phy_power_t *power) {
    int enable = 0;
    int rv =  SOC_E_NONE;

     rv = soc_phyctrl_enable_get(unit, port, &enable);

     if (enable) {
         power->tx = phymodPowerOn;
         power->rx = phymodPowerOn;
     } else {
         power->tx = phymodPowerOff;
         power->rx = phymodPowerOff;
     }
     return rv;
}

int portmod_port_legacy_tx_ln_ctrl_get(int unit, int port, phymod_phy_tx_lane_control_t* control) {
    int enable = 0;
    int rv =  SOC_E_NONE;

    rv = soc_phyctrl_enable_get(unit, port, &enable);

    if (enable) {
        *control = phymodTxSquelchOff;
    } else {
        *control = phymodTxSquelchOn;
    }

    return rv;
}

int portmod_port_legacy_tx_ln_ctrl_set(int unit, int port, const phymod_phy_tx_lane_control_t* control) {
    int enable = 0;
    int rv = SOC_E_NONE;

    if(*control == phymodTxSquelchOff) {
        enable = 1;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else if (*control == phymodTxSquelchOn) {
        enable = 0;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else {
        /* SOC_PHY_CONTROL_XXXX*/
    }
    return rv;
}

int portmod_port_legacy_rx_ln_ctrl_get(int unit, int port, phymod_phy_rx_lane_control_t* control) {
    int enable = 0;
    int rv =  SOC_E_NONE;

    rv = soc_phyctrl_enable_get(unit, port, &enable);

    if (enable) {
        *control = phymodRxSquelchOff;
    } else {
        *control = phymodRxSquelchOn;
    }

    return rv;
    
}

int portmod_port_legacy_rx_ln_ctrl_set(int unit, int port, const phymod_phy_rx_lane_control_t* control) {
    int enable = 0;
    int rv = SOC_E_NONE;

    if(*control == phymodRxSquelchOff) {
        enable = 1;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else if (*control == phymodRxSquelchOn) {
        enable = 0;
        rv = soc_phyctrl_enable_set(unit, port, enable);
    } else {
        /* SOC_PHY_CONTROL_XXXX*/
    }
    return rv;
}

int portmod_port_legacy_polarity_set(int unit, int port, const phymod_polarity_t *polarity) {
    return SOC_E_NONE;
}

int portmod_port_legacy_phy_control_set(int unit, int port, int phyn, int phy_lane, int sys_side,
                                        soc_phy_control_t control, uint32_t value) {
    /* GPORT will never have phyn == -1 , which mean local port */    
    if( phyn == -1 ){
        return soc_phyctrl_control_set(unit, port, control, value);
    } else {
        /* GPORT case */
        return soc_phyctrl_redirect_control_set(unit, port, phyn, phy_lane, 
                                                sys_side, control, value);
    }
}

int portmod_port_legacy_phy_control_get(int unit, int port, int phyn, int phy_lane, int sys_side,
                                        soc_phy_control_t control, uint32_t *value) {

    /* GPORT will never have phyn == -1 , which mean local port */    
    if( phyn == -1 ){
        return soc_phyctrl_control_get(unit, port, control, value);
    } else {
        /* GPORT case */
        return soc_phyctrl_redirect_control_get(unit, port, phyn, phy_lane, sys_side, 
                                                control, value);
    }
}

int portmod_port_legacy_phy_link_get(int unit, int port, uint32_t *link) {
    int link_status;
    int rv = SOC_E_NONE;
    
    link_status = 0; 
    rv =  soc_phyctrl_link_get(unit, port,&link_status);
    *link = link_status;
    return rv;
}

int portmod_port_legacy_cl72_set(int unit, int port, uint32_t cl72_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_CL72, cl72_en);

    return rv;
}

int portmod_port_legacy_cl72_get(int unit, int port, uint32_t *cl72_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_CL72, cl72_en);

    return rv;
}

int portmod_port_legacy_eee_set(int unit, int port, uint32_t eee_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_EEE, eee_en);

    return rv;
}

int portmod_port_legacy_eee_get(int unit, int port, uint32_t *eee_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_EEE, eee_en);

    return rv;
}


int portmod_port_legacy_fec_get(int unit, int port, uint32_t *fec_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_get(unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION, fec_en);

    return rv;
}

int portmod_port_legacy_fec_set(int unit, int port, uint32_t fec_en) {
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_control_set(unit, port, SOC_PHY_CONTROL_FORWARD_ERROR_CORRECTION, fec_en);

    return rv;
}

void _portmod_port_legacy_portmod_to_soc_ability(portmod_port_ability_t *portmod_ability, soc_port_ability_t *soc_ability) {
    /* currently portmod abilty and soc ability are the same */
    sal_memcpy(soc_ability, portmod_ability, sizeof(soc_port_ability_t));
}

void _portmod_port_legacy_soc_to_portmod_ability(soc_port_ability_t *soc_ability, portmod_port_ability_t *portmod_ability) {
    sal_memcpy(portmod_ability, soc_ability, sizeof(portmod_port_ability_t));
}

int portmod_port_legacy_advert_set(int unit, int port, const portmod_port_ability_t* portmod_ability) {
    int rv = SOC_E_NONE;
    soc_port_ability_t port_ability;
    portmod_port_ability_t an_ability;

    sal_memset(&port_ability, 0, sizeof(soc_port_ability_t));
    sal_memcpy(&an_ability, portmod_ability, sizeof(portmod_port_ability_t));
    /* convert portmod ability to soc ability */
    _portmod_port_legacy_portmod_to_soc_ability(&an_ability, &port_ability);
    rv = soc_phyctrl_ability_advert_set(unit, port, &port_ability);

    return rv;
}

int portmod_port_legacy_advert_get(int unit, int port, portmod_port_ability_t* portmod_ability) {
    int rv = SOC_E_NONE;
    soc_port_ability_t port_ability;

    sal_memset(&port_ability, 0, sizeof(soc_port_ability_t));
    rv = soc_phyctrl_ability_advert_get(unit, port, &port_ability);
    if (rv < SOC_E_NONE) {
        return rv;
    }
    /* convert soc ability to portmod ability */
    _portmod_port_legacy_soc_to_portmod_ability(&port_ability, portmod_ability);

    return rv;
}

int portmod_port_legacy_ability_remote_get(int unit, int port, portmod_port_ability_t * portmod_ability) {
    int rv = SOC_E_NONE;
    soc_port_ability_t port_ability;

    sal_memset(&port_ability, 0, sizeof(soc_port_ability_t));

    rv = soc_phyctrl_ability_remote_get(unit, port, &port_ability);
    if (rv < SOC_E_NONE) {
        return rv;
    }
    _portmod_port_legacy_soc_to_portmod_ability(&port_ability, portmod_ability);

    return rv;
}

int portmod_port_legacy_ability_local_get(int unit, int port, portmod_port_ability_t *portmod_ability) {
    int rv = SOC_E_NONE;
    soc_port_ability_t soc_ability;

    sal_memset(&soc_ability, 0, sizeof(soc_port_ability_t));

    rv = soc_phyctrl_ability_local_get(unit, port, &soc_ability);
    _portmod_port_legacy_soc_to_portmod_ability(&soc_ability, portmod_ability);
    return rv;
}

int portmod_port_legacy_duplex_get(int unit, int port, int* duplex)
{
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_duplex_get(unit, port, duplex);

    return(rv);
}

int portmod_port_legacy_duplex_set(int unit, int port,  int duplex)
{
    int rv = SOC_E_NONE;

    rv = soc_phyctrl_duplex_set(unit, port, duplex);

    return(rv);
}

/* TBD Need to modify this function to return type of autoneg cl73/cl37/BAM etc instead of status */
int portmod_port_legacy_an_get(int unit, int port, phymod_autoneg_control_t *an_config,
                               uint32_t * an_done)
{
    int rv = SOC_E_NONE;
    int an_enable, an_is_done;

    an_enable = 0, an_is_done = 0;

    rv = soc_phyctrl_auto_negotiate_get(unit, port, &an_enable, &an_is_done);

    an_config->enable = an_enable;
    *an_done = an_is_done;

    return rv;

}

int portmod_port_legacy_an_status_get(int unit, int port, phymod_autoneg_status_t *an_status)
{
    int rv = SOC_E_NONE;
    int an_enable, an_is_done;

    an_enable = 0, an_is_done = 0;

    rv = soc_phyctrl_auto_negotiate_get(unit, port, &an_enable, &an_is_done);

    an_status->enabled = an_enable;

    return rv;
}

int portmod_port_legacy_an_set(int unit, int port,  const phymod_autoneg_control_t *an_config)
{

    return soc_phyctrl_auto_negotiate_set(unit, port, (int)an_config->enable);

}

int portmod_port_legacy_tx_set(const phymod_phy_access_t* phy_access, const int chain_length, const phymod_tx_t* tx) {
    int rv = SOC_E_NONE;
    return rv;
}


int portmod_port_legacy_phy_mdix_set(int unit, int port, soc_port_mdix_t mode) {

    return soc_phyctrl_mdix_set(unit, port, mode);

}

int portmod_port_legacy_phy_mdix_get(int unit, int port, soc_port_mdix_t *mode) {

   return soc_phyctrl_mdix_get(unit, port, mode);
}

int portmod_port_legacy_phy_mdix_status_get(int unit, soc_port_t port,
                                             soc_port_mdix_status_t *status) {
    return  soc_phyctrl_mdix_status_get(unit, port, status);
}

int portmod_legacy_ext_phy_init(int unit, pbmp_t pbmp)
{
   int    port, legacy_phy, rv = 0;
   pbmp_t legacy_pbmp, okay_pbmp;

    SOC_PBMP_CLEAR(legacy_pbmp);
    SOC_PBMP_CLEAR(okay_pbmp);

    PBMP_ITER(pbmp, port) {
        rv = portmod_port_check_legacy_phy(unit, port, &legacy_phy);
        if (rv) return (rv);

        /* check if the port is legacy */
        if (legacy_phy) {
            SOC_PBMP_PORT_ADD(legacy_pbmp, port);
        }
    }
    /* ignore the error and allow ext phys to be initialized when no firmware downloaded */
    (void)soc_phyctrl_pbm_probe_init(unit, legacy_pbmp, &okay_pbmp);
    return SOC_E_NONE;
}

int portmod_port_legacy_phy_reset_set(int unit, int port)
{
    return soc_phyctrl_reset(unit, port, NULL);
}

int portmod_port_legacy_timesync_config_set( int unit, int port,
                                            const portmod_phy_timesync_config_t* config)                                           
{
    int rv = 0;
    portmod_phy_timesync_config_t soc_config;

    sal_memcpy(&soc_config, config, sizeof(soc_port_phy_timesync_config_t));
    
    rv = soc_port_phy_timesync_config_set(unit, port, &soc_config);
    return (rv);
}

int portmod_port_legacy_timesync_config_get( int unit, int port,
                                             portmod_phy_timesync_config_t* config)                                           
{
    int rv = 0;
    rv = soc_port_phy_timesync_config_get(unit, port, config);
    return (rv);
}


int portmod_port_legacy_control_phy_timesync_set( int unit, int port,
                                                  const soc_port_control_phy_timesync_t type,
                                                  const uint64 value)
{
    int rv = 0;

    rv = soc_port_control_phy_timesync_set(unit, port, type, value);
    return (rv);
}

int portmod_port_legacy_control_phy_timesync_get( int unit, int port, 
                                                  const soc_port_control_phy_timesync_t type, 
                                                  uint64* value) 
{
    int rv = 0;

    rv = soc_port_control_phy_timesync_get(unit, port, type, value);
    return (rv);
}

int portmod_port_legacy_phy_link_up_event(int unit, int port)
{
    return soc_phyctrl_linkup_evt(unit, port);
}

int portmod_port_legacy_phy_link_down_event(int unit, int port)
{
    return soc_phyctrl_linkdn_evt(unit, port);
}

int portmod_port_legacy_medium_config_set( int unit, int port, soc_port_medium_t medium,
                                           soc_phy_config_t *cfg)
{
    return soc_phyctrl_medium_config_set(unit, port, medium, cfg);
}

int portmod_port_legacy_medium_config_get( int unit, int port, soc_port_medium_t medium,
                                           soc_phy_config_t *cfg)
{
    return soc_phyctrl_medium_config_get(unit, port, medium, cfg);
}

int portmod_port_legacy_medium_get(int unit, int port, soc_port_medium_t* medium)
{
    return soc_phyctrl_medium_get(unit, port, medium);
}

int portmod_port_legacy_reg_read(int unit, int port, uint32 flags, uint32 phy_addr, uint32 *phy_data) 
{
    return soc_phyctrl_reg_read(unit, port, flags, phy_addr, phy_data);
}

int portmod_port_legacy_reg_write(int unit, int port, uint32 flags, uint32 phy_addr, uint32 phy_data) 
{
    return soc_phyctrl_reg_write(unit, port, flags, phy_addr, phy_data);
}

int portmod_port_legacy_multi_get(int unit, soc_port_t port, uint32 flags,
                 uint32 dev_addr, uint32 offset, int max_size, uint8 *data, int *actual_size)
{
    return soc_phyctrl_port_phy_multi_get(unit, port, flags, dev_addr, offset, max_size, data, actual_size);
}

int portmod_port_legacy_is_skip_spd_sync(int unit, int port) {

    if(PHY_FLAGS_TST(unit, port, PHY_FLAGS_NO_SYS_LINE_SPD_SYNC)) {
        return 1;
    }

    return 0;
}

int portmod_port_legacy_phy_drv_name_get(int unit, int port,
                                         char *name, int len)
{
    return (soc_phyctrl_drv_name_get( unit, port, name, len));
}

int portmod_port_legacy_phy_flags_test(int unit, int port, int flag) 
{
    return PHY_FLAGS_TST(unit, port, flag); 
}

int portmod_port_legacy_master_set(int unit, int port, int master_mode)
{
    return soc_phyctrl_master_set(unit, port, master_mode);
}

int portmod_port_legacy_master_get(int unit, int port, int* master_mode)
{
    return soc_phyctrl_master_get(unit, port, master_mode);
} 

int portmod_port_legacy_op_mode_get(int unit, int port, phymod_operation_mode_t* phy_op_mode)
{
    if (PHY_PASSTHRU_MODE(unit, port)) {
        *phy_op_mode = phymodOperationModePassthru;
    } else if (PHY_REPEATER(unit, port)) {
        *phy_op_mode = phymodOperationModeRepeater;
    } else {
        *phy_op_mode = phymodOperationModeRetimer;
    }

    return SOC_E_NONE;
}

#endif /* PORTMOD_SUPPORT */
