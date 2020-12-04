/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 RX
 */


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_RX

#include <bcm/types.h>

#include <shared/bsl.h>

#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <soc/defs.h>
#include <soc/types.h>
#include <soc/error.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>

#include <soc/dfe/fe3200/fe3200_rx.h>
#include <soc/dfe/fe3200/fe3200_stack.h>

#define LINKS_ARRAY_SIZE (SOC_DFE_MAX_NOF_LINKS/32 + (SOC_DFE_MAX_NOF_LINKS%32? 1:0) + 1 )

soc_error_t
soc_fe3200_rx_cpu_address_modid_set(int unit, int num_of_cpu_addresses, int *cpu_addresses)
{
    int i;
    bcm_module_t alrc_max_base_index, update_base_index;
    uint32 mem_val[LINKS_ARRAY_SIZE];
    uint32 reg_val32;
    soc_dfe_fabric_isolate_type_t isolate_type = soc_dfe_fabric_isolate_type_none;
    soc_dcmn_isolation_status_t isolation_status = soc_dcmn_isolation_status_active;

    SOCDNX_INIT_FUNC_DEFS;

    
    if ((num_of_cpu_addresses < 0) || (SOC_FE3200_RX_MAX_NOF_CPU_BUFFERS > SOC_FE3200_RX_MAX_NOF_CPU_BUFFERS))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("num_of_cpu_addresses invalid \n")));
    }

    
    if (SOC_DFE_IS_FE13(unit))
    {
        SOCDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));
    } else {
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_get,(unit, &isolation_status)));
    }
    if (isolate_type == soc_dfe_fabric_isolate_type_none && isolation_status == soc_dcmn_isolation_status_active) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("bcm_rx_start/stop should not be called when traffic is enabled.\nTo disable traffic use bcm_fabric_control_set with bcmFabricIsolate/bcmFabricShutdown parameter.\n ")));
    }

    

    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_all_reachable_get(unit, &alrc_max_base_index));
    SOCDNX_IF_ERR_EXIT(soc_fe3200_stk_module_max_fap_get(unit, &update_base_index));

    for (i = 0 ; i < num_of_cpu_addresses ; i++)
    {
        if (!((cpu_addresses[i] >= alrc_max_base_index) && (cpu_addresses[i] <= update_base_index)))
        {
            LOG_CLI((BSL_META_U(unit,
                "warning: cpu address is not in the interval [AlrcMaxBaseIndex,UpdateBaseIndex] (the ignored interval for calculating all reachable vector)\n")));
        }
    }


    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_1r(unit, &reg_val32));
    
    if (num_of_cpu_addresses >= 1)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0f, cpu_addresses[0]);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0_ENf, num_of_cpu_addresses >= 1 ? 1:0);

    if (num_of_cpu_addresses >= 2)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1f, cpu_addresses[1]);
    }
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1_ENf, num_of_cpu_addresses >= 2 ? 1:0);

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_1r(unit, reg_val32));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_2r(unit, &reg_val32));

    if (num_of_cpu_addresses >= 3)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_2f, cpu_addresses[2]);
    }

    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_2_ENf, num_of_cpu_addresses >= 3 ? 1:0);

    if (num_of_cpu_addresses >= 4)
    {
        soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_3f, cpu_addresses[3]);
    }

    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val32, FE_DEST_ID_3_ENf, num_of_cpu_addresses >= 4 ? 1:0);

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_2r(unit, reg_val32));

    if (SOC_DFE_IS_FE13(unit) && !SOC_DFE_CONFIG(unit).fabric_local_routing_enable)
    {
        for (i=0 ; i < SOC_DFE_DEFS_GET(unit, nof_instances_dch)/2; i++) { 
            SOCDNX_IF_ERR_EXIT(READ_DCH_GLOBAL_FE_DEST_IDS_1r(unit, i, &reg_val32));
            soc_reg_field_set(unit, DCH_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_0_ENf, 0X0);
            soc_reg_field_set(unit, DCH_GLOBAL_FE_DEST_IDS_1r, &reg_val32, FE_DEST_ID_1_ENf, 0x0);
            SOCDNX_IF_ERR_EXIT(WRITE_DCH_GLOBAL_FE_DEST_IDS_1r(unit, i, reg_val32));
        }

        
        SHR_BITSET_RANGE(mem_val, 0, SOC_DFE_DEFS_GET(unit, nof_links));
        for (i = 0 ; i < num_of_cpu_addresses ; i++)
        {
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit,MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 0, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 1, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 2, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_DUCTPm(unit, 3, MEM_BLOCK_ALL,cpu_addresses[i], mem_val));
        }                  
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_rx_cpu_address_modid_init(int unit)
{
    uint32 reg_val;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_1r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_0_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_1r, &reg_val, FE_DEST_ID_1_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_1r(unit, reg_val));

    SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_FE_DEST_IDS_2r(unit, &reg_val));
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_2_ENf, 0);
    soc_reg_field_set(unit, ECI_GLOBAL_FE_DEST_IDS_2r, &reg_val, FE_DEST_ID_3_ENf, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_FE_DEST_IDS_2r(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME
