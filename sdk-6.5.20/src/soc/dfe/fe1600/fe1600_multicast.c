/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 MULTICAST
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MCAST
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/mem.h>

#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <shared/bitop.h>

#if defined(BCM_88750_A0)

#include <soc/dfe/fe1600/fe1600_defs.h>
#include <soc/dfe/fe1600/fe1600_fabric_topology.h>
#include <soc/dfe/fe1600/fe1600_multicast.h>
#include <soc/dfe/fe1600/fe1600_fabric_multicast.h>


soc_error_t 
soc_fe1600_multicast_egress_add(int unit, bcm_multicast_t group, soc_gport_t port)
{
    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];
    SOCDNX_INIT_FUNC_DEFS;
    
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MCTm(unit, 0, MEM_BLOCK_ANY, group, data));
      
    SHR_BITSET(data, port);
    
    
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, group, data));
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_multicast_egress_delete(int unit, bcm_multicast_t group, soc_gport_t port)
{
    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MCTm(unit, 0, MEM_BLOCK_ANY, group, data));
      
    SHR_BITCLR(data, port);
    
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, group, data));
    
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_multicast_egress_delete_all(int unit, bcm_multicast_t group)
{
    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];
    SOCDNX_INIT_FUNC_DEFS;
    
    sal_memset(data, 0, sizeof(uint32)*SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32);
      
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, group, data));
     
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_multicast_egress_get(int unit, bcm_multicast_t group, int port_max, soc_gport_t *port_array, int *port_count)
{
    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];
    soc_gport_t i;
    int table, index, is_first_half;
    uint32 mc_table_entry_size;
    int bit, rv;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_multicast_read_info(unit, group, &table, &index, &is_first_half));
    SOCDNX_IF_ERR_EXIT(READ_RTP_MCTm(unit, table, MEM_BLOCK_ANY, index, data));
    
    *port_count = 0;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
    SOCDNX_IF_ERR_EXIT(rv);

    for(i = 0; i < mc_table_entry_size; i++)
    {
        if (is_first_half)
        {
            bit = i;
        }
        else 
        {
            bit = i + mc_table_entry_size;
        }
        if(SHR_BITGET(data, bit))
        {
            if(*port_count >= port_max)
            {
                *port_count = 0;
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("port_array is too small")));
            }
            
            port_array[*port_count] = i;
            (*port_count)++;
        }
    }
     
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_multicast_egress_set(int unit, bcm_multicast_t group, int port_count, soc_gport_t *port_array)
{
    uint32 data[SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32];
    int i;
    SOCDNX_INIT_FUNC_DEFS;
    
    sal_memset(data, 0, sizeof(uint32)*SOC_FE1600_MULTICAST_ROW_SIZE_IN_UINT32);
  
    for(i = 0 ; i<port_count ; i++)
    {
        SHR_BITSET(data,port_array[i]);
    }
    
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ANY, group, data));
     
exit:
    SOCDNX_FUNC_RETURN; 
}


soc_error_t 
soc_fe1600_multicast_table_size_get(int unit, uint32* mc_table_size)
{
	soc_dfe_multicast_table_mode_t multicast_mode;
    SOCDNX_INIT_FUNC_DEFS;
	
#ifdef BCM_88754_A0
    if (SOC_IS_BCM88754_A0(unit))
    {
        *mc_table_size = 32*1024;
    }
#endif 
	SOCDNX_IF_ERR_EXIT(soc_fe1600_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dfe_multicast_table_mode_64k:
			*mc_table_size = 64*1024;
			break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("wrong mc_table_mode value %d"),SOC_DFE_CONFIG(unit).fe_mc_id_range));
	}
exit:
    SOCDNX_FUNC_RETURN;    
}

soc_error_t
soc_fe1600_multicast_mode_get(int unit, soc_dfe_multicast_table_mode_t* multicast_mode)
{
	SOCDNX_INIT_FUNC_DEFS;
	*multicast_mode = soc_dfe_multicast_table_mode_64k; 
	SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe1600_multicast_table_entry_size_get(int unit, uint32 *entry_size)
{
	SOCDNX_INIT_FUNC_DEFS;
	*entry_size = 128;
	SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

