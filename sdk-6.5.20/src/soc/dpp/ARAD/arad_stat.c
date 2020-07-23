/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC ARAD FABRIC STAT
 */
 
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT



#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/dpp/drv.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>
#include <shared/bitop.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/dpp_config_imp_defs.h>
#include <soc/dpp/ARAD/arad_defs.h>
#include <soc/dpp/ARAD/arad_stat.h>
#include <bcm_int/dpp/utils.h>
#include <soc/dpp/mbcm.h>
#include <bcm_int/dpp/error.h>
#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/port.h>
#include <bcm/stat.h>

#include <bcm_int/petra_dispatch.h>

int 
soc_arad_mac_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read){

    int blk_idx, lane_idx ,link,rv, port_offset;
    uint32 control_reg;
    uint32 first_mac_counter;
    uint64 mask;
    int length, counter_locked=0;
    uint32 offset = 0, phy_port;
    int ilkn_over_fabric_port;
    SOCDNX_INIT_FUNC_DEFS;

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    if (IS_IL_PORT(unit,port)) {
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &offset)); 
        ilkn_over_fabric_port = SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, offset);
    } else {
        offset = 0;
        ilkn_over_fabric_port = 0;
    }

    if(IS_SFI_PORT(unit,port) || ilkn_over_fabric_port) {
       if (ilkn_over_fabric_port) {
           port_offset = SOC_DPP_DEFS_GET(unit, first_fabric_link_id);
           SOCDNX_IF_ERR_EXIT(soc_port_sw_db_first_phy_port_get(unit, port , &phy_port));
           link = phy_port - SOC_DPP_FIRST_FABRIC_PHY_PORT(unit);
       } else {
            port_offset = SOC_DPP_DEFS_GET(unit, first_fabric_link_id); 
            link = SOC_DPP_FABRIC_PORT_TO_LINK(unit, port - port_offset);
       }

       blk_idx = link/SOC_ARAD_NOF_LINKS_IN_MAC;
       lane_idx = link % SOC_ARAD_NOF_LINKS_IN_MAC;

        first_mac_counter = SOC_DPP_DEFS_GET(unit, mac_counter_first);

        SOCDNX_IF_ERR_EXIT(MBCM_DPP_SOC_DRIVER_CALL(unit, mbcm_dpp_stat_counter_length_get, (unit, counter_id, &length)));

        counter_id = counter_id-first_mac_counter;
        
        COUNTER_LOCK(unit);
        counter_locked = 1;

        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);
        SOC_DPP_ALLOW_WARMBOOT_WRITE(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg), rv);
        SOCDNX_IF_ERR_EXIT(rv);
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

        counter_locked = 0;
        COUNTER_UNLOCK(unit);

        COMPILER_64_MASK_CREATE(mask, length, 0);
        COMPILER_64_AND((*val), mask);

    } else {
        COMPILER_64_SET(*val, 0, 0);
    }
    

exit:
    if (counter_locked) {
        COUNTER_UNLOCK(unit);
    }
    SOCDNX_FUNC_RETURN;
}

STATIC int 
soc_arad_nif_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read)
{
    int soc_sand_rv, soc_sand_dev_id;
    SOC_SAND_64CNT  soc_sand_value;
    SOC_SAND_64CNT  soc_sand_value_1, soc_sand_value_2;
    SOCDNX_INIT_FUNC_DEFS;

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    if(!IS_SFI_PORT(unit,port)) {
        

        soc_sand_dev_id = (unit);
        sal_memset(&soc_sand_value, 0, sizeof(soc_sand_value));

            switch (counter_id) {
            case ARAD_NIF_TX_NON_UNICAST_PACKETS:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_TX_BCAST_PACKETS, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_TX_MCAST_BURSTS, &soc_sand_value_2);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_2);
                
                break;
            case ARAD_NIF_RX_NON_UNICAST_PACKETS:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_BCAST_PACKETS, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_MCAST_BURSTS, &soc_sand_value_2);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_2);
                break;
            case ARAD_NIF_RX_LEN_1515CFG_MAX:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_1519_2043, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_2044_4091, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_4092_9212, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, ARAD_NIF_RX_LEN_9213CFG_MAX, &soc_sand_value_1);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                        counter_id, port, port));
                }
                soc_sand_64cnt_add_64cnt(&soc_sand_value, &soc_sand_value_1);
                break;
            default:
                soc_sand_rv = arad_nif_counter_get(soc_sand_dev_id, port, counter_id, &soc_sand_value);
                if (SOC_SAND_FAILURE(soc_sand_rv)) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG("soc_dpp_port_counter_get: failed (SOC_SAND_RV: %d) to get NIF counter (type:%d) if_id:%d port:0x%x"),soc_sand_rv,
                                                    counter_id, port, port));
                }
            }
        COMPILER_64_SET(*val, soc_sand_value.u64.arr[1], soc_sand_value.u64.arr[0]);

    } else {
        COMPILER_64_SET(*val, 0, 0);
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}

soc_controlled_counter_t soc_arad_controlled_counter[] = {
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_BCAST_PACKETS,
        "RX BCAST PACKETS",
        "RX BCAST PACKETS",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_MCAST_BURSTS,   
        "RX MCAST BURSTS",
        "RX MCAST BURSTS",    
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_ERR_PACKETS,      
        "RX ERR PACKETS", 
        "RX ERR PACKETS", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
        },
        {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_BELOW_MIN,     
        "RX LEN BELOW MIN",
        "RX LEN BELOW MIN",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_MIN_59,
        "RX LEN MIN 59", 
        "RX LEN MIN 59",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_60,  
        "RX LEN 60", 
        "RX LEN 60",            
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_61_123,     
        "RX LEN 61 123",   
        "RX LEN 61 123", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_124_251,      
        "RX LEN 124 251", 
        "RX LEN 124 251", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_252_507,       
        "RX LEN 252 507",
        "RX LEN 252 507",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_508_1019,      
        "RX LEN 508 1019",
        "RX LEN 508 1019",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1020_1514,    
        "RX LEN 1020 1514", 
        "RX LEN 1020 1514", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
        },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1515_1518,
        "RX LEN 1515 1518",
        "RX LEN 1515 1518",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
        },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1519_2043,  
        "RX LEN 1519 2043",   
        "RX LEN 1519 2043",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_2044_4091,
        "RX LEN 2044 4091",
        "RX LEN 2044 4091",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_4092_9212, 
        "RX LEN 4092 9212",  
        "RX LEN 4092 9212",   
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_9213CFG_MAX,  
        "RX LEN 9213CFG MAX", 
        "RX LEN 9213CFG MAX", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_1515CFG_MAX,  
        "RX LEN 1515CFG MAX", 
        "RX LEN 1515CFG MAX", 
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW  | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_LEN_ABOVE_MAX,
        "RX LEN ABOVE MAX",
        "RX LEN ABOVE MAX",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_BELOW_MIN, 
        "TX LEN BELOW MIN",
        "TX LEN BELOW MIN",    
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_MIN_59,  
        "TX LEN MIN 59",
        "TX LEN MIN 59",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_60,
        "TX LEN 60", 
        "TX LEN 60", 
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_61_123,  
        "TX LEN 61 123",
        "TX LEN 61 123",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_124_251,
        "TX LEN 124 251",
        "TX LEN 124 251",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_252_507, 
        "TX LEN 252 507",
        "TX LEN 252 507",      
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_508_1019,
        "TX LEN 508 1019",
        "TX LEN 508 1019",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1020_1514,
        "TX LEN 1020 1514",
        "TX LEN 1020 1514",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1515_1518,    
        "TX LEN 1515 1518",
        "TX LEN 1515 1518", 
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_1519_2043, 
        "TX LEN 1519 2043",
        "TX LEN 1519 2043",    
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_2044_4091,
        "TX LEN 2044 4091",   
        "TX LEN 2044 4091",   
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_4092_9212,
        "TX LEN 4092 9212",
        "TX LEN 4092 9212",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_LEN_9213CFG_MAX, 
        "TX LEN 9213CFG MAX",  
        "TX LEN 9213CFG MAX",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_BCAST_PACKETS,  
        "TX BCAST PACKETS",
        "TX BCAST PACKETS",      
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_MCAST_BURSTS,
        "TX MCAST BURSTS",
        "TX MCAST BURSTS",     
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_ERR_PACKETS,
        "TX ERR PACKETS",
        "TX ERR PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_OK_OCTETS,
        "RX OK OCTETS",  
        "RX OK OCTETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_OK_OCTETS,
        "TX OK OCTETS",
        "TX OK OCTETS",         
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_MEDIUM | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_OK_PACKETS,
        "RX OK PACKETS",
        "RX OK PACKETS",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_OK_PACKETS, 
        "TX OK PACKETS",
        "TX OK PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_NON_UNICAST_PACKETS,
        "RX N UC PACKETS",
        "RX N UC PACKETS",        
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW   | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_NON_UNICAST_PACKETS, 
        "TX N UC PACKETS",
        "TX N UC PACKETS",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW  | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_RX_ILKN_PER_CHANNEL, 
        "RX ILKN PER CHANNEL",
        "RX ILKN PER CHANNEL",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW  | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_nif_controlled_counter_get,
        ARAD_NIF_TX_ILKN_PER_CHANNEL, 
        "TX ILKN PER CHANNEL",
        "TX ILKN PER CHANNEL",       
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW  | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER,
        "RX CRC errors",
        "RX CRC errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR, 
        "RX (L)FEC correctable \\ BEC crc \\ 8b/10b disparity",
        "RX correctable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER,
        "RX Control cells",
        "RX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER,
        "RX Data cell",
        "RX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER,
        "RX Data byte",
        "RX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL,
        "RX dropped retransmitted control",
        "RX drop retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT,
        "TX BEC retransmit",
        "TX BEC retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT,
        "RX BEC retransmit",
        "RX BEC retransmit",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS,
        "TX Asyn fifo rate at units of 40 bits",
        "TX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE | _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS,
        "RX Asyn fifo rate at units of 40 bits",
        "RX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE | _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_LOW | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS, 
        "RX (L)FEC uncorrectable \\ BEC fault \\ 8b/10b code errors",
        "RX uncorrectable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY, 
        "RX LLFC Primary",
        "RX LLFC Primary",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY, 
        "RX LLFC Secondary",
        "RX LLFC Secondary",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        NULL,
        -1,
        "",
        "",
        0,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    }
};


soc_error_t 
soc_arad_fabric_stat_init(int unit)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}


soc_error_t 
soc_arad_stat_nif_init(int unit)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_WARM_BOOT(unit)) {
        SOC_EXIT;
    }

    
    SOCDNX_IF_ERR_EXIT(soc_arad_stat_clear_on_read_set(unit, 0));

    SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
    soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_CNT_ALL_BY_PKTf, 0x1);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));

    
    if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_physical) {
        
    } else if (SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode == soc_arad_stat_ilkn_counters_mode_packets_per_channel) {
        SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_ILKN_0_CNT_PER_CHf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_ILKN_1_CNT_PER_CHf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_RX_EOP_COUNT_ENABLEf, 0x1);
        soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_TX_EOP_COUNT_ENABLEf, 0x1);
        SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_INIT, (_BSL_SOCDNX_MSG("soc_arad_stat_nif_init: unavail ilkn_counters_mode(%u)"), SOC_DPP_CONFIG(unit)->arad->init.ports.ilkn_counters_mode));
    }

exit:
    SOCDNX_FUNC_RETURN;
}




soc_error_t
soc_arad_mapping_stat_get(int unit, soc_port_t port, uint32 *cnt_type, int *num_cntrs_p, bcm_stat_val_t type, int num_cntrs_in)
{
    int pcs;
    int num_cntrs ;
    SOCDNX_INIT_FUNC_DEFS;

    *num_cntrs_p = num_cntrs_in;

    if (cnt_type == (ARAD_NIF_COUNTER_TYPE *)0) {
        SOCDNX_EXIT_WITH_ERR(
            SOC_E_PARAM,
                (_BSL_SOCDNX_MSG("Either cnt_type (0x%08lX)"),
                                            (unsigned long)cnt_type)); 
    }
    if (*num_cntrs_p <= 0) {
        SOCDNX_EXIT_WITH_ERR(
            SOC_E_PARAM,(_BSL_SOCDNX_MSG("*num_cntrs_p (%d) is zero or negative"), *num_cntrs_p)); 
    }
    
    if (!SOC_ARAD_STAT_COUNTER_MODE_PHISYCAL(unit, port)) {
        *num_cntrs_p = 1 ;
        switch(type) {
        case snmpIfInBroadcastPkts:
            cnt_type[0] = ARAD_NIF_RX_BCAST_PACKETS;
            break;
        case snmpIfInMulticastPkts:
            cnt_type[0] = ARAD_NIF_RX_MCAST_BURSTS;
            break;
        case snmpIfInErrors:
            cnt_type[0] = ARAD_NIF_RX_ERR_PACKETS;
            break;
        case snmpEtherStatsUndersizePkts:
            cnt_type[0] = ARAD_NIF_RX_LEN_BELOW_MIN;
            break;
        case snmpIfOutBroadcastPkts: 
            cnt_type[0] = ARAD_NIF_TX_BCAST_PACKETS;
            break;
        case snmpIfOutMulticastPkts: 
            cnt_type[0] = ARAD_NIF_TX_MCAST_BURSTS;
            break;
        case snmpIfOutErrors: 
            cnt_type[0] = ARAD_NIF_TX_ERR_PACKETS;
            break;
        case snmpIfInOctets:  
            cnt_type[0] = ARAD_NIF_RX_OK_OCTETS;
            break;
        case snmpIfOutOctets:  
            cnt_type[0] = ARAD_NIF_TX_OK_OCTETS;
            break;
        case snmpEtherStatsRXNoErrors:
            cnt_type[0] = ARAD_NIF_RX_OK_PACKETS;
            break;
        case snmpIfInNUcastPkts:
            cnt_type[0] = ARAD_NIF_RX_NON_UNICAST_PACKETS;
            break;
        case snmpEtherStatsTXNoErrors:   
            cnt_type[0] = ARAD_NIF_TX_OK_PACKETS;
            break;
        case snmpIfOutNUcastPkts:
            cnt_type[0] = ARAD_NIF_TX_NON_UNICAST_PACKETS;
            break;
        default:
            *num_cntrs_p = 0 ;
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("stat type %d isn't supported"), type)); 
        }
    } else {
        num_cntrs = *num_cntrs_p ;
        *num_cntrs_p = 1 ;
        switch(type) {
            case snmpIfInBroadcastPkts:
                cnt_type[0] = ARAD_NIF_RX_BCAST_PACKETS;
                break;
            case snmpIfInMulticastPkts:
                cnt_type[0] = ARAD_NIF_RX_MCAST_BURSTS;
                break;
            case snmpIfInErrors:
                cnt_type[0] = ARAD_NIF_RX_ERR_PACKETS;
                break;
            case snmpEtherStatsUndersizePkts:
                cnt_type[0] = ARAD_NIF_RX_LEN_BELOW_MIN;
                break;
            case snmpBcmReceivedPkts64Octets:
            case snmpEtherStatsPkts64Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_60;
                break;
            case snmpBcmReceivedPkts65to127Octets:
            case snmpEtherStatsPkts65to127Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_61_123;
                break;
            case snmpBcmReceivedPkts128to255Octets:
            case snmpEtherStatsPkts128to255Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_124_251;
                break;
            case snmpBcmReceivedPkts256to511Octets:
            case snmpEtherStatsPkts256to511Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_252_507;
                break;
            case snmpBcmReceivedPkts512to1023Octets:
            case snmpEtherStatsPkts512to1023Octets: 
                cnt_type[0] = ARAD_NIF_RX_LEN_508_1019;
                break;
            case snmpBcmReceivedPkts1024to1518Octets:
            case snmpEtherStatsPkts1024to1518Octets:  
                cnt_type[0] = ARAD_NIF_RX_LEN_1020_1514;
                break;
            case snmpEtherStatsOversizePkts: 
                cnt_type[0] = ARAD_NIF_RX_LEN_1515CFG_MAX;
                
                break;
            case snmpBcmReceivedPkts2048to4095Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_2044_4091;
                break;
            case snmpBcmReceivedPkts4095to9216Octets:
                cnt_type[0] = ARAD_NIF_RX_LEN_4092_9212;
                break;
            case snmpIfOutBroadcastPkts: 
                cnt_type[0] = ARAD_NIF_TX_BCAST_PACKETS;
                break;
            case snmpIfOutMulticastPkts: 
                cnt_type[0] = ARAD_NIF_TX_MCAST_BURSTS;
                break;
            case snmpIfOutErrors: 
                cnt_type[0] = ARAD_NIF_TX_ERR_PACKETS;
                break;
            case snmpIfInOctets:  
                cnt_type[0] = ARAD_NIF_RX_OK_OCTETS;
                break;
            case snmpIfOutOctets:  
                cnt_type[0] = ARAD_NIF_TX_OK_OCTETS;
                break;
            case snmpEtherStatsRXNoErrors:
                cnt_type[0] = ARAD_NIF_RX_OK_PACKETS;
                break;
            case snmpIfInNUcastPkts:
                cnt_type[0] = ARAD_NIF_RX_NON_UNICAST_PACKETS;
                break;
            case snmpEtherStatsTXNoErrors:   
                cnt_type[0] = ARAD_NIF_TX_OK_PACKETS;
                break;
            case snmpIfOutNUcastPkts:
                cnt_type[0] = ARAD_NIF_TX_NON_UNICAST_PACKETS;
                break;
            case snmpBcmTransmittedPkts64Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_60;
                break;
            case snmpBcmTransmittedPkts65to127Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_61_123;
                break;
            case snmpBcmTransmittedPkts128to255Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_124_251;
                break;
            case snmpBcmTransmittedPkts256to511Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_252_507;
                break;
            case snmpBcmTransmittedPkts512to1023Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_508_1019;
                break;
            case snmpBcmTransmittedPkts1024to1518Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_1020_1514;
                break;
            case snmpBcmTransmittedPkts1519to2047Octets: 
                cnt_type[0] = ARAD_NIF_TX_LEN_1519_2043;
                if (num_cntrs <= 1) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_FULL ,
                        (_BSL_SOCDNX_MSG("provided num_cntrs (%d) is smaller than required (2)"),num_cntrs)) ; 
                }
                *num_cntrs_p = 2 ;
                cnt_type[1] = ARAD_NIF_TX_LEN_1515_1518;
                break;
            case snmpBcmTransmittedPkts2048to4095Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_2044_4091;
                break;
            case snmpBcmTransmittedPkts4095to9216Octets:
                cnt_type[0] = ARAD_NIF_TX_LEN_4092_9212;
                break;
            case snmpBcmTxControlCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER;
                break;
            case snmpBcmTxDataCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER;
                break;
            case snmpBcmTxDataBytes:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER;
                break;
            case snmpBcmRxCrcErrors:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER;
                break;
            case snmpBcmRxFecCorrectable:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_8_9_legacy_fec != pcs && soc_dcmn_port_pcs_64_66_fec != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for FEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR;
                break;
            case snmpBcmRxBecCrcErrors:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_64_66_bec != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for BEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_CRC_ERROR;
                break;
            case snmpBcmRxDisparityErrors:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_8_10 != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for 8b/10b ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_8B_10B_DISPARITY_ERRORS;
                break;
            case snmpBcmRxControlCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER;
                break;
            case snmpBcmRxDataCells:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER;
                break;
            case snmpBcmRxDataBytes:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER;
                break;
            case snmpBcmRxDroppedRetransmittedControl:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL;
                break;
            case snmpBcmTxBecRetransmit:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT;
                break;
            case snmpBcmRxBecRetransmit:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT;
                break;
            case snmpBcmTxAsynFifoRate:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS;
                break;
            case snmpBcmRxAsynFifoRate:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS;
                break;
            case snmpBcmRxFecUncorrectable:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_8_9_legacy_fec != pcs && soc_dcmn_port_pcs_64_66_fec != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for FEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS;
                break;
            case snmpBcmRxBecRxFault:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_64_66_bec != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOC_MSG("port: %d, counter %d supported only for BEC ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_BEC_RX_FAULT;
                break;
            case snmpBcmRxCodeErrors:
                SOCDNX_IF_ERR_EXIT(bcm_petra_port_control_get(unit, port, bcmPortControlPCS, &pcs));
                if(soc_dcmn_port_pcs_8_10 != pcs) {
                    SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for 8b/10b ports"),port, type)); 
                }
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_8B_10B_CODE_ERRORS;
                break;
            case snmpBcmRxLlfcPrimary:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LLFC_PRIMARY;
                break;
            case snmpBcmRxLlfcSecondary:
                cnt_type[0] = SOC_ARAD_MAC_COUNTERS_RX_LLFC_SECONDARY;
                break;
            default:
                *num_cntrs_p = 0 ;
                SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("stat type %d isn't supported"), type)); 
            }
        }
exit:
    SOCDNX_FUNC_RETURN;
}






soc_error_t 
soc_arad_stat_clear_on_read_set(int unit, int enable)
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_NBI_STATISTICSr(unit, &reg_val));
    soc_reg_field_set(unit, NBI_STATISTICSr, &reg_val, STAT_CLEAR_ON_READf, enable);
    SOCDNX_IF_ERR_EXIT(WRITE_NBI_STATISTICSr(unit, reg_val));

exit:
    SOCDNX_FUNC_RETURN;
}
soc_error_t 
soc_arad_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable, int *printable) {
    soc_control_t	*soc;
    uint32 channel;
    SOCDNX_INIT_FUNC_DEFS;

    *enable = 1;
    *printable = 1;
    soc = SOC_CONTROL(unit);

    if (BCM_PBMP_MEMBER(PBMP_SFI_ALL(unit), port)) {
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_MAC)) {
            *enable = 0;
            SOC_EXIT;
        }
    } else {
        
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NIF)) {
            *enable = 0;
        }
    }
    if(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE) {
        *printable = 0;
    }

    
    if (index == ARAD_NIF_RX_ILKN_PER_CHANNEL || index == ARAD_NIF_TX_ILKN_PER_CHANNEL) {
        if (! (SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port) && SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port))) {
            *enable = 0;
        }

        
        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_channel_get(unit, port, &channel));
        if (channel > SOC_ARAD_NIF_ILKN_COUNTER_PER_CHANNEL_CANNEL_SUPPORTED_MAX) {
            *enable = 0;
        }
    } else { 
        if (SOC_PBMP_MEMBER(PBMP_IL_ALL(unit), port)) {
            soc_port_t phy_port, port_base;

            phy_port = SOC_INFO(unit).port_l2p_mapping[port];
            port_base = SOC_INFO(unit).port_p2l_mapping[phy_port];
            if (port != port_base) {
                *enable = 0;
            }
            if (SOC_ARAD_STAT_COUNTER_MODE_PACKETS_PER_CHANNEL(unit, port) ) {
                
                switch (index) {
                    case ARAD_NIF_RX_LEN_BELOW_MIN:
                    case ARAD_NIF_RX_LEN_MIN_59:
                    case ARAD_NIF_RX_LEN_60:
                    case ARAD_NIF_RX_LEN_61_123:
                    case ARAD_NIF_RX_LEN_124_251:
                    case ARAD_NIF_RX_LEN_252_507:
                    case ARAD_NIF_RX_LEN_508_1019:
                    case ARAD_NIF_RX_LEN_1020_1514:
                    case ARAD_NIF_RX_LEN_1515_1518:
                    case ARAD_NIF_RX_LEN_1519_2043:
                    case ARAD_NIF_RX_LEN_2044_4091:
                    case ARAD_NIF_RX_LEN_4092_9212:
                    case ARAD_NIF_RX_LEN_9213CFG_MAX:
                    case ARAD_NIF_RX_LEN_1515CFG_MAX:
                    case ARAD_NIF_RX_LEN_ABOVE_MAX:
                    case ARAD_NIF_TX_LEN_BELOW_MIN:
                    case ARAD_NIF_TX_LEN_MIN_59:
                    case ARAD_NIF_TX_LEN_60:
                    case ARAD_NIF_TX_LEN_61_123:
                    case ARAD_NIF_TX_LEN_124_251:
                    case ARAD_NIF_TX_LEN_252_507:
                    case ARAD_NIF_TX_LEN_508_1019:
                    case ARAD_NIF_TX_LEN_1020_1514:
                    case ARAD_NIF_TX_LEN_1515_1518:
                    case ARAD_NIF_TX_LEN_1519_2043:
                    case ARAD_NIF_TX_LEN_2044_4091:
                    case ARAD_NIF_TX_LEN_4092_9212:
                        *enable = 0;
                        break;
                }
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_arad_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (counter_id)
    {
        case SOC_ARAD_MAC_COUNTERS_RX_DROPPED_RETRANSMITTED_CONTROL:
        case SOC_ARAD_MAC_COUNTERS_TX_BEC_RETRANSMIT:
        case SOC_ARAD_MAC_COUNTERS_RX_BEC_RETRANSMIT:
        case SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_UNCORRECTABLE_ERRORS:
            *length = 16;
            break;

        case SOC_ARAD_MAC_COUNTERS_RX_CRC_ERRORS_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_RX_LFEC_FEC_CORRECTABLE_ERROR:
        case SOC_ARAD_MAC_COUNTERS_TX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS:
        case SOC_ARAD_MAC_COUNTERS_RX_ASYN_FIFO_RATE_AT_UNITS_OF_40_BITS:
            *length = 32;
            break;

        case SOC_ARAD_MAC_COUNTERS_TX_CONTROL_CELLS_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_TX_DATA_CELL_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_TX_DATA_BYTE_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_RX_CONTROL_CELLS_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_RX_DATA_CELL_COUNTER:
        case SOC_ARAD_MAC_COUNTERS_RX_DATA_BYTE_COUNTER:
            *length = 48;
            break;

        default:
            *length = 0;
            break;
    }

	SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_arad_stat_path_info_get(int unit, soc_dpp_stat_path_info_t *info)
{
    uint32 reg_val=0, count32;
    uint64 reg64_val, count64;
    soc_reg_above_64_val_t reg_val_above_64; 
    int core, found;

    SOCDNX_INIT_FUNC_DEFS;

    info->ingress_core = -1;
    info->egress_core = -1;
    info->drop = soc_dpp_stat_path_drop_stage_none;
    
    if (SOC_IS_JERICHO(unit) && !SOC_IS_QAX(unit))
    {
        found = 0;
        SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
            
            SOCDNX_IF_ERR_EXIT(READ_IRR_IQM_INTERFACE_COUNTERr(unit, core, reg_val_above_64));
            count32 =  soc_reg_above_64_field32_get(unit, IRR_IQM_INTERFACE_COUNTERr, reg_val_above_64, IQM_N_PC_COUNTERf);
            if (count32)
            {   
                info->ingress_core = core;
                found = 1;
                break;
            }
        }
    }
    else
    {
        info->ingress_core = 0;
        found = 1;
    }



    if (found) {
        
        found = 0;
        if (SOC_REG_IS_VALID(unit, FDT_TRANSMITTED_DATA_CELLS_COUNTERr)) {
            SOCDNX_IF_ERR_EXIT(READ_FDT_TRANSMITTED_DATA_CELLS_COUNTERr(unit, &reg_val));
        } else if (SOC_REG_IS_VALID(unit, ITE_INCOMING_PACKET_CNTRr)) {
            SOCDNX_IF_ERR_EXIT(READ_ITE_INCOMING_PACKET_CNTRr(unit, &reg_val));
        }
        found += (reg_val ? 1 : 0);
        if (SOC_REG_IS_VALID(unit, EGQ_IPT_PACKET_COUNTERr))
        {
            SOCDNX_IF_ERR_EXIT(READ_EGQ_IPT_PACKET_COUNTERr(unit, &reg64_val));
            if (!COMPILER_64_IS_ZERO(reg64_val))
            {
                found = 1;
            }
        } 
#ifdef BCM_JERICHO_SUPPORT
        else {
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                reg_val = 0;
                if (SOC_REG_IS_VALID(unit, FDA_EGQ_CELLS_IN_CNT_IPTr)) {
                    SOCDNX_IF_ERR_EXIT(READ_FDA_EGQ_CELLS_IN_CNT_IPTr(unit, core, &reg_val));
                } else if (SOC_REG_IS_VALID(unit, EGQ_FQP_PACKET_COUNTERr)) {
                    SOCDNX_IF_ERR_EXIT(READ_EGQ_FQP_PACKET_COUNTERr(unit, core, &reg64_val));
                    reg_val = !COMPILER_64_IS_ZERO(reg64_val); 
                }
                found += (reg_val ? 1 : 0);
            }
        }
#endif 
        if (found) {
            
            found = 0;
            SOC_DPP_CORES_ITER(SOC_CORE_ALL, core) {
                SOCDNX_IF_ERR_EXIT(READ_EPNI_EPE_PACKET_COUNTERr(unit, core, &reg64_val));
                count64 = soc_reg64_field_get(unit, EPNI_EPE_PACKET_COUNTERr, reg64_val, EPE_PACKET_COUNTERf);
                if (!COMPILER_64_IS_ZERO(count64)) {
                    info->egress_core = core;
                    found = 1;
                    break;
                }
            }

            if (!found) {
                info->drop = soc_dpp_stat_path_drop_stage_egress_tm; 
            }
        } else {
            info->drop = soc_dpp_stat_path_drop_stage_ingress_tm; 
        }
    } else {
        info->drop = soc_dpp_stat_path_drop_stage_ingress_no_packet; 
    }
    

exit:
    SOCDNX_FUNC_RETURN;
}


#undef _ERR_MSG_MODULE_NAME

