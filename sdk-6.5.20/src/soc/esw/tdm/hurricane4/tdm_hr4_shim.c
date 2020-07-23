/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * $All Rights Reserved.$
 *
 * TDM chip to core API shim layer
 */
#ifdef _TDM_STANDALONE
	#include <tdm_top.h>
#else
	#include <soc/tdm/core/tdm_top.h>
#endif


/**
@name: tdm_hr4_shim_get_port_pm
@param:

Return PM number to which the input port belongs
 */
int
tdm_hr4_shim_get_port_pm(tdm_mod_t *_tdm)
{
    return (tdm_hr4_cmn_get_port_pm(_tdm->_core_data.vars_pkg.port, _tdm));
}


/**
@name: tdm_hr4_shim_get_pipe_ethernet
@param:

Returns BOOL_TRUE if pipe of the given port has traffic entirely Ethernet,
otherwise returns BOOL_FALSE.
 */
int
tdm_hr4_shim_get_pipe_ethernet(tdm_mod_t *_tdm)
{
    int result = BOOL_TRUE;

    if (_tdm->_core_data.vars_pkg.port<=64) {
        result = tdm_hr4_cmn_get_pipe_ethernet(0, _tdm);
    } else if (_tdm->_core_data.vars_pkg.port<=128){
        result = tdm_hr4_cmn_get_pipe_ethernet(1, _tdm);
    }

    return (result);
}


/**
@name: tdm_chip_th2_shim__core_vbs_scheduler_ovs
@param:

Passthru function for oversub scheduling request from TD2/TD2+
 */
int
tdm_hr4_shim_core_vbs_scheduler_ovs(tdm_mod_t *_tdm)
{
    if (_tdm->_chip_data.soc_pkg.flex_port_en) {
        /* Used for FlexPort */
        /* return tdm_hr4_vbs_scheduler_ovs_flex_port(_tdm); */
        /* return tdm_hr4_vbs_scheduler_ovs_flex_port_delta(_tdm) */;
        return tdm_core_vbs_scheduler_ovs(_tdm);
    } else {
        /* Used for initial config */
        return tdm_core_vbs_scheduler_ovs(_tdm);
    }
}
