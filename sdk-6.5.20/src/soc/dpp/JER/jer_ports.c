/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_ports.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

#define SOC_JER_INVALID_CTXT_ADDRESS 0x3

#define SOC_JER_ILKN_NIF_PORT_ILKN0  38
#define SOC_JER_ILKN_NIF_PORT_ILKN1  42
#define SOC_JER_ILKN_NIF_PORT_ILKN2  46
#define SOC_JER_ILKN_NIF_PORT_ILKN3  50
#define SOC_JER_ILKN_NIF_PORT_ILKN4  98
#define SOC_JER_ILKN_NIF_PORT_ILKN5  102

#define SOC_JER_PORT_NO_LANE_SWAP   0x3210

#define SOC_JER_RCY_CTXT_MAP_START(unit, core_id) (core_id == 0 ? \
        SOC_DPP_IMP_DEFS_GET(unit, rcy_ctxt_map_start_core_0) :   \
        SOC_DPP_IMP_DEFS_GET(unit, rcy_ctxt_map_start_core_1))

#define SOC_JER_RCY_INTERFACE(unit, core_id) (core_id == 0 ? \
    SOC_DPP_IMP_DEFS_GET(unit, rcy_0_interface) :            \
    SOC_DPP_IMP_DEFS_GET(unit, rcy_1_interface))


#define  SOC_JER_NOF_CPU_IF_CHANNELS_MAX 64

#include <soc/types.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_occupation_bitmap.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/JER/jer_nif.h>
#include <soc/dpp/QAX/qax_nif.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_fabric.h>
#include <soc/dpp/JER/jer_tdm.h>
#include <soc/dpp/QAX/qax_link_bonding.h>
#include <soc/dpp/TMC/tmc_api_ports.h>
#include <soc/dpp/JER/JER_PP/jer_pp_oam.h>


#define JER_EGQ_FQ_RDY_TH_ID_10GB    12
#define JER_EGQ_FQ_RDY_TH_ID_25GB    18 
#define JER_EGQ_FQ_RDY_TH_ID_40GB    23
#define JER_EGQ_FQ_RDY_TH_ID_100GB   47
#define JER_EGQ_FQ_RDY_TH_ID_200GB   64
#define JER_EGQ_FQ_RDY_TH_ID_300GB   70
#define JER_EGQ_FQ_RDY_TH_ID_CPU_OAM 23
#define JER_EGQ_FQ_RDY_TH_ID_OLP     23
#define JER_EGQ_FQ_RDY_TH_ID_RCY     70
#define JER_EGQ_FQ_RDY_TH_ID_OFF     0

int soc_jer_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (ilkn_intf_offset) {
    case 0:
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN0;
        break;
    case 1:
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN1;
        break;
    case 2: 
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN2;
        break;
    case 3:
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN3;
        break;
    case 4:
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN4;
        break;
    case 5:
        *nif_port = SOC_JER_ILKN_NIF_PORT_ILKN5;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid ILKN interface offset %d"),ilkn_intf_offset));
        break;
    }
exit:
    SOCDNX_FUNC_RETURN;
}



int soc_jer_port_ilkn_has_separate_context(int unit, soc_port_t port, 
                                           int is_dedicated_tdm_context, 
                                           soc_pbmp_t* tdm_queuing_pbmp)
{
    return (IS_TDM_PORT(unit, port) || (is_dedicated_tdm_context && SOC_PBMP_MEMBER(*tdm_queuing_pbmp, port)));
}


int soc_jer_port_ilkn_context_reference_port_get( int unit, soc_port_t port, 
                                                  int is_dynamic_port,
                                                  soc_port_t* ref_port)
{

    soc_port_if_t interface;
    soc_pbmp_t ports_pbmp;
    soc_port_t temp_port;
    int is_dedicated_tdm_context;
    soc_pbmp_t tdm_queuing_pbmp;
    uint32 protocol_offset;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config;

    SOCDNX_INIT_FUNC_DEFS;

    *ref_port = SOC_PORT_INVALID;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if (interface == SOC_PORT_IF_ILKN) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
        ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_get(unit, &tdm_queuing_pbmp));
        is_dedicated_tdm_context =  (ilkn_config->interleaved && ilkn_config->dedicated_tdm_context);
        
        if(!soc_jer_port_ilkn_has_separate_context(unit, port, is_dedicated_tdm_context, &tdm_queuing_pbmp)) { 
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp));
            
            SOC_PBMP_ITER(ports_pbmp, temp_port) {
                if (!soc_jer_port_ilkn_has_separate_context(unit, temp_port, is_dedicated_tdm_context, &tdm_queuing_pbmp)) {
                    if (temp_port != port) {
                        
                        *ref_port = temp_port;
                        break;
                    }
                    if (!is_dynamic_port) {
                        break; 
                    }
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;

}


int soc_jer_port_ilkn_same_context_group_size_get( int unit, soc_port_t port, 
                                                   int* size)
{

    soc_port_if_t interface;
    soc_pbmp_t ports_pbmp;
    soc_port_t temp_port;
    int is_dedicated_tdm_context;
    soc_pbmp_t tdm_queuing_pbmp;
    uint32 protocol_offset;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config;

    SOCDNX_INIT_FUNC_DEFS;

    *size = 0;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if (interface == SOC_PORT_IF_ILKN) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
        ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_get(unit, &tdm_queuing_pbmp));
        is_dedicated_tdm_context =  (ilkn_config->interleaved && ilkn_config->dedicated_tdm_context);

        if(!soc_jer_port_ilkn_has_separate_context(unit, port, is_dedicated_tdm_context, &tdm_queuing_pbmp)) { 
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &ports_pbmp));
            
            SOC_PBMP_ITER(ports_pbmp, temp_port) {
                if (temp_port != port && !soc_jer_port_ilkn_has_separate_context(unit, temp_port, is_dedicated_tdm_context, &tdm_queuing_pbmp)) {
                    *size = *size + 1;
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;

}


int soc_jer_port_context_alloc_required( int unit, soc_port_t port, 
                                         int is_dynamic_port,
                                         int* allocate_new,  soc_port_t* ref_port)
{
    soc_port_if_t interface;
    uint32 is_master_port;
    int is_dedicated_tdm_context, has_separate_context;
    soc_pbmp_t tdm_queuing_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    *allocate_new = 0;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if (interface == SOC_PORT_IF_ILKN) {

        ARAD_PORTS_ILKN_CONFIG* ilkn_config;
        uint32 protocol_offset;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
        ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_get(unit, &tdm_queuing_pbmp));
        is_dedicated_tdm_context =  (interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved && ilkn_config->dedicated_tdm_context);
        has_separate_context = soc_jer_port_ilkn_has_separate_context(unit, port, is_dedicated_tdm_context, &tdm_queuing_pbmp);

        if (has_separate_context || (ilkn_config->interleaved && !is_dedicated_tdm_context)) {
            *allocate_new = 1;
            *ref_port = SOC_PORT_INVALID;
        }
        else {
            SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_context_reference_port_get(unit, port, is_dynamic_port, ref_port));
            
            *allocate_new = (*ref_port == SOC_PORT_INVALID); 

        }
        
    }
    else {
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
        
        if (is_master_port ||
            (interface == SOC_PORT_IF_RCY && !SOC_DPP_CONFIG(unit)->jer->tm.single_reassembly_for_rcy))
        {
            *allocate_new = 1;
            *ref_port = SOC_PORT_INVALID;
        } else {
            *allocate_new = 0;
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, ref_port));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_jer_port_context_dealloc_required( int unit, soc_port_t port, 
                                           int* need_dealloc)
{
    soc_port_if_t interface;
    uint32 num_of_channels;
    int is_last_port;
    int is_dedicated_tdm_context;
    soc_pbmp_t tdm_queuing_pbmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));

    is_last_port = (num_of_channels==1 ? 1 : 0);

    *need_dealloc = 0;

    if (is_last_port) {
        *need_dealloc = 1;
    }
    else if (interface == SOC_PORT_IF_ILKN ) {
        ARAD_PORTS_ILKN_CONFIG* ilkn_config;
        uint32 protocol_offset;
        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
        ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

        is_dedicated_tdm_context =  (interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved && ilkn_config->dedicated_tdm_context);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_get(unit, &tdm_queuing_pbmp));

        if ((ilkn_config->interleaved && !is_dedicated_tdm_context) || (is_dedicated_tdm_context && SOC_PBMP_MEMBER(tdm_queuing_pbmp, port))) {
            *need_dealloc = 1;
        }
        else {
            int grp_size;
            SOCDNX_IF_ERR_EXIT(soc_jer_port_ilkn_same_context_group_size_get(unit, port, &grp_size));
            
            *need_dealloc = (grp_size == 0); 
        }
    }

exit:
    SOCDNX_FUNC_RETURN;


}

int
  soc_jer_port_to_interface_ingress_map_set(
      int                   unit,
      soc_port_t            port, 
      int                   is_dynamic_port
   )
{
    soc_port_if_t interface;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config = NULL;
    uint32 data, channel, ctxt_id, index, tm_port, phy_port, nif_port;
    int core;
    uint32 protocol_offset, flags, is_nif;
    uint32 ctxt_channelized, ctxt_addr, ref_ctxt_id;
    uint32 is_master_port, reassebly_ctxt = 0xFFFFFFFF, ref_channel, base_ctxt_id;
    uint8 found;
    soc_reg_above_64_val_t data_above_64;
    soc_port_t ref_port;
    SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;
    soc_error_t rv;
    int allocate_new = 0;
    uint32 tdm_reassembly_context, reassebly_ctxt_2 = 0xFFFFFFFF;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));

    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);
    if((interface == SOC_PORT_IF_ERP) || SOC_PORT_IS_LB_MODEM(flags)) {
        
    }
    else if(interface == SOC_PORT_IF_OLP) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
        }

        data = 0;
        if (SOC_IS_QAX(unit)) {
            soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_REASSEMBLY_CONTEXTf, reassebly_ctxt);
            soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_PORT_TERMINATION_CONTEXTf, tm_port);
        } else {
            soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
            soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OLP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
        if (!found) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
        }

        data = 0;
        if (SOC_IS_QAX(unit)) {
            soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_REASSEMBLY_CONTEXTf, reassebly_ctxt);
            soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_PORT_TERMINATION_CONTEXTf, tm_port);
        } else {
            soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_REASSEMBLY_CONTEXTf, reassebly_ctxt);
            soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, OAMP_N_PORT_TERMINATION_CONTEXTf, tm_port);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if(interface == SOC_PORT_IF_IPSEC) {
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt
               .reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(
                    unit,
                    reassembly_ctxt_occ,
                    &reassebly_ctxt,
                    &found));

        if (!found) {
            SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
        }

        data = 0;
        soc_reg_field_set(unit,
                IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr,
                &data,
                IPSEC_REASSEMBLY_CONTEXTf,
                reassebly_ctxt);
        soc_reg_field_set(unit,
                IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr,
                &data,
                IPSEC_PORT_TERMINATION_CONTEXTf,
                tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr(
                    unit,
                    data));
    }
    else if ((interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (interface == SOC_PORT_IF_CPU) ||(interface == SOC_PORT_IF_SAT)  || (interface == SOC_PORT_IF_LBG) || (is_master_port && is_nif)) 
    {   
        ctxt_channelized = 0;
        ctxt_addr = SOC_JER_INVALID_CTXT_ADDRESS;

        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

            rv = MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_ilkn_nif_port_get, (unit, protocol_offset, &nif_port));
            SOCDNX_IF_ERR_EXIT(rv);

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, ilkn_ctxt_map_start) + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
            index = nif_port;
            ctxt_channelized = ilkn_config->interleaved ? 1 : 0;  
            ctxt_addr = ilkn_config->interleaved ? protocol_offset : SOC_JER_INVALID_CTXT_ADDRESS;      
        } 
        else if (interface == SOC_PORT_IF_RCY){
            base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(unit, core);
            ctxt_id = base_ctxt_id + channel;
            index = SOC_JER_RCY_INTERFACE(unit, core);
        } else if (interface == SOC_PORT_IF_CPU) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, cpu_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
            index = SOC_DPP_IMP_DEFS_GET(unit, cpu_interface);
        } else if (interface == SOC_PORT_IF_SAT) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, sat_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
            index = SOC_DPP_IMP_DEFS_GET(unit, sat_interface);
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
            phy_port--; 

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, nif_ctxt_map_start) + phy_port;
            ctxt_id = base_ctxt_id;
            index = phy_port;
        } 

        data = 0;
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_IS_CHANNELIZEDf, &ctxt_channelized);
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf, &ctxt_addr);  
        soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, CTXT_MAP_BASE_ADDRESSf, &base_ctxt_id);  
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, index, &data));

        

        if (interface == SOC_PORT_IF_LBG) {
            if (SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                index++;
                base_ctxt_id++;
                data = 0;
                soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_IS_CHANNELIZEDf, &ctxt_channelized);
                soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf, &ctxt_addr);  
                soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, CTXT_MAP_BASE_ADDRESSf, &base_ctxt_id);  
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, index, &data));
            }
#ifdef BCM_LB_SUPPORT
            SOCDNX_IF_ERR_EXIT(qax_lb_lbg_to_ingress_receive_editor(unit, port));
#endif
        }

        if (IS_TDM_PORT(unit, port) && ((interface == SOC_PORT_IF_ILKN) || is_nif)) {
            
            data = 0;
            SOCDNX_IF_ERR_EXIT(arad_tdm_local_to_reassembly_context_get(unit, (uint32)port, &tdm_reassembly_context));
            soc_mem_field_set(unit, IRE_NIF_TDM_CTXT_MAPm, &data, TDM_CONTEXTf, &tdm_reassembly_context);
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IRE_NIF_TDM_CTXT_MAPm, MEM_BLOCK_ANY, ctxt_id, &data));
        }

        
        if (IS_TDM_PORT(unit, port)) {
            SOC_EXIT;
        }

        SOCDNX_IF_ERR_EXIT(soc_jer_port_context_alloc_required(unit, port, is_dynamic_port, &allocate_new, &ref_port));

        if (allocate_new)
        {
            
            rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
            SOCDNX_IF_ERR_EXIT(rv);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt, &found));
            if (!found) {
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
            }

            if ((interface == SOC_PORT_IF_LBG) && SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                
                rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
                SOCDNX_IF_ERR_EXIT(rv);
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_alloc_next(unit, reassembly_ctxt_occ, &reassebly_ctxt_2, &found));
                if (!found) {
                    
                    SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
                    SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_RESOURCE);
                }
            }
        } else {
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, ref_port, &ref_channel));
            ref_ctxt_id = base_ctxt_id + ref_channel;

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ref_ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        }

        data = 0;
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        if ((interface == SOC_PORT_IF_LBG) && SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            data = 0;
            soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt_2);
            soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
            SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, (ctxt_id+1), &data)); 
        }

         if(interface == SOC_PORT_IF_ILKN && ilkn_config->interleaved) {
            if (SOC_IS_JERICHO(unit) && !soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "interleaved_error_drop_single_context", 0))
            {
                SOC_REG_ABOVE_64_CLEAR(data_above_64);
                SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
                if (reassebly_ctxt >= sizeof(data_above_64)*8) {
                    
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid reassembly_ctxt %d"), reassebly_ctxt));
                }
                SHR_BITSET(data_above_64, reassebly_ctxt);
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
            }
        }

        
        if (interface == SOC_PORT_IF_CPU) {
            if (!SOC_IS_QAX(unit)) {
                SOC_REG_ABOVE_64_CLEAR(data_above_64);
                SOCDNX_IF_ERR_EXIT(READ_IRE_CPU_CHANNEL_PIPE_MAPr(unit, data_above_64));
                if(core == 1) {
                    SHR_BITSET(data_above_64, channel);
                } else { 
                    SHR_BITCLR(data_above_64, channel);
                }
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_CPU_CHANNEL_PIPE_MAPr(unit, data_above_64));            
            }
        }
    }

    
    if (interface != SOC_PORT_IF_ERP && reassebly_ctxt != 0xFFFFFFFF) {
        if (!SOC_IS_QAX(unit)) {
            SOC_REG_ABOVE_64_CLEAR(data_above_64);
            SOCDNX_IF_ERR_EXIT(READ_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
            if(core == 1) {
                SHR_BITSET(data_above_64, reassebly_ctxt);
            } else { 
                SHR_BITCLR(data_above_64, reassebly_ctxt);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_port_ingr_reassembly_context_get(
      SOC_SAND_IN int         unit,
      SOC_SAND_IN soc_port_t  port,
      SOC_SAND_OUT uint32     *port_termination_context,
      SOC_SAND_OUT uint32     *reassembly_context
  )
{
    soc_port_if_t interface;
    uint32 data, channel, ctxt_id, tm_port, phy_port;
    int core;
    uint32 protocol_offset, flags, is_nif;
    uint32 is_master_port, base_ctxt_id;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    *port_termination_context = 0xffffffff;
    *reassembly_context = 0xffffffff;

    if(interface == SOC_PORT_IF_ERP) {
        
        *port_termination_context = 0xffffffff;
        *reassembly_context = 0xffffffff;
    }
    else if(interface == SOC_PORT_IF_OLP) {
    	SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        *reassembly_context = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, (SOC_IS_QAX(unit)? OLP_REASSEMBLY_CONTEXTf : OLP_N_REASSEMBLY_CONTEXTf));
        *port_termination_context = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, (SOC_IS_QAX(unit) ? OLP_PORT_TERMINATION_CONTEXTf : OLP_N_PORT_TERMINATION_CONTEXTf));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
    	SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        *reassembly_context = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, (SOC_IS_QAX(unit)? OAMP_REASSEMBLY_CONTEXTf : OAMP_N_REASSEMBLY_CONTEXTf));
        *port_termination_context = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, (SOC_IS_QAX(unit) ? OAMP_PORT_TERMINATION_CONTEXTf : OAMP_N_PORT_TERMINATION_CONTEXTf));
    }
    else if(interface == SOC_PORT_IF_IPSEC) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, &data));
        *reassembly_context = soc_reg_field_get(unit, IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, IPSEC_REASSEMBLY_CONTEXTf);
        *port_termination_context = soc_reg_field_get(unit, IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, IPSEC_PORT_TERMINATION_CONTEXTf);
    }
    else if ((interface == SOC_PORT_IF_SAT) || (interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (interface == SOC_PORT_IF_CPU) || (interface == SOC_PORT_IF_LBG) || (is_nif))
    {
        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
            phy_port--; 

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, ilkn_ctxt_map_start) + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
        }
        else if (interface == SOC_PORT_IF_RCY){
            base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(unit, core);
            ctxt_id = base_ctxt_id + channel;
        } else if (interface == SOC_PORT_IF_CPU) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, cpu_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
        } else if (interface == SOC_PORT_IF_SAT) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, sat_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
            phy_port--; 

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, nif_ctxt_map_start) + phy_port;
            ctxt_id = base_ctxt_id;
        }

        SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        *reassembly_context = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf);
        *port_termination_context = soc_mem_field32_get(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_port_ingr_reassembly_context_set(
      SOC_SAND_IN int         unit,
      SOC_SAND_IN soc_port_t  port,
      uint32      reassembly_context
  )
{
    soc_port_if_t interface;
    uint32 data, channel, ctxt_id, tm_port, phy_port;
    int core;
    uint32 protocol_offset, flags, is_nif;
    uint32 base_ctxt_id;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    if(interface == SOC_PORT_IF_ERP) {
        
    }
    else if(interface == SOC_PORT_IF_OLP) {
    	SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        soc_reg_field_set(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, (SOC_IS_QAX(unit)? OLP_REASSEMBLY_CONTEXTf : OLP_N_REASSEMBLY_CONTEXTf), reassembly_context);
    	SOCDNX_IF_ERR_EXIT(WRITE_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
    	SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        soc_reg_field_set(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, (SOC_IS_QAX(unit)? OAMP_REASSEMBLY_CONTEXTf : OAMP_N_REASSEMBLY_CONTEXTf), reassembly_context);
    	SOCDNX_IF_ERR_EXIT(WRITE_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, data));
    }
    else if(interface == SOC_PORT_IF_IPSEC) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, &data));
        soc_reg_field_set(unit, IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr, &data, IPSEC_REASSEMBLY_CONTEXTf, reassembly_context);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, data));
    }
    else if ((interface == SOC_PORT_IF_SAT) || (interface == SOC_PORT_IF_ILKN) || (interface == SOC_PORT_IF_RCY) || (interface == SOC_PORT_IF_CPU) || (interface == SOC_PORT_IF_LBG) || (is_nif))
    {
        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
            phy_port--; 

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, ilkn_ctxt_map_start) + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
        }
        else if (interface == SOC_PORT_IF_RCY){
            base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(unit, core);
            ctxt_id = base_ctxt_id + channel;
        } else if (interface == SOC_PORT_IF_CPU) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, cpu_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
        } else if (interface == SOC_PORT_IF_SAT) {
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, sat_ctxt_map_start);
            ctxt_id = base_ctxt_id + channel;
        } else {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
            phy_port--; 

            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, nif_ctxt_map_start) + phy_port;
            ctxt_id = base_ctxt_id;
        }

        SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassembly_context);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  soc_jer_port_to_interface_ingress_unmap_set(
      int                   unit,
      soc_port_t            port
   )
{
    soc_port_if_t interface;
    ARAD_PORTS_ILKN_CONFIG* ilkn_config = NULL;
    uint32 data, channel, ctxt_id, tm_port, num_of_channels;
    int core;
    uint32 is_last_port, phy_port, reassebly_ctxt, is_master_port;
    soc_reg_above_64_val_t data_above_64;
    uint32 protocol_offset, base_ctxt_id, flags, is_nif;
    soc_port_t next_master;
    SOC_SAND_OCC_BM_PTR reassembly_ctxt_occ;
    soc_error_t rv;
    int need_dealloc;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));
    is_last_port = (num_of_channels==1 ? 1 : 0);
    is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

    if(interface == SOC_PORT_IF_ERP || SOC_PORT_IS_LB_MODEM(flags)) {
        
        is_last_port = SOC_PORT_IS_LB_MODEM(flags) ? 1 : is_last_port;
        
    } if(interface == SOC_PORT_IF_OLP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        if (SOC_IS_QAX(unit)) {
            reassebly_ctxt = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OLP_REASSEMBLY_CONTEXTf);
        } else {
            reassebly_ctxt = soc_reg_field_get(unit, IRE_OLP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OLP_N_REASSEMBLY_CONTEXTf);
        }
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
    }
    else if(interface == SOC_PORT_IF_OAMP) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, core, &data));
        if (SOC_IS_QAX(unit)) {
            reassebly_ctxt = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_REASSEMBLY_CONTEXTf);
        } else {
            reassebly_ctxt = soc_reg_field_get(unit, IRE_OAMP_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, OAMP_N_REASSEMBLY_CONTEXTf);
        }
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
    }
    else if(interface == SOC_PORT_IF_IPSEC) {
        SOCDNX_IF_ERR_EXIT(READ_IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr(unit, &data));
        reassebly_ctxt = soc_reg_field_get(unit, IRE_IPSEC_REASSEMBLY_CONTEXT_CONFIGURATIONr, data, IPSEC_REASSEMBLY_CONTEXTf);
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
    }
    else if((interface == SOC_PORT_IF_ILKN) ||
            (interface == SOC_PORT_IF_RCY)  ||
            (interface == SOC_PORT_IF_LBG)  ||
            (is_last_port && (interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_SAT || is_nif))) {

        if (IS_TDM_PORT(unit, port) && ((interface == SOC_PORT_IF_ILKN) || is_nif)) {
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset));
            base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, ilkn_ctxt_map_start) + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
            ctxt_id = base_ctxt_id + channel;
            data = 0;
            SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, IRE_NIF_TDM_CTXT_MAPm, MEM_BLOCK_ANY, ctxt_id, &data));
        }

        
        if (IS_TDM_PORT(unit, port)) {
            SOC_EXIT;
        }

        if(interface == SOC_PORT_IF_ILKN) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
            ilkn_config = &SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn[protocol_offset];

            SOCDNX_IF_ERR_EXIT(soc_jer_port_context_dealloc_required(unit, port, &need_dealloc));
            if (need_dealloc) { 
                base_ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, ilkn_ctxt_map_start) + (SOC_DPP_MAX_NOF_CHANNELS*protocol_offset);
                ctxt_id = base_ctxt_id + channel;
                SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
                soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);
                data = 0;
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

                
                rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
                SOCDNX_IF_ERR_EXIT(rv);
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));

                if(ilkn_config->interleaved) {
                    SOC_REG_ABOVE_64_CLEAR(data_above_64);
                    SOCDNX_IF_ERR_EXIT(READ_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
                    SHR_BITCLR(data_above_64, reassebly_ctxt);
                    SOCDNX_IF_ERR_EXIT(WRITE_IRE_NIF_PORT_TO_CTXT_BIT_MAPm(unit, MEM_BLOCK_ANY, protocol_offset, data_above_64));
                }
            }
        } else {
            switch(interface) {
                case SOC_PORT_IF_RCY:
                    ctxt_id = SOC_JER_RCY_CTXT_MAP_START(unit, core) + channel;
                    break;
                case SOC_PORT_IF_CPU:
                    ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, cpu_ctxt_map_start) + channel;
                    break;
                case SOC_PORT_IF_SAT:
                    ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, sat_ctxt_map_start) + channel;
                    break;
                default:
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
                    phy_port--; 
                    ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, nif_ctxt_map_start) + phy_port;
                    break;
            }

            SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
            soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

            
            rv = sw_state_access[unit].dpp.soc.arad.tm.reassembly_ctxt.reassembly_ctxt_occ.get(unit, &reassembly_ctxt_occ);
            SOCDNX_IF_ERR_EXIT(rv);
            SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
            if ((interface == SOC_PORT_IF_LBG) && SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
                SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, (ctxt_id+1), &data));
                soc_mem_field_get(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassebly_ctxt);

                
                SOCDNX_SAND_IF_ERR_EXIT(soc_sand_occ_bm_occup_status_set(unit, reassembly_ctxt_occ,reassebly_ctxt,0));
            }
        }
    }

    
    if (is_nif && is_master_port && !is_last_port && (interface != SOC_PORT_IF_ILKN)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_next_master_get(unit, port, &next_master));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, next_master, &tm_port, &core));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
        phy_port--; 
        ctxt_id = SOC_DPP_IMP_DEFS_GET(unit, nif_ctxt_map_start) + phy_port;

        SOCDNX_IF_ERR_EXIT(READ_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
        soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &tm_port);
        SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_jer_ports_phy_to_quad_get(
    int                   unit,
    soc_port_t            first_phy, 
    uint32*               quad
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (first_phy < 37) {
        (*quad) = (first_phy - 1) / 4;
    } else if (first_phy < 85) {
        (*quad) = ((first_phy - 37) / 16) + 9;
    } else if (first_phy < 97) {
        (*quad) = ((first_phy - 85) / 4) + 12;
    } else {
        (*quad) = ((first_phy - 97) / 16) + 15;
    }

    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_qax_ports_phy_to_quad_get(
    int                   unit,
    soc_port_t            first_phy, 
    uint32*               quad
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if (first_phy < 17) {
        (*quad) = (first_phy - 1) / 4;
    } else if (first_phy < 65) {
        (*quad) = ((first_phy - 17) / 16) + 4;
    } else if (first_phy < 69) {
        (*quad) = ((first_phy - 65) / 4) + 7;
    } else {
        (*quad) = ((first_phy - 69) / 16) + 8;
    }

    SOCDNX_FUNC_RETURN;
}

#define EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT 18
#define JER_PORTS_INVALID_INTF 0xFF

int
  soc_jer_port_to_interface_egress_map_set(
    int                   unit,
    soc_port_t            port
   )
{
    uint32 tm_port, egr_if, reg32[1];
    int ps, is_channelized;
    soc_reg_above_64_val_t data;
    uint32 q_pair_num, nof_q_pairs;
    soc_port_if_t interface;
    ARAD_EGQ_PPCT_TBL_DATA ppct_data;
    uint32 offset, channel, cpu_channel = 0, if_rate_mbps, otm_port_number, ifc_delay, ifc_val;
    uint32 fq_rdy_sel_val, base_q_pair = 0, phy_port;
    int rv, core; 
    uint32 cancel_en1, cancel_en2, cancel_en3, cancel_en4, quad;
    uint32 protocol_offset, is_nif, flags;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

    uint32 is_skip_nif_for_lb = 0, is_skip_egq_for_lb = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    if(interface != SOC_PORT_IF_ERP) {
        ifc_delay = 1;
        cancel_en1 = 0;
        cancel_en2 = 0;
        cancel_en3 = 0;
        cancel_en4 = 0;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        if ((interface == SOC_PORT_IF_LBG) && (if_rate_mbps >= 20000)) {
            
            if_rate_mbps = 40000;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OAMP) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_CPU_OAM;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (interface == SOC_PORT_IF_OLP) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OLP;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (interface == SOC_PORT_IF_RCY) {
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_RCY;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else if (if_rate_mbps == 0) { 
            fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OFF;
            cancel_en1 = 1;
            cancel_en2 = 1;
            cancel_en3 = 1;
        } else {
            if (if_rate_mbps <= 12500) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_10GB;
                cancel_en1 = 1;
                cancel_en2 = 1;
                cancel_en3 = 1;
            } else if (if_rate_mbps <= 30000) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_25GB;
                cancel_en1 = 1;
                cancel_en2 = 1;
                cancel_en3 = 1;
            } else if (if_rate_mbps <= 50000) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_40GB;
                cancel_en1 = 1;
            } else if (if_rate_mbps <= 100000) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_100GB;
            } else if (if_rate_mbps <= 150000) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_200GB;
            } else if (if_rate_mbps <= 200000) { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_200GB;
            } else { 
                fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_300GB;
            }

            

            if (SOC_IS_QAX(unit) && if_rate_mbps <= 50000) {
                uint32 core_freq = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency/1000; 
                int min_packet_size = 64;
				
                cancel_en1 = ((core_freq/4*min_packet_size * SOC_SAND_NOF_BITS_IN_BYTE) >= if_rate_mbps);
                if (if_rate_mbps <= 30000) {
                    
                    cancel_en2 = ((core_freq/6*min_packet_size * SOC_SAND_NOF_BITS_IN_BYTE) >= if_rate_mbps);
                    cancel_en3 = ((core_freq/8*min_packet_size * SOC_SAND_NOF_BITS_IN_BYTE) >= if_rate_mbps);
                }

            }
        }		

        if (!SOC_PORT_IS_LB_MODEM(flags)) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
            SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
            SOCDNX_IF_ERR_EXIT(rv);
        }
        else {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
            SOCDNX_IF_ERR_EXIT(rv);
        }

#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) 
        {
            if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (SOC_QAX_EGR_IF_LBG_RESERVE == egr_if)) {
                is_skip_nif_for_lb = 1;
                is_skip_egq_for_lb = 1;
            }

            if (SOC_PORT_IS_LB_MODEM(flags)) {
                is_skip_egq_for_lb = 1;
            }
        }
#endif

        if ((is_skip_egq_for_lb != 1)) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &fq_rdy_sel_val);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_1f, &cancel_en1);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_2f, &cancel_en2);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_3f, &cancel_en3);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_4f, &cancel_en4);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, IFC_DELAYf, &ifc_delay);
            otm_port_number = is_channelized ? ARAD_EGR_INVALID_BASE_Q_PAIR : base_q_pair;
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, &otm_port_number);
            ifc_val = (interface == SOC_PORT_IF_QSGMII || interface == SOC_PORT_IF_SGMII)? 1 : 0;
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, IFC_IS_QSGMIIf, &ifc_val);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
            if(interface == SOC_PORT_IF_LBG) {
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), (egr_if+1), data));
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));

            
            for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
            {
                offset = base_q_pair+q_pair_num;
                SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
                
                if (interface == SOC_PORT_IF_CPU) {
                    cpu_channel = offset%SOC_JER_NOF_CPU_IF_CHANNELS_MAX;
                    soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &cpu_channel);
                }
                else {
                    soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
                }
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), offset, data));
            }

            if (SOC_DPP_CONFIG((unit))->tm.queue_level_interface_enable) 
            {
                if(interface == SOC_PORT_IF_ILKN) {
                    int curr_mc_q_pair, curr_uc_q_pair;

                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));

                    for (q_pair_num = 0; q_pair_num < nof_q_pairs; q_pair_num++)
                    {
                        curr_uc_q_pair = base_q_pair+q_pair_num;
                        curr_mc_q_pair = ARAD_EGR_QUEUING_MC_QUEUE_OFFSET(curr_uc_q_pair);

                        SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_uc_q_pair, data));
                        channel = SOC_DPP_CONFIG((unit))->tm.uc_q_pair2channel_id[protocol_offset][q_pair_num];
                        soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
                        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_uc_q_pair, data));

                        SOCDNX_IF_ERR_EXIT(READ_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_mc_q_pair, data));
                        channel = SOC_DPP_CONFIG((unit))->tm.mc_q_pair2channel_id[protocol_offset][q_pair_num];
                        soc_mem_field_set(unit, EGQ_PCTm, data, PORT_CH_NUMf, &channel);
                        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PCTm(unit, EGQ_BLOCK(unit, core), curr_mc_q_pair, data));
                    }
                }
            }

            rv = arad_egq_ppct_tbl_get_unsafe(unit, core, base_q_pair, &(ppct_data));
            SOCDNX_IF_ERR_EXIT(rv);

            ppct_data.cgm_interface = egr_if;

            rv = arad_egq_ppct_tbl_set_unsafe(unit, core, base_q_pair, &(ppct_data));
            SOCDNX_IF_ERR_EXIT(rv);

            
            SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,
                        mbcm_dpp_port_control_tx_nif_enable_set,
                        (unit, port, TRUE)));

        }

        if((egr_if < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces)) && (is_skip_egq_for_lb != 1)) {
            
            ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
            SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));    
            SHR_BITSET(data, ps);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));   

            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg32));
            if(is_channelized) {
                SHR_BITSET(reg32, egr_if);
            } else {
                SHR_BITCLR(reg32, egr_if);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_IFC_IS_CHANNELIZEDr(unit, core, *reg32));
        }


        
        if((is_nif || SOC_PORT_IS_LB_MODEM(flags)) && 
           (is_skip_nif_for_lb != 1)) {
            if(interface == SOC_PORT_IF_ILKN) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));

                if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
                        SOC_SAND_SET_BIT(egr_if, 0, 0); 
                }
                
                SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_HRFr(unit, protocol_offset, egr_if));

                
                if (!SOC_IS_QAX(unit)) {
					SOCDNX_IF_ERR_EXIT(READ_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, reg32));
					if(core == 1) {
						SHR_BITSET(reg32, EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT + protocol_offset);
					} else { 
						SHR_BITCLR(reg32, EGQ_CORE_FOR_NIF_QMLF_FIRST_ILKN_BIT + protocol_offset);
					}

					SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, *reg32));
                }
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
                if (SOC_IS_QAX(unit)) {
                    SOCDNX_IF_ERR_EXIT(soc_qax_ports_phy_to_quad_get(unit, phy_port, &quad));
                } else {
                    SOCDNX_IF_ERR_EXIT(soc_jer_ports_phy_to_quad_get(unit, phy_port, &quad));
                }
                phy_port--;
                if (SOC_IS_QUX(unit)) {
                    SOCDNX_IF_ERR_EXIT(WRITE_NIF_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, egr_if));
                } else {
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, egr_if));
                } 

                if (!SOC_IS_QAX(unit)) {
                    SOCDNX_IF_ERR_EXIT(READ_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, reg32));
                    if(core == 1) {
                        SHR_BITSET(reg32, quad);
                    } else { 
                        SHR_BITCLR(reg32, quad);
                    }
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_CORE_FOR_NIF_QMLFr(unit, *reg32));
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC int
  soc_jer_port_to_interface_egress_unmap_set(
      int                   unit,
      soc_port_t            port
   )
{
    uint32 tm_port, tm_port_i, egr_if, flags, is_nif;
    int core, core_i, ps, ps_i;
    int skip_ps_mapping = FALSE;
    soc_reg_above_64_val_t data;
    uint32 fq_rdy_sel_val, invalid_otm;
    uint32 cancel_en1, cancel_en2, cancel_en3, cancel_en4;
    uint32 base_q_pair = ARAD_EGR_INVALID_BASE_Q_PAIR, base_q_pair_i, reg32[1], if_rate_mbps, higher_req_en;
    uint32 is_last_port, num_of_channels, phy_port, protocol_offset;
    soc_port_if_t interface;
    soc_error_t rv = SOC_E_NONE;
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 is_skip_nif_for_lb = 0, is_skip_egq_for_lb = 0;
    uint32 flags_i = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface != SOC_PORT_IF_ERP) {
        cancel_en1 = 1;
        cancel_en2 = 1;
        cancel_en3 = 1;
        cancel_en4 = 0;

        fq_rdy_sel_val = JER_EGQ_FQ_RDY_TH_ID_OFF;

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        if (!SOC_PORT_IS_LB_MODEM(flags)) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
            SOCDNX_IF_ERR_EXIT(rv);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
            SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));
        }
        else {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
            SOCDNX_IF_ERR_EXIT(rv);
        }

#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) 
        {
            if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (SOC_QAX_EGR_IF_LBG_RESERVE == egr_if)) {
                is_skip_nif_for_lb = 1;
                is_skip_egq_for_lb = 1;
            }

            if (SOC_PORT_IS_LB_MODEM(flags)) {
                is_skip_egq_for_lb = 1;
            }
        }
#endif

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_of_channels_get(unit, port, &num_of_channels));
        is_last_port = (num_of_channels==1 ? 1 : 0);
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        invalid_otm = 0xFF;
        higher_req_en = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS ? 1 : 0);

        if (is_last_port && (is_skip_egq_for_lb != 1)) {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egr_if, data));
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, FQ_RDY_THf, &fq_rdy_sel_val);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_1f, &cancel_en1);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_2f, &cancel_en2);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_3f, &cancel_en3);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NIF_CANCEL_EN_4f, &cancel_en4);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, OTM_PORT_NUMBERf, &invalid_otm);
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &higher_req_en);
            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        }

        if((egr_if < SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces)) && (is_skip_egq_for_lb != 1)) {
            
            ps = ARAD_BASE_PORT_TC2PS(base_q_pair);
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_ports_to_same_interface_get(unit, port, &pbmp));
            SOC_PBMP_ITER(pbmp, port_i) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags_i));
                if (SOC_PORT_IS_LB_MODEM(flags_i)) {
                    continue;
                }

                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port_i, &core_i));
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core_i, tm_port_i, &base_q_pair_i));
                ps_i = ARAD_BASE_PORT_TC2PS(base_q_pair_i);

                if ((port != port_i) && (ps == ps_i) && (core == core_i)) {
                    skip_ps_mapping = TRUE;
                    break;
                }
            }


            if (!skip_ps_mapping) {
                SOCDNX_IF_ERR_EXIT(READ_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));
                SHR_BITCLR(data, ps);
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_MAP_PS_TO_IFCm(unit, EGQ_BLOCK(unit, core), egr_if, data));
            }

            if (is_last_port) {
                
                SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg32));
                SHR_BITCLR(reg32, egr_if);
                SOCDNX_IF_ERR_EXIT(WRITE_EGQ_IFC_IS_CHANNELIZEDr(unit, core, *reg32));
            }
        }

        
        if((is_nif && (is_skip_nif_for_lb != 1)) || SOC_PORT_IS_LB_MODEM(flags)) {
            if(is_last_port || SOC_PORT_IS_LB_MODEM(flags)) {
                if(interface == SOC_PORT_IF_ILKN) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));

                    
                    SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_HRFr(unit, protocol_offset, JER_PORTS_INVALID_INTF));
                } else {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
                    phy_port--;
                    if (SOC_IS_QUX(unit)) {
                        SOCDNX_IF_ERR_EXIT(WRITE_NIF_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, JER_PORTS_INVALID_INTF));
                    } else {
                        SOCDNX_IF_ERR_EXIT(WRITE_NBIH_EGQ_PORT_FOR_NIF_PORTr(unit, phy_port, JER_PORTS_INVALID_INTF));
                    }
                }
            }
        }
    }



exit:
    SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_port_synce_clk_sel_get(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  *port)
{
    uint32
        reg_val,
        phy_line,
        phy_port = 0x0,
        lane_map,
        swapped_lane;
    soc_reg_t
        synce_cfg_reg = INVALIDr;
    int phy_idx, pmx_base_lane = SOC_DPP_DEFS_GET(unit, pmx_base_lane);

    SOCDNX_INIT_FUNC_DEFS;

    synce_cfg_reg = SOC_IS_QUX(unit) ? NIF_SYNC_ETH_CFGr : NBIH_SYNC_ETH_CFGr;
    SOCDNX_IF_ERR_EXIT(
        soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_cfg_num, &reg_val));

    phy_line = soc_reg_field_get(unit, synce_cfg_reg, reg_val, SYNC_ETH_CLOCK_SEL_Nf);

    if (!(SOC_IS_QUX(unit) && phy_line >= pmx_base_lane)) {

        lane_map = soc_property_suffix_num_get(unit,
                                               phy_line / NUM_OF_LANES_IN_PM,
                                               spn_PHY_RX_LANE_MAP, "quad",
                                               SOC_JER_PORT_NO_LANE_SWAP) & 0xffff;
        if (lane_map != SOC_JER_PORT_NO_LANE_SWAP) {
            for (phy_idx = 0; phy_idx < NUM_OF_LANES_IN_PM; phy_idx++) {
                swapped_lane = (lane_map >> (phy_idx * NUM_OF_LANES_IN_PM)) & 0xf;
                if ((phy_line % NUM_OF_LANES_IN_PM) == swapped_lane) {
                    break;
                }
            }
            phy_line = phy_line - (phy_line % NUM_OF_LANES_IN_PM) + phy_idx;
        }
    }
    phy_line += 1;  

    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,
                mbcm_dpp_qsgmii_offsets_add, (unit, phy_line, &phy_port)));
    *port = SOC_INFO(unit).port_p2l_mapping[phy_port];

exit:
    SOCDNX_FUNC_RETURN; 
}

uint32 soc_jer_port_synce_clk_sel_set(
                            int                         unit,
                            uint32                      synce_cfg_num,
                            soc_port_t                  port)
{
    uint32
        reg_val,
        phy_line,
        phy_port = 0x0,
        is_master,
        swapped_lane;
    soc_reg_t
        synce_cfg_reg = INVALIDr;
    uint16
        lane_map;
    SOC_QUX_NIF_PLL_TYPE
        qux_pll_type = SOC_QUX_NIF_NOF_PLL_TYPE;

    SOCDNX_INIT_FUNC_DEFS;

    synce_cfg_reg = SOC_IS_QUX(unit) ? NIF_SYNC_ETH_CFGr : NBIH_SYNC_ETH_CFGr;
    SOCDNX_IF_ERR_EXIT(
        soc_reg32_get(unit, synce_cfg_reg, REG_PORT_ANY, synce_cfg_num, &reg_val));

    if (SOC_IS_QUX(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_qux_port_pll_type_get(unit, port, &qux_pll_type));
    }

    
    if (SOC_IS_QAX_WITH_FABRIC_ENABLED(unit) && IS_SFI_PORT(unit, port)) {
        phy_line = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port);
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit,  port, &phy_port));
        

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,
                mbcm_dpp_qsgmii_offsets_remove,
                (unit, phy_port, &phy_line)));

        phy_line -= 0x1; 
    }
    if (qux_pll_type == SOC_QUX_NIF_PLL_TYPE_PMX) {
        swapped_lane = phy_line;
    } else {
        lane_map = soc_property_suffix_num_get(unit,
                                               phy_line / NUM_OF_LANES_IN_PM,
                                               spn_PHY_RX_LANE_MAP, "quad",
                                               SOC_JER_PORT_NO_LANE_SWAP) & 0xffff;
        if (lane_map != SOC_JER_PORT_NO_LANE_SWAP) {
            swapped_lane = ((lane_map >> ((phy_line % NUM_OF_LANES_IN_PM) * NUM_OF_LANES_IN_PM)) & 0xf);
            swapped_lane = phy_line - (phy_line % NUM_OF_LANES_IN_PM) + swapped_lane;
        } else {
            swapped_lane = phy_line;
        }
    }
    soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_CLOCK_SEL_Nf,  swapped_lane);
    soc_reg_field_set(unit, synce_cfg_reg, &reg_val, SYNC_ETH_VALID_SEL_Nf,  phy_line);

    SOCDNX_IF_ERR_EXIT(
        soc_reg32_set(unit, synce_cfg_reg, REG_PORT_ANY, synce_cfg_num, reg_val));

    
    if (!SOC_IS_QUX(unit) && !IS_SFI_PORT(unit, port)) {
        is_master = synce_cfg_num? 0 : 1;
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_sync_e_enable_set(unit, is_master, 0));
    }
exit:
    SOCDNX_FUNC_RETURN; 
}

int
  soc_jer_port_to_interface_map_set(
      int                   unit,
      soc_port_t            port,
      int                   unmap,
      int                   is_dynamic_port
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    if(!unmap) {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_ingress_map_set(unit, port, is_dynamic_port));
        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_to_interface_egress_map_set, (unit, port)));
    } else {
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_ingress_unmap_set(unit, port));
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_egress_unmap_set(unit, port));
    }


    exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_ingress_enable_get(
    int                   unit,
    soc_port_t            port,
    int                   *enable)
{
    uint32     port_termination_context;
    uint32     reassembly_context;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_port_ingr_reassembly_context_get(unit, port,
                                                                &port_termination_context, &reassembly_context));
        
    *enable = ((reassembly_context != ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED) ? TRUE : FALSE);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_ingress_enable_set(
    int                   unit,
    soc_port_t            port,
    int                   enable)
{
    uint32     port_termination_context;
    uint32     reassembly_context, new_reassembly_context;
    int current_enable;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_port_ingress_enable_get(unit, port, &current_enable));
    
    if (enable != current_enable)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_port_ingr_reassembly_context_get(unit, port,
                                                                    &port_termination_context, &reassembly_context));
        if (enable == FALSE)
        {
            
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.port_disabled_reassembly_context.set(unit, port, reassembly_context));
            
            new_reassembly_context = ARAD_PORTS_REASSEMBLY_CONTEXT_UNMAPPED;
        }
        else
        {
            SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.port_disabled_reassembly_context.get(unit, port, &new_reassembly_context));
        }
        
        SOCDNX_IF_ERR_EXIT(soc_jer_port_ingr_reassembly_context_set(unit, port, new_reassembly_context));
    }
exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_port_rate_egress_pps_set (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 pps, 
    SOC_SAND_IN uint32 burst
    )
{
    uint32 limit_max, core_clock_speed, nof_tiks = 0;
    uint64 nof_tiks64, pps64, reg64_val;
    int max_length, fmac_index, fmac_inner_link;
    SOCDNX_INIT_FUNC_DEFS;

    
    core_clock_speed = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency * 1000 ;
    
    COMPILER_64_SET(nof_tiks64, 0, burst);
    COMPILER_64_UMUL_32(nof_tiks64, core_clock_speed); 

    
    if (pps != 0) {
        COMPILER_64_SET(pps64, 0, pps);
        COMPILER_64_UDIV_64(nof_tiks64, pps64);
        nof_tiks = COMPILER_64_LO(nof_tiks64);
    }

    
    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_COUNTf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  burst) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too big - should be < %u"), burst, limit_max)); 
    }

    max_length = soc_reg_field_length(unit, FMAC_TX_CELL_LIMITr,  CELL_LIMIT_PERIODf);
    limit_max = 0;
    SHR_BITSET_RANGE(&limit_max, 0, max_length);
    if (limit_max <  nof_tiks) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too big relative to pps %u"), burst, nof_tiks)); 
    }

    if (nof_tiks == 0 && burst != 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("num of ticks %u is too big relative to burst %u"), nof_tiks, burst)); 
    }

    
    if (burst == 0) {
        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_index, &fmac_inner_link));
        COMPILER_64_SET(reg64_val, 0, 0);
        SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));
        SOC_EXIT;
    } else if (burst < 3){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Burst %u is too small - should be > 3"), burst));  
    }

    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_index, &fmac_inner_link));
    COMPILER_64_SET(reg64_val, 0, 0);
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_COUNTf, burst - 2 );
    soc_reg64_field32_set(unit, FMAC_TX_CELL_LIMITr, &reg64_val, CELL_LIMIT_PERIODf, nof_tiks);
    SOCDNX_IF_ERR_EXIT(WRITE_FMAC_TX_CELL_LIMITr(unit, fmac_index, reg64_val));

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_port_rate_egress_pps_get (
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *pps, 
    SOC_SAND_OUT uint32 *burst
  )
{
    uint32 nof_tiks, core_clock_speed;
    uint64 reg64_val;
    uint64 pps64, nof_tiks64;
    int fmac_index, fmac_inner_link; 
    SOCDNX_INIT_FUNC_DEFS;
    
    SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_index, &fmac_inner_link));

    SOCDNX_IF_ERR_EXIT(READ_FMAC_TX_CELL_LIMITr(unit, fmac_index, &reg64_val));
    *burst =  soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_COUNTf) + 2 ;
    nof_tiks = soc_reg64_field32_get(unit, FMAC_TX_CELL_LIMITr, reg64_val, CELL_LIMIT_PERIODf);

    if (*burst == 0 || nof_tiks == 0) {
        *pps = 0;
        *burst = 0;
        SOC_EXIT;
    }
    
    
    core_clock_speed = SOC_DPP_CONFIG(unit)->arad->init.core_freq.frequency;
    COMPILER_64_SET(pps64, 0, *burst);
    COMPILER_64_UMUL_32(pps64, core_clock_speed); 
    COMPILER_64_UMUL_32(pps64, 1000); 

    COMPILER_64_SET(nof_tiks64, 0, nof_tiks);
    COMPILER_64_UDIV_64(pps64, nof_tiks64);

    *pps = COMPILER_64_LO(pps64); 

exit:
    SOCDNX_FUNC_RETURN;
}

int
  soc_jer_port_reference_clock_set(
      int                   unit,
      soc_port_t            port
   )
{
    soc_dcmn_init_serdes_ref_clock_t    nif_clk_freq;

    SOCDNX_INIT_FUNC_DEFS;

    
#ifdef BCM_QUX_SUPPORT
    if (SOC_IS_QUX(unit)) {
        SOC_QUX_NIF_PLL_TYPE    qux_pll_type;
        SOCDNX_IF_ERR_EXIT(soc_qux_port_pll_type_get(unit, port, &qux_pll_type));
        switch (qux_pll_type) {
            case SOC_QUX_NIF_PLL_TYPE_PML:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[0];
                break;
            case SOC_QUX_NIF_PLL_TYPE_PMX:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmx_out;
                break;
            default:
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
        }
    } else
#endif
    {
        SOC_JER_NIF_PLL_TYPE     jer_pll_type;
        SOCDNX_IF_ERR_EXIT(soc_jer_port_pll_type_get(unit, port, &jer_pll_type));
        switch (jer_pll_type) {
            case SOC_JER_NIF_PLL_TYPE_PMH:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pmh_out;
                break;
            case SOC_JER_NIF_PLL_TYPE_PML0:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[0];
                break;
            case SOC_JER_NIF_PLL_TYPE_PML1:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_pml_out[1];
                break;
            case SOC_JER_NIF_PLL_TYPE_FABRIC0:
                nif_clk_freq = SOC_DPP_CONFIG(unit)->jer->pll.ref_clk_fabric_out[0];
                break;
            default:
                SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
        }
    }

    
    if(nif_clk_freq == soc_dcmn_init_serdes_ref_clock_125) {
        SOC_INFO(unit).port_refclk_int[port] = 125;
    } else if (nif_clk_freq == soc_dcmn_init_serdes_ref_clock_156_25){
        SOC_INFO(unit).port_refclk_int[port] = 156;
    } else if (nif_clk_freq == soc_dcmn_init_serdes_ref_clock_25){
        SOC_INFO(unit).port_refclk_int[port] = 25;
    } else if (nif_clk_freq == soc_dcmn_init_serdes_ref_clock_disable){
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Can't set Reference clock for port %d, PLL disabled."), port));
    } else {
        SOCDNX_EXIT_WITH_ERR_NO_MSG(SOC_E_PARAM);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_port_header_type_set(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  int                        core_id,
    SOC_SAND_IN  uint32                     tm_port,
    SOC_SAND_IN  SOC_TMC_PORT_DIRECTION     direction_ndx,
    SOC_SAND_IN  SOC_TMC_PORT_HEADER_TYPE   header_type
  )
{
    uint32 channel, offset;
    soc_reg_t reg;
    soc_port_t reg_port;
    soc_reg_above_64_val_t reg_above_64_val;
    uint32 reg32_val[1];
    uint32 phy, lane, flags;
    soc_port_if_t interface_type;
    soc_port_t logical_port;
    uint32 data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(handle_sand_result(arad_port_header_type_set(unit, core_id, tm_port, direction_ndx, header_type)));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core_id, tm_port, &logical_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, logical_port, &flags));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, logical_port, &interface_type));
    SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);

        
    if ((SOC_PORT_IS_NETWORK_INTERFACE(flags)) || (interface_type == SOC_PORT_IF_CPU)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, logical_port, &channel)); 
        if (interface_type == SOC_PORT_IF_CPU) {
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, IRE_CPU_CHANNEL_IS_TDMr, REG_PORT_ANY, 0, reg_above_64_val));
            if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                SHR_BITSET(reg_above_64_val, channel);
            } else {
                SHR_BITCLR(reg_above_64_val, channel);
            }
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, IRE_CPU_CHANNEL_IS_TDMr, REG_PORT_ANY, 0, reg_above_64_val));

            if ((header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) && !SOC_IS_QUX(unit)) {
                SOCDNX_IF_ERR_EXIT(READ_IRE_CPU_TDM_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &data));
                soc_mem_field_set(unit, IRE_CPU_TDM_CTXT_MAPm, &data, TDM_CONTEXTf, (uint32*)&logical_port);
                SOCDNX_IF_ERR_EXIT(WRITE_IRE_CPU_TDM_CTXT_MAPm(unit, MEM_BLOCK_ANY, channel, &data));
            }

        } else {

            if (interface_type == SOC_PORT_IF_ILKN) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, logical_port, 0, &offset));

                if (!SOC_IS_QAX(unit))
                {
                	reg = (offset < 2) ? NBIH_ILKN_CHANNEL_IS_TDM_PORTr : NBIL_ILKN_CHANNEL_IS_TDM_PORTr;
                    reg_port = (offset < 2) ? REG_PORT_ANY : (offset / 4);

                    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, (offset & 1), reg_above_64_val));
                    if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                        SHR_BITSET(reg_above_64_val, channel);
                    } else {
                        SHR_BITCLR(reg_above_64_val, channel);
                    }
                    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, (offset & 1), reg_above_64_val));
                }
                else
                {
                	switch (offset)
                	{
                	case 0:
                		reg = NBIH_ILKN_CHANNEL_IS_TDM_PORT_0r;
                		reg_port = REG_PORT_ANY;
                		break;
                	case 2:
                		reg = NBIH_ILKN_CHANNEL_IS_TDM_PORT_2r;
                		reg_port = REG_PORT_ANY;
                		break;
                	case 1:
                	case 3:
                		reg = NBIL_ILKN_CHANNEL_IS_TDM_PORTr;
                		reg_port = offset >> 1;
                		break;
                	default:
				SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid ILKN offset %d"), offset));          		
                	}

                    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_get(unit, reg, reg_port, 0, reg_above_64_val));
                    if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                        SHR_BITSET(reg_above_64_val, channel);
                    } else {
                        SHR_BITCLR(reg_above_64_val, channel);
                    }
                    SOCDNX_IF_ERR_EXIT(soc_reg_above_64_set(unit, reg, reg_port, 0, reg_above_64_val));
                }

            } else {
                  
                if (!soc_feature(unit, soc_feature_no_tdm)) {
                if ((direction_ndx == SOC_TMC_PORT_DIRECTION_INCOMING) || (direction_ndx == SOC_TMC_PORT_DIRECTION_BOTH)) {
                    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, logical_port, &phy)); 
                    SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit,
                            mbcm_dpp_qsgmii_offsets_remove,
                            (unit, phy, &lane)));
                    lane--; 
                    SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, lane / 4, reg32_val));
                    if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                        data = 1;
                    } else {
                        data = 0;
                    }
                    switch (lane % 4) {
                        case 0:
                            soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_0_QMLF_Nf, data);
                            break;
                        case 1:
                            soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_1_QMLF_Nf, data);
                            break;
                        case 2:
                            soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_2_QMLF_Nf, data);
                            break;
                        case 3:
                            soc_reg_field_set(unit, NBIH_RX_QMLF_CONFIGr, reg32_val, RX_IS_TDM_MLF_3_QMLF_Nf, data);
                            break;
                    }
                    SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_QMLF_CONFIGr, REG_PORT_ANY, lane / 4, *reg32_val));

                    if (core_id) {
                        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, REG_PORT_ANY, 0, reg32_val));
                        if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                            SHR_BITSET(reg32_val, lane / 4);
                        } else {
                            SHR_BITCLR(reg32_val, lane / 4);
                        }
                        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_REQ_PIPE_1_TDM_ENr, REG_PORT_ANY, 0, *reg32_val));
                    } else {
                        SOCDNX_IF_ERR_EXIT(soc_reg32_get(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, reg32_val));
                        if (header_type == SOC_TMC_PORT_HEADER_TYPE_TDM) {
                            SHR_BITSET(reg32_val, lane / 4);
                        } else {
                            SHR_BITCLR(reg32_val, lane / 4);
                        }
                        SOCDNX_IF_ERR_EXIT(soc_reg32_set(unit, NBIH_RX_REQ_PIPE_0_TDM_ENr, REG_PORT_ANY, 0, *reg32_val));
                    }
                }
                }
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_control_tx_nif_enable_set(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int        enable)
{
    uint32 tm_port, base_q_pair, curr_q_pair, nof_q_pairs;
    int core;
    soc_reg_above_64_val_t data_above_64;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port, &nof_q_pairs));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    if (SOC_IS_QAX(unit)) {
	    SOCDNX_IF_ERR_EXIT(READ_ECGM_CGM_QUEUE_IS_DISABLEDr(unit, data_above_64));
	}
	else {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));
    }

    
    for (curr_q_pair = base_q_pair; curr_q_pair < (base_q_pair + nof_q_pairs); curr_q_pair++) {
        if (enable) {
            SHR_BITCLR(data_above_64, curr_q_pair); 
            SHR_BITCLR(data_above_64, ARAD_EGR_NOF_BASE_Q_PAIRS+curr_q_pair); 
        } else {
            SHR_BITSET(data_above_64, curr_q_pair); 
            SHR_BITSET(data_above_64, ARAD_EGR_NOF_BASE_Q_PAIRS+curr_q_pair); 
        }
    }

    if (SOC_IS_QAX(unit)) {
        SOCDNX_IF_ERR_EXIT(WRITE_ECGM_CGM_QUEUE_IS_DISABLEDr(unit, data_above_64));
	}
	else {
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_control_tx_nif_enable_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_OUT int        *enable)
{
    uint32 tm_port, base_q_pair;
    int core;
    soc_reg_above_64_val_t data_above_64;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port, &base_q_pair);
    SOCDNX_IF_ERR_EXIT(rv);

    SOC_REG_ABOVE_64_CLEAR(data_above_64);
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_QUEUE_IS_DISABLEDr(unit, core, data_above_64));

    
    *enable = !(SHR_BITGET(data_above_64, base_q_pair));

exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_nrdy_th_profile_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_IN uint32 profile)
{
    uint32 tm_port, egr_if;
    soc_reg_above_64_val_t data;
    uint32 th_profile;
    uint32 flags;
    int core;
    soc_port_if_t interface;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;

    SOCDNX_INIT_FUNC_DEFS; 
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags)); 
    if (SOC_PORT_IS_STAT_INTERFACE(flags) || SOC_PORT_IS_ELK_INTERFACE(flags))
    {
        SOC_EXIT;
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    if ((interface == SOC_PORT_IF_ILKN) && (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON)) {
        SOC_SAND_SET_BIT(egr_if, 0, 0); 
    }
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
    th_profile = profile;
    soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, &th_profile);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

    if((interface == SOC_PORT_IF_LBG) || 
      ((interface == SOC_PORT_IF_ILKN) && (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON))) {
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), (egr_if+1), data));    
    }


exit:
    SOCDNX_FUNC_RETURN;
}


int
soc_jer_port_nrdy_th_profile_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *profile)
{
    uint32 tm_port, egr_if;
    soc_reg_above_64_val_t data;
    int core;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
    soc_mem_field_get(unit, EGQ_PER_IFC_CFGm, data, NRDY_TH_SELf, profile);

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value)
{
    uint32 num_of_lanes, if_rate_mbps;
    soc_port_if_t interface;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OAMP) {
        *value = 27;
    } else if (interface == SOC_PORT_IF_OLP) {
        *value = 14;
    } else if (interface == SOC_PORT_IF_RCY) {
        *value = 230;
    } else if (interface == SOC_PORT_IF_QSGMII) {
        *value = 26;
    } else {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

        if (num_of_lanes == 1) {
            if (if_rate_mbps <= 12500) {
                *value = 112;
            } else  { 
                *value = 96;
            }

        } else if (num_of_lanes == 2) {
            if (if_rate_mbps <= 12500) {
                *value = 240;
            } else if (if_rate_mbps <= 25000) {
                *value = 224;
            } else if (if_rate_mbps < 50000) {
                *value = 192;
            }else { 
                *value = 128;
            }

        } else if (num_of_lanes == 4) {
            if (if_rate_mbps <= 12500) {
                *value = 496;
            } else if (if_rate_mbps <= 25000) {
                *value = 480;
            } else if (if_rate_mbps < 50000) {
                *value = 448;
            } else { 
                *value = 384;
            }
        } else if (interface == SOC_PORT_IF_ILKN) {
            if (if_rate_mbps <= 150000) {
                *value = 384;
            } else { 
                *value = 256;
            }
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration")));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_port_mirrored_channel_and_context_map(int unit, int core, uint32 termination_context,
                                             uint32 reassembly_context, uint32 channel)
{
    uint32 ctxt_id, index, base_ctxt_id, data, ctxt_channelized, ctxt_addr;
    soc_reg_above_64_val_t data_above_64;

    SOCDNX_INIT_FUNC_DEFS;
 
    base_ctxt_id = SOC_JER_RCY_CTXT_MAP_START(unit, core);
    ctxt_id = base_ctxt_id + channel;
    index = SOC_JER_RCY_INTERFACE(unit, core);

    data = 0;
    ctxt_channelized = 0;
    ctxt_addr = SOC_JER_INVALID_CTXT_ADDRESS;
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_IS_CHANNELIZEDf, &ctxt_channelized);
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, NIF_PORT_TO_CTXT_BIT_MAP_ADDRESSf, &ctxt_addr);  
    soc_mem_field_set(unit, IRE_PORT_TO_BASE_ADDRESS_MAPm, &data, CTXT_MAP_BASE_ADDRESSf, &base_ctxt_id);  
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_PORT_TO_BASE_ADDRESS_MAPm(unit, MEM_BLOCK_ANY, index, &data));


    data = 0;
    soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, REASSEMBLY_CONTEXTf, &reassembly_context);
    soc_mem_field_set(unit, IRE_CTXT_MAPm, &data, PORT_TERMINATION_CONTEXTf, &termination_context);
    SOCDNX_IF_ERR_EXIT(WRITE_IRE_CTXT_MAPm(unit, MEM_BLOCK_ANY, ctxt_id, &data));

    
    if (!SOC_IS_QAX(unit)) {
        if (reassembly_context != ARAD_PORTS_IF_UNMAPPED_INDICATION) {
            SOC_REG_ABOVE_64_CLEAR(data_above_64);
            SOCDNX_IF_ERR_EXIT(READ_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
            if(core == 1) {
                SHR_BITSET(data_above_64, reassembly_context);
            } else { 
                SHR_BITCLR(data_above_64, reassembly_context);
            }
            SOCDNX_IF_ERR_EXIT(WRITE_IDR_REASSEMBLY_CONTEXT_PIPE_MAPr(unit, data_above_64));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}





soc_error_t 
soc_jer_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_64);

            
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, GLOBAL_TAG_SWAP_N_SIZEf, ports_swap_info->global_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_OFFSET_0f, ports_swap_info->tag_swap_n_offset_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_OFFSET_1f, ports_swap_info->tag_swap_n_offset_1);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_0_TAG_SWAP_N_SIZEf, ports_swap_info->tpid_0_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_1_TAG_SWAP_N_SIZEf, ports_swap_info->tpid_1_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_TPID_0f, ports_swap_info->tag_swap_n_tpid_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_N_TPID_1f, ports_swap_info->tag_swap_n_tpid_1);

    
    for (i = 0 ; i < SOC_DPP_DEFS_GET(unit, nof_cores) ; i++) {
        rv = WRITE_IRE_TAG_SWAP_CONFIGURATIONr_REG64(unit, i, reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_jer_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_IRE_TAG_SWAP_CONFIGURATIONr_REG64(unit, 0, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);


            
    ports_swap_info->global_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, GLOBAL_TAG_SWAP_N_SIZEf);   
    ports_swap_info->tag_swap_n_offset_0    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_OFFSET_0f);  
    ports_swap_info->tag_swap_n_offset_1    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_OFFSET_1f);
    ports_swap_info->tpid_0_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_0_TAG_SWAP_N_SIZEf);
    ports_swap_info->tpid_1_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_1_TAG_SWAP_N_SIZEf);
    ports_swap_info->tag_swap_n_tpid_0      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_TPID_0f);
    ports_swap_info->tag_swap_n_tpid_1      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_N_TPID_1f);

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_jer_port_fabric_detach(int unit, soc_port_t port)
{
    int rc, is_first_link, i, fsrd_block_, fmac_block_, fmac_inner_link;
    int blk;
    soc_info_t          *si;
    
    SOCDNX_INIT_FUNC_DEFS;

    si  = &SOC_INFO(unit);

    SOC_PBMP_PORT_REMOVE(si->sfi.bitmap, port);
    SOC_PBMP_PORT_REMOVE(si->port.bitmap, port);
    SOC_PBMP_PORT_REMOVE(si->all.bitmap, port);

    SOC_PBMP_PORT_ADD(si->sfi.disabled_bitmap, port);
    SOC_PBMP_PORT_ADD(si->port.disabled_bitmap, port);
    SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);

    rc = portmod_port_remove(unit, port);
    SOCDNX_IF_ERR_EXIT(rc);

    rc = soc_jer_port_first_link_in_fsrd_get(unit, port , &is_first_link, 0);

    if (is_first_link == 1) 
    {
        fsrd_block_ = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port)/SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

        SOCDNX_IF_ERR_EXIT(soc_jer_fabric_port_to_fmac(unit, port, &fmac_block_, &fmac_inner_link));

        for (i = fmac_block_; i < fmac_block_ +  SOC_DPP_DEFS_GET(unit, nof_quads_in_fsrd) ; i++)
        {
            blk = si->fmac_block[fmac_block_];
            si->block_valid[blk] = 0;
        }

        if (!SOC_IS_QAX(unit)) {

            blk = si->fsrd_block[fsrd_block_];
            si->block_valid[blk] = 0;

            rc = soc_jer_port_update_fsrd_block(unit, port, 0);
            SOCDNX_IF_ERR_EXIT(rc);

        }
    }


    exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_port_first_link_in_fsrd_get(int unit, soc_port_t port, int *is_first_link, int enable)
{
    int fsrd_block, i;
    int counter_mask_fsrd_bmp = 0;
    pbmp_t mask_fsrd_bmp;

    SOCDNX_INIT_FUNC_DEFS;

    SOC_PBMP_CLEAR(mask_fsrd_bmp);

    fsrd_block = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port)/SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

    for (i=fsrd_block*SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd) ; i<fsrd_block*SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd) + SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd); i++) 
    {
        SOC_PBMP_PORT_ADD(mask_fsrd_bmp, SOC_DPP_FABRIC_LINK_TO_PORT(unit,i));
    }

    SOC_PBMP_AND(mask_fsrd_bmp, PBMP_SFI_ALL(unit)); 

    SOC_PBMP_COUNT(mask_fsrd_bmp, counter_mask_fsrd_bmp);

    

    if (counter_mask_fsrd_bmp==0) 
    {
        *is_first_link = 1;
    }
    else
    {
        *is_first_link = 0;
    }

    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_jer_port_update_fsrd_block(int unit, soc_port_t port, int enable)
{
    int fsrd_block;
    uint32 field_val;
    uint64 reg_val64;
    SOCDNX_INIT_FUNC_DEFS;

    fsrd_block =  SOC_DPP_FABRIC_PORT_TO_LINK(unit, port)/SOC_DPP_DEFS_GET(unit, nof_links_in_fsrd);

    SOCDNX_IF_ERR_EXIT(READ_ECI_BLOCKS_POWER_DOWNr(unit, &reg_val64));

    field_val = enable? 0: 1;

    switch (fsrd_block)
    {
        case 0:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_8f, field_val);
            break;
        case 1:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_9f, field_val);
            break;
        case 2:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_10f, field_val);
            break;
        case 3:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_11f, field_val);
            break;
        case 4:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_12f, field_val);
            break;
        case 5:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_13f, field_val);
            break;
        case 6:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_14f, field_val);
            break;
        case 7:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_15f, field_val);
            break;
        case 8:
            soc_reg64_field32_set(unit, ECI_BLOCKS_POWER_DOWNr, &reg_val64, BLOCKS_POWER_DOWN_16f, field_val);
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("invalid fsrd block")));
            break;
    }

    SOCDNX_IF_ERR_EXIT(WRITE_ECI_BLOCKS_POWER_DOWNr(unit, reg_val64));

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_port_oam_ts_init(int                   unit,
                                     soc_port_t            port)
{
    uint32                       res;
    uint32                       reg32;
    uint32                       timestamp_and_ssp = 0;
    SOCDNX_INIT_FUNC_DEFS;

    res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.timestamp_and_ssp.get(unit,port,&timestamp_and_ssp);
    if (timestamp_and_ssp) {
        res = READ_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, &reg32);
        SOCDNX_SAND_IF_ERR_RETURN(res);
        reg32 = soc_reg_field_get(unit, IHB_BUILD_OAM_TS_HEADERr, reg32, BUILD_OAM_TS_HEADERf);
        soc_reg_field_set(unit, IHB_BUILD_OAM_TS_HEADERr, &reg32, BUILD_OAM_TS_HEADERf, reg32 | 0x4);
        res = WRITE_IHB_BUILD_OAM_TS_HEADERr(unit, SOC_CORE_ALL, reg32);
        SOCDNX_SAND_IF_ERR_RETURN(res);

        res = WRITE_IHB_OAM_SUB_TYPE_MAPr(unit, SOC_CORE_ALL,  0x10);
        SOCDNX_SAND_IF_ERR_RETURN(res);
        res = soc_jer_pp_oam_init_eci_tod(unit, 0, 1);
        SOCDNX_SAND_IF_ERR_RETURN(res);
    }
    SOCDNX_FUNC_RETURN;
}



soc_error_t soc_jer_port_timestamp_and_ssp_update(int                   unit,
                                                  soc_port_t            port)
{
    uint32                       res;
    uint32                       prog_editor_value;
    int                          core = 0;
    uint32                       tm_port = 0;
    uint32                       pp_port = 0;
    uint32                       base_q_pair,nof_pairs,curr_q_pair; 
    ARAD_EGQ_PCT_TBL_DATA        pct_tbl_data;
    ARAD_PP_EPNI_PP_PCT_TBL_DATA tbl_data;
    uint32                       timestamp_and_ssp = 0;
    uint32                       tm_profile = 0;


     SOCDNX_INIT_FUNC_DEFS;

     res = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.timestamp_and_ssp.get(unit,port,&timestamp_and_ssp);
     if (timestamp_and_ssp) {
     res = arad_egr_prog_editor_profile_get(unit, ARAD_PRGE_TM_SELECT_TIMESTAMP_AND_SSP, &tm_profile);
     prog_editor_value = timestamp_and_ssp;
     res = soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core);
     SOCDNX_SAND_IF_ERR_RETURN(res); 
     res = soc_port_sw_db_local_to_pp_port_get(unit, port, &pp_port, &core);
     SOCDNX_SAND_IF_ERR_RETURN(res);    
     
     res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, core, tm_port, &base_q_pair);
     SOCDNX_SAND_IF_ERR_RETURN(res); 
     res = soc_port_sw_db_tm_port_to_out_port_priority_get(unit, core, tm_port, &nof_pairs);
     SOCDNX_SAND_IF_ERR_RETURN(res); 
     
     
     for (curr_q_pair = base_q_pair; curr_q_pair < base_q_pair + nof_pairs; curr_q_pair++) {
     
         res = arad_egq_pct_tbl_get_unsafe(
               unit,
               core,
               curr_q_pair,
               &pct_tbl_data
             );
         SOCDNX_SAND_IF_ERR_RETURN(res); 
     
         pct_tbl_data.prog_editor_value = prog_editor_value; 
         pct_tbl_data.prog_editor_profile = tm_profile;
     
         res = arad_egq_pct_tbl_set_unsafe(
               unit,
               core,
               curr_q_pair,
               &pct_tbl_data
             );
         SOCDNX_SAND_IF_ERR_RETURN(res); 
     }
    if (tm_port) {
         res = arad_egq_pct_tbl_get_unsafe(unit,core,tm_port,&pct_tbl_data);
         SOCDNX_SAND_IF_ERR_RETURN(res); 
         pct_tbl_data.prog_editor_value = prog_editor_value; 
         pct_tbl_data.prog_editor_profile = tm_profile;
         res = arad_egq_pct_tbl_set_unsafe(unit, core,tm_port,&pct_tbl_data); 
        SOCDNX_SAND_IF_ERR_RETURN(res);
    }
    
    res = arad_pp_epni_pp_pct_tbl_get_unsafe(unit, core,  pp_port, &tbl_data);
    SOCDNX_SAND_IF_ERR_RETURN(res);

    tbl_data.prge_profile = 0;

    res = arad_pp_epni_pp_pct_tbl_set_unsafe(unit, core, pp_port, &tbl_data);
    SOCDNX_SAND_IF_ERR_RETURN(res);
    }
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

