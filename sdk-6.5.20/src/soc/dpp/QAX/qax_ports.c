/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: qax_ports.c
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/mbcm.h>
#include <soc/dpp/JER/jer_ports.h>
#include <soc/dpp/JER/jer_egr_queuing.h>
#include <soc/dpp/QAX/qax_link_bonding.h>

#define SOC_QAX_ILKN_NIF_PORT_ILKN0  120
#define SOC_QAX_ILKN_NIF_PORT_ILKN1  121
#define SOC_QAX_ILKN_NIF_PORT_ILKN2  122
#define SOC_QAX_ILKN_NIF_PORT_ILKN3  123


int
soc_qux_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value)
{
    uint32 num_of_lanes, if_rate_mbps;
    soc_port_if_t interface;
    uint32 pmx_base_lane = SOC_DPP_DEFS_GET(unit, pmx_base_lane);
    uint32 phy_port;
    int speed = 0;
#ifdef BCM_LB_SUPPORT
    uint32 tm_port = 0, egress_offset = 0;
    int core = 0, is_lb_port = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
            is_lb_port = 1;
        }
    }
#endif

    if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OLP) {
        *value = 36;
    } else if (interface == SOC_PORT_IF_OAMP) {
        *value = 66;
    } else if (interface == SOC_PORT_IF_RCY) {
        *value = 517;
    } else if (interface == SOC_PORT_IF_IPSEC || interface == SOC_PORT_IF_SAT) {
        *value = 130;
    } else if ((interface == SOC_PORT_IF_LBG) || (is_lb_port == 1)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));
        if (speed <= 1000) {
            *value = 13;
        } else if (speed <= 10000) {
            *value = 32;
        } else if (speed <= 20000) {
            *value = 61;
        } else if (speed <= 30000) {
            *value = 91;
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d)"), port));
        }
    } else { 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
        phy_port--; 

        if (phy_port < pmx_base_lane) { 
            if (num_of_lanes == 1) {
                if (if_rate_mbps <= 2500) { 
                    *value = 142;
                } else if (if_rate_mbps <= 12000) { 
                    *value = 90;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else if (num_of_lanes == 2) {
                if (if_rate_mbps <= 25000) { 
                    *value = 170;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else if (num_of_lanes == 4) {
                if (if_rate_mbps <= 48000) { 
                    *value = 360;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) num_of_lanes(%d)"), port, num_of_lanes));
            }
        }
        else { 
            if (num_of_lanes == 1) {
                if (if_rate_mbps <= 2500) { 
                    *value = 46;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else if (num_of_lanes == 2) {
                if (if_rate_mbps <= 2500) { 
                    *value = 110;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else if (num_of_lanes == 4) {
                if (if_rate_mbps == 10000) { 
                    *value = 186;
                } else {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) if_rate_mbps(%d)"), port, if_rate_mbps));
                }
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration port(%d) num_of_lanes(%d)"), port, num_of_lanes));
            }
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
soc_qax_port_nrdy_th_optimal_value_get(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN soc_port_t port, 
    SOC_SAND_OUT uint32 *value)
{
    uint32 num_of_lanes, if_rate_mbps;
    soc_port_if_t interface;
#ifdef BCM_LB_SUPPORT
    uint32 tm_port = 0, egress_offset = 0;
    int core = 0;
#endif

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_QUX(unit)) {
        return soc_qux_port_nrdy_th_optimal_value_get(unit, port, value);
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_rate_get(unit, port, &if_rate_mbps));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface == SOC_PORT_IF_CPU || interface == SOC_PORT_IF_OLP) {
        *value = 36;
    } else if (interface == SOC_PORT_IF_OAMP) {
        *value = 66;
    } else if (interface == SOC_PORT_IF_RCY) {
        *value = 518;
    } else if (interface == SOC_PORT_IF_ILKN) {
        if (if_rate_mbps <= 300000) {
            *value = 260;
        } else { 
            *value = 518;
        }
    } else if (interface == SOC_PORT_IF_QSGMII) {
        *value = 22;
    } else if (interface == SOC_PORT_IF_IPSEC || interface == SOC_PORT_IF_SAT) {
        *value = 130;
    } else if (interface == SOC_PORT_IF_LBG) {
        *value = 30;
    } else { 
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));

        if (num_of_lanes == 1) {
            if (if_rate_mbps <= 1000) { 
                *value = 112;
            } else if (if_rate_mbps <= 10000) { 
                *value = 96;
            } else  { 
                *value = 64;
            }

        } else if (num_of_lanes == 2) {
            if (if_rate_mbps <= 10000) { 
                *value = 224;
            } else if (if_rate_mbps <= 25000) { 
                *value = 192;
            } else { 
                *value = 128;
            }

        } else if (num_of_lanes == 4) {
            if (if_rate_mbps <= 10000) { 
                *value = 480;
            } else if (if_rate_mbps <= 25000) { 
                *value = 448;
            } else if (if_rate_mbps <= 50000) { 
                *value = 384;
            } else { 
                *value = 260;
            }
        } else {
            SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid NRDY threshold configuration")));
        }
    }

#ifdef BCM_LB_SUPPORT
    if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port2egress_offset, (unit, core, tm_port, &egress_offset)));

        if (egress_offset == SOC_QAX_EGR_IF_LBG_RESERVE) {
            *value = 30;
        }
    }
#endif

exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_qax_port_swap_global_info_set(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;
    int i;

    SOCDNX_INIT_FUNC_DEFS;

    COMPILER_64_ZERO(reg_val_64);

            
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, GLOBAL_TAG_SWAP_SIZEf, ports_swap_info->global_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_OFFSET_0f, ports_swap_info->tag_swap_n_offset_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_OFFSET_1f, ports_swap_info->tag_swap_n_offset_1);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_0_TAG_SWAP_SIZEf, ports_swap_info->tpid_0_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TPID_1_TAG_SWAP_SIZEf, ports_swap_info->tpid_1_tag_swap_n_size);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_TPID_0f, ports_swap_info->tag_swap_n_tpid_0);
    soc_reg64_field32_set(unit, IRE_TAG_SWAP_CONFIGURATIONr, &reg_val_64, TAG_SWAP_TPID_1f, ports_swap_info->tag_swap_n_tpid_1);

    
    for (i = 0 ; i < SOC_DPP_DEFS_GET(unit, nof_cores) ; i++) {
        rv = WRITE_IRE_TAG_SWAP_CONFIGURATIONr_REG64(unit, i, reg_val_64);
        SOCDNX_IF_ERR_EXIT(rv);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_qax_port_swap_global_info_get(int unit, SOC_TMC_PORT_SWAP_GLOBAL_INFO *ports_swap_info){
    int rv;
    uint64 reg_val_64;

    SOCDNX_INIT_FUNC_DEFS;

    
    rv = READ_IRE_TAG_SWAP_CONFIGURATIONr_REG64(unit, 0, &reg_val_64);
    SOCDNX_IF_ERR_EXIT(rv);


            
    ports_swap_info->global_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, GLOBAL_TAG_SWAP_SIZEf);   
    ports_swap_info->tag_swap_n_offset_0    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_OFFSET_0f);  
    ports_swap_info->tag_swap_n_offset_1    = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_OFFSET_1f);
    ports_swap_info->tpid_0_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_0_TAG_SWAP_SIZEf);
    ports_swap_info->tpid_1_tag_swap_n_size = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TPID_1_TAG_SWAP_SIZEf);
    ports_swap_info->tag_swap_n_tpid_0      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_TPID_0f);
    ports_swap_info->tag_swap_n_tpid_1      = soc_reg64_field32_get(unit, IRE_TAG_SWAP_CONFIGURATIONr, reg_val_64, TAG_SWAP_TPID_1f);

exit:
    SOCDNX_FUNC_RETURN;
}


int soc_qax_port_ilkn_nif_port_get(int unit, uint32 ilkn_intf_offset, uint32* nif_port)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (ilkn_intf_offset) {
    case 0:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN0;
        break;
    case 1:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN1;
        break;
    case 2:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN2;
        break;
    case 3:
        *nif_port = SOC_QAX_ILKN_NIF_PORT_ILKN3;
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_CONFIG, (_BSL_SOCDNX_MSG("Invalid ILKN interface offset %d"),ilkn_intf_offset));
        break;
    }
exit:
    SOCDNX_FUNC_RETURN;
}

int soc_qax_port_to_interface_egress_map_set(int unit, soc_port_t port) {

    int core = -1, rv = 0;
    uint32 field_zero = 0, egr_if, reg32[1], tm_port;
    uint32 num_ports_nbih, num_ports_nbil, num_lanes_per_nbi, nbi_inst;
    uint32 pm_num, pm_internal, nif_port, is_skip_nif_for_lb = 0, is_nif;
    soc_port_if_t interface;
    uint32 protocol_offset, flags, phy_port;
    SOCDNX_INIT_FUNC_DEFS;

    num_ports_nbih = SOC_DPP_DEFS_GET(unit, nof_ports_nbih);
    num_ports_nbil = SOC_DPP_DEFS_GET(unit, nof_ports_nbil);
    num_lanes_per_nbi = SOC_DPP_DEFS_GET(unit, nof_lanes_per_nbi);    

    SOCDNX_IF_ERR_EXIT(soc_jer_port_to_interface_egress_map_set(unit, port));

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_interface_type_get(unit, port, &interface));

    if(interface != SOC_PORT_IF_ERP) {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit, port, &flags));
        is_nif = SOC_PORT_IS_NETWORK_INTERFACE(flags);

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_local_to_tm_port_get(unit, port, &tm_port, &core));

        if (!SOC_PORT_IS_LB_MODEM(flags)) {
            SOCDNX_IF_ERR_EXIT(soc_jer_egr_port2egress_offset(unit, core, tm_port, &egr_if));
        } else {
            rv = sw_state_access[unit].dpp.soc.arad.tm.logical_ports_info.egr_interface.get(unit, port, &egr_if);
            SOCDNX_IF_ERR_EXIT(rv);
        }

#ifdef BCM_LB_SUPPORT
        if (SOC_IS_QAX(unit) && SOC_DPP_CONFIG(unit)->qax->lbi_en) {
            if (SOC_QAX_EGR_IF_LBG_RESERVE == egr_if) {
                is_skip_nif_for_lb = 1;
            }
        }
#endif

        if((is_nif || SOC_PORT_IS_LB_MODEM(flags)) && (is_skip_nif_for_lb != 1)) {

            if (interface == SOC_PORT_IF_ILKN) {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &protocol_offset ));
                pm_num = protocol_offset + 12; 
                pm_internal=0;
                nif_port=0;
            } else {
                SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port ));
                nif_port = phy_port;
                --nif_port; 
                SOCDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_qsgmii_offsets_remove, (unit, phy_port, &phy_port)));
                --phy_port; 
                nbi_inst = phy_port / num_lanes_per_nbi;
                pm_num = phy_port / 4;
                switch (nbi_inst) {
                case 0:
                    pm_internal = phy_port % 4;
                    break;
                case 1:
                    pm_internal = (nif_port - num_ports_nbih) % 16;
                    break;
                case 2:
                    pm_internal = (nif_port - num_ports_nbih - num_ports_nbil) % 16;
                    break;
                }
            }

            SOCDNX_IF_ERR_EXIT(READ_EPNI_IFC_2_NIF_PORT_MAPm(unit, EPNI_BLOCK(unit, core), egr_if, reg32));
            soc_mem_field_set(unit, EPNI_IFC_2_NIF_PORT_MAPm, reg32, PM_NUMf, &pm_num);
            soc_mem_field_set(unit, EPNI_IFC_2_NIF_PORT_MAPm, reg32, PM_INTERNAL_PORTf, &pm_internal);
            soc_mem_field_set(unit, EPNI_IFC_2_NIF_PORT_MAPm, reg32, BICAST_VALIDf, &field_zero);
            soc_mem_field_set(unit, EPNI_IFC_2_NIF_PORT_MAPm, reg32, NIF_PORT_NUMf, &nif_port);
            SOCDNX_IF_ERR_EXIT(WRITE_EPNI_IFC_2_NIF_PORT_MAPm(unit, EPNI_BLOCK(unit ,core), egr_if, reg32));
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
