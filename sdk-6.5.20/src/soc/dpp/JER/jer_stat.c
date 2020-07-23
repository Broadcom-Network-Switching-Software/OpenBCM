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

#include <soc/dpp/drv.h>
#include <soc/dpp/port.h>
#include <soc/dpp/JER/jer_stat.h>
#include <soc/dpp/JER/jer_nif.h>

#include <soc/dcmn/dcmn_port.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>

#include <soc/dpp/port_sw_db.h>
#include <soc/dpp/mbcm.h>



soc_error_t 
soc_jer_stat_nif_init(int unit)
{
    SOCDNX_INIT_FUNC_DEFS;


    SOCDNX_FUNC_RETURN;
}

int 
soc_jer_nif_controlled_counter_clear(int unit, int port) {

    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    if (IS_IL_PORT(unit,port)) {
        SOC_DPP_ALLOW_WARMBOOT_WRITE(jer_nif_ilkn_counter_clear(unit, port), rv); 
        SOCDNX_IF_ERR_EXIT(rv);
    }
exit:
    SOCDNX_FUNC_RETURN;
}



STATIC int 
soc_jer_nif_controlled_counter_get(int unit, int counter_id, int port, uint64* val, uint32* clear_on_read)
{
    int rv, phys_returned;
    uint32 val_32 = 0;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy;
    phymod_phy_stat_t stat;
    uint32 i, shift;
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane=-1;
    params.phyn=0;

    if (clear_on_read) {
        *clear_on_read = TRUE;
    }
    if(!IS_SFI_PORT(unit,port)) {

        COMPILER_64_ZERO(*val);

            switch (counter_id) {
            case soc_jer_counters_ilkn_rx_pkt_counter:
            case soc_jer_counters_ilkn_tx_pkt_counter:
            case soc_jer_counters_ilkn_rx_byte_counter:
            case soc_jer_counters_ilkn_tx_byte_counter:
            case soc_jer_counters_ilkn_rx_err_pkt_counter:
            case soc_jer_counters_ilkn_tx_err_pkt_counter:
                if (IS_IL_PORT(unit,port)) {
                    SOC_DPP_ALLOW_WARMBOOT_WRITE(jer_nif_ilkn_counter_get(unit, port, counter_id, val), rv); 
                    SOCDNX_IF_ERR_EXIT(rv);
                }
                break;
            case soc_jer_counters_rx_eth_stats_drop_events:
                if ((!IS_IL_PORT(unit,port) || (IS_IL_PORT(unit,port) && SOC_IS_JERICHO_PLUS(unit))) && !IS_QSGMII_PORT(unit, port)) {
                    SOC_DPP_ALLOW_WARMBOOT_WRITE(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_port_prd_drop_count_get,(unit, port, val)), rv); 
                    SOCDNX_IF_ERR_EXIT(rv);
                }
                break;
            case soc_jer_counters_nif_rx_fec_correctable_error:
                if (!IS_IL_PORT(unit,port)) {
                    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SOC_DPP_ALLOW_WARMBOOT_WRITE(phymod_phy_fec_correctable_counter_get(&phy, &val_32),rv);
                    SOCDNX_IF_ERR_EXIT(rv);
                    COMPILER_64_SET(*val, 0, val_32);
                }
                break;
            case soc_jer_counters_nif_rx_fec_uncorrrectable_errors:
                if (!IS_IL_PORT(unit,port)) {
                    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SOC_DPP_ALLOW_WARMBOOT_WRITE(phymod_phy_fec_uncorrectable_counter_get(&phy, &val_32),rv);
                    SOCDNX_IF_ERR_EXIT(rv);
                    COMPILER_64_SET(*val, 0, val_32);
                }
                break;
            case soc_jer_counters_nif_rx_bip_error:
                if (!IS_IL_PORT(unit,port)) {
                    SOCDNX_IF_ERR_EXIT(portmod_port_phy_lane_access_get(unit, port, &params, 1, &phy, &phys_returned, NULL));
                    SOC_DPP_ALLOW_WARMBOOT_WRITE(phymod_phy_stat_get(&phy, &stat),rv);
                    SOCDNX_IF_ERR_EXIT(rv);
                    for (i = 0; i < NUM_OF_LANES_IN_PM; i++) {
                        shift = i;
                        if (phy.access.lane_mask & (1U << shift)) {
                            COMPILER_64_ADD_32(*val, stat.pcs_ber_count[i]);
                            COMPILER_64_ADD_32(*val, stat.pcs_bip_err_count[i]);
                        }
                    }
                }
                break;
            default:
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Counter type %d is not supported for port %d"), counter_id, port));            
            }
    } else {
        COMPILER_64_SET(*val, 0, 0);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_controlled_counter_t soc_jer_controlled_counter[] = {
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_control_cells_counter,
        "TX Control cells",
        "TX Control cells",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
	{
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_data_cell_counter,
        "TX Data cell",
        "TX Data cell",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
	{
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_tx_data_byte_counter,
        "TX Data byte",
        "TX Data byte",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_data_cells,
		"RX CRC errors",
		"RX CRC errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_control_cells_nonbypass,
		"RX CRC errors nonbypass",
		"RX CRC errors nonbypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_crc_errors_control_cells_bypass,
		"RX CRC errors bypass",
		"RX CRC errors bypass",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_fec_correctable_error,
		"RX FEC correctable",
		"RX FEC correctable",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_control_cells_counter,
		"RX Control cells",
		"RX Control cells",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_data_cell_counter,
		"RX Data cell",
		"RX Data cell",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_data_byte_counter,
		"RX Data byte ",
		"RX Data byte",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_dropped_retransmitted_control,
		"RX Dropped retransmitted control",
		"RX Dropped retransmitted control",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_dummy_val_1,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_arad_mac_controlled_counter_get,
        soc_jer_counters_dummy_val_2,
        "Dummy value",
        "Dummy value",
        _SOC_CONTROLLED_COUNTER_FLAG_INVALID,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_tx_asyn_fifo_rate,
		"TX Asyn fifo rate",
		"TX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_asyn_fifo_rate,
		"RX Asyn fifo rate",
		"RX Asyn fifo rate",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_lfec_fec_uncorrrectable_errors,
		"RX FEC uncorrectable errors",
		"RX FEC uncorrectable errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_primary_pipe,
		"RX Llfc primary pipe",
		"RX Llfc primary pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_second_pipe,
		"RX Llfc second pipe",
		"RX Llfc second pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_llfc_third_pipe,
		"RX Llfc third pipe",
		"RX Llfc third pipe",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_kpcs_errors_counter,
		"RX Kpcs errors",
		"RX Kpcs errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
	{
		soc_arad_mac_controlled_counter_get,
		soc_jer_counters_rx_kpcs_bypass_errors_counter,
		"RX Kpcs bypass errors",
		"RX Kpcs bypass errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
		soc_jer_nif_controlled_counter_get,
		soc_jer_counters_ilkn_rx_pkt_counter,
		"RX snmp stats no errors",
		"RX snmp stats no errors",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
		soc_jer_nif_controlled_counter_get,
		soc_jer_counters_ilkn_tx_pkt_counter,
		"TX snmp stats no errors",
		"TX snmp stats no errors",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
        soc_jer_nif_controlled_counter_get,
        soc_jer_counters_ilkn_rx_byte_counter,
        "RX ILKN byte counter",
        "RX ILKN byte counter",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_jer_nif_controlled_counter_get,
        soc_jer_counters_ilkn_tx_byte_counter,
        "TX ILKN byte counter",
        "TX ILKN byte counter",
        _SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
		soc_jer_nif_controlled_counter_get,
		soc_jer_counters_ilkn_rx_err_pkt_counter,
		"RX snmp If in err pkts",
		"RX snmp If in err pkts",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
		soc_jer_nif_controlled_counter_get,
		soc_jer_counters_ilkn_tx_err_pkt_counter,
		"RX snmp If out err pkts",
		"RX snmp If out err pkts",
		_SOC_CONTROLLED_COUNTER_FLAG_TX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_ILKN,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
		soc_jer_nif_controlled_counter_get,
		soc_jer_counters_rx_eth_stats_drop_events,
		"RX nif drop events",
		"RX nif drop events",
		_SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
		COUNTER_IDX_NOT_INITIALIZED,
		NULL,
		NULL
	},
    {
        soc_jer_nif_controlled_counter_get,
        soc_jer_counters_nif_rx_fec_correctable_error,
        "RX NIF FEC correctable",
        "RX NIF FEC correctable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
    {
        soc_jer_nif_controlled_counter_get,
        soc_jer_counters_nif_rx_fec_uncorrrectable_errors,
        "RX NIF FEC uncorrectable",
        "RX NIF FEC uncorrectable",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
        COUNTER_IDX_NOT_INITIALIZED,
        NULL,
        NULL
    },
   {
        soc_jer_nif_controlled_counter_get,
        soc_jer_counters_nif_rx_bip_error,
        "RX NIF BIP error count",
        "RX NIF BIP error count",
        _SOC_CONTROLLED_COUNTER_FLAG_RX | _SOC_CONTROLLED_COUNTER_FLAG_HIGH | _SOC_CONTROLLED_COUNTER_FLAG_MAC | _SOC_CONTROLLED_COUNTER_FLAG_NIF,
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
soc_jer_fabric_stat_init(int unit){

    uint32 reg_val;    
    uint32 fmac_index;
    SOCDNX_INIT_FUNC_DEFS;
    if (!SOC_WARM_BOOT(unit) && !soc_feature(unit, soc_feature_no_fabric)) {
        SOCDNX_IF_ERR_EXIT(READ_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, 0, &reg_val));
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_COUNTER_MODEf, 0);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, DATA_BYTE_COUNTER_HEADERf, 1);
        soc_reg_field_set(unit, FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr, &reg_val, COUNTER_CLEAR_ON_READf, 1);
        if (!SOC_IS_FLAIR(unit)) {
            
            SOCDNX_IF_ERR_EXIT(WRITE_BRDC_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, reg_val));
        } else {
            for (fmac_index = 0; fmac_index < SOC_DPP_DEFS_GET(unit, nof_instances_fmac); fmac_index++)
            {
                SOCDNX_IF_ERR_EXIT(WRITE_FMAC_FMAL_STATISTICS_OUTPUT_CONTROLr(unit, fmac_index, reg_val));
            }
        }
    }
exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
soc_jer_mapping_stat_get(int unit,soc_port_t port, uint32 *counters,int *array_size ,bcm_stat_val_t type,int max_array_size)
{
	SOCDNX_INIT_FUNC_DEFS;
    SOCDNX_NULL_CHECK(counters);
    SOCDNX_NULL_CHECK(array_size);
    switch(type) {
    case snmpBcmTxControlCells:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_tx_control_cells_counter;
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
            counters[0] = soc_jer_counters_tx_data_cell_counter;
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
            counters[0] = soc_jer_counters_tx_data_byte_counter;
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
            counters[0] = soc_jer_counters_rx_crc_errors_data_cells;
            counters[1] = soc_jer_counters_rx_crc_errors_control_cells_nonbypass;
            counters[2] = soc_jer_counters_rx_crc_errors_control_cells_bypass;
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
            if (IS_IL_PORT(unit, port)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("snmpBcmRxFecCorrectable not supported for ILKN")));
            } else if (IS_SFI_PORT(unit, port)) {
                counters[0] = soc_jer_counters_rx_fec_correctable_error;
            } else {
                counters[0] = soc_jer_counters_nif_rx_fec_correctable_error;
            }
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
            counters[0] = soc_jer_counters_rx_control_cells_counter;
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
            counters[0] = soc_jer_counters_rx_data_cell_counter;
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
            counters[0] = soc_jer_counters_rx_data_byte_counter;
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
            counters[0] = soc_jer_counters_rx_dropped_retransmitted_control;
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
            counters[0] = soc_jer_counters_tx_asyn_fifo_rate;
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
            counters[0] = soc_jer_counters_rx_asyn_fifo_rate;
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
            if (IS_IL_PORT(unit, port)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("snmpBcmRxFecUncorrectable not supported for ILKN")));
            } else if(IS_SFI_PORT(unit, port)) {
                counters[0] = soc_jer_counters_rx_lfec_fec_uncorrrectable_errors;
            } else {
                counters[0] = soc_jer_counters_nif_rx_fec_uncorrrectable_errors;
            }
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
		break;
    case snmpBcmRxBipErrorCount:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            if (IS_IL_PORT(unit, port)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("snmpBcmRxBipErrorCount not supported for ILKN")));
            } else if (IS_QSGMII_PORT(unit, port)) {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("snmpBcmRxBipErrorCount not supported for QSGMII")));
            } else {
                counters[0] = soc_jer_counters_nif_rx_bip_error;
            }
        }
        else
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpIfInNUcastPkts:
        *array_size=0;
        break;
    case snmpEtherStatsRXNoErrors:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_ilkn_rx_pkt_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpIfOutNUcastPkts:
        *array_size=0;
        break;
    case snmpEtherStatsTXNoErrors:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_ilkn_tx_pkt_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpIfInErrors:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_ilkn_rx_err_pkt_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpIfOutErrors:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_ilkn_tx_err_pkt_counter;
        } 
        else 
        {
            *array_size=0;
            SOCDNX_EXIT_WITH_ERR(SOC_E_LIMIT, (_BSL_SOCDNX_MSG("array size > max array size received")));
        }
        break;
    case snmpEtherStatsDropEvents:
        *array_size=1;
        if (*array_size<=max_array_size)  
        {
            counters[0] = soc_jer_counters_rx_eth_stats_drop_events;
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
soc_jer_stat_counter_length_get(int unit, int counter_id, int *length)
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (counter_id)
    {
        case soc_jer_counters_rx_crc_errors_control_cells_nonbypass:
        case soc_jer_counters_rx_crc_errors_control_cells_bypass:
        case soc_jer_counters_rx_lfec_fec_uncorrrectable_errors:
        case soc_jer_counters_rx_kpcs_errors_counter:
        case soc_jer_counters_rx_kpcs_bypass_errors_counter:
            *length = 16;
            break;

        case soc_jer_counters_rx_crc_errors_data_cells:
        case soc_jer_counters_rx_fec_correctable_error:
        case soc_jer_counters_tx_asyn_fifo_rate:
        case soc_jer_counters_rx_asyn_fifo_rate:
            *length = 32;
            break;

        case soc_jer_counters_tx_control_cells_counter:
        case soc_jer_counters_tx_data_cell_counter:
        case soc_jer_counters_tx_data_byte_counter:
        case soc_jer_counters_rx_control_cells_counter:
        case soc_jer_counters_rx_data_cell_counter:
        case soc_jer_counters_rx_data_byte_counter:
            *length = 48;
            break;

        case soc_jer_counters_rx_llfc_primary_pipe:
        case soc_jer_counters_rx_llfc_second_pipe:
        case soc_jer_counters_rx_llfc_third_pipe:
            *length = 64;
            break;

        default:
            *length = 0;
            break;
    }


    SOCDNX_FUNC_RETURN;
}

soc_error_t 
soc_jer_stat_controlled_counter_enable_get(int unit, soc_port_t port, int index, int *enable, int *printable) {
    uint32 flags, ilkn_id;
    soc_control_t   *soc;
    SOCDNX_INIT_FUNC_DEFS;

    *enable = 1;
    *printable = 1;
    soc = SOC_CONTROL(unit);

    if (IS_SFI_PORT(unit, port)) 
    {
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_MAC)) 
        {
            *enable = 0;
            SOC_EXIT;
        }
    } else if (IS_IL_PORT(unit, port))
    {

        SOCDNX_IF_ERR_EXIT(soc_port_sw_db_protocol_offset_get(unit, port, 0, &ilkn_id));
        flags = _SOC_CONTROLLED_COUNTER_FLAG_ILKN;

        
        if (SOC_DPP_IS_ILKN_PORT_OVER_FABRIC(unit, ilkn_id)) {
            flags |= _SOC_CONTROLLED_COUNTER_FLAG_MAC;
        }

        
        if (soc->controlled_counters[index].counter_id == soc_jer_counters_rx_eth_stats_drop_events && SOC_IS_JERICHO_PLUS(unit)) {
            flags |= _SOC_CONTROLLED_COUNTER_FLAG_NIF;
        }

        
        if (!(soc->controlled_counters[index].flags & flags)) 
        {
            *enable = 0;
        }
    }
    else
    {
        
        if (!(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NIF)) 
        {
            *enable = 0;
        }
    }

    if(soc->controlled_counters[index].flags & _SOC_CONTROLLED_COUNTER_FLAG_NOT_PRINTABLE) 
    {
        *printable = 0;
    }
    
    if (IS_QSGMII_PORT(unit, port) && soc->controlled_counters[index].counter_id == soc_jer_counters_rx_eth_stats_drop_events)
    {
        *enable = 0;
    }


exit:
    SOCDNX_FUNC_RETURN;
}



#undef _ERR_MSG_MODULE_NAME
