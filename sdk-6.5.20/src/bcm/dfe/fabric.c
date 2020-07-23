/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * DFE FABRIC
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_FABRIC
#include <shared/bsl.h>
#include <bcm/types.h>
#include <bcm/module.h>
#include <bcm/error.h>
#include <bcm_int/common/debug.h>
#include <bcm_int/dfe_dispatch.h>
#include <bcm/debug.h>
#include <bcm/fabric.h>

#include <bcm_int/dfe/fabric.h>
#include <bcm_int/dfe/dfe_modid_group_map.h>
#include <bcm_int/dfe/dfe_modid_local_map.h>
#include <bcm_int/dfe/dfe_multicast_id_map.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <soc/dfe/cmn/dfe_fabric.h>
#include <soc/dfe/cmn/dfe_multicast.h>
#include <soc/dfe/cmn/dfe_port.h>
#include <soc/dfe/cmn/dfe_warm_boot.h>
#include <soc/dfe/cmn/dfe_fabric_source_routed_cell.h>
#include <soc/dfe/cmn/dfe_fabric_cell.h>
#include <soc/dfe/cmn/mbcm.h>

#include <soc/dcmn/dcmn_cells_buffer.h>
#include <soc/dcmn/dcmn_captured_buffer.h>

/**********************************************************/
/*                  Verifications                         */
/**********************************************************/\


#define DFE_FABRIC_CONTROL_SOURCE_VALID_CHECK(arg) \
     switch(arg)\
     {\
        case soc_dcmn_fabric_control_source_none:\
        case soc_dcmn_fabric_control_source_primary:\
        case soc_dcmn_fabric_control_source_secondary:\
        case soc_dcmn_fabric_control_source_both:\
            break;\
        default:\
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("arg %d is not a valid control source"), arg)); \
    }


#define DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(arg) \
     switch(arg) \
     { \
        case soc_dfe_fabric_link_fifo_type_index_0: \
        case soc_dfe_fabric_link_fifo_type_index_1: \
          break; \
        default:  \
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fifo type %d is not valid"), arg)); \
     } 

#define DFE_FABRIC_PIPE_TYPE_VALID_CHECK(arg) \
     if (SOC_DCMN_FABRIC_PIPE_IS_INIT(arg) ) \
     { \
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("arg %d is not a valid pipe type control ") , arg)); \
     } 
     

/**********************************************************/
/*                  Internal functions                    */
/**********************************************************/  

STATIC int
_bcm_dfe_fabric_link_control_weight_get(int unit,bcm_port_t link, int is_prim,int *arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    if(SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes < 2)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Weight isn't relevant for single pipe device")));
    }
    
    /*get registers*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_weight_get,(unit, link, is_prim, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN; 
  
}

STATIC int 
_bcm_dfe_fabric_link_control_set_cell_format(int unit, bcm_port_t link, int arg)
{
    int rc;
    int is_enabled;
    BCMDNX_INIT_FUNC_DEFS;
    

    /*Verification*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_format_verify,(unit, link, (soc_dfe_fabric_link_cell_size_t)arg));
    BCMDNX_IF_ERR_EXIT(rc);

    /*Disable port*/
    rc = bcm_dfe_port_enable_get(unit, link, &is_enabled);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_enabled) {
        rc = bcm_dfe_port_enable_set(unit, link, 0);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /*Set value*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_format_set,(unit, link, (soc_dfe_fabric_link_cell_size_t)arg));
    BCMDNX_IF_ERR_EXIT(rc);

    /*Enable port*/
    if (is_enabled) {
        rc = bcm_dfe_port_enable_set(unit, link, is_enabled);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /*ALDWP should be configured at init time for every speed change*/
    /*Avoiding ALDWP config at init time*/
    rc = bcm_dfe_init_check(unit);
    if (rc == BCM_E_UNIT /*init*/)
    {
        /*Do nothing*/
    } else if (rc == BCM_E_NONE)
    {
        rc = MBCM_DFE_DRIVER_CALL(unit ,mbcm_dfe_fabric_links_aldwp_config ,(unit, link));      
        BCMDNX_IF_ERR_EXIT(rc);
    } else {
        BCMDNX_IF_ERR_EXIT(rc);
    }
     
exit:   
    BCMDNX_FUNC_RETURN; 
}

STATIC int 
_bcm_dfe_fabric_links_cell_interleaving_set(int unit, bcm_port_t link, int arg)
{
    int rc;
    int is_enabled;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dfe_port_enable_get(unit, link, &is_enabled);
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_enabled) {
        rc = bcm_dfe_port_enable_set(unit, link, 0);
        BCMDNX_IF_ERR_EXIT(rc);
    }

    rc =  MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_interleaving_set,(unit, link, arg ? 1 : 0));
    BCMDNX_IF_ERR_EXIT(rc);

    if (is_enabled) {
        rc = bcm_dfe_port_enable_set(unit, link, is_enabled);
        BCMDNX_IF_ERR_EXIT(rc);
    }
     
exit:   
    BCMDNX_FUNC_RETURN; 
}

STATIC int
_bcm_dfe_fabric_link_control_set_weight(int unit,bcm_port_t link, int is_prim,int arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    if(SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes< 2)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Weight isn't relevant for single pipe device")));
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_weight_validate,(unit, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
    /*set registers*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_weight_set,(unit, link, is_prim, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN; 
  
}

STATIC int
_bcm_dfe_fabric_link_control_set_llfc_control_source(int unit, bcm_port_t link, soc_dcmn_fabric_pipe_t arg)
{
    int rc, nof_pipes;
    uint32 mask=0;
    BCMDNX_INIT_FUNC_DEFS;
    
    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&mask,nof_pipes);
    
    /*validation*/
    if ( (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_CLEAR(arg))) &&  (!(SOC_DCMN_FABRIC_PIPE_ALL_PIPES_IS_SET(arg,mask))) ) /* if enable, then all bits must be on */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("LLFC must be for all pipes")));
    }
    
    /*set registers*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_llf_control_source_set,(unit, link, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_fabric_link_control_set_repeater_destination_per_link(int unit,bcm_port_t link, int arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    DFE_LINK_INPUT_CHECK(unit, link);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_repeater_destination_set,(unit, link, arg));
    BCMDNX_IF_ERR_EXIT(rc);

exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_fabric_link_control_validate(int unit, _bcm_dfe_fifo_type_handle_t fifo_type, bcm_fabric_link_threshold_type_t type)
{
    int rc, is_rx, is_tx;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dfe_fifo_type_is_rx(unit, fifo_type, &is_rx);
    BCMDNX_IF_ERR_EXIT(rc);
    rc = bcm_dfe_fifo_type_is_tx(unit, fifo_type, &is_tx);
    BCMDNX_IF_ERR_EXIT(rc);

    if(!is_rx)
    {
        switch(type)
        {
            case bcmFabricLinkRxFifoLLFC:
            case bcmFabricLinkRxGciLvl1FC:
            case bcmFabricLinkRxGciLvl2FC:
            case bcmFabricLinkRxGciLvl3FC:
            case bcmFabricLinkRxPrio0Drop:
            case bcmFabricLinkRxPrio1Drop:
            case bcmFabricLinkRxPrio2Drop:
            case bcmFabricLinkRxPrio3Drop:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fifo_type can't configure RX threshold")));  
            default:
              break;
        }
    }
    
    if(!is_tx)
    {
        switch(type)
        {
            case bcmFabricLinkRciFC:
            case bcmFabricLinkFE1TxBypassLLFC:
            case bcmFabricLinkTxGciLvl1FC:
            case bcmFabricLinkTxGciLvl2FC:
            case bcmFabricLinkTxGciLvl3FC:
            case bcmFabricLinkTxPrio0Drop:
            case bcmFabricLinkTxPrio1Drop:
            case bcmFabricLinkTxPrio2Drop:
            case bcmFabricLinkTxPrio3Drop:
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("fifo_type can't configure TX threshold")));  
            default:
              break;
        }
    }
    
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dfe_fabric_control_set_rci_gci_control_source(int unit, bcm_fabric_control_t type, soc_dcmn_fabric_pipe_t arg)
{
    int rc,nof_pipes;
    uint32 mask=0,negative_mask=0;
    BCMDNX_INIT_FUNC_DEFS;
    

    /*validation*/
    nof_pipes=SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&mask,nof_pipes);
    
    if(SOC_DCMN_FABRIC_PIPE_IS_INIT(arg))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Can't disable RCI/GCI")));  
    }

    negative_mask=~mask;

    if (SOC_DCMN_FABRIC_PIPE_ALL_PIPES_GET(arg,negative_mask)) /* if some bit is on , and that bit is > number of pipes */
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("GCI/RCI has invalid configuration- more than configured number of pipes ")));
    }
    
    /*set registers*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rci_gci_control_source_set,(unit, type, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN;
  
}

STATIC int 
_bcm_dfe_fabric_multicast_low_prio_drop_select(int unit, int arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Verification*/
    if(!DFE_IS_PRIORITY_VALID(arg)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal priority %d"), arg));
    }
    
    /*Set register*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_set,(unit, (soc_dfe_fabric_priority_t)arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_fabric_control_set_low_prio_threshold(int unit, bcm_fabric_control_t type, int arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validate threshold*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_threshold_validate,(unit, type, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_threshold_set,(unit, type, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int 
_bcm_dfe_fabric_control_set_priority_range(int unit, bcm_fabric_control_t type, int arg)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_priority_range_validate,(unit, type, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_priority_range_set,(unit, type, arg));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:   
    BCMDNX_FUNC_RETURN;
}


STATIC int
_bcm_dfe_fabric_link_repeater_enable_set(int unit, bcm_port_t port, int enable)
{
    int rv;
    uint32 burst, pps;
    int speed, empty_cell_size;
    uint64 empty_cell_size64, pps64;
    BCMDNX_INIT_FUNC_DEFS;

    if (enable)
    {
        /* 
         * Repeater link enable -
         *  if cell shaper is enabled set empty cell size accordingly.                      -
         *  Else set to repeater link default cell size.                                                                                -
         */
        rv = bcm_dfe_port_rate_egress_pps_get(unit, port, &pps, &burst);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = bcm_dfe_port_speed_get(unit, port, &speed);
        BCMDNX_IF_ERR_EXIT(rv);

        if (pps == 0)
        {
            rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_repeater_enable_set,(unit, port, 1, SOC_DFE_DEFS_GET(unit, repeater_default_empty_cell_size)));
            BCMDNX_IF_ERR_EXIT(rv); 
        } else {
            /*empty_cell_size [bytes/cell] = speed [bytes/sec] / pps [cells/sec]*/
            COMPILER_64_SET(empty_cell_size64, 0, speed);
            COMPILER_64_UMUL_32(empty_cell_size64, (1000000 / 8)); /*Mega-bits to bytes*/

            COMPILER_64_SET(pps64, 0, pps);
            COMPILER_64_UDIV_64(empty_cell_size64, pps64); 

            empty_cell_size = COMPILER_64_LO(empty_cell_size64);

            rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_repeater_enable_set,(unit, port, 1, empty_cell_size));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    } else {
        /*Disable repeater link and reset empty cell size*/
        rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_repeater_enable_set,(unit, port, 0, SOC_DFE_DEFS_GET(unit, repeater_none_empty_cell_size)));
        BCMDNX_IF_ERR_EXIT(rv);           
    }
   
exit:   
    BCMDNX_FUNC_RETURN;
}

STATIC int
_bcm_dfe_fabric_link_repeater_enable_get(int unit, bcm_port_t port, int *enable)
{
    int rv;
    int empty_cell_size;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_repeater_enable_get,(unit, port, enable, &empty_cell_size));
    BCMDNX_IF_ERR_EXIT(rv); 

exit:   
    BCMDNX_FUNC_RETURN;
}

/* 
 * Repeater link configures depands on the relevant link rate and in the cell shaper.
 * This function should be called whenever the link speed or the link cell shaper changed
 */
int
_bcm_dfe_fabric_link_repeater_update(int unit, bcm_port_t port)
{
    int rc;
    int enable;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_fabric_link_control_get(unit, port, bcmFabricLinkRepeaterEnable, &enable);
    BCMDNX_IF_ERR_EXIT(rc);

    if (enable)
    {
        /*re-eable repeater link will adjust empty cell size*/
        rc = bcm_dfe_fabric_link_control_set(unit, port, bcmFabricLinkRepeaterEnable, enable);
        BCMDNX_IF_ERR_EXIT(rc);
    }

exit:   
    BCMDNX_FUNC_RETURN;
}
/**********************************************************/
/*                  Implementation                        */
/**********************************************************/  

/*
 * Function:
 *      bcm_fabric_init
 * Purpose:
 *      Initialize Fabric module
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
bcm_dfe_fabric_init(
    int unit)
{
    int i, rc, repeater_dest, isolate_type;
    pbmp_t valid_link_bitmap;
    int repeater_link_enable;
    bcm_port_t link;
    char *vsc_str;
    int vsc;
    _bcm_dfe_fifo_type_handle_t fifo_type_handle;
    int channel;
    soc_dfe_property_info_t soc_property_info;
    int is_soc_property_supported;
    uint32 mc_table_size;

    BCMDNX_INIT_FUNC_DEFS;

    DFE_UNIT_LOCK_TAKE(unit);

    /*Init FIFO types - just for FE1600 - FE3200 do not use FIFO handlers*/
    if (!SOC_WARM_BOOT(unit) && SOC_IS_FE1600(unit)) 
    {
        for(i=0 ; i<soc_dfe_fabric_nof_link_fifo_types ; i++) 
        {
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, FIFO_HANDLERS, i, &fifo_type_handle);
            BCMDNX_IF_ERR_EXIT(rc);
            rc = bcm_dfe_fifo_type_clear(unit, &fifo_type_handle);
            BCMDNX_IF_ERR_EXIT(rc);
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_SET(unit, FIFO_HANDLERS, i, &fifo_type_handle);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }

    /*Set speed max for all ports*/
    SOC_PBMP_ASSIGN(valid_link_bitmap, SOC_INFO(unit).sfi.bitmap);
    SOC_PBMP_ITER(valid_link_bitmap, i)
    {
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_port_speed_max,(unit, i, &(SOC_INFO(unit).port_speed_max[i]))); 
        BCMDNX_IF_ERR_EXIT(rc);
    }

    /*Clear topology maps*/
    if (!SOC_WARM_BOOT(unit)) {
        rc = bcm_dfe_modid_group_map_clear(unit);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_modid_local_map_clear(unit);
        BCMDNX_IF_ERR_EXIT(rc);
    }    

    /*Create buffer for saved cells*/
    rc = dcmn_cells_buffer_create(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer, 30);
    BCMDNX_IF_ERR_EXIT(rc);

    rc = dcmn_captured_buffer_create(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer, 50);
    BCMDNX_IF_ERR_EXIT(rc);

    /*Initialize FIFO DMA*/
    rc = soc_dfe_fifo_dma_init(unit);
    BCMDNX_IF_ERR_EXIT(rc);
    
    if (SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable != SOC_DFE_PROPERTY_UNAVAIL && SOC_DFE_CONFIG(unit).fabric_cell_fifo_dma_enable)
    {
        for (channel = SOC_DFE_DEFS_GET(unit, fifo_dma_fabric_cell_first_channel);
              channel < SOC_DFE_DEFS_GET(unit, fifo_dma_nof_fabric_cell_channels);
              channel++)
        {
            SOC_DFE_ALLOW_WARMBOOT_WRITE(soc_dfe_fifo_dma_channel_init(unit, channel),rc);
            BCMDNX_IF_ERR_EXIT(rc);
        }

    }

    
    /*Initialize inband routes*/
    if (!SOC_WARM_BOOT(unit)) {
        sal_memset(SOC_DFE_CONTROL(unit)->inband_routes, 0, sizeof(soc_fabric_inband_route_t)*SOC_DFE_NUM_OF_ROUTES);
    }

    /* 
     *Set repeater  releated configurations
     */
    if (!SOC_WARM_BOOT(unit)) {
        if(soc_dfe_fabric_device_mode_repeater == SOC_DFE_CONFIG(unit).fabric_device_mode) {
            SOC_PBMP_ITER(valid_link_bitmap, i)
            {
                repeater_dest = soc_property_port_get(unit, i, spn_REPEATER_LINK_DEST, -1);
                if(repeater_dest >= 0) {
                    rc = bcm_dfe_fabric_link_control_set(unit, i, bcmFabricLinkRepeaterDestinationLink, repeater_dest);
                    BCMDNX_IF_ERR_EXIT(rc);
                }
            }
        } else {
            /*links that connected to a repeater*/
            PBMP_SFI_ITER(unit, link)
            {
                repeater_link_enable = soc_dfe_property_port_get(unit, spn_REPEATER_LINK_ENABLE, link, 0, SOC_DFE_PROPERTY_UNAVAIL);                                 
                if (repeater_link_enable != SOC_DFE_PROPERTY_UNAVAIL /*unsupported or not configured*/ && repeater_link_enable)
                {
                    rc = bcm_dfe_fabric_link_control_set(unit, link, bcmFabricLinkRepeaterEnable, 1);
                    BCMDNX_IF_ERR_EXIT(rc);           
                }
            }
        }
    }

    /* 
     *Cell Format related configuration
     */
    if (!SOC_WARM_BOOT(unit))
    {
        PBMP_SFI_ITER(unit, link)
        {
            soc_dfe_check_soc_property(unit,spn_FABRIC_CELL_FORMAT ,&is_soc_property_supported,&soc_property_info);
            if (is_soc_property_supported)
            {
                vsc_str = soc_dfe_property_port_get_str(unit, spn_FABRIC_CELL_FORMAT, link, 0, NULL);
                rc = soc_dfe_property_str_to_enum(unit, spn_FABRIC_CELL_FORMAT, soc_dfe_property_str_enum_fabric_cell_format,  vsc_str,  &vsc);
                BCMDNX_IF_ERR_EXIT(rc);

                BCMDNX_IF_ERR_EXIT(bcm_dfe_fabric_link_control_set(unit, link, bcmFabricLinkCellFormat, vsc));
            }

            /*Enable PCP by default*/
            if (soc_feature(unit, soc_feature_fabric_cell_pcp))
            {
                BCMDNX_IF_ERR_EXIT(bcm_dfe_fabric_link_control_set(unit, link, bcmFabricLinkPcpEnable, 0x1));
            }
        }
    }

    /*Set FE2 filtered mode*/
    if (!SOC_WARM_BOOT(unit)) {
        if (SOC_DFE_IS_FE2(unit)) {
            BCMDNX_IF_ERR_EXIT(soc_dfe_control_cell_filter_clear(unit));
            BCMDNX_IF_ERR_EXIT(soc_dfe_control_cell_filter_set(unit, SOC_CELL_FILTER_FLAG_DONT_FORWARD | SOC_CELL_FILTER_FLAG_DONT_TRAP, soc_dcmn_fe2_filtered_cell,  0, NULL, NULL));
        }
    }


    /* Initalizing valid info for isolation status*/
    if (!SOC_WARM_BOOT(unit) || SOC_IS_RELOADING(unit))
    {
        isolate_type = soc_dfe_fabric_isolate_type_none;
        BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_TYPE, &isolate_type));
    }

    if (!SOC_WARM_BOOT(unit))
    {
        if (SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable == 1) /* set all valid multicast ID's to low priority */
        {
            BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_size_get, (unit, &mc_table_size)));
            BCMDNX_IF_ERR_EXIT(bcm_dfe_fabric_control_set(unit, bcmFabricMcLowPrioMin, 0));
            BCMDNX_IF_ERR_EXIT(bcm_dfe_fabric_control_set(unit, bcmFabricMcLowPrioMax, mc_table_size-1));
        }
    }
    

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *      bcm_fabric_deinit
 * Purpose:
 *      Internal function -  Deinitialize Fabric module
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_xxx
 */
int
_bcm_dfe_fabric_deinit(
    int unit)
{
    int rc;
    BCMDNX_INIT_FUNC_DEFS;

    rc = dcmn_cells_buffer_destroy(unit, &SOC_DFE_CONTROL(unit)->sr_cells_buffer);
    BCMDNX_IF_ERR_CONT(rc);

    rc = dcmn_captured_buffer_destroy(unit, &SOC_DFE_CONTROL(unit)->captured_cells_buffer);
    BCMDNX_IF_ERR_CONT(rc);

    rc = soc_dfe_fifo_dma_deinit(unit);
    BCMDNX_IF_ERR_CONT(rc);

    BCMDNX_FUNC_RETURN;
}
/*
 * Function:
 *      bcm_dfe_fabric_control_set
 * Purpose:
 *      set fabric control attributes.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) Fabric control parameter 
 *      arg  - (IN) Argument whose meaning is dependent on type 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_control_set(
    int unit, 
    bcm_fabric_control_t type, 
    int arg)
{
    int rc = BCM_E_NONE;
    int isolate_type;
    int enable;
    pbmp_t pbmp_unisolated_links, pbmp_active_links;
    soc_port_t port;
    soc_dcmn_isolation_status_t link_isolation_status, device_isolation_status;
    BCMDNX_INIT_FUNC_DEFS;
    
    DFE_UNIT_LOCK_TAKE(unit);

    switch(type)
    {
        case bcmFabricMcLowPrioMin:
        case bcmFabricMcLowPrioMax:
        case bcmFabricMcMidPrioMin:
        case bcmFabricMcMidPrioMax:
            if (!(SOC_DFE_CONFIG(unit).fe_mc_priority_map_enable == 1))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported controls when mc priority is determined by the cell"), type));  
            }
            rc = _bcm_dfe_fabric_control_set_priority_range(unit, type, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricEnableMcLowPrioDrop:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_priority_drop_enable_set,(unit, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMcLowPrioDropThUp:
        case bcmFabricMcLowPrioDropThDown:
            rc = _bcm_dfe_fabric_control_set_low_prio_threshold(unit, type, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMcLowPrioDropSelect:
            rc = _bcm_dfe_fabric_multicast_low_prio_drop_select(unit, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
       case bcmFabricIsolate:
           if (SOC_DFE_IS_FE2(unit))
           {
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_set,(unit, arg ? soc_dcmn_isolation_status_isolated : soc_dcmn_isolation_status_active)); 
                BCMDNX_IF_ERR_EXIT(rc);
           } else if (SOC_DFE_IS_FE13(unit)) {
               if (arg)
               {
                   BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));
                   if (isolate_type != soc_dfe_fabric_isolate_type_none)
                   {
                       BCM_EXIT;
                   }

                   /*save device status in order to save isolate info*/
                    SOC_PBMP_CLEAR(pbmp_unisolated_links);

                    if (!SOC_IS_RELOADING(unit))
                    {
                        /*Read isolate state for all links including shut down links*/
                        for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links); port++)
                        {
                            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_isolate_get, (unit, port, &link_isolation_status));
                            SOCDNX_IF_ERR_EXIT(rc);
                            if (link_isolation_status == soc_dcmn_isolation_status_active)
                            {
                                SOC_PBMP_PORT_ADD(pbmp_unisolated_links, port);
                            }
                        }
                    }
                    else
                    {
                        /*for easy-reload replay ==> unisolate all links*/
                        for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links); port++)
                        {
                            SOC_PBMP_PORT_ADD(pbmp_unisolated_links, port);
                        }
                    }

                    isolate_type = soc_dfe_fabric_isolate_type_isolate;

                    BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_UNISOLATED_LINKS, &pbmp_unisolated_links));
                    BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_TYPE, &isolate_type));
               } else {
                   /*read the device state prior to isolation*/
                   BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_UNISOLATED_LINKS, &pbmp_unisolated_links));
                   BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));

                   if (isolate_type != soc_dfe_fabric_isolate_type_isolate)
                   {
                       BCM_PBMP_CLEAR(pbmp_unisolated_links);
                   }
                   
                   /*delete isolate info*/
                   isolate_type = soc_dfe_fabric_isolate_type_none;
                   BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_TYPE, &isolate_type));
               }

               /*isolate/unisoalte FE13*/
               rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_drv_fe13_isolate_set ,(unit, pbmp_unisolated_links, arg)); 
               BCMDNX_IF_ERR_EXIT(rc);
           } else {
               BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported control %d.\n"), type));
           }
           break;
        case bcmFabricShutdown:
            if (arg) /* shutdown */
            {
                /*save device info - in order to recover after disabling shut down*/
                SOC_PBMP_CLEAR(pbmp_unisolated_links);
                SOC_PBMP_CLEAR(pbmp_active_links);
                SOC_PBMP_ITER(PBMP_SFI_ALL(unit), port)
                {
                    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_phy_enable_get, (unit, port, &enable));
                    SOCDNX_IF_ERR_EXIT(rc);
                    if (enable) {
                        SOC_PBMP_PORT_ADD(pbmp_active_links, port);
                    }
                }

                /*Read isolate state for all links including shut down links*/
                for (port = 0; port < SOC_DFE_DEFS_GET(unit, nof_links); port++)
                {
                    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_isolate_get, (unit, port, &link_isolation_status));
                    SOCDNX_IF_ERR_EXIT(rc);
                    if (link_isolation_status == soc_dcmn_isolation_status_active)
                    {
                        SOC_PBMP_PORT_ADD(pbmp_unisolated_links, port);
                    }
                }

                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_isolate_get, (unit, &device_isolation_status));
                SOCDNX_IF_ERR_EXIT(rc);

                isolate_type = soc_dfe_fabric_isolate_type_shutdown;

                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_UNISOLATED_LINKS, &pbmp_unisolated_links));
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_ACTIVE_LINKS, &pbmp_active_links));
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_ISOLATE_DEVICE, &device_isolation_status));
                
                /*Shut down device*/
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_graceful_shutdown_set, (unit, pbmp_active_links, 1, pbmp_unisolated_links, 0)); 
                BCMDNX_IF_ERR_EXIT(rc);            

            } else { /* bring up */
                /*recover info prior*/ 
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_UNISOLATED_LINKS, &pbmp_unisolated_links));
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_ACTIVE_LINKS, &pbmp_active_links));
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_ISOLATE_DEVICE, &device_isolation_status));
                BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));

                if (isolate_type == soc_dfe_fabric_isolate_type_shutdown)
                {
                                    /*Shut down device*/
                    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_graceful_shutdown_set, (unit, pbmp_active_links, 0, pbmp_unisolated_links, device_isolation_status == soc_dcmn_isolation_status_isolated ? 1:0)); 
                    BCMDNX_IF_ERR_EXIT(rc);            
                       
                } else {
                    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_drv_graceful_shutdown_set, (unit, PBMP_SFI_ALL(unit), 0, PBMP_SFI_ALL(unit), 0)); 
                    BCMDNX_IF_ERR_EXIT(rc);                      
                }
                isolate_type = soc_dfe_fabric_isolate_type_none;
                
            }
            BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_SET(unit, ISOLATE_TYPE, &isolate_type));
            break;
        case bcmFabricRCIControlSource:
        case bcmFabricGCIControlSource:
            rc = _bcm_dfe_fabric_control_set_rci_gci_control_source(unit, type, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));    
    }
  
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_control_get
 * Purpose:
 *      get fabric control attributes.
 * Parameters:
 *      unit - (IN)  Unit number.
 *      type - (IN)  Fabric control parameter
 *      arg -  (OUT) Value whose meaning is dependent on type
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_control_get(
    int unit, 
    bcm_fabric_control_t type, 
    int *arg)
{
    int rc = BCM_E_NONE;
    soc_dfe_fabric_isolate_type_t isolate_type;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(arg);
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    switch(type)
    {
        case bcmFabricMcLowPrioMin:
        case bcmFabricMcLowPrioMax:
        case bcmFabricMcMidPrioMin:
        case bcmFabricMcMidPrioMax:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_priority_range_get,(unit, type, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricEnableMcLowPrioDrop:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_priority_drop_enable_get,(unit, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMcLowPrioDropThUp:
        case bcmFabricMcLowPrioDropThDown:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_threshold_get,(unit, type, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricMcLowPrioDropSelect:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_low_prio_drop_select_priority_get,(unit, (soc_dfe_fabric_priority_t*)arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
       case bcmFabricIsolate:
           if (SOC_DFE_IS_FE13(unit))
           {
               BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));
               *arg = (isolate_type == soc_dfe_fabric_isolate_type_isolate ? 1 : 0);
           } else {
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_isolate_get,(unit, (soc_dcmn_isolation_status_t*)arg));
                BCMDNX_IF_ERR_EXIT(rc);
           }
            break;
        case bcmFabricRCIControlSource:
        case bcmFabricGCIControlSource:
            rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rci_gci_control_source_get,(unit, type,(soc_dcmn_fabric_pipe_t*) arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
       case bcmFabricShutdown:
           BCMDNX_IF_ERR_EXIT(SOC_DFE_WARM_BOOT_VAR_GET(unit, ISOLATE_TYPE, &isolate_type));
           *arg = (isolate_type == soc_dfe_fabric_isolate_type_shutdown ? 1 : 0);
           break;
        default:
            BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"), type));       
    }
    
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_modid_group_set
 * Purpose:
 *      Set of Modules belong to a group
 * Parameters:
 *      unit        - (IN) Unit number.
 *      group       - (IN) Group Number, should be created using the MACRO BCM_FABRIC_GROUP_MODID_SET
 *      modid_count - (IN) Size of modid_array 
 *      modid_array - (IN) list of Modules IDs
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_modid_group_set(
    int unit, 
    bcm_module_t group, 
    int modid_count, 
    bcm_module_t *modid_array)
{
    int i, rc, is_valid;
    bcm_module_t module, grp;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Validation*/
    BCMDNX_NULL_CHECK(modid_array);
    
    if(!BCM_FABRIC_MODID_IS_GROUP(group))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%d is not a group"), group)); 
    }
    
    grp = BCM_FABRIC_GROUP_MODID_UNSET(group);
    if(!DFE_MODID_GROUP_IS_LEGAL(unit, grp))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Group %d is out of range"), group)); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*validate each module*/
    for(i=0 ; i<modid_count ; i++)
    {
        if(BCM_FABRIC_MODID_IS_LOCAL(modid_array[i]))
        {
            module = BCM_FABRIC_LOCAL_MODID_UNSET(modid_array[i]);
            rc = bcm_dfe_modid_local_map_is_valid(unit, module, &is_valid);
            BCMDNX_IF_ERR_EXIT(rc);
            if(!is_valid)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module %d index %d is invalid local modid"), modid_array[i], i)); 
            }
        }
        else
        {
            if(modid_array[i] >= SOC_DFE_MODID_NOF) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module %d index %d is out of rang"), modid_array[i], i)); 
            }
        }

    }
      
    /*save group*/
    rc = bcm_dfe_modid_group_map_clear_group(unit,grp);
    BCMDNX_IF_ERR_EXIT(rc);
    for(i=0 ; i<modid_count ; i++)
    {
        if(BCM_FABRIC_MODID_IS_LOCAL(modid_array[i]))
        {
            rc = bcm_dfe_modid_local_map_get(unit, BCM_FABRIC_LOCAL_MODID_UNSET(modid_array[i]), &module);
            BCMDNX_IF_ERR_EXIT(rc);
        }
        else
        {
            module = modid_array[i];
        }
          
        rc = bcm_dfe_modid_group_map_add(unit,grp,module);    
        BCMDNX_IF_ERR_EXIT(rc);
    }

    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);    
    
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_modid_group_get
 * Purpose:
 *      Get of Modules belong to a group
 * Parameters:
 *      unit            - (IN) Unit number.
 *      group           - (IN) Group Number, should be created using the MACRO BCM_FABRIC_GROUP_MODID_SET
 *      modid_max_count - (IN) Max size of modid_array 
 *      modid_array    - (OUT) Size of modid_array 
 *      modid_count     - (OUT) List of modules IDs 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_modid_group_get(
    int unit, 
    bcm_module_t group, 
    int modid_max_count, 
    bcm_module_t *modid_array, 
    int *modid_count)
{
    bcm_module_t grp;
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
       
    /*Validation*/
    BCMDNX_NULL_CHECK(modid_array);
    BCMDNX_NULL_CHECK(modid_count);
    
    if(!BCM_FABRIC_MODID_IS_GROUP(group))
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%d is not a group"), group)); 
    }
    
    grp = BCM_FABRIC_GROUP_MODID_UNSET(group);
    if(!DFE_MODID_GROUP_IS_LEGAL(unit, grp))
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Group %d is out of range"), group)); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*Get Group*/
    rc = bcm_dfe_modid_group_map_get_list(unit, grp, modid_max_count, modid_array, modid_count);
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_modid_group_find
 * Purpose:
 *      Find the relevant group for a given module id 
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      modid       - (IN)  Module ID 
 *      group       - (OUT) Group ID
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_modid_group_find(
    int unit, 
    bcm_module_t modid, 
    bcm_module_t *group)
{
    int m, rc = BCM_E_NONE, is_valid;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(group);

    DFE_UNIT_LOCK_TAKE(unit);
    
    if(BCM_FABRIC_MODID_IS_LOCAL(modid)) 
    {
        rc = bcm_dfe_modid_local_map_is_valid(unit,BCM_FABRIC_LOCAL_MODID_UNSET(modid), &is_valid);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_valid) 
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("module %d is invalid local modid"), modid));
        } else {
            rc = bcm_dfe_modid_local_map_get(unit, BCM_FABRIC_LOCAL_MODID_UNSET(modid), &m);
            BCMDNX_IF_ERR_EXIT(rc);
        }  
    } else {
        m = modid;
    }

    
    rc = bcm_dfe_modid_group_map_get_group(unit, m, group);
    BCMDNX_IF_ERR_EXIT(rc);
   
    if(DFE_MODID_GROUP_INVALID == *group) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_NOT_FOUND, (_BSL_BCM_MSG("module %d not found"), modid));
    }

exit:
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_modid_local_mapping_set
 * Purpose:
 *      Mapping a Module ID to local space
 * Parameters:
 *      unit        - (IN) Unit number.
 *      local_modid - (IN) Local module Id, should be created using the MACRO BCM_FABRIC_LOCAL_MODID_SET
 *      modid       - (IN) Module Id to map
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_modid_local_mapping_set(
    int unit, 
    bcm_module_t local_modid, 
    bcm_module_t modid)
{
    bcm_module_t local;
    int rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Validation*/
    if(!BCM_FABRIC_MODID_IS_LOCAL(local_modid))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%d is not a local module id"), local_modid)); 
    }
    
    local = BCM_FABRIC_LOCAL_MODID_UNSET(local_modid);
    
    if(!DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local module id %d is out of range"), local_modid)); 
    }
    
    if(!SOC_CHECK_MODULE_ID(modid))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module id %d is out of range"), modid)); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*Will override previous mapping to same local module id if any*/
    rc = bcm_dfe_modid_local_map_add(unit, local, modid, 1);
    BCMDNX_IF_ERR_EXIT(rc);
    
exit:  
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_modid_local_mapping_get
 * Purpose:
 *      Get the mapping a Module ID to local space
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      local_modid - (IN)  Local module Id, should be created using the MACRO BCM_FABRIC_LOCAL_MODID_SET
 *      modid       - (OUT) Returned Module Id 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_modid_local_mapping_get(
    int unit, 
    bcm_module_t local_modid, 
    bcm_module_t *modid)
{
    int rc, is_valid;
    bcm_module_t local;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Validation*/
    BCMDNX_NULL_CHECK(modid);
  
    if(!BCM_FABRIC_MODID_IS_LOCAL(local_modid))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("%d is not a local module id"), local_modid)); 
    }
    
    local = BCM_FABRIC_LOCAL_MODID_UNSET(local_modid);
    
    if(!DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, local))
    {
         BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local module id %d is out of range"), local_modid)); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    rc = bcm_dfe_modid_local_map_is_valid(unit, local, &is_valid);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_valid)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("local module id %d is invalid"), local_modid)); 
    }
    
    rc = bcm_dfe_modid_local_map_get(unit, local, modid);
    BCMDNX_IF_ERR_EXIT(rc);
    
    
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_link_topology_set
 * Purpose:
 *      Update links topology, set all the links connected to a destination
 * Parameters:
 *      unit        - (IN) Unit number.
 *      destination - (IN) Destination which all links in links_array connected to 
 *      links_count - (IN) Number of links in links_array 
 *      links_array - (IN) Array of all the links that needs to be changed 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_topology_set(
       int unit, 
       bcm_module_t destination,
       int links_count, 
       bcm_port_t *links_array)
{
    int i, rc, is_valid;
    bcm_module_t local_id;
    int min_nof_links;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(links_array);

    if(links_count < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal links_count"))); 
    }
    
    /*validate links*/
    for(i=0; i<links_count ; i++)
    { 
        if (links_array[i] != _SHR_PORT_INVALID) /* Skip invalid ports (virtual links support) */
        {
                DFE_LINK_INPUT_CHECK(unit, links_array[i]);
        }
    }
    
    /*validate destination type*/
    if(SOC_DFE_CONTROL(unit)->cfg.fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe2)
    {
        if(!BCM_FABRIC_MODID_IS_GROUP(destination))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In FE2 multistage destination must be a group"))); 
        }
        local_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
        if(!DFE_MODID_GROUP_IS_LEGAL(unit, local_id))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Group %d is out of range"), destination)); 
        }
    } else {
        if(BCM_FABRIC_MODID_IS_GROUP(destination))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only in FE2 multistage destination can be a group"))); 
        } 

    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*destination*/
    if(BCM_FABRIC_MODID_IS_GROUP(destination))
    {
        local_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    } else if(BCM_FABRIC_MODID_IS_LOCAL(destination)) {
        local_id = BCM_FABRIC_LOCAL_MODID_UNSET(destination);
        rc = bcm_dfe_modid_local_map_is_valid(unit, local_id, &is_valid);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_valid)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Local id %d wasn't allocated"), destination)); 
        }
    } else {
        /*destination is module id*/
        rc = bcm_dfe_modid_local_map_modid_to_local(unit, destination, &local_id);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_topology_set,(unit, local_id, links_count, links_array));
    BCMDNX_IF_ERR_EXIT(rc);

    /*MIN number of links*/
   SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_nof_links_to_min_nof_links_default, (unit, links_count, &min_nof_links)));
   SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_topology_min_nof_links_set, (unit, min_nof_links)));
    
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}


/*
 * Function:
 *      bcm_dfe_fabric_link_topology_get
 * Purpose:
 *      Get links topology.
 * Parameters:
 *      unit            - (IN)  Unit number.
 *      destination     - (IN)  Destination which all links in links_array connected to
 *      links_count_max - (IN)  Max number of links in links_array
 *      links_count     - (OUT) Size if links_array 
 *      links_array     - (OUT) Array of all the links that configured to be connected with destination 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_topology_get(
       int unit, 
       bcm_module_t destination,
       int links_count_max, 
       int *links_count, 
       bcm_port_t *links_array)
{
    bcm_module_t local_id;
    int rc = BCM_E_NONE, is_valid;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*validation*/
    BCMDNX_NULL_CHECK(links_count);
    BCMDNX_NULL_CHECK(links_array);
    
    if(0 == links_count_max)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("links_array size must be possitive"))); 
    }
    
    /*validate destination type*/
    if(SOC_DFE_CONTROL(unit)->cfg.fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe2)
    {
        if(!BCM_FABRIC_MODID_IS_GROUP(destination))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In FE2 multistage destination must be a group"))); 
        }
        local_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
        if(!DFE_MODID_GROUP_IS_LEGAL(unit, local_id))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Group %d is out of range"),destination)); 
        }
    }
    else
    {
        if(BCM_FABRIC_MODID_IS_GROUP(destination))
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Only in FE2 multistage destination can be a group"))); 
        }

    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    /*destination*/
    if(BCM_FABRIC_MODID_IS_GROUP(destination))
    {
        local_id = BCM_FABRIC_GROUP_MODID_UNSET(destination);
    }
    else if(BCM_FABRIC_MODID_IS_LOCAL(destination))
    {
        local_id = BCM_FABRIC_LOCAL_MODID_UNSET(destination);
        rc = bcm_dfe_modid_local_map_is_valid(unit, local_id, &is_valid);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_valid)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Local id %d wasn't allocated"),destination)); 
        }
    }
    else
    {
        /*destination is module id*/
        rc = bcm_dfe_modid_local_map_modid_to_local(unit, destination, &local_id);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_topology_get,(unit, local_id, links_count_max, links_count, links_array));
    BCMDNX_IF_ERR_EXIT(rc);
    
    
exit:  
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_multicast_set
 * Purpose:
 *      Set the replications of the multicast group in the fabric element 
 *      element
 * Parameters:
 *      unit         - (IN) Unit number.
 *      group        - (IN) Group number
 *      flags        - (IN) Configuration parameters
 *      destid_count - (IN) Size of destid_array
 *      destid_array - (IN) Array of destination IDs  
 * Note:
 *      destid_array represent the following: 
 *        FE2 (Single Stage) => Module ID 
 *        FE2 (Single Stage - Module Id above NUMBER_OF_LINKS-1) => Module ID. The SDK configures h/w with LOCAL_ID. NOTE: User can also pass LOCAL_ID. 
 *        FE13 => Module ID (multi-stage). SDK configures h/w with LOCAL_ID. NOTE: User can also pass LOCAL_ID 
 *        FE2 (multi Stage) => FAPs Group 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_multicast_set(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    uint32 destid_count, 
    bcm_module_t *destid_array)
{
    bcm_gport_t* port_array = NULL;
    int rc = BCM_E_NONE;
    int i, is_legal, is_free, is_valid;
    uint32 mc_table_entry_size;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    BCMDNX_NULL_CHECK(destid_array);
    
    if(destid_count > DFE_MAX_MODULES) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("too many destinations"))); 
    }

    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"),group)); 
    }
    
    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"),group)); 
    }

    rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_multicast_table_entry_size_get, (unit, &mc_table_entry_size));
    BCMDNX_IF_ERR_EXIT(rc);
    
    /*build port array*/
    BCMDNX_ALLOC(port_array, sizeof(bcm_gport_t) * destid_count, "port array");
    if(NULL == port_array) {
         BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Failed to allocate port_array"))); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    for(i=0 ; i < destid_count && BCM_E_NONE == rc ; i++)
    {
        /*convert to ports*/
        if(soc_dfe_fabric_device_mode_multi_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode)
        {
            /*destid must be a FAPs group*/
            if(BCM_FABRIC_MODID_IS_GROUP(destid_array[i]))
            {
                if ((BCM_FABRIC_GROUP_MODID_UNSET(destid_array[i])) >= mc_table_entry_size)
                {
                    BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Destination id  %d is larger than multicast table entry size %d"), BCM_FABRIC_GROUP_MODID_UNSET(destid_array[i]), mc_table_entry_size));
                }
                port_array[i] = BCM_FABRIC_GROUP_MODID_UNSET(destid_array[i]);
            } 
            else 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("In FE2 multi stage destid must be a FAPs group, failed on index %d"),i)); 
            }
        }
        else
        {
          /*destid must be Module ID or Local Module ID*/
          if(BCM_FABRIC_MODID_IS_GROUP(destid_array[i]))
          {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("If not FE2 multi stage destid must not be a FAPs group, failed on index %d"),i)); 
          }
          else
          {
               if(BCM_FABRIC_MODID_IS_LOCAL(destid_array[i]))
               {
                   if ((BCM_FABRIC_LOCAL_MODID_UNSET(destid_array[i])) >= mc_table_entry_size)
                   {
                       BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Destination id  %d is larger than multicast table entry size %d"), BCM_FABRIC_LOCAL_MODID_UNSET(destid_array[i]), mc_table_entry_size));
                   }
                   port_array[i] = BCM_FABRIC_LOCAL_MODID_UNSET(destid_array[i]);
                   rc = bcm_dfe_modid_local_map_is_valid(unit, port_array[i], &is_valid);
                   BCMDNX_IF_ERR_EXIT(rc);
                   if(!is_valid) {
                        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Local id %d isn't mapped, failed on index %d"),port_array[i], i));
                   }
               }
               else
               {
                   if((soc_dfe_multicast_mode_direct == SOC_DFE_CONFIG(unit).fabric_multicast_mode) &&  DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, destid_array[i]))
                   {
                       if (destid_array[i] >= mc_table_entry_size)
                       {
                           BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Destination id  %d is larger than multicast table entry size %d"), destid_array[i], mc_table_entry_size));
                       }
                       port_array[i] = destid_array[i];
                   }
                   else
                   {
                       /*Module ID, need to convert to local*/
                       rc = bcm_dfe_modid_local_map_modid_to_local(unit, destid_array[i], &(port_array[i]));
                       BCMDNX_IF_ERR_EXIT(rc);
                   }
               }
          }  
      }
    }
    
    /*Write to reg*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_set,(unit, group, destid_count, (soc_gport_t*)port_array));
    BCMDNX_IF_ERR_EXIT(rc);
   
exit: 
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    BCM_FREE(port_array); 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_multicast_get
 * Purpose:
 *      Get the replications of the multicast group in the fabric
 *      element
 * Parameters:
 *      unit             - (IN)  Unit number.
 *      group            - (IN)  Group number
 *      flags            - (IN)  Configuration parameters
 *      destid_count_max - (IN)  Max size of destid_array
 *      destid_count     - (OUT) Size of destid_array
 *      destid_array     - (OUT) Array of destination IDs
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_multicast_get(
    int unit, 
    bcm_multicast_t group, 
    uint32 flags, 
    int destid_count_max, 
    int *destid_count, 
    bcm_module_t *destid_array)
{
    bcm_gport_t* port_array = NULL;
    int rc = BCM_E_NONE;
    int i, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    BCMDNX_NULL_CHECK(destid_count);
    BCMDNX_NULL_CHECK(destid_array);
    
    if(destid_count_max > DFE_MAX_MODULES || destid_count_max<0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("illegal destid_count_max"))); 
    }

    rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
    BCMDNX_IF_ERR_EXIT(rc);
    if(!is_legal)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"),group)); 
    }
    
    rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
    BCMDNX_IF_ERR_EXIT(rc);
    if(is_free)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"),group)); 
    }
    
    /*get port array*/
    BCMDNX_ALLOC(port_array, sizeof(bcm_gport_t) * destid_count_max, "port array");
    if(NULL == port_array) {
       BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("Failed to allocate port_array"))); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_multicast_egress_get,(unit, group, destid_count_max, (soc_gport_t*)port_array, destid_count));
    BCMDNX_IF_ERR_EXIT(rc);
    
    for(i=0 ; i<*destid_count ; i++)
    {
        if(soc_dfe_fabric_device_mode_multi_stage_fe2 == SOC_DFE_CONFIG(unit).fabric_device_mode)
        {
            /*port is FAPs group ID*/
            destid_array[i] = BCM_FABRIC_GROUP_MODID_SET(port_array[i]);
        }
        else
        {
             if((soc_dfe_multicast_mode_direct == SOC_DFE_CONFIG(unit).fabric_multicast_mode) &&  DFE_MODID_LOCAL_MAP_ROW_VALIDATE(unit, port_array[i]))
             {
                destid_array[i] = port_array[i];
             }
             else
             {
                 /*port is Local Module ID*/
                 rc = bcm_dfe_modid_local_map_get(unit, port_array[i], &(destid_array[i]));
                 BCMDNX_IF_ERR_EXIT(rc);
             } 
        }
    }
    
exit: 
    BCM_FREE(port_array); 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/* Get a set of replications of the multicast groups in the fabric element */
int 
bcm_dfe_fabric_multicast_multi_get(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array)
{
    int rc = BCM_E_NONE;
    int i, is_free, is_legal;
    bcm_multicast_t group;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*Valiodation*/
    BCMDNX_NULL_CHECK(groups);
    BCMDNX_NULL_CHECK(dest_array);

    /*group validation*/
    for (i=0; i< ngroups; i++) {
        group = groups[i];
        rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_legal)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"),group)); 
        }

        rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
        BCMDNX_IF_ERR_EXIT(rc);
        if(is_free)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"),group)); 
        }
    }

    DFE_UNIT_LOCK_TAKE(unit);
    
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_multi_get,(unit, flags, ngroups, groups, dest_array));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;

}

/* 
 * Set replications of the multicast groups in the fabric
 * element
 */
int 
bcm_dfe_fabric_multicast_multi_set(
    int unit, 
    uint32 flags, 
    uint32 ngroups, 
    bcm_multicast_t *groups, 
    bcm_fabric_module_vector_t *dest_array)
{
    bcm_multicast_t group;
    int rc = BCM_E_NONE;
    int i, is_legal, is_free;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*parametes are required only for set stage*/
    if (!flags || (flags & _SHR_FABRIC_MULTICAST_SET_ONLY))
    {
        /*Valiodation*/
        BCMDNX_NULL_CHECK(dest_array);
        BCMDNX_NULL_CHECK(groups);
        
        /*group_id and module vector validation*/
        for (i=0; i< ngroups; i++) 
        {
            group = groups[i];
            rc = bcm_dfe_multicst_id_map_is_legal_id(unit, group, &is_legal);
            BCMDNX_IF_ERR_EXIT(rc);
            if(!is_legal)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d is illegal"),group)); 
            }
            
            rc = bcm_dfe_multicst_id_map_is_free_id(unit, group, &is_free);
            BCMDNX_IF_ERR_EXIT(rc);
            if(is_free)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("group %d wasn't created"),group)); 
            }
        }
    }
    
    
    DFE_UNIT_LOCK_TAKE(unit);
    
   
    /*Write to reg*/
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_multicast_multi_set,(unit, flags, ngroups, groups, dest_array));
    BCMDNX_IF_ERR_EXIT(rc);
   
exit: 
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_link_control_set
 * Purpose:
 *      Set fabric-link attribute (fifo ype) per link. Each link might
 *      have two fifo types; one per RX and one per TX
 * Parameters:
 *      unit - (IN) Unit number.
 *      link - (IN) Link Number 
 *      type - (IN) Control type
 *      arg  - (IN) Value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_control_set(
    int unit, 
    bcm_port_t link, 
    bcm_fabric_link_control_t type, 
    int arg)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*General validation*/
    DFE_LINK_INPUT_CHECK(unit, link);
    
    DFE_UNIT_LOCK_TAKE(unit);

    switch(type)
    {
        case bcmFabricLinkCellFormat:
            rc = _bcm_dfe_fabric_link_control_set_cell_format(unit, link, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricLinkCellInterleavingEnable:
            rc = _bcm_dfe_fabric_links_cell_interleaving_set(unit, link, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricLinkPrimaryWeight:
              rc = _bcm_dfe_fabric_link_control_set_weight(unit, link, 1, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkSecondaryWeight:
              rc = _bcm_dfe_fabric_link_control_set_weight(unit, link, 0, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkIsSecondaryOnly:
              rc =  MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_secondary_only_set,(unit, link, arg ? 1 : 0));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLLFControlSource:
              rc = _bcm_dfe_fabric_link_control_set_llfc_control_source(unit, link, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;       
        case bcmFabricLinkRepeaterDestinationLink:
              rc = _bcm_dfe_fabric_link_control_set_repeater_destination_per_link(unit, link, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;   
        case bcmFabricLinkIsolate:
              rc =  MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_isolate_set,(unit,link, arg ? soc_dcmn_isolation_status_isolated : soc_dcmn_isolation_status_active));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkPcpEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pcp_enable_set, (unit, link, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
       case bcmFabricLinkRepeaterEnable:
           rc = _bcm_dfe_fabric_link_repeater_enable_set(unit, link, arg);
           BCMDNX_IF_ERR_EXIT(rc);
           break;
        default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
    }  


    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 

}

/*
 * Function:
 *      bcm_dfe_fabric_link_control_get
 * Purpose:
 *       Get fabric-link attribute (fifo type) per link and direction
 * Parameters:
 *      unit - (IN)  Unit number.
 *      link - (IN)  Link Number 
 *      type - (IN)  Control type 
 *      arg  - (OUT) Value
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_control_get(
    int unit, 
    bcm_port_t link, 
    bcm_fabric_link_control_t type, 
    int *arg)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    /*General validation*/
    BCMDNX_NULL_CHECK(arg);
    DFE_LINK_INPUT_CHECK(unit, link);
    
    DFE_UNIT_LOCK_TAKE(unit); 
      
    switch(type)
    {
        case bcmFabricLinkCellFormat:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_format_get,(unit,link,(soc_dfe_fabric_link_cell_size_t*)arg));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkCellInterleavingEnable:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_cell_interleaving_get,(unit, link, arg)); 
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkPrimaryWeight:
              rc = _bcm_dfe_fabric_link_control_weight_get(unit, link, 1, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkSecondaryWeight:
              rc = _bcm_dfe_fabric_link_control_weight_get(unit, link, 0, arg);
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkIsSecondaryOnly:
              rc =  MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_secondary_only_get,(unit, link, arg));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLLFControlSource: 
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_llf_control_source_get,(unit,link,(soc_dcmn_fabric_pipe_t*)arg));
              BCMDNX_IF_ERR_EXIT(rc);
              break;   
        case bcmFabricLinkRepeaterDestinationLink:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_topology_repeater_destination_get,(unit, link, arg));
              BCMDNX_IF_ERR_EXIT(rc);
              break;   
        case bcmFabricLinkIsolate:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_isolate_get,(unit,link, (soc_dcmn_isolation_status_t*)arg)); 
              BCMDNX_IF_ERR_EXIT(rc);
              break;
        case bcmFabricLinkPcpEnable:
            rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pcp_enable_get, (unit, link, arg));
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        case bcmFabricLinkRepeaterEnable:
            rc = _bcm_dfe_fabric_link_repeater_enable_get(unit, link, arg);
            BCMDNX_IF_ERR_EXIT(rc);
            break;
        default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type)); 
    }  
   
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;    
}

/*
 * Function:
 *      bcm_dfe_fabric_link_threshold_add
 * Purpose:
 *      Add thresholds set
 * Parameters:
 *      unit      - (IN)     Unit number.
 *      flags     - (IN)     Configuration Parameters. 
                             If bcm_dfe_fabric_LINK_THRESHOLD_WITH_ID flag is set the fifo_type will be used as IN parameter, 
                             otherwise will be used as OUT parameter
 *      fifo_type - (IN/OUT) Handle of the FIFO type
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_threshold_add(
    int unit, 
    uint32 flags, 
    int *fifo_type)
{
    int i, rc, is_overlap;
    _bcm_dfe_fifo_type_handle_t new_fifo_type, fifo_type_handle;
    int is_rx=0, is_tx=0, is_fe1=0, is_fe3=0, is_primary=0, is_secondary=0;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(fifo_type);
    
    rc = bcm_dfe_fifo_type_clear(unit, &new_fifo_type);
    BCMDNX_IF_ERR_EXIT(rc);
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    if(flags & BCM_FABRIC_LINK_THRESHOLD_WITH_ID)
    {
        /*fifo_type is an input*/
        rc = bcm_dfe_fifo_type_get_id(unit, *fifo_type, &i);
        BCMDNX_IF_ERR_EXIT(rc);
        DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(i);

        /*all relevant bits are free*/
        rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, FIFO_HANDLERS, i, &fifo_type_handle);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_overlap(unit, *fifo_type, fifo_type_handle, &is_overlap);
        BCMDNX_IF_ERR_EXIT(rc);
        if(!is_overlap)
        {
             bcm_dfe_fifo_type_add(unit, i, *fifo_type);
        }
        else
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("fifo_type for specified flags already allocated"))); 
        }

        rc = bcm_dfe_fifo_type_set_handle_flag(unit, (uint32*)fifo_type); /* this is required for the new attach implementation , we need to indicate that we are using a "real" handle */
        BCMDNX_IF_ERR_EXIT(rc);
    }
    else
    {
        /*set fifo_type*/
        if(flags & BCM_FABRIC_LINK_THRESHOLD_RX_FIFO_ONLY)
            is_rx = 1;
        else if(flags & BCM_FABRIC_LINK_THRESHOLD_TX_FIFO_ONLY)
            is_tx = 1;
        else 
        {
            is_rx = 1;
            is_tx = 1;
        }
          
        if(SOC_DFE_CONFIG(unit).fabric_device_mode == soc_dfe_fabric_device_mode_multi_stage_fe13)
        {
            if(flags & BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY)
                is_fe1 = 1;
            else if(flags & BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY)
                is_fe3 = 1;
            else
            {
                 is_fe1 = 1;
                 is_fe3 = 1;
            }
        }
        else
        {
             is_fe1 = 1;
             is_fe3 = 1;
        }
       
        if(flags & BCM_FABRIC_LINK_TH_PRIM_ONLY)
            is_primary = 1;
        else if(flags & BCM_FABRIC_LINK_TH_SCND_ONLY)
            is_secondary = 1;
        else
        {
            is_primary = 1;
            is_secondary = 1;
        }  
      
        rc = bcm_dfe_fifo_type_set(unit, &new_fifo_type, is_rx, is_tx, is_fe1, is_fe3, is_primary, is_secondary);
        BCMDNX_IF_ERR_EXIT(rc);
        
        for(i=0 ; i< soc_dfe_fabric_nof_link_fifo_types ; i++)
        {
            /*all relevant bits are free*/
            rc = SOC_DFE_WARM_BOOT_ARR_VAR_GET(unit, FIFO_HANDLERS, i, &fifo_type_handle);
            BCMDNX_IF_ERR_EXIT(rc);

            rc = bcm_dfe_fifo_type_is_overlap(unit, new_fifo_type, fifo_type_handle, &is_overlap);
            BCMDNX_IF_ERR_EXIT(rc);
            if(!is_overlap)
            {
                bcm_dfe_fifo_type_add(unit, i, new_fifo_type);
                bcm_dfe_fifo_type_set_id(unit, &new_fifo_type, i);
                rc = bcm_dfe_fifo_type_set_handle_flag(unit, &new_fifo_type);
                BCMDNX_IF_ERR_EXIT(rc);
                *fifo_type = new_fifo_type;
                BCM_EXIT;
            }
        }
            
        /*If we're here fifo typed wasn't added*/
        BCMDNX_ERR_EXIT_MSG(BCM_E_EXISTS, (_BSL_BCM_MSG("fifo_type for specified flags already allocated"))); 
      
    } 
    
    
exit: 
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      _bcm_dfe_fabric_fifo_info_get
 * Purpose:
 *      Getting the parameters required for bcm_dfe_fabric_link_thresholds_pipe_get
 * Parameters:
 *      unit                            - (IN)    Unit number.
 *      pipe_id                         - (IN)    0/1/2   
 *      direction                       - (IN)    rx/tx/middle
 *      fifo_type, pipe_get, flags_get  - (OUT) should be used when calling to bcm_dfe_fabric_link_thresholds_pipe_get
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int
_bcm_dfe_fabric_fifo_info_get(
   int unit,
   bcm_port_t port,
   bcm_fabric_pipe_t pipe_id,
   soc_dcmn_fabric_direction_t  direction,
   uint32 *fifo_type,
   bcm_fabric_pipe_t *pipe_get,
   uint32 *flags_get)
{
    int rv, i;
    int fifo_index;
    int is_rx, is_tx, is_fe1, is_fe3, is_primary, is_secondary;
    bcm_port_t links[SOC_DFE_MAX_NOF_LINKS];
    uint32 links_count;
    soc_dfe_fabric_link_device_mode_t link_mode;
    int found;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(fifo_type);
    BCMDNX_NULL_CHECK(pipe_get);
    BCMDNX_NULL_CHECK(flags_get);

    /*Middle stage is a special case - no FIFOs per port*/
    if (direction  == soc_dcmn_fabric_direction_middle)
    {
        *fifo_type = soc_dfe_fabric_link_fifo_type_index_0;
        *pipe_get = pipe_id;
        *flags_get = 0;
        BCM_EXIT;
    } else if (direction  == soc_dcmn_fabric_direction_middle_local_switch_low ||
               direction  == soc_dcmn_fabric_direction_middle_local_switch_high)
    {
        *fifo_type = soc_dfe_fabric_link_fifo_type_index_1;
        *pipe_get = pipe_id;
        *flags_get = (direction  == soc_dcmn_fabric_direction_middle_local_switch_low) ? BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY : BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY;
        BCM_EXIT;
    }

    /*Verify*/
    if (pipe_id >= SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid pipe , more than number of pipes")));
    }
    DFE_LINK_INPUT_CHECK(unit, port);


    DFE_UNIT_LOCK_TAKE(unit);

    is_rx = (direction == soc_dcmn_fabric_direction_rx ? 1 : 0);
    is_tx = (direction == soc_dcmn_fabric_direction_tx ? 1 : 0);

    rv = bcm_dfe_fifo_type_clear(unit, fifo_type);
    BCMDNX_IF_ERR_EXIT(rv);

    BCMDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_device_mode_get,(unit, port, is_rx, &link_mode)));
    if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe1)
    {
        is_fe1 = 1;
        is_fe3 = 0;
    } else if (link_mode == soc_dfe_fabric_link_device_mode_multi_stage_fe3) {
        is_fe1 = 0;
        is_fe3 = 1;
    } else {
        is_fe1 = is_fe3 = 1;
    }


    /*Iterate over fifo types */
    found = 0;
    for(fifo_index = 0 ; fifo_index < soc_dfe_fabric_nof_link_fifo_types; fifo_index++) 
    {
        /*Check if the relvant fifo is attached to the fifo type*/
        rv = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_link_type_get,(unit, pipe_id, fifo_index, is_rx, is_tx, is_fe1, is_fe3, SOC_DFE_MAX_NOF_LINKS, links, &links_count));
        BCMDNX_IF_ERR_EXIT(rv);

        for (i = 0; i < links_count; i++)
        {
            if (port == links[i])
            {
                found = 1;
                if (SOC_IS_FE1600(unit))
                {
                    is_primary = (pipe_id == bcmFabricPipe0 ? 1 : 0);
                    is_secondary = (pipe_id == bcmFabricPipe1 ? 1 : 0);
                    /*legacy implementation*/
                    /*Build the handle*/
                    rv = bcm_dfe_fifo_type_set(unit, fifo_type , is_rx, is_tx, is_fe1, is_fe3, is_primary, is_secondary);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = bcm_dfe_fifo_type_set_id(unit, fifo_type, fifo_index);
                    BCMDNX_IF_ERR_EXIT(rv);
                    rv = bcm_dfe_fifo_type_set_handle_flag(unit, fifo_type);
                    BCMDNX_IF_ERR_EXIT(rv);
    
                    *pipe_get = bcmFabricPipeAll;
                    *flags_get = 0;
                } else {
                    *fifo_type = fifo_index;
                    *pipe_get = pipe_id;
                    if (is_fe1 && !is_fe3)
                    {
                        *flags_get = BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY;
                    } else if (!is_fe1 && is_fe3)
                    {
                        *flags_get = BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY;
                    } else {
                        *flags_get = 0;
                    }
                }
            }
        }
    }

    if (!found)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("fifo type wasn't found.\n")));
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_fabric_link_threshold_delete
 * Purpose:
 *      Delete thresholds set
 * Parameters:
 *      unit      - (IN) Unit number.
 *      fifo_type - (IN) Handle of the FIFO type 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_threshold_delete(
    int unit, 
    int fifo_type)
{
    int i, rc;
    BCMDNX_INIT_FUNC_DEFS;
    
    rc = bcm_dfe_fifo_type_get_id(unit, fifo_type, &i);
    BCMDNX_IF_ERR_EXIT(rc);
    DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(i)
      
    DFE_UNIT_LOCK_TAKE(unit);
    /*clear fifo_type*/
    rc = bcm_dfe_fifo_type_sub(unit, i, fifo_type);
    BCMDNX_IF_ERR_EXIT(rc);
    
exit: 
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
    
}

/*
 * Function:
 *      bcm_dfe_fabric_link_thresholds_set
 * Purpose:
 *      Set fifo_type thresholds
 * Parameters:
 *      unit      - (IN) Unit number.
 *      fifo_type - (IN) fifo_type you would like to update
 *      count     - (IN) Size of the array
 *      type      - (IN) Array of bcm_fabric_link_threshold_type_t to set 
 *      value     - (IN) value[i] is the threshold to be assigned with type[i]
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_thresholds_set(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t* type, 
    int *value)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_fabric_link_thresholds_pipe_set(unit, fifo_type, bcmFabricPipeAll, 0, count, type, value);
    BCMDNX_IF_ERR_EXIT(rc);

exit: 
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_fabric_link_thresholds_get
 * Purpose:
 *      Get fifo_type thresholds
 * Parameters:
 *      unit      - (IN)  Unit number.
 *      fifo_type - (IN)  fifo_type you would like to get
 *      count     - (IN)  Size of the array
 *      type      - (IN)  Array of bcm_fabric_link_threshold_type_t to get
 *      value     - (OUT) value[i] is the threshold assigned with type[i] 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_thresholds_get(
    int unit, 
    int fifo_type, 
    uint32 count, 
    bcm_fabric_link_threshold_type_t *type, 
    int *value)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;

    rc = bcm_dfe_fabric_link_thresholds_pipe_get(unit, fifo_type, bcmFabricPipeAll, 0,count, type, value);
    BCMDNX_IF_ERR_EXIT(rc);

exit:   
    BCMDNX_FUNC_RETURN;  
}

int 
bcm_dfe_fabric_link_thresholds_pipe_set(
    int unit,
    int fifo_type,
    bcm_fabric_pipe_t pipe,
    uint32 flags,
    uint32 count,
    bcm_fabric_link_threshold_type_t *type,
    int *value)
{
    int i, type_idx, is_fe1, is_fe3, is_primary, is_secondary, is_handle_flag, nof_pipes;
    int rc = BCM_E_NONE;
    soc_dcmn_fabric_pipe_t fabric_pipe;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);
    SOC_DCMN_FABRIC_PIPE_INIT(fabric_pipe);
 
    if (SOC_IS_FE1600(unit) && (pipe !=  bcmFabricPipeAll))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid pipe for fe1600 "))); 
    }

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    if (pipe >= nof_pipes)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid pipe , more than number of pipes")));
    }

    /* validate flags */
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_thresholds_flags_validate,(unit, flags));
    BCMDNX_IF_ERR_EXIT(rc);

    /*get type*/
    rc = bcm_dfe_fifo_type_get_handle_flag(unit, fifo_type, &is_handle_flag);
    BCMDNX_IF_ERR_EXIT(rc);

    if (!is_handle_flag)
    {
        if (flags & BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY)
        {
            is_fe1 = 1; 
            is_fe3 = 0;
        }
        else if (flags & BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY)
        {
            is_fe1 = 0;
            is_fe3 = 1;
        }
        else
        {
            is_fe1 = 1;
            is_fe3 = 1;
        }
        is_primary = 1;
        is_secondary = 1;
        type_idx =  fifo_type;
    }
    else
    {
        rc = bcm_dfe_fifo_type_get_id(unit, fifo_type, &type_idx); 
        BCMDNX_IF_ERR_EXIT(rc);

        rc =  bcm_dfe_fifo_type_is_primary(unit, fifo_type, &is_primary);
        BCMDNX_IF_ERR_EXIT(rc);
        rc =  bcm_dfe_fifo_type_is_secondary(unit, fifo_type, &is_secondary);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = bcm_dfe_fifo_type_is_fe1(unit, fifo_type, &is_fe1);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe3(unit, fifo_type, &is_fe3);
        BCMDNX_IF_ERR_EXIT(rc); 
    }
    DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(type_idx);

    if (pipe == bcmFabricPipeAll)
    {
        SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&fabric_pipe, nof_pipes);
        if (!is_primary)
        {
            SOC_DCMN_FABRIC_PIPE_CLEAR(&fabric_pipe, 0);
        }
        if (!is_secondary)
        {
            SOC_DCMN_FABRIC_PIPE_CLEAR(&fabric_pipe, 1);
        }
    }
    else
    {
        SOC_DCMN_FABRIC_PIPE_SET(&fabric_pipe, pipe - bcmFabricPipe0);
    }


    for(i = 0 ; i<count ; i++) {
        /*Validate type*/
        if (is_handle_flag)
        {
            rc = _bcm_dfe_fabric_link_control_validate(unit, fifo_type, type[i]);
            BCMDNX_IF_ERR_EXIT(rc);
        }

        switch (type[i])
        {
            case bcmFabricLinkRxFifoLLFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_llfc_threshold_validate,(unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkFE1TxBypassLLFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_validate,(unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRciFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_rci_threshold_validate,(unit, type[i],type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxGciLvl1FC:
            case bcmFabricLinkTxGciLvl2FC:
            case bcmFabricLinkTxGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_gci_threshold_validate,(unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxGciLvl1FC:
            case bcmFabricLinkRxGciLvl2FC:
            case bcmFabricLinkRxGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_gci_threshold_validate,(unit, type[i], type_idx, fabric_pipe, value[i],is_fe1,is_fe3));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxPrio0Drop:
            case bcmFabricLinkTxPrio1Drop:
            case bcmFabricLinkTxPrio2Drop:
            case bcmFabricLinkTxPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_drop_threshold_validate,(unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxPrio0Drop:
            case bcmFabricLinkRxPrio1Drop:
            case bcmFabricLinkRxPrio2Drop:
            case bcmFabricLinkRxPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_drop_threshold_validate,(unit, type[i], fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxRciLvl1FC:
            case bcmFabricLinkRxRciLvl2FC:
            case bcmFabricLinkRxRciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_rci_threshold_validate, (unit, type[i], type_idx, fabric_pipe, value[i], is_fe1, is_fe3));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxFull:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_full_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxMcLowPrioDrop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_validate, (unit,type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidGciLvl1FC:
            case bcmFabricLinkMidGciLvl2FC:
            case bcmFabricLinkMidGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_gci_threshold_validate, (unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidRciLvl1FC:
            case bcmFabricLinkMidRciLvl2FC:
            case bcmFabricLinkMidRciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_rci_threshold_validate, (unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;
                
            case bcmFabricLinkMidPrio0Drop:
            case bcmFabricLinkMidPrio1Drop:
            case bcmFabricLinkMidPrio2Drop:
            case bcmFabricLinkMidPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_validate, (unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidAlmostFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break; 

            case bcmFabricLinkMidFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;
            
            case bcmFabricLinkTxRciLvl1FC:
            case bcmFabricLinkTxRciLvl2FC:
            case bcmFabricLinkTxRciLvl3FC:
                rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_validate, (unit, type[i], type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxAlmostFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_full_threshold_validate, (unit, type_idx, fabric_pipe, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            default:
                break;

        }

    }

    DFE_UNIT_LOCK_TAKE(unit);

    for (i = 0 ; i<count ; i++)
    {
        switch (type[i])
        {
            case bcmFabricLinkRxFifoLLFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_llfc_threshold_set,(unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkFE1TxBypassLLFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_set,(unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRciFC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_rci_threshold_set,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxGciLvl1FC:
            case bcmFabricLinkTxGciLvl2FC:
            case bcmFabricLinkTxGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_gci_threshold_set,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxGciLvl1FC:
            case bcmFabricLinkRxGciLvl2FC:
            case bcmFabricLinkRxGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_gci_threshold_set,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxPrio0Drop:
            case bcmFabricLinkTxPrio1Drop:
            case bcmFabricLinkTxPrio2Drop:
            case bcmFabricLinkTxPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_drop_threshold_set,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxPrio0Drop:
            case bcmFabricLinkRxPrio1Drop:
            case bcmFabricLinkRxPrio2Drop:
            case bcmFabricLinkRxPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_drop_threshold_set,(unit, type[i], fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxRciLvl1FC:
            case bcmFabricLinkRxRciLvl2FC:
            case bcmFabricLinkRxRciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_rci_threshold_set, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxFull:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_full_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkRxMcLowPrioDrop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_set, (unit,type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidGciLvl1FC:
            case bcmFabricLinkMidGciLvl2FC:
            case bcmFabricLinkMidGciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_gci_threshold_set, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidRciLvl1FC:
            case bcmFabricLinkMidRciLvl2FC:
            case bcmFabricLinkMidRciLvl3FC:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_rci_threshold_set, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidPrio0Drop:
            case bcmFabricLinkMidPrio1Drop:
            case bcmFabricLinkMidPrio2Drop:
            case bcmFabricLinkMidPrio3Drop:
                rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_set, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidAlmostFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_set, (unit, type_idx, fabric_pipe, -1, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;
                 
            case bcmFabricLinkMidFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i], 1));
                BCMDNX_IF_ERR_EXIT(rc);
                break; 
                
            case bcmFabricLinkTxRciLvl1FC:
            case bcmFabricLinkTxRciLvl2FC:
            case bcmFabricLinkTxRciLvl3FC:
                rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_set, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;                                                

            case bcmFabricLinkTxAlmostFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i], 1));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkTxFifoSize:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i], 1));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            case bcmFabricLinkMidFull:
                rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_full_threshold_set, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, value[i]));
                BCMDNX_IF_ERR_EXIT(rc);
                break;

            default:
                break;
        }
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

int 
bcm_dfe_fabric_link_thresholds_pipe_get(
    int unit,
    int fifo_type,
    bcm_fabric_pipe_t pipe,
    uint32 flags,
    uint32 count,
    bcm_fabric_link_threshold_type_t *type,
    int *value)
{
    int i, type_idx, is_fe1, is_fe3, is_primary, is_secondary, is_handle_flag, nof_pipes;
    soc_dcmn_fabric_pipe_t fabric_pipe;
    int rc = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(type);
    BCMDNX_NULL_CHECK(value);
    SOC_DCMN_FABRIC_PIPE_INIT(fabric_pipe);

    if (SOC_IS_FE1600(unit) && pipe !=  bcmFabricPipeAll)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid pipe for fe1600 "))); 
    }

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;
    if (pipe >= nof_pipes)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid pipe , more than number of pipes")));
    }

    rc = bcm_dfe_fifo_type_get_handle_flag(unit, fifo_type, &is_handle_flag);
    BCMDNX_IF_ERR_EXIT(rc);

    if (!is_handle_flag)
    {
        if (flags & BCM_FABRIC_LINK_TH_FE1_LINKS_ONLY)
        {
            is_fe1 = 1; 
            is_fe3 = 0;
        }
        else if (flags & BCM_FABRIC_LINK_TH_FE3_LINKS_ONLY)
        {
            is_fe1 = 0;
            is_fe3 = 1;
        }
        else
        {
            is_fe1 = 1;
            is_fe3 = 1;
        }
        is_primary = 1;
        is_secondary = 1;
        type_idx =  fifo_type;

    }
    else
    {
        rc = bcm_dfe_fifo_type_get_id(unit, fifo_type, &type_idx); 
        BCMDNX_IF_ERR_EXIT(rc);

        rc =  bcm_dfe_fifo_type_is_primary(unit, fifo_type, &is_primary);
        BCMDNX_IF_ERR_EXIT(rc);
        rc =  bcm_dfe_fifo_type_is_secondary(unit, fifo_type, &is_secondary);
        BCMDNX_IF_ERR_EXIT(rc);

        rc = bcm_dfe_fifo_type_is_fe1(unit, fifo_type, &is_fe1);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe3(unit, fifo_type, &is_fe3);
        BCMDNX_IF_ERR_EXIT(rc); 
    }

    DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(type_idx);

    if (pipe == bcmFabricPipeAll)
    {
        SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&fabric_pipe, nof_pipes);
        if (!is_primary)
        {
            SOC_DCMN_FABRIC_PIPE_CLEAR(&fabric_pipe, 0);
        }
        if (!is_secondary)
        {
            SOC_DCMN_FABRIC_PIPE_CLEAR(&fabric_pipe, 1);
        }
    }
    else
    {
        SOC_DCMN_FABRIC_PIPE_SET(&fabric_pipe, pipe - bcmFabricPipe0);
    }

    for(i = 0 ; i<count ; i++)
    { 
        if (is_handle_flag)
        {
            rc = _bcm_dfe_fabric_link_control_validate(unit, fifo_type, type[i]);
            BCMDNX_IF_ERR_EXIT(rc);
        }
    }
    


    DFE_UNIT_LOCK_TAKE(unit);

    for(i = 0 ; i<count; i++)
    {
        switch(type[i])
        {
          case bcmFabricLinkRxFifoLLFC: 
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_llfc_threshold_get,(unit, type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkFE1TxBypassLLFC: 
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_bypass_llfc_threshold_get,(unit, type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkRciFC: 
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_rci_threshold_get,(unit, type[i],type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkTxGciLvl1FC: 
          case bcmFabricLinkTxGciLvl2FC:
          case bcmFabricLinkTxGciLvl3FC:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_gci_threshold_get,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkRxGciLvl1FC: 
          case bcmFabricLinkRxGciLvl2FC:
          case bcmFabricLinkRxGciLvl3FC:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_gci_threshold_get,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkTxPrio0Drop:
          case bcmFabricLinkTxPrio1Drop: 
          case bcmFabricLinkTxPrio2Drop:
          case bcmFabricLinkTxPrio3Drop:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_tx_drop_threshold_get,(unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
          
          case bcmFabricLinkRxPrio0Drop: 
          case bcmFabricLinkRxPrio1Drop:
          case bcmFabricLinkRxPrio2Drop:
          case bcmFabricLinkRxPrio3Drop:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_drop_threshold_get,(unit, type[i], fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkRxRciLvl1FC:
          case bcmFabricLinkRxRciLvl2FC:
          case bcmFabricLinkRxRciLvl3FC:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_rci_threshold_get, (unit, type[i], type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkRxFull:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_full_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, is_fe3,  &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkRxFifoSize:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_fifo_size_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkRxMcLowPrioDrop:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_rx_multicast_low_prio_drop_threshold_get, (unit,type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkMidGciLvl1FC:
          case bcmFabricLinkMidGciLvl2FC:
          case bcmFabricLinkMidGciLvl3FC:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_gci_threshold_get, (unit, type[i], type_idx, fabric_pipe, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkMidRciLvl1FC:
          case bcmFabricLinkMidRciLvl2FC:
          case bcmFabricLinkMidRciLvl3FC:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_rci_threshold_get, (unit, type[i], type_idx, fabric_pipe, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkMidPrio0Drop:
          case bcmFabricLinkMidPrio1Drop:
          case bcmFabricLinkMidPrio2Drop:
          case bcmFabricLinkMidPrio3Drop:
              rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_flow_control_mid_prio_drop_threshold_get, (unit, type[i], type_idx, fabric_pipe, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;

          case bcmFabricLinkMidAlmostFull:
              rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_almost_full_threshold_get, (unit, type_idx, fabric_pipe, 0, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break; 

          case bcmFabricLinkMidFifoSize:
              rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_fifo_size_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break; 

          case bcmFabricLinkTxRciLvl1FC:
          case bcmFabricLinkTxRciLvl2FC:
          case bcmFabricLinkTxRciLvl3FC:
              rc =  MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_rci_threshold_fc_get, (unit, type[i],type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break; 
              
          case bcmFabricLinkTxAlmostFull:
              rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_almost_full_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;
              
          case bcmFabricLinkTxFifoSize:
              rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_tx_fifo_size_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, is_fe3, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break; 
              
          case bcmFabricLinkMidFull:
              rc = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_flow_control_mid_full_threshold_get, (unit, type_idx, fabric_pipe, is_fe1, &(value[i])));
              BCMDNX_IF_ERR_EXIT(rc);
              break;                                                
                        
          default:
              BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("Unsupported Type %d"),type[i])); 
        }
    }
  
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}



/*
 * Function:
 *      bcm_fabric_link_thresholds_attach
 * Purpose:
 *      Attach links to fifo_type
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      fifo_type           - (IN)  fifo_type
 *      links_count         - (IN)  Number of links
 *      links               - (IN)  Links array
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_thresholds_attach(
    int unit, 
    int fifo_type, 
    uint32     links_count,
    bcm_port_t* links)
{
    int fifo_type_index, rc = BCM_E_NONE;
    int is_rx, is_tx, is_fe1, is_fe3, is_primary, is_secondary, is_handle_flag, nof_pipes;
    soc_dcmn_fabric_pipe_t pipe;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(links);
    SOC_DCMN_FABRIC_PIPE_INIT(pipe);

    rc = bcm_dfe_fifo_type_get_handle_flag(unit, fifo_type, &is_handle_flag);
    BCMDNX_IF_ERR_EXIT(rc);

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (!is_handle_flag) /* direct attach - without fifo handle */
    {
        fifo_type_index = fifo_type;
        is_rx = 1;
        is_tx = 1;
        is_fe1 = 1;
        is_fe3 = 1;
        SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&pipe, nof_pipes);
    }
    else 
    {
        rc = bcm_dfe_fifo_type_get_id(unit, fifo_type, &fifo_type_index);
        BCMDNX_IF_ERR_EXIT(rc);

        /*get pipe*/
        rc =  bcm_dfe_fifo_type_is_primary(unit, fifo_type, &is_primary);
        BCMDNX_IF_ERR_EXIT(rc);
        rc =  bcm_dfe_fifo_type_is_secondary(unit, fifo_type, &is_secondary);
        BCMDNX_IF_ERR_EXIT(rc);

        if(is_primary)
        {
            SOC_DCMN_FABRIC_PIPE_SET(&pipe,0);
        }
        if(is_secondary)
        {
            SOC_DCMN_FABRIC_PIPE_SET(&pipe,1);
        }

        DFE_FABRIC_CONTROL_SOURCE_VALID_CHECK(pipe);
        
        rc = bcm_dfe_fifo_type_is_rx(unit, fifo_type, &is_rx);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_tx(unit, fifo_type, &is_tx);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe1(unit, fifo_type, &is_fe1);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe3(unit, fifo_type, &is_fe3);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(fifo_type_index);

    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_link_type_set,(unit, pipe, fifo_type_index, is_rx, is_tx, is_fe1, is_fe3, links_count, links));
    BCMDNX_IF_ERR_EXIT(rc);
       
exit:
    SOC_DFE_MARK_WARMBOOT_DIRTY_BIT(unit);
    DFE_UNIT_LOCK_RELEASE(unit); 
    BCMDNX_FUNC_RETURN;  
     
}


/*
 * Function:
 *      bcm_fabric_link_threshold_retrieve
 * Purpose:
 *      Retrieve links which are attached to a fifo_type
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      fifo_type           - (IN)  fifo_type
 *      links_count_max     - (IN)  Max size of links
 *      links               - (OUT) Links array
 *      links_count         - (OUT) Actual size of links
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int         
bcm_dfe_fabric_link_thresholds_retrieve(
    int unit, 
    int fifo_type, 
    uint32     links_count_max,
    bcm_port_t* links, 
    uint32*     links_count)
{
    int fifo_type_index, rc = BCM_E_NONE;
    int is_rx, is_tx, is_fe1, is_fe3, is_primary, is_secondary, is_handle_flag, nof_pipes;
    soc_dcmn_fabric_pipe_t pipe;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(links);
    BCMDNX_NULL_CHECK(links_count);
    SOC_DCMN_FABRIC_PIPE_INIT(pipe);

    rc = bcm_dfe_fifo_type_get_handle_flag(unit, fifo_type, &is_handle_flag);
    BCMDNX_IF_ERR_EXIT(rc);

    nof_pipes = SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes;

    if (!is_handle_flag) /* direct attach - without fifo handle */
    {
        fifo_type_index = fifo_type;
        is_rx = 1;
        is_tx = 1;
        is_fe1 = 1;
        is_fe3 = 1;
        SOC_DCMN_FABRIC_PIPE_ALL_PIPES_SET(&pipe, nof_pipes);

    }
    else 
    {
        rc = bcm_dfe_fifo_type_get_id(unit, fifo_type, &fifo_type_index);
        BCMDNX_IF_ERR_EXIT(rc);
        DFE_FABRIC_FIFO_TYPE_INDEX_VALID_CHECK(fifo_type_index);
       
        /*get pipe*/
        rc =  bcm_dfe_fifo_type_is_primary(unit, fifo_type, &is_primary);
        BCMDNX_IF_ERR_EXIT(rc);
        rc =  bcm_dfe_fifo_type_is_secondary(unit, fifo_type, &is_secondary);
        BCMDNX_IF_ERR_EXIT(rc);

        if(is_primary)
        {
            SOC_DCMN_FABRIC_PIPE_SET(&pipe,0);
        }
        if(is_secondary)
        {
            SOC_DCMN_FABRIC_PIPE_SET(&pipe,1);
        }

        DFE_FABRIC_CONTROL_SOURCE_VALID_CHECK(pipe);
       
        rc = bcm_dfe_fifo_type_is_rx(unit, fifo_type, &is_rx);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_tx(unit, fifo_type, &is_tx);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe1(unit, fifo_type, &is_fe1);
        BCMDNX_IF_ERR_EXIT(rc);
        rc = bcm_dfe_fifo_type_is_fe3(unit, fifo_type, &is_fe3);
        BCMDNX_IF_ERR_EXIT(rc);
    }
    
    DFE_UNIT_LOCK_TAKE(unit);  
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_links_link_type_get,(unit, pipe, fifo_type_index, is_rx, is_tx, is_fe1, is_fe3, links_count_max, links, links_count));
    BCMDNX_IF_ERR_EXIT(rc);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_fabric_link_status_get
 * Purpose:
 *      Get status of the link
 * Parameters:
 *      unit                - (IN)  Unit number.
 *      link_id             - (IN)  Link number 
 *      link_status         - (OUT) Current status of the link 
 *      errored_token_count - (OUT) Low value, indicates bad link connectivity or link down, based on reachability cells 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_status_get(
    int unit, 
    bcm_port_t link_id, 
    uint32 *link_status, 
    uint32 *errored_token_count)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(link_status);
    BCMDNX_NULL_CHECK(errored_token_count);
    
    DFE_LINK_INPUT_CHECK(unit, link_id);
    
    DFE_UNIT_LOCK_TAKE(unit);
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_status_get,(unit, link_id, link_status, errored_token_count));
    BCMDNX_IF_ERR_EXIT(rc);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_fabric_link_connectivity_status_get
 * Purpose:
 *      Retrieves the current link-partner information of a link, for
 *      all existing links up to link_partner_max
 * Parameters:
 *      unit               - (IN)  Unit number.
 *      link_partner_max   - (IN)  Max size of link_partner_array. 
                                   If smaller than number of links the function returns with the error BCM_E_FULL
 *      link_partner_array - (OUT) Array of connectivity 
 *      link_partner_count - (OUT) Size of link_partner_array 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_connectivity_status_get(
    int unit, 
    int link_partner_max, 
    bcm_fabric_link_connectivity_t *link_partner_array, 
    int *link_partner_count)
{
    bcm_port_t link_id = 0;
    int rc = BCM_E_NONE;
    int max_link_id;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(link_partner_array);
    BCMDNX_NULL_CHECK(link_partner_count);
    
    DFE_UNIT_LOCK_TAKE(unit);
    
    *link_partner_count = 0;

    max_link_id = SOC_DFE_DEFS_GET(unit, nof_links);

    for(link_id = 0 ; link_id<max_link_id ;  link_id++)
    {
        if (!PBMP_MEMBER(PBMP_SFI_ALL(unit), link_id))
        {
            continue;
        }
        if(*link_partner_count >= link_partner_max)
        {
            *link_partner_count = 0;
            BCMDNX_ERR_EXIT_MSG(BCM_E_FULL, (_BSL_BCM_MSG("link_partner_array is too small"))); 
        }
          
        rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_connectivity_status_get,(unit, link_id, &(link_partner_array[*link_partner_count]))); 
        BCMDNX_IF_ERR_EXIT(rc);
        
        (*link_partner_count)++;
    }
    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}

/*
 * Function:
 *      bcm_dfe_fabric_link_connectivity_status_single_get
 * Purpose:
 *      Retrieves the current link-partner information of a link, for
 *      all existing links up to link_partner_max
 * Parameters:
 *      unit               - (IN)  Unit number.
 *      link_id            - (IN)  Link id
 *      link_partner_array - (OUT) link_id link partner info
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_link_connectivity_status_single_get(
    int unit, 
    bcm_port_t link_id, 
    bcm_fabric_link_connectivity_t *link_partner_info)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(link_partner_info);
    DFE_LINK_INPUT_CHECK(unit, link_id);
    
    DFE_UNIT_LOCK_TAKE(unit);

    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_link_connectivity_status_get,(unit, link_id, link_partner_info)); 
    BCMDNX_IF_ERR_EXIT(rc);
    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;  
}


/*
 * Function:
 *      bcm_dfe_fabric_reachability_status_get
 * Purpose:
 *      Retrieves the links through which a remote module ID is
 *      reachable
 * Parameters:
 *      unit        - (IN)  Unit number.
 *      moduleid    - (IN)  Module Id 
 *      links_max   - (IN)  Max size of links_array. If the size doesnt correlate to the devices number of links, 
                            the function returns with the error BCM_E_FULL
 *      links_array - (OUT) bitmap of the links from which moduleid is reachable 
 *      links_count - (OUT) Size of links_array 
 * Returns:
 *      BCM_E_xxx
 * Notes:
 */
int 
bcm_dfe_fabric_reachability_status_get(
    int unit, 
    int moduleid, 
    int links_max, 
    uint32 *links_array, 
    int *links_count)
{
    int rc = BCM_E_NONE;
    BCMDNX_INIT_FUNC_DEFS;
    
    BCMDNX_NULL_CHECK(links_array);
    BCMDNX_NULL_CHECK(links_count);
    
    if(!SOC_CHECK_MODULE_ID(moduleid))
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Illegal moduleid %d"),moduleid)); 
    }
    
    DFE_UNIT_LOCK_TAKE(unit); 
    rc = MBCM_DFE_DRIVER_CALL(unit,mbcm_dfe_fabric_reachability_status_get,(unit, moduleid, links_max, links_array, links_count));
    BCMDNX_IF_ERR_EXIT(rc);
    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN; 
}
/*
 * 
 * Function:
 *      bcm_dfe_fabric_route_rx
 * Purpose:
 *      Receive Source routed data cells
 * Parameters:
 *      unit                (IN) Unit number.
 *      flags               (IN) flags
 *      data_out_max_size   (IN) max "data_out" size
 *      data_out            (OUT) the received data.
 *      data_out_size       (OUT) "data_out" actual size.
 */
int 
bcm_dfe_fabric_route_rx(
    int unit, 
    uint32 flags, 
    uint32 data_out_max_size, 
    uint32 *data_out, 
    uint32 *data_out_size)
{
    BCMDNX_INIT_FUNC_DEFS;

    /*Verify*/
    BCMDNX_NULL_CHECK(data_out);
    BCMDNX_NULL_CHECK(data_out_size);
    if (data_out_max_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("buffer is too small")));
    }

    DFE_UNIT_LOCK_TAKE(unit);

    BCMDNX_IF_ERR_EXIT(soc_dfe_sr_cell_payload_receive(unit, flags, data_out_max_size, data_out_size, data_out));

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * 
 * Function:
 *      bcm_dfe_fabric_route_tx
 * Purpose:
 *      Send Source routed data cells
 * Parameters:
 *      unit         (IN) Unit number.
 *      flags        (IN) flags
 *      route        (IN) Specify the path for the generated cells.
 *      data_in_size (IN) "data_in" size (uint32 units).
 *      data_in      (IN) The data to send.
 */
int 
bcm_dfe_fabric_route_tx(
    int unit, 
    uint32 flags, 
    bcm_fabric_route_t *route, 
    uint32 data_in_size, 
    uint32 *data_in)
{
    soc_dcmn_fabric_route_t soc_route;
    BCMDNX_INIT_FUNC_DEFS;

    /*verify*/
    BCMDNX_NULL_CHECK(route);
    BCMDNX_NULL_CHECK(route->hop_ids);
    BCMDNX_NULL_CHECK(data_in);
    if (data_in_size == 0)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("no data to send")));
    }

    DFE_UNIT_LOCK_TAKE(unit);

    soc_route.pipe_id = route->pipe_id;
    soc_route.number_of_hops = route->number_of_hops;
    soc_route.hop_ids = route->hop_ids;
    BCMDNX_IF_ERR_EXIT(soc_dfe_sr_cell_send(unit, flags, &soc_route, data_in_size, data_in));

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/* 
 * Temporary internal files for lab validation
 * Reuired for FE3200 Tape-out
 */
/*
 * 
 * Function:
 *      _bcm_dfe_fabric_link_flow_status_cell_format_set
 * Purpose:
 *      Internal function - which setting the flow status cell format according to fap dest
 * Parameters:
 *      unit         (IN) Unit number.
 *      port         (IN) port #
 *      cell_format  (IN) BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V1 or BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V2
 */
int
_bcm_dfe_fabric_link_flow_status_cell_format_set(int unit, bcm_port_t port, int cell_format)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_flow_status_control_cell_format_set, (unit, port, (soc_dfe_fabric_link_cell_size_t) cell_format));
    BCMDNX_IF_ERR_EXIT(rv);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * 
 * Function:
 *      _bcm_dfe_fabric_link_flow_status_cell_format_set
 * Purpose:
 *      Internal function - which getting the flow status cell format according to fap dest
 * Parameters:
 *      unit         (IN) Unit number.
 *      port         (IN) port #
 *      cell_format  (IN) BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V1 or BCM_FABRIC_LINK_CELL_FORMAT_VSC256_V2
 */
int
_bcm_dfe_fabric_link_flow_status_cell_format_get(int unit, bcm_port_t port, int *cell_format)
{
    int rv;
    BCMDNX_INIT_FUNC_DEFS;

    rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_flow_status_control_cell_format_get, (unit, port, (soc_dfe_fabric_link_cell_size_t *)cell_format));
    BCMDNX_IF_ERR_EXIT(rv);

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_link_remote_pipe_mapping_get
 * Purpose:
 *      Set per link the mapping between the local pipe to the remote pipe.
 * Parameters:
 *      unit            (IN) Unit number.
 *      port            (IN) port #.
 *      mapping_config  (IN) pipe mapping configuration.
 */

int 
bcm_dfe_fabric_link_remote_pipe_mapping_get(
    int unit, 
    bcm_port_t port, 
    bcm_fabric_link_remote_pipe_mapping_t *mapping_config)
{
    int rv;
    soc_dfe_fabric_link_remote_pipe_mapping_t soc_mapping_config;
    int i;
    BCMDNX_INIT_FUNC_DEFS;

    /*Verify*/
    DFE_LINK_INPUT_CHECK(unit, port);

    if (SOC_DFE_IS_REPEATER(unit))
    {
        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_repeater_nof_remote_pipe_get, (unit, port, &mapping_config->num_of_remote_pipes));
        BCMDNX_IF_ERR_EXIT(rv);
    }
    else
    {
        sal_memset(&soc_mapping_config, 0, sizeof(soc_dfe_fabric_link_remote_pipe_mapping_t)); 

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pipe_map_get, (unit, port, &soc_mapping_config));
        BCMDNX_IF_ERR_EXIT(rv);

        if (soc_mapping_config.num_of_remote_pipes > mapping_config->remote_pipe_mapping_max_size)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Size of buffer should be bigger than the number of remote pipes")));
        }

        mapping_config->num_of_remote_pipes = soc_mapping_config.num_of_remote_pipes;
        for (i = 0; i <  mapping_config->num_of_remote_pipes; i++)
        {
            mapping_config->remote_pipe_mapping[i] = soc_mapping_config.remote_pipe_mapping[i]  + bcmFabricPipe0;
        }
    }
    
exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:
 *      bcm_dfe_fabric_link_remote_pipe_mapping_set
 * Purpose:
 *      Get per link the mapping between the local pipe to the remote pipe.
 * Parameters:
 *      unit            (IN) Unit number.
 *      port            (IN) port #.
 *      mapping_config  (OUT) pipe mapping configuration.
 */
int 
bcm_dfe_fabric_link_remote_pipe_mapping_set(
    int unit, 
    bcm_port_t port, 
    bcm_fabric_link_remote_pipe_mapping_t *mapping_config)
{
    int rv;
    soc_dfe_fabric_link_remote_pipe_mapping_t soc_mapping_config;
    int i;
    BCMDNX_INIT_FUNC_DEFS;

    if (SOC_DFE_IS_REPEATER(unit))
    {
        /*Verify*/
        if (mapping_config->num_of_remote_pipes > SOC_DFE_MAX_NOF_PIPES)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of remote pipes (%d), is not supported, link %d.\n"), mapping_config->num_of_remote_pipes, port));
        }

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_repeater_nof_remote_pipe_set, (unit, port, mapping_config->num_of_remote_pipes));
        BCMDNX_IF_ERR_EXIT(rv);

    } 
    else 
    {
        /*Verify*/
        if (mapping_config->num_of_remote_pipes > SOC_DFE_MAX_NOF_PIPES)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Number of remote pipes (%d), is not supported, link %d.\n"), mapping_config->num_of_remote_pipes, port));
        }

        if (mapping_config->num_of_remote_pipes > mapping_config->remote_pipe_mapping_max_size)
        {
            BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Buffer max size is too small relative to number of remote pipes (%d), link %d.\n"), mapping_config->num_of_remote_pipes, port));
        }
        for (i = 0; i < mapping_config->num_of_remote_pipes; i++)
        {
            if ((mapping_config->remote_pipe_mapping[i] - bcmFabricPipe0) < 0 ||
                 (mapping_config->remote_pipe_mapping[i] - bcmFabricPipe0) >= SOC_DFE_FABRIC_PIPES_CONFIG(unit).nof_pipes)
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("invalid local pipe (%d), link %d"), (mapping_config->remote_pipe_mapping[i] - bcmFabricPipe0), port));
            }
        }
        DFE_LINK_INPUT_CHECK(unit, port);

        sal_memset(&soc_mapping_config, 0, sizeof(soc_dfe_fabric_link_remote_pipe_mapping_t));

        soc_mapping_config.num_of_remote_pipes = mapping_config->num_of_remote_pipes;
        for (i = 0; i <mapping_config->num_of_remote_pipes; i++)
        {
            soc_mapping_config.remote_pipe_mapping[i] = mapping_config->remote_pipe_mapping[i] - bcmFabricPipe0;
        }

        rv = MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_fabric_links_pipe_map_set, (unit, port, soc_mapping_config));
        BCMDNX_IF_ERR_EXIT(rv);
    }

exit: 
    DFE_UNIT_LOCK_RELEASE(unit);
    BCMDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

