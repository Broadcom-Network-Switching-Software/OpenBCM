/** \file mib_stat_dbal.c
 * 
 *
 * MIB stat procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COMMON

#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_mib.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <sal/types.h>
#include <sal/core/boot.h>
#include <soc/drv.h>
#include "mib_stat_dbal.h"

static soc_reg_above_64_val_t *ctrs_saved_row[SOC_MAX_NUM_DEVICES];

/*Initialize the database for storing counter values per row*/
/*The global ctrs_saved_row was protected by COUNTER_LOCK().*/
void
dnx_mib_counter_values_database_init(
    int unit)
{
    int number_of_entries;
    int nof_row_per_cdu_lane = dnx_data_mib.general.nof_row_per_cdu_lane_get(unit);
    int nof_phys_per_cdu;

    nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    if (ctrs_saved_row[unit] == NULL)
    {
        number_of_entries = DNX_DATA_MAX_NIF_ETH_CDU_NOF * nof_row_per_cdu_lane * nof_phys_per_cdu;
        ctrs_saved_row[unit] = sal_alloc(number_of_entries * sizeof(soc_reg_above_64_val_t), "ctrs_saved_row");

        sal_memset(ctrs_saved_row[unit], 0, number_of_entries * sizeof(soc_reg_above_64_val_t));
    }
}

/*Deinitialize the database for storing counter values per row*/
void
dnx_mib_counter_values_database_deinit(
    int unit)
{

    SHR_FREE(ctrs_saved_row[unit]);
}

/**
* \brief
*   initialize mib stat fabric by fmac instance
* \param [in] unit - unit id
* \param [in] fmac_idx - fmac instance
* \return
*   shr_error_e - Error Type
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
mib_stat_fabric_fmac_hw_init(
    int unit,
    int fmac_idx)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /** Write default values to DBAL_TABLE_FABRIC_STAT_CTRL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLEAR_ON_READ, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_MODE, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DATA_COUNTER_HEADER, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get fabric counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] fmac_idx - fabric index.
 * \param [in] lane_idx - fabric lane index.
 * \param [in] hw_counter_id - counter hw select id (used for indirect hw access).
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_mib_fmac_stat_dbal_get(
    int unit,
    uint32 fmac_idx,
    uint32 lane_idx,
    int hw_counter_id,
    uint64 *val)
{

    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");
    if (hw_counter_id < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid counter id");
    }

    /** select read which counter */

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_CTRL, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE_SELECT, INST_SINGLE, lane_idx);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNTER_SELECT, INST_SINGLE, hw_counter_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    DBAL_HANDLE_FREE(unit, entry_handle_id);

    /** request the relevant counter*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_COUNTERS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FMAC_ID, fmac_idx);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_FMAC_COUNTER, INST_SINGLE, val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_nif_eth_stat_dbal_table_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    dbal_tables_e * table_id)
{
    dnx_algo_port_ethu_access_info_t ethu_info;
    int result;
    int ii;
    dbal_table_field_info_t field_info = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    /** check for PM type - set keys accordingly */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
    {
        dbal_tables_e tables_id[] =
            { DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART1, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_RX_PART2,
            DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART1, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_TX_PART2
        };

        /** find out which CLU table */
        for (ii = 0; ii < COUNTOF(tables_id); ii++)
        {
            result =
                dbal_tables_field_info_get_no_err(unit, tables_id[ii], field_id, FALSE, 0, INST_SINGLE, &field_info);
            if (result == _SHR_E_NONE)
            {
                *table_id = tables_id[ii];
                break;
            }
            else if (result == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(result);
            }
        }
    }
    else if (ethu_info.imb_type == imbDispatchTypeImb_clu)
    {
        dbal_tables_e tables_id[] =
            { DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART1, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_RX_PART2,
            DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART1, DBAL_TABLE_NIF_CLU_MIB_COUNTERS_COMMON_TX_PART2,
            DBAL_TABLE_NIF_CLU_MIB_COUNTERS_DEDICATED
        };

        /** find out which CLU table */
        for (ii = 0; ii < COUNTOF(tables_id); ii++)
        {
            result =
                dbal_tables_field_info_get_no_err(unit, tables_id[ii], field_id, FALSE, 0, INST_SINGLE, &field_info);
            if (result == _SHR_E_NONE)
            {
                *table_id = tables_id[ii];
                break;
            }
            else if (result == _SHR_E_NOT_FOUND)
            {
                continue;
            }
            else
            {
                SHR_IF_ERR_EXIT(result);
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PM type %d is not supported for port %d.", ethu_info.imb_type, port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function get one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] field_id - dbal_field.
 * \param [in] port - logic port num.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_mib_nif_eth_stat_dbal_get(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 *val)
{
    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    dnx_algo_port_ethu_access_info_t ethu_info;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    /** get dbal table id */
    SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_table_get(unit, port, field_id, &table_id));

    /** check for PM type - set keys accordingly */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

        nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
        pm_idx = first_phy / nof_phys_per_cdu;
        lane_idx = first_phy % nof_phys_per_cdu;

        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);
    }
    else if (ethu_info.imb_type == imbDispatchTypeImb_clu)
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU_LOGICAL_PORT, port);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PM type %d is not supported for port %d.", ethu_info.imb_type, port);
    }

    dbal_value_field64_request(unit, entry_handle_id, field_id, INST_SINGLE, val);

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /** since CLMAC access on adapter doesn't work on some devices, return always 0 for adapter */
#ifdef ADAPTER_SERVER_MODE
    COMPILER_64_ZERO(*val);
#endif

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function set one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] field_id - dbal_field.
 * \param [in] port - logic port num.
 * \param [out] val - the vaule of the counter.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_mib_nif_eth_stat_dbal_set(
    int unit,
    bcm_port_t port,
    dbal_fields_e field_id,
    uint64 val)
{

    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    dnx_algo_port_ethu_access_info_t ethu_info;
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** get dbal table id */
    SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_table_get(unit, port, field_id, &table_id));

    /** Set keys according to PM type */
    SHR_IF_ERR_EXIT(dnx_algo_port_ethu_access_info_get(unit, port, &ethu_info));
    if (ethu_info.imb_type == imbDispatchTypeImb_cdu)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

        nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
        pm_idx = first_phy / nof_phys_per_cdu;
        lane_idx = first_phy % nof_phys_per_cdu;

        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);
    }
    else if (ethu_info.imb_type == imbDispatchTypeImb_clu)
    {
        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLU_LOGICAL_PORT, port);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "PM type %d is not supported for port %d.", ethu_info.imb_type, port);
    }

    /** set value field */
    dbal_entry_value_field64_set(unit, entry_handle_id, field_id, INST_SINGLE, val);

    /** commit value */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_counter_id_to_dbal_field_id_get(
    int unit,
    int port,
    dnx_mib_counter_t counter_id,
    dbal_fields_e * field_id,
    uint32 *clear_on_read)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (counter_id)
    {
        case dnx_mib_counter_nif_eth_r64:
            *field_id = DBAL_FIELD_R64;
            break;
        case dnx_mib_counter_nif_eth_r127:
            *field_id = DBAL_FIELD_R127;
            break;
        case dnx_mib_counter_nif_eth_r255:
            *field_id = DBAL_FIELD_R255;
            break;
        case dnx_mib_counter_nif_eth_r511:
            *field_id = DBAL_FIELD_R511;
            break;
        case dnx_mib_counter_nif_eth_r1023:
            *field_id = DBAL_FIELD_R1023;
            break;
        case dnx_mib_counter_nif_eth_r1518:
            *field_id = DBAL_FIELD_R1518;
            break;
        case dnx_mib_counter_nif_eth_rmgv:
            *field_id = DBAL_FIELD_RMGV;
            break;
        case dnx_mib_counter_nif_eth_r2047:
            *field_id = DBAL_FIELD_R2047;
            break;
        case dnx_mib_counter_nif_eth_r4095:
            *field_id = DBAL_FIELD_R4095;
            break;
        case dnx_mib_counter_nif_eth_r9216:
            *field_id = DBAL_FIELD_R9216;
            break;
        case dnx_mib_counter_nif_eth_r16383:
            *field_id = DBAL_FIELD_R16383;
            break;
        case dnx_mib_counter_nif_eth_rbca:
            *field_id = DBAL_FIELD_RBCA;
            break;
        case dnx_mib_counter_nif_eth_rprog0:
            *field_id = DBAL_FIELD_RPROG0;
            break;
        case dnx_mib_counter_nif_eth_rprog1:
            *field_id = DBAL_FIELD_RPROG1;
            break;
        case dnx_mib_counter_nif_eth_rprog2:
            *field_id = DBAL_FIELD_RPROG2;
            break;
        case dnx_mib_counter_nif_eth_rprog3:
            *field_id = DBAL_FIELD_RPROG3;
            break;
        case dnx_mib_counter_nif_eth_rpkt:
            *field_id = DBAL_FIELD_RPKT;
            break;
        case dnx_mib_counter_nif_eth_rpok:
            *field_id = DBAL_FIELD_RPOK;
            break;
        case dnx_mib_counter_nif_eth_ruca:
            *field_id = DBAL_FIELD_RUCA;
            break;
        case dnx_mib_counter_nif_eth_rmca:
            *field_id = DBAL_FIELD_RMCA;
            break;
        case dnx_mib_counter_nif_eth_rxpf:
            *field_id = DBAL_FIELD_RXPF;
            break;
        case dnx_mib_counter_nif_eth_rxpp:
            *field_id = DBAL_FIELD_RXPP;
            break;
        case dnx_mib_counter_nif_eth_rxcf:
            *field_id = DBAL_FIELD_RXCF;
            break;
        case dnx_mib_counter_nif_eth_rfcs:
            *field_id = DBAL_FIELD_RFCS;
            break;
        case dnx_mib_counter_nif_eth_rerpkt:
            *field_id = DBAL_FIELD_RERPKT;
            break;
        case dnx_mib_counter_nif_eth_rflr:
            *field_id = DBAL_FIELD_RFLR;
            break;
        case dnx_mib_counter_nif_eth_rjbr:
            *field_id = DBAL_FIELD_RJBR;
            break;
        case dnx_mib_counter_nif_eth_rmtue:
            *field_id = DBAL_FIELD_RMTUE;
            break;
        case dnx_mib_counter_nif_eth_rovr:
            *field_id = DBAL_FIELD_ROVR;
            break;
        case dnx_mib_counter_nif_eth_rvln:
            *field_id = DBAL_FIELD_RVLN;
            break;
        case dnx_mib_counter_nif_eth_rdvln:
            *field_id = DBAL_FIELD_RDVLN;
            break;
        case dnx_mib_counter_nif_eth_rxuo:
            *field_id = DBAL_FIELD_RXUO;
            break;
        case dnx_mib_counter_nif_eth_rxuda:
            *field_id = DBAL_FIELD_RXUDA;
            break;
        case dnx_mib_counter_nif_eth_rxwsa:
            *field_id = DBAL_FIELD_RXWSA;
            break;
        case dnx_mib_counter_nif_eth_rprm:
            *field_id = DBAL_FIELD_RPRM;
            break;
        case dnx_mib_counter_nif_eth_rpfc0:
            *field_id = DBAL_FIELD_RPFC0;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff0:
            *field_id = DBAL_FIELD_RPFCOFF0;
            break;
        case dnx_mib_counter_nif_eth_rpfc1:
            *field_id = DBAL_FIELD_RPFC1;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff1:
            *field_id = DBAL_FIELD_RPFCOFF1;
            break;
        case dnx_mib_counter_nif_eth_rpfc2:
            *field_id = DBAL_FIELD_RPFC2;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff2:
            *field_id = DBAL_FIELD_RPFCOFF2;
            break;
        case dnx_mib_counter_nif_eth_rpfc3:
            *field_id = DBAL_FIELD_RPFC3;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff3:
            *field_id = DBAL_FIELD_RPFCOFF3;
            break;
        case dnx_mib_counter_nif_eth_rpfc4:
            *field_id = DBAL_FIELD_RPFC4;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff4:
            *field_id = DBAL_FIELD_RPFCOFF4;
            break;
        case dnx_mib_counter_nif_eth_rpfc5:
            *field_id = DBAL_FIELD_RPFC5;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff5:
            *field_id = DBAL_FIELD_RPFCOFF5;
            break;
        case dnx_mib_counter_nif_eth_rpfc6:
            *field_id = DBAL_FIELD_RPFC6;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff6:
            *field_id = DBAL_FIELD_RPFCOFF6;
            break;
        case dnx_mib_counter_nif_eth_rpfc7:
            *field_id = DBAL_FIELD_RPFC7;
            break;
        case dnx_mib_counter_nif_eth_rpfcoff7:
            *field_id = DBAL_FIELD_RPFCOFF7;
            break;
        case dnx_mib_counter_nif_eth_rund:
            *field_id = DBAL_FIELD_RUND;
            break;
        case dnx_mib_counter_nif_eth_rfrg:
            *field_id = DBAL_FIELD_RFRG;
            break;
        case dnx_mib_counter_nif_eth_rrpkt:
            *field_id = DBAL_FIELD_RRPKT;
            break;
        case dnx_mib_counter_nif_eth_t64:
            *field_id = DBAL_FIELD_T64;
            break;
        case dnx_mib_counter_nif_eth_t127:
            *field_id = DBAL_FIELD_T127;
            break;
        case dnx_mib_counter_nif_eth_t255:
            *field_id = DBAL_FIELD_T255;
            break;
        case dnx_mib_counter_nif_eth_t511:
            *field_id = DBAL_FIELD_T511;
            break;
        case dnx_mib_counter_nif_eth_t1023:
            *field_id = DBAL_FIELD_T1023;
            break;
        case dnx_mib_counter_nif_eth_t1518:
            *field_id = DBAL_FIELD_T1518;
            break;
        case dnx_mib_counter_nif_eth_tmgv:
            *field_id = DBAL_FIELD_TMGV;
            break;
        case dnx_mib_counter_nif_eth_t2047:
            *field_id = DBAL_FIELD_T2047;
            break;
        case dnx_mib_counter_nif_eth_t4095:
            *field_id = DBAL_FIELD_T4095;
            break;
        case dnx_mib_counter_nif_eth_t9216:
            *field_id = DBAL_FIELD_T9216;
            break;
        case dnx_mib_counter_nif_eth_t16383:
            *field_id = DBAL_FIELD_T16383;
            break;
        case dnx_mib_counter_nif_eth_tbca:
            *field_id = DBAL_FIELD_TBCA;
            break;
        case dnx_mib_counter_nif_eth_tpfc0:
            *field_id = DBAL_FIELD_TPFC0;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff0:
            *field_id = DBAL_FIELD_TPFCOFF0;
            break;
        case dnx_mib_counter_nif_eth_tpfc1:
            *field_id = DBAL_FIELD_TPFC1;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff1:
            *field_id = DBAL_FIELD_TPFCOFF1;
            break;
        case dnx_mib_counter_nif_eth_tpfc2:
            *field_id = DBAL_FIELD_TPFC2;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff2:
            *field_id = DBAL_FIELD_TPFCOFF2;
            break;
        case dnx_mib_counter_nif_eth_tpfc3:
            *field_id = DBAL_FIELD_TPFC3;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff3:
            *field_id = DBAL_FIELD_TPFCOFF3;
            break;
        case dnx_mib_counter_nif_eth_tpfc4:
            *field_id = DBAL_FIELD_TPFC4;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff4:
            *field_id = DBAL_FIELD_TPFCOFF4;
            break;
        case dnx_mib_counter_nif_eth_tpfc5:
            *field_id = DBAL_FIELD_TPFC5;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff5:
            *field_id = DBAL_FIELD_TPFCOFF5;
            break;
        case dnx_mib_counter_nif_eth_tpfc6:
            *field_id = DBAL_FIELD_TPFC6;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff6:
            *field_id = DBAL_FIELD_TPFCOFF6;
            break;
        case dnx_mib_counter_nif_eth_tpfc7:
            *field_id = DBAL_FIELD_TPFC7;
            break;
        case dnx_mib_counter_nif_eth_tpfcoff7:
            *field_id = DBAL_FIELD_TPFCOFF7;
            break;
        case dnx_mib_counter_nif_eth_tpkt:
            *field_id = DBAL_FIELD_TPKT;
            break;
        case dnx_mib_counter_nif_eth_tpok:
            *field_id = DBAL_FIELD_TPOK;
            break;
        case dnx_mib_counter_nif_eth_tuca:
            *field_id = DBAL_FIELD_TUCA;
            break;
        case dnx_mib_counter_nif_eth_tufl:
            *field_id = DBAL_FIELD_TUFL;
            break;
        case dnx_mib_counter_nif_eth_tmca:
            *field_id = DBAL_FIELD_TMCA;
            break;
        case dnx_mib_counter_nif_eth_txpf:
            *field_id = DBAL_FIELD_TXPF;
            break;
        case dnx_mib_counter_nif_eth_txpp:
            *field_id = DBAL_FIELD_TXPP;
            break;
        case dnx_mib_counter_nif_eth_txcf:
            *field_id = DBAL_FIELD_TXCF;
            break;
        case dnx_mib_counter_nif_eth_tfcs:
            *field_id = DBAL_FIELD_TFCS;
            break;
        case dnx_mib_counter_nif_eth_terr:
            *field_id = DBAL_FIELD_TERR;
            break;
        case dnx_mib_counter_nif_eth_tovr:
            *field_id = DBAL_FIELD_TOVR;
            break;
        case dnx_mib_counter_nif_eth_tjbr:
            *field_id = DBAL_FIELD_TJBR;
            break;
        case dnx_mib_counter_nif_eth_trpkt:
            *field_id = DBAL_FIELD_TRPKT;
            break;
        case dnx_mib_counter_nif_eth_tfrg:
            *field_id = DBAL_FIELD_TFRG;
            break;
        case dnx_mib_counter_nif_eth_tvln:
            *field_id = DBAL_FIELD_TVLN;
            break;
        case dnx_mib_counter_nif_eth_tdvln:
            *field_id = DBAL_FIELD_TDVLN;
            break;
        case dnx_mib_counter_nif_eth_rbyt:
            *field_id = DBAL_FIELD_RBYT;
            break;
        case dnx_mib_counter_nif_eth_rrbyt:
            *field_id = DBAL_FIELD_RRBYT;
            break;
        case dnx_mib_counter_nif_eth_tbyt:
            *field_id = DBAL_FIELD_TBYT;
            break;

        case dnx_mib_counter_nif_eth_rverify:
            *field_id = DBAL_FIELD_RVERIFY;
            break;
        case dnx_mib_counter_nif_eth_rreply:
            *field_id = DBAL_FIELD_RREPLY;
            break;
        case dnx_mib_counter_nif_eth_rmcrc:
            *field_id = DBAL_FIELD_RMCRC;
            break;
        case dnx_mib_counter_nif_eth_rtrfu:
            *field_id = DBAL_FIELD_RTRFU;
            break;
        case dnx_mib_counter_nif_eth_rschcrc:
            *field_id = DBAL_FIELD_RSCHCRC;
            break;
        case dnx_mib_counter_nif_eth_rx_eee_lpi_event:
            *field_id = DBAL_FIELD_RX_EEE_LPI_EVENT;
            break;
        case dnx_mib_counter_nif_eth_rx_eee_lpi_duration:
            *field_id = DBAL_FIELD_RX_EEE_LPI_DURATION;
            break;
        case dnx_mib_counter_nif_eth_rx_llfc_phy:
            *field_id = DBAL_FIELD_RX_LLFC_PHY;
            break;
        case dnx_mib_counter_nif_eth_rx_llfc_log:
            *field_id = DBAL_FIELD_RX_LLFC_LOG;
            break;
        case dnx_mib_counter_nif_eth_rx_llfc_crc:
            *field_id = DBAL_FIELD_RX_LLFC_CRC;
            break;
        case dnx_mib_counter_nif_eth_rx_hcfc:
            *field_id = DBAL_FIELD_RX_HCFC;
            break;
        case dnx_mib_counter_nif_eth_rx_hcfc_crc:
            *field_id = DBAL_FIELD_RX_HCFC_CRC;
            break;
        case dnx_mib_counter_nif_eth_tverify:
            *field_id = DBAL_FIELD_TVERIFY;
            break;
        case dnx_mib_counter_nif_eth_treply:
            *field_id = DBAL_FIELD_TREPLY;
            break;
        case dnx_mib_counter_nif_eth_tscl:
            *field_id = DBAL_FIELD_TSCL;
            break;
        case dnx_mib_counter_nif_eth_tmcl:
            *field_id = DBAL_FIELD_TMCL;
            break;
        case dnx_mib_counter_nif_eth_tlcl:
            *field_id = DBAL_FIELD_TLCL;
            break;
        case dnx_mib_counter_nif_eth_txcl:
            *field_id = DBAL_FIELD_TXCL;
            break;
        case dnx_mib_counter_nif_eth_tx_eee_lpi_event:
            *field_id = DBAL_FIELD_TX_EEE_LPI_EVENT;
            break;
        case dnx_mib_counter_nif_eth_tx_eee_lpi_duration:
            *field_id = DBAL_FIELD_TX_EEE_LPI_DURATION;
            break;
        case dnx_mib_counter_nif_eth_tx_llfc_log:
            *field_id = DBAL_FIELD_TX_LLFC_LOG;
            break;
        case dnx_mib_counter_nif_eth_tx_hcfc:
            *field_id = DBAL_FIELD_TX_HCFC;
            break;
        case dnx_mib_counter_nif_eth_tncl:
            *field_id = DBAL_FIELD_TNCL;
            break;
        case dnx_mib_counter_nif_eth_xthol:
            *field_id = DBAL_FIELD_XTHOL;
            break;
        case dnx_mib_counter_nif_eth_tspare0:
            *field_id = DBAL_FIELD_TSPARE0;
            break;
        case dnx_mib_counter_nif_eth_tspare1:
            *field_id = DBAL_FIELD_TSPARE1;
            break;
        case dnx_mib_counter_nif_eth_tspare2:
            *field_id = DBAL_FIELD_TSPARE2;
            break;
        case dnx_mib_counter_nif_eth_tspare3:
            *field_id = DBAL_FIELD_TSPARE3;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for port %d.", counter_id, port);
    }

    /** find out whether the field is clear on read (read only) */
    if (clear_on_read)
    {
        dbal_tables_e table_id;
        dbal_table_field_info_t field_info;

        /** get dbal table id */
        SHR_IF_ERR_EXIT(dnx_mib_nif_eth_stat_dbal_table_get(unit, port, *field_id, &table_id));

        SHR_IF_ERR_EXIT(dbal_tables_field_info_get(unit, table_id, *field_id, FALSE, 0, INST_SINGLE, &field_info));

        if (field_info.permission == DBAL_PERMISSION_READONLY)
        {
            *clear_on_read = TRUE;
        }
        else
        {
            *clear_on_read = FALSE;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function get one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] counter_id - counter id.
 * \param [out] access_field_id - the dbal field id.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_mib_nif_cdu_optimized_stat_dbal_table_field_get(
    int unit,
    int counter_id,
    dbal_fields_e * access_field_id)
{
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;
    SHR_FUNC_INIT_VARS(unit);

    counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);
    switch (counter_data->row)
    {
        case 0:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW0;
            break;
        case 1:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW1;
            break;
        case 2:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW2;
            break;
        case 3:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW3;
            break;
        case 4:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW4;
            break;
        case 5:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW5;
            break;
        case 6:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW6;
            break;
        case 7:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW7;
            break;
        case 8:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW8;
            break;
        case 9:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW9;
            break;
        case 10:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW10;
            break;
        case 11:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW11;
            break;
        case 12:
            *access_field_id = DBAL_FIELD_CDU_MIB_ROW12;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Counter type %d is not supported for cdu optimized table.", counter_id);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function get one cdmib counter
 * 
 * \param [in] unit     - unit id.
 * \param [in] counter_id - counter id.
 * \param [out] access_table_id - table id.
 * \param [out] access_field_id - field id.
 *   
 * \return
 *   shr_error_e
 */
shr_error_e
dnx_mib_nif_cdu_optimized_stat_dbal_table_info_get(
    int unit,
    int counter_id,
    dbal_tables_e * access_table_id,
    dbal_fields_e * access_field_id)
{
    int result;
    int ii;
    dbal_table_field_info_t field_info = { 0 };
    dbal_tables_e tables_id[] = { DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW0_2, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW3_5,
        DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW6_8, DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW9_11,
        DBAL_TABLE_NIF_CDU_MIB_COUNTERS_ROW12_12
    };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_mib_nif_cdu_optimized_stat_dbal_table_field_get(unit, counter_id, access_field_id));

    /** find out which CLU table */
    for (ii = 0; ii < COUNTOF(tables_id); ii++)
    {
        result =
            dbal_tables_field_info_get_no_err(unit, tables_id[ii], *access_field_id, FALSE, 0, INST_SINGLE,
                                              &field_info);
        if (result == _SHR_E_NONE)
        {
            *access_table_id = tables_id[ii];
            break;
        }
        else if (result == _SHR_E_NOT_FOUND)
        {
            continue;
        }
        else
        {
            SHR_IF_ERR_EXIT(result);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_nif_cdu_optimized_stat_dbal_get(
    int unit,
    bcm_port_t port,
    int counter_id,
    uint64 *val)
{
    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    uint32 ctr_low, ctr_hi;
    int ctrs_db_offset, nof_row_per_cdu_lane;
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    pm_idx = first_phy / nof_phys_per_cdu;
    lane_idx = first_phy % nof_phys_per_cdu;

    counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);
    /*
     * Cacluate the offset in DB
     */
    nof_row_per_cdu_lane = dnx_data_mib.general.nof_row_per_cdu_lane_get(unit);
    ctrs_db_offset = first_phy * nof_row_per_cdu_lane + counter_data->row;

    /*
     * Get counter value from HW
     */
    if (counter_data->offset_in_row == 0)
    {
        SHR_IF_ERR_EXIT(dnx_mib_nif_cdu_optimized_stat_dbal_table_info_get(unit, counter_id, &table_id, &field_id));

        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);
         /** request the data */
        dbal_value_field_arr32_request(unit, entry_handle_id, field_id, INST_SINGLE,
                                       ctrs_saved_row[unit][ctrs_db_offset]);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Get counter value from SW DB that stored the request values
     */
    SOC_REG_ABOVE_64_WORD_GET(ctrs_saved_row[unit][ctrs_db_offset], &ctr_low, (2 * counter_data->offset_in_row));
    SOC_REG_ABOVE_64_WORD_GET(ctrs_saved_row[unit][ctrs_db_offset], &ctr_hi, (2 * counter_data->offset_in_row + 1));
    COMPILER_64_SET(*val, ctr_hi, ctr_low);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_mib_nif_cdu_optimized_stat_dbal_clear(
    int unit,
    bcm_port_t port,
    int counter_id)
{
    uint32 entry_handle_id;
    uint32 nof_phys_per_cdu, pm_idx, lane_idx;
    int first_phy;
    dbal_tables_e table_id;
    dbal_fields_e field_id;
    int ctrs_db_offset, nof_row_per_cdu_lane;
    const dnx_data_mib_controlled_counters_counter_data_t *counter_data;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &first_phy));

    nof_phys_per_cdu = dnx_data_nif.eth.nof_lanes_in_cdu_get(unit);
    pm_idx = first_phy / nof_phys_per_cdu;
    lane_idx = first_phy % nof_phys_per_cdu;

    counter_data = dnx_data_mib.controlled_counters.counter_data_get(unit, counter_id);
    /*
     * Cacluate the offset in DB
     */
    nof_row_per_cdu_lane = dnx_data_mib.general.nof_row_per_cdu_lane_get(unit);
    ctrs_db_offset = first_phy * nof_row_per_cdu_lane + counter_data->row;

    /*
     * Set counter value into HW
     */
    if (counter_data->offset_in_row == 0)
    {
        SOC_REG_ABOVE_64_CLEAR(ctrs_saved_row[unit][ctrs_db_offset]);
        SHR_IF_ERR_EXIT(dnx_mib_nif_cdu_optimized_stat_dbal_table_info_get(unit, counter_id, &table_id, &field_id));

        /** alloc DBAL table handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
        /** set key fields */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INDEX, pm_idx);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_LANE, lane_idx);
         /** request the data */
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, field_id, INST_SINGLE,
                                         ctrs_saved_row[unit][ctrs_db_offset]);
        /** Performing the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
