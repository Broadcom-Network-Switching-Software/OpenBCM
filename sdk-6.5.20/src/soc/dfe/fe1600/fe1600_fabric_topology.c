/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE1600 FABRIC TOPOLOGY
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/fabric.h>
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

#define LINKS_ARRAY_SIZE (SOC_DFE_MAX_NOF_LINKS/32 + (SOC_DFE_MAX_NOF_LINKS%32? 1:0) + 1 )


soc_error_t 
soc_fe1600_fabric_topology_isolate_set(int unit, soc_dcmn_isolation_status_t val)
{
    uint32 reg_val32;

    SOCDNX_INIT_FUNC_DEFS;
    SOC_FE1600_ONLY(unit);

    if(soc_dcmn_isolation_status_active == val) {

        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_mesh_topology_reset(unit));

        sal_usleep(2000);
                   
        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_rmgr_set(unit, 1));
    } else {
        uint64 reg64_val, reg64_ones;
        COMPILER_64_ZERO(reg64_val);

        
        SOCDNX_IF_ERR_EXIT(READ_RTP_REG_0063r(unit, &reg_val32));
            
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0063r(unit, 0x0));
                
        
        SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_rmgr_set(unit, 0));

        
        sal_usleep(30000);

        
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0063r(unit, reg_val32));
        
        
        SOCDNX_IF_ERR_EXIT(READ_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, &reg64_val));
        COMPILER_64_ALLONES(reg64_ones);
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_ones));
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg64_val));
    }

    sal_usleep(20000);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_topology_reachability_mask_set(int unit, soc_pbmp_t active_links, soc_dcmn_isolation_status_t val)
{
    soc_port_t port;
    soc_reg_above_64_val_t reg_val;
    uint32 reg32_val = 0;
    SOCDNX_INIT_FUNC_DEFS;
	SOC_FE1600_ONLY(unit);
    
    if (val == soc_dcmn_isolation_status_isolated)
    {
        
        SOCDNX_IF_ERR_EXIT(READ_RTP_REG_0063r(unit, &reg32_val));

            
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0063r(unit, 0x0));
    }

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
    
    SOC_PBMP_ITER(active_links, port) {
        if (val == soc_dcmn_isolation_status_isolated) {
            
            SHR_BITCLR(reg_val, port);
        } else {
            
            SHR_BITSET(reg_val, port);
        }
    }
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_ALLOWED_LINKS_REGISTERr(unit, reg_val));
    
    if (val == soc_dcmn_isolation_status_isolated)
    {
        
        sal_usleep(30000);

        
        SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0063r(unit, reg32_val));
    }


exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_fe1600_fabric_topology_isolate_get(int unit, soc_dcmn_isolation_status_t* val)
{
    uint64 reg_val64, value;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit,&reg_val64));
      
    value = soc_reg64_field_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, RMGRf); 
    if(COMPILER_64_IS_ZERO(value))
        *val = soc_dcmn_isolation_status_isolated;
    else
        *val = soc_dcmn_isolation_status_active;
      
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_link_topology_set(int unit, soc_module_t destination_local_id, int links_count, soc_port_t *links_array)
{
    int i;
	int nof_rtp_mclbt_instances,max_link_score;
    uint32 bmp[LINKS_ARRAY_SIZE];
    uint32 totsf_val, slsct_val, score, sctinc_val, sctinc, last_score = 0;
    soc_port_t link;
    soc_field_t scrub_en;
    uint64 reg_val64;
    int nof_links;
    SOCDNX_INIT_FUNC_DEFS;
    
        
    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    if( (soc_dfe_load_balancing_mode_normal != SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) && (links_count > nof_links/2) ) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("links_count > 64 is allowed only in normal load balancing mode")));
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    scrub_en = soc_reg64_field32_get(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, reg_val64, SCT_SCRUB_ENABLEf);  
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, 0);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

    
    totsf_val = 0;
    soc_mem_field_set(unit, RTP_TOTSFm, &totsf_val, LINK_NUMf, (uint32*)&links_count);
    
    
    bmp[0] = bmp[1] = bmp[2] = bmp[3] = bmp[4] = 0;
    
    for(i = 0 ; i<links_count ; i++)
    {
        link = links_array[i];
        if (link != SOC_PORT_INVALID) 
        {
            SHR_BITSET(bmp,link);
        }   
    }
    
    
    nof_rtp_mclbt_instances = SOC_DFE_DEFS_GET(unit, nof_rtp_mclbtp_instances); 
	for (i=0;i<nof_rtp_mclbt_instances;i++)
	{
		SOCDNX_IF_ERR_EXIT(WRITE_RTP_MCLBTPm(unit,i,MEM_BLOCK_ALL,destination_local_id,bmp));
	}
    
    
    nof_rtp_mclbt_instances = SOC_DFE_DEFS_GET(unit, nof_rtp_mclbts_instances); 
	for (i=0;i<nof_rtp_mclbt_instances;i++)
	{
		SOCDNX_IF_ERR_EXIT(WRITE_RTP_MCLBTSm(unit,i,MEM_BLOCK_ALL,destination_local_id,bmp));
	}
    
    
    for(i = 0 ; i<links_count ; i++)
    {

        link = links_array[i];
        if (link != SOC_PORT_INVALID) 
        {
            
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_RCGLBTm(unit, MEM_BLOCK_ALL, link, bmp));
            
            
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_TOTSFm(unit, MEM_BLOCK_ALL, link, &totsf_val));
            
            
            slsct_val = 0;
            max_link_score = SOC_DFE_DEFS_GET(unit, max_link_score);
            if(soc_dfe_load_balancing_mode_destination_unreachable == SOC_DFE_CONFIG(unit).fabric_load_balancing_mode) {
                score = max_link_score ;  
            } else {
                score = (max_link_score* (i + 1)) / links_count;    
            }
            soc_mem_field_set(unit, RTP_SLSCTm, &slsct_val, LINK_NUMf, &score);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_SLSCTm(unit, MEM_BLOCK_ALL, link, &slsct_val));
    
            
            sctinc_val = 0;
            sctinc = score-last_score;
            last_score=score;
            soc_mem_field_set(unit, RTP_SCTINCm, &sctinc_val, LINK_NUMf, &sctinc);
            SOCDNX_IF_ERR_EXIT(WRITE_RTP_SCTINCm(unit, MEM_BLOCK_ALL, link, &sctinc_val));
        }
    }
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, &reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, SCT_SCRUB_ENABLEf, scrub_en);                       
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_link_topology_get(int unit, soc_module_t destination_local_id, int links_count_max, int *links_count, soc_port_t *links_array)
{
    uint32 bmp[5];
    uint32 totsf_val;
    int i, link_index;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_MCLBTPm(unit, 0, MEM_BLOCK_ANY, destination_local_id, bmp));
    *links_count = 0;
    
    PBMP_SFI_ITER(unit, i)
    {      
        if(SHR_BITGET(bmp,i)) 
        {
            SOCDNX_IF_ERR_EXIT(READ_RTP_TOTSFm(unit, MEM_BLOCK_ALL, i, &totsf_val));
            soc_mem_field_get(unit, RTP_TOTSFm, &totsf_val, LINK_NUMf, (uint32*)links_count);
            break;
        }
    }

    if ( (*links_count) > links_count_max)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_FULL, (_BSL_SOCDNX_MSG("links_array is too small")));
    }

    link_index=0;
    PBMP_SFI_ITER(unit, i)
    {
        if(SHR_BITGET(bmp,i))
        {
            links_array[link_index] = i;
            (link_index)++;
        }
    }
    
    for ( ; link_index < (*links_count) ; link_index++)
    {
        links_array[link_index] = -1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC soc_error_t 
soc_fe1600_fabric_topology_repeater_destination_dcmc_set(int unit, soc_port_t source, soc_port_t destination)
{
    uint32 reg_val, mask,mask_bits=0;
    int inner_lnk, reg_select;
	int nof_links, nof_bits_per_link;
    soc_port_t source_mask;
    SOCDNX_INIT_FUNC_DEFS;
    

        
    reg_select = INT_DEVIDE(destination,4);
  
    switch(reg_select)
    {
        case 0:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_0r(unit,&reg_val));
            break;
        case 1:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_1r(unit,&reg_val));
            break;
        case 2:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_2r(unit,&reg_val));
            break;
        case 3:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_3r(unit,&reg_val));
            break;
        case 4:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_4r(unit,&reg_val));
            break;
        case 5:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_5r(unit,&reg_val));
            break;
        case 6:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_6r(unit,&reg_val));
            break;
        case 7:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_7r(unit,&reg_val));
            break;
        case 8:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_8r(unit,&reg_val));
            break;
        case 9:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_9r(unit,&reg_val));
            break;
        case 10:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_10r(unit,&reg_val));
            break;
        case 11:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_11r(unit,&reg_val));
            break;
        case 12:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_12r(unit,&reg_val));
            break;
        case 13:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_13r(unit,&reg_val));
            break;
        case 14:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_14r(unit,&reg_val));
            break;
        case 15:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_15r(unit,&reg_val));
            break;
        case 16:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_16r(unit,&reg_val));
            break;
        case 17:      
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_17r(unit,&reg_val));
            break;
        case 18:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_18r(unit,&reg_val));
            break;
        case 19:      
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_19r(unit,&reg_val));
            break;
        case 20:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_20r(unit,&reg_val));
            break;
        case 21:      
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_21r(unit,&reg_val));
            break;
        case 22:      
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_22r(unit,&reg_val));
            break;
        case 23:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_23r(unit,&reg_val));
            break;
        case 24:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_24r(unit,&reg_val));
            break;
        case 25:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_25r(unit,&reg_val));
            break;
        case 26:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_26r(unit,&reg_val));
            break;
        case 27:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_27r(unit,&reg_val));
            break;
        case 28:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_28r(unit,&reg_val));
            break;
        case 29:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_29r(unit,&reg_val));
            break;
        case 30:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_30r(unit,&reg_val));
            break;
        case 31:
            SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_31r(unit,&reg_val));
            break;
#ifdef BCM_88950_A0
		case 32:
			SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_32r(unit,&reg_val));
			break;
		case 33:
			SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_33r(unit,&reg_val));
			break;
		case 34:
			SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_34r(unit,&reg_val));
			break;
		case 35:
			SOCDNX_IF_ERR_EXIT(READ_DCMC_REPEATER_ROUTING_TABLE_35r(unit,&reg_val));
			break;
#endif
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Can't find register for destination %d"),destination));
    }
    
    nof_links =  SOC_DFE_DEFS_GET(unit, nof_links);
	nof_bits_per_link=soc_fe1600_log2_round_up(nof_links); 
	SHR_BITSET_RANGE(&mask_bits,0,nof_bits_per_link);
    inner_lnk = destination % 4;  
    mask = ~(mask_bits << (nof_bits_per_link*inner_lnk));
    source_mask = source << (nof_bits_per_link*inner_lnk);
    reg_val &= mask;
    reg_val |= source_mask; 
    
    switch(reg_select)
    {
        case 0:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_0r(unit,reg_val));
            break;
        case 1:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_1r(unit,reg_val));
            break;
        case 2:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_2r(unit,reg_val));
            break;
        case 3:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_3r(unit,reg_val));
            break;
        case 4:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_4r(unit,reg_val));
            break;
        case 5:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_5r(unit,reg_val));
            break;
        case 6:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_6r(unit,reg_val));
            break;
        case 7:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_7r(unit,reg_val));
            break;
        case 8:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_8r(unit,reg_val));
            break;
        case 9:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_9r(unit,reg_val));
            break;
        case 10:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_10r(unit,reg_val));
            break;
        case 11:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_11r(unit,reg_val));
            break;
        case 12:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_12r(unit,reg_val));
            break;
        case 13:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_13r(unit,reg_val));
            break;
        case 14:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_14r(unit,reg_val));
            break;
        case 15:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_15r(unit,reg_val));
            break;
        case 16:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_16r(unit,reg_val));
            break;
        case 17:      
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_17r(unit,reg_val));
            break;
        case 18:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_18r(unit,reg_val));
            break;
        case 19:      
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_19r(unit,reg_val));
            break;
        case 20:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_20r(unit,reg_val));
            break;
        case 21:      
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_21r(unit,reg_val));
            break;
        case 22:      
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_22r(unit,reg_val));
            break;
        case 23:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_23r(unit,reg_val));
            break;
        case 24:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_24r(unit,reg_val));
            break;
        case 25:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_25r(unit,reg_val));
            break;
        case 26:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_26r(unit,reg_val));
            break;
        case 27:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_27r(unit,reg_val));
            break;
        case 28:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_28r(unit,reg_val));
            break;
        case 29:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_29r(unit,reg_val));
            break;
        case 30:
            SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_30r(unit,reg_val));
            break;
		case 31:
			SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_31r(unit,reg_val));
			break;
#ifdef BCM_88950_A0
		case 32:
			SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_32r(unit,reg_val));
			break;
		case 33:
			SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_33r(unit,reg_val));
			break;
		case 34:
			SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_34r(unit,reg_val));
			break;
		case 35:
			SOCDNX_IF_ERR_EXIT(WRITE_DCMC_REPEATER_ROUTING_TABLE_35r(unit,reg_val));
			break;
#endif
    }
    
exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_fe1600_fabric_topology_repeater_destination_set(int unit, soc_port_t source, soc_port_t destination)
{

    uint32 rtp_bm_raw[LINKS_ARRAY_SIZE];
    uint32 rtp_reg_val[LINKS_ARRAY_SIZE];
    uint32 hi,lo;
    uint64 reg_val64;
    int n;
	int rtp_rmhmt_group_size;
    SOCDNX_INIT_FUNC_DEFS;

    
    
    
    
    SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_repeater_destination_dcmc_set(unit, source, destination));
    SOCDNX_IF_ERR_EXIT(soc_fe1600_fabric_topology_repeater_destination_dcmc_set(unit, destination, source));

 
    
    
    
    
    

    hi = 0;
    lo = 0;
  
    rtp_bm_raw[0] = rtp_bm_raw[1] = rtp_bm_raw[2] = rtp_bm_raw[3] = rtp_bm_raw[4] = 0;
    SHR_BITSET(rtp_bm_raw, destination);
    SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, source, rtp_reg_val));
    soc_mem_field_set(unit, RTP_RMHMTm, rtp_reg_val, LINK_BIT_MAPf, rtp_bm_raw);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit,MEM_BLOCK_ALL,source, rtp_reg_val));

    rtp_rmhmt_group_size =  SOC_DFE_DEFS_GET(unit, rtp_rmhmt_group_size);
    
    n = INT_DEVIDE(source, rtp_rmhmt_group_size);
    if(n<32) {
        SHR_BITSET(&lo,n);
    } else {
        SHR_BITSET(&hi,n-32);
    }

    rtp_bm_raw[0] = rtp_bm_raw[1] = rtp_bm_raw[2] = rtp_bm_raw[3] = rtp_bm_raw[4] = 0;
    SHR_BITSET(rtp_bm_raw, source);
    SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, destination, rtp_reg_val));
    soc_mem_field_set(unit, RTP_RMHMTm, rtp_reg_val, LINK_BIT_MAPf, rtp_bm_raw);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_RMHMTm(unit,MEM_BLOCK_ALL,destination, rtp_reg_val));

    
    COMPILER_64_SET(reg_val64, 0, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg_val64));
    
    n = INT_DEVIDE(destination, rtp_rmhmt_group_size);
    if(n<32) {
        SHR_BITSET(&lo,n);
    } else {
        SHR_BITSET(&hi,n-32);
    }

    COMPILER_64_SET(reg_val64, hi, lo);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_UNICAST_ROUTE_UPDATE_DIRTYr(unit, reg_val64));
    
exit:
    SOCDNX_FUNC_RETURN;
} 


soc_error_t 
soc_fe1600_fabric_topology_repeater_destination_get(int unit, soc_port_t source, soc_port_t* destination)
{
    uint32 rtp_reg_val[LINKS_ARRAY_SIZE];
    int i,nof_links;
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(READ_RTP_RMHMTm(unit, MEM_BLOCK_ANY, source, rtp_reg_val));
    nof_links = SOC_DFE_DEFS_GET(unit, nof_links);
    
    for(i = 0 ; i< nof_links ; i++)
    {
        
        if (!IS_SFI_PORT(unit, i)) {
            continue;
        }

        if(SHR_BITGET(rtp_reg_val,i))
        {
            *destination = i;
            SOC_EXIT;
        }
    } 
    
    SOCDNX_EXIT_WITH_ERR(SOC_E_NOT_FOUND, (_BSL_SOCDNX_MSG("source %d, repeater destination wasn't found"),source));

exit:
    SOCDNX_FUNC_RETURN;  
  
}


soc_error_t
soc_fe1600_fabric_topology_nof_links_to_min_nof_links_default(int unit, int nof_links, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_fe1600_fabric_topology_min_nof_links_set(int unit, int min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    if (min_nof_links != 0)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("FE1600 do not support min number of links feature")));
    }
exit:
        SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_fe1600_fabric_topology_min_nof_links_get(int unit, int *min_nof_links)
{
    SOCDNX_INIT_FUNC_DEFS;

    
    *min_nof_links = 0;

    SOCDNX_FUNC_RETURN; 
}


soc_error_t
soc_fe1600_fabric_topology_rmgr_set(int unit, int enable)
{
    uint64 reg_val64;
    uint32 core_clock_speed;
    uint32 rmgr;
    uint32 rmgr_units_total;
    uint32 rmgr_at_core_clock_steps;
	int rate_per_link;
    SOCDNX_INIT_FUNC_DEFS;


    
    if (enable)
    {
        core_clock_speed = SOC_DFE_CONFIG(unit).core_clock_speed;
        rmgr_units_total = SOC_DFE_DEFS_GET(unit, rtp_rmgr_nof_links) * SOC_DFE_DEFS_GET(unit, rtp_rmgr_units);
        rate_per_link = SOC_DFE_IMP_DEFS_GET(unit, rtp_reachabilty_gen_rate_per_link);

        rmgr_at_core_clock_steps= ((rate_per_link / 1000) * core_clock_speed )/ 1000;
        rmgr = rmgr_at_core_clock_steps / rmgr_units_total;
    } else {
        rmgr = 0;
    }

    SOCDNX_IF_ERR_EXIT(READ_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit,&reg_val64));
    soc_reg64_field32_set(unit, RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr, &reg_val64, RMGRf, rmgr);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REACHABILITY_MESSAGE_GENERATOR_CONFIGURATIONr_REG64(unit,reg_val64));

exit:
    SOCDNX_FUNC_RETURN; 
}



int soc_fe1600_log2_round_up(uint32 x)
{
	uint32 bitmask=1<<31;
	int leftmost_one_digit=-1; 
	int nof_ones=0; 
	int counter=31;
	if (x==0) 
	{
		return -1;
	}
	while (x!=0 && nof_ones<=1)
	{
		if (x&bitmask)
		{
			if (leftmost_one_digit==-1)
			{
				leftmost_one_digit=counter;
			}
			nof_ones++;
		}
		x=x<<1;
		counter--;
	}
	if (nof_ones>1) leftmost_one_digit++;
	return leftmost_one_digit;


}

soc_error_t 
soc_fe1600_fabric_topology_mesh_topology_reset(int unit)
{
    uint32 reg_val32;
    int ccs_index,nof_instances_ccs;
    SOCDNX_INIT_FUNC_DEFS;

    nof_instances_ccs = SOC_DFE_DEFS_GET(unit, nof_instances_ccs);
    
    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++)
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_REG_0126r(unit, ccs_index, &reg_val32));
        soc_reg_field_set(unit, CCS_REG_0126r, &reg_val32, FIELD_1_1f, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0126r(unit, ccs_index, reg_val32));
    }

    sal_usleep(1000);

     
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    
    SOCDNX_IF_ERR_EXIT(READ_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, &reg_val32));
    soc_reg_field_set(unit, MESH_TOPOLOGY_MESH_TOPOLOGYr, &reg_val32, RESERVED_5f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_MESH_TOPOLOGY_MESH_TOPOLOGYr(unit, reg_val32));
    

    for (ccs_index = 0; ccs_index < nof_instances_ccs ; ccs_index++) 
    {
        SOCDNX_IF_ERR_EXIT(READ_CCS_REG_0126r(unit, ccs_index, &reg_val32));
        soc_reg_field_set(unit, CCS_REG_0126r, &reg_val32, FIELD_1_1f, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_CCS_REG_0126r(unit, ccs_index, reg_val32));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe1600_fabric_topology_all_reachable_integrity_update(int unit)
{
    int rv, i;
    int size;
    int index_max = 0, index_min = 0;
    int mem_flags = 0;
    int entry_words;
    uint32 reg_val32;
    uint32 *entry_array = NULL, *entry = NULL;
    soc_reg_above_64_val_t intergrity_vec, table_entry;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(intergrity_vec);
    SOCDNX_IF_ERR_EXIT(READ_RTP_LINK_INTEGRITY_VECTORr(unit, intergrity_vec));

    index_max = soc_mem_index_max(unit, RTP_RMHMTm);
    index_min = soc_mem_index_min(unit, RTP_RMHMTm);

    
    size = SOC_MEM_TABLE_BYTES(unit, RTP_RMHMTm);
    
    entry_array = soc_cm_salloc(unit, size, "RTP_RMHMTm");

    if (entry_array == NULL) {
         SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("failed to allocate memory")));
    }

    rv = soc_mem_array_read_range(unit, RTP_RMHMTm, 0 , MEM_BLOCK_ANY, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);

    entry_words = BYTES2WORDS(soc_mem_entry_bytes(unit, RTP_RMHMTm));
    for (i = index_min ; i < index_max; i++)
    {
        entry = entry_array + (entry_words * i);
        SOC_REG_ABOVE_64_CLEAR(table_entry);
        soc_mem_field_get(unit, RTP_RMHMTm, entry, LINK_BIT_MAPf, table_entry);
        SOC_REG_ABOVE_64_AND(table_entry,intergrity_vec);
        soc_mem_field_set(unit, RTP_RMHMTm, entry, LINK_BIT_MAPf, table_entry);
    }

    rv = soc_mem_array_write_range(unit, mem_flags, RTP_RMHMTm, 0, MEM_BLOCK_ALL, index_min, index_max, entry_array);
    SOCDNX_IF_ERR_EXIT(rv);


    
    SOCDNX_IF_ERR_EXIT(READ_RTP_REG_0069r(unit, &reg_val32));
    soc_reg_field_set(unit, RTP_REG_0069r, &reg_val32, FIELD_9_9f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0069r(unit, reg_val32));
    sal_usleep(1000);
    soc_reg_field_set(unit, RTP_REG_0069r, &reg_val32, FIELD_9_9f, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0069r(unit, reg_val32));
    sal_usleep(10000);
    soc_reg_field_set(unit, RTP_REG_0069r, &reg_val32, FIELD_9_9f, 0);
    SOCDNX_IF_ERR_EXIT(WRITE_RTP_REG_0069r(unit, reg_val32));

exit:

    if (entry_array != NULL) {
        soc_cm_sfree(unit, entry_array);
    }

    SOCDNX_FUNC_RETURN;
}

#endif 

#undef _ERR_MSG_MODULE_NAME

