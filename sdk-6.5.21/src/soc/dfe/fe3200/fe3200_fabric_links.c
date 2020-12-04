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
#include <soc/dfe/fe3200/fe3200_port.h>


#define SOC_FE3200_FABRIC_LINKS_VSC128_MAX_CELL_SIZE        (128  + 8  + 1 )
#define SOC_FE3200_FABRIC_LINKS_VSC256_MAX_CELL_SIZE        (256  + 16  + 1)
#define SOC_FE3200_FABRIC_LINKS_ALDWP_MIN                   (0x2)


soc_error_t
soc_fe3200_fabric_links_llf_control_source_set(int unit, soc_port_t link, soc_dcmn_fabric_pipe_t val) {
   soc_port_t inner_lnk;
   uint32 blk_id;
   uint64 reg_val;
   uint32 field_val_rx[1], field_val_tx[1];
   SOCDNX_INIT_FUNC_DEFS;

   blk_id = INT_DEVIDE(link, 4);
   inner_lnk = link % 4;

   
   SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, blk_id, 0, &reg_val));
   *field_val_rx = soc_reg64_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
   *field_val_tx = soc_reg64_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
   if (SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_CLEAR(val)) {
      SHR_BITCLR(field_val_rx, inner_lnk);
      SHR_BITCLR(field_val_tx, inner_lnk);
   } else {
      SHR_BITSET(field_val_rx, inner_lnk);
      SHR_BITSET(field_val_tx, inner_lnk);
   }
   soc_reg64_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_RX_ENf, *field_val_rx);
   soc_reg64_field_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, &reg_val, LNK_LVL_FC_TX_ENf, *field_val_tx);
   SOCDNX_IF_ERR_EXIT(soc_reg_set(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, blk_id, 0, reg_val));

exit:
   SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_llf_control_source_get(int unit, soc_port_t link, soc_dcmn_fabric_pipe_t *val) {
   soc_port_t inner_lnk;
   int nof_pipes;
   uint32 blk_id;
   uint64 reg_val;
   uint32 field_val_rx[1], field_val_tx[1];
   SOCDNX_INIT_FUNC_DEFS;

   blk_id = INT_DEVIDE(link, 4);
   inner_lnk = link % 4;
   nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
   
   SOCDNX_IF_ERR_EXIT(soc_reg_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, blk_id, 0, &reg_val));
   *field_val_rx = soc_reg64_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_RX_ENf);
   *field_val_tx = soc_reg64_field_get(unit, FMAC_LINK_LEVEL_FLOW_CONTROL_ENABLE_REGISTERr, reg_val, LNK_LVL_FC_TX_ENf);
   if (!SHR_BITGET(field_val_rx, inner_lnk) && !SHR_BITGET(field_val_tx, inner_lnk)) {
       SOC_DCMN_FABRIC_PIPE_INIT(*val);
   } else {
       SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(val,nof_pipes);       
   }

exit:
   SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_isolate_set(int unit, soc_port_t link, soc_dcmn_isolation_status_t val) {
    soc_reg_above_64_val_t reachability_allowed_bm;
    soc_reg_above_64_val_t multicast_allowed_bm;
    soc_reg_above_64_val_t specific_link_bm;
    uint32 reg32_val;
    uint64 reg64_val, reg64_ones;
    SOCDNX_INIT_FUNC_DEFS;

    if (soc_dcmn_isolation_status_active == val) {

        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);
        SOC_REG_ABOVE_64_OR(reachability_allowed_bm, specific_link_bm);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

        SOCDNX_IF_ERR_EXIT(READ_RTP_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);
        SOC_REG_ABOVE_64_OR(multicast_allowed_bm, specific_link_bm);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));

    } else {

        {
            SOCDNX_IF_ERR_EXIT(READ_RTP_RESERVED_10r(unit, &reg32_val));
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, 0x0));
        }

        SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);
        SOC_REG_ABOVE_64_NOT(specific_link_bm);
        SOC_REG_ABOVE_64_AND(reachability_allowed_bm, specific_link_bm);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

        sal_usleep(10000);

        {
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RESERVED_10r(unit, reg32_val));
        }

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, &reg64_val));
        COMPILER_64_ALLONES(reg64_ones);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_ones));

        SOCDNX_IF_ERR_EXIT(READ_RTP_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));
        SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);
        SOC_REG_ABOVE_64_NOT(specific_link_bm);
        SOC_REG_ABOVE_64_AND(multicast_allowed_bm, specific_link_bm);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_MULICAST_ALLOWED_LINKS_REGISTERr(unit, multicast_allowed_bm));

        sal_usleep(1000);

        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_val));
    }

    exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_bmp_isolate_set(int unit, soc_pbmp_t bitmap, soc_dcmn_isolation_status_t val) {
   soc_reg_above_64_val_t reachability_allowed_bm;
   soc_reg_above_64_val_t specific_link_bm;
   soc_port_t link;
   SOCDNX_INIT_FUNC_DEFS;

   SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

   SOC_PBMP_ITER(bitmap, link) {
      
      SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);

      if (soc_dcmn_isolation_status_active == val) {
         
         SOC_REG_ABOVE_64_OR(reachability_allowed_bm, specific_link_bm);
      } else {
         
         SOC_REG_ABOVE_64_NOT(specific_link_bm);
         SOC_REG_ABOVE_64_AND(reachability_allowed_bm, specific_link_bm);
      }
   }

    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

   sal_usleep(20000);

exit:
   SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_isolate_get(int unit, soc_port_t link, soc_dcmn_isolation_status_t *val) {
   soc_reg_above_64_val_t reachability_allowed_bm;
   soc_reg_above_64_val_t specific_link_bm;
   SOCDNX_INIT_FUNC_DEFS;

   SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reachability_allowed_bm));

   SOC_REG_ABOVE_64_CREATE_MASK(specific_link_bm, 1, link);

   SOC_REG_ABOVE_64_AND(specific_link_bm, reachability_allowed_bm);
   if (!SOC_REG_ABOVE_64_IS_ZERO(specific_link_bm)) *val = soc_dcmn_isolation_status_active;
   else *val = soc_dcmn_isolation_status_isolated;

exit:
   SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_fabric_link_device_mode_get(int unit,soc_port_t port, int is_rx, soc_dfe_fabric_link_device_mode_t *link_device_mode)
{
    int nof_links, asymmetrical_quad = 0;
    SOCDNX_INIT_FUNC_DEFS;

    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    switch (SOC_DFE_CONFIG(unit).fabric_device_mode)
    {
       case soc_dfe_fabric_device_mode_multi_stage_fe2:
       case soc_dfe_fabric_device_mode_single_stage_fe2:
           *link_device_mode=soc_dfe_fabric_link_device_mode_fe2;
           break;

       case soc_dfe_fabric_device_mode_repeater:
           *link_device_mode=soc_dfe_fabric_link_device_mode_repeater;
           break;

       case soc_dfe_fabric_device_mode_multi_stage_fe13:
           if (is_rx)
           {
               *link_device_mode = (port < nof_links / 2) ? soc_dfe_fabric_link_device_mode_multi_stage_fe1 : soc_dfe_fabric_link_device_mode_multi_stage_fe3;
           } else {
               *link_device_mode = (port < nof_links / 2) ? soc_dfe_fabric_link_device_mode_multi_stage_fe3 : soc_dfe_fabric_link_device_mode_multi_stage_fe1;
           }
           break;

       case soc_dfe_fabric_device_mode_multi_stage_fe13_asymmetric:
           MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_asymmetrical_quad_get, (unit, port, &asymmetrical_quad));
           if ((asymmetrical_quad == 0 || asymmetrical_quad == 1)) 
           {
               if (is_rx)
               {
                   *link_device_mode = soc_dfe_fabric_link_device_mode_multi_stage_fe1;
               }
               else
               {
                   *link_device_mode = soc_dfe_fabric_link_device_mode_multi_stage_fe3;
               }
           }
           else 
           {
               if (is_rx)
               {
                   *link_device_mode = (port < nof_links / 2) ? soc_dfe_fabric_link_device_mode_multi_stage_fe1 : soc_dfe_fabric_link_device_mode_multi_stage_fe3;
               } 
               else 
               {
                   *link_device_mode = (port < nof_links / 2) ? soc_dfe_fabric_link_device_mode_multi_stage_fe3 : soc_dfe_fabric_link_device_mode_multi_stage_fe1;
               }
           }
           break;

       default:
           SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("device is not in a valid mode")));
           break;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe3200_fabric_links_aldwp_config(int unit, soc_port_t port)
{
    int rv;
    uint32 max_cell_size, aldwp, highest_aldwp, max_aldwp;
    int speed;
    int dch_instance, dch_inner_link;
    int offset=0;
    soc_dfe_fabric_link_device_mode_t device_mode;
    soc_dfe_fabric_link_cell_size_t cell_type;
    soc_port_t port_index;
    soc_dcmn_port_pcs_t encoding = 0;
    int enable = 0;
    soc_pbmp_t pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    

    

    
    highest_aldwp = 0;
    

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_link_to_block_mapping, (unit, port, &dch_instance, &dch_inner_link, SOC_BLK_DCH));
    SOCDNX_IF_ERR_EXIT(rv);

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit,port, 1, &device_mode)));

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_block_pbmp_get, (unit, SOC_BLK_DCH, dch_instance, &pbmp));
    SOCDNX_IF_ERR_EXIT(rv);
    SOC_PBMP_ITER(pbmp, port_index)
    {
        
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_phy_enable_get, (unit, port_index,&enable)); 
        SOCDNX_IF_ERR_EXIT(rv);
        if (!enable)
        {
            
            continue;
        }

        
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_speed_get, (unit, port_index, &speed));
        SOCDNX_IF_ERR_EXIT(rv);
        if (speed == 0)
        {
            
            continue;
        }

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_pcs_get, (unit, port_index, &encoding));
        if (encoding == soc_dcmn_port_pcs_8_10)
        {
            speed = (speed * FE3200_PORT_PCS_8_10_EFFECTIVE_RATE_PERCENT) / 100;
        }

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_cell_format_get, (unit, port_index, &cell_type));
        SOCDNX_IF_ERR_EXIT(rv);
        switch (cell_type)
        {
            case soc_dfe_fabric_link_cell_size_VSC128:
                if (SOC_DFE_CONFIG(unit).fabric_merge_cells)
                {
                    max_cell_size = 2 * SOC_FE3200_FABRIC_LINKS_VSC128_MAX_CELL_SIZE; 
                } else {
                    max_cell_size = SOC_FE3200_FABRIC_LINKS_VSC128_MAX_CELL_SIZE;
                }
                break;
            case soc_dfe_fabric_link_cell_size_VSC256_V1:
            case soc_dfe_fabric_link_cell_size_VSC256_V2: 
                max_cell_size = SOC_FE3200_FABRIC_LINKS_VSC256_MAX_CELL_SIZE;
                break;
            
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unknown cell format: %u"), cell_type));
        }

        
        aldwp = (3* max_cell_size * SOC_DFE_CONFIG(unit).core_clock_speed) / (speed / 8 ); 
        aldwp = aldwp /(1024*64)  + ((aldwp % (1024*64) != 0) ? 1 : 0) ;

        
        highest_aldwp = (aldwp > highest_aldwp ? aldwp : highest_aldwp); 
        
    }

    
    if (highest_aldwp != 0 )
    {
        if (device_mode == soc_dfe_fabric_link_device_mode_fe2)
        {
            offset=1;
        }
        else if (device_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe3)
        {
            offset=2;
        }
         highest_aldwp += offset;
    }


    max_aldwp = SOC_DFE_DEFS_GET(unit,aldwp_max_val);
    
    if (device_mode == soc_dfe_fabric_link_device_mode_repeater)
    {
        highest_aldwp=0;
    }
    else if (highest_aldwp == 0) 
    {
        
        SOC_EXIT;
    } else if (highest_aldwp < SOC_FE3200_FABRIC_LINKS_ALDWP_MIN)
    {
        highest_aldwp = SOC_FE3200_FABRIC_LINKS_ALDWP_MIN;
    } else if (highest_aldwp > max_aldwp)
    {
        highest_aldwp = max_aldwp;
    }

    
    {
        uint64 reg64_val;

        SOCDNX_IF_ERR_EXIT(READ_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_instance, &reg64_val));
        soc_reg64_field32_set(unit, DCH_DCH_ENABLERS_REGISTER_1r, &reg64_val, FIELD_21_26f, highest_aldwp);
        SOCDNX_IF_ERR_EXIT(WRITE_DCH_DCH_ENABLERS_REGISTER_1r(unit, dch_instance, reg64_val));
    }

exit:
   SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_fe3200_fabric_links_aldwp_init(int unit)
{
    int rv;
    int dch_instance,
        nof_dch,
        nof_links_in_dcq;
    SOCDNX_INIT_FUNC_DEFS;

    nof_dch = SOC_DFE_DEFS_GET(unit, nof_instances_dch);
    nof_links_in_dcq = SOC_DFE_DEFS_GET(unit, nof_links_in_dcq);

    for (dch_instance = 0; dch_instance < nof_dch; dch_instance++)
    {
        rv = soc_fe3200_fabric_links_aldwp_config(unit, dch_instance *  nof_links_in_dcq);
        SOCDNX_IF_ERR_EXIT(rv);
    }
 
exit:
   SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_fabric_links_validate_link(int unit, soc_port_t link) {
   soc_info_t          *si;
   soc_pbmp_t pbmp_valid;
   int nof_links;
   SOCDNX_INIT_FUNC_DEFS;
   

   
   si  = &SOC_INFO(unit);
   SOC_PBMP_ASSIGN(pbmp_valid, si->sfi.bitmap);

   nof_links = SOC_DFE_DEFS_GET(unit, nof_links);

   if (link < nof_links  && link >= 0 && SOC_PBMP_MEMBER(pbmp_valid, link)) {
      SOC_EXIT;
   } else {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("link %d is out of range"),link));
   }

exit:
   SOCDNX_FUNC_RETURN;
}

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
