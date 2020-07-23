/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_INIT
#include <shared/bsl.h>
#include <sal/core/libc.h>
#include <soc/dcmn/fabric.h>
#include <soc/dcmn/error.h>

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif
#ifdef BCM_DPP_SUPPORT
#include <soc/dpp/mbcm.h>
#endif


int
soc_dcmn_fabric_pipe_map_initalize_valid_configurations(int unit, int min_tdm_priority, soc_dcmn_fabric_pipe_map_t* fabric_pipe_map_valid_config)
{
    int *config_uc;
    int *config_mc;
    int i, index = 0;
    SOCDNX_INIT_FUNC_DEFS;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name ,"UC,MC,TDM", sizeof(fabric_pipe_map_valid_config[0].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_triple_uc_mc_tdm;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;
    for (i = min_tdm_priority; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 2;
    }
    
    index++;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=2; config_mc[3]=1;

    index++;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (2,3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=2;config_mc[2]=config_mc[3]=1;

    index++;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name,"UC,HP MC (1,2,3),LP MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=3;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_triple_uc_hp_mc_lp_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=2;config_mc[1]=config_mc[2]=config_mc[3]=1;

    index++;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name, "NON_TDM,TDM", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=2;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_dual_tdm_non_tdm;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=0;
    for (i = min_tdm_priority; i < SOC_DCMN_FABRIC_PIPE_MAX_NUM_OF_PRIORITIES; i++)
    {
        config_uc[i] = config_mc[i] = 1;
    }

    index++;

    
    sal_strncpy(fabric_pipe_map_valid_config[index].name, "UC,MC", sizeof(fabric_pipe_map_valid_config[index].name));
    fabric_pipe_map_valid_config[index].nof_pipes=2;
    fabric_pipe_map_valid_config[index].mapping_type = soc_dcmn_fabric_pipe_map_dual_uc_mc;
    config_uc = fabric_pipe_map_valid_config[index].config_uc;
    config_mc = fabric_pipe_map_valid_config[index].config_mc;
    config_uc[0]=config_uc[1]=config_uc[2]=config_uc[3]=0;
    config_mc[0]=config_mc[1]=config_mc[2]=config_mc[3]=1;

    SOCDNX_FUNC_RETURN;
}

int
soc_dcmn_fabric_mesh_topology_diag_get(int unit, soc_dcmn_fabric_mesh_topology_diag_t *mesh_topology_diag)
{
    SOCDNX_INIT_FUNC_DEFS;

#ifdef BCM_DFE_SUPPORT
    if (SOC_IS_DFE(unit))
    {

        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_diag_mesh_topology_get, (unit, mesh_topology_diag)));
    } else
#endif 
#ifdef BCM_DPP_SUPPORT
    if (SOC_IS_DPP(unit))
    {
        if (!soc_feature(unit, soc_feature_no_fabric)) {
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_mesh_topology_get, (unit, mesh_topology_diag)));
        }
    } else
#endif 
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("feature unavail")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

