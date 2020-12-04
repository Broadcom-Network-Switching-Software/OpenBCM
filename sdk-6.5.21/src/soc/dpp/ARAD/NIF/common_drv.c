/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC COMMON DRV
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PORT
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/ARAD/NIF/ports_manager.h>
#include <soc/cm.h>
#include <soc/mcm/memregs.h>
#include <soc/cmic.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>


soc_error_t 
soc_common_drv_bypass_mode_set(int unit, soc_port_t port, uint32 bypass_mode) 
{
    uint32 reg_val, quad;
    soc_pbmp_t quads_in_use;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_pm_serdes_quads_in_use_get(unit,port,&quads_in_use));


    SOCDNX_IF_ERR_EXIT(READ_NBI_WC_BYPASS_MODEr(unit, &reg_val));

    SOC_PBMP_ITER(quads_in_use, quad){
        switch(quad) {
            case 0:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_0_BPS_MODEf, bypass_mode);
                break;
            case 1:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_1_BPS_MODEf, bypass_mode);
                break;
            case 2:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_2_BPS_MODEf, bypass_mode);
                break;
            case 3:
                break;
            case 4:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_4_BPS_MODEf, bypass_mode);
                break;
            case 5:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_5_BPS_MODEf, bypass_mode);
                break;
            case 6:
                soc_reg_field_set(unit, NBI_WC_BYPASS_MODEr, &reg_val, WC_6_BPS_MODEf, bypass_mode);
                break;
            case 7:
                break;
            default:
                break;
        }
    }

    SOCDNX_IF_ERR_EXIT(WRITE_NBI_WC_BYPASS_MODEr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_common_drv_mac_mode_reg_hdr_set(int unit, soc_port_t port, soc_reg_t mac_mode_reg, uint32 *reg_val)
{   
    soc_encap_mode_t encap_mode;
    uint32 hdr_mode;

    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_encap_mode_get(unit, port , &encap_mode));
    switch(encap_mode){
    case SOC_ENCAP_IEEE:
        hdr_mode = 0;
        break;
    case SOC_ENCAP_HIGIG2:
        hdr_mode = 2;
        break;
#ifdef BCM_88660_A0
    case SOC_ENCAP_SOP_ONLY:
        hdr_mode = 5;
        break;
#endif 
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("Unsupported encap mode %d"), encap_mode));
    }
    soc_reg_field_set(unit, mac_mode_reg, reg_val, HDR_MODEf, hdr_mode);
exit:
    SOCDNX_FUNC_RETURN;
}



soc_error_t 
soc_common_drv_fast_port_set(int unit, soc_port_t port, int is_ilkn, int turn_on) 
{
    uint32 phy_port, nif_intern_id, reg_val, fld_val[1];
    ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
    SOCDNX_INIT_FUNC_DEFS;

    ilkn_tdm_dedicated_queuing = SOC_DPP_CONFIG(unit)->arad->init.ilkn_tdm_dedicated_queuing;

    
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    if (phy_port == 0) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("CPU port can't be set as fast port")));
    }

    nif_intern_id = phy_port-1;

    SOCDNX_IF_ERR_EXIT(READ_EGQ_EGRESS_INTERFACE_FAST_PORT_CONFIGURATIONr(unit, &reg_val));
    *fld_val = soc_reg_field_get(unit, EGQ_EGRESS_INTERFACE_FAST_PORT_CONFIGURATIONr, reg_val, NIF_FAST_PORT_ENf);
    if (turn_on) {
        SHR_BITSET(fld_val, nif_intern_id);
    }
    else {
        SHR_BITCLR(fld_val, nif_intern_id);
    }
    if(is_ilkn && ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON == ilkn_tdm_dedicated_queuing) {
        if (turn_on) {
            SHR_BITSET(fld_val, nif_intern_id+1);
        }
        else {
            SHR_BITCLR(fld_val, nif_intern_id+1);
        }
    }
    soc_reg_field_set(unit, EGQ_EGRESS_INTERFACE_FAST_PORT_CONFIGURATIONr, &reg_val, NIF_FAST_PORT_ENf, *fld_val);
    SOCDNX_IF_ERR_EXIT(WRITE_EGQ_EGRESS_INTERFACE_FAST_PORT_CONFIGURATIONr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN; 
}



soc_error_t
soc_common_link_status_clear(int unit, soc_port_t port)
{
    int port_index;
    uint32 reg32_val, temp_val32, phy_port;
    SOCDNX_INIT_FUNC_DEFS
        
    
    SOCDNX_IF_ERR_EXIT(READ_PORT_LINKSTATUS_DOWN_CLEARr(unit, port, &reg32_val));
    temp_val32 = 0;
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    port_index = SOC_DRIVER(unit)->port_info[phy_port].bindex;
    SHR_BITSET(&temp_val32, port_index);
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_LINKSTATUS_DOWN_CLEARr(unit, port, temp_val32));
    sal_usleep(10);
    
    SOCDNX_IF_ERR_EXIT(WRITE_PORT_LINKSTATUS_DOWN_CLEARr(unit, port, reg32_val));

exit:
    SOCDNX_FUNC_RETURN
}


soc_error_t
soc_common_link_status_get(int unit, soc_port_t port, int *is_latch_down)
{
    int port_index;
    uint32 reg32_val[1], phy_port;
    SOCDNX_INIT_FUNC_DEFS
        
    
    SOCDNX_IF_ERR_EXIT(READ_PORT_LINKSTATUS_DOWNr(unit, port, reg32_val));
    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port, &phy_port));
    port_index = SOC_DRIVER(unit)->port_info[phy_port].bindex;
    (*is_latch_down) = (SHR_BITGET(reg32_val, port_index) ? 1 : 0);
        
exit:
    SOCDNX_FUNC_RETURN
}

soc_error_t
soc_common_drv_core_port_mode_get(int unit, soc_port_t port, uint32 *core_port_mode) 
{
    uint32 num_of_lanes, is_valid;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_num_lanes_get(unit, port, &num_of_lanes));
    if (1 == num_of_lanes) {
        *core_port_mode = 2;
    } else if (2 == num_of_lanes) {
        *core_port_mode = 1;
    } else {
        *core_port_mode = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_common_drv_phy_port_mode_get(int unit, soc_port_t port, uint32 *phy_port_mode) 
{
    uint32 is_valid;
    int speed;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_is_valid_port_get(unit, port, &is_valid));
    if (!is_valid) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PORT, (_BSL_SOCDNX_MSG("Port %d is invalid"),port));
    }

    SOCDNX_IF_ERR_EXIT(soc_port_sw_db_speed_get(unit, port, &speed));

    if (speed <= 12500 || speed == 23000) {
        *phy_port_mode = 2;
    } else if (speed <= 21000) {
        *phy_port_mode = 1;
    } else {
        *phy_port_mode = 0;
    }

exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

