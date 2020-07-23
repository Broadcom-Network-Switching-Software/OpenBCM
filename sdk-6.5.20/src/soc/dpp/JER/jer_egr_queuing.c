/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: jer_egr_queuing.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_EGRESS
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/types.h>
#include <soc/register.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_ofp_rates.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/ARAD/arad_egr_queuing.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#define SOC_JER_EGR_MAX_ALPHA_VALUE (7)
#define SOC_JER_EGR_MIN_ALPHA_VALUE (-7)

typedef struct soc_jer_egr_if_s {
    uint32 egr_if_in_use[SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES)];
} soc_jer_egr_if_t;

STATIC soc_jer_egr_if_t jer_egr_if[SOC_MAX_NUM_DEVICES][SOC_DPP_DEFS_MAX(NOF_CORES)];

int 
soc_jer_egr_interface_init(int unit) 
{
    soc_pbmp_t ports_bm;
    int core;
    uint32 egr_if;
    soc_port_t port;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        for(egr_if=0 ; egr_if<SOC_DPP_IMP_DEFS_MAX(NOF_CORE_INTERFACES) ; egr_if++) {
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = 0;
        }
    }

    
#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable)
    {
        for (egr_if = SOC_QAX_EGR_IF_LBG_MIN; egr_if < (SOC_QAX_EGR_IF_LBG_MAX+1) ; egr_if++) {
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = 1;
        }

        if (SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            egr_if = SOC_QAX_EGR_IF_LBG_RESERVE;
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = 1;

            egr_if = SOC_QAX_EGR_IF_LBG_RESERVE2;
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = 1;
        }
    }
#endif

    if (SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm));

        SOC_PBMP_ITER(ports_bm, port) {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
            SOCDNX_IF_ERR_EXIT(rv);

            if(egr_if == INVALID_EGR_INTERFACE) {
                continue;
            }

            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
            jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] + 1;
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_egr_interface_alloc(int unit, soc_port_t port) 
{
    int core, is_channelized, high_rate_port=0, is_tdm_queuing = 0;
    uint32 rate_mps, egr_if = 0, other_egr_if = 0;
    soc_port_if_t interface_type;
    soc_port_t master_port;
    int start, end, i;
    uint32     flags;
    uint32 ilkn_oversub_enable = 0;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
#ifdef BCM_LB_SUPPORT
    soc_pbmp_t modem_pbmp;
#endif
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_master_channel_get(unit, port, &master_port));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port, (uint8*)&is_tdm_queuing));
    if (SOC_IS_QAX(unit))
    {
        ilkn_oversub_enable = soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ilkn_oversub_enable", 0);
    }

    if((master_port == port) || SOC_PORT_IS_LB_MODEM(flags)) { 
         
         switch(interface_type) {
            case SOC_PORT_IF_CPU:
                egr_if = SOC_JER_EGR_IF_CPU;
                break;

            case SOC_PORT_IF_OLP:
                egr_if = SOC_JER_EGR_IF_OLP;
                break;

            case SOC_PORT_IF_OAMP:
                egr_if = SOC_JER_EGR_IF_OAMP;
                break;

            case SOC_PORT_IF_RCY:
                egr_if = SOC_JER_EGR_IF_RCY;
                break;

            case SOC_PORT_IF_SAT:
                egr_if = SOC_JER_EGR_IF_SAT;
                break;

            case SOC_PORT_IF_IPSEC:
                egr_if = SOC_JER_EGR_IF_IPSEC;
                break;

#ifdef BCM_LB_SUPPORT
            case SOC_PORT_IF_LBG:
                SOCDNX_IF_ERR_EXIT(qax_lb_lbg_egr_interface_get_by_port(unit, port, &egr_if));
                break;
#endif

            case SOC_PORT_IF_ILKN:
                egr_if = INVALID_EGR_INTERFACE;
                
                other_egr_if = 0;
                if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
                    if (IS_TDM_PORT(unit, port) || is_tdm_queuing) {
                        if ((jer_egr_if[unit][core].egr_if_in_use[0] == 0) && (jer_egr_if[unit][core].egr_if_in_use[1] == 0)){
                            egr_if = 0;
                            other_egr_if = 1;
                        } else if ((jer_egr_if[unit][core].egr_if_in_use[16] == 0) && (jer_egr_if[unit][core].egr_if_in_use[17] == 0)) {
                            egr_if = 16;
                            other_egr_if = 17;
                        } else if (((jer_egr_if[unit][core].egr_if_in_use[2] == 0) && (jer_egr_if[unit][core].egr_if_in_use[3] == 0)) && SOC_IS_QAX(unit)) {
                            egr_if = 2;
                            other_egr_if = 3;
                        } else if (((jer_egr_if[unit][core].egr_if_in_use[4] == 0) && (jer_egr_if[unit][core].egr_if_in_use[5] == 0)) && SOC_IS_QAX(unit)) {
                            egr_if = 4;
                            other_egr_if = 5;
                        } else {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface, 2 interfaces are needed in ILKN dedicated mode")));
                        }
                    } else {
                        if ((jer_egr_if[unit][core].egr_if_in_use[0] == 0) && (jer_egr_if[unit][core].egr_if_in_use[1] == 0)){
                            egr_if = 1;
                            other_egr_if = 0;
                        } else if ((jer_egr_if[unit][core].egr_if_in_use[16] == 0) && (jer_egr_if[unit][core].egr_if_in_use[17] == 0)) {
                            egr_if = 17;
                            other_egr_if = 16;
                        } else if (((jer_egr_if[unit][core].egr_if_in_use[2] == 0) && (jer_egr_if[unit][core].egr_if_in_use[3] == 0)) && SOC_IS_QAX(unit)) {
                            egr_if = 3;
                            other_egr_if = 2;
                        } else if (((jer_egr_if[unit][core].egr_if_in_use[4] == 0) && (jer_egr_if[unit][core].egr_if_in_use[5] == 0)) && SOC_IS_QAX(unit)) {
                            egr_if = 5;
                            other_egr_if = 4;
                        } else {
                            SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface,  2 interfaces are needed in ILKN dedicated mode")));
                        }
                    }

                    if (jer_egr_if[unit][core].egr_if_in_use[other_egr_if] != 0) {
                        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface, 2 interfaces are needed in ILKN dedicated mode")));
                    }
                }
                else
                {
                    
                    if (ilkn_oversub_enable)
                    {
                        if (jer_egr_if[unit][core].egr_if_in_use[0] == 0){
                            egr_if = 0;
                        } else if (jer_egr_if[unit][core].egr_if_in_use[16] == 0) {
                            egr_if = 16;
                        }
                    }
                }
                break;

             default:
                egr_if = INVALID_EGR_INTERFACE;
                break;
        }

#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->link_bonding_enable) 
        {
            if (SOC_PORT_IS_LB_MODEM(flags)) {
                SOCDNX_IF_ERR_EXIT(qax_lb_lbg_egr_interface_alloc(unit, port, &egr_if)); 
            }

            if (SOC_DPP_CONFIG(unit)->qax->lbi_en && (egr_if == INVALID_EGR_INTERFACE)) {
                SOCDNX_IF_ERR_EXIT(qax_lb_modem_ports_on_same_interface_get(unit, port, &modem_pbmp)); 
                if (SOC_PBMP_NOT_NULL(modem_pbmp)) {
                    egr_if = SOC_QAX_EGR_IF_LBG_RESERVE;
                }
            }            
        }
#endif

        if(egr_if == INVALID_EGR_INTERFACE) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_channelized_port_get(unit, port, &is_channelized));
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &rate_mps));
            high_rate_port = ((rate_mps >= 100000) || (SOC_IS_QUX(unit) && (rate_mps >= 10000))) ? 1 : 0;

            
            if(!high_rate_port && !is_channelized) {
                start = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces) + SOC_DPP_IMP_DEFS_GET(unit, nof_special_non_channelized_interfaces);
                end = SOC_DPP_IMP_DEFS_GET(unit, nof_core_interfaces);

                for(i=start ; i<end ; i++) {
                	  if ((SOC_JER_EGR_IF_CPU == i) || (SOC_JER_EGR_IF_OLP == i) ||
                        (SOC_JER_EGR_IF_OAMP == i) || (SOC_JER_EGR_IF_RCY == i) ||
                        (SOC_JER_EGR_IF_SAT == i) || (SOC_JER_EGR_IF_IPSEC == i)) {
                        continue;
                    }
                    if(jer_egr_if[unit][core].egr_if_in_use[i] == 0) {
                        egr_if = i;
                        break;
                    }
                }
            }

            
            if(egr_if == INVALID_EGR_INTERFACE) {
                start = 0;
                end = SOC_DPP_IMP_DEFS_GET(unit, nof_channelized_interfaces) - SOC_DPP_IMP_DEFS_GET(unit, nof_special_channelized_interfaces);

                for(i=start ; i<end ; i++) {
                    
                    if ((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && (i == 0 || i == 1 || i == 16 || i== 17)) {
                        continue;
                    }
                    if ((ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) && SOC_IS_QAX(unit) && (i == 2 || i == 3 || i == 4 || i== 5)) {
                        continue;
                    }

                    if(jer_egr_if[unit][core].egr_if_in_use[i] == 0) {
                        if(ilkn_oversub_enable && (i == 0 || i == 16)) {
                            continue;
                        }
                        egr_if = i;
                        break;
                    }
                }
            }

            if(egr_if == INVALID_EGR_INTERFACE) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Failed to allocate egr interface for port %d"), port));
            }
        }
    } else {
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, master_port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN) {
            SOC_SAND_SET_BIT(egr_if, 0x0, 0); 
            if (!(IS_TDM_PORT(unit, port) || is_tdm_queuing))
            {
                egr_if +=1; 
            }

        }
    }

    if (interface_type != SOC_PORT_IF_LBG) {
        jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] + 1;
    }
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.set(unit, port, egr_if);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_egr_interface_free(int unit, soc_port_t port) 
{
    uint32 egr_if;
    int core;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
    SOCDNX_IF_ERR_EXIT(rv);

    if(egr_if != INVALID_EGR_INTERFACE) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));
        jer_egr_if[unit][core].egr_if_in_use[egr_if] = jer_egr_if[unit][core].egr_if_in_use[egr_if] - 1;
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int
  soc_jer_egr_port2egress_offset(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              tm_port,
    SOC_SAND_OUT uint32              *egr_if
  )
{
    soc_port_t
        port;
    soc_error_t rv;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_tm_to_local_port_get(unit, core, tm_port, &port));
    rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, egr_if);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

int
    soc_jer_egr_is_channelized(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core, 
    SOC_SAND_IN  uint32                tm_port, 
    SOC_SAND_OUT uint32                *is_channalzied)
{
    uint32 reg_val[1], egr_if;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));

    
    if(egr_if < 32 ) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_IFC_IS_CHANNELIZEDr(unit, core, reg_val));
        if(SHR_BITGET(reg_val, egr_if)) {
            *is_channalzied = 1;
        } else {
            *is_channalzied = 0;
        }
    } else {
        *is_channalzied = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_nrdy_th_profile_data_set(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_IN uint32 profile_data)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    data = 0;
    
    if (profile <= 2) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_0_2r(unit, core, &data));
        if (profile == 0) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_0f, profile_data);
        } else if (profile == 1) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_1f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_0_2r, &data, NRDY_TH_2f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_0_2r(unit, core, data));
    }

    else if (profile >= 3 && profile <= 5) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_3_5r(unit, core, &data));
        if (profile == 3) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_3f, profile_data);
        } else if (profile == 4) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_4f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_3_5r, &data, NRDY_TH_5f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_3_5r(unit, core, data));
    }

    else if (profile >= 6 && profile <= 8) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_6_8r(unit, core, &data));
        if (profile == 6) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_6f, profile_data);
        } else if (profile == 7) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_7f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_6_8r, &data, NRDY_TH_8f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_6_8r(unit, core, data));
    }

    else if (profile >= 9 && profile <= 11) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_9_11r(unit, core, &data));
        if (profile == 9) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_9f, profile_data);
        } else if (profile == 10) {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_10f, profile_data);
        } else {
            soc_reg_field_set(unit, EGQ_NRDY_TH_9_11r, &data, NRDY_TH_11f, profile_data);
        }
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_NRDY_TH_9_11r(unit, core, data));
    }

    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Invalid profile id %d "), profile));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_nrdy_th_profile_data_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN int core,
    SOC_SAND_IN uint32 profile,
    SOC_SAND_OUT uint32 *profile_data)
{
    uint32 data;
    SOCDNX_INIT_FUNC_DEFS;

    data = 0;
    
    if (profile <= 2) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_0_2r(unit, core, &data));
        if (profile == 0) {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_0f);
        } else if (profile == 1) {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_1f);
        } else {
            *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_0_2r, data, NRDY_TH_2f);
        }
    }

    else if (profile >= 3 && profile <= 5) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_3_5r(unit, core, &data));
        if (profile == 3) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_3f);
        } else if (profile == 4) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_4f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_3_5r, data, NRDY_TH_5f);
        }
    }

    else if (profile >= 6 && profile <= 8) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_6_8r(unit, core, &data));
        if (profile == 6) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_6f);
        } else if (profile == 7) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_7f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_6_8r, data, NRDY_TH_8f);
        }
    }

    else if (profile >= 9 && profile <= 11) {
        SOCDNX_IF_ERR_EXIT(READ_EGQ_NRDY_TH_9_11r(unit, core, &data));
        if (profile == 9) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_9f);
        } else if (profile == 10) {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_10f);
        } else {
             *profile_data = soc_reg_field_get(unit, EGQ_NRDY_TH_9_11r, data, NRDY_TH_11f);
        }
    }

    else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("Invalid profile id %d "), profile));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_jer_egr_tm_init(int unit)
{   
    soc_pbmp_t pbmp;
    soc_port_t port_i;
    uint32 tm_port, base_q_pair, priority_mode, is_master, tdm_egress_priority, tdm_egress_dp;
    uint32 reg_val, fld_val, flags;
    SOC_TMC_EGR_PORT_SHAPER_MODE shaper_mode;
    int core, is_tdm_queuing = 0;
    soc_port_if_t interface_type;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_reg_above_64_val_t reg_val_above_64, fld_val_above_64;
    soc_error_t rv;
    SOC_TMC_EGR_OFP_SCH_INFO ofp_sch_info;

    SOCDNX_INIT_FUNC_DEFS;

    
    reg_val = 0;
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, PQP_CALENDER_SWITCH_ENf, 1);
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, FQP_CALENDER_SWITCH_ENf, 1);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, SOC_CORE_ALL, reg_val));

    SOCDNX_IF_ERR_EXIT(arad_egr_queuing_init(unit, SOC_DPP_CONFIG(unit)->arad->init.eg_cgm_scheme));

    
    

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, 0, &pbmp));

    SOC_PBMP_ITER(pbmp, port_i) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port_i, &flags));
        if (SOC_PORT_IS_NOT_VALID_FOR_EGRESS_TM(flags)) {
            continue;
        }
       
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port_i, &tm_port, &core));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.base_q_pair.get(unit, port_i, &base_q_pair);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_out_port_priority_get(unit, port_i, &priority_mode));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.shaper_mode.get(unit, port_i, &shaper_mode);
        SOCDNX_IF_ERR_EXIT(rv);
        

        
        SOCDNX_IF_ERR_EXIT(arad_egr_dsp_pp_to_base_q_pair_set(unit, core,  tm_port, base_q_pair));

        
        SOCDNX_IF_ERR_EXIT(arad_egr_dsp_pp_priorities_mode_set(unit,core, tm_port, priority_mode));

        
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_dsp_pp_shaper_mode_set_unsafe(unit, core, tm_port, shaper_mode));

        
        SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_map_set(unit, port_i, 0 , 0 ));

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port_i, &is_master));
        if (is_master) {
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port_i, &interface_type));
            if (interface_type == SOC_PORT_IF_ILKN) { 
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 1, 1));
            } else if (interface_type == SOC_PORT_IF_CAUI) {
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 0, 1));
            } else {
                SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fast_port_set(unit, port_i, 0, 0));
            }
        }
        
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port_i, (uint8*)&is_tdm_queuing));
        if (IS_TDM_PORT(unit, port_i) || is_tdm_queuing) {
            SOC_TMC_EGR_OFP_SCH_INFO_clear(&ofp_sch_info);
            SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_scheduling_get(unit, core, tm_port, &ofp_sch_info));
            ofp_sch_info.nif_priority = SOC_TMC_EGR_OFP_INTERFACE_PRIO_HIGH;
            SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_scheduling_set(unit, core, tm_port, &ofp_sch_info));
        }
    }

    
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
    tdm_egress_priority = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_priority;
    tdm_egress_dp = SOC_DPP_CONFIG(unit)->arad->init.tdm_egress_dp;

    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {

        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            
            fld_val = 0x1;
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_0_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_1_INTERLEAVE_ENf,  fld_val));
            if (SOC_IS_QAX(unit)) {
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_2_INTERLEAVE_ENf,  fld_val));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_2_INTERLEAVE_ENf,  fld_val));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_ILAKEN_3_INTERLEAVE_ENf,  fld_val));
                SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EPNI_TDM_EPE_CONFIGURATIONr, core, 0, TDM_ILAKEN_3_INTERLEAVE_ENf,  fld_val));
            }

            
            SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, EGRESS_TDM_MODEf,  fld_val));
        }

        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_PKT_TCf, tdm_egress_priority));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_TDM_GENERAL_CONFIGURATIONr, core, 0, TDM_PKT_DPf, tdm_egress_dp));
    }

    
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOC_REG_ABOVE_64_CLEAR(reg_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITSET_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAG_Lf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,NIF_NO_FRAG_Lf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITSET_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, NIF_NO_FRAG_Hf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,NIF_NO_FRAG_Hf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, CPU_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,CPU_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, OLP_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,OLP_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, OAM_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,OAM_NO_FRAGf,fld_val_above_64);

        SOC_REG_ABOVE_64_CLEAR(fld_val_above_64);
        SHR_BITCLR_RANGE(fld_val_above_64, 0, soc_reg_field_length(unit, EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr, RCY_NO_FRAGf));
        soc_reg_above_64_field_set(unit,EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr,reg_val_above_64,RCY_NO_FRAGf,fld_val_above_64);

        SOCDNX_IF_ERR_EXIT(WRITE_EPNI_EGRESS_INTERFACE_NO_FRAGMENTATION_MODE_CONFIGURATIONr(unit, core, reg_val_above_64));
    }

    
    if (SOC_IS_QAX(unit) && soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "ilkn_oversub_enable", 0))
    {
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, 0, 0, ILAKEN_0_OVERSUB_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, 0, 0, ILAKEN_1_OVERSUB_ENf,  0x1));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
int is_all_equal(uint32 *numbers, uint32 nof_numbers){
    uint32 i, comparator = numbers[0];
    for (i = 1; i < nof_numbers; i++) {
        if (numbers[i] != comparator) {
            return FALSE;
        }
    }
    return TRUE;
}


STATIC
void get_smallest_idx(uint32 *numbers, uint32 nof_numbers, uint32 *smallest_idx){
    int i;
    *smallest_idx = 0;
    for (i = 1; i < nof_numbers; i++) {
        if (numbers[i] < numbers[*smallest_idx]) {
            *smallest_idx = i;
        }
    }
}


STATIC
uint32 get_last_value(uint32 *arr, uint32 arr_length){
    uint32 i, last_idx = 0;

    for(i = 0; i < arr_length; i++){
        if(arr[i] != 0){
            last_idx = i;
        }
    }

    return last_idx;
}

STATIC
int soc_jer_egr_lcm_above_sum_get(int unit, uint32 *numbers, uint32 nof_numbers, uint32 sum, uint32 *lcm){
    uint32 curr_iter[SOC_PBMP_PORT_MAX] = {0}, current_lcm = 0, smallest_idx = 0;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(numbers);
    sal_memcpy(curr_iter, numbers, nof_numbers * sizeof(curr_iter[0]));

    while (current_lcm < sum) {
        if(is_all_equal(curr_iter, nof_numbers)){
            if (current_lcm != curr_iter[0]) {
                current_lcm = curr_iter[0]; 
                continue;
            }
        }
        get_smallest_idx(curr_iter, nof_numbers, &smallest_idx);
        curr_iter[smallest_idx] += numbers[smallest_idx];
    }

    *lcm = current_lcm;
exit:
    SOCDNX_FUNC_RETURN;    
}




STATIC
int soc_jer_egr_q_nif_rate_to_lanes(int unit, uint32 rate, uint32 *lanes)
{
	SOCDNX_INIT_FUNC_DEFS;
	SOCDNX_NULL_CHECK(lanes);
	
    if (rate == 0) 
    {   
       *lanes = 0;
    }

    
    if(rate > 0 && rate <= 10000){
        *lanes = 1;
    } else if (rate > 10000 && rate <= 20000) {
        *lanes = 2;
    } else if (rate > 20000 && rate <= 100000) {
        *lanes = 4;
    } else if (rate > 100000 && rate <= 140000) {
        *lanes = 8;
    } else if (rate > 140000) {
       *lanes = 16;
    }   

exit:
    SOCDNX_FUNC_RETURN; 
}


#define SOC_JER_CAL_LANES_OTHERS   4 
#define SOC_JER_CAL_LANES_OLP      1 
#define SOC_JER_CAL_LANES_OAMP     1 
#define SOC_JER_CAL_LANES_CPU      1 
#define SOC_JER_CAL_LANES_RCY      1 
#define SOC_JER_CAL_LANES_SAT      4 
#define SOC_JER_CAL_LANES_IPSEC    4 

#define SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED   28
#define SOC_JER_FQP_CAL_IF_OTHERS_GIVE                29
#define SOC_JER_FQP_CAL_IF_1_GIG_NIFS                 30
#define SOC_JER_FQP_CAL_IF_INVALID                    31

#define SOC_JER_PQP_CAL_IF_1_GIG_NIFS                 129
#define SOC_JER_PQP_CAL_IF_INVALID                    128

#define SOC_JER_CAL_ILLEGAL_NIF                       0xfff

#define SOC_JER_MAX_CAL_LENGTH                        256

int
  soc_jer_egr_q_nif_cal_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN int core,
    SOC_SAND_IN soc_jer_egr_cal_type_t cal_to_update
  )
{
    soc_port_t port;
    uint32 egr_if, flags, is_master, other_pqp_to_fill;
    uint32 if_rate_mbps, total_lanes_sum = 0, lane_rate;
    soc_pbmp_t port_bm;
    uint32  calendar_length, nof_padding_if;
    int i, j = 0, nof_active_if = 0;
    int is_special_int = 0;
    soc_field_t cal_set_field;
    soc_mem_t cal_mem;
    uint32 reg_val, current_cal, next_cal, next_cal_offset;
    uint32 lanes[SOC_PBMP_PORT_MAX] = {0}, lcm_temp[SOC_PBMP_PORT_MAX] = {0};
    uint32 lanes_temp_others = 0, lanes_temp_rate_others = 0;
    uint32 lanes_temp_cpu = 1, lanes_temp_olp = 1;
    uint32 lanes_temp_oamp = 1, lanes_temp_rcy = 1, lanes_temp_sat = 1, lanes_temp_ipsec = 1;
    uint32 lanes_temp_lb = 1;
    ARAD_OFP_RATES_CAL_SCH *calendar = NULL;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_port_if_t interface_type;
    uint32 *temp_buff = NULL;
    uint32 lanes_temp = 0, num_of_lanes;
    uint32 if_1g_idx = (cal_to_update == socJerCalTypeFQP ? SOC_JER_FQP_CAL_IF_1_GIG_NIFS : SOC_JER_PQP_CAL_IF_1_GIG_NIFS);
    uint32 dummy_start = 0;
    uint32 tdm_egr_if_mask = 0xfffe;
#ifdef BCM_LB_SUPPORT
    uint32 lbg_reserve_exist = 0;
#endif
    uint32 fqp_padding_value = 0;

    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_ALLOC(calendar, ARAD_OFP_RATES_CAL_SCH, 1,"calendar egr_q_nif_cal");
    SOCDNX_ALLOC(temp_buff, uint32, SOC_JER_MAX_CAL_LENGTH, "temp_buff");

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_core_get(unit, core, 0, &port_bm));

    SOC_PBMP_ITER(port_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        is_special_int = 0;

        if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
            
            continue;
        }
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            continue;
        }
        if (SOC_PORT_IS_LB_MODEM(flags)){
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(!is_master) {
            continue;
        }
        
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));

#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) 
    {
        if (egr_if == SOC_QAX_EGR_IF_LBG_RESERVE) {
            lbg_reserve_exist = 1;
            continue;
        }
    }
#endif

        switch (interface_type) {
            case SOC_PORT_IF_NOCXN:
            {
                continue;
            }
            case SOC_PORT_IF_ERP:
            {
                continue;
            }
            case SOC_PORT_IF_CPU:
            case SOC_PORT_IF_OLP:
            case SOC_PORT_IF_OAMP:
            case SOC_PORT_IF_RCY:
            case SOC_PORT_IF_SAT:
            case SOC_PORT_IF_IPSEC:
            {
                is_special_int = 1;
                break;
            }
            case SOC_PORT_IF_LBG:
            default:
            {
                break;
            }
        }

        
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;;
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON) {
            if (interface_type == SOC_PORT_IF_ILKN) {
               egr_if = egr_if&tdm_egr_if_mask;
            }
        }
        
        
        if (!(is_special_int)) 
        {
            
            SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

            lane_rate = if_rate_mbps / num_of_lanes;

            if (lane_rate >= 20000) { 
                lanes_temp = 2 * num_of_lanes;
            } else if ((interface_type == SOC_PORT_IF_DNX_XAUI) || (interface_type == SOC_PORT_IF_RXAUI)) {
                lanes_temp = SOC_SAND_MAX(if_rate_mbps/10000, 1);
            } else {
                lanes_temp = num_of_lanes;
            }
        }

        
        switch (interface_type) {
            case SOC_PORT_IF_ILKN:
            {
                
                lanes[egr_if] = lanes_temp;
                lanes[egr_if] +=  (lanes[egr_if] % 4 == 0 ? 0 : 4 - (lanes[egr_if] % 4));
                total_lanes_sum += lanes[egr_if];
                break;
            }
            case SOC_PORT_IF_SGMII:
            case SOC_PORT_IF_QSGMII:
            {
                
                lanes[if_1g_idx] += lanes_temp;
                break;
            }
            case SOC_PORT_IF_CPU:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_cpu);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_OLP:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_olp);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_OAMP:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_oamp);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_RCY:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_rcy);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_SAT:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_sat);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_IPSEC:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps,&lanes_temp_ipsec);
                lanes_temp_rate_others += if_rate_mbps;
                break;
            }
            case SOC_PORT_IF_LBG:
            {
                soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_lb);
                total_lanes_sum += lanes_temp_lb; 
                lanes[egr_if] = lanes_temp_lb;
                if (cal_to_update == socJerCalTypeFQP) {
                    total_lanes_sum += lanes_temp_lb; 
                    lanes[egr_if+1] = lanes_temp_lb;
                }
                break;
            }
            default:
            {
                total_lanes_sum += lanes_temp; 
                lanes[egr_if] = lanes_temp;
                break;
            }
        }
    }

    
    if(lanes[if_1g_idx] > 0 && lanes[if_1g_idx] < 10){
        lanes[if_1g_idx] = 10;
    } else if (lanes[if_1g_idx] > 10 && lanes[if_1g_idx] < 20) {
        lanes[if_1g_idx] = 20;
    } else if (lanes[if_1g_idx] > 20 && lanes[if_1g_idx] < 40) {
        lanes[if_1g_idx] = 40;
    } else if (lanes[if_1g_idx] > 40 && lanes[if_1g_idx] < 80) {
        lanes[if_1g_idx] = 80;
    } else if (lanes[if_1g_idx] > 80 && lanes[if_1g_idx] < 160) {
        lanes[if_1g_idx] = 160;
    }
    lanes[if_1g_idx] /= 10;
    total_lanes_sum += lanes[if_1g_idx];

#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en && (lbg_reserve_exist == 1))
    {
        if_rate_mbps = 10000;
        soc_jer_egr_q_nif_rate_to_lanes(unit, if_rate_mbps, &lanes_temp_lb);
        total_lanes_sum += lanes_temp_lb; 
        lanes[SOC_QAX_EGR_IF_LBG_RESERVE] = lanes_temp_lb;
    }
#endif

    
    if(cal_to_update == socJerCalTypeFQP) { 
        soc_jer_egr_q_nif_rate_to_lanes(unit, lanes_temp_rate_others, &lanes_temp_others);
        lanes_temp_others = (lanes_temp_others > 4 ? lanes_temp_others : 4);
        total_lanes_sum += lanes_temp_others;
        lanes[SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED] = lanes_temp_others;
    } else  {

        total_lanes_sum += lanes_temp_cpu;
        lanes[SOC_JER_EGR_IF_CPU] = lanes_temp_cpu;

        total_lanes_sum += lanes_temp_olp;
        lanes[SOC_JER_EGR_IF_OLP] = lanes_temp_olp;

        total_lanes_sum += lanes_temp_oamp;
        lanes[SOC_JER_EGR_IF_OAMP] = lanes_temp_oamp;

        total_lanes_sum += lanes_temp_rcy;
        lanes[SOC_JER_EGR_IF_RCY] = lanes_temp_rcy;

        if (SOC_IS_QAX(unit)) {
            total_lanes_sum += lanes_temp_sat;
            lanes[SOC_JER_EGR_IF_SAT] = lanes_temp_sat;

            total_lanes_sum += lanes_temp_ipsec;
            lanes[SOC_JER_EGR_IF_IPSEC] = lanes_temp_ipsec;
        }
    }

    
    for (i = 0; i < SOC_PBMP_PORT_MAX; i++) {
        if (lanes[i] != 0) {
            nof_active_if++;
        }
    }
    for (i = 0, j = 0; i < SOC_PBMP_PORT_MAX; i++) {
        if (lanes[i] != 0) {
            lcm_temp[j++] = lanes[i];
        }
    }
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_lcm_above_sum_get(unit, lcm_temp, nof_active_if, total_lanes_sum, &calendar_length)); 

    
    nof_padding_if = calendar_length - total_lanes_sum;

    dummy_start = get_last_value(lanes, SOC_PBMP_PORT_MAX) + 1;

    for (i = 0; i < nof_padding_if; i++) {
        lanes[dummy_start + i] = 1;
    }

    
    SOCDNX_IF_ERR_EXIT(arad_ofp_rates_fixed_len_cal_build(unit, lanes, dummy_start + nof_padding_if + 1, calendar_length, SOC_JER_MAX_CAL_LENGTH, 1, 0, calendar->slots));

    
    if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "enhanced_egress_calendar_config", 0) == TRUE) 
    {
        
        fqp_padding_value = SOC_JER_FQP_CAL_IF_INVALID;
    }
    else
    {
        
        fqp_padding_value = SOC_JER_FQP_CAL_IF_OTHERS_GIVE_BW_IF_NEEDED;
    }

    
    other_pqp_to_fill = SOC_JER_EGR_IF_MIN;
    for (i = 0; i < calendar_length; i++)
    {
        if (calendar->slots[i] >= dummy_start) {
            if (cal_to_update == socJerCalTypeFQP) { 
                calendar->slots[i] = fqp_padding_value;
            } else { 
                calendar->slots[i] = other_pqp_to_fill;
                other_pqp_to_fill = (other_pqp_to_fill == SOC_JER_EGR_IF_MAX) ?
                    SOC_JER_EGR_IF_MIN : other_pqp_to_fill+1;
            }
        }
    }
    
    for (i=calendar_length; i<SOC_JER_MAX_CAL_LENGTH; i++) {
        if (cal_to_update == socJerCalTypePQP) {
            calendar->slots[i] = SOC_JER_PQP_CAL_IF_INVALID; 
        } else {
            calendar->slots[i] = SOC_JER_FQP_CAL_IF_INVALID;
        }
    }

    
    if (cal_to_update == socJerCalTypeFQP) { 
        cal_set_field = FQP_CALENDER_SET_SELf;
        cal_mem = EGQ_FQP_NIF_PORT_MUXm;
    } else  {
        cal_set_field = PQP_CALENDER_SET_SELf;
        cal_mem = EGQ_PQP_NIF_PORT_MUXm;
    }

    SOCDNX_IF_ERR_EXIT(READ_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, core, &reg_val));
    current_cal = soc_reg_field_get(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, reg_val, cal_set_field);
    next_cal = current_cal ? 0 : 1;
    next_cal_offset = next_cal ? SOC_JER_MAX_CAL_LENGTH : 0;

#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QUX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en && (lbg_reserve_exist == 1) &&
        (cal_to_update == socJerCalTypeFQP)) {
        int num_of_reserve = 0, slot_idx = 0;
        uint32 ifc_slots[SOC_JER_MAX_CAL_LENGTH];
        for (i = 0; i < SOC_JER_MAX_CAL_LENGTH; i++) {
            if (SOC_QAX_EGR_IF_LBG_RESERVE == calendar->slots[i]) {
                num_of_reserve++;
            }
            ifc_slots[i] = calendar->slots[i];
        }
        if ((calendar_length + num_of_reserve) > SOC_JER_MAX_CAL_LENGTH) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Illegal calendar len")));
        }
        for (i = 0; i < SOC_JER_MAX_CAL_LENGTH; i++) {
            calendar->slots[slot_idx++] = ifc_slots[i];
            if (SOC_QAX_EGR_IF_LBG_RESERVE == calendar->slots[i]) {
                calendar->slots[slot_idx++] = SOC_QAX_EGR_IF_LBG_RESERVE2;
            }
        }
    }
#endif

    for (i=0; i<SOC_JER_MAX_CAL_LENGTH; i++) {
        SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, cal_mem, EGQ_BLOCK(unit, core), i+next_cal_offset, &(calendar->slots[i])));
    }

    
    soc_reg_field_set(unit, EGQ_PQP_AND_FQP_CALENDER_SETTINGr, &reg_val, cal_set_field, next_cal);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQP_AND_FQP_CALENDER_SETTINGr(unit, core, reg_val));

exit:
    SOC_FREE(temp_buff);
    SOC_FREE(calendar);
    SOCDNX_FUNC_RETURN;
}

int
  soc_jer_egr_q_fqp_scheduler_config(
    SOC_SAND_IN int unit
  )
{
    soc_pbmp_t port_bm;
    uint32 flags, is_master, if_rate_mbps, egr_if, field_val;
    soc_port_t port;
    int core, is_tdm_queuing = 0;
    soc_reg_above_64_val_t data;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    soc_port_if_t interface_type;
    soc_error_t rv;

    SOCDNX_INIT_FUNC_DEFS;
  
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_RCY_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OAM_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OLP_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_DEL_REQ_ENf,  0x1));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_CPU_REQ_ENf,  0x1));

        
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_RCY_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OAM_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_OLP_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_DEL_HIGHER_ENf,  0x0));
        SOCDNX_IF_ERR_EXIT(soc_reg_above_64_field32_modify(unit, EGQ_FQP_SCHEDULER_CONFIGURATIONr, core, 0, CFG_CPU_HIGHER_ENf,  0x0));
    }

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_valid_ports_get(unit, SOC_PORT_FLAGS_NETWORK_INTERFACE, &port_bm));

    SOC_PBMP_ITER(port_bm, port) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));

        if (SOC_PORT_IS_STAT_INTERFACE(flags)) {
            
            continue;
        }
        if (SOC_PORT_IS_ELK_INTERFACE(flags)){
            continue;
        }
        if (SOC_PORT_IS_LB_MODEM(flags)){
            continue;
        }
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_master_get(unit, port, &is_master));
        if(!is_master) {
            continue;
        }

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
        rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port, (uint8*)&is_tdm_queuing));
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_core_get(unit, port, &core));

        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

        field_val = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS) ? 0 : 1; 
        soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &field_val);

        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));


        
        ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface_type));
        
        if (ilkn_tdm_dedicated_queuing == ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON && interface_type == SOC_PORT_IF_ILKN) {
            if (IS_TDM_PORT(unit, port) || is_tdm_queuing) {
                egr_if++;
            } else {
                egr_if--;
            }

            
            SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));

            field_val = (if_rate_mbps >= ARAD_EGR_QUEUEING_HIGER_REQ_MAX_RATE_MBPS) ? 0 : 1; 
            soc_mem_field_set(unit, EGQ_PER_IFC_CFGm, data, HIGHER_REQ_EN_PER_MALf, &field_val);

            SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit ,core), egr_if, data));
        }
    }
exit:
    SOCDNX_FUNC_RETURN;

}

int
  soc_jer_egr_q_nif_cal_set_all(
    SOC_SAND_IN int unit
  )
{
    int core;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_fqp_scheduler_config(unit));
    SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_nif_cal_set(unit, core, socJerCalTypeFQP));
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_q_nif_cal_set(unit, core, socJerCalTypePQP));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


int 
soc_jer_egr_q_fast_port_set(
        SOC_SAND_IN  int unit, 
        SOC_SAND_IN  soc_port_t port, 
        SOC_SAND_IN  int is_ilkn, 
        SOC_SAND_IN  int turn_on
        )
{
    uint32 egress_offset, data, fld_val, tm_port;
    int core, is_tdm_queuing = 0;
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core)); 

    SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egress_offset));

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

    
    fld_val = (turn_on ? 1 : 0);
    soc_EGQ_PER_IFC_CFGm_field32_set(unit, &data, FAST_PORT_ENf, fld_val);

    
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

    
    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;
    SOCDNX_IF_ERR_EXIT(sw_state_access[unit].dpp.soc.jericho.tm.is_tdm_queuing_on.pbmp_member(unit, port, (uint8*)&is_tdm_queuing));
    if (is_ilkn && ilkn_tdm_dedicated_queuing) {
        if (IS_TDM_PORT(unit, port) || is_tdm_queuing) {
            egress_offset++;
        } else {
            egress_offset--;
        }

        
        SOCDNX_IF_ERR_EXIT(READ_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));

        
        soc_EGQ_PER_IFC_CFGm_field32_set(unit, &data, FAST_PORT_ENf, fld_val);

        
        SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PER_IFC_CFGm(unit, EGQ_BLOCK(unit, core), egress_offset, &data));
    }

exit:
    SOCDNX_FUNC_RETURN; 
}

STATIC
uint32 _soc_jer_convert_alpha_to_value(int unit, int alpha_value, uint32* field_val)
{
    SOCDNX_INIT_FUNC_DEFS;
    if (alpha_value > SOC_JER_EGR_MAX_ALPHA_VALUE || alpha_value < SOC_JER_EGR_MIN_ALPHA_VALUE)
    {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid alpha value %d "), alpha_value));
    }

    else if (alpha_value == 0)
    {
        *field_val = 0;
    }
    else if (alpha_value > 0)
    {
        *field_val = alpha_value;
    }
    else
    {
        *field_val = -1*alpha_value;
        *field_val |= 0x8; 
    }

exit:
    SOCDNX_FUNC_RETURN
}

uint32 _soc_jer_convert_field_val_to_alpha(int unit, uint32 field_val, int* alpha_value)
{
    uint32 abs_value = 0;
    SOCDNX_INIT_FUNC_DEFS;
    abs_value = field_val & 0x7; 
    *alpha_value = abs_value;
    if (field_val & 0x8)
    {
        *alpha_value = *alpha_value * (-1);
    }

    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_queuing_sp_tc_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_IN    int   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    soc_reg_above_64_val_t reg;
    uint32 alpha_field_val;

    SOCDNX_INIT_FUNC_DEFS;
    if (threshold_type == soc_dpp_cosq_threshold_data_buffers)
    {
        
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers_max, threshold_value, drop_type));
        
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers_min, threshold_value, drop_type));
        
        SOCDNX_IF_ERR_EXIT(soc_jer_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_thereshold_data_buffers_alpha, 0, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_packet_descriptors || threshold_type == soc_dpp_cosq_threshold_available_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));
    }
  
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_max)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_set_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers, threshold_value, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_min)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_0_MINIMUMLIMITf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_1_MINIMUMLIMITf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_2_MINIMUMLIMITf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_3_MINIMUMLIMITf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_4_MINIMUMLIMITf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_5_MINIMUMLIMITf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_6_MINIMUMLIMITf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_7_MINIMUMLIMITf, threshold_value);
                    break;
            }
        }
        else  
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_0_MINIMUMLIMITf, threshold_value);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_1_MINIMUMLIMITf, threshold_value);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_2_MINIMUMLIMITf, threshold_value);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_3_MINIMUMLIMITf, threshold_value);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_4_MINIMUMLIMITf, threshold_value);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_5_MINIMUMLIMITf, threshold_value);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_6_MINIMUMLIMITf, threshold_value);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_7_MINIMUMLIMITf, threshold_value);
                    break;              
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
    }
    else if (threshold_type == soc_dpp_cosq_thereshold_data_buffers_alpha)
    {
        SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, threshold_value, &alpha_field_val));
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));

        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_0_ALPHAf, alpha_field_val);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_1_ALPHAf, alpha_field_val);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_2_ALPHAf, alpha_field_val);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_3_ALPHAf, alpha_field_val);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_4_ALPHAf, alpha_field_val);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_5_ALPHAf, alpha_field_val);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_6_ALPHAf, alpha_field_val);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_7_ALPHAf, alpha_field_val);
                    break;
            }
        }
        else 
        {
            switch (tc)
            {
                case 0:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_0_ALPHAf, alpha_field_val);
                    break;
                case 1:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_1_ALPHAf, alpha_field_val);
                    break;
                case 2:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_2_ALPHAf, alpha_field_val);
                    break;
                case 3:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_3_ALPHAf, alpha_field_val);
                    break;
                case 4:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_4_ALPHAf, alpha_field_val);
                    break;
                case 5:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_5_ALPHAf, alpha_field_val);
                    break;
                case 6:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_6_ALPHAf, alpha_field_val);
                    break;
                case 7:
                    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_7_ALPHAf, alpha_field_val);
                    break;
            }
        }
        SOCDNX_IF_ERR_EXIT(WRITE_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32
  soc_jer_egr_queuing_sp_tc_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    tc,
    SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type,
    SOC_SAND_OUT   int*   threshold_value,
    SOC_SAND_IN    soc_dpp_cosq_threshold_global_type_t drop_type
  )
{
    soc_reg_above_64_val_t reg;
    uint32 field_val = 0;

    SOCDNX_INIT_FUNC_DEFS;
    if (threshold_type == soc_dpp_cosq_threshold_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));  
    }
    else if (threshold_type == soc_dpp_cosq_threshold_packet_descriptors || threshold_type == soc_dpp_cosq_threshold_available_data_buffers)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, threshold_type, threshold_value, drop_type));
    }
  
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_max)
    {
        SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sp_tc_drop_get_unsafe(unit, core, tc, soc_dpp_cosq_threshold_data_buffers, threshold_value, drop_type));
    }
    else if (threshold_type == soc_dpp_cosq_threshold_data_buffers_min)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_0_MINIMUMLIMITf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_1_MINIMUMLIMITf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_2_MINIMUMLIMITf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_3_MINIMUMLIMITf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_4_MINIMUMLIMITf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_5_MINIMUMLIMITf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_6_MINIMUMLIMITf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_7_MINIMUMLIMITf);
                    break;
            }
        }
        else  
        {
            switch (tc)
            {
                case 0:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_0_MINIMUMLIMITf);
                    break;
                case 1:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_1_MINIMUMLIMITf);
                    break;
                case 2:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_2_MINIMUMLIMITf);
                    break;
                case 3:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_3_MINIMUMLIMITf);
                    break;
                case 4:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_4_MINIMUMLIMITf);
                    break;
                case 5:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_5_MINIMUMLIMITf);
                    break;
                case 6:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_6_MINIMUMLIMITf);
                    break;
                case 7:
                    *threshold_value = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_7_MINIMUMLIMITf);
                    break;              
            }
        }
    }
    else if (threshold_type == soc_dpp_cosq_thereshold_data_buffers_alpha)
    {
        SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr(unit, core, reg));

        if (drop_type == soc_dpp_cosq_threshold_global_type_service_pool_0)
        {
            switch (tc)
            {
                case 0:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_0_ALPHAf);
                    break;
                case 1:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_1_ALPHAf);
                    break;
                case 2:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_2_ALPHAf);
                    break;
                case 3:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_3_ALPHAf);
                    break;
                case 4:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_4_ALPHAf);
                    break;
                case 5:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_5_ALPHAf);
                    break;
                case 6:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_6_ALPHAf);
                    break;
                case 7:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_0_TC_7_ALPHAf);
                    break;
            }
        }
        else 
        {
            switch (tc)
            {
                case 0:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_0_ALPHAf);
                    break;
                case 1:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_1_ALPHAf);
                    break;
                case 2:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_2_ALPHAf);
                    break;
                case 3:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_3_ALPHAf);
                    break;
                case 4:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_4_ALPHAf);
                    break;
                case 5:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_5_ALPHAf);
                    break;
                case 6:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_6_ALPHAf);
                    break;
                case 7:
                    field_val = soc_reg_above_64_field32_get(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_ALPHAr, reg, DB_SERVICE_POOL_1_TC_7_ALPHAf);
                    break;
            }
        }
        SOCDNX_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val, threshold_value));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


uint32 
  soc_jer_egr_queuing_sch_unsch_drop_set_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_IN    SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 field_32;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_min;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_MC_PD_SHARED_MIN_THf, &field_32);

    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_alpha, &field_32));
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_MC_PD_SHARED_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_sch_unsch_drop_set_unsafe(unit, core, threshold_type, dev_thresh));
    
exit:
    SOCDNX_FUNC_RETURN;
}



uint32 
  soc_jer_egr_queuing_sch_unsch_drop_get_unsafe(
    SOC_SAND_IN    int    unit,
    SOC_SAND_IN    int    core,
    SOC_SAND_IN    int    threshold_type,
    SOC_SAND_OUT   SOC_TMC_EGR_QUEUING_DEV_TH *dev_thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 field_32;

    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_MC_PD_SHARED_MIN_THf, &field_32);
    dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_min = field_32;

    soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_MC_PD_SHARED_ALPHAf, &field_32);

    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_32, (int *)&dev_thresh->thresh_type[threshold_type].mc_shared.descriptors_alpha));
    
exit:
    SOCDNX_FUNC_RETURN;
}

uint32
jer_egr_threshold_types_verify(
   SOC_SAND_IN    int   unit,
   SOC_SAND_IN    soc_dpp_cosq_threshold_type_t threshold_type)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (threshold_type)
    {
        
        case soc_dpp_cosq_threshold_bytes:
        case soc_dpp_cosq_threshold_packet_descriptors:
        case soc_dpp_cosq_threshold_packets:
        case soc_dpp_cosq_threshold_data_buffers:
        case soc_dpp_cosq_threshold_available_packet_descriptors:
        case soc_dpp_cosq_threshold_available_data_buffers:
        case soc_dpp_cosq_threshold_buffer_descriptor_buffers:
        case soc_dpp_cosq_threshold_buffer_descriptors:
        case soc_dpp_cosq_threshold_dbuffs:
        case soc_dpp_cosq_threshold_full_dbuffs:
        case soc_dpp_cosq_threshold_mini_dbuffs:
        case soc_dpp_cosq_threshold_dynamic_weight:
        case soc_dpp_cosq_threshold_packet_descriptors_min:
        case soc_dpp_cosq_threshold_packet_descriptors_max:
        case soc_dpp_cosq_threshold_packet_descriptors_alpha:
        case soc_dpp_cosq_threshold_data_buffers_min:
        case soc_dpp_cosq_threshold_data_buffers_max:
        case soc_dpp_cosq_thereshold_data_buffers_alpha:
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Invalid threshold type %d "), threshold_type));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

STATIC
  uint32
    soc_jer_egr_unsched_drop_q_pair_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  uint32                drop_precedence,
    SOC_SAND_IN  ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  uint32
    offset;
  soc_reg_above_64_val_t data;
  uint32 field_val;
  SOCDNX_INIT_FUNC_DEFS;

  SOCDNX_NULL_CHECK(thresh_info);

  if(thresh_info->packet_descriptors > SOC_DPP_DEFS_GET(unit, egq_qdct_pd_max_val))
  {
      SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("packet descriptors is above max value")));
  }
  
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  SOCDNX_IF_ERR_EXIT(READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_MC_PD_MIN_TH_DP_0f + drop_precedence, thresh_info->packet_descriptors_min);

  SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, thresh_info->packet_descriptors_alpha, &field_val));
  soc_EGQ_QDCT_TABLEm_field32_set(unit, data, QUEUE_MC_PD_ALPHA_DP_0f + drop_precedence, field_val);
  SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
exit:
  SOCDNX_FUNC_RETURN;
}



uint32
  soc_jer_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  int                 profile,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  ARAD_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *exact_thresh
  )
{
    uint32 egress_tc;
    ARAD_EGR_THRESH_INFO thresh_info;

    SOCDNX_INIT_FUNC_DEFS;

    egress_tc = prio_ndx;

    
    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_unsched_drop_set_unsafe(unit, core, profile ,prio_ndx, dp_ndx, thresh, exact_thresh));

    
   
    arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);
    thresh_info.packet_descriptors_min = thresh->queue_pkts_consumed_min[profile];
    thresh_info.packet_descriptors_alpha = thresh->queue_pkts_consumed_alpha[profile];
    SOCDNX_IF_ERR_EXIT(soc_jer_egr_unsched_drop_q_pair_thresh_set_unsafe(
               unit,
               core,
               egress_tc,
               profile,
               dp_ndx,
               &thresh_info
           ));
       

    exact_thresh->queue_pkts_consumed_min[profile] = thresh_info.packet_descriptors_min;
    exact_thresh->queue_pkts_consumed_alpha[profile] = thresh_info.packet_descriptors_alpha;
  

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_sched_port_fc_thresh_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  int                threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH *thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 field_32;
    SOCDNX_INIT_FUNC_DEFS;

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = thresh->packet_descriptors_min;
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_MIN_FC_THf, &field_32);
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, thresh->packet_descriptors_alpha, &field_32));
    soc_EGQ_PDCT_TABLEm_field_set(unit, mem, PORT_UC_PD_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_PQST_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    field_32 = thresh->data_buffers_min;
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_MIN_FC_THf, &field_32);
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, thresh->data_buffers_alpha, &field_32));
    soc_EGQ_PQST_TABLEm_field_set(unit, mem, PORT_UC_DB_ALPHAf, &field_32);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_PQST_TABLEm(unit, EGQ_BLOCK(unit, core), threshold_type, mem));
    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_sched_port_fc_thresh_set_unsafe(unit, core, threshold_type, thresh));

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_sched_q_fc_thresh_set_unsafe(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  int       core,
    SOC_SAND_IN  int       prio,
    SOC_SAND_IN  int threshold_type,
    SOC_SAND_IN  SOC_TMC_EGR_FC_OFP_THRESH  *thresh
  )
{
    soc_reg_above_64_val_t mem;
    uint32 offset;
    uint32 field_val;
    ARAD_EGR_THRESH_INFO thresh_info;
    SOCDNX_INIT_FUNC_DEFS;

    offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type, prio);

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));
    soc_EGQ_QDCT_TABLEm_field32_set(unit, mem, QUEUE_UC_PD_MIN_FC_THf, thresh->packet_descriptors_min);
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, thresh->packet_descriptors_alpha, &field_val));
    soc_EGQ_QDCT_TABLEm_field32_set(unit, mem, QUEUE_UC_PD_ALPHAf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));

    
    SOCDNX_IF_ERR_EXIT(READ_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));
    soc_EGQ_QQST_TABLEm_field32_set(unit, mem, QUEUE_UC_DB_MIN_FC_THf, thresh->data_buffers_min);
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, thresh->data_buffers_alpha, &field_val));
    soc_EGQ_QQST_TABLEm_field32_set(unit, mem, QUEUE_UC_DB_ALPHAf, field_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, mem));
    
    thresh_info.dbuff = thresh->data_buffers;
    thresh_info.packet_descriptors = thresh->packet_descriptors;

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_ofp_fc_q_pair_thresh_set_unsafe(
               unit,
               core,
               prio,
               threshold_type,
               &thresh_info));

exit:
    SOCDNX_FUNC_RETURN;

}

uint32	
  soc_jer_egr_queuing_if_fc_uc_set_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_IN ARAD_EGR_QUEUING_IF_UC_FC	*info
  )
{
    soc_reg_above_64_val_t reg_pd_min, reg_db_min;
    uint32 reg_pd_alpha, reg_db_alpha, field_val1, field_val2;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr(unit, core, reg_pd_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr(unit, core, &reg_pd_alpha));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr(unit, core, reg_db_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr(unit, core, &reg_db_alpha));

    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, info->size256_th_alpha, &field_val1));
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_alpha_to_value(unit, info->pd_th_alpha, &field_val2));
	
    switch(uc_if_profile_ndx)
    {
        case 0:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_0f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_0f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_0f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_0f, field_val1);
            break;
        case 1:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_1f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_1f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_1f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_1f, field_val1);
            break;
        case 2:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_2f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_2f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_2f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_2f, field_val1);
            break;
        case 3:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_3f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_3f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_3f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_3f, field_val1);
            break;
        case 4:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_4f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_4f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_4f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_4f, field_val1);
            break;
        case 5:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_5f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_5f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_5f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_5f, field_val1);
            break;
        case 6:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_6f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_6f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_6f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_6f, field_val1);
            break;
        case 7:
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_7f, info->pd_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, &reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_7f, field_val2);
            soc_reg_above_64_field32_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_7f, info->size256_th_min);
            soc_reg_field_set(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, &reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_7f, field_val1);
            break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("invalid profile")));
    }

    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr(unit, core, reg_pd_min));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr(unit, core, reg_pd_alpha));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr(unit, core, reg_db_min));
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr(unit, core, reg_db_alpha));

    SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_if_fc_uc_set_unsafe(unit, core, uc_if_profile_ndx, info));

exit:
    SOCDNX_FUNC_RETURN;
}


int	
  soc_jer_egr_queuing_if_fc_uc_get_unsafe(
	SOC_SAND_IN	int	unit,
    SOC_SAND_IN int core,
	SOC_SAND_IN	uint32	uc_if_profile_ndx,
	SOC_SAND_OUT ARAD_EGR_QUEUING_IF_UC_FC	*info
  )
{
    soc_reg_above_64_val_t reg_pd_min, reg_db_min;
    uint32 reg_pd_alpha, reg_db_alpha, field_val1 = 0, field_val2 = 0;
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr(unit, core, reg_pd_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr(unit, core, &reg_pd_alpha));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr(unit, core, reg_db_min));
    SOCDNX_IF_ERR_EXIT(READ_CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr(unit, core, &reg_db_alpha));

	SOCDNX_SAND_IF_ERR_EXIT(arad_egr_queuing_if_fc_uc_get_unsafe(unit, core, uc_if_profile_ndx, info));
	
    switch(uc_if_profile_ndx)
    {
        case 0:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_0f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_0f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_0f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_0f);
            break;
        case 1:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_1f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_1f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_1f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_1f);
            break;
        case 2:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_2f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_2f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_2f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_2f);
            break;
        case 3:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_3f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_3f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_3f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_3f);
            break;
        case 4:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_4f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_4f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_4f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_4f);
            break;
        case 5:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_5f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_5f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_5f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_5f);
            break;
        case 6:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_6f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_6f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_6f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_6f);
            break;
        case 7:
            info->pd_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_MIN_THr, reg_pd_min, CGM_UC_PD_INTERFACE_FC_MIN_TH_7f);
            field_val2 = soc_reg_field_get(unit, CGM_CGM_UC_PD_INTERFACE_FC_ALPHAr, reg_pd_alpha, CGM_UC_PD_INTERFACE_FC_ALPHA_7f);
            info->size256_th_min = soc_reg_above_64_field32_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_MIN_THr, reg_db_min, CGM_UC_SIZE_256_INTERFACE_FC_MIN_TH_7f);
            field_val1 = soc_reg_field_get(unit, CGM_CGM_UC_SIZE_256_INTERFACE_FC_ALPHAr, reg_db_alpha, CGM_UC_SIZE_256_INTERFACE_FC_ALPHA_7f);
            break;
		default:
			SOCDNX_EXIT_WITH_ERR(SOC_E_RESOURCE, (_BSL_SOCDNX_MSG("invalid profile")));
    }

    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val1, (int *)(&info->size256_th_alpha)));
    SOCDNX_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val2, (int *)(&info->pd_th_alpha)));
	
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qux_egr_queuing_init_thresholds(int unit, int port_rate, int nof_priorities, int nof_channels, SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds)
{
    SOCDNX_INIT_FUNC_DEFS;

    cgm_init_thresholds->mc_reserved_pds  = 40; 

    if(port_rate <= 2500)  
    {
        cgm_init_thresholds->threshold_port = 60;
        cgm_init_thresholds->threshold_queue = 30;
        cgm_init_thresholds->port_mc_drop_pds = 347;
        cgm_init_thresholds->port_mc_drop_dbs = 3470;
        switch(nof_priorities)
        {
            case 1:
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 40;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 10;
                break;
            default:
                break;
        }
    } 
    else if(port_rate <= 12500) 
    { 
        cgm_init_thresholds->threshold_port = 136;
        cgm_init_thresholds->threshold_queue = 68;
        cgm_init_thresholds->port_mc_drop_pds = 648;
        cgm_init_thresholds->port_mc_drop_dbs = 6480;
        switch(nof_priorities)
        {
            case 1:
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 80;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 20;
                break;
            default:
                break;      
        }
    }
    else 
    { 
        cgm_init_thresholds->threshold_port = 511;
        cgm_init_thresholds->port_mc_drop_pds = 2431;
        cgm_init_thresholds->port_mc_drop_dbs = 24310;
        cgm_init_thresholds->threshold_queue = 256;
        switch(nof_priorities)
        {
            case 1:
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 320;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 80;
                break;
            default:
                break;
        }
    } 

    cgm_init_thresholds->mc_reserved_pds = cgm_init_thresholds->mc_reserved_pds / nof_channels;
    cgm_init_thresholds->drop_pds_th = 2045;
    cgm_init_thresholds->drop_dbs_th = 2045;
    cgm_init_thresholds->drop_pds_th_tc = 2045;

    SOCDNX_FUNC_RETURN;
}

int soc_jer_egr_queuing_init_thresholds(int unit, int port_rate, int nof_priorities, int nof_channels, SOC_TMC_EGR_QUEUING_CGM_INIT_THRESHOLDS* cgm_init_thresholds)
{
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
        return soc_qux_egr_queuing_init_thresholds(unit, port_rate, nof_priorities, nof_channels, cgm_init_thresholds);
    }

    cgm_init_thresholds->mc_reserved_pds  = 40; 

    if(port_rate <= 2500) 
    { 
        cgm_init_thresholds->threshold_port = 128;
        cgm_init_thresholds->threshold_queue = 128;
        cgm_init_thresholds->port_mc_drop_pds = 135;
        cgm_init_thresholds->port_mc_drop_dbs = 1350;
        cgm_init_thresholds->mc_reserved_pds = 40;
    } 
    else if(port_rate <= 12500) 
    { 
        cgm_init_thresholds->threshold_port = 167;
        cgm_init_thresholds->threshold_queue = 84;
        cgm_init_thresholds->port_mc_drop_pds = 722;
        cgm_init_thresholds->port_mc_drop_dbs = 7220;
        switch(nof_priorities)
        {
            case 1:
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 160;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 40;
                break;
            default:
                break;      
        }
    }
    else if (port_rate <= 30000) 
    {
        cgm_init_thresholds->threshold_port = 500;
        cgm_init_thresholds->threshold_queue = 250;
        cgm_init_thresholds->port_mc_drop_pds = 2167;
        cgm_init_thresholds->port_mc_drop_dbs = 21670;
        switch (nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 500;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 400;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds  = 100;
                break;
            default:
                break;
        }
    } 
    else if(port_rate <= 50000) 
    { 
        cgm_init_thresholds->threshold_port = 500;
        cgm_init_thresholds->threshold_queue = 250;
        cgm_init_thresholds->port_mc_drop_pds = 2167;
        cgm_init_thresholds->port_mc_drop_dbs = 21670;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 500;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 640;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 160;
                break;
            default:
                break;
        }
    } 
    else if(port_rate <= 127000) 
    { 
        cgm_init_thresholds->threshold_port = 2000;
        cgm_init_thresholds->threshold_queue = 1000;
        cgm_init_thresholds->port_mc_drop_pds = 8667;
        cgm_init_thresholds->port_mc_drop_dbs = 86670;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 2000;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 1600;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 400;
                break;
            default:
                break;
        }
    } 
    else  
    { 
        cgm_init_thresholds->threshold_port = 3000;
        cgm_init_thresholds->threshold_queue = 1500;
        cgm_init_thresholds->port_mc_drop_pds = 13000;
        cgm_init_thresholds->port_mc_drop_dbs = 130000;
        switch(nof_priorities)
        {
            case 1:
                cgm_init_thresholds->threshold_queue = 3000;
                break;
            case 2:
                cgm_init_thresholds->mc_reserved_pds = 1600;
                break;
            case 8:
                cgm_init_thresholds->mc_reserved_pds = 1200;
                break;
            default:
                break;
        }
    }

    cgm_init_thresholds->mc_reserved_pds = cgm_init_thresholds->mc_reserved_pds / nof_channels;
    cgm_init_thresholds->drop_pds_th = 6000;
    cgm_init_thresholds->drop_dbs_th = 6000;
    cgm_init_thresholds->drop_pds_th_tc = 4000;

    SOCDNX_FUNC_RETURN;
}


STATIC
  uint32
    soc_jer_egr_ofp_fc_q_pair_thresh_get_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  int                 core,
      SOC_SAND_IN  uint32                 egress_tc,
      SOC_SAND_IN  uint32                 threshold_type,
      SOC_SAND_OUT ARAD_EGR_THRESH_INFO      *thresh_info
    )
{
  uint32
    offset;
  soc_reg_above_64_val_t data;
  uint32 res, field_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  SOC_SAND_CHECK_NULL_INPUT(thresh_info);

  SOC_REG_ABOVE_64_CLEAR(data);
  
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1930, exit, READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  thresh_info->packet_descriptors = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_UC_PD_MAX_FC_THf);
  thresh_info->packet_descriptors_min = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_UC_PD_MIN_FC_THf);
  field_val = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_UC_PD_ALPHAf);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1940, exit, _soc_jer_convert_field_val_to_alpha(unit, field_val, &(thresh_info->packet_descriptors_alpha)));
    
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1950, exit, READ_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  thresh_info->dbuff = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_UC_DB_MAX_FC_THf);
  thresh_info->dbuff_min = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_UC_DB_MIN_FC_THf);
  field_val = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_UC_DB_ALPHAf);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1960, exit, _soc_jer_convert_field_val_to_alpha(unit, field_val, &(thresh_info->dbuff_alpha)));
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_egr_ofp_fc_q_pair_thresh_get_unsafe()",egress_tc,threshold_type); 
}


int
  jer_egr_ofp_fc_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  ARAD_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT ARAD_EGR_FC_OFP_THRESH   *thresh
  )
{
    int res;
    uint32 field_val;
    ARAD_EGR_THRESH_INFO thresh_info;
    soc_reg_above_64_val_t mem;

    SOCDNX_INIT_FUNC_DEFS;

    if(prio_ndx == ARAD_EGR_Q_PRIO_ALL)
    {
    SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_PDCT_TABLEm(unit, EGQ_BLOCK(unit, core), ofp_type_ndx, mem));
        soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_UC_PD_MIN_FC_THf, &thresh->packet_descriptors_min);
        soc_EGQ_PDCT_TABLEm_field_get(unit, mem, PORT_UC_PD_ALPHAf, &field_val);
        SOCDNX_SAND_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val, &(thresh->packet_descriptors_alpha)));

        
        SOCDNX_SAND_IF_ERR_EXIT(READ_EGQ_PQST_TABLEm(unit, EGQ_BLOCK(unit, core), ofp_type_ndx, mem));
        soc_EGQ_PQST_TABLEm_field_get(unit, mem, PORT_UC_DB_MIN_FC_THf, &thresh->data_buffers_min);
        soc_EGQ_PQST_TABLEm_field_get(unit, mem, PORT_UC_DB_ALPHAf, &field_val);
        SOCDNX_SAND_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val, &(thresh->data_buffers_alpha)));
    }
    else
    {
        res = soc_jer_egr_ofp_fc_q_pair_thresh_get_unsafe(unit, core, prio_ndx, ofp_type_ndx, &thresh_info);
        SOCDNX_SAND_IF_ERR_EXIT(res);
        thresh->words = thresh_info.dbuff;
        thresh->data_buffers_min = thresh_info.dbuff_min;
        thresh->data_buffers_alpha = thresh_info.dbuff_alpha;

        thresh->packet_descriptors = thresh_info.packet_descriptors;
        thresh->packet_descriptors_min = thresh_info.packet_descriptors_min;
        thresh->packet_descriptors_alpha = thresh_info.packet_descriptors_alpha;
    }

    res = arad_egr_ofp_fc_get_unsafe(unit, core, prio_ndx, ofp_type_ndx, thresh);
    SOCDNX_SAND_IF_ERR_EXIT(res);


exit:
    SOCDNX_FUNC_RETURN;
}


STATIC
  uint32
    jer_egr_unsched_drop_q_pair_thresh_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  uint32                egress_tc,
    SOC_SAND_IN  uint32                threshold_type,
    SOC_SAND_IN  uint32                drop_precedence,
    SOC_SAND_OUT ARAD_EGR_THRESH_INFO     *thresh_info
  )
{
  soc_reg_above_64_val_t data;
  uint32 offset, field_val;
  
  SOCDNX_INIT_FUNC_DEFS;
  
  
  offset = ARAD_EGQ_QDCT_TABLE_KEY_ENTRY(threshold_type,egress_tc);
  
  
  SOCDNX_IF_ERR_EXIT(READ_EGQ_QDCT_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  thresh_info->packet_descriptors = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_MC_PD_MAX_TH_DP_0f + drop_precedence);
  thresh_info->packet_descriptors_min = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_MC_PD_MIN_TH_DP_0f + drop_precedence);
  field_val = soc_EGQ_QDCT_TABLEm_field32_get(unit, data, QUEUE_MC_PD_ALPHA_DP_0f + drop_precedence);
  SOCDNX_IF_ERR_EXIT(_soc_jer_convert_field_val_to_alpha(unit, field_val, &(thresh_info->packet_descriptors_alpha)));
  
    
  SOCDNX_IF_ERR_EXIT(READ_EGQ_QQST_TABLEm(unit, EGQ_BLOCK(unit, core), offset, data));
  thresh_info->dbuff = soc_EGQ_QQST_TABLEm_field32_get(unit, data, QUEUE_MC_DB_DP_0_THf + drop_precedence);

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  soc_jer_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  ARAD_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT ARAD_EGR_DROP_THRESH     *thresh
  )
{

  ARAD_EGR_THRESH_INFO
    thresh_info;
  uint32
    egress_tc;
  ARAD_EGR_PORT_THRESH_TYPE
    thresh_type;


  SOCDNX_INIT_FUNC_DEFS;

  egress_tc = prio_ndx;

  for (thresh_type = ARAD_EGR_PORT_THRESH_TYPE_0; thresh_type < ARAD_EGR_PORT_NOF_THRESH_TYPES; thresh_type++)
  {     
     arad_ARAD_EGR_THRESH_INFO_clear(&thresh_info);

     SOCDNX_IF_ERR_EXIT(jer_egr_unsched_drop_q_pair_thresh_get_unsafe(
             unit,
             core,
             egress_tc,
             thresh_type,
             dp_ndx,
             &thresh_info));

     thresh->queue_words_consumed[thresh_type] = thresh_info.dbuff;
     thresh->queue_pkts_consumed[thresh_type] = thresh_info.packet_descriptors;
     thresh->queue_pkts_consumed_min[thresh_type] = thresh_info.packet_descriptors_min;
     thresh->queue_pkts_consumed_alpha[thresh_type] = thresh_info.packet_descriptors_alpha;
  }

exit:
  SOCDNX_FUNC_RETURN;
}

uint32 soc_jer_sp_tc_init(int unit, int core, ARAD_EGR_QUEUING_DEV_TH    *info)
{
    soc_reg_above_64_val_t reg;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_0_MINIMUMLIMITf, info->pool_tc[0][0].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_1_MINIMUMLIMITf, info->pool_tc[0][1].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_2_MINIMUMLIMITf, info->pool_tc[0][2].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_3_MINIMUMLIMITf, info->pool_tc[0][3].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_4_MINIMUMLIMITf, info->pool_tc[0][4].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_5_MINIMUMLIMITf, info->pool_tc[0][5].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_6_MINIMUMLIMITf, info->pool_tc[0][6].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_0_TC_7_MINIMUMLIMITf, info->pool_tc[0][7].mc.buffers);

    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_0_MINIMUMLIMITf, info->pool_tc[1][0].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_1_MINIMUMLIMITf, info->pool_tc[1][1].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_2_MINIMUMLIMITf, info->pool_tc[1][2].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_3_MINIMUMLIMITf, info->pool_tc[1][3].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_4_MINIMUMLIMITf, info->pool_tc[1][4].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_5_MINIMUMLIMITf, info->pool_tc[1][5].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_6_MINIMUMLIMITf, info->pool_tc[1][6].mc.buffers);
    soc_reg_above_64_field32_set(unit, CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr, reg, DB_SERVICE_POOL_1_TC_7_MINIMUMLIMITf, info->pool_tc[1][7].mc.buffers);
    SOCDNX_IF_ERR_EXIT(WRITE_CGM_CONGESTION_MANAGEMENT_DB_SERVICE_POOL_MINIMUM_THRESHOLDSr(unit, core, reg));

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

