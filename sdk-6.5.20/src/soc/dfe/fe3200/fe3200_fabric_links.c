/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 FABRIC LINKS
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>

#include <soc/dcmn/error.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dfe/fe3200/fe3200_fabric_links.h>

 
soc_error_t
soc_fe3200_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val) 
{
    uint64 reg64_val,
        bitmap;
    int blk,
        inner_link,
        nof_links_in_ccs;

    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_ccs = SOC_DFE_DEFS_GET(unit, nof_links_in_ccs);

    blk = link / nof_links_in_ccs;
    inner_link = link % nof_links_in_ccs;

    switch (val)
    {
       case soc_dfe_fabric_link_cell_size_VSC256_V1:
           SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
           bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
           COMPILER_64_BITSET(bitmap, inner_link);
           soc_reg64_field_set(unit, CCS_ARAD_BMPr, &reg64_val, ARAD_BMPf, bitmap);
           SOCDNX_IF_ERR_EXIT(WRITE_CCS_ARAD_BMPr(unit, blk, reg64_val));
           break;

       case soc_dfe_fabric_link_cell_size_VSC256_V2:
           SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
           bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
           COMPILER_64_BITCLR(bitmap, inner_link);
           soc_reg64_field_set(unit, CCS_ARAD_BMPr, &reg64_val, ARAD_BMPf, bitmap);
           SOCDNX_IF_ERR_EXIT(WRITE_CCS_ARAD_BMPr(unit, blk, reg64_val));        
           break;

       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("unsupported  cell format val %d"), val));
           break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val)
{
    uint64 reg64_val,
        bitmap;
    int blk,
        inner_link,
        is_vsc256_v1,
        nof_links_in_ccs;

    SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_ccs = SOC_DFE_DEFS_GET(unit, nof_links_in_ccs);

    blk = link / nof_links_in_ccs;
    inner_link = link % nof_links_in_ccs;


    SOCDNX_IF_ERR_EXIT(READ_CCS_ARAD_BMPr(unit, blk, &reg64_val));
    bitmap = soc_reg64_field_get(unit, CCS_ARAD_BMPr, reg64_val, ARAD_BMPf);
    is_vsc256_v1 = COMPILER_64_BITTEST(bitmap, inner_link);
    *val = is_vsc256_v1 ? 
        soc_dfe_fabric_link_cell_size_VSC256_V1 : soc_dfe_fabric_link_cell_size_VSC256_V2;                     

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_weight_validate(int unit, int val) 
{
    SOCDNX_INIT_FUNC_DEFS;
    if ((val > SOC_FE3200_WFQ_WEIGHT_MAX) || (val <= 0))
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid weight\n")));
    }
    exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_weight_set(int unit, soc_port_t link, int is_prim, int val)
{
    int pipe;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    pipe = is_prim ? 0 : 1;
   
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_set,(unit, pipe, link, soc_dfe_cosq_weight_mode_regular, val)); 
    SOCDNX_IF_ERR_EXIT(rc); 

    exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_fe3200_fabric_links_weight_get(int unit, soc_port_t link, int is_prim, int *val)
{
    int pipe;
    int rc;
    SOCDNX_INIT_FUNC_DEFS;

    pipe = is_prim ? 0 : 1;

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_cosq_pipe_tx_weight_get,(unit, pipe, link, soc_dfe_cosq_weight_mode_regular, val));
    SOCDNX_IF_ERR_EXIT(rc);
    exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_secondary_only_set(int unit, soc_port_t link, int val)
{
    soc_dfe_fabric_link_remote_pipe_mapping_t soc_mapping_config; 
    int rc;

    SOCDNX_INIT_FUNC_DEFS;

    soc_mapping_config.num_of_remote_pipes=1; 
    if (val == 1) {
        soc_mapping_config.remote_pipe_mapping[0] = 1; 
    } else {
        soc_mapping_config.remote_pipe_mapping[0] = 0; 
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pipe_map_set, (unit, link, soc_mapping_config)); 
    SOCDNX_IF_ERR_EXIT(rc); 

    exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_fabric_links_secondary_only_get(int unit, soc_port_t link, int *val)
{
    soc_dfe_fabric_link_remote_pipe_mapping_t soc_mapping_config; 
    int rc, i;

    SOCDNX_INIT_FUNC_DEFS;

    soc_mapping_config.num_of_remote_pipes=0; 
    for (i = 0; i < SOC_DFE_MAX_NOF_PIPES; i ++) {
        soc_mapping_config.remote_pipe_mapping[i] = 0; 
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pipe_map_get, (unit, link, &soc_mapping_config));
    SOCDNX_IF_ERR_EXIT(rc);

    if ( (soc_mapping_config.num_of_remote_pipes==1) && (soc_mapping_config.remote_pipe_mapping[0] == 1) ) {
        *val = 1;
    } else {
        *val = 0;
    }

    exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_fe3200_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable, int empty_cell_size)
{
    int nof_links_in_mac,
        fmac_instance,
        inner_link, inner_link_enable,
        include_empties;
    uint32 reg32_val;
    uint64 reg64_val;
   SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

    fmac_instance = port / nof_links_in_mac;
    inner_link = port % nof_links_in_mac;


    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_TX_LLFC_CELL_SIZEf, empty_cell_size); 
    soc_reg_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf, enable); 
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, reg32_val));


    
   
   if (enable)
   {
       include_empties = 1;
   } else {
       include_empties = 0;
       
       for (inner_link = 0; inner_link < nof_links_in_mac; inner_link++)
       {
           SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
           inner_link_enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf);

           if (inner_link_enable)
           {
               include_empties = 1;
               break;
           }
       }
   }

   SOCDNX_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_instance, &reg64_val));
   soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_LLFC_CELLS_GENf, include_empties); 
   SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_instance, reg64_val));


exit:
   SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_fe3200_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable, int *empty_cell_size)
{
    int nof_links_in_mac,
        fmac_instance,
        inner_link;
    uint32 reg32_val;

   SOCDNX_INIT_FUNC_DEFS;

    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);

    fmac_instance = port / nof_links_in_mac;
    inner_link = port % nof_links_in_mac;

    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr_REG32(unit, fmac_instance, inner_link, &reg32_val));
    *empty_cell_size = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZEf); 
    *enable = soc_reg_field_get(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_TX_LLFC_CELL_SIZE_OVERRIDEf);

exit:
   SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable)
{
    int dch_instance, dcl_instance, fmac_instance;
    int dch_inner_link, dcl_inner_link, fmac_inner_link;
    uint64 reg64_val;
    uint32 reg32_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    
    SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    if (enable)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(READ_DCL_LINK_CELL_PACKING_ENr(unit, dcl_instance, &reg64_val));
    if (enable)
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    SOCDNX_IF_ERR_EXIT(WRITE_DCL_LINK_CELL_PACKING_ENr(unit, dcl_instance, reg64_val));

    
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PCP_ENABLEDf, enable ? 1 : 0);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, reg32_val));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable)
{
    int dch_instance;
    int dch_inner_link;
    uint64 reg64_val;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    *enable = COMPILER_64_BITTEST(reg64_val, dch_inner_link);
    
exit:
    SOCDNX_FUNC_RETURN;
}



#define _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE      (2) 
#define _SOC_FE3200_REMOTE_PIPE_MAPPING_MAX     (2)
soc_error_t 
soc_fe3200_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t pipe_map)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    int dch_instance,
           dch_inner_link,
           dcl_instance,
           dcl_inner_link,
           fmac_instance,
           fmac_inner_link;
    int rv;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    
    if (pipe_map.num_of_remote_pipes > _SOC_FE3200_REMOTE_PIPE_MAPPING_MAX)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_BCM_MSG("Max of supported number of pipe mapping is 2")));
    }
    
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port,&dch_instance,&dch_inner_link, SOC_BLK_DCH)); 
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dcl_instance, &dcl_inner_link, SOC_BLK_DCL));
    SOCDNX_IF_ERR_EXIT(rv);
    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    
    
    rv = READ_DCH_TWO_PIPES_BMPr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 2 || 
        (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2))
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_TWO_PIPES_BMPr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_DCH_THREE_PIPES_BMPr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_THREE_PIPES_BMPr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = READ_DCL_TWO_PIPES_BMPr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 2 ||
        (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 2))
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_TWO_PIPES_BMPr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    rv = READ_DCL_THREE_PIPES_BMPr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes == 0 && SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes  == 3)
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_THREE_PIPES_BMPr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , &reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf, 
                      pipe_map.num_of_remote_pipes == 0 ? SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes - 1  : pipe_map.num_of_remote_pipes - 1);
    rv  = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    if (pipe_map.num_of_remote_pipes >= 1)
    {
        
        rv = READ_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[0], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = READ_DCL_REMOTE_PRI_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dcl_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[0], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCL_REMOTE_PRI_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

    }

    
    if (pipe_map.num_of_remote_pipes >= 2)
    {
        
        rv = READ_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[1], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);

        
        rv = READ_DCL_REMOTE_SEC_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, dcl_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[1], 0, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
        rv = WRITE_DCL_REMOTE_SEC_PIPE_IDXr(unit, dcl_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    

    
    rv = READ_DCH_PIPES_MAP_ENr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes != 0 )
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    rv = WRITE_DCH_PIPES_MAP_ENr(unit, dch_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv = READ_DCL_PIPES_MAP_ENr(unit, dcl_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (pipe_map.num_of_remote_pipes != 0 )
    {
        COMPILER_64_BITSET(reg64_val, dcl_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dcl_inner_link);
    }
    rv = WRITE_DCL_PIPES_MAP_ENr(unit, dcl_instance, reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dfe_fabric_link_remote_pipe_mapping_t *pipe_map)
{
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val_2_pipes,
           reg64_val;
    int dch_instance,
           dch_inner_link;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

   


    
    rv = READ_DCH_PIPES_MAP_ENr(unit, dch_instance, &reg64_val);
    SOCDNX_IF_ERR_EXIT(rv);
    if (!COMPILER_64_BITTEST(reg64_val, dch_inner_link))
    {
        
        pipe_map->num_of_remote_pipes= 0;
        SOC_EXIT;
    }

    
    
    rv = READ_DCH_TWO_PIPES_BMPr(unit, dch_instance, &reg64_val_2_pipes);
    SOCDNX_IF_ERR_EXIT(rv);
    if (COMPILER_64_BITTEST(reg64_val_2_pipes, dch_inner_link)) {
        pipe_map->num_of_remote_pipes = 2;
    } else {
        pipe_map->num_of_remote_pipes = 1;
    }


    
    if (pipe_map->num_of_remote_pipes >= 1)
    {
        
        rv = READ_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[0], 0, reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
    }

    
    if (pipe_map->num_of_remote_pipes >= 2)
    {
        
        rv = READ_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SOCDNX_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[1], 0, reg_above_64_val, dch_inner_link*_SOC_FE3200_REMOTE_PIPE_INDEX_SIZE, _SOC_FE3200_REMOTE_PIPE_INDEX_SIZE);
    }

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_fe3200_fabric_links_repeater_nof_remote_pipe_set(int unit, int port, uint32 nof_remote_pipe)
{
    int rv;
    int fmac_instance, fmac_inner_link;
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    
    rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , &reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf, 
                      nof_remote_pipe == 0 ? SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes - 1  : nof_remote_pipe - 1);
    rv  = WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_fe3200_fabric_links_repeater_nof_remote_pipe_get(int unit, int port, uint32 *nof_remote_pipe)
{
    int rv;
    int fmac_instance, fmac_inner_link; 
    uint32 reg32_val;
    SOCDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SOCDNX_IF_ERR_EXIT(rv);

    rv  = READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link , &reg32_val);
    SOCDNX_IF_ERR_EXIT(rv);
    *nof_remote_pipe = soc_reg_field_get(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, reg32_val, FMAL_N_PARALLEL_DATA_PATHf) + 1; 

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_cell_format_verify(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t val) 
{
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("The cell format is automatically according to PCP configuration and number pipes.\n")));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t
soc_fe3200_fabric_links_cell_format_get(int unit, soc_port_t link, soc_dfe_fabric_link_cell_size_t *val)
{
    SOCDNX_INIT_FUNC_DEFS;

    *val = soc_dfe_fabric_link_cell_size_VSC256_V2;

    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
