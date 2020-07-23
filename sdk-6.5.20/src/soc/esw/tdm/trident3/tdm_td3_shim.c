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
@name: tdm_td3_shim_get_port_pm
@param:

Return PM number to which the input port belongs
 */
int
tdm_td3_shim_get_port_pm(tdm_mod_t *_tdm)
{
    return (tdm_td3_cmn_get_port_pm(_tdm, _tdm->_core_data.vars_pkg.port));
}


/**
@name: tdm_td3_shim_get_pipe_ethernet
@param:

Returns BOOL_TRUE if pipe of the given port has traffic entirely Ethernet,
otherwise returns BOOL_FALSE.
 */
int
tdm_td3_shim_get_pipe_ethernet(tdm_mod_t *_tdm)
{
    return (tdm_td3_cmn_get_pipe_ethernet(_tdm,
                                          _tdm->_core_data.vars_pkg.cal_id));
}
