/** \file imb_framer_internal.c
 *
 *  FEU internal logicals, including
 *      NB interface
 *      SB interface
 *      Tiny MAC
 *      Rx/Tx Buffers
 *
 */
/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#ifdef _SOC_MEMREGS_H
#undef _SOC_MEMREGS_H
#endif

#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/portmod/portmod.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/mcm/memregs.h>
#include <soc/register.h>

#include "imb_framer_internal.h"
#include <soc/dnx/dnx_err_recovery_manager.h>

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

extern int soc_reg32_set(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 data);

extern int soc_reg32_get(
    int unit,
    soc_reg_t reg,
    int port,
    int index,
    uint32 *data);
/*
 * If the calendar supports "switch_en"
 */
#define IMB_FRAMER_CAL_SWITCH_EN_SUPPORT(unit, cal_type)                                                       \
    ((cal_type == IMB_FRAMER_SB_RX_CALENDAR) ||                                                                \
     ((cal_type == IMB_FRAMER_NB_RX_CALENDAR || cal_type == IMB_FRAMER_NB_TX_CALENDAR) &&                      \
       dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_cal_switch_enable_support)) ||               \
      ((cal_type == IMB_FRAMER_SB_TX_CALENDAR) &&                                                              \
        dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_cal_switch_en_supported)))
/**
 * \brief - Reset the FlexE core.
 * see .h file
 */
int
imb_framer_internal_flexe_core_reset(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_CORE_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_RESET, INST_SINGLE, in_reset);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Reset the FlexE Tiny MAC.
 * see .h file
 */
int
imb_framer_internal_tiny_mac_reset(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_TINY_MAC_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAC_RESET, INST_SINGLE,
                                 in_reset ? DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Define the TX delay in Northbound interface.
 * see .h file
 */
int
imb_framer_internal_nb_tx_delay_set(
    int unit,
    int tx_delay)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_TX_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_DELAY, INST_SINGLE, tx_delay);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Define the TX delay in Northbound interface.
 * see .h file
 */
int
imb_framer_internal_nb_tx_delay_per_pm_set(
    int unit,
    int pm_id,
    int tx_delay)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_TX_PM_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_id);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_DELAY, INST_SINGLE, tx_delay);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Define the TX fifo threshold for first read.
 * see .h file
 */
int
imb_framer_internal_nb_tx_fifo_threshold_set(
    int unit,
    int tx_threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_TX_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_FIFO_THRESHOLD, INST_SINGLE, tx_threshold);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable the clock from TSC
 * see .h file
 */
int
imb_framer_internal_clock_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_CLOCK_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_ENABLE, INST_SINGLE, enable);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Select the clock source from TSC
 * see .h file
 */
int
imb_framer_internal_clock_source_set(
    int unit,
    int clock_source)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_CLOCK_CTRL, &entry_handle_id));

    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLOCK_SEL, INST_SINGLE, clock_source);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE core to PCS port mapping.
 *
 * see .h file
 */
int
imb_framer_internal_flexe_core_to_pcs_port_map_set(
    int unit,
    int flexe_core_port,
    int pm_id,
    int pcs_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_FLEXE_CORE_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEXE_CORE_PORT, flexe_core_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, INST_SINGLE, pm_id);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, INST_SINGLE, pcs_port);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure PCS port to flexe core port mapping.
 *
 * see .h file
 */
int
imb_framer_internal_pcs_to_flexe_core_port_map_set(
    int unit,
    int pm_id,
    int pcs_port,
    int flexe_core_port)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEXE_CORE_PORT, INST_SINGLE, flexe_core_port);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure PCS port fifo size.
 *    for speed <= 50G, the fifo size is 0
 *    for speed = 100G, the fifo size is 1
 *    for speed = 200G, the fifo size is 2
 *    for speed = 400G, the fifo size is 3
 *
 * see .h file
 */
int
imb_framer_internal_pcs_port_fifo_size_set(
    int unit,
    int pm_id,
    int pcs_port,
    int fifo_size)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_SIZE, INST_SINGLE, fifo_size);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable PCS port per PM.
 *
 * see .h file
 */
int
imb_framer_internal_pcs_port_enable_set(
    int unit,
    int pm_id,
    int pcs_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable the FlexE core port.
 *
 * see .h file
 */
int
imb_framer_internal_flexe_core_port_enable_set(
    int unit,
    int flexe_core_port,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_FLEXE_CORE_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEXE_CORE_PORT, flexe_core_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable Tx data to PM.
 * see .h file
 */
int
imb_feu_internal_tx_data_to_pm_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_DATA, INST_SINGLE, enable ? 0 : 1);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the logical FIFO threshold after overflow.
 * after the fifo reaches overflow, it will not resume writing
 * until fifo level will get below this value.
 *
 * see .h file
 */
int
imb_feu_internal_rmc_thr_after_ovf_set(
    int unit,
    int rmc_id,
    int thr_after_ovf)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THR_AFTER_OVF, INST_SINGLE, thr_after_ovf);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set TX start threshold. this threshold represent the
 *        number of 64B words to be accumulated in the MLF
 *        before transmitting towards the PM. This is to prevent
 *        TX MAC starvation and is important for systems with
 *        oversubscription.
 *
 *  see .h file
 */
int
imb_feu_internal_tx_start_thr_set(
    int unit,
    int client_channel,
    int start_thr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_THR, INST_SINGLE, start_thr);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get start tx threshold
 *
 *  see .h file
 */
int
imb_feu_internal_tx_start_thr_get(
    int unit,
    int client_channel,
    int *start_thr)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_START_TX_THR, INST_SINGLE, (uint32 *) start_thr);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FEU fifo link list controller
 *
 *  see .h file
 */
static int
imb_framer_internal_fifo_link_list_ctrl_set(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int fifo_index,
    int first_entry,
    int nof_entries,
    int enable)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;
    int config_mem_units = TRUE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ALGO_FLEXE_FIFO_SB_RX:
            table_id = DBAL_TABLE_NIF_FRAMER_SB_RX_CLIENT_CTRL;
            break;
        case DNX_ALGO_FLEXE_FIFO_SB_TX:
            table_id = DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL;
            break;
        case DNX_ALGO_FLEXE_FIFO_TMC:
            table_id = DBAL_TABLE_NIF_FEU_TX_TMC_CTRL;
            break;
        case DNX_ALGO_FLEXE_FIFO_RMC:
            table_id = DBAL_TABLE_NIF_FEU_RX_RMC_CTRL;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fifo type %d.\n", type);
            break;
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set key fields
     */
    if (type == DNX_ALGO_FLEXE_FIFO_RMC)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, fifo_index);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, fifo_index);
    }
    /*
     * set value field
     */
    if ((type == DNX_ALGO_FLEXE_FIFO_SB_RX) || (type == DNX_ALGO_FLEXE_FIFO_SB_TX))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FIFO_ENABLE, INST_SINGLE, enable);
        /*
         * Check if mem units size configurable
         */
        if (!dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_fifo_size_configurable))
        {
            config_mem_units = FALSE;
        }
        else
        {
            if (type == DNX_ALGO_FLEXE_FIFO_SB_RX)
            {
                nof_entries = nof_entries * dnx_data_nif.mac_client.nof_units_per_sb_rx_fifo_get(unit);
            }
            else
            {
                nof_entries = nof_entries * dnx_data_nif.mac_client.nof_units_per_sb_tx_fifo_get(unit);
            }
        }
    }
    if (config_mem_units)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEM_UNITS, INST_SINGLE,
                                     enable ? nof_entries : 0);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_MEM_UNIT, INST_SINGLE, first_entry);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FEU fifo link list
 *
 *  see .h file
 */
static int
imb_framer_internal_fifo_link_list_config(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int entry_idx,
    int next_entry_indx,
    int enable)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case DNX_ALGO_FLEXE_FIFO_SB_RX:
            table_id = DBAL_TABLE_NIF_FRAMER_SB_RX_FIFO_LINK_LIST;
            break;
        case DNX_ALGO_FLEXE_FIFO_SB_TX:
            table_id = DBAL_TABLE_NIF_FRAMER_SB_TX_FIFO_LINK_LIST;
            break;
        case DNX_ALGO_FLEXE_FIFO_TMC:
            table_id = DBAL_TABLE_NIF_FEU_TX_TMC_MEM_LINK_LIST;
            break;
        case DNX_ALGO_FLEXE_FIFO_RMC:
            table_id = DBAL_TABLE_NIF_FEU_RX_RMC_MEM_LINK_LIST;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported fifo type %d.\n", type);
            break;
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CURR_MEM_UNIT, entry_idx);
    /*
     * set value field
     */
    if (!enable)
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id,
                                                   DBAL_FIELD_NEXT_MEM_UNIT, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NEXT_MEM_UNIT, INST_SINGLE, next_entry_indx);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the Busb link list fifo
 *
 * \param [in] unit - chip unit id
 * \param [in] type - Link list fifo type
 * \param [in] fifo_index - RMC port or FlexE client port
 * \param [in] fifo_entries - FIFO entries that needs to be configured
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_framer_internal_fifo_link_list_enable_set(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type,
    int fifo_index,
    bcm_pbmp_t * fifo_entries,
    int enable)
{
    int first_entry, last_entry, pre_entry = -1, entry_idx = -1;
    int is_first_entry = TRUE, nof_entries;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_FIRST(*fifo_entries, first_entry);
    _SHR_PBMP_COUNT(*fifo_entries, nof_entries);
    /*
     * Configue the link list controller
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_ctrl_set
                    (unit, type, fifo_index, first_entry, nof_entries, enable));

    /*
     * Only need to configure the link list if enable
     */
    _SHR_PBMP_ITER(*fifo_entries, entry_idx)
    {
        if (is_first_entry)
        {
            is_first_entry = FALSE;
        }
        else
        {
            /*
             * Configue the link list
             */
            SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_config(unit, type, pre_entry, entry_idx, enable));
        }
        pre_entry = entry_idx;
    }
    /*
     * The last entry should point to the first one
     */
    _SHR_PBMP_LAST(*fifo_entries, last_entry);
    SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_config(unit, type, last_entry, first_entry, enable));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Force the SB fifo configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] type - Link list fifo type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_framer_internal_sb_fifo_force_switch_set(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type)
{
    dbal_tables_e table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    table_id =
        (type == DNX_ALGO_FLEXE_FIFO_SB_RX) ? DBAL_TABLE_NIF_FRAMER_SB_RX_CTRL : DBAL_TABLE_NIF_FRAMER_SB_TX_CTRL;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * Write it to 1 enable forcing
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_LINK_LIST_UPDATE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Write it back to 0
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_LINK_LIST_UPDATE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Switch the SB Link list fifo for G.hao
 *
 * \param [in] unit - chip unit id
 * \param [in] type - Link list fifo type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_framer_internal_sb_fifo_ghao_switch_set(
    int unit,
    dnx_algo_flexe_link_list_fifo_type_e type)
{
    dbal_tables_e ctrl_table_id, status_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    ctrl_table_id =
        (type == DNX_ALGO_FLEXE_FIFO_SB_RX) ? DBAL_TABLE_NIF_FRAMER_SB_RX_CTRL : DBAL_TABLE_NIF_FRAMER_SB_TX_CTRL;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ctrl_table_id, &entry_handle_id));
    /*
     * Write it to 1 enable Ghao Link list switching
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GHAO_LINK_LIST_UPDATE, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Polling the switch done bit
     */
    status_table_id =
        (type ==
         DNX_ALGO_FLEXE_FIFO_SB_RX) ? DBAL_TABLE_NIF_FRAMER_SB_RX_FIFO_LINK_LIST_STATUS :
        DBAL_TABLE_NIF_FRAMER_SB_TX_FIFO_LINK_LIST_STATUS;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, status_table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, DBAL_FIELD_IS_SWITCH_DONE, 1));
    /*
     * Write it back to 0
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, ctrl_table_id, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GHAO_LINK_LIST_UPDATE, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set BusB client link list G.hao configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - FlexE client channel
 * \param [in] fifo_type - the link list fifo type
 * \param [in] delta_val - the delta value between new and old entries
 * \param [in] flags - flags for G.hao actions
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_framer_internal_port_fifo_link_list_ghao_config_set(
    int unit,
    int client_channel,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int delta_val,
    uint32 flags)
{
    bcm_pbmp_t fifo_entries, updated_fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FLEXE_RESOURCE_PREPARE_GET(flags))
    {
        /*
         * Update fifo link list
         */
        if (delta_val > 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_alloc
                            (unit, client_channel, delta_val, fifo_type, &updated_fifo_entries));
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_free
                            (unit, client_channel, utilex_abs(delta_val), fifo_type, &updated_fifo_entries));
        }
    }
    if (IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
    {
        /*
         * Configure the Link List into HW
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get(unit, client_channel, fifo_type, &fifo_entries));
        SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_enable_set
                        (unit, fifo_type, client_channel, &fifo_entries, 1));
        /*
         * Start switching
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_sb_fifo_ghao_switch_set(unit, fifo_type));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set BusB client link list force configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - FlexE client channel
 * \param [in] rmc_id - RMC ID
 * \param [in] speed - port speed
 * \param [in] fifo_type - the link list fifo type
 * \param [in] enable - enable or disable the config
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_framer_internal_port_fifo_link_list_force_config_set(
    int unit,
    int client_channel,
    int rmc_id,
    int speed,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int enable)
{
    int fifo_index, nof_entries;
    bcm_pbmp_t fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    if (fifo_type != DNX_ALGO_FLEXE_FIFO_RMC)
    {
        fifo_index = client_channel;
    }
    else
    {
        fifo_index = rmc_id;
    }

    if (enable)
    {
        /*
         * Set SB Rx in reset before configuring the FIFO
         * This is only required when special client OAM client 80 is supported.
         * The special client 80 is used to trigger the SB calendar when adding BusB client, so
         * when adding the first busb client, need to enable client 80 internally.
         */
        if ((fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) && (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0))
        {
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
        }
        /*
         * Allocate fifo link list
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_entries_required_get(unit, fifo_type, speed, &nof_entries));
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_alloc(unit, fifo_index, nof_entries, fifo_type, &fifo_entries));
        /*
         * Configure the Link List into HW
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_enable_set(unit, fifo_type, fifo_index, &fifo_entries, 1));
        /*
         * Take SB Rx out of reset after configuring the fifo
         */
        if ((fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) && (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0))
        {
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        }
    }
    else
    {
        /*
         * Set SB Rx in reset before disabling the FIFO
         */
        if ((fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) && (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0))
        {
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
        }
        /*
         * Free fifo link list in DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_free(unit, fifo_index, -1, fifo_type, &fifo_entries));

        if (!_SHR_PBMP_IS_NULL(fifo_entries))
        {
            /*
             * Disable link list
             */
            SHR_IF_ERR_EXIT(imb_framer_internal_fifo_link_list_enable_set
                            (unit, fifo_type, fifo_index, &fifo_entries, 0));
        }
    }
    if (((fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) || (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_TX)) &&
        dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_sb_fifo_force_switch_set(unit, fifo_type));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the FEU calendar DBAL table name according to
 *    the calendar type
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_type - calendar type
 * \param [out] ctrl_table - the calendar control table
 * \param [out] status_table - the calendar status table
 * \param [out] cal_table - the calendar slots table
 * \param [out] cal_field - the calendar field
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_framer_internal_calendar_table_get(
    int unit,
    imb_framer_calendar_type_e cal_type,
    dbal_tables_e * ctrl_table,
    dbal_tables_e * status_table,
    dbal_tables_e * cal_table,
    dbal_fields_e * cal_field)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (cal_type)
    {
        case IMB_FRAMER_NB_RX_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FRAMER_NB_RX_TDM_PATTERN_CTRL;
            *cal_table = DBAL_TABLE_NIF_FRAMER_NB_RX_TDM_PATTERN;
            *status_table = DBAL_TABLE_NIF_FRAMER_NB_RX_CALENDAR_STATUS;
            *cal_field = DBAL_FIELD_FLEXE_CORE_PORT;
            break;
        case IMB_FRAMER_NB_TX_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FRAMER_NB_TX_TDM_PATTERN_CTRL;
            *cal_table = DBAL_TABLE_NIF_FRAMER_NB_TX_TDM_PATTERN;
            *status_table = DBAL_TABLE_NIF_FRAMER_NB_TX_CALENDAR_STATUS;
            *cal_field = DBAL_FIELD_FLEXE_CORE_PORT;
            break;
        case IMB_FRAMER_SB_RX_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FRAMER_SB_RX_CALENDAR_CTRL;
            *status_table = DBAL_TABLE_NIF_FRAMER_SB_RX_CALENDAR_STATUS;
            *cal_table = DBAL_TABLE_NIF_FRAMER_SB_RX_CALENDAR;
            *cal_field = DBAL_FIELD_CLIENT_CHANNEL;
            break;
        case IMB_FRAMER_SB_TX_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FRAMER_SB_TX_CREDIT_CALENDAR_CTRL;
            *status_table = DBAL_TABLE_NIF_FRAMER_SB_TX_CREDIT_CALENDAR_STATUS;
            *cal_table = DBAL_TABLE_NIF_FRAMER_SB_TX_CREDIT_CALENDAR;
            *cal_field = DBAL_FIELD_CLIENT_CHANNEL;
            break;
        case IMB_FEU_RMC_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FEU_RMC_CALENDAR_CTRL;
            *cal_table = DBAL_TABLE_NIF_FEU_RMC_CALENDAR;
            *cal_field = DBAL_FIELD_RMC_ID;
            break;
        case IMB_FEU_HI_OR_ESB_RMC_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FEU_RMC_CALENDAR_CTRL;
            *cal_table = DBAL_TABLE_NIF_FEU_RMC_CALENDAR;
            *cal_field = DBAL_FIELD_HI_OR_ESB_RMC_ID;
            break;
        case IMB_FEU_TMC_CALENDAR:
            *ctrl_table = DBAL_TABLE_NIF_FEU_TMC_CALENDAR_CTRL;
            *cal_table = DBAL_TABLE_NIF_FEU_TMC_CALENDAR;
            *cal_field = DBAL_FIELD_CLIENT_CHANNEL;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported calendar type.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the current active calendar ID
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_type - calendar type
 * \param [out] active_cal - active calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_framer_internal_active_calendar_get(
    int unit,
    imb_framer_calendar_type_e cal_type,
    uint32 *active_cal)
{
    dbal_tables_e calendar_table, ctrl_table, status_table;
    dbal_fields_e calendar_field;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_table_get
                    (unit, cal_type, &ctrl_table, &status_table, &calendar_table, &calendar_field));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ctrl_table, &entry_handle_id));
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ACTIVE_CAL, INST_SINGLE, active_cal);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the current active calendar ID
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_type - calendar type
 * \param [in] active_cal - active calendar ID
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_framer_internal_active_calendar_set(
    int unit,
    imb_framer_calendar_type_e cal_type,
    uint32 active_cal)
{
    dbal_tables_e calendar_table, ctrl_table, status_table;
    dbal_fields_e calendar_field;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_table_get
                    (unit, cal_type, &ctrl_table, &status_table, &calendar_table, &calendar_field));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, ctrl_table, &entry_handle_id));

    /*
     * Configure field
     */
    if (IMB_FRAMER_CAL_SWITCH_EN_SUPPORT(unit, cal_type))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_SWITCH_ENABLE, INST_SINGLE, 1);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ACTIVE_CAL, INST_SINGLE, active_cal);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Polling working cal for G.hao calendar
     */
    if (IMB_FRAMER_CAL_SWITCH_EN_SUPPORT(unit, cal_type) &&
        dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_g_hao_supported))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, status_table, entry_handle_id));
        SHR_IF_ERR_EXIT(dnxcmn_polling(unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id,
                                       DBAL_FIELD_WORKING_CAL, active_cal));
        /*
         * Disable switch enable
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, ctrl_table, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_SWITCH_ENABLE, INST_SINGLE, 0);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the calendar slots
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_type - calendar type
 * \param [in] cal_id - calendar ID to configure
 * \param [in] cal_len - calendar length
 * \param [in] calendar - calendar slots info
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
imb_framer_internal_calendar_slots_config(
    int unit,
    imb_framer_calendar_type_e cal_type,
    int cal_id,
    int cal_len,
    uint32 *calendar)
{
    int i, enable;
    dbal_tables_e calendar_table, ctrl_table, status_table;
    dbal_fields_e calendar_field;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_table_get
                    (unit, cal_type, &ctrl_table, &status_table, &calendar_table, &calendar_field));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, calendar_table, &entry_handle_id));
    /*
     * set calendar ID
     */
    if ((cal_type != IMB_FEU_RMC_CALENDAR) && (cal_type != IMB_FEU_HI_OR_ESB_RMC_CALENDAR))
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_INDEX, cal_id);
    }
    for (i = 0; i < cal_len; i++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, i);
        /*
         * set value field
         */
        if (calendar[i] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
        {
            dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, calendar_field,
                                                       INST_SINGLE, DBAL_PREDEF_VAL_MAX_VALUE);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, calendar_field, INST_SINGLE, calendar[i]);
        }

        if ((cal_type == IMB_FRAMER_NB_RX_CALENDAR)
            && dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_nb_cal_slot_enable_support))
        {
            enable = (calendar[i] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID) ? 0 : 1;
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
        }
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
imb_framer_internal_calendar_bubble_get(
    int unit,
    imb_framer_calendar_type_e calendar_type,
    uint32 *bubble_val)
{
    int nof_mask_bits;
    SHR_FUNC_INIT_VARS(unit);

    switch (calendar_type)
    {
        case IMB_FRAMER_NB_RX_CALENDAR:
            nof_mask_bits = dnx_data_nif.flexe.nb_cal_slot_nof_bits_get(unit);
            break;

        case IMB_FRAMER_SB_RX_CALENDAR:
        case IMB_FRAMER_SB_TX_CALENDAR:
            nof_mask_bits = utilex_log2_round_up(dnx_data_nif.mac_client.nof_clients_get(unit));
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar type %d is not supported.", calendar_type);
    }

    *bubble_val = ((1 << nof_mask_bits) - 1);
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Read the shared arch calendar slots
 *
 * \param [in] unit - chip unit id
 * \param [in] calendar_type - calendar type
 * \param [in] calendar_length - calendar length
 * \param [out] calendar - calendar slots info
 * \param [out] nof_slots_per_client - nof calendar slots per port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
imb_framer_internal_active_shared_arch_calendar_get(
    int unit,
    imb_framer_calendar_type_e calendar_type,
    uint32 calendar_length,
    uint32 *nof_slots_per_client,
    uint32 *calendar)
{
    uint32 bubble_val;
    dbal_fields_e calendar_field;
    dbal_tables_e calendar_table, ctrl_table, status_table;
    uint32 entry_handle_id, client_val, active_calendar_id, calendar_index;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Currently supported only for SB Tx/Rx calendars */
    if (!dnx_data_nif.global.feature_get(unit, dnx_data_nif_global_is_shared_arch))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_bubble_get(unit, calendar_type, &bubble_val));

    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_table_get
                    (unit, calendar_type, &ctrl_table, &status_table, &calendar_table, &calendar_field));

    SHR_IF_ERR_EXIT(imb_framer_internal_active_calendar_get(unit, calendar_type, &active_calendar_id));

    /** alloc DBAL table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, calendar_table, &entry_handle_id));

    /** set calendar ID */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_INDEX, active_calendar_id);

    for (calendar_index = 0; calendar_index < calendar_length; ++calendar_index)
    {

        /** set value field */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, calendar_index);
        dbal_value_field32_request(unit, entry_handle_id, calendar_field, INST_SINGLE, &client_val);

        /** commit value */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        /** Converting bubble value if necessary or counting slots per client */
        if (client_val != bubble_val)
        {
            calendar[calendar_index] = client_val;
            ++nof_slots_per_client[client_val];
        }
        else
        {
            calendar[calendar_index] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable RMC calendar sampling
 *
 * \param [in] unit - chip unit id
 * \param [in] enable - Enable or disable
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_feu_internal_rmc_calender_sampling_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RMC_CALENDAR_CTRL, &entry_handle_id));
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAMPLE_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the sb rx slot info
 *
 * \param [in] unit - chip unit id
 * \param [in] cal_idx - calendar index
 * \param [in] slot_idx - slot index
 * \param [out] val - calendar value
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_framer_internal_sb_rx_cal_slot_get(
    int unit,
    int cal_idx,
    int slot_idx,
    uint32 *val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CALENDAR, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_INDEX, cal_idx);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, slot_idx);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, INST_SINGLE, val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Decide which calendar in SB rx should be configured
 *
 * \param [in] unit - chip unit id
 * \param [out] cal_to_config - which calendar should be configured
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
imb_framer_internal_sb_rx_cal_to_config_get(
    int unit,
    int *cal_to_config)
{
    uint32 active_cal, val, default_val;
    int slot_idx, cal_is_null = TRUE;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CALENDAR, DBAL_FIELD_CLIENT_CHANNEL, 0, 0, 0, &field_info));
    default_val = (1 << field_info.field_nof_bits) - 1;

    SHR_IF_ERR_EXIT(imb_framer_internal_active_calendar_get(unit, IMB_FRAMER_SB_RX_CALENDAR, &active_cal));

    for (slot_idx = 0; slot_idx < dnx_data_nif.mac_client.nof_clients_get(unit); ++slot_idx)
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_sb_rx_cal_slot_get(unit, active_cal, slot_idx, &val));
        if (val != default_val)
        {
            cal_is_null = FALSE;
            break;
        }
    }
    if (cal_is_null)
    {
        /*
         * If the working calendar is NULL, use the current calendar
         */
        *cal_to_config = active_cal;
    }
    else
    {
        /*
         * If the working calendar is not NULL, use the next calendar
         */
        *cal_to_config = (active_cal == 0) ? 1 : 0;
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Common function for get the shadow calendar ID:
 *        Northbound Rx calendar
 *        Southbound Rx/Tx calendar
 *        RMC/TMC calendar
 *
 */
static shr_error_e
imb_framer_internal_calendar_to_config_get(
    int unit,
    imb_framer_calendar_type_e cal_type,
    int *cal_to_config)
{
    uint32 active_cal = 0;

    SHR_FUNC_INIT_VARS(unit);

    if ((cal_type == IMB_FRAMER_SB_RX_CALENDAR) && (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0))
    {
        /*
         * 1. For SB rx, if the previous calendar is a NULL calendar, we cannot
         *    switch the calendar.
         * 2. SB rx calendar needs OAM client to active this calendar.
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_sb_rx_cal_to_config_get(unit, cal_to_config));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_active_calendar_get(unit, cal_type, &active_cal));
        *cal_to_config = (active_cal == 0) ? 1 : 0;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Common function for configuring FlexE
 *    shadow calendar, including:
 *        Northbound Rx calendar
 *        Southbound Rx/Tx calendar
 *        RMC/TMC calendar
 *
 */
shr_error_e
imb_framer_internal_shadow_calendar_config(
    int unit,
    imb_framer_calendar_type_e cal_type,
    int cal_len,
    uint32 *calendar,
    int *cal_to_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if ((cal_type == IMB_FEU_RMC_CALENDAR) || (cal_type == IMB_FEU_HI_OR_ESB_RMC_CALENDAR))
    {
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_calender_sampling_enable_set(unit, 0));
        *cal_to_config = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_to_config_get(unit, cal_type, cal_to_config));
    }
    SHR_IF_ERR_EXIT(imb_framer_internal_calendar_slots_config(unit, cal_type, *cal_to_config, cal_len, calendar));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common function for activating FlexE
 *    calendar, including:
 *        Northbound Rx calendar
 *        Southbound Rx/Tx calendar
 *        RMC/TMC calendar
 *
 */
shr_error_e
imb_framer_internal_calendar_switch(
    int unit,
    imb_framer_calendar_type_e cal_type,
    int cal_to_switch)
{
    int standby_cal_id;

    SHR_FUNC_INIT_VARS(unit);

    if ((cal_type == IMB_FEU_RMC_CALENDAR) || (cal_type == IMB_FEU_HI_OR_ESB_RMC_CALENDAR))
    {
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_calender_sampling_enable_set(unit, 1));
    }
    else
    {
        /*
         * If the calendar switching is not for G.hao, the standby_cal_id
         * is returned when configuring standby calendar.
         * For some special cases, the cal_to_switch relies on the calendar ID
         * returned when configruing standby calendar.
         */
        if (cal_to_switch == -1)
        {
            SHR_IF_ERR_EXIT(imb_framer_internal_calendar_to_config_get(unit, cal_type, &standby_cal_id));
        }
        else
        {
            standby_cal_id = cal_to_switch;
        }
        SHR_IF_ERR_EXIT(imb_framer_internal_active_calendar_set(unit, cal_type, standby_cal_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the original number of slots for give FlexE
 *    core port in NB RX calendar
 *
 */
shr_error_e
imb_framer_internal_nb_cal_original_tdm_slots_nof_get(
    int unit,
    int flexe_core_port,
    int *nof_slots)
{
    int i, cal_len;
    uint32 active_cal, entry_handle_id, slot_enable, cal_slot;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_internal_active_calendar_get(unit, IMB_FRAMER_NB_RX_CALENDAR, &active_cal));

    *nof_slots = 0;
    cal_len = dnx_data_nif.flexe.nof_pcs_get(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_RX_TDM_PATTERN, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAL_INDEX, active_cal);

    for (i = 0; i < cal_len; i++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, i);
        /*
         * request for value field
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, &slot_enable);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLEXE_CORE_PORT, INST_SINGLE, &cal_slot);
        /*
         * commit value
         */
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

        if ((slot_enable == 1) && (cal_slot == flexe_core_port))
        {
            ++(*nof_slots);
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable SB Tx credit counter table
 *
 * see .h file
 */
shr_error_e
imb_framer_internal_tx_credit_counter_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CREDIT_COUNTER_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the RMC current FIFO level
 *
 * \param [in] unit - chip unit id
 * \param [in] rmc_id - rmc id to check fifo level
 * \param [out] fifo_level - RMC FIFO fullnes level
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_internal_rmc_level_get(
    int unit,
    uint32 rmc_id,
    uint32 *fifo_level)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FIFO_LEVEL, INST_SINGLE, fifo_level);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port in the FEU.
 *
 * see .h file
 */
int
imb_feu_internal_port_rx_reset_set(
    int unit,
    bcm_port_t port,
    int rmc_id,
    int in_reset)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 entry_handle_id;
    int ii, nof_rmcs_to_reset, rmc_id_to_reset, nof_priority_groups;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.mac_client.priority_groups_nof_get(unit);
    nof_rmcs_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? nof_priority_groups : 1;
    /*
     * Reset the port in RX - port is represented by logical FIFOs
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));

    for (ii = 0; ii < nof_rmcs_to_reset; ++ii)
    {
        /*
         * get logical fifo info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, ii, &rmc));
        rmc_id_to_reset = (rmc_id == IMB_COSQ_ALL_RMCS) ? rmc.rmc_id : rmc_id;
        if (rmc_id_to_reset == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id_to_reset);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                     in_reset ?
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                     DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Covert the NIF scheduler priority to RMC traffic type
 *
 * \param [in] unit - chip unit id
 * \param [in] sch_priority - NIF scheduler priorty
 * \param [out] traffic_type - RMC traffic type
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_internal_sch_priority_to_traffic_type_get(
    int unit,
    bcm_port_nif_scheduler_t sch_priority,
    dbal_enum_value_field_feu_rmc_traffic_type_e * traffic_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (sch_priority)
    {
        case bcmPortNifSchedulerLow:
        {
            *traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_IRE_LP;
            break;
        }
        case bcmPortNifSchedulerHigh:
        {
            *traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_IRE_HP;
            break;
        }
        case bcmPortNifSchedulerTDM:
        {
            *traffic_type = DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ESB;
            break;
        }
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported NIF scheduler priority\n");
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get RMC traffic type
 *
 * \param [in] unit - chip unit id
 * \param [in] rmc_id - RMC ID
 * \param [out] traffic_type - RMC traffic type
 *
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_internal_rmc_traffic_type_get(
    int unit,
    int rmc_id,
    dbal_enum_value_field_feu_rmc_traffic_type_e * traffic_type)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TRAFFIC_TYPE, INST_SINGLE, traffic_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See imb_feu_internal.h
 */
int
imb_feu_internal_rmc_to_client_map_set(
    int unit,
    int client_channel,
    dnx_algo_port_rmc_info_t * rmc,
    int rmc_id_is_changed,
    int enable)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_feu_rmc_traffic_type_e traffic_type, ori_traffic_type;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_internal_rmc_traffic_type_get(unit, rmc->rmc_id, &ori_traffic_type));
    /*
     * For ILU LP and HP, the RMC traffic type is decided and cleared by
     * bcm_port_flexe_flow_set/clear API.
     */
    if ((ori_traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_LP)
        || (ori_traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ILU_HP))
    {
        SHR_EXIT();
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc->rmc_id);
    /*
     * set value fields
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_sch_priority_to_traffic_type_get(unit, rmc->sch_priority, &traffic_type));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRAFFIC_TYPE, INST_SINGLE,
                                 enable ? traffic_type : 0);

    if (rmc_id_is_changed)
    {
        if (traffic_type == DBAL_ENUM_FVAL_FEU_RMC_TRAFFIC_TYPE_ESB)
        {
            /*
             * If the traffic is to ESB, need to configure the destination port to
             * an invalid value, the flow set API will configure the dest port
             */
            SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                            (unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, DBAL_FIELD_PORT_OR_CHANNEL, 0, 0, 0, &field_info));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OR_CHANNEL, INST_SINGLE,
                                         enable ? ((1 << field_info.field_nof_bits) - 1) : 0);
        }
        else
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_OR_CHANNEL, INST_SINGLE,
                                         enable ? client_channel : 0);
        }
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See imb_feu_internal.h
 */
int
imb_feu_internal_client_to_rmc_map(
    int unit,
    int client_channel,
    dnx_algo_port_rmc_info_t * rmc,
    int enable)
{
    uint32 entry_handle_id, def_val;
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_NIF_FEU_RX_PRIORITY_TO_RMC_MAP, DBAL_FIELD_RMC, 0, 0, 0, &field_info));
    def_val = (1 << field_info.field_nof_bits) - 1;
    /*
     * map RMC to lane + PRD priority 
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_PRIORITY_TO_RMC_MAP, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * RMC can be mapped to more than one PRD priority of the client(even to all priorities), so each priority is
     * checked independently
     * In PRD bypass mode, recommend to mapping all the priority to one RMC, even though we override
     * the PRD priority for each client.
     */
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_0)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 0);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE,
                                     enable ? rmc->rmc_id : def_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_1)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE,
                                     enable ? rmc->rmc_id : def_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_2)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 2);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE,
                                     enable ? rmc->rmc_id : def_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_3)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE,
                                     enable ? rmc->rmc_id : def_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (rmc->prd_priority & BCM_PORT_F_PRIORITY_TDM)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PRIORITY, DBAL_DEFINE_NIF_PRD_PRIORITY_TDM);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMC, INST_SINGLE,
                                     enable ? rmc->rmc_id : def_val);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See imb_feu_internal.h
 */
int
imb_feu_internal_scheduler_nof_ports_get(
    int unit,
    bcm_port_nif_scheduler_t sch_prio,
    int *nof_ports)
{
    dnx_algo_port_rmc_info_t rmc;
    bcm_pbmp_t feu_ports;
    bcm_port_t logical_port;
    int ii, ports_counter, nof_priority_groups;
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.mac_client.priority_groups_nof_get(unit);
    /*
     * Get all the FlexE MAC clients
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC, 0, &feu_ports));

    ports_counter = 0;
    BCM_PBMP_ITER(feu_ports, logical_port)
    {
        for (ii = 0; ii < nof_priority_groups; ++ii)
        {
            /*
             * Get logical fifo information
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, ii, &rmc));
            if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_prio))
            {
                ++ports_counter;
            }
        }
    }
    *nof_ports = ports_counter;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable / disable egress flush. Egress flush means
 *        continously geanting credits to the port in order to
 *        free occupied resources.
 * see .h file
 */
int
imb_feu_internal_tx_egress_flush_set(
    int unit,
    int client_channel,
    int flush_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_EN, INST_SINGLE, flush_enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get egress flush status
 *
 * see .h file
 */
int
imb_feu_internal_tx_egress_flush_get(
    int unit,
    int client_channel,
    uint32 *flush_enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * Get field value
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FLUSH_EN, INST_SINGLE, flush_enable);
    /*
     * Get entry
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Reset the port tx in the NMG
 * see .h file
 */
int
imb_feu_internal_tx_nmg_reset_hw_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set reset indication
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id,
                                 DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset TMC fifo in FEU
 *
 * see .h file
 */
int
imb_feu_internal_tx_reset_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the indirect access control for
 *    Tiny MAC
 *
 * see .h file
 */
int
imb_framer_internal_mac_access_control_set(
    int unit,
    bcm_port_t port)
{
    int client_channel;
    dbal_enum_value_field_tiny_mac_reg_type_e access_type;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_TINY_MAC_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    if (port != -1)
    {
        access_type = DBAL_ENUM_FVAL_TINY_MAC_REG_TYPE_PER_PORT;
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, INST_SINGLE, client_channel);
    }
    else
    {
        access_type = DBAL_ENUM_FVAL_TINY_MAC_REG_TYPE_GENERAL;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_REGISTER_TYPE, INST_SINGLE, access_type);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure Tiny MAC according to the
 *    table and field info
 *
 * see .h file
 */
int
imb_framer_internal_mac_set(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e * field_id,
    uint32 *val,
    int nof_fields)
{
    int index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set value field
     */
    for (index = 0; index < nof_fields; ++index)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id[index], INST_SINGLE, val[index]);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Tiny MAC value according to the
 *    table and field info
 *
 * see .h file
 */
int
imb_framer_internal_mac_get(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 *val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set value field
     */
    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Polling the required TinyMAC field
 *
 * see .h file
 */
int
imb_framer_internal_mac_polling(
    int unit,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 expected_val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set value field
     */
    SHR_IF_ERR_EXIT(dnxcmn_polling(unit, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, entry_handle_id, field_id, expected_val));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure credit value from TMC to EGQ.
 *
 * see .h file
 */
int
imb_feu_internal_port_tx_credit_val_set(
    int unit,
    int client_channel,
    int credit_val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_VAL, INST_SINGLE, credit_val);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Stop credit from TMC to EGQ.
 *
 * see .h file
 */
int
imb_feu_internal_stop_credit_to_egq_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_STOP_EGRESS_CREDITS, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the TX reset credit value.
 *
 * see .h file
 */
int
imb_feu_internal_tx_reset_credit_value_set(
    int unit,
    uint32 value)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CREDIT_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_VALUE, INST_SINGLE, value);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the TX credit.
 *
 * see .h file
 */
int
imb_feu_internal_tx_credit_reset(
    int unit,
    int client_channel,
    int reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RESET, INST_SINGLE, reset);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FLEXEWP/FSAR/FSCL/FASIC block in/out of reset
 *
 * see .h file
 */
shr_error_e
imb_framer_internal_flexe_blocks_soft_reset(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_BLOCKS_SOFT_RESET, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_RESET, INST_SINGLE, in_reset);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FLEXEWP/FSAR/FSCL/FASIC SBUS in/out of reset
 *
 * see .h file
 */
shr_error_e
imb_framer_internal_flexe_blocks_sbus_reset(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_BLOCKS_SBUS_RESET, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SBUS_RESET, INST_SINGLE, in_reset);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FLEXEWP/FSAR/FSCL/FASIC in/out soft init
 *
 * see .h file
 */
shr_error_e
imb_framer_internal_flexe_blocks_soft_init_enable(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_BLOCKS_SOFT_INIT, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_INIT, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Take target TMC credit in/out of reset
 *
 * see .h file
 */
shr_error_e
imb_feu_internal_target_tmc_credit_reset(
    int unit,
    int first_phy,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, first_phy);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_RSTN, INST_SINGLE, in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ITMH TC + DP offsets for the PRD.
 *
 * see .h file
 */
int
imb_feu_prd_itmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set TC and DP offset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ITMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set FTMH  TC + DP offsets for the PRD
 *
 * see .h file
 */
int
imb_feu_prd_ftmh_offsets_hw_set(
    int unit,
    uint32 tc_offset,
    uint32 dp_offset)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     * Set TC and DP offset
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_TC_OFFSET, INST_SINGLE, tc_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FTMH_DP_OFFSET, INST_SINGLE, dp_offset);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * Enable or disable PRD bypass logic in FEU
 *
 * see .h file
 */
int
imb_feu_internal_prd_bypass_set(
    int unit,
    uint32 is_bypass)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_HARD_STAGE_CTRL, &entry_handle_id));
    /*
     *  Set Bypass mode
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_BYPASS, INST_SINGLE, is_bypass);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure weight for each NIF scheduler
 *
 * see .h file
 */
int
imb_feu_internal_scheduler_config_set(
    int unit,
    uint32 sch_prio,
    uint32 sch_weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_SCHEDULER_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, sch_weight);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get weight for each NIF scheduler
 *
 * see .h file
 */
int
imb_feu_internal_scheduler_config_get(
    int unit,
    uint32 sch_prio,
    uint32 *sch_weight)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_SCHEDULER_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_prio);
    /*
     * request value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SCH_WEIGHT, INST_SINGLE, sch_weight);
    /*
     * commit the request
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
imb_feu_internal_rmc_drop_data_set(
    int unit,
    int rmc_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * Set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DROP_DATA, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_feu_internal_rx_nmg_flush_context_enable_set(
    int unit,
    uint32 sch_priority,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_SCHEDULER_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_priority);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_CONTEXT, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * see .h file
 */
int
imb_feu_internal_rmc_flush_mode_enable_set(
    int unit,
    int rmc_id,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * Set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See .h file
 */
int
imb_feu_internal_rx_flush_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLUSH_ENABLE, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See .h file
 */
int
imb_feu_internal_rx_rmc_counter_polling(
    int unit,
    int rmc_id)
{
    uint32 entry_handle_id;
    sal_usecs_t time_out;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_COUNTER, &entry_handle_id));
    /*
     * Set The RMC to count
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, INST_SINGLE, rmc_id);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    /*
     * Reset DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_NIF_FEU_RX_RMC_COUNTER, entry_handle_id));
    /*
     * Wait for RMC packets counter to be empty.
     */
    time_out = IMB_COSQ_RMC_COUNTER_POLLING_TIME_OUT_USEC;
    SHR_IF_ERR_EXIT(dnxcmn_polling
                    (unit, time_out, IMB_COSQ_RMC_COUNTER_POLLING_TIME_OUT_USEC, entry_handle_id, DBAL_FIELD_COUNTER,
                     0));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_feu_internal_nmg_credit_mask_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_TX_CLIENT_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_TO_EGQ_CRDT_MASK, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * see .h file
 */
int
imb_feu_internal_rx_nmg_per_sch_prio_reset(
    int unit,
    uint32 sch_priority,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_SCHEDULER_CTRL, &entry_handle_id));

    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SCH_PRIORITY, sch_priority);
    /*
     * set reset indication
     */
    dbal_entry_value_field32_set(unit,
                                 entry_handle_id,
                                 DBAL_FIELD_NMG_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
int
imb_feu_internal_feu_credit_mask_set(
    int unit,
    int tmc,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_L1_TX_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TMC, tmc);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L1_CREDIT_MASK, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * See .h file
 */
int
imb_framer_internal_pcs_msbus_mode_enable_set(
    int unit,
    int pm_id,
    int pcs_port,
    int l1_eth_en,
    int flexe_en)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEXE_MODE_EN, INST_SINGLE, flexe_en);
    if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_l1_eth_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L1_ETH_MODE_EN, INST_SINGLE, l1_eth_en);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the client in FlexEWP SB rx.
 *
 * see .h file
 */
int
imb_framer_internal_sb_client_rx_reset_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the client in FlexEWP SB tx.
 *
 * see .h file
 */
int
imb_framer_internal_sb_client_tx_reset_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Reset the tinymac encap in FlexEWP SB tx.
 *
 * see .h file
 */
int
imb_framer_internal_sb_client_enc_reset_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Reset the tinymac decap in FlexEWP SB rx.
 *
 * see .h file
 */
int
imb_framer_internal_sb_client_dec_reset_set(
    int unit,
    int client_channel,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEC_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable 66b control blocks in encap, including LF/RF/LI/LPI
 *
 * see .h file
 */
int
imb_framer_internal_enc_control_blocks_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_66B_CONTROL_BLOCK_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable 66b control blocks in decap, including LF/RF/LI/LPI
 *
 * see .h file
 */
int
imb_framer_internal_dec_control_blocks_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_66B_CONTROL_BLOCK_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable cl82 in encap
 *
 * see .h file
 */
int
imb_framer_internal_enc_cl82_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CL82_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable cl82 in decap
 *
 * see .h file
 */
int
imb_framer_internal_dec_cl82_enable_set(
    int unit,
    int client_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CL82_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the decap data path.
 *
 * see .h file
 */
int
imb_framer_internal_dec_datapath_reset_set(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CTRL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEC_DATAPATH_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Reset the encap data path.
 *
 * see .h file
 */
int
imb_framer_internal_enc_datapath_reset_set(
    int unit,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CTRL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENC_DATAPATH_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure if decoder is ready to decode
 *
 * see .h file
 */
int
imb_framer_internal_dec_ready_set(
    int unit,
    int is_ready)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_RX_CTRL, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DECODER_READY, INST_SINGLE, is_ready);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
imb_feu_rx_prd_port_profile_map_hw_set(
    int unit,
    int client_channel,
    int prd_profile)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROFILE_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, INST_SINGLE, prd_profile);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port RX in FlexEWP NB.
 *
 * see .h file
 */
int
imb_framer_internal_nb_port_rx_reset_set(
    int unit,
    int flexe_core_port,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_FLEXE_CORE_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FLEXE_CORE_PORT, flexe_core_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the port Tx in FlexEWP NB.
 *
 * see .h file
 */
int
imb_framer_internal_nb_port_tx_reset_set(
    int unit,
    int pm_index,
    int pcs_port,
    int in_reset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RESET, INST_SINGLE,
                                 in_reset ?
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_IN_RESET :
                                 DBAL_ENUM_FVAL_NIF_RESET_REVERSE_LOGIC_OUT_OF_RESET);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - enable PRD hard stage per RMC. this effectively
 *        enable the PRD feature.
 *
 * see .h file
 */
int
imb_feu_prd_hard_stage_enable_hw_set(
    int unit,
    uint32 rmc,
    uint32 enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for the PRD hard stage per RMC
 *
 * see .h file
 */
int
imb_feu_prd_hard_stage_enable_hw_get(
    int unit,
    uint32 rmc,
    uint32 *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_ENABLERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE_HARD_STAGE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore indication of IP
 *        dscp even if packet is identified as IP.
 *
 * see .h file
 */
int
imb_feu_prd_ignore_ip_dscp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores IP
 *        DSCP
 *
 * see .h file
 */
int
imb_feu_prd_ignore_ip_dscp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_ip_dscp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_IP_DSCP, INST_SINGLE, ignore_ip_dscp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD parser to ignore MPLS EXP even if packet is
 *        identified as MPLS
 *
 * see .h file
 */
int
imb_feu_prd_ignore_mpls_exp_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        MPLS EXP
 *
 * see .h file
 */
int
imb_feu_prd_ignore_mpls_exp_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_mpls_exp)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_MPLS_EXP, INST_SINGLE, ignore_mpls_exp);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore inner tag PCP DEI
 *        indication even if packet is identified as double
 *        tagged
 *
 * see .h file
 */
int
imb_feu_prd_ignore_inner_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        inner VLAN tag
 *
 * see .h file
 */
int
imb_feu_prd_ignore_inner_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_inner_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_INNER_ETH_TAG, INST_SINGLE, ignore_inner_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD Parser whether to ignore outer tag PCP DEI
 *        indication even if packet is identified as VLAN tagged
 *
 * see .h file
 */
int
imb_feu_prd_ignore_outer_tag_hw_set(
    int unit,
    int prd_profile,
    uint32 ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get from PRD parser indication whether it ignores
 *        outer VLAN tag
 *
 * see .h file
 */
int
imb_feu_prd_ignore_outer_tag_hw_get(
    int unit,
    int prd_profile,
    uint32 *ignore_outer_tag)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IGNORE_OUTER_ETH_TAG, INST_SINGLE, ignore_outer_tag);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set default priority for the PRD hard stage parser.
 *        if the packet is not identified as IP/MPLS/double
 *        tagged/single tagged or if the relevant indications
 *        are set to ignore, the default priority will be given
 *        to the packet. (the priority from the hard stage can
 *        be later overridden by the priority from the soft
 *        stage.
 *
 * see .h file
 */
int
imb_feu_prd_default_priority_hw_set(
    int unit,
    int prd_profile,
    uint32 default_priority)
{
    uint32 entry_handle_id;
    uint32 is_tdm_supported = dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported);
    uint32 is_tdm = 0;
    uint32 max_priority = dnx_data_nif.prd.nof_priorities_get(unit) - 1;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    is_tdm = (default_priority == BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM) ? 1 : 0;

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE,
                                 (is_tdm) ? max_priority : default_priority);
    if (is_tdm_supported)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get default priority given in the PRD hard stage
 *        parser.
 *
 * see .h file
 */
int
imb_feu_prd_default_priority_hw_get(
    int unit,
    int prd_profile,
    uint32 *default_priority)
{
    uint32 entry_handle_id;
    uint32 is_tdm_supported = dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported);
    uint32 is_tdm = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_DEFAULT_PRIORITY, INST_SINGLE, default_priority);
    if (is_tdm_supported)
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &is_tdm);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Return TDM as default priority if it is set 
     */
    if (is_tdm)
    {
        *default_priority = BCM_COSQ_INGRESS_PORT_DROP_PRIORITY_TDM;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable PRD soft stage per port. PRD soft stage is a
 *        TCAM for which the user configures both the table and
 *        the key. if there is a hit in the TCAM, the priority
 *        from the TCAM entry will override the priority from
 *        the hard stage
 *
 * see .h file
 */
int
imb_feu_prd_soft_stage_enable_hw_set(
    int unit,
    int prd_profile,
    uint32 enable_eth,
    uint32 enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication for PRD soft stage
 *
 * see .h file
 */
int
imb_feu_prd_soft_stage_enable_hw_get(
    int unit,
    int prd_profile,
    uint32 *enable_eth,
    uint32 *enable_tm)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_ETH, INST_SINGLE, enable_eth);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SOFT_STAGE_ENABLE_TM, INST_SINGLE, enable_tm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set outer tag size for the port. if port is port
 *        Extender, the outer tag size should be set to 8B,
 *        otherwise 4B
 *
 * see .h file
 */
int
imb_feu_prd_outer_tag_size_hw_set(
    int unit,
    int prd_profile,
    uint32 outer_tag_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OUTER_TAG_SIZE, INST_SINGLE, outer_tag_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD port type, according to the header type of
 *        the port
 *
 * see .h file
 */
int
imb_feu_prd_port_type_hw_set(
    int unit,
    int prd_profile,
    uint32 prd_port_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_PORT_PROPERTIES, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_TYPE, INST_SINGLE, prd_port_type);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - fill the TM priority map (PRD hard stage). add the
 *        the map priority value to be returned per TC + DP
 *        values
 *
 * see .h file
 */
int
imb_feu_prd_map_tm_tc_dp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from TM priority map (PRD hard
 *        stage). get the priority given for a combination of
 *        TC+DP.
 *
 * see .h file
 */
int
imb_feu_prd_map_tm_tc_dp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 tc,
    uint32 dp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TM_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the IP priority table (PRD hard stage). set
 *        priority value for a specific DSCP.
 *
 * see .h file
 */
int
imb_feu_prd_map_ip_dscp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from IP priority table (PRD hard
 *        stage). get the priority returned for a specific DSCP
 *
 * See .h file
 */
int
imb_feu_prd_map_ip_dscp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 dscp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_IP_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DSCP, dscp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the MPLS priority table (PRD hard stage). set
 *        priority value for a specific EXP value.
 *
 * see .h file
 */
int
imb_feu_prd_map_mpls_exp_hw_set(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the MPLS priority table. get
 *        the priority returned for a specific EXP value
 *
 * see .h file
 */
int
imb_feu_prd_map_mpls_exp_hw_get(
    int unit,
    bcm_port_t port,
    uint32 exp,
    uint32 *priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_MPLS_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EXP, exp);

    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the ETH (VLAN) Priority table (PRD hard
 *        stage). set a priority value to match a specific
 *        PCP+DEI combination
 *
 * see .h file
 */
int
imb_feu_prd_map_eth_pcp_dei_hw_set(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief - get information from the ETH (VLAN) priority table
 *        (PRD hard stage). get the priority returned for a
 *        specific combination of PCP+DEI
 *
 * see .h file
 */
int
imb_feu_prd_map_eth_pcp_dei_hw_get(
    int unit,
    bcm_port_t port,
    uint32 pcp,
    uint32 dei,
    uint32 *priority)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_VLAN_PRIORITY_MAP, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCP, pcp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DEI, dei);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set one of the TPID values per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
imb_feu_prd_tpid_hw_set(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the TPID value per port profile to be recognized
 *        by the PRD parser. if a packet TPID is not recognized,
 *        the packet will get the default priority. each port
 *        can have four TPID values.
 *
 * see .h file
 */
int
imb_feu_prd_tpid_hw_get(
    int unit,
    uint32 prd_profile,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TPID, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRD_PROFILE, prd_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, tpid_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TPID, INST_SINGLE, tpid_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set PRD thresholds per priority. based on the
 *        priority given from the parser, the packet is sent to
 *        the correct RMC. in each RMC there is a thresholds per
 *        priority which is mapped to it.
 *
 * see .h file
 */
int
imb_feu_prd_threshold_hw_set(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current threshold for a specific priority in
 *        a specific RMC.
 *
 * see .h file
 */
int
imb_feu_prd_threshold_hw_get(
    int unit,
    uint32 rmc,
    uint32 priority,
    uint32 *threshold)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_THRESHOLDS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, priority);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_THRESHOLD, INST_SINGLE, threshold);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD drop counter value, per RMC
 *
 * see .h file
 */
int
imb_feu_prd_drop_count_hw_get(
    int unit,
    uint32 rmc,
    uint64 *count)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_COUNTERS, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_DROP_COUNTER, INST_SINGLE, count);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set configurable ether type to a ether type code.
 *        the ether type codes are meaningful in the PRD soft
 *        stage (TCAM).
 *
 * See .h file
 */
int
imb_feu_prd_custom_ether_type_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type value mapped to a specific ether
 *        type code, out of the configurable ether types (codes
 *        1-6).
 *
 * see .h file
 */
int
imb_feu_prd_custom_ether_type_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_CONFIGURABLE_ETHER_TYPE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_VAL, INST_SINGLE, ether_type_val);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the offset base for the TCAM key. it means the
 *        offsets that compose the key will start from this
 *        offset base. the offset base have 3 options:
 * 0=>start of packet
 * 1=>end of eth header
 * 2=>end of header after eth header.
 *
 * see .h file
 */
int
imb_feu_prd_tcam_entry_key_offset_base_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_base)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM_KEY_OFFSET_BASE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the current offset base for the given ether
 *        code.
 *
 * see .h file
 */
int
imb_feu_prd_tcam_entry_key_offset_base_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM_KEY_OFFSET_BASE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET_BASE, INST_SINGLE, offset_base);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set ether type size (in bytes) for the given ether
 *        type code. the ether type size is only used if the
 *        offset base for the key is "end of header after eth
 *        header"
 *
 * see .h file
 */
int
imb_feu_prd_ether_type_size_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_ETHER_TYPE_CODE_HEADER_SIZE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get the ether type currently set for a specific
 *        ether type code
 *
 * see .h file
 */
int
imb_feu_prd_ether_type_size_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_ETHER_TYPE_CODE_HEADER_SIZE, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_SIZE, INST_SINGLE, ether_type_size);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set the PRD soft stage (TCAM) key. there is a key
 *        per ether type (total of 16 keys). the key is build of
 *        4 offsets given in the packet header. from each
 *        offset, 8 bits are taken to create a total of 32 bit.
 *        when comparing to the TCAM entries, the ether type
 *        code joins the key to create 36bit key:
 *        |ether type code|offset 4|offset 3|offset 2|offset 1|
 *        -----------------------------------------------------
 *        35              31       23       15       7        0
 *
 * see .h file
 */
int
imb_feu_prd_tcam_entry_key_offset_hw_set(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information about the TCAM key for a specific
 *        ether type.
 *
 * see .h file
 */
int
imb_feu_prd_tcam_entry_key_offset_hw_get(
    int unit,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM_KEY, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, ether_type_code);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OFFSET_INDEX, offset_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OFFSET, INST_SINGLE, offset_value);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - fill the PRD soft stage TCAM table. there are 32
 *        entries in the table. if there is a hit, the priority
 *        for the packet will be taken from the TCAM entry
 *        information
 *
 * see .h file
 */

int
imb_feu_prd_tcam_entry_hw_set(
    int unit,
    uint32 entry_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 entry_info->ether_code_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 entry_info->ether_code_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                                 entry_info->offset_array_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                                 entry_info->offset_array_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, entry_info->tdm);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get information from the PRD soft stage (TCAM)
 *        table. get specific entry information
 *
 * see .h file
 */
int
imb_feu_prd_tcam_entry_hw_get(
    int unit,
    uint32 entry_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_TCAM, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_ENTRY_ID, entry_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &entry_info->ether_code_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &entry_info->ether_code_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_VALUE, INST_SINGLE,
                               &entry_info->offset_array_val);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENTRY_OFFSETS_MASK, INST_SINGLE,
                               &entry_info->offset_array_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &entry_info->priority);
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, &entry_info->tdm);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, &entry_info->is_entry_valid);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set Control Frame properties to be recognized by the
 *        PRD parser. if a control frame is identified, it
 *        automatically gets the highest priority (3). each
 *        packet is compared against all control frame
 *        properties of the ETHU
 *
 * see .h file
 */
int
imb_feu_prd_control_frame_hw_set(
    int unit,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                                 control_frame_config->mac_da_val);
    dbal_entry_value_field64_set(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                                 control_frame_config->mac_da_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                                 control_frame_config->ether_type_code);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                                 control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Control Frame properties recognized by the
 *        parser.
 *
 * see .h file
 */
int
imb_feu_prd_control_frame_hw_get(
    int unit,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_CONTROL_FRAME, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, control_frame_index);
    /*
     * request for value field
     */
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_VALUE, INST_SINGLE,
                               &control_frame_config->mac_da_val);
    dbal_value_field64_request(unit, entry_handle_id, DBAL_FIELD_MAC_DA_MASK, INST_SINGLE,
                               &control_frame_config->mac_da_mask);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE, INST_SINGLE,
                               &control_frame_config->ether_type_code);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ETHER_TYPE_CODE_MASK, INST_SINGLE,
                               &control_frame_config->ether_type_code_mask);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set MPLS special label properties. if one of the
 *        MPLS labels is identified as special label and its
 *        value match one of the special label values, priority
 *        for the packet is taken from the special label
 *        properties.
 *
 * see .h file
 */
int
imb_feu_prd_mpls_special_label_hw_set(
    int unit,
    uint32 special_label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, label_config->label_value);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, label_config->priority);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear SB RX client FIFO overflow interrupt in wrapper
 *
 * see .h file
 */
int
imb_framer_internal_sb_port_rx_overflow_clear(
    int unit)
{
    uint32 reg_val;
    SHR_FUNC_INIT_VARS(unit);

    reg_val = 1 << 10;
    SHR_IF_ERR_EXIT(WRITE_FLEXEWP_INTERRUPT_REGISTERr(unit, reg_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get current configuration of MPLS special label
 *
 * see .h file
 */
int
imb_feu_prd_mpls_special_label_hw_get(
    int unit,
    uint32 special_label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_MPLS_SPECIAL_LABEL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INDEX, special_label_index);
    /*
     * request for value field
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_LABEL_VALUE, INST_SINGLE, &label_config->label_value);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, &label_config->priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &label_config->is_tdm);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SB RX client FIFO overflow interrupt in wrapper
 *
 * see .h file
 */
int
imb_framer_internal_sb_port_rx_overflow_get(
    int unit,
    uint32 *is_overflow)
{
    uint32 reg32_val;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(READ_FLEXEWP_INTERRUPT_REGISTERr(unit, &reg32_val));
    *is_overflow = SHR_BITGET(&reg32_val, 10);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get default pm_id_and pcs port for FLEXEWP_FLEXE_PORT_SOURCE
 *
 * see .h file
 */
int
imb_framer_internal_default_pm_id_and_pcs_port_get(
    int unit,
    int *pm_id,
    int *pcs_port)
{
    dbal_table_field_info_t field_info;

    SHR_FUNC_INIT_VARS(unit);

    *pm_id = 0;
    *pcs_port = 0;

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_NIF_FRAMER_NB_FLEXE_CORE_PORT_CTRL, DBAL_FIELD_FRAMER_PM, 0, 0, 0, &field_info));
    *pm_id = (1 << field_info.field_nof_bits) - 1;

    SHR_IF_ERR_EXIT(dbal_tables_field_info_get
                    (unit, DBAL_TABLE_NIF_FRAMER_NB_FLEXE_CORE_PORT_CTRL, DBAL_FIELD_PCS_PORT, 0, 0, 0, &field_info));
    *pcs_port = (1 << field_info.field_nof_bits) - 1;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get one flexe counter from HW.
 *
 * see .h file
 */
int
imb_framer_internal_mac_counter_get(
    int unit,
    dbal_fields_e field_id,
    uint64 *val)
{
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info = { 0 };
    dbal_tables_e table_id;
    int result;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    result =
        dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART1, field_id, FALSE, 0,
                                          INST_SINGLE, &field_info);
    if (result == _SHR_E_NONE)
    {
        table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART1;
    }
    else
    {
        result =
            dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART2, field_id, FALSE, 0,
                                              INST_SINGLE, &field_info);
        if (result == _SHR_E_NONE)
        {
            table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART2;
        }
        else
        {
            result =
                dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART1, field_id, FALSE, 0,
                                                  INST_SINGLE, &field_info);
            if (result == _SHR_E_NONE)
            {
                table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART1;
            }
            else
            {
                table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART2;
            }
        }
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * get value field
     */
    dbal_value_field64_request(unit, entry_handle_id, field_id, INST_SINGLE, val);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - set one flexe counter from HW.
 *
 * see .h file
 */
int
imb_framer_internal_mac_counter_set(
    int unit,
    dbal_fields_e field_id,
    const uint64 *val)
{
    uint32 entry_handle_id;
    dbal_table_field_info_t field_info = { 0 };
    dbal_tables_e table_id;
    int result;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    result =
        dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART1, field_id, FALSE, 0,
                                          INST_SINGLE, &field_info);
    if (result == _SHR_E_NONE)
    {
        table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART1;
    }
    else
    {
        result =
            dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART2, field_id, FALSE, 0,
                                              INST_SINGLE, &field_info);
        if (result == _SHR_E_NONE)
        {
            table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_RX_PART2;
        }
        else
        {
            result =
                dbal_tables_field_info_get_no_err(unit, DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART1, field_id, FALSE, 0,
                                                  INST_SINGLE, &field_info);
            if (result == _SHR_E_NONE)
            {
                table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART1;
            }
            else
            {
                table_id = DBAL_TABLE_TINY_MAC_MIB_COUNTERS_TX_PART2;
            }
        }
    }

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field64_set(unit, entry_handle_id, field_id, INST_SINGLE, *val);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable Tx credit for Rx valid in case of
 *    local fault.
 *
 * see .h file
 */
int
imb_framer_internal_tx_credit_for_rx_valid_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_RX_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CREDIT_FOR_RX_VALID_EN, INST_SINGLE, enable);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable led functional in flexe wrapper
 *
 * see .h file
 */
int
imb_framer_internal_led_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_LED_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LED_DISABLE, INST_SINGLE, !enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LED_SCAN_DELAY, INST_SINGLE, 51);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LED_MASK_LF, INST_SINGLE, 0);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Program flexmac2_lpbk_Tdm_table in tinymac
 *
 * see .h file
 */

int
imb_framer_internal_mac_loopback_tdm_table_hw_set(
    int unit,
    int client_channel,
    int valid,
    int tdm_slot_start,
    int nof_slot)
{
    int i, table_index, field_offset;
    dbal_fields_e valid_field[] = { DBAL_FIELD_TX_MSBUS_CREDIT_VALID_0, DBAL_FIELD_TX_MSBUS_CREDIT_VALID_1,
        DBAL_FIELD_TX_MSBUS_CREDIT_VALID_2, DBAL_FIELD_TX_MSBUS_CREDIT_VALID_3
    };
    dbal_fields_e channel_field[] = { DBAL_FIELD_TX_MSBUS_PORT_ID_0, DBAL_FIELD_TX_MSBUS_PORT_ID_1,
        DBAL_FIELD_TX_MSBUS_PORT_ID_2, DBAL_FIELD_TX_MSBUS_PORT_ID_3
    };
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (tdm_slot_start + nof_slot > dnx_data_nif.mac_client.max_nof_slots_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "TinyMac loopback tdm slot is out of range\n");
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TINY_MAC_LPBK_TDM_TABLE, &entry_handle_id));

    for (i = tdm_slot_start; i < tdm_slot_start + nof_slot; i++)
    {
        table_index = i / 4;
        field_offset = i % 4;
        /*
         * set key fields
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_TABLE_IDX, table_index);
        /*
         * set value field
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, valid_field[field_offset], INST_SINGLE, (valid != 0));
        dbal_entry_value_field32_set(unit, entry_handle_id, channel_field[field_offset], INST_SINGLE, client_channel);
        /*
         * commit the value
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure start TX for FEU RMC
 *
 * see .h file 
 */
int
imb_feu_internal_port_rx_start_tx_thr_set(
    int unit,
    int rmc_id,
    uint32 start_tx,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_RMC_CTRL, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FEU_RMC_ID, rmc_id);
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_EN, INST_SINGLE, enable);
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_START_TX_THR, INST_SINGLE, start_tx);
    }
    else
    {
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id,
                                                   DBAL_FIELD_START_TX_THR, INST_SINGLE, DBAL_PREDEF_VAL_DEFAULT_VALUE);
    }
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/disable LF/RF/LI for L1 switching in ENC
 *
 * see .h file 
 */
int
imb_framer_internal_lf_rf_li_encap_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_TINY_MAC_CTRL, &entry_handle_id));
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LF_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RF_ENABLE, INST_SINGLE, enable);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LI_ENABLE, INST_SINGLE, enable);
    /*
     * commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 *
 * see .h file
 */
int
imb_feu_tdm_mask_hw_set(
    int unit,
    uint32 value)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FEU_RX_CTRL, &entry_handle_id));
    /*
     *  Set value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MASK_TDM_INDICATION, INST_SINGLE, value);
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}
/**
 * \brief - get all master logical ports on Framer
 */
int
imb_framer_master_ports_get(
    int unit,
    bcm_pbmp_t * framer_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    _SHR_PBMP_CLEAR(*framer_ports);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_MAC_STAT_ONLY |
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_FLEXE_MAC_PROTECTION, framer_ports));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the latest packet PRD priority and TDM indication of a port
 *
 */
int
imb_feu_prd_port_last_packet_priority_tdm_hw_get(
    int unit,
    uint32 *priority,
    uint32 *tdm,
    uint32 *valid)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_PRD_FEU_LAST_RECEIVED_PACKET, &entry_handle_id));

    /*
     * get TDM and PRD priority values and their validaity
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PRIORITY, INST_SINGLE, priority);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TDM, INST_SINGLE, tdm);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_VALID, INST_SINGLE, valid);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * see .h file
 */
int
imb_framer_internal_sar_to_egq_mapping_set(
    int unit,
    int sar_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SAR_TO_EGQ_MAPPING, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SAR_CHANNEL, sar_channel);
    /*
     *  Set value
     */
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAR_EGQ_PORT_ID, INST_SINGLE, sar_channel);
    }
    else
    {
        /** set it to dbal max value*/
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_SAR_EGQ_PORT_ID, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * see .h file
 */
int
imb_framer_internal_egq_to_sar_mapping_set(
    int unit,
    int sar_channel,
    int enable)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_EGQ_TO_SAR_MAPPING, &entry_handle_id));
    /*
     * set key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SAR_EGQ_PORT_ID, sar_channel);
    /*
     *  Set value
     */
    if (enable)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SAR_CHANNEL, INST_SINGLE, sar_channel);
    }
    else
    {
        /** set it to dbal max value*/
        dbal_entry_value_field_predefine_value_set(unit, entry_handle_id, DBAL_FIELD_SAR_CHANNEL, INST_SINGLE,
                                                   DBAL_PREDEF_VAL_MAX_VALUE);
    }
    /*
     * Commit value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * see .h file
 */
int
imb_framer_internal_port_transcode_set(
    int unit,
    int pm_index,
    int pcs_port,
    int is_rx,
    imb_framer_xcode_config_t * xcode_cfg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_NB_PCS_PORT_CTRL, &entry_handle_id));
    /*
     * Set Key
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FRAMER_PM, pm_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PCS_PORT, pcs_port);
    /*
     * set value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TRANSPARENT_AM, INST_SINGLE, xcode_cfg->trans_am_en);

    if (is_rx)
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRANSCODE_EN, INST_SINGLE,
                                     xcode_cfg->xcode_en);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRANSCODE_SHIFT_EN, INST_SINGLE,
                                     xcode_cfg->xcode_shift_en);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_TRANSCODE_CL82_EN, INST_SINGLE,
                                     xcode_cfg->xcode_cl82_en);
    }
    else
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_TRANSCODE_EN, INST_SINGLE,
                                     xcode_cfg->xcode_en);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_TRANSCODE_SHIFT_EN, INST_SINGLE,
                                     xcode_cfg->xcode_shift_en);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_TRANSCODE_CL82_EN, INST_SINGLE,
                                     xcode_cfg->xcode_cl82_en);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_AM_COMPENSATION_EN, INST_SINGLE,
                                     xcode_cfg->am_comp_en);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * see .h file
 */
int
imb_framer_internal_sb_tx_fifo_init_credit_set(
    int unit,
    int client_channel,
    int init_credit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_NIF_FRAMER_SB_TX_CLIENT_CTRL, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_CHANNEL, client_channel);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CLIENT_START_TX_THR, INST_SINGLE, init_credit);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#undef _ERR_MSG_MODULE_NAME
