/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC RAMON STAT
 */

#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_STAT

#include <shared/bsl.h>
#include <soc/dnxf/ramon/ramon_stat.h>
#include <soc/dnxf/ramon/ramon_port.h>
#include <soc/error.h>
#include <soc/dnxc/error.h>
#include <soc/defs.h>
#include <soc/error.h>
#include <soc/mcm/allenum.h>
#include <soc/mcm/memregs.h>

#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/cmn/dnxf_defs.h>
#include <shared/bitop.h>
#include <soc/dnxc/error.h>
#include <bcm/stat.h>
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/dnxc_ha.h>
#include <shared/fabric.h>


#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>

soc_controlled_counter_t soc_ramon_controlled_counter[] = {
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_data_cells,
        "RX CRC errors",
        "RX CRC errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_control_cells_nonbypass,
        "RX CRC errors nonbypass",
        "RX CRC errors nonbypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_crc_errors_control_cells_bypass,
        "RX CRC errors bypass",
        "RX CRC errors bypass",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_fec_correctable_error,
        "RX FEC correctable error",
        "RX FEC correctable error",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_control_cells_counter,
        "RX Control cells",
        "RX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_data_cell_counter,
        "RX Data cell",
        "RX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_data_byte_counter,
        "RX Data byte ",
        "RX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_dropped_retransmitted_control,
        "RX Dropped retransmitted control",
        "RX Dropped retransmitted control",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_dummy_val_1,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_dummy_val_2,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_tx_asyn_fifo_rate,
        "TX Asyn fifo rate",
        "TX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_asyn_fifo_rate,
        "RX Asyn fifo rate",
        "RX Asyn fifo rate",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors,
        "RX FEC uncorrectable errors",
        "RX FEC uncorrectable errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_primary_pipe,
        "RX Llfc primary pipe",
        "RX Llfc primary pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_second_pipe,
        "RX Llfc second pipe",
        "RX Llfc second pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_llfc_third_pipe,
        "RX Llfc third pipe",
        "RX Llfc third pipe",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_kpcs_errors_counter,
        "RX Kpcs errors",
        "RX Kpcs errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_kpcs_bypass_errors_counter,
        "RX Kpcs bypass errors",
        "RX Kpcs bypass errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_rs_fec_bit_error_counter,
        "RX RS Fec bit errors",
        "RX RS Fec bit errors",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_ramon_controlled_counter_get,
        soc_ramon_counters_rx_rs_fec_symbol_error_counter,
        "RX RS Fec sybmol errors",
        "RX RS Fec sybmol errors",
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
 
 
shr_error_e
soc_ramon_controlled_counter_set(int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SOC_CONTROL(unit)->controlled_counters = soc_ramon_controlled_counter;

    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_stat_init(int unit){

    uint32 reg_val;
    int fmac_index = 0;
    soc_pbmp_t enabled_fmacs_bmp;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_ramon_port_enabled_fmac_blocks_get(unit, &enabled_fmacs_bmp));

    if (!SOC_WARM_BOOT(unit)) {
        SOC_PBMP_ITER(enabled_fmacs_bmp, fmac_index)
        {
            SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, fmac_index, &reg_val));
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
            soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
            SHR_IF_ERR_EXIT(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, fmac_index, reg_val));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_mapping_stat_get(int unit,bcm_port_t port, int *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(counters, _SHR_E_PARAM, "counters");
    SHR_NULL_CHECK(array_size, _SHR_E_PARAM, "array_size");
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_tx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_tx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_tx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxCrcErrors:
        *array_size=3;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_crc_errors_data_cells;
            counters[1] = soc_ramon_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_ramon_counters_rx_crc_errors_control_cells_bypass;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxFecCorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_fec_correctable_error;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_control_cells_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDataCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_data_cell_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDataBytes:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_data_byte_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxDroppedRetransmittedControl:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_dropped_retransmitted_control;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmTxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_tx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxAsynFifoRate:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_asyn_fifo_rate;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxFecUncorrectable:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxRsFecBitError:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_rs_fec_bit_error_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    case snmpBcmRxRsFecSymbolError:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_ramon_counters_rx_rs_fec_symbol_error_counter;
        } 
        else 
        {
            *array_size=0;
            SHR_ERR_EXIT(_SHR_E_LIMIT, "array size > max array size received");
        }
        break;
    default:
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, stat counter %d isn't supported",port, type); 
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
soc_ramon_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (counter_id)
    {
        case soc_ramon_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_ramon_counters_rx_crc_errors_control_cells_bypass:
        case soc_ramon_counters_rx_kpcs_errors_counter:
        case soc_ramon_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_ramon_counters_rx_crc_errors_data_cells:
        case soc_ramon_counters_rx_fec_correctable_error:
        case soc_ramon_counters_tx_asyn_fifo_rate:
        case soc_ramon_counters_rx_asyn_fifo_rate:
        case soc_ramon_counters_rx_lfec_fec_uncorrrectable_errors:
            *length = 32;
            break;

        case soc_ramon_counters_tx_control_cells_counter:
        case soc_ramon_counters_tx_data_cell_counter:
        case soc_ramon_counters_tx_data_byte_counter:
        case soc_ramon_counters_rx_control_cells_counter:
        case soc_ramon_counters_rx_data_cell_counter:
        case soc_ramon_counters_rx_data_byte_counter:
        case soc_ramon_counters_rx_rs_fec_bit_error_counter:
        case soc_ramon_counters_rx_rs_fec_symbol_error_counter:
            *length = 48;
            break;

        case soc_ramon_counters_rx_llfc_primary_pipe:
        case soc_ramon_counters_rx_llfc_second_pipe:
        case soc_ramon_counters_rx_llfc_third_pipe:
            *length = 64;
            break;

        default:
            *length = 0;
            break;
    }


    SHR_FUNC_EXIT;
}


shr_error_e 
soc_ramon_stat_is_supported_type(int unit, bcm_port_t port, bcm_stat_val_t type)
{
    bcm_port_resource_t resource;
    SHR_FUNC_INIT_VARS(unit);

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

    case snmpBcmRxRsFecBitError:
    case snmpBcmRxRsFecSymbolError:
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
        if(bcmPortPhyFecRs108 != resource.fec_type &&
           bcmPortPhyFecRs206 != resource.fec_type &&
           bcmPortPhyFecRs304 != resource.fec_type &&
           bcmPortPhyFecRs545 != resource.fec_type)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, counter %d supported only for RS FEC ports", port, type); 
        }
        break;

    
    case snmpBcmRxFecCorrectable:
    case snmpBcmRxFecUncorrectable:
        SHR_IF_ERR_EXIT(soc_dnxc_port_resource_get(unit, port, &resource));
        if(bcmPortPhyFecBaseR != resource.fec_type &&
           bcmPortPhyFecRs108 != resource.fec_type &&
           bcmPortPhyFecRs206 != resource.fec_type &&
           bcmPortPhyFecRs304 != resource.fec_type &&
           bcmPortPhyFecRs545 != resource.fec_type)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, counter %d supported only for FEC ports",port, type); 
        }
        break;

    default:
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "port: %d, stat type %d isn't supported",port, type); 
    }

exit:
    SHR_FUNC_EXIT;
}

int soc_ramon_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read){

    int blk_idx, lane_idx,nof_links_in_fmac;
    uint32 control_reg;
    uint64 mask;
    int length;
    int  rc = _SHR_E_NONE;
    DNXF_UNIT_LOCK_INIT(unit);
    SHR_FUNC_INIT_VARS(unit);

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
    blk_idx = INT_DEVIDE(port, nof_links_in_fmac);
    lane_idx = port % nof_links_in_fmac;

    DNXF_UNIT_LOCK_TAKE(unit);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, &control_reg));
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, LANE_SELECTf, lane_idx);
    soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &control_reg, COUNTER_SELECTf, counter_id);

    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, blk_idx, control_reg);
    
    SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));

    SHR_IF_ERR_EXIT(rc);
    SHR_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUTr(unit, blk_idx, val));

    SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL(unit, mbcm_dnxf_stat_counter_length_get, (unit, counter_id, &length)));
    
    COMPILER_64_MASK_CREATE(mask, length, 0);
    COMPILER_64_AND((*val), mask);
    

exit:
    DNXF_UNIT_LOCK_RELEASE(unit);
    SHR_FUNC_EXIT;
}



shr_error_e
soc_ramon_stat_get(int unit,bcm_port_t port,uint64 *value,int *counters,int arr_size)
{
    int i=0;
    int rc=0;
    uint64 val64;
    SHR_FUNC_INIT_VARS(unit);
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
            
            rc = soc_ramon_controlled_counter_get(unit, counters[i], port, &val64, NULL);
        }
        SHR_IF_ERR_EXIT(rc);
        COMPILER_64_ADD_64(*value,val64);


    }
exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
soc_ramon_fabric_stat_verify(int unit, int link, int pipe)
{
    int nof_pipes, nof_links;
    SHR_FUNC_INIT_VARS(unit);

    nof_pipes = dnxf_data_fabric.pipes.max_nof_pipes_get(unit);
    nof_links = dnxf_data_port.general.nof_links_get(unit);

    if ( pipe > _SOC_RAMON_FABRIC_STAT_PIPE_NO_VERIFY)
    {
        SHR_RANGE_VERIFY(pipe, 0, nof_pipes-1, _SHR_E_PARAM, "pipe %d argument is out of range.", pipe);
    }

    if ( link >= nof_links)
    {
        SHR_RANGE_VERIFY(link, 0, nof_links-1, _SHR_E_PARAM, "link %d argument is out of range.", link);
    }

exit:
    SHR_FUNC_EXIT;
}


shr_error_e
soc_ramon_fabric_stat_get(int unit, bcm_fabric_stat_index_t index, bcm_fabric_stat_t stat, uint64 *value)
{
    uint32 reg_val;
    int block_idx, pipe_idx, prio_idx;
    int nof_dch_blocks, nof_lcm_blocks, nof_links_in_fmac, nof_links_in_qrh;
    int link, pipe;
    int max_nof_pipes, max_nof_priorities;
    int fmac_idx, link_idx_in_fmac, qrh_idx, link_idx_in_qrh;
    uint64 count;
    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ZERO(*value);
    COMPILER_64_ZERO(count);

    nof_dch_blocks = dnxf_data_device.blocks.nof_instances_dch_get(unit);
    nof_lcm_blocks = dnxf_data_device.blocks.nof_instances_lcm_get(unit);
    max_nof_pipes = dnxf_data_fabric.pipes.max_nof_pipes_get(unit);
    max_nof_priorities = dnxf_data_fabric.congestion.nof_threshold_priorities_get(unit);

    if (_SHR_FABRIC_STAT_INDEX_IS_DEVICE(index))
    {
        switch (stat)
        {
            case bcmFabricStatDeviceReachDrop:
                
                for ( block_idx = 0; block_idx < nof_dch_blocks; block_idx++)
                {
                    SHR_IF_ERR_EXIT(READ_DCH_UNREACH_DEST_CNTr(unit, block_idx, &reg_val));
                    count = soc_reg64_field32_get(unit, DCH_UNREACH_DEST_CNTr, reg_val, UNREACH_DEST_CNTf);
                    COMPILER_64_ADD_64(*value, count);
                }
                break;
            case bcmFabricStatDeviceGlobalDrop:
                
                for ( block_idx = 0; block_idx < nof_dch_blocks; block_idx++)
                {
                    
                    SHR_IF_ERR_EXIT(READ_DCH_UNREACH_DEST_CNTr(unit, block_idx, &reg_val));
                    count = soc_reg64_field32_get(unit, DCH_UNREACH_DEST_CNTr, reg_val, UNREACH_DEST_CNTf);
                    COMPILER_64_ADD_64(*value, count);

                    
                    SHR_IF_ERR_EXIT(READ_DCH_UN_EXP_DISC_CNT_3r(unit, block_idx, &reg_val));
                    count = soc_reg64_field32_get(unit, DCH_UN_EXP_DISC_CNT_3r, reg_val, UN_EXP_DISC_CNT_3f);
                    COMPILER_64_ADD_64(*value, count);

                    for (pipe_idx = 0; pipe_idx < max_nof_pipes; pipe_idx++)
                    {
                        
                        SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_40r(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_AUTO_DOC_NAME_40r, reg_val, AUTO_DOC_NAME_40f);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_DCH_UN_EXP_DISC_CNT_3_PNr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_UN_EXP_DISC_CNT_3_PNr, reg_val, UN_EXP_DISC_CNT_3_PNf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_DCH_UN_EXP_DISC_CNT_PNr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_UN_EXP_DISC_CNT_PNr, reg_val, UN_EXP_DISC_CNT_PNf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_DCH_FIFO_DISCARD_CNT_Pr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_FIFO_DISCARD_CNT_Pr, reg_val, FIFO_DISCARD_CNT_P_Nf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_DCH_AUTO_DOC_NAME_41r(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_AUTO_DOC_NAME_41r, reg_val, AUTO_DOC_NAME_41f);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_DCH_DROP_LOW_PRIORITY_CNT_Pr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, DCH_DROP_LOW_PRIORITY_CNT_Pr, reg_val, DROP_LOW_PRIORITY_CNT_P_Nf);
                        COMPILER_64_ADD_64(*value, count);
                    }
                }

                
                for ( block_idx = 0; block_idx < nof_lcm_blocks; block_idx++)
                {
                    
                    SHR_IF_ERR_EXIT(READ_LCM_ADMIT_DROP_CNTr(unit, block_idx, 0 , &reg_val));
                    count = soc_reg64_field32_get(unit, LCM_ADMIT_DROP_CNTr, reg_val, ADMIT_DROP_REPLICATION_CNT_Nf);
                    COMPILER_64_ADD_64(*value, count);
                    SHR_IF_ERR_EXIT(READ_LCM_ADMIT_DROP_CNTr(unit, block_idx, 1 , &reg_val));
                    count = soc_reg64_field32_get(unit, LCM_ADMIT_DROP_CNTr, reg_val, ADMIT_DROP_REPLICATION_CNT_Nf);
                    COMPILER_64_ADD_64(*value, count);

                    
                    SHR_IF_ERR_EXIT(READ_LCM_ADMIT_DROP_CPU_CNTr(unit, block_idx, &reg_val));
                    count = soc_reg64_field32_get(unit, LCM_ADMIT_DROP_CPU_CNTr, reg_val, ADMIT_DROP_CPU_CNT_0f);
                    COMPILER_64_ADD_64(*value, count);
                    count = soc_reg64_field32_get(unit, LCM_ADMIT_DROP_CPU_CNTr, reg_val, ADMIT_DROP_CPU_CNT_1f);
                    COMPILER_64_ADD_64(*value, count);

                    for (prio_idx = 0; prio_idx < max_nof_priorities; prio_idx++)
                    {
                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_DROPPED_IP_PR_P_0_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_DROPPED_IP_PR_P_0_CNTr, reg_val, DTL_DROPPED_IP_PR_NP_0_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_DROPPED_IP_PR_P_1_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_DROPPED_IP_PR_P_1_CNTr, reg_val, DTL_DROPPED_IP_PR_NP_1_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_DROPPED_IP_PR_P_2_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_DROPPED_IP_PR_P_2_CNTr, reg_val, DTL_DROPPED_IP_PR_NP_2_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_LCLRT_DROPPED_IP_PR_P_0_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_LCLRT_DROPPED_IP_PR_P_0_CNTr, reg_val, DTL_LCLRT_DROPPED_IP_PR_NP_0_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_LCLRT_DROPPED_IP_PR_P_1_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_LCLRT_DROPPED_IP_PR_P_1_CNTr, reg_val, DTL_LCLRT_DROPPED_IP_PR_NP_1_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_LCLRT_DROPPED_IP_PR_P_2_CNTr(unit, block_idx, prio_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_LCLRT_DROPPED_IP_PR_P_2_CNTr, reg_val, DTL_LCLRT_DROPPED_IP_PR_NP_2_CNTf);
                        COMPILER_64_ADD_64(*value, count);
                    }

                    for (pipe_idx = 0; pipe_idx < max_nof_pipes; pipe_idx++)
                    {
                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_LCLRT_DROPPED_SRC_P_CNTr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_LCLRT_DROPPED_SRC_P_CNTr, reg_val, DTL_LCLRT_DROPPED_SRC_P_N_CNTf);
                        COMPILER_64_ADD_64(*value, count);

                        
                        SHR_IF_ERR_EXIT(READ_LCM_DTL_DROPPED_SRC_P_CNTr(unit, block_idx, pipe_idx, &reg_val));
                        count = soc_reg64_field32_get(unit, LCM_DTL_DROPPED_SRC_P_CNTr, reg_val, DTL_DROPPED_SRC_P_N_CNTf);
                        COMPILER_64_ADD_64(*value, count);
                    }
                }
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Enum %d you passed with stat is not supported with DEVICE index type.", stat);
        }
    }
    else if (_SHR_FABRIC_STAT_INDEX_IS_LINK(index))
    {
        switch (stat)
        {
            case bcmFabricStatQueueLinkMaxWmkLevel:
                link = _SHR_FABRIC_STAT_INDEX_LINK_GET(index);
                
                SHR_IF_ERR_EXIT(soc_ramon_fabric_stat_verify(unit, link, _SOC_RAMON_FABRIC_STAT_PIPE_NO_VERIFY));

                nof_links_in_fmac = dnxf_data_device.blocks.nof_links_in_fmac_get(unit);
                fmac_idx = link / nof_links_in_fmac;
                link_idx_in_fmac = link % nof_links_in_fmac;

                SHR_IF_ERR_EXIT(READ_FMAC_FMAL_RCI_GCI_WMKr(unit, fmac_idx, link_idx_in_fmac, &reg_val));
                count = soc_reg64_field32_get(unit, FMAC_FMAL_RCI_GCI_WMKr, reg_val, FMAL_N_TX_RCI_WMKf);
                COMPILER_64_ADD_64(*value, count);
                break;

            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Enum %d you passed with stat is not supported with LINK index type.", stat);
        }
    }
    else if (_SHR_FABRIC_STAT_INDEX_IS_LINK_PIPE(index))
    {
        switch (stat)
        {
            case bcmFabricStatQueueLinkPipeCurrOccupancyBytes:
                link = _SHR_FABRIC_STAT_INDEX_LINK_GET(index);
                pipe = _SHR_FABRIC_STAT_INDEX_PIPE_GET(index);

                
                SHR_IF_ERR_EXIT(soc_ramon_fabric_stat_verify(unit, link, pipe));

                nof_links_in_qrh = dnxf_data_device.blocks.nof_links_in_qrh_get(unit);
                qrh_idx = link / nof_links_in_qrh;
                link_idx_in_qrh = link % nof_links_in_qrh;

                
                SHR_IF_ERR_EXIT(READ_QRH_LOAD_BALANCE_MAX_DEBUG_LINK_Pr(unit, qrh_idx, pipe, &reg_val));
                soc_reg_field_set(unit, QRH_LOAD_BALANCE_MAX_DEBUG_LINK_Pr, &reg_val, LOAD_BALANCE_MAX_DEBUG_LINK_NUM_P_Nf, link_idx_in_qrh);
                SHR_IF_ERR_EXIT(WRITE_QRH_LOAD_BALANCE_MAX_DEBUG_LINK_Pr(unit, qrh_idx, pipe, reg_val));

                
                SHR_IF_ERR_EXIT(READ_QRH_LOAD_BALANCE_MAX_DEBUG_LINK_Pr(unit, qrh_idx, pipe, &reg_val));
                count = soc_reg64_field32_get(unit, QRH_LOAD_BALANCE_MAX_DEBUG_LINK_Pr, reg_val, LOAD_BALANCE_MAX_DEBUG_LINK_VALUE_P_Nf);
                
                COMPILER_64_UMUL_32(count, 64);
                COMPILER_64_ADD_64(*value, count);

                break;
            default:
                SHR_ERR_EXIT(_SHR_E_PARAM, "Enum %d you passed with stat is not supported with LINK_PIPE index type.", stat);
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_fabric_stat_index_t type you passed is not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}

#undef BSL_LOG_MODULE
