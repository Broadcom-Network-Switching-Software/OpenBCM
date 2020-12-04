/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip data structure scanning functions
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_hr4_scan_port_pm_old
@param:

Returns the PM number to which the input port belongs according to pmap
 */
int
tdm_hr4_scan_port_pm_old(tdm_mod_t *_tdm_s)
{
    HR4_TOKEN_CHECK(_tdm_s->_core_data.vars_pkg.port) {
        return tdm_find_pm( _tdm_s );
    }

    return HR4_NUM_EXT_PORTS;
}


/**
@name: tdm_hr4_scan_port_pm
@param:

Returns the PM number to which the input port belongs according to formula
 */
int
tdm_hr4_scan_port_pm(tdm_mod_t *_tdm)
{
    int port_phy, port_pm;
    int param_phy_lo, param_phy_hi, param_pm_lanes;

    param_phy_lo   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_lo;
    param_phy_hi   = _tdm->_chip_data.soc_pkg.soc_vars.fp_port_hi;
    param_pm_lanes = _tdm->_chip_data.soc_pkg.pmap_num_lanes;
    
    port_pm   = _tdm->_chip_data.soc_pkg.num_ext_ports;
    port_phy = _tdm->_core_data.vars_pkg.port;
    
    if (port_phy>=param_phy_lo && port_phy<=param_phy_hi && param_pm_lanes>0) {
        port_pm = (port_phy-1)/param_pm_lanes;
    }

    return port_pm;
}


/**
@name: tdm_hr4_scan_pipe_ethernet
@param:

Returns BOOL_TRUE if pipe of the given port has traffic entirely Ethernet,
otherwise returns BOOL_FALSE.
 */
int
tdm_hr4_scan_pipe_ethernet(tdm_mod_t *_tdm)
{
    int i, idx_s=0, idx_e=0, port, port_speed, port_state, result=BOOL_TRUE;

    port = _tdm->_core_data.vars_pkg.port;

    if (port<=64) {
        idx_s = 1;
        idx_e = 64;
    } else if (port<=128){
        idx_s = 65;
        idx_e = 128;
    }

    for (i=idx_s; i<idx_e; i++) {
        port_state = _tdm->_chip_data.soc_pkg.state[i-1];
        port_speed = _tdm->_chip_data.soc_pkg.speed[i];
        if ( (port_speed>SPEED_0) && 
             (port_state==PORT_STATE__LINERATE_HG ||
              port_state==PORT_STATE__OVERSUB_HG   ) ){
            result = BOOL_FALSE;
            break;
        }
    }

    return result;
}
