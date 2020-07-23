/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON FABRIC LINKS
 */
#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_FABRIC
#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/error.h>

#include <soc/dnxc/error.h>

#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

#include <soc/dnxf/swstate/auto_generated/access/dnxf_access.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_fabric.h>
#include <soc/dnxf/cmn/dnxf_port.h>

#include <soc/dnxf/cmn/mbcm.h>

#include <soc/dnxf/ramon/ramon_fabric_links.h>

#include <soc/portmod/portmod.h>
#include <phymod/phymod.h>



#define SOC_RAMON_FABRIC_LINKS_VSC256_MAX_CELL_SIZE        (256  + 16  + 1)
#define SOC_RAMON_FABRIC_LINKS_ALDWP_MIN                   (0x2)

#define SOC_RAMON_FABRIC_LINKS_FMAC_INDEX_GET(unit, link_index) \
    ((link_index) / (dnxf_data_device.blocks.nof_links_in_fmac_get(unit)))

#define SOC_RAMON_FABRIC_LINKS_CORE_INDEX_GET(unit, link_index) \
    ((link_index) / (dnxf_data_device.blocks.nof_links_in_phy_core_get(unit)))

#define SOC_RAMON_FABRIC_LINKS_INDEX_IN_FMAC_GET(unit, lane_index_in_pm) \
    ((lane_index_in_pm) % (dnxf_data_device.blocks.nof_links_in_fmac_get(unit)))

#define SOC_RAMON_FABRIC_LINKS_INDEX_IN_CORE_GET(unit, lane_index_in_pm) \
    ((lane_index_in_pm) % (dnxf_data_device.blocks.nof_links_in_phy_core_get(unit)))
 

shr_error_e
soc_ramon_fabric_links_validate_link(int unit, soc_port_t link) {

    SHR_FUNC_INIT_VARS(unit);

    if (link < 0 || link >= dnxf_data_port.general.nof_links_get(unit) || !IS_SFI_PORT(unit, link)) {
        SHR_ERR_EXIT(_SHR_E_PARAM, "link %d is out of range",link);
    }

exit:
   SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_fabric_links_flow_status_control_cell_format_set(int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t val) 
{
    uint32 reg32_val,
        bitmap[1];
    int blk,
        inner_link,
        nof_links_in_cch;

    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_cch = dnxf_data_device.blocks.nof_links_in_cch_get(unit);

    blk = link / nof_links_in_cch;
    inner_link = link % nof_links_in_cch;

    switch (val)
    {
       case soc_dnxf_fabric_link_cell_size_VSC256_V1:
           SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_ARAD_BMPr, blk, 0, &reg32_val));
           *bitmap = soc_reg_field_get(unit, CCH_ARAD_BMPr, reg32_val, ARAD_BMPf);
           SHR_BITSET(bitmap, inner_link);
           soc_reg_field_set(unit, CCH_ARAD_BMPr, &reg32_val, ARAD_BMPf, *bitmap);
           SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_ARAD_BMPr, blk, 0, reg32_val));
           break;

       case soc_dnxf_fabric_link_cell_size_VSC256_V2:
           SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_ARAD_BMPr, blk, 0, &reg32_val));
           *bitmap = soc_reg_field_get(unit, CCH_ARAD_BMPr, reg32_val, ARAD_BMPf);
           SHR_BITCLR(bitmap, inner_link);
           soc_reg_field_set(unit, CCH_ARAD_BMPr, &reg32_val, ARAD_BMPf, *bitmap);
           SHR_IF_ERR_EXIT(soc_reg32_set(unit, CCH_ARAD_BMPr, blk, 0, reg32_val));
           break;

       default:
           SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported  cell format val %d", val);
           break;
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_fabric_links_flow_status_control_cell_format_get(int unit, soc_port_t link, soc_dnxf_fabric_link_cell_size_t *val)
{
    uint32 reg32_val,
        bitmap[1];
    int blk,
        inner_link,
        is_vsc256_v1,
        nof_links_in_cch;

    SHR_FUNC_INIT_VARS(unit);

    nof_links_in_cch = dnxf_data_device.blocks.nof_links_in_cch_get(unit);

    blk = link / nof_links_in_cch;
    inner_link = link % nof_links_in_cch;


    SHR_IF_ERR_EXIT(soc_reg32_get(unit, CCH_ARAD_BMPr, blk, 0, &reg32_val));

    *bitmap = soc_reg_field_get(unit, CCH_ARAD_BMPr, reg32_val, ARAD_BMPf);
    is_vsc256_v1 = SHR_BITGET(bitmap, inner_link);
    *val = is_vsc256_v1 ? 
        soc_dnxf_fabric_link_cell_size_VSC256_V1 : soc_dnxf_fabric_link_cell_size_VSC256_V2;                     

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_fabric_link_repeater_enable_set(int unit, soc_port_t port, int enable)
{
    int nof_links_in_fmac = 0,
        fmac_instance = 0,
        include_empties = 0,
        link = 0,
        first_link_in_fmac = 0;
    int port_original_state;
    uint8 is_link_connected = 0;
    uint64 reg64_val;
    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(reg64_val);

    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    fmac_instance = port / nof_links_in_fmac;

    
    if (enable)
    {
        SHR_IF_ERR_EXIT(dnxf_state.port.is_connected_to_repeater.bit_set(unit, port));
        include_empties = 1;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnxf_state.port.is_connected_to_repeater.bit_clear(unit, port));

        first_link_in_fmac = fmac_instance * nof_links_in_fmac;

        for (link = first_link_in_fmac; link < first_link_in_fmac + nof_links_in_fmac; ++link)
        {
            SHR_IF_ERR_EXIT(dnxf_state.port.is_connected_to_repeater.bit_get(unit, link, &is_link_connected));
            if (is_link_connected)
            {
                include_empties = 1;
                break;
            }
        }
    }
    
    SHR_IF_ERR_EXIT(soc_dnxf_port_disable_and_save_enable_state(unit, port, &port_original_state));

    
    SHR_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_instance, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_LLFC_CELLS_GENf, include_empties); 
    SHR_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_instance, reg64_val));

    COMPILER_64_ZERO(reg64_val);
    SHR_IF_ERR_EXIT(READ_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_instance, &reg64_val));
    soc_reg64_field32_set(unit, FMAC_GENERAL_CONFIGURATION_REGISTERr, &reg64_val, GENERAL_CONFIGURATION_REGISTER_FIELD_1f, include_empties);
    SHR_IF_ERR_EXIT(WRITE_FMAC_GENERAL_CONFIGURATION_REGISTERr(unit, fmac_instance, reg64_val));

    
    SHR_IF_ERR_EXIT(soc_dnxf_port_restore_enable_state(unit, port, port_original_state));

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_fabric_link_repeater_enable_get(int unit, soc_port_t port, int *enable)
{
    uint8 is_link_connected = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnxf_state.port.is_connected_to_repeater.bit_get(unit, port, &is_link_connected));
    *enable = is_link_connected;

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_links_pcp_enable_set(int unit, soc_port_t port, int enable)
{
    int dch_instance, fmac_instance;
    int dch_inner_link, fmac_inner_link;
    uint64 reg64_val;
    uint32 reg32_val;
    int orig_enabled;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SHR_IF_ERR_EXIT(rv);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC));
    SHR_IF_ERR_EXIT(rv);

    
    SHR_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    if (enable)
    {
        COMPILER_64_BITSET(reg64_val, dch_inner_link);
    } else {
        COMPILER_64_BITCLR(reg64_val, dch_inner_link);
    }
    SHR_IF_ERR_EXIT(WRITE_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, reg64_val));

    SHR_IF_ERR_EXIT(soc_dnxf_port_disable_and_save_enable_state(unit, port, &orig_enabled));

    
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PCP_ENABLEDf, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, reg32_val));

    
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, &reg64_val));
    soc_reg64_field_set(unit, FMAC_FMAL_TX_GENERAL_CONFIGURATIONr, &reg64_val, FMAL_N_TX_SIMPLE_UNPACKING_ENf, enable ? 0 : 1);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_TX_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, reg64_val));

    SHR_IF_ERR_EXIT(soc_dnxf_port_restore_enable_state(unit, port, orig_enabled));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_links_pcp_enable_get(int unit, soc_port_t port, int *enable)
{
    int dch_instance;
    int dch_inner_link;
    uint64 reg64_val;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(READ_DCH_LINK_CELL_PACKING_ENr(unit, dch_instance, &reg64_val));
    *enable = COMPILER_64_BITTEST(reg64_val, dch_inner_link);
    
exit:
    SHR_FUNC_EXIT;
}



#define _SOC_RAMON_REMOTE_PIPE_INDEX_SIZE      (2) 
#define _SOC_RAMON_REMOTE_PIPE_MAPPING_MAX     (2)
#define _SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS      (3) 
shr_error_e 
soc_ramon_fabric_links_pipe_map_set(int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t pipe_map)
{
    uint32 reg32_val = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    int fmac_instance = 0, fmac_inner_link = 0;
    int instance_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {0};
    int inner_link_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {0};
    int block_i = 0;
    soc_reg_t two_pipes_bmp_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {DCH_TWO_PIPES_BMPr, DCML_TWO_PIPES_BMPr, LCM_TWO_PIPES_BMPr};
    soc_reg_t three_pipes_bmp_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {DCH_THREE_PIPES_BMPr, DCML_THREE_PIPES_BMPr, LCM_THREE_PIPES_BMPr};
    soc_reg_t remote_pri_pipe_idx_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {DCH_REMOTE_PRI_PIPE_IDXr, DCML_REMOTE_PRI_PIPE_IDXr, LCM_REMOTE_PRI_PIPE_IDXr};
    soc_reg_t remote_sec_pipe_idx_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {DCH_REMOTE_SEC_PIPE_IDXr, DCML_REMOTE_SEC_PIPE_IDXr, LCM_REMOTE_SEC_PIPE_IDXr};
    soc_reg_t pipes_mapping_en_array[_SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS] = {DCH_PIPES_MAPPING_ENr, DCML_PIPES_MAPPING_ENr, LCM_PIPES_MAPPING_ENr};
    SHR_FUNC_INIT_VARS(unit);

    
    if (pipe_map.num_of_remote_pipes > _SOC_RAMON_REMOTE_PIPE_MAPPING_MAX)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Max of supported number of pipe mapping is 2");
    }
    
    
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &instance_array[0], &inner_link_array[0], SOC_BLK_DCH))); 
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &instance_array[1], &inner_link_array[1], SOC_BLK_DCML)));
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &instance_array[2], &inner_link_array[2], SOC_BLK_LCM)));
    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &fmac_instance, &fmac_inner_link, SOC_BLK_FMAC)));

    
    for (block_i = 0; block_i < _SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS; ++block_i)
    {
        
        reg32_val = 0;

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, two_pipes_bmp_array[block_i], instance_array[block_i], 0, &reg32_val));
        if (pipe_map.num_of_remote_pipes == 2 || 
            (pipe_map.num_of_remote_pipes == 0 && dnxf_data_fabric.pipes.nof_pipes_get(unit) == 2))
        {
            SHR_BITSET(&reg32_val, inner_link_array[block_i]);
        } else {
            SHR_BITCLR(&reg32_val, inner_link_array[block_i]);
        }
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, two_pipes_bmp_array[block_i], instance_array[block_i], 0, reg32_val));

        
        reg32_val = 0;

        SHR_IF_ERR_EXIT(soc_reg32_get(unit, three_pipes_bmp_array[block_i], instance_array[block_i], 0, &reg32_val));
        if (pipe_map.num_of_remote_pipes == 0 && dnxf_data_fabric.pipes.nof_pipes_get(unit) == 3)
        {
            SHR_BITSET(&reg32_val, inner_link_array[block_i]);
        } else {
            SHR_BITCLR(&reg32_val, inner_link_array[block_i]);
        }
        SHR_IF_ERR_EXIT(soc_reg32_set(unit, three_pipes_bmp_array[block_i], instance_array[block_i], 0, reg32_val));
    }

    
    reg32_val = 0;

    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, &reg32_val));
    soc_reg_field_set(unit, FMAC_FMAL_GENERAL_CONFIGURATIONr, &reg32_val, FMAL_N_PARALLEL_DATA_PATHf, 
                      pipe_map.num_of_remote_pipes == 0 ? dnxf_data_fabric.pipes.nof_pipes_get(unit) - 1  : pipe_map.num_of_remote_pipes - 1);
    SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_GENERAL_CONFIGURATIONr(unit, fmac_instance, fmac_inner_link, reg32_val));

    
    if (pipe_map.num_of_remote_pipes >= 1)
    {
        
        for (block_i = 0; block_i < _SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS; ++block_i)
        {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, remote_pri_pipe_idx_array[block_i], instance_array[block_i], 0, reg_above_64_val));
            SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, inner_link_array[block_i]*_SOC_RAMON_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[0], 0, _SOC_RAMON_REMOTE_PIPE_INDEX_SIZE);
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, remote_pri_pipe_idx_array[block_i], instance_array[block_i], 0, reg_above_64_val));
        }
    }

    
    if (pipe_map.num_of_remote_pipes >= 2)
    {
        
        for (block_i = 0; block_i < _SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS; ++block_i)
        {
            SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

            SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, remote_sec_pipe_idx_array[block_i], instance_array[block_i], 0, reg_above_64_val));
            SOC_REG_ABOVE_64_RANGE_COPY(reg_above_64_val, inner_link_array[block_i]*_SOC_RAMON_REMOTE_PIPE_INDEX_SIZE, &pipe_map.remote_pipe_mapping[1], 0, _SOC_RAMON_REMOTE_PIPE_INDEX_SIZE);
            SHR_IF_ERR_EXIT(soc_reg_above_64_set(unit, remote_sec_pipe_idx_array[block_i], instance_array[block_i], 0, reg_above_64_val));
        }
    }

    
    for (block_i = 0; block_i < _SOC_RAMON_REMOTE_PIPE_NOF_BLOCKS; ++block_i)
    {
        reg32_val = 0;

        soc_reg32_get(unit, pipes_mapping_en_array[block_i], instance_array[block_i], 0, &reg32_val);
        if (pipe_map.num_of_remote_pipes != 0 )
        {
            SHR_BITSET(&reg32_val, inner_link_array[block_i]);
        } else {
            SHR_BITCLR(&reg32_val, inner_link_array[block_i]);
        }
        soc_reg32_set(unit, pipes_mapping_en_array[block_i], instance_array[block_i], 0, reg32_val);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_links_pipe_map_get(int unit, soc_port_t port, soc_dnxf_fabric_link_remote_pipe_mapping_t *pipe_map)
{
    uint32 reg32_val = 0;
    soc_reg_above_64_val_t reg_above_64_val;
    uint64 reg64_val;
    int dch_instance,
           dch_inner_link;
    int rv;
    SHR_FUNC_INIT_VARS(unit);

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SHR_IF_ERR_EXIT(rv);

   
    
    rv = READ_DCH_PIPES_MAPPING_ENr(unit, dch_instance, &reg32_val);
    SHR_IF_ERR_EXIT(rv);
    if (!SHR_BITGET(&reg32_val, dch_inner_link))
    {
        
        pipe_map->num_of_remote_pipes= 0;
        SHR_EXIT();
    }

    
    
    rv = READ_DCH_TWO_PIPES_BMPr(unit, dch_instance, &reg64_val);
    SHR_IF_ERR_EXIT(rv);
    if (COMPILER_64_BITTEST(reg64_val, dch_inner_link)) {
        pipe_map->num_of_remote_pipes = 2;
    } else {
        pipe_map->num_of_remote_pipes = 1;
    }

    
    if (pipe_map->num_of_remote_pipes >= 1)
    {
        
        rv = READ_DCH_REMOTE_PRI_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SHR_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[0], 0, reg_above_64_val, dch_inner_link*_SOC_RAMON_REMOTE_PIPE_INDEX_SIZE, _SOC_RAMON_REMOTE_PIPE_INDEX_SIZE);
    }

    
    if (pipe_map->num_of_remote_pipes >= 2)
    {
        
        rv = READ_DCH_REMOTE_SEC_PIPE_IDXr(unit, dch_instance, reg_above_64_val);
        SHR_IF_ERR_EXIT(rv);
        SOC_REG_ABOVE_64_RANGE_COPY(&pipe_map->remote_pipe_mapping[1], 0, reg_above_64_val, dch_inner_link*_SOC_RAMON_REMOTE_PIPE_INDEX_SIZE, _SOC_RAMON_REMOTE_PIPE_INDEX_SIZE);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_links_link_integrity_down_set(int unit, soc_port_t port, int force_down)
{
    soc_reg_above_64_val_t reg_above_64_val;
    SHR_FUNC_INIT_VARS(unit);

    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

    
    SHR_IF_ERR_EXIT(READ_RTP_FORCE_LINK_INTEGRITY_DOWNr(unit, reg_above_64_val));
    SHR_BITWRITE(reg_above_64_val, port, force_down);
    SHR_IF_ERR_EXIT(WRITE_RTP_FORCE_LINK_INTEGRITY_DOWNr(unit, reg_above_64_val));

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_links_isolate_set(int unit, soc_port_t link, soc_dnxc_isolation_status_t val) {
    soc_reg_above_64_val_t reachability_allowed_bm;
    soc_reg_above_64_val_t multicast_allowed_bm;
    soc_reg_above_64_val_t specific_link_bm;
    uint32 reg32_val;
    uint64 reg64_val, reg64_ones;
    SHR_FUNC_INIT_VARS(unit);

    if (soc_dnxc_isolation_status_active == val) {
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);

        SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));
        SOC_REG_ABOVE_64_OR(reachability_allowed_bm, specific_link_bm);
        SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

        SHR_IF_ERR_EXIT(READ_QRH_MULICAST_ALLOWED_LINKS_REGISTERr(unit, 0, multicast_allowed_bm));
        SOC_REG_ABOVE_64_OR(multicast_allowed_bm, specific_link_bm);
        SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));
    } else {
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);
        SOC_REG_ABOVE_64_NOT(specific_link_bm);

        SHR_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit, &reg32_val));
        
        SHR_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, 0x0));

        SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));
        SOC_REG_ABOVE_64_AND(reachability_allowed_bm, specific_link_bm);
        SHR_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

        sal_usleep(10000);

        
        SHR_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, reg32_val));

        
        SHR_IF_ERR_EXIT(READ_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, &reg64_val));
        COMPILER_64_ALLONES(reg64_ones);
        SHR_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_ones));

        
        SHR_IF_ERR_EXIT(READ_QRH_MULICAST_ALLOWED_LINKS_REGISTERr(unit, 0, multicast_allowed_bm));
        SOC_REG_ABOVE_64_AND(multicast_allowed_bm, specific_link_bm);
        SHR_IF_ERR_EXIT(WRITE_BRDC_QRH_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));

        sal_usleep(1000);

        SHR_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_val));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_links_isolate_get(int unit, soc_port_t link, soc_dnxc_isolation_status_t *val) {
    soc_reg_above_64_val_t reachability_allowed_bm;
    soc_reg_above_64_val_t specific_link_bm;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

    SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);

    SOC_REG_ABOVE_64_AND(specific_link_bm, reachability_allowed_bm);
    if (!SOC_REG_ABOVE_64_IS_ZERO(specific_link_bm)) *val = soc_dnxc_isolation_status_active;
    else *val = soc_dnxc_isolation_status_isolated;

exit:
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_fabric_links_llf_control_source_set(int unit, soc_port_t link, soc_dnxc_fabric_pipe_t val) {
   soc_port_t inner_lnk;
   int link_original_state;
   uint32 blk_id, reg_val;
   uint32 field_val_rx[1], field_val_tx[1];
   SHR_FUNC_INIT_VARS(unit);

   blk_id = INT_DEVIDE(link, 4);
   inner_lnk = link % 4;

   
   SHR_IF_ERR_EXIT(soc_dnxf_port_disable_and_save_enable_state(unit, link, &link_original_state));

   
   SHR_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, &reg_val));
   *field_val_rx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
   *field_val_tx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
   if (SOC_DNXC_FABRIC_PIPE_ALL_PIPES_IS_CLEAR(val)) {
      SHR_BITCLR(field_val_rx, inner_lnk);
      SHR_BITCLR(field_val_tx, inner_lnk);
   } else {
      SHR_BITSET(field_val_rx, inner_lnk);
      SHR_BITSET(field_val_tx, inner_lnk);
   }
   soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_RX_ENf, *field_val_rx);
   soc_reg_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_TX_ENf, *field_val_tx);
   SHR_IF_ERR_EXIT(WRITE_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, reg_val));

   
   SHR_IF_ERR_EXIT(soc_dnxf_port_restore_enable_state(unit, link, link_original_state));
exit:
   SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_links_llf_control_source_get(int unit, soc_port_t link, soc_dnxc_fabric_pipe_t *val) {
   soc_port_t inner_lnk;
   int nof_pipes;
   uint32 blk_id, reg_val;
   uint32 field_val_rx[1], field_val_tx[1];
   SHR_FUNC_INIT_VARS(unit);

   blk_id = INT_DEVIDE(link, 4);
   inner_lnk = link % 4;
   nof_pipes = dnxf_data_fabric.pipes.nof_pipes_get(unit);
   
   SHR_IF_ERR_EXIT(READ_FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr(unit, blk_id, &reg_val));
   *field_val_rx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
   *field_val_tx = soc_reg_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
   if (!SHR_BITGET(field_val_rx, inner_lnk) && !SHR_BITGET(field_val_tx, inner_lnk)) {
       SOC_DNXC_FABRIC_PIPE_INIT(*val);
   } else {
       SOC_DNXC_FABRIC_PIPE_ALL_PIPES_SET(val,nof_pipes);       
   }

exit:
   SHR_FUNC_EXIT;
}




shr_error_e 
soc_ramon_fabric_links_aldwp_config(int unit, soc_port_t port)
{
    int rv;
    uint32 max_cell_size, aldwp, highest_aldwp, max_aldwp;
    int speed;
    int dch_instance, dch_inner_link;
    int offset=0;
    soc_dnxf_fabric_link_device_mode_t device_mode;
    soc_port_t port_index;
    uint32 reg32_val;
    int enable = 0, pipe_index;
    soc_pbmp_t pbmp;
    bcm_port_resource_t resource;

    SHR_FUNC_INIT_VARS(unit);

    

    

    
    highest_aldwp = 0;
    

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SHR_IF_ERR_EXIT(rv);

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_link_device_mode_get,(unit,port, 1, &device_mode)));

    rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_drv_block_pbmp_get, (unit, SOC_BLK_DCH, dch_instance, &pbmp));
    SHR_IF_ERR_EXIT(rv);
    SOC_PBMP_ITER(pbmp, port_index)
    {
        
        rv = MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_port_enable_get, (unit, port_index,&enable)); 
        SHR_IF_ERR_EXIT(rv);
        if (!enable)
        {
            
            continue;
        }

        rv = soc_dnxc_port_resource_get(unit, port_index, &resource);
        SHR_IF_ERR_EXIT(rv);

        speed = resource.speed;

        max_cell_size = SOC_RAMON_FABRIC_LINKS_VSC256_MAX_CELL_SIZE;

        
        aldwp = (3* max_cell_size * dnxf_data_device.general.core_clock_khz_get(unit)) / (speed / 8 ); 
        aldwp = aldwp /(1024*64)  + ((aldwp % (1024*64) != 0) ? 1 : 0) ;

        
        highest_aldwp = (aldwp > highest_aldwp ? aldwp : highest_aldwp); 
        
    }

    
    if (highest_aldwp != 0 )
    {
        if (device_mode == soc_dnxf_fabric_link_device_mode_fe2)
        {
            offset=1;
        }
        else if (device_mode == soc_dnxf_fabric_link_device_mode_multi_stage_fe3)
        {
            offset=2;
        }
         highest_aldwp += offset;
    }


    max_aldwp = dnxf_data_port.general.aldwp_max_val_get(unit);
    
    if (device_mode == soc_dnxf_fabric_link_device_mode_repeater)
    {
        highest_aldwp=0;
    }
    else if (highest_aldwp == 0) 
    {
        
        SHR_EXIT();
    } else if (highest_aldwp < SOC_RAMON_FABRIC_LINKS_ALDWP_MIN)
    {
        highest_aldwp = SOC_RAMON_FABRIC_LINKS_ALDWP_MIN;
    } else if (highest_aldwp > max_aldwp)
    {
        highest_aldwp = max_aldwp;
    }

    
    
    for (pipe_index=0 ; pipe_index < dnxf_data_fabric.pipes.nof_pipes_get(unit) ; pipe_index++)
    {
        SHR_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_2_Pr(unit, dch_instance, pipe_index, &reg32_val));
        soc_reg_field_set(unit, DCH_DCH_ENABLERS_REGISTER_2_Pr, &reg32_val, AUTO_DOC_NAME_11f, highest_aldwp);
        SHR_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_2_Pr(unit, dch_instance, pipe_index, reg32_val));
    }

exit:
   SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_fabric_links_aldwp_init(int unit)
{
    int rv;
    int dch_instance,
        nof_dch,
        nof_links_in_dcq;
    SHR_FUNC_INIT_VARS(unit);

    nof_dch = dnxf_data_device.blocks.nof_instances_dch_get(unit);
    nof_links_in_dcq = dnxf_data_device.blocks.nof_links_in_dcq_get(unit);

    for (dch_instance = 0; dch_instance < nof_dch; dch_instance++)
    {
        rv = soc_ramon_fabric_links_aldwp_config(unit, dch_instance *  nof_links_in_dcq);
        SHR_IF_ERR_EXIT(rv);
    }
 
exit:
   SHR_FUNC_EXIT;
}

shr_error_e 
soc_ramon_fabric_link_device_mode_get(int unit,soc_port_t port, int is_rx, soc_dnxf_fabric_link_device_mode_t *link_device_mode)
{
    int nof_links = 0;
    SHR_FUNC_INIT_VARS(unit);

    nof_links = dnxf_data_port.general.nof_links_get(unit);
    switch (dnxf_data_fabric.general.device_mode_get(unit))
    {
       case soc_dnxf_fabric_device_mode_multi_stage_fe2:
       case soc_dnxf_fabric_device_mode_single_stage_fe2:
           *link_device_mode=soc_dnxf_fabric_link_device_mode_fe2;
           break;

       case soc_dnxf_fabric_device_mode_multi_stage_fe13:
           if (is_rx)
           {
               *link_device_mode = (port < nof_links / 2) ? soc_dnxf_fabric_link_device_mode_multi_stage_fe1 : soc_dnxf_fabric_link_device_mode_multi_stage_fe3;
           } else {
               *link_device_mode = (port < nof_links / 2) ? soc_dnxf_fabric_link_device_mode_multi_stage_fe3 : soc_dnxf_fabric_link_device_mode_multi_stage_fe1;
           }
           break;

       default:
           SHR_ERR_EXIT(_SHR_E_FAIL, "device is not in a valid mode");
           break;
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_ramon_fabric_links_tx_swap_set(int unit, soc_port_t link_a, soc_port_t link_b, int enable)
{
    phymod_lane_map_t lane_map;
    dnxc_port_fabric_init_config_t port_config;
    uint32 flags = 0;
    int link_a_index, link_b_index;
    int pm_index;
    SHR_FUNC_INIT_VARS(unit);

    
    pm_index = link_a / dnxf_data_device.blocks.nof_links_in_phy_core_get(unit);
    SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_pm_map_get(unit, pm_index, dnxf_data_device.blocks.nof_links_in_phy_core_get(unit), port_config.lane2serdes));

    
    link_a_index = SOC_RAMON_FABRIC_LINKS_INDEX_IN_CORE_GET(unit, link_a);
    link_b_index = SOC_RAMON_FABRIC_LINKS_INDEX_IN_CORE_GET(unit, link_b);

    
    PORTMOD_LANE_MAP_MAC_ONLY_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_lane_map_get(unit, link_a, flags, &lane_map));


    if (enable)
    {
        
        lane_map.lane_map_tx[link_a_index] = port_config.lane2serdes[link_b_index].tx_id;
        lane_map.lane_map_tx[link_b_index] = port_config.lane2serdes[link_a_index].tx_id;
    }
    else
    {
        
        lane_map.lane_map_tx[link_a_index] = port_config.lane2serdes[link_a_index].tx_id;
        lane_map.lane_map_tx[link_b_index] = port_config.lane2serdes[link_b_index].tx_id;
    }

    
    SHR_IF_ERR_EXIT(portmod_port_lane_map_set(unit, link_a, flags, &lane_map));

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_ramon_fabric_links_retimer_loopback_peer_get(int unit, soc_port_t link_a, soc_port_t *link_b)
{
    phymod_lane_map_t lane_map;
    dnxc_port_fabric_init_config_t port_config;
    uint32 flags = 0;
    int link_a_index;
    int pm_index;
    int lane_index;
    SHR_FUNC_INIT_VARS(unit);

    *link_b = -1;

    
    pm_index = link_a / dnxf_data_device.blocks.nof_links_in_phy_core_get(unit);
    SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_pm_map_get(unit, pm_index, dnxf_data_device.blocks.nof_links_in_phy_core_get(unit), port_config.lane2serdes));

    
    link_a_index = SOC_RAMON_FABRIC_LINKS_INDEX_IN_CORE_GET(unit, link_a);

    
    PORTMOD_LANE_MAP_MAC_ONLY_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_lane_map_get(unit, link_a, flags, &lane_map));

    
    for (lane_index = 0 ; lane_index < dnxf_data_device.blocks.nof_links_in_phy_core_get(unit) ; lane_index++)
    {
        if (lane_map.lane_map_tx[link_a_index] == port_config.lane2serdes[lane_index].tx_id)
        {
            
            *link_b = lane_index + SOC_RAMON_FABRIC_LINKS_CORE_INDEX_GET(unit, link_a) * dnxf_data_device.blocks.nof_links_in_phy_core_get(unit);
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


static shr_error_e
soc_ramon_fabric_links_retimer_cdr_lock_poll(int unit, soc_port_t link, int min_nof_polls, int usec_timeout, int* rx_locked)
{
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    soc_timeout_init(&to, usec_timeout, min_nof_polls);
    while(1)
    {
        SHR_IF_ERR_EXIT(portmod_port_cdr_lock_get(unit, link, rx_locked));
        if (*rx_locked)
        {
            break;
        }

        if (soc_timeout_check(&to))
        {
            *rx_locked = 0;
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_link_retimer_calibrate(int unit,  soc_port_t link_a, soc_port_t link_b)
{
    int min_nof_polls, usec_timeout, rx_locked;
    SHR_FUNC_INIT_VARS(unit);

    
    
    
    min_nof_polls = 1;
    
    usec_timeout = 30000;

    SHR_IF_ERR_EXIT(soc_ramon_fabric_links_retimer_cdr_lock_poll(unit, link_a, min_nof_polls, usec_timeout, &rx_locked));
    if (!rx_locked)
    {
        SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Link %d: timeout - Rx is not locked\n", link_a);
    }

    if (link_b != link_a)
    {
        SHR_IF_ERR_EXIT(soc_ramon_fabric_links_retimer_cdr_lock_poll(unit, link_b, min_nof_polls, usec_timeout, &rx_locked));
        if (!rx_locked)
        {
            SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Link %d: timeout - Rx is not locked\n", link_b);
        }
    }

    
    
    
    
    sal_usleep(100000);

    SHR_IF_ERR_EXIT(portmod_port_pass_through_calibrate(unit, link_a));
    if (link_b != link_a)
    {
        SHR_IF_ERR_EXIT(portmod_port_pass_through_calibrate(unit, link_b));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_link_retimer_connect_set(int unit, soc_port_t link_a, soc_port_t link_b)
{
    soc_port_t connected_link = -1;
    int enable = FALSE;
    int loopback_en = FALSE;
    int tx_swap_required = FALSE;
    int is_enabled = -1;
    int is_loopback = -1;
    int link_a_fec_enabled = 0, link_b_fec_enabled = 0;
    portmod_speed_config_t link_a_speed_config, link_b_speed_config;
    int flags = 0;
    int link_a_enabled, connected_link_enabled;
    SHR_FUNC_INIT_VARS(unit);

    
    
    
    
    if (link_b != -1)
    {
        
        SHR_IF_ERR_EXIT(portmod_port_pass_through_get(unit, link_a, &is_loopback, &is_enabled));
        if (is_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "link %d retimer connect: link already connected ", link_a);
        }

        SHR_IF_ERR_EXIT(portmod_port_pass_through_get(unit, link_b, &is_loopback, &is_enabled));
        if (is_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "link %d retimer connect: link already connected ", link_b);
        }

        
        SHR_IF_ERR_EXIT(portmod_port_pass_through_fec_enable_get(unit, link_a, &link_a_fec_enabled));
        SHR_IF_ERR_EXIT(portmod_port_pass_through_fec_enable_get(unit, link_b, &link_b_fec_enabled));
        if (link_a_fec_enabled != link_b_fec_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "retimer FEC enable is %d for link %d, but %d for link %d. it should be the same for both links ",
                    link_a_fec_enabled, link_a, link_b_fec_enabled, link_b);
        }
        if (link_a_fec_enabled && link_b_fec_enabled)
        {
            SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, link_a, &link_a_speed_config));
            SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, link_b, &link_b_speed_config));
            if (link_a_speed_config.fec != link_b_speed_config.fec)
            {
                SHR_ERR_EXIT(_SHR_E_CONFIG, "FEC type is %d for link %d, but %d for link %d. it should be the same for both links ",
                    link_a_speed_config.fec, link_a, link_b_speed_config.fec, link_b);
            }
        }
    }
    
    else
    {
        SHR_IF_ERR_EXIT(portmod_port_pass_through_get(unit, link_a, &is_loopback, &is_enabled));
        if (!is_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "link %d retimer disconnect: retimer is not connected ", link_a);
        }
    }

    
    
    
    
    if (link_b != -1)
    {
        connected_link = link_b;
    }
    
    else
    {
        SHR_IF_ERR_EXIT(soc_ramon_fabric_link_retimer_connect_get(unit, link_a, &connected_link));
    }

    
    
    
    PORTMOD_PORT_ENABLE_MAC_SET(flags);
    SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, link_a, flags, &link_a_enabled));
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, link_a, flags, 0));
    if (connected_link != link_a)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_get(unit, connected_link, flags, &connected_link_enabled));
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, connected_link, flags, 0));
    }

    
    
    
    if (link_b != -1)
    {
        
        if (link_a == link_b)
        {
            loopback_en = TRUE;
            tx_swap_required = FALSE;
        }
        
        else if (SOC_RAMON_FABRIC_LINKS_FMAC_INDEX_GET(unit, link_a) == SOC_RAMON_FABRIC_LINKS_FMAC_INDEX_GET(unit, link_b))
        {
            loopback_en = TRUE;
            tx_swap_required = TRUE;
        }
        
        else if (link_b == ( dnxf_data_port.retimer.links_connection_get(unit, link_a))->connected_link )
        {
            loopback_en = FALSE;
            tx_swap_required = FALSE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "link %d can't be connected to link %d.", link_a, link_b);
        }

        enable = TRUE;

        
        
        

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, link_a, enable)));

        if (connected_link != link_a)
        {
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, connected_link, enable)));
        }

        sal_usleep(100);

        
        if (tx_swap_required)
        {
            SHR_IF_ERR_EXIT(soc_ramon_fabric_links_tx_swap_set(unit, link_a, connected_link, enable));
        }

        
        SHR_IF_ERR_EXIT(portmod_port_pass_through_set(unit, link_a, loopback_en, enable));

        if (connected_link != link_a)
        {
            SHR_IF_ERR_EXIT(portmod_port_pass_through_set(unit, connected_link, loopback_en, enable));
        }
    }
    
    
    
    else
    {
        
        if (link_a == connected_link)
        {
            tx_swap_required = FALSE;
        }
        
        else if (SOC_RAMON_FABRIC_LINKS_FMAC_INDEX_GET(unit, link_a) == SOC_RAMON_FABRIC_LINKS_FMAC_INDEX_GET(unit, connected_link))
        {
            tx_swap_required = TRUE;
        }
        
        else if (connected_link == ( dnxf_data_port.retimer.links_connection_get(unit, link_a))->connected_link )
        {
            tx_swap_required = FALSE;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "link %d can't be disconnected from link %d.", link_a, connected_link);
        }

        loopback_en = FALSE;
        enable = FALSE;

        
        
        

        
        SHR_IF_ERR_EXIT(portmod_port_pass_through_set(unit, link_a, loopback_en, enable));

        if (connected_link != link_a)
        {
            SHR_IF_ERR_EXIT(portmod_port_pass_through_set(unit, connected_link, loopback_en, enable));
        }

        
        if (tx_swap_required)
        {
            SHR_IF_ERR_EXIT(soc_ramon_fabric_links_tx_swap_set(unit, link_a, connected_link, enable));
        }

        
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, link_a, enable)));

        if (connected_link != link_a)
        {
            SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit,mbcm_dnxf_fabric_links_link_integrity_down_set,(unit, connected_link, enable)));
        }
    }

    
    
    
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, link_a, flags, 1));
    if (connected_link != link_a)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, connected_link, flags, 1));
    }

    
    
    
    
    if (link_b != -1)
    {
        SHR_IF_ERR_EXIT(soc_ramon_fabric_link_retimer_calibrate(unit, link_a, link_b));
    }

    
    
    
    SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, link_a, flags, link_a_enabled));
    if (connected_link != link_a)
    {
        SHR_IF_ERR_EXIT(portmod_port_enable_set(unit, connected_link, flags, connected_link_enabled));
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_link_retimer_connect_get(int unit, soc_port_t link_a, soc_port_t *link_b)
{
    int is_enabled = -1;
    int is_loopback = -1;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_port_pass_through_get(unit, link_a, &is_loopback, &is_enabled));

    
    if (!is_enabled)
    {
        *link_b = -1;
    }
    
    else if (!is_loopback)
    {
        *link_b = (dnxf_data_port.retimer.links_connection_get(unit, link_a))->connected_link;
    }
    
    else
    {
        SHR_IF_ERR_EXIT(soc_ramon_fabric_links_retimer_loopback_peer_get(unit, link_a, link_b));
        if (*link_b < 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "no peer found for link %d.", link_a);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
