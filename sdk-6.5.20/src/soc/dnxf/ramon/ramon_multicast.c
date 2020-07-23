/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON MULTICAST
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MCAST

#include <shared/bsl.h>
#include <soc/dnxc/error.h>

#include <soc/dnxf/ramon/ramon_multicast.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_fabric_multicast.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_max_fabric.h>

shr_error_e
soc_ramon_multicast_mode_get(int unit, soc_dnxf_multicast_table_mode_t* multicast_mode)
{

	SHR_FUNC_INIT_VARS(unit);

    *multicast_mode = soc_dnxf_multicast_table_mode_128k_half;  
    switch (dnxf_data_fabric.multicast.id_range_get(unit))
    {
        case soc_dnxf_multicast_table_mode_64k:
        case soc_dnxf_multicast_table_mode_128k:
        case soc_dnxf_multicast_table_mode_128k_half:
        case soc_dnxf_multicast_table_mode_256k:
        case soc_dnxf_multicast_table_mode_256k_half:
        case soc_dnxf_multicast_table_mode_512k_half:
            *multicast_mode = dnxf_data_fabric.multicast.id_range_get(unit);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Wrong mc_table_mode value %d",
                         dnxf_data_fabric.multicast.id_range_get(unit));
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_multicast_table_size_get(int unit, uint32* mc_table_size)
{
	soc_dnxf_multicast_table_mode_t multicast_mode;
	SHR_FUNC_INIT_VARS(unit);
	SHR_IF_ERR_EXIT(soc_ramon_multicast_mode_get(unit, &multicast_mode));

	switch (multicast_mode)
	{
		case soc_dnxf_multicast_table_mode_64k:
			*mc_table_size = 64*1024;
			break;
		case soc_dnxf_multicast_table_mode_128k:
			*mc_table_size = 128*1024;
			break;
		case soc_dnxf_multicast_table_mode_128k_half:
			*mc_table_size = 128*1024;
			break;
        case soc_dnxf_multicast_table_mode_256k:
            *mc_table_size = 256*1024;
            break;
        case soc_dnxf_multicast_table_mode_256k_half:
            *mc_table_size = 256*1024;
            break;
        case soc_dnxf_multicast_table_mode_512k_half:
            *mc_table_size = 512*1024;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong mc_table_mode value %d",
                         dnxf_data_fabric.multicast.id_range_get(unit));
            break;
	}
exit:
	SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_multicast_table_entry_size_get(int unit, uint32* entry_size)
{
	soc_dnxf_multicast_table_mode_t multicast_mode;
	SHR_FUNC_INIT_VARS(unit);

	SHR_IF_ERR_EXIT(soc_ramon_multicast_mode_get(unit, &multicast_mode));
	switch (multicast_mode)
	{
		case soc_dnxf_multicast_table_mode_64k:
			*entry_size = 192;
			break;
		case soc_dnxf_multicast_table_mode_128k:
			*entry_size = 192;
			break;
		case soc_dnxf_multicast_table_mode_128k_half:
			*entry_size = 96;
			break;
        case soc_dnxf_multicast_table_mode_256k:
            *entry_size = 192;
            break;
        case soc_dnxf_multicast_table_mode_256k_half:
            *entry_size = 96;
            break;
        case soc_dnxf_multicast_table_mode_512k_half:
            *entry_size = 96;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong mc_table_mode value %d",
                         dnxf_data_fabric.multicast.id_range_get(unit));
    }
exit:
	SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_multicast_egress_add(int unit, soc_multicast_t group, soc_gport_t port)
{
    uint32 data[DNXF_DATA_MAX_FABRIC_MULTICAST_TABLE_ROW_SIZE_IN_UINT32];
    SHR_FUNC_INIT_VARS(unit);
    
    SHR_IF_ERR_EXIT(READ_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ANY, group, data));
      
    SHR_BITSET(data, port);
    
    
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, group, data));
    
exit:
    SHR_FUNC_EXIT; 
}


shr_error_e 
soc_ramon_multicast_egress_delete(int unit, soc_multicast_t group, soc_gport_t port)
{
    uint32 data[DNXF_DATA_MAX_FABRIC_MULTICAST_TABLE_ROW_SIZE_IN_UINT32];
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ANY, group, data));
      
    SHR_BITCLR(data, port);
    
    SHR_IF_ERR_EXIT(WRITE_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ALL, group, data));
    
exit:
    SHR_FUNC_EXIT; 
}


shr_error_e 
soc_ramon_multicast_egress_delete_all(int unit, soc_multicast_t group)
{
    bcm_fabric_module_vector_t dest_array = {0};
    SHR_FUNC_INIT_VARS(unit);
    
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_set, (unit, 0, 1, &group, &dest_array, NULL)));
     
exit:
    SHR_FUNC_EXIT; 
}


shr_error_e 
soc_ramon_multicast_egress_get(int unit, soc_multicast_t group, int port_max, soc_gport_t *port_array, int *port_count)
{
    uint32 data[DNXF_DATA_MAX_FABRIC_MULTICAST_TABLE_ROW_SIZE_IN_UINT32];
    int rv, i, port_count_tmp;
    uint32 mc_table_entry_size;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_MULTI_CAST_TABLE_UPDATEm(unit, MEM_BLOCK_ANY, group, data));

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
    SHR_IF_ERR_EXIT(rv);

    port_count_tmp = 0;

    for(i = 0; i < mc_table_entry_size; ++i) {
        if(SHR_BITGET(data, i)) {
            if(port_count_tmp >= port_max)
            {
                *port_count = 0;
                SHR_ERR_EXIT(_SHR_E_PARAM, "port_array is too small");
            }
            port_array[port_count_tmp] = i;
            port_count_tmp++;
        }
    }
    *port_count = port_count_tmp;

exit:
    SHR_FUNC_EXIT; 
}


shr_error_e 
soc_ramon_multicast_egress_set(int unit, soc_multicast_t group, int port_count, soc_gport_t *port_array)
{
    bcm_fabric_module_vector_t dest_array = {0};
    int i_port = 0;
    SHR_FUNC_INIT_VARS(unit);
    
    for(i_port = 0 ; i_port < port_count ; i_port++)
    {
        SHR_BITSET(dest_array, port_array[i_port]);
    }
    
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_fabric_multicast_multi_set, (unit, 0, 1, &group, &dest_array, NULL)));

exit:
    SHR_FUNC_EXIT; 
}

#undef BSL_LOG_MODULE
