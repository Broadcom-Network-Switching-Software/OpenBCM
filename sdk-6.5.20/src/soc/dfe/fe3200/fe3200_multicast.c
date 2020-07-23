/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 MULTICAST
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MCAST

#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dfe/fe3200/fe3200_multicast.h>

#include <soc/dfe/cmn/dfe_drv.h>

soc_error_t
soc_fe3200_multicast_mode_get(int unit, soc_dfe_multicast_table_mode_t* multicast_mode)
{
	SOCDNX_INIT_FUNC_DEFS;
	*multicast_mode = soc_dfe_multicast_table_mode_128k_half; 
	switch(SOC_DFE_CONFIG(unit).fe_mc_id_range)
	{
		case soc_dfe_multicast_table_mode_64k:
		case soc_dfe_multicast_table_mode_128k:
		case soc_dfe_multicast_table_mode_128k_half:
			*multicast_mode =  SOC_DFE_CONFIG(unit).fe_mc_id_range;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("Wrong mc_table_mode value %d"), SOC_DFE_CONFIG(unit).fe_mc_id_range));
	}
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_multicast_table_size_get(int unit, uint32* mc_table_size)
{
	soc_dfe_multicast_table_mode_t multicast_mode;
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_IF_ERR_EXIT(soc_fe3200_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dfe_multicast_table_mode_64k:
			*mc_table_size = 64*1024;
			break;
		case soc_dfe_multicast_table_mode_128k:
			*mc_table_size = 128*1024;
			break;
		case soc_dfe_multicast_table_mode_128k_half:
			*mc_table_size = 128*1024;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
            break;
	}
exit:
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_multicast_table_entry_size_get(int unit, uint32* entry_size)
{
	soc_dfe_multicast_table_mode_t multicast_mode;
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_IF_ERR_EXIT(soc_fe3200_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dfe_multicast_table_mode_64k:
			*entry_size = 144;
			break;
		case soc_dfe_multicast_table_mode_128k:
			*entry_size = 144;
			break;
		case soc_dfe_multicast_table_mode_128k_half:
			*entry_size = 72;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
	}
exit:
	SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
