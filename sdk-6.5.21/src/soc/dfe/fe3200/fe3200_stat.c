/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC FE3200 STAT
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_STAT
#include <shared/bsl.h>
#include <soc/dfe/fe3200/fe3200_stat.h>
#include <soc/error.h>
#include <soc/dcmn/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dfe/cmn/dfe_drv.h>
#include <soc/dfe/cmn/dfe_defs.h>
#include <shared/bitop.h>
#include <soc/dcmn/error.h>
#include <bcm/stat.h>
#include <soc/dcmn/dcmn_defs.h>

int soc_fe3200_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read){

    int blk_idx, lane_idx,nof_links_in_mac;
    uint32 control_reg;
    uint64 mask;
    int length;
    int  rc;
    SOCDNX_INIT_FUNC_DEFS; 

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);
    blk_idx = INT_DEVIDE(port, nof_links_in_mac);
    lane_idx = port % nof_links_in_mac;

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);
    SOC_DFE_ALLOW_WARMBOOT_WRITE(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg),rc);
    SOCDNX_IF_ERR_EXIT(rc);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stat_counter_length_get, (unit, counter_id, &length)));
    
    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_AND((*val), mask);
    

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_fe3200_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size)
{
    int i=0;
    int rc=0;
    uint64 val64;
    SOCDNX_INIT_FUNC_DEFS;
    COMPILER_64_ZERO(*value);
    for (i=0;i<arr_size;i++)
    {
        if (COUNTER_IS_COLLECTED(SOC_CONTROL(unit)->controlled_counters[counters[i]])) {
            if (SOC_CONTROL(unit)->counter_interval != 0) {
                rc = soc_counter_get(unit, port, counters[i], 0, &val64); 
            } else {
                
                rc = SOC_CONTROL(unit)->controlled_counters[counters[i]].controlled_counter_f(unit, SOC_CONTROL(unit)->controlled_counters[counters[i]].counter_id, port, &val64, NULL);
            }
        } else {
            
            rc = soc_fe3200_controlled_counter_get(unit, counters[i], port, &val64, NULL);
        }
        SOCDNX_IF_ERR_EXIT(rc);
        COMPILER_64_ADD_64(*value,val64);


    }
exit:
    SOCDNX_FUNC_RETURN;

}

soc_error_t 
soc_fe3200_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type)
{
    soc_dcmn_port_pcs_t pcs;
    SOCDNX_INIT_FUNC_DEFS;

    switch(type) {
    case snmpBcmTxControlCells:
    case snmpBcmTxDataCells:
    case snmpBcmTxDataBytes:
    case snmpBcmRxCrcErrors:
    case snmpBcmRxControlCells:
    case snmpBcmRxDataCells:
    case snmpBcmRxDataBytes:
    case snmpBcmRxDroppedRetransmittedControl:
    case snmpBcmTxAsynFifoRate:
    case snmpBcmRxAsynFifoRate:
        break;

    
    case snmpBcmRxFecCorrectable:
    case snmpBcmRxFecUncorrectable:
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_pcs_get, (unit, port, &pcs)));
        if(soc_dcmn_port_pcs_8_9_legacy_fec != pcs && 
           soc_dcmn_port_pcs_64_66_fec != pcs &&
           soc_dcmn_port_pcs_64_66_ll_rs_fec != pcs &&
           soc_dcmn_port_pcs_64_66_rs_fec != pcs) 
        {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for FEC ports"),port, type)); 
        }
        break;

    
    case snmpBcmRxBecCrcErrors:
    case snmpBcmRxBecRxFault:
    case snmpBcmTxBecRetransmit:
    case snmpBcmRxBecRetransmit:
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_pcs_get, (unit, port, &pcs)));
        if(soc_dcmn_port_pcs_64_66_bec != pcs) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for BEC ports"),port, type)); 
        }
        break;
        
    
    case snmpBcmRxDisparityErrors:
    case snmpBcmRxCodeErrors:
        SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_port_control_pcs_get, (unit, port, &pcs)));
        if(soc_dcmn_port_pcs_8_10 != pcs) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, counter %d supported only for 8b/10b ports"),port, type)); 
        }
        break;

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, stat type %d isn't supported"),port, type)); 
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int soc_fe_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read){

    int blk_idx, lane_idx,nof_links_in_mac;
    uint32 control_reg;
    uint64 mask;
    int length;
    int  rc;
    SOCDNX_INIT_FUNC_DEFS; 

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    nof_links_in_mac = SOC_DFE_DEFS_GET(unit, nof_links_in_mac);
    blk_idx = INT_DEVIDE(port, nof_links_in_mac);
    lane_idx = port % nof_links_in_mac;

    DFE_UNIT_LOCK_TAKE_SOCDNX(unit);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);
    SOC_DFE_ALLOW_WARMBOOT_WRITE(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg),rc);
    SOCDNX_IF_ERR_EXIT(rc);
    SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

    SOCDNX_IF_ERR_EXIT(MBCM_DFE_DRIVER_CALL(unit, mbcm_dfe_stat_counter_length_get, (unit, counter_id, &length)));
    
    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_AND((*val), mask);
    

exit:
    DFE_UNIT_LOCK_RELEASE_SOCDNX(unit);
    SOCDNX_FUNC_RETURN;
}


soc_controlled_counter_t soc_fe3200_controlled_counter[] = {
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_crc_errors_data_cells,
        "RX CRC errors",
        "RX CRC errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass,
        "RX CRC errors nonbypass",
        "RX CRC errors nonbypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_crc_errors_control_cells_bypass,
        "RX CRC errors bypass",
        "RX CRC errors bypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_fec_correctable_error,
        "RX FEC correctable error",
        "RX FEC correctable error",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_control_cells_counter,
        "RX Control cells",
        "RX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_data_cell_counter,
        "RX Data cell",
        "RX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_data_byte_counter,
        "RX Data byte ",
        "RX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_dropped_retransmitted_control,
        "RX Dropped retransmitted control",
        "RX Dropped retransmitted control",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_dummy_val_1,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_dummy_val_2,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_tx_asyn_fifo_rate,
        "TX Asyn fifo rate",
        "TX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_asyn_fifo_rate,
        "RX Asyn fifo rate",
        "RX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors,
        "RX FEC uncorrectable errors",
        "RX FEC uncorrectable errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_llfc_primary_pipe,
        "RX Llfc primary pipe",
        "RX Llfc primary pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_llfc_second_pipe,
        "RX Llfc second pipe",
        "RX Llfc second pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_llfc_third_pipe,
        "RX Llfc third pipe",
        "RX Llfc third pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_kpcs_errors_counter,
        "RX Kpcs errors",
        "RX Kpcs errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL
    },
    {
        soc_fe_controlled_counter_get,
        soc_fe3200_counters_rx_kpcs_bypass_errors_counter,
        "RX Kpcs bypass errors",
        "RX Kpcs bypass errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
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
soc_fe3200_controlled_counter_set(int unit)    
{
    SOCDNX_INIT_FUNC_DEFS;
    SOC_CONTROL(unit)->controlled_counters = soc_fe3200_controlled_counter;


    SOCDNX_FUNC_RETURN;

}


soc_error_t 
soc_fe3200_stat_init(int unit){

    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    if (!SOC_WARM_BOOT(unit)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_AC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
        SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_BD_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
    SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(counters);
    SOCDNX_NULL_CHECK(array_size);
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_tx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmTxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_tx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmTxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_tx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxCrcErrors:
        *array_size=3;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_crc_errors_data_cells;
            counters[1] = soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_fe3200_counters_rx_crc_errors_control_cells_bypass;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxFecCorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_fec_correctable_error;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxDroppedRetransmittedControl:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_dropped_retransmitted_control;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmTxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_tx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpBcmRxFecUncorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_UNAVAIL, (_BSL_SOCDNX_MSG("port: %d, stat counter %d isn't supported"),port, type)); 
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_fe3200_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (counter_id)
    {
        case soc_fe3200_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_fe3200_counters_rx_crc_errors_control_cells_bypass:
        case soc_fe3200_counters_rx_lfec_fec_uncorrrectable_errors:
        case soc_fe3200_counters_rx_kpcs_errors_counter:
        case soc_fe3200_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_fe3200_counters_rx_crc_errors_data_cells:
        case soc_fe3200_counters_rx_fec_correctable_error:
        case soc_fe3200_counters_tx_asyn_fifo_rate:
        case soc_fe3200_counters_rx_asyn_fifo_rate:
            *length = 32;
            break;

        case soc_fe3200_counters_tx_control_cells_counter:
        case soc_fe3200_counters_tx_data_cell_counter:
        case soc_fe3200_counters_tx_data_byte_counter:
        case soc_fe3200_counters_rx_control_cells_counter:
        case soc_fe3200_counters_rx_data_cell_counter:
        case soc_fe3200_counters_rx_data_byte_counter:
            *length = 48;
            break;

        case soc_fe3200_counters_rx_llfc_primary_pipe:
        case soc_fe3200_counters_rx_llfc_second_pipe:
        case soc_fe3200_counters_rx_llfc_third_pipe:
            *length = 64;
            break;

        default:
            *length = 0;
            break;
    }


    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME
