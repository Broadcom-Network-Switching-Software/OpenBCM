/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TDM
#define SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_0_BIT    (0x1)
#define SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_1_BIT    (0x2)
#define SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_2_BIT    (0x4)
#define SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_LOCAL_0_BIT   (0x8)
#define SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_LOCAL_1_BIT   (0x10)
#define IRE_TDM_MESH_MC_BITS_MAX 5

#include <soc/dcmn/error.h>

#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_tdm.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/JER/jer_mgmt.h>


 
int jer_tdm_ingress_failover_set(int unit, bcm_pbmp_t tdm_enable_pbmp)
{
    soc_reg_above_64_val_t reg_above_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    for (i=0; i <= (SOC_REG_INFO(unit, IRE_TDM_CONTEXT_DROPr).fields[0].len-1)/32; i++) {
        reg_above_64[i] = 0xFFFFFFFF - tdm_enable_pbmp.pbits[i];
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IRE_TDM_CONTEXT_DROPr, REG_PORT_ANY, 0, reg_above_64));

exit:
    SOCDNX_FUNC_RETURN;
}

 
int jer_tdm_ingress_failover_get(int unit, bcm_pbmp_t *tdm_enable_pbmp)
{
    soc_reg_above_64_val_t reg_above_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IRE_TDM_CONTEXT_DROPr, REG_PORT_ANY, 0, reg_above_64));

    for (i=0; i <= (SOC_REG_INFO(unit, IRE_TDM_CONTEXT_DROPr).fields[0].len-1)/32; i++) {
        tdm_enable_pbmp->pbits[i] = 0xFFFFFFFF - reg_above_64[i];
    }

exit:
    SOCDNX_FUNC_RETURN;
}



int
  jer_tdm_init(int unit)
{
    uint32
        fld_val,
        tdm_found,
        tdm_egress_priority,
        tdm_egress_dp,
        tm_port;
    ARAD_MGMT_TDM_MODE
        tdm_mode;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE
        ilkn_tdm_dedicated_queuing;
        uint8
        is_local;
    soc_reg_above_64_val_t
        data;
    int fabric_priority;
    int core, port_i;
    uint32 is_valid;
    SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_REG_ABOVE_64_CLEAR(data);

    SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_tdm_unit_has_tdm(unit,&tdm_found)));
    tdm_mode = SOC_DPP_CONFIG(unit)->arad->init.tdm_mode;
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;  
    tdm_egress_priority = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_priority;
    tdm_egress_dp = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_dp;
    fabric_priority = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;

    arad_sw_db_tdm_mode_set(unit, tdm_mode);
    arad_sw_db_ilkn_tdm_dedicated_queuing_set(unit, ilkn_tdm_dedicated_queuing);

    
    if (!SOC_IS_QAX(unit)) {
        fld_val = ARAD_TDM_CELL_SIZE_MIN;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MIN_SIZEf,  fld_val));

        fld_val = ARAD_TDM_CELL_SIZE_MAX;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_SIZEr, REG_PORT_ANY, 0, TDM_MAX_SIZEf,  fld_val));    
    }
    else{ 
        if (!soc_feature(unit, soc_feature_no_tdm)) {
            fld_val = ARAD_TDM_CELL_SIZE_MIN;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_CONFIGURATIONSr, REG_PORT_ANY, 0, TDM_MIN_SIZEf,  fld_val));

            fld_val = ARAD_TDM_CELL_SIZE_MAX;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_CONFIGURATIONSr, REG_PORT_ANY, 0, TDM_MAX_SIZEf,  fld_val));    
        }
        SOC_REG_ABOVE_64_CLEAR(data); 
        soc_reg_above_64_field32_set(unit, EPNI_REG_0303r, data, FIELD_11_11f, 1);
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, EPNI_REG_0303r, REG_PORT_ANY, 0, data));
    }
    
    fld_val = ARAD_TDM_VERSION_ID;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FTMH_VERSIONf, fld_val));

    
    if (!SOC_IS_QAX(unit)) {
        
        fld_val = 0x1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPT_IPT_ENABLESr, REG_PORT_ANY, 0, TDM_ENf,  fld_val));
    }

    
    fld_val = ARAD_TDM_PUSH_QUEUE_TYPE;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPEf,  fld_val));

    fld_val = 0x1;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IPS_PUSH_QUEUE_TYPES_CONFIGr, SOC_CORE_ALL, 0, PUSH_QUEUE_TYPE_ENf,  fld_val));

    
    if (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_STA)
    {
        is_local = (SOC_DPP_CONFIG(unit)->arad->init.fabric.connect_mode == ARAD_FABRIC_CONNECT_MODE_SINGLE_FAP)? TRUE:FALSE;

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, FORCE_ALL_LOCALf,  is_local));
    }

    if (!soc_feature(unit, soc_feature_no_fabric)) {
        
        
        fld_val = 0x2;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_LINK_BITMAP_CONFIGURATIONr, REG_PORT_ANY, 0, IRE_TDM_MASK_MODEf,  fld_val));

        
        fld_val = 1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, TDM_FMC_ENf,  fld_val));

        
        fld_val = (SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_mesh_multicast_enable != 2) ? 1 : 0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_FDT_ENABLER_REGISTERr, REG_PORT_ANY, 0, TDM_MESH_MC_BMP_SRCf,  fld_val));

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDA_FDA_ENABLERSr, REG_PORT_ANY, 0, TDM_HEADER_PRIORITYf,  fabric_priority));
    }
    
    if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
        
        fld_val = 0x1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
        if (SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_2_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_2_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_3_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_ILAKEN_3_INTERLEAVE_ENf,  fld_val));
        }
    }

      
    if (tdm_found) 
    {
        
        fld_val = 0x1;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, EGRESS_TDM_MODEf,  fld_val));

        
        fld_val = (tdm_mode == ARAD_MGMT_TDM_MODE_TDM_OPT)?0x1:0x0;
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_2_BYTES_FTMHf,  fld_val));  
        if (!SOC_IS_QAX(unit)) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, FDT_TDM_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_FTMH_OPTIMIZEDf,  fld_val));
        }
        else{
        	if (!soc_feature(unit, soc_feature_no_tdm)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, IRE_TDM_CONFIGURATIONSr, REG_PORT_ANY, 0, TDM_FTMH_OPTIMIZEDf,  fld_val));
            }
        }
    }

    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, SOC_CORE_ALL, 0, TDM_CONTEXT_MODEf, tdm_found));

    
    fld_val = 0;
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_EGRESS_REPLICATION_GENERAL_CONFIGr, SOC_CORE_ALL, 0, TDM_REP_FORMAT_ENf,  fld_val));

    
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_TCf,  tdm_egress_priority));
    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, REG_PORT_ANY, 0, TDM_PKT_DPf,  tdm_egress_dp));

    
    SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);

    for (port_i = 0; port_i < SOC_MAX_NUM_PORTS; ++port_i) {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port_i, &is_valid));
        if (!is_valid) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));

        if (IS_TDM_PORT(unit, port_i)) {
            SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_egr_ofp_scheduling_get(unit, core, tm_port, &ofp_sch_info)));
            ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;
            SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_egr_ofp_scheduling_set(unit, core, tm_port, &ofp_sch_info)));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

STATIC CONST int soc_jer_fabric_ire_mesh_multicast_replication_dests[] = {
    SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_0_BIT,
    SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_1_BIT,
    SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_DEST_2_BIT
};

STATIC uint32 
soc_jer_fabric_multicast_rep_bitmap_convert(
   SOC_SAND_IN  uint32                         unit, 
   SOC_SAND_IN  uint32                         destid_count,
   SOC_SAND_IN  soc_module_t                  *destid_array,
   SOC_SAND_OUT uint32                        *mc_rep
   ){
    
    soc_module_t remote_dest;
    int i, max_external_faps;
    uint32 my_core0_fap_id, my_core1_fap_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(jer_mgmt_system_fap_id_get(unit, &my_core0_fap_id));
    my_core1_fap_id = my_core0_fap_id + ((SOC_DPP_DEFS_GET(unit, nof_cores) == 2) ? 1 : 0);

    max_external_faps = (IRE_TDM_MESH_MC_BITS_MAX - SOC_DPP_DEFS_GET(unit, nof_cores)); 

    *mc_rep =0;
    for (i = 0; i < destid_count; ++i) {
        if (destid_array[i] == my_core0_fap_id){
            *mc_rep |= SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_LOCAL_0_BIT;
        } else if (destid_array[i] == my_core1_fap_id){
            *mc_rep |= SOC_JER_FABRIC_TDM_MESH_MC_REPLICATION_LOCAL_1_BIT;
        } else { 
            
            remote_dest = SOC_DPP_FABRIC_GROUP_MODID_UNSET(destid_array[i]);
            if (remote_dest >= 0 && remote_dest < max_external_faps){ 
                *mc_rep |= soc_jer_fabric_ire_mesh_multicast_replication_dests[remote_dest];
            }
            else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("dest %d is invalid"),destid_array[i]));
            }
        }
    }

    exit:
      SOCDNX_FUNC_RETURN;
}


int
jer_mesh_tdm_multicast_set(
   int                            unit, 
   soc_port_t                     port,
   uint32                         flags, 
   uint32                         destid_count,
   soc_module_t                   *destid_array
   ){
    ARAD_IRE_TDM_CONFIG_TBL_DATA arad_ire_tdm_config_tbl_data;
    uint32 mc_rep=0;
    uint32 reassembly_context;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOC_SAND_IF_ERR_EXIT(arad_tdm_local_to_reassembly_context_get(unit, port, &reassembly_context));

    SOC_SAND_IF_ERR_EXIT(arad_ire_tdm_config_tbl_get_unsafe(unit, reassembly_context, &arad_ire_tdm_config_tbl_data ));
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_multicast_rep_bitmap_convert(unit, destid_count, destid_array, &mc_rep));
    arad_ire_tdm_config_tbl_data.mc_replication = mc_rep; 
    SOC_SAND_IF_ERR_EXIT(arad_ire_tdm_config_tbl_set_unsafe(unit, reassembly_context, &arad_ire_tdm_config_tbl_data));
        
    exit:
        SOCDNX_FUNC_RETURN;
}

