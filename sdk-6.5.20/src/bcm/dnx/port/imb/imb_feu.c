/** \file imb_feu.c
 *
 *  FlexE client procedures for DNX.
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_feu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm/types.h>
#include <soc/util.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>

#include <sal/core/boot.h>

#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/stat/mib/mib_stat_dbal.h>

#include "imb_feu_internal.h"
#include "imb_ethu_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_FEU_MAC_DRAIN_CELL_TIMEOUT_USEC      (250000)
#define IMB_FEU_MAC_RSV_MASK_ETH_TYPE_CHECK_BIT  (5)

/*
 * FEU Tiny MAC reset mode
 */
typedef enum
{
    IMB_FEU_MAC_SOFT_RESET_MODE_IN_OUT = 0,
    IMB_FEU_MAC_SOFT_RESET_MODE_IN = 1,
    IMB_FEU_MAC_SOFT_RESET_MODE_OUT = 2
} imb_feu_mac_soft_reset_mode_e;
/*
 * FEU Tiny MAC Ctrl info
 */
typedef struct imb_feu_mac_ctrl_s
{
    uint32 rx_enable;
    uint32 tx_enable;
    uint32 soft_reset;
} imb_feu_mac_ctrl_t;
/*
 * FEU Tiny MAC drain cell info
 */
typedef struct imb_feu_mac_drain_cells_s
{
    uint32 rx_pfc_en;
    uint32 llfc_en;
    uint32 rx_pause;
    uint32 tx_pause;
} imb_feu_mac_drain_cells_t;

static int imb_feu_mac_get(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 *val);

static int imb_feu_mac_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 val);

static int imb_feu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int rmc_id_is_changed,
    int enable);

static int imb_feu_scheduler_config_set(
    int unit,
    bcm_port_nif_scheduler_t sch_prio,
    int weight);

static int imb_feu_master_ports_get(
    int unit,
    bcm_pbmp_t * feu_ports);

static int imb_feu_port_fifo_link_list_config_set(
    int unit,
    int client_channel,
    int rmc_id,
    int speed,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int enable);

static int imb_feu_prd_rmc_fifo_size_get(
    int unit,
    bcm_port_t port,
    int *rmc_fifo_size);

/**
 * \brief - Initialize FEU:
 *    Set some Reg values to get to desired SW start point.
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for
 *        initalization.
 * \param [out] imb_specific_info - IMB specific info to be
 *        initialized.
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
imb_feu_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int i;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Initialize register values
     */
    if (!sw_state_is_warm_boot(unit))
    {
        for (i = 0; i < dnx_data_nif.flexe.nof_clients_get(unit); ++i)
        {
            /*
             * Disable data to Portmacro for MAC client.
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_tx_data_to_pm_enable_set(unit, i, 0));
        }
        /*
         * Clear all Schedulers.
         */
        SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set(unit, bcmPortNifSchedulerLow, 0));
        SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set(unit, bcmPortNifSchedulerHigh, 0));
        SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set(unit, bcmPortNifSchedulerTDM, 0));
        /*
         * Set all RMCs thrshold after overflow to 0.
         */
        for (i = 0; i < dnx_data_nif.flexe.nof_logical_fifos_get(unit); ++i)
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_rmc_thr_after_ovf_set(unit, i, 0));
        }
        /*
         * Reset and mask credit in ETH TMC. If the eth TMC is the target TMC for L1 switching,
         * it should be out of reset and unmask.
         */
        for (i = 0; i < dnx_data_nif.phys.nof_phys_get(unit); ++i)
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_target_tmc_credit_reset(unit, i, IMB_COMMON_IN_RESET));
            SHR_IF_ERR_EXIT(imb_feu_internal_feu_credit_mask_set(unit, i, 1));
        }
        /*
         *  Disable masking of TDM indication from parser. CLU/CDU/FEU has a feature to mask TDM indication received from PP
         *  parser. If we don't disable the masking by default priority 3 will be marked as TDM. Set
         *  RX_PORT_MAP_MASK_PARSER_TDM_BITf in FEU_RX_PORT_MAP_MASK_PARSER_TDM to '0'.
         */
        if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported))
        {
            SHR_IF_ERR_EXIT(imb_feu_tdm_mask_hw_set(unit, 0));
        }
        /*
         * If PRD is NOT bypassed.
         */
        if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_prd_is_bypassed))
        {
            /*
             * Turn off the bypass logic - prd bypass is enabled by default
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_prd_bypass_set(unit, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure MAX packet size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - MAX packet size
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
imb_feu_port_max_packet_size_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RX_MAX_SIZE, DBAL_FIELD_RX_MAX_SIZE, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure MAX CRC mode
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] mode - CRC mode
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
imb_feu_port_crc_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_CRC_MODE, mode));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get MAX packet size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - MAX packet size value
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
imb_feu_port_max_packet_size_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_MAX_SIZE, DBAL_FIELD_RX_MAX_SIZE, (uint32 *) value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set pad size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - pad size
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
imb_feu_port_pad_size_set(
    int unit,
    bcm_port_t port,
    int value)
{
    int enable;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * If PAD_EN is set, packets smaller than PAD_THRESHOLD are padded
     * to this size. PAD_THRESHOLD must be >=64 and <= 96.
     */
    enable = value > 0 ? 1 : 0;
    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_EN, enable));
    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_THRESHOLD, value));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get pad size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - pad size
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
imb_feu_port_pad_size_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    uint32 enable;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_EN, &enable));
    if (!enable)
    {
        *value = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL,
                                        DBAL_FIELD_PAD_THRESHOLD, (uint32 *) value));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure MIB counter max size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] value - counter max size
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
imb_feu_port_cntmaxsize_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_MAX_SIZE, value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get MIB counter max size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] value - counter max size
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
imb_feu_port_cntmaxsize_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(imb_feu_mac_get
                    (unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_MAX_SIZE, (uint32 *) value));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure MIB counter max size in MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable/disable mib function
 * \param [in] clear - clear mib counter or not
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
imb_feu_port_mib_counter_control_set(
    int unit,
    bcm_port_t port,
    int enable,
    int clear)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_ENABLE, enable));
    /*
     * Enable and Clear the MAC MIB counters
     * No need to explicitly set the clear flag to 0 since the
     * function internally unsets the clear flag
     */
    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_CLEAR, clear));
    if (clear)
    {
        SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_CLEAR, 0));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * imb_feu_port_mib_counter_get
 *
 * \brief Get flexe counter
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  counter_id       - counter type
 * \param [out]  counter_val     - flexe counter value
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
imb_feu_port_mib_counter_get(
    int unit,
    bcm_port_t port,
    int counter_id,
    uint64 *counter_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    COMPILER_64_ZERO(*counter_val);
    /*
     * map counter id to dbal field Here we use exsiting NIF ETH counter mapping function since tiny mac mib is the
     * same as CDMAC's
     */
    SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, NULL));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    SHR_IF_ERR_EXIT(imb_feu_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_feu_internal_mac_counter_get(unit, field_id, counter_val));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * imb_feu_port_mib_counter_set
 *
 * \brief Set flexe counter,mainly purpose is clear counter
 *
 * \param [in]  unit            - unit id
 * \param [in]  port            - logical port
 * \param [in]  counter_id      - counter type
 * \param [in]  counter_val     - flexe counter value
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
imb_feu_port_mib_counter_set(
    int unit,
    bcm_port_t port,
    int counter_id,
    const uint64 *counter_val)
{
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * map counter id to dbal field Here we use exsiting NIF ETH counter mapping function since tiny mac mib is the
     * same as CDMAC's
     */
    SHR_IF_ERR_EXIT(dnx_mib_counter_id_to_dbal_field_id_get(unit, port, counter_id, &field_id, NULL));
    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    SHR_IF_ERR_EXIT(imb_feu_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_feu_internal_mac_counter_set(unit, field_id, counter_val));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure soft reset in TINY MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] in_reset - in or out of reset
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
imb_feu_port_mac_ctrl_soft_reset_set(
    int unit,
    bcm_port_t port,
    uint32 in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_SOFT_RESET, in_reset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get soft reset in TINY MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] in_reset - in or out of reset
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
imb_feu_port_mac_ctrl_soft_reset_get(
    int unit,
    bcm_port_t port,
    uint32 *in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_SOFT_RESET, in_reset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TINY MAC rx
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
int
imb_feu_port_mac_rx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_RX_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TINY MAC rx enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_mac_rx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_RX_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TINY MAC Tx
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
int
imb_feu_port_mac_tx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_TX_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TINY MAC Tx enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_mac_tx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_TX_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure fault source for TX in Tiny MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - configuration value
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
imb_feu_port_mac_tx_fault_src_set(
    int unit,
    bcm_port_t port,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_FAULT_SOURCE_FOR_TX, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure RSV mask in TINY MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] val - RSV mask value
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
imb_feu_port_mac_rsv_mask_set(
    int unit,
    bcm_port_t port,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RSV_MASK, DBAL_FIELD_MASK, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure RSV mask in TINY MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] val - RSV mask value
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
imb_feu_port_mac_rsv_mask_get(
    int unit,
    bcm_port_t port,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RSV_MASK, DBAL_FIELD_MASK, val));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TINY MAC Tx pause
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_tx_pause_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_TX_PAUSE_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TINY MAC Tx pause enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_mac_tx_pause_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_TX_PAUSE_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TINY MAC rx pause
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_rx_pause_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_RX_PAUSE_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TINY MAC rx pause enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_mac_rx_pause_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_RX_PAUSE_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable TINY MAC rx PFC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_rx_pfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PFC_CTRL, DBAL_FIELD_RX_PFC_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get TINY MAC rx pfc enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_mac_rx_pfc_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PFC_CTRL, DBAL_FIELD_RX_PFC_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable TINY MAC tx discard
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_tx_discard_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_DISCARD_EN, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the cell count in TX FIFO
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] cell_cnt - cell count
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
imb_feu_port_mac_txfifo_cell_cnt_get(
    int unit,
    bcm_port_t port,
    uint32 *cell_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TXFIFO_STATUS, DBAL_FIELD_CELL_CNT, cell_cnt));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the MAC ctrl info for egress queue drain
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] mac_ctrl - MAC control info
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
imb_feu_port_mac_egress_queue_drain_get(
    int unit,
    bcm_port_t port,
    imb_feu_mac_ctrl_t * mac_ctrl)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_get(unit, port, &mac_ctrl->rx_enable));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_enable_get(unit, port, &mac_ctrl->tx_enable));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_get(unit, port, &mac_ctrl->soft_reset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the MAC drain cell info
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] drain_cells - drain cell info
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
imb_feu_port_mac_drain_cell_get(
    int unit,
    bcm_port_t port,
    imb_feu_mac_drain_cells_t * drain_cells)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pfc_enable_get(unit, port, &drain_cells->rx_pfc_en));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pause_enable_get(unit, port, &drain_cells->rx_pause));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_pause_enable_get(unit, port, &drain_cells->tx_pause));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - MAC drain cell start
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_drain_cell_start(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Don't disable TX since it stops egress and hangs if CPU sends
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_enable_set(unit, port, 1));
    /*
     * Disable RX
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_set(unit, port, 0));

    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pause_enable_set(unit, port, 0));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pfc_enable_set(unit, port, 0));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, 1));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_discard_set(unit, port, 1));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Stop MAC drain cell
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] drain_cells - drain cell info
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
imb_feu_port_mac_drain_cell_stop(
    int unit,
    bcm_port_t port,
    const imb_feu_mac_drain_cells_t * drain_cells)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clear Discard fields
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_discard_set(unit, port, 0));
    /*
     * set pause fields
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pause_enable_set(unit, port, drain_cells->rx_pause));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_pause_enable_set(unit, port, drain_cells->tx_pause));
    /*
     * set pfc rx_en fields
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_pfc_enable_set(unit, port, drain_cells->rx_pfc_en));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable Rx for egress queue drain
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rx_en - enable or disable Rx
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
imb_feu_port_mac_egress_queue_drain_rx_enable(
    int unit,
    bcm_port_t port,
    uint32 rx_en)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable RX, de-assert SOFT_RESET
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_set(unit, port, rx_en ? 1 : 0));
    /*
     * Bring mac out of reset
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Recover the orignal MAC control info
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] mac_ctrl - MAC control info
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
imb_feu_port_mac_ctrl_recover(
    int unit,
    bcm_port_t port,
    imb_feu_mac_ctrl_t * mac_ctrl)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_set(unit, port, mac_ctrl->rx_enable));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_enable_set(unit, port, mac_ctrl->tx_enable));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, mac_ctrl->soft_reset));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable or disable the MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Don't disable TX since it stops egress and hangs if CPU sends
     */
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_enable_set(unit, port, 1));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_set(unit, port, enable ? 1 : 0));
    SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, enable ? 0 : 1));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Reset the FlexE client credit in FEU-TX
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_port_credit_tx_reset(
    int unit,
    bcm_port_t port)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get CDU access info
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    /*
     * Reset the port credit value in TX
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_reset_credit_value_set(unit, 0));
    /*
     * Reset the port credit in TX - sequence 0 1 0
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_credit_reset(unit, client_channel, 0));
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_credit_reset(unit, client_channel, 1));
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_credit_reset(unit, client_channel, 0));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Tiny MAC soft reset sequences
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] reset_mode - IN/OUT/IN_OUT reset
 * \param [out] saved_mac_ctrl - Saved MAC control info
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
imb_feu_port_mac_soft_reset_set(
    int unit,
    bcm_port_t port,
    imb_feu_mac_soft_reset_mode_e reset_mode,
    imb_feu_mac_ctrl_t * saved_mac_ctrl)
{
    uint32 cell_count;
    imb_feu_mac_drain_cells_t drain_cells;
    imb_feu_mac_ctrl_t mac_ctrl;
    soc_timeout_t to;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * portmodCallBackActionTypePre - nothing to do
     */
    if (reset_mode != IMB_FEU_MAC_SOFT_RESET_MODE_OUT)
    {
        SHR_IF_ERR_EXIT(imb_feu_port_mac_egress_queue_drain_get(unit, port, &mac_ctrl));
        /*
         * Store the original MAC control info.
         */
        if (saved_mac_ctrl != NULL)
        {
            sal_memcpy(saved_mac_ctrl, &mac_ctrl, sizeof(imb_feu_mac_ctrl_t));
        }
        /*
         * Drain cells
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_drain_cell_get(unit, port, &drain_cells));
        /*
         * Start TX FIFO draining
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_drain_cell_start(unit, port));
        /*
         * De-assert SOFT_RESET to let the drain start
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, 0));

        if (!SAL_BOOT_PLISIM)
        {
            /*
             * Wait until TX fifo cell count is 0
             */
            soc_timeout_init(&to, IMB_FEU_MAC_DRAIN_CELL_TIMEOUT_USEC, 0);
            for (;;)
            {
                SHR_IF_ERR_EXIT(imb_feu_port_mac_txfifo_cell_cnt_get(unit, port, &cell_count));
                if (cell_count == 0)
                {
                    break;
                }
                if (soc_timeout_check(&to))
                {
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "ERROR: u=%d p=%d timeout draining TX FIFO (%d cells remain)\n", unit,
                                 port, cell_count);
                }
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_feu_port_mac_txfifo_cell_cnt_get(unit, port, &cell_count));
        }

        /*
         * Stop TX FIFO draining
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_drain_cell_stop(unit, port, &drain_cells));

        /*
         * Put port into SOFT_RESET
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_soft_reset_set(unit, port, 1));
    }

    if (reset_mode != IMB_FEU_MAC_SOFT_RESET_MODE_IN)
    {
        /*
         * portmodCallBackActionTypeDuring
         */
        SHR_IF_ERR_EXIT(imb_feu_port_credit_tx_reset(unit, port));
        /*
         * For out of reset operation, mac control should be taken from input
         */
        if (reset_mode == IMB_FEU_MAC_SOFT_RESET_MODE_OUT)
        {
            SHR_NULL_CHECK(saved_mac_ctrl, _SHR_E_PARAM, "saved_mac_ctrl");
            sal_memcpy(&mac_ctrl, saved_mac_ctrl, sizeof(imb_feu_mac_ctrl_t));
        }
        /*
         * Enable TX, restore RX, de-assert SOFT_RESET
         */
        SHR_IF_ERR_EXIT(imb_feu_port_mac_egress_queue_drain_rx_enable(unit, port, mac_ctrl.rx_enable));
        /*
         * Restore clmac_CTRL to original value, make sure the mac is not in soft reset status
         */
        mac_ctrl.soft_reset = 0;
        SHR_IF_ERR_EXIT(imb_feu_port_mac_ctrl_recover(unit, port, &mac_ctrl));
        /*
         * portmodCallBackActionTypePost - nothing to do
         */
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set Average inter-packet gap
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ipg_value -IPG value
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
imb_feu_port_tx_average_ipg_set(
    int unit,
    bcm_port_t port,
    int ipg_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set
                    (unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_AVERAGE_IPG, BITS2BYTES(ipg_value)));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get Average inter-packet gap
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] ipg_value -IPG value
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
imb_feu_port_tx_average_ipg_get(
    int unit,
    bcm_port_t port,
    int *ipg_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get
                    (unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_AVERAGE_IPG, (uint32 *) ipg_value));
    *ipg_value = BYTES2BITS(*ipg_value);

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure TX threshold in Tiny MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_thr - Tx threshold
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
imb_feu_port_mac_tx_thr_set(
    int unit,
    bcm_port_t port,
    int tx_thr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_TX_THRESHOLD, tx_thr));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configurations for Tiny mac initialziation
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
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
imb_feu_mac_init(
    int unit,
    bcm_port_t port)
{
    int client_channel, average_ipg_in_bits, tx_thr;
    uint32 rsv_mask[1];
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * CRC append mode
     */
    SHR_IF_ERR_EXIT(imb_feu_port_crc_mode_set(unit, port, 0));
    /*
     * Configure tx_threshold in Tiny MAC
     */
    tx_thr = dnx_data_nif.flexe.mac_tx_threshold_get(unit);
    SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_thr_set(unit, port, tx_thr));
    /*
     * Config average IPG for L1 ports
     */
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info))
    {
        average_ipg_in_bits = dnx_data_nif.flexe.average_ipg_for_l1_get(unit);
        SHR_IF_ERR_EXIT(imb_feu_port_tx_average_ipg_set(unit, port, average_ipg_in_bits));
    }
    if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_max_packet_size_set(unit, port, 512));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_feu_port_max_packet_size_set(unit, port, 10240));
        SHR_IF_ERR_EXIT(imb_feu_port_mib_counter_control_set(unit, port, 1, 1));
        SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_fault_src_set(unit, port, 1));
        SHR_IF_ERR_EXIT(imb_feu_port_mac_rsv_mask_get(unit, port, rsv_mask));
        SHR_BITCLR(rsv_mask, IMB_FEU_MAC_RSV_MASK_ETH_TYPE_CHECK_BIT);
        SHR_IF_ERR_EXIT(imb_feu_port_mac_rsv_mask_set(unit, port, rsv_mask[0]));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free all the RMC resources
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_id_is_changed - Indicate if the RMC ID is changed
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
imb_feu_rmc_resource_free(
    int unit,
    bcm_port_t port,
    int rmc_id_is_changed)
{
    int i, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc_arr[DNX_DATA_MAX_NIF_FLEXE_PRIORITY_GROUPS_NOF];
    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (i = 0; i < nof_priority_groups; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc_arr[i]));
        if (rmc_arr[i].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        /*
         * set the RMC to invalid values
         */
        rmc_arr[i].first_entry = 0;
        rmc_arr[i].last_entry = 0;
        rmc_arr[i].prd_priority = IMB_PRD_PRIORITY_ALL;
        rmc_arr[i].thr_after_ovf = 0;
    }

    SHR_IF_ERR_EXIT(imb_feu_port_priority_config_enable_set
                    (unit, port, rmc_arr, nof_priority_groups, rmc_id_is_changed, 0));

    for (i = 0; i < nof_priority_groups; ++i)
    {
        if (rmc_arr[i].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        /*
         * Free link list
         */
        SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                        (unit, -1, rmc_arr[i].rmc_id, 0, DNX_ALGO_FLEXE_FIFO_RMC, 0));
        /*
         * Free RMC
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_rmc_id_free(unit, rmc_arr[i].rmc_id));
        /*
         * Clear SW DB
         */
        rmc_arr[i].rmc_id = IMB_COSQ_RMC_INVALID;
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, i, &rmc_arr[i]));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add new FlexE client to FEU
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - not used currently
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
imb_feu_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int client_channel, stop_credit_to_egq = 0;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, FALSE, FALSE))
    {
        /*
         * Exit directly for BusA client and SAR
         */
        SHR_EXIT();
    }
    /*
     * Enable Tx data
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_data_to_pm_enable_set(unit, client_channel, 1));
    /*
     * Configure if need to stop credit from TMC to EGQ
     */
    stop_credit_to_egq =
        dnx_data_nif.flexe.feature_get(unit,
                                       dnx_data_nif_flexe_l1_mismatch_rate_support) ? 0
        : (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info) ? 1 : 0);
    SHR_IF_ERR_EXIT(imb_feu_internal_stop_credit_to_egq_enable_set(unit, client_channel, stop_credit_to_egq));
    /*
     * Mask the credit to EGQ in NMG
     */
    SHR_IF_ERR_EXIT(imb_feu_internal_nmg_credit_mask_set(unit, client_channel, stop_credit_to_egq));
    /*
     * Configure Tiny Mac
     */
    SHR_IF_ERR_EXIT(imb_feu_mac_init(unit, port));
    /*
     * Disable the port
     */
    SHR_IF_ERR_EXIT(imb_feu_port_enable_set(unit, port, 0, 0));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - port interface rate
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int idx, tx_thr = -1, client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Lookup TX threshold in table.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    for (idx = 0; idx < dnx_data_nif.flexe.start_tx_threshold_table_info_get(unit)->key_size[0]; idx++)
    {
        if (speed <= dnx_data_nif.flexe.start_tx_threshold_table_get(unit, idx)->speed)
        {
            tx_thr =
                DNX_ALGO_PORT_TYPE_IS_L1(unit, port_info) ? dnx_data_nif.flexe.start_tx_threshold_table_get(unit,
                                                                                                            idx)->start_thr_for_l1
                : dnx_data_nif.flexe.start_tx_threshold_table_get(unit, idx)->start_thr;

            if (tx_thr == 0)
            {
                /** If the value is 0, means the there is no valid start_thr for the current speed */
                continue;
            }
            else
            {
                break;
            }
        }
    }

    if (tx_thr < 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unsupported speed %d (port %d)\n", speed, port);
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(imb_feu_internal_tx_start_thr_set(unit, client_channel, tx_thr));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Flush packets in FEU RX
 */
static int
imb_feu_port_rx_flush_and_reset(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_rmc_info_t rmc;
    int nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    int priority_group;
    uint32 nof_bits_per_sec;
    uint32 time_usec, dbal_sch_priority;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Run flush sequence for each of the RMCs
     */
    for (priority_group = 0; priority_group < nof_priority_groups; ++priority_group)
    {
        /*
         * get logical fifo info
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, priority_group, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC id not valid
             */
            continue;
        }
        /**
         * Enable flush sequence
         */
        /*
         * Disable Data to RMC from PM
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_drop_data_set(unit, rmc.rmc_id, 1));
        /*
         * In case SOP entered the fifo, we need to wait until all the packet
         * will enter.
         * Worst case is maximum packet size on a minimum bandwidth port (0.5Gbps, 1588 channel).
         */
        nof_bits_per_sec = dnx_data_nif.eth.packet_size_max_get(unit) * UTILEX_NOF_BITS_IN_CHAR;
        time_usec = UTILEX_DIV_ROUND_UP(nof_bits_per_sec, 500);
        sal_usleep(time_usec);
        /*
         * Enable flush contexts in NMG
         */
        SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, rmc.sch_priority, &dbal_sch_priority));
        SHR_IF_ERR_EXIT(imb_feu_internal_rx_nmg_flush_context_enable_set(unit, dbal_sch_priority, 1));
        /*
         * Enable Flush mode in NMG
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(unit, 0, 1));
        /*
         * Enable flush mode for RMC
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_flush_mode_enable_set(unit, rmc.rmc_id, 1));
        /*
         * Start flushing
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rx_flush_enable_set(unit, 1));
        /*
         * Wait until RMC counter is empty
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rx_rmc_counter_polling(unit, rmc.rmc_id));
        /**
         * Reset the RMC
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_port_rx_reset_set(unit, port, rmc.rmc_id, IMB_COMMON_IN_RESET));
        /**
         * Disable flush sequence
         */
        /*
         * Disable flush mode for RMC
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_flush_mode_enable_set(unit, rmc.rmc_id, 0));
        /*
         * Stop flushing
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rx_flush_enable_set(unit, 0));
        /*
         * Disable Flush mode in NMG
         */
        SHR_IF_ERR_EXIT(imb_ethu_port_rx_nmg_flush_mode_enable_hw_set(unit, 0, 0));
        /*
         * Disable flush contexts in NMG
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rx_nmg_flush_context_enable_set(unit, dbal_sch_priority, 0));
        /*
         * Enable Data to RMC from PM
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_drop_data_set(unit, rmc.rmc_id, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset the priority group in the scheduler.
 *   Since the sch_priority is common for all ports in the FEU, need
 *   to update the reset only if our port is the only port in the FEU
 *   which has this sch_priority.
 */
static int
imb_feu_port_rx_nmg_per_sch_prio_reset(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    int in_reset)
{
    bcm_pbmp_t feu_ports;
    bcm_port_t port_i;
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    int rmc_index = 0;
    uint32 dbal_sch_priority;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get ETH ports on FEU
     */
    SHR_IF_ERR_EXIT(imb_feu_master_ports_get(unit, &feu_ports));
    _SHR_PBMP_PORT_REMOVE(feu_ports, port);
    /*
     * Search for other ports in the FEU that have the same sch_priority,
     * if there are other ports using this priority, we cannot reset.
     */
    _SHR_PBMP_ITER(feu_ports, port_i)
    {
        for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port_i, 0, rmc_index, &rmc));
            if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority))
            {
                /*
                 * Return directly since we cannot perform reset
                 */
                SHR_EXIT();
            }
        }
    }
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_priority, &dbal_sch_priority));
    SHR_IF_ERR_EXIT(imb_feu_internal_rx_nmg_per_sch_prio_reset(unit, dbal_sch_priority, in_reset));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset priority groups in the scheduler
 *   Configure the specific sch_priority, or all sch_priorities
 *   of the port in case sch_priority=IMB_COSQ_ALL_SCH_PRIO.
 */
static int
imb_feu_port_rx_nmg_sch_prio_reset(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    int in_reset)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    int rmc_index = 0;
    bcm_port_nif_scheduler_t sch_priority_to_reset;

    SHR_FUNC_INIT_VARS(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        sch_priority_to_reset = (sch_priority == IMB_COSQ_ALL_SCH_PRIO) ? rmc.sch_priority : sch_priority;
        if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority_to_reset))
        {
            SHR_IF_ERR_EXIT(imb_feu_port_rx_nmg_per_sch_prio_reset(unit, port, sch_priority_to_reset, in_reset));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable FlexE client
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags
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
int
imb_feu_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    dnx_algo_port_info_s port_info;
    int client_channel, has_tx_speed;
    uint32 sb_rx_fifo_overflow = 1;
    uint32 overflow_check_cnt = 100;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, FALSE, FALSE))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));

    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_egress_flush_set(unit, client_channel, 0));

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, TRUE));
        }

        /*
         * reset the port in FEU TX
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_nmg_reset_hw_set(unit, client_channel, IMB_COMMON_IN_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));

        /*
         * take the port out of reset in FEU TX
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_nmg_reset_hw_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        /*
         * take All the scheduler in NMG out of reset
         */
        SHR_IF_ERR_EXIT(imb_feu_port_rx_nmg_sch_prio_reset(unit, port, IMB_COSQ_ALL_SCH_PRIO, IMB_COMMON_OUT_OF_RESET));
        /*
         * take the port out of reset in FEU RX
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_port_rx_reset_set(unit, port, IMB_COSQ_ALL_RMCS, IMB_COMMON_OUT_OF_RESET));
        /*
         * Wait for NIF to finish transmitting initial credits
         */
        sal_usleep(10);

        /*
         * EGQ credit set
         */
        if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info) && has_tx_speed)
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_default_set(unit, port));
        }
        SHR_IF_ERR_EXIT(imb_feu_port_mac_tx_enable_set(unit, port, enable));
        SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_set(unit, port, enable));
        SHR_IF_ERR_EXIT(imb_feu_port_mac_soft_reset_set(unit, port, IMB_FEU_MAC_SOFT_RESET_MODE_IN_OUT, NULL));
        /*
         * Enable Qpairs
         */
        if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, FALSE));
        }

        if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_double_slots_for_small_client)
            && !dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_double_slots_for_all_client))
        {
            /** out of reset SB client rx fifo */
            do
            {
                SHR_IF_ERR_EXIT(imb_feu_internal_sb_port_rx_overflow_clear(unit));
                SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
                SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
                /** wait for interrupt update */
                sal_usleep(2000);
                SHR_IF_ERR_EXIT(imb_feu_internal_sb_port_rx_overflow_get(unit, &sb_rx_fifo_overflow));
            }
            while (sb_rx_fifo_overflow && (--overflow_check_cnt));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        }
    }
    else
    {
        /*
         * disable Qpairs
         */
        if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, FALSE));
        }
        SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
        /*
         * take All the scheduler in NMG in reset
         */
        SHR_IF_ERR_EXIT(imb_feu_port_rx_nmg_sch_prio_reset(unit, port, IMB_COSQ_ALL_SCH_PRIO, IMB_COMMON_IN_RESET));
        /*
         * reset the port in FEU (RMC)
         */
        SHR_IF_ERR_EXIT(imb_feu_port_rx_flush_and_reset(unit, port));
        /*
         * flush the egress
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_egress_flush_set(unit, client_channel, 1));

        SHR_IF_ERR_EXIT(imb_feu_port_mac_enable_set(unit, port, 0));

        /*
         * Configure TXI logic and IRDY threshold
         */
        if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
            && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, FALSE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE Client enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - Enable or disable
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
imb_feu_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    *enable = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE(unit, port_info, FALSE, FALSE))
    {
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(imb_feu_port_mac_rx_enable_get(unit, port, (uint32 *) enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FlexE link list fifo
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
imb_feu_fifo_link_list_enable_set(
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
    SHR_IF_ERR_EXIT(imb_feu_internal_fifo_link_list_ctrl_set(unit, type, fifo_index, first_entry, nof_entries, enable));

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
            SHR_IF_ERR_EXIT(imb_feu_internal_fifo_link_list_config(unit, type, pre_entry, entry_idx, enable));
        }
        pre_entry = entry_idx;
    }
    /*
     * The last entry should point to the first one
     */
    _SHR_PBMP_LAST(*fifo_entries, last_entry);
    SHR_IF_ERR_EXIT(imb_feu_internal_fifo_link_list_config(unit, type, last_entry, first_entry, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for SB RX
 *
 * \param [in] unit - chip unit id
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
imb_feu_sb_rx_calendar_set(
    int unit)
{
    int speed, client_channel, max_nof_clients, has_rx_speed;
    uint32 slots_per_client[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    uint32 calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the FlexE MAC clients
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
    _SHR_PBMP_ITER(logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        if (!has_rx_speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
        if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, client_channel))
        {
            /*
             * Force the slot to "1"
             */
            slots_per_client[client_channel] = 1;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
            slots_per_client[client_channel] = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
        /*
         * Always enable OAM client
         */
        slots_per_client[dnx_data_nif.flexe.oam_client_channel_get(unit)] = 1;
    }

    /** Build uniform calendar */
    max_nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                    (unit, slots_per_client, max_nof_clients, max_nof_clients, calendar));

    SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_SB_RX_CALENDAR, max_nof_clients, calendar));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for SB TX
 *
 * \param [in] unit - chip unit id
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
imb_feu_sb_tx_calendar_set(
    int unit)
{
    int speed, client_channel, nof_clients = 0, nof_flexe_clients = 0, has_tx_speed;
    uint32 slots_per_client[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    uint32 calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    int oam_channel_is_active = 0, ptp_channel_is_active = 0;
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the FlexE MAC clients
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
    _SHR_PBMP_ITER(logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        if (!has_tx_speed)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
        if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            oam_channel_is_active = 1;
        }
        else if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, client_channel))
        {
            ptp_channel_is_active = 1;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
            slots_per_client[client_channel] = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
    }

    /** Build uniform calendar */
    nof_flexe_clients = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                    (unit, slots_per_client, nof_flexe_clients, nof_flexe_clients, calendar));
    /*
     * For SB tx, put the slots for channel 80 and 81 at the last.
     */
    calendar[dnx_data_nif.flexe.oam_client_channel_get(unit)] =
        oam_channel_is_active ? dnx_data_nif.flexe.oam_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    calendar[dnx_data_nif.flexe.ptp_client_channel_get(unit)] =
        ptp_channel_is_active ? dnx_data_nif.flexe.ptp_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;

    nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_SB_TX_CALENDAR, nof_clients, calendar));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for TMC
 *
 * \param [in] unit - chip unit id
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
imb_feu_tmc_calendar_set(
    int unit)
{
    int speed, client_channel, nof_clients = 0, nof_flexe_clients, has_tx_speed;
    uint32 slots_per_client[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    uint32 calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_CLIENTS] = { 0 };
    uint32 tmp_calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_FLEXE_CLIENTS] = { 0 };
    int i, index = 0, oam_channel_is_active = 0, ptp_channel_is_active = 0;
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all the FlexE MAC clients
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
    _SHR_PBMP_ITER(logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
        if (!has_tx_speed)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
        if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            oam_channel_is_active = 1;
        }
        else if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP(unit, client_channel))
        {
            ptp_channel_is_active = 1;
        }
        else
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
            slots_per_client[client_channel] = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        }
    }
    /** Build uniform calendar */
    nof_flexe_clients = dnx_data_nif.flexe.nof_flexe_clients_get(unit);
    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                    (unit, slots_per_client, nof_flexe_clients, nof_flexe_clients, tmp_calendar));

    calendar[dnx_data_nif.flexe.oam_tx_cal_slot_get(unit)] =
        oam_channel_is_active ? dnx_data_nif.flexe.oam_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    calendar[dnx_data_nif.flexe.ptp_tx_cal_slot_get(unit)] =
        ptp_channel_is_active ? dnx_data_nif.flexe.ptp_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    /*
     * For TMC, we put the slots for channel 80 and 81 at the special location
     * defined in DNX DATA.
     */
    for (i = 0; i < dnx_data_nif.flexe.nof_clients_get(unit); ++i)
    {
        if ((i != dnx_data_nif.flexe.oam_tx_cal_slot_get(unit)) && (i != dnx_data_nif.flexe.ptp_tx_cal_slot_get(unit)))
        {
            calendar[i] = tmp_calendar[index];
            ++index;
        }
    }
    nof_clients = dnx_data_nif.flexe.nof_clients_get(unit);
    SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_TMC_CALENDAR, nof_clients, calendar));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for RMC
 *
 * \param [in] unit - chip unit id
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
imb_feu_rmc_calendar_set(
    int unit)
{
    int speed, client_channel, has_rx_speed;
    int i, nof_rmcs = 0, nof_priority_groups, cal_len;
    uint32 slots_per_rmc[DNX_DATA_MAX_NIF_FLEXE_NOF_LOGICAL_FIFOS] = { 0 };
    uint32 slots_per_hi_or_esb_rmc[DNX_DATA_MAX_NIF_FLEXE_NOF_LOGICAL_FIFOS] = { 0 };
    uint32 calendar[DNX_DATA_MAX_NIF_FLEXE_NOF_LOGICAL_FIFOS] = { 0 };
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get all the FlexE MAC clients
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC,
                     DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);
    _SHR_PBMP_ITER(logical_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
        if (!has_rx_speed)
        {
            continue;
        }
        SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, logical_port, &client_channel));
        for (i = 0; i < nof_priority_groups; ++i)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, logical_port, 0, i, &rmc));
            if (rmc.rmc_id != IMB_COSQ_RMC_INVALID)
            {
                if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
                {
                    /*
                     * Force the slot to "1"
                     */
                    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rmc_cal_per_priority)
                        && (rmc.sch_priority != bcmPortNifSchedulerLow))
                    {
                        /*
                         * High or ESB RMC slots
                         */
                        slots_per_hi_or_esb_rmc[rmc.rmc_id] = 1;
                    }
                    else
                    {
                        /*
                         * If rmc_cal_per_priority is enabled, it is for low RMC slot
                         * If rmc_cal_per_priority is disabled, it is for all the RMC slot
                         */
                        slots_per_rmc[rmc.rmc_id] = 1;
                    }
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
                    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rmc_cal_per_priority)
                        && (rmc.sch_priority != bcmPortNifSchedulerLow))
                    {
                        /*
                         * High or ESB RMC slots
                         */
                        slots_per_hi_or_esb_rmc[rmc.rmc_id] =
                            speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
                    }
                    else
                    {
                        /*
                         * If rmc_cal_per_priority is enabled, it is for low RMC slot
                         * If rmc_cal_per_priority is disabled, it is for all the RMC slot
                         */
                        slots_per_rmc[rmc.rmc_id] = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
                    }
                }
            }
        }
    }
    /** Build uniform calendar */
    nof_rmcs = dnx_data_nif.flexe.nof_logical_fifos_get(unit);
    cal_len =
        dnx_data_nif.flexe.feature_get(unit,
                                       dnx_data_nif_flexe_rmc_cal_per_priority) ? dnx_data_nif.
        flexe.nof_clients_get(unit) : dnx_data_nif.flexe.nof_logical_fifos_get(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build(unit, slots_per_rmc, nof_rmcs, cal_len, calendar));
    SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_RMC_CALENDAR, cal_len, calendar));

    if (dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_rmc_cal_per_priority))
    {
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build(unit, slots_per_hi_or_esb_rmc, nof_rmcs, cal_len, calendar));
        SHR_IF_ERR_EXIT(imb_feu_internal_calendar_config(unit, IMB_FEU_HI_OR_ESB_RMC_CALENDAR, cal_len, calendar));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FEU client calendars
 *
 * \param [in] unit - chip unit id
 * \param [in] port - port ID
 * \param [in] flags - port resource flags
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * If there is no RX and TX flag, it means to alloc resource for
     * both directions.
     */
    if (!IMB_FLEXE_DIRECTION_RX_ONLY_GET(flags) && !IMB_FLEXE_DIRECTION_TX_ONLY_GET(flags))
    {
        IMB_FLEXE_DIRECTION_RX_ONLY_SET(flags);
        IMB_FLEXE_DIRECTION_TX_ONLY_SET(flags);
    }

    if (IMB_FLEXE_DIRECTION_RX_ONLY_GET(flags))
    {
        /*
         * SB RX calendar
         */
        SHR_IF_ERR_EXIT(imb_feu_sb_rx_calendar_set(unit));
    }

    if (IMB_FLEXE_DIRECTION_TX_ONLY_GET(flags))
    {
        /*
         * SB Tx calendar
         */
        SHR_IF_ERR_EXIT(imb_feu_sb_tx_calendar_set(unit));
        /*
         * TMC calendar
         */
        SHR_IF_ERR_EXIT(imb_feu_tmc_calendar_set(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the credit from TMC to EGQ
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - client speed
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_tx_credit_to_egq_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    int client_channel, credit_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_imb_feu_tx_credit_val_get(unit, speed, &credit_val));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    SHR_IF_ERR_EXIT(imb_feu_internal_port_tx_credit_val_set(unit, client_channel, credit_val));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set FlexE client port speed configuration
 *
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
static int
imb_feu_port_fifo_link_list_config_set(
    int unit,
    int client_channel,
    int rmc_id,
    int speed,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int enable)
{
    int fifo_index;
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
         */
        if (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX)
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
        }
        /*
         * Allocate fifo link list
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_alloc(unit, fifo_index, speed, fifo_type, &fifo_entries));
        /*
         * Configure the Link List into HW
         */
        SHR_IF_ERR_EXIT(imb_feu_fifo_link_list_enable_set(unit, fifo_type, fifo_index, &fifo_entries, 1));
        /*
         * Take SB Rx out of reset after configuring the fifo
         */
        if (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX)
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        }
    }
    else
    {
        /*
         * Set SB Rx in reset before disabling the FIFO
         */
        if (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX)
        {
            SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
        }
        /*
         * Free fifo link list in DB
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_free(unit, fifo_index, fifo_type, &fifo_entries));

        if (!_SHR_PBMP_IS_NULL(fifo_entries))
        {
            /*
             * Disable link list
             */
            SHR_IF_ERR_EXIT(imb_feu_fifo_link_list_enable_set(unit, fifo_type, fifo_index, &fifo_entries, 0));
        }

    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FIFO resources for
 *    SB RX FIFO
 *    TMC
 *    RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags, Rx or Tx
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
int
imb_feu_port_fifo_resource_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int client_channel, oam_channel;
    int new_nof_entries, orig_nof_entries;
    int speed, oam_speed, has_speed;
    bcm_pbmp_t fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    if (IMB_FLEXE_DIRECTION_RX_ONLY_GET(flags))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        }
        else
        {
            speed = 0;
        }
        /*
         * Get required Entries
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_entries_required_get
                        (unit, DNX_ALGO_FLEXE_FIFO_SB_RX, speed, &new_nof_entries));
        /*
         * Get original nof entries
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get
                        (unit, client_channel, DNX_ALGO_FLEXE_FIFO_SB_RX, &fifo_entries));
        _SHR_PBMP_COUNT(fifo_entries, orig_nof_entries);

        if (orig_nof_entries != new_nof_entries)
        {
            if (orig_nof_entries != 0)
            {
                /*
                 * if original speed is not zero
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, client_channel, -1, 0, DNX_ALGO_FLEXE_FIFO_SB_RX, 0));
            }
            if (has_speed)
            {
                /*
                 * Allocate new SB RX fifo link list and enable it
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, client_channel, -1, speed, DNX_ALGO_FLEXE_FIFO_SB_RX, 1));
            }
        }
        /*
         * Always enable OAM channel by default, which is used to active SB RX calendar
         */
        if (!DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            oam_channel = dnx_data_nif.flexe.oam_client_channel_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get
                            (unit, oam_channel, DNX_ALGO_FLEXE_FIFO_SB_RX, &fifo_entries));
            /*
             * Allocate the link list only when fifo_entries is NULL
             */
            if (_SHR_PBMP_IS_NULL(fifo_entries))
            {
                oam_speed = dnx_data_nif.flexe.oam_channel_max_bandwidth_get(unit);
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, oam_channel, -1, oam_speed, DNX_ALGO_FLEXE_FIFO_SB_RX, 1));
            }
        }
    }

    if (IMB_FLEXE_DIRECTION_TX_ONLY_GET(flags))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        }
        else
        {
            speed = 0;
        }
        /*
         * Get required Entries
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_entries_required_get
                        (unit, DNX_ALGO_FLEXE_FIFO_TMC, speed, &new_nof_entries));
        /*
         * Get original nof entries
         */
        SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get
                        (unit, client_channel, DNX_ALGO_FLEXE_FIFO_TMC, &fifo_entries));
        _SHR_PBMP_COUNT(fifo_entries, orig_nof_entries);

        if (has_speed)
        {
            /*
             * Configure Tx credit to EGQ
             */
            SHR_IF_ERR_EXIT(imb_feu_tx_credit_to_egq_set(unit, port, speed));
        }
        /*
         * Configure Tx credit counter
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_credit_counter_enable_set(unit, client_channel, 1));

        /*
         * If there is speed change, we need to free the fifo link list first
         */
        if (orig_nof_entries != new_nof_entries)
        {
            if (orig_nof_entries != 0)
            {
                /*
                 * Free SB RX fifo link list
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, client_channel, -1, 0, DNX_ALGO_FLEXE_FIFO_TMC, 0));
            }
            if (has_speed)
            {
                /*
                 * Allocate new SB RX fifo link list and enable it
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, client_channel, -1, speed, DNX_ALGO_FLEXE_FIFO_TMC, 1));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Update the priority configurations,
 *    including SW DB and HW resources
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority configurations
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_port_priority_config_update(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int i, new_nof_entries, speed, orig_nof_groups = 0, orig_nof_entries = 0;
    int total_nof_mem_entries = 0, has_speed, rmc_id_is_changed = 1;
    bcm_pbmp_t fifo_entries;
    dnx_algo_port_rmc_info_t rmc[DNX_DATA_MAX_NIF_FLEXE_PRIORITY_GROUPS_NOF];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the original configuration
     */
    for (i = 0; i < dnx_data_nif.flexe.priority_groups_nof_get(unit); ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc[i]));
        if (rmc[i].rmc_id != IMB_COSQ_RMC_INVALID)
        {
            orig_nof_groups++;
        }
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
    if (has_speed)
    {
        /*
         * Get new nof entries for each RMC
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        new_nof_entries = speed / dnx_data_nif.flexe.client_speed_granularity_get(unit);
        /*
         * Get original nof entries for one RMC
         */
        if (orig_nof_groups != 0)
        {
            SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get
                            (unit, rmc[0].rmc_id, DNX_ALGO_FLEXE_FIFO_RMC, &fifo_entries));
            _SHR_PBMP_COUNT(fifo_entries, orig_nof_entries);
        }
        /*
         * If the speed is always valid value and the number of priority group is not changed,
         * the RMC ID is also not changed 
         */
        if (orig_nof_entries && new_nof_entries && ((orig_nof_groups == priority_config->nof_priority_groups)))
        {
            rmc_id_is_changed = 0;
        }
        /*
         * If the nof priority groups or nof entries are changed,
         * need to free the original resource and allocated again.
         */
        if ((orig_nof_groups != priority_config->nof_priority_groups) || (new_nof_entries != orig_nof_entries))
        {
            /*
             * Need to free the resource only when the original resource has been allocated
             */
            if (orig_nof_groups != 0)
            {
                SHR_IF_ERR_EXIT(imb_feu_rmc_resource_free(unit, port, rmc_id_is_changed));
            }
            for (i = 0; i < priority_config->nof_priority_groups; ++i)
            {
                /*
                 * Allocate new RMC ID for each priority group
                 */
                SHR_IF_ERR_EXIT(dnx_algo_flexe_rmc_id_alloc(unit, &rmc[i].rmc_id));
                /*
                 * Configure FIFO Link list for each RMC
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                                (unit, -1, rmc[i].rmc_id, speed, DNX_ALGO_FLEXE_FIFO_RMC, 1));
                /*
                 * Get overflow threshold
                 */
                total_nof_mem_entries = new_nof_entries * dnx_data_nif.flexe.nof_mem_entries_per_slot_get(unit);
                SHR_IF_ERR_EXIT(dnx_algo_port_imb_threshold_after_ovf_get
                                (unit, port, total_nof_mem_entries, &rmc[i].thr_after_ovf));
            }
        }
        /*
         * Update the DB
         */
        for (i = 0; i < priority_config->nof_priority_groups; ++i)
        {
            rmc[i].sch_priority = priority_config->priority_groups[i].sch_priority;
            rmc[i].prd_priority = priority_config->priority_groups[i].source_priority;
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_set(unit, port, 0, i, &rmc[i]));
        }
        /*
         * Update HW
         */
        SHR_IF_ERR_EXIT(imb_feu_port_priority_config_enable_set
                        (unit, port, rmc, priority_config->nof_priority_groups, rmc_id_is_changed, 1));
    }
    else
    {
        /*
         * If the RX speed is 0, clear the original resources.
         */
        if (orig_nof_groups != 0)
        {
            SHR_IF_ERR_EXIT(imb_feu_rmc_resource_free(unit, port, rmc_id_is_changed));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify function for IMB FEU priority configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority configurations
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_port_priority_config_verify(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    int nof_prio_groups_supported, i, nof_entries, prd_priority_mask;
    int prio_found[DNX_DATA_MAX_NIF_ETH_PRIORITY_GROUPS_NOF] = { 0 };
    bcm_port_nif_scheduler_t sch_prio;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    nof_prio_groups_supported = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    if (priority_config->nof_priority_groups > nof_prio_groups_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Number of priority groups %d is not supported\n",
                     priority_config->nof_priority_groups);
    }

    for (i = 0; i < priority_config->nof_priority_groups; ++i)
    {
        nof_entries = priority_config->priority_groups[i].num_of_entries;
        sch_prio = priority_config->priority_groups[i].sch_priority;
        prd_priority_mask = priority_config->priority_groups[i].source_priority;

        SHR_VAL_VERIFY(nof_entries, 0, _SHR_E_PARAM, "num_of_entries is not configurable.\n");

        if (prio_found[sch_prio])
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Having two priority groups with the same scheduler priority is not supported\n");
        }
        prio_found[sch_prio] = 1;
        if (prd_priority_mask == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d is not mapped to any source priority\n", i);
        }
        else if (prd_priority_mask & (~IMB_PRD_PRIORITY_ALL))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Priority group %d source priority is invalid\n", i);
        }
        /*
         * Check if the NIF scheduler priority assignment is correct.
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC_L1(unit, port_info))
        {
            if (sch_prio != bcmPortNifSchedulerTDM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "For L1 port, NIF scheduler priority must be TDM.\n");
            }
        }
        else
        {
            if (sch_prio == bcmPortNifSchedulerTDM)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "For non-L1 port, NIF scheduler priority cannot be TDM.\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - polling to make sure RMC FIFO is empty before
 *        reseting it.
 *
 * \param [in] unit - chip unit id
 * \param [in] rmc_id - RMC id to poll for empty
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
imb_feu_rmc_empty_wait(
    int unit,
    uint32 rmc_id)
{
    uint32 fifo_level;
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_internal_rmc_level_get(unit, rmc_id, &fifo_level));

    soc_timeout_init(&to, 10000 /* 10 millisec */ , 10 /* min polls */ );
    while (fifo_level)
    {
        if (soc_timeout_check(&to))
        {
            /*
             * Note that even after timeout expires, the status should be checked
             * one more time.  Otherwise there is a race condition where an
             * ill-placed O/S task reschedule could cause a false timeout.
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_rmc_level_get(unit, rmc_id, &fifo_level));
            if (fifo_level)
            {
                SHR_ERR_EXIT(_SHR_E_TIMEOUT, "Timeout while waiting for FIFO %d to clear", rmc_id);
            }
            break;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get all master logical ports on FEU
 */
static int
imb_feu_master_ports_get(
    int unit,
    bcm_pbmp_t * feu_ports)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    _SHR_PBMP_CLEAR(*feu_ports);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FLEXE_MAC, 0, feu_ports));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get nof master logical ports on FEU
 */
static int
imb_feu_nof_master_ports_get(
    int unit,
    int *nof_ports)
{
    bcm_pbmp_t feu_ports;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    *nof_ports = 0;
    SHR_IF_ERR_EXIT(imb_feu_master_ports_get(unit, &feu_ports));
    _SHR_PBMP_COUNT(feu_ports, *nof_ports);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FEU current bandwidth
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] is_port_include - indication whether to include /
 *        exclude specified port in the BW calculation
 * \param [out] bandwidth - BW calculated
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
imb_feu_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth)
{
    bcm_pbmp_t feu_ports;
    bcm_port_t port_i;
    int speed, has_speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_master_ports_get(unit, &feu_ports));

    *bandwidth = 0;
    if (!is_port_include)
    {
        /*
         * don't include the current port in the calculation (means port is in process of being removed)
         */
        _SHR_PBMP_PORT_REMOVE(feu_ports, port);
    }
    _SHR_PBMP_ITER(feu_ports, port_i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port_i, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port_i, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        }
        else
        {
            speed = 0;
        }
        *bandwidth += speed;
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure weight for each NIF scheduler
 */
static int
imb_feu_scheduler_config_set(
    int unit,
    bcm_port_nif_scheduler_t sch_prio,
    int weight)
{
    uint32 dbal_sch_prio, dbal_sch_weight;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * convert bcm sch_prio to DBAL sch_prio
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
    /*
     * convert to DBAL weight from reguler int (0-4)
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_weight_get(unit, weight, &dbal_sch_weight));

    SHR_IF_ERR_EXIT(imb_feu_internal_scheduler_config_set(unit, dbal_sch_prio, dbal_sch_weight));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - get port FEU active indication in the specified
 *        scheduler.
 * see .h file
 */
int
imb_feu_port_scheduler_active_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 *active)
{
    uint32 dbal_sch_prio, weight;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * convert bcm sch_prio to DBAL sch_prio
     */
    SHR_IF_ERR_EXIT(imb_common_dbal_sch_prio_from_bcm_sch_prio_get(unit, sch_prio, &dbal_sch_prio));
    SHR_IF_ERR_EXIT(imb_feu_internal_scheduler_config_get(unit, dbal_sch_prio, &weight));

    *active = weight ? 1 : 0;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the port's FEU as active in the specified
 *        scheduler
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_prio - NIF scheduler priority
 * \param [in] is_active - enable or disable
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
imb_feu_port_scheduler_active_set(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_prio,
    uint32 is_active)
{
    uint32 curr_active;
    int nof_ports = 0, weight = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (sch_prio == bcmPortNifSchedulerLow)
    {
        /*
         * Get the weight for the low priority scheduler
         */
        SHR_IF_ERR_EXIT(dnx_port_algo_imb_ethu_scheduler_weight_get(unit, port, is_active, &weight));
        SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set(unit, sch_prio, weight));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_feu_port_scheduler_active_get(unit, port, sch_prio, &curr_active));
        if (is_active == curr_active)
        {
            /*
             * Nothing to do
             */
            SHR_EXIT();
        }
        else if (is_active)
        {
            /*
             * This is the first port in the FEU that needs this priority
             */
            /*
             * for TDM / High priority - we always set the weight to the max BW.
             */
            SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set
                            (unit, sch_prio, dnx_data_nif.scheduler.nof_weight_bits_get(unit)));
        }
        else if (is_active == 0)
        {
            /*
             * before disabling this FEU in the High / TDM scheduler, we need to check if other
             * ports on the FEU need this scheduler
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_scheduler_nof_ports_get(unit, sch_prio, &nof_ports));

            if (nof_ports == 1)
            {
                SHR_IF_ERR_EXIT(imb_feu_scheduler_config_set(unit, sch_prio, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FEU scheduler
 *
 * \param [in] unit   - chip unit id
 * \param [in] port   - logical port
 * \param [in] enable - 0:disable port, 1:enable port
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *  * None
 */
int
imb_feu_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);
    max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }

        /** Set the port's FEU as active in the specified scheduler */
        SHR_IF_ERR_EXIT(imb_feu_port_scheduler_active_set(unit, port, rmc.sch_priority, enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable the port priority configurations
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_arr - RMC array
 * \param [in] nof_rmcs - RMC array size
 * \param [in] rmc_id_is_changed - Indicate if it is for speed update
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
imb_feu_port_priority_config_enable_set(
    int unit,
    bcm_port_t port,
    dnx_algo_port_rmc_info_t * rmc_arr,
    int nof_rmcs,
    int rmc_id_is_changed,
    int enable)
{
    int i, client_channel;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * before we start the sequence, we need to make sure all the RMC FIFOs are empty, and reset them
     */
    for (i = 0; i < nof_rmcs; ++i)
    {
        if (rmc_arr[i].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }
        /*
         *  wait for the FIFO to clear before reseting it
         */
        SHR_IF_ERR_EXIT(imb_feu_rmc_empty_wait(unit, rmc_arr[i].rmc_id));
        /*
         * take All the scheduler in NMG in reset
         */
        SHR_IF_ERR_EXIT(imb_feu_port_rx_nmg_sch_prio_reset(unit, port, rmc_arr[i].sch_priority, IMB_COMMON_IN_RESET));
        /*
         * take the port in reset in FEU RX
         */
        SHR_IF_ERR_EXIT(imb_feu_port_rx_flush_and_reset(unit, port));
    }

    SHR_IF_ERR_EXIT(imb_feu_port_scheduler_config_set(unit, port, enable));

    /*
     * change the priority configuration
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    for (i = 0; i < nof_rmcs; ++i)
    {
        if (rmc_arr[i].rmc_id == IMB_COSQ_RMC_INVALID)
        {
            continue;
        }
        /*
         * Map RMC to client
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_to_client_map_set
                        (unit, client_channel, &rmc_arr[i], rmc_id_is_changed, enable));
        /*
         * Map client to RMC
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_client_to_rmc_map(unit, client_channel, &rmc_arr[i], enable));
        /*
         * Set RX threshold after overflow
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_rmc_thr_after_ovf_set(unit, rmc_arr[i].rmc_id, rmc_arr[i].thr_after_ovf));

    }
    if (enable)
    {
        /*
         * take the RMCs out of reset
         */
        for (i = 0; i < nof_rmcs; ++i)
        {
            if (rmc_arr[i].rmc_id == IMB_COSQ_RMC_INVALID)
            {
                continue;
            }
            /*
             * take All the scheduler in NMG in reset
             */
            SHR_IF_ERR_EXIT(imb_feu_port_rx_nmg_sch_prio_reset
                            (unit, port, rmc_arr[i].sch_priority, IMB_COMMON_OUT_OF_RESET));
            /*
             * Take RMCs out of reset in the CDU and NMG
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_port_rx_reset_set(unit, port, rmc_arr[i].rmc_id, IMB_COMMON_OUT_OF_RESET));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the given priority groups configration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority_config - priority groups configuration
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
imb_feu_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * verify data is correct
     */
    SHR_IF_ERR_EXIT(imb_feu_port_priority_config_verify(unit, port, priority_config));
    /*
     * 1. Release the original RMC resource if needed
     * 2. Update new configuration in SW DB
     * 3. Set new configuration in HW
     */
    SHR_IF_ERR_EXIT(imb_feu_port_priority_config_update(unit, port, priority_config));
    /*
     * Build RMC calendar
     */
    SHR_IF_ERR_EXIT(imb_feu_rmc_calendar_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the given priority groups configration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] priority_config - priority groups configuration
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
imb_feu_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    int i, nof_priority_groups;
    int rmc_fifo_size;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    sal_memset(priority_config, 0, sizeof(bcm_port_prio_config_t));

    for (i = 0; i < nof_priority_groups; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        priority_config->priority_groups[i].sch_priority = rmc.sch_priority;
        priority_config->priority_groups[i].source_priority = rmc.prd_priority;
        /*
         * Get RMC fifo size
         */
        SHR_IF_ERR_EXIT(imb_feu_prd_rmc_fifo_size_get(unit, port, &rmc_fifo_size));
        priority_config->priority_groups[i].num_of_entries = rmc_fifo_size;

        ++priority_config->nof_priority_groups;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure start tx for RMC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_thr - start tx threshold for RMC
 * \param [in] enable - Enable or disable start tx
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
imb_feu_port_rx_start_tx_thr_set(
    int unit,
    bcm_port_t port,
    int tx_thr,
    int enable)
{
    int i, nof_priority_groups;
    dnx_algo_port_rmc_info_t rmc;

    SHR_FUNC_INIT_VARS(unit);

    nof_priority_groups = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (i = 0; i < nof_priority_groups; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, i, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_feu_internal_port_rx_start_tx_thr_set(unit, rmc.rmc_id, tx_thr, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure L1 Mismatch rate configuration for RMC.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tx_speed - The speed for target TMC
 * \param [in] enable - Enable or disable start tx
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
imb_feu_port_l1_mismatch_rate_rx_config_set(
    int unit,
    bcm_port_t port,
    int tx_speed,
    int enable)
{
    int idx, rx_speed, tx_thr, is_found = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &rx_speed));
    /*
     * Check if the speed combination is legal
     */
    for (idx = 0; idx < dnx_data_nif.flexe.l1_mismatch_rate_table_info_get(unit)->key_size[0]; idx++)
    {
        if ((rx_speed == dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->client_speed) &&
            (tx_speed == dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->eth_speed))
        {
            is_found = 1;
            break;
        }
    }
    if (is_found)
    {
        /*
         * Configure start tx
         */
        if (rx_speed < tx_speed)
        {
            /*
             * Get start Tx threshold
             */
            tx_thr = dnx_data_nif.flexe.l1_mismatch_rate_table_get(unit, idx)->tx_thr;
            SHR_IF_ERR_EXIT(imb_feu_port_rx_start_tx_thr_set(unit, port, tx_thr, enable));
        }
    }
    else if (!dnx_data_nif.flexe.feature_get(unit, dnx_data_nif_flexe_disable_l1_mismatch_rate_checking))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "L1 mismatch rate switching doesn't support client speed %d and eth speed %d.\n",
                     rx_speed, tx_speed);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common Tiny MAC configuration API
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] table - Tiny MAC table
 * \param [in] field - Tiny MAC field
 * \param [in] val - configuration value
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
imb_feu_mac_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    SHR_IF_ERR_EXIT(imb_feu_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_feu_internal_mac_set(unit, table, field, val));
exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common Tiny MAC get API
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] table - Tiny MAC table
 * \param [in] field - Tiny MAC field
 * \param [out] val - configuration value
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
imb_feu_mac_get(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    SHR_IF_ERR_EXIT(imb_feu_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_feu_internal_mac_get(unit, table, field, val));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free resources when removing ports.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags
 *
 * \return
 *   int - see _SHR_E_*
 *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_remove_resource_free(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int speed, client_channel, nof_ports, oam_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    if (IMB_FLEXE_DIRECTION_RX_ONLY_GET(flags))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
        /*
         * Free the RMC resources, including SW DB, Hw resources.
         */
        SHR_IF_ERR_EXIT(imb_feu_rmc_resource_free(unit, port, 1));
        /*
         * Free the SB Rx resources.
         */
        SHR_IF_ERR_EXIT(imb_feu_nof_master_ports_get(unit, &nof_ports));
        if ((nof_ports > 1) && DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            /*
             * When removing OAM client, we need to check if there is other active client.
             * If yes, we need to keep OAM client active in SB RX.
             */
            SHR_IF_ERR_EXIT(imb_feu_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        }
        else
        {
            SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                            (unit, client_channel, -1, speed, DNX_ALGO_FLEXE_FIFO_SB_RX, 0));
        }
        if ((nof_ports == 1) && !DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            /*
             * If this is the last MAC client in FEU and it is not OAM client, need to
             * remove the OAM client resource in SB RX.
             */
            speed = dnx_data_nif.flexe.oam_channel_max_bandwidth_get(unit);
            oam_channel = dnx_data_nif.flexe.oam_client_channel_get(unit);
            SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                            (unit, oam_channel, -1, speed, DNX_ALGO_FLEXE_FIFO_SB_RX, 0));
        }
    }
    if (IMB_FLEXE_DIRECTION_TX_ONLY_GET(flags))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &speed));
        /*
         * Free TMC resources
         */
        SHR_IF_ERR_EXIT(imb_feu_port_fifo_link_list_config_set
                        (unit, client_channel, -1, speed, DNX_ALGO_FLEXE_FIFO_TMC, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove a port from the FEU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E_*
 *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_detach(
    int unit,
    bcm_port_t port)
{
    int client_channel, enable, has_speed;
    dnx_algo_port_info_s port_info;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_CLIENT(unit, port_info))
    {
        SHR_EXIT();
    }
    /*
     * Check if the speed is configured for the client, if speed is
     * not configured, we don't need to free some resources, check
     * Rx and Tx speed separately
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_RX, &has_speed));
    if (has_speed)
    {
        IMB_FLEXE_DIRECTION_RX_ONLY_SET(flags);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed)
    {
        IMB_FLEXE_DIRECTION_TX_ONLY_SET(flags);
    }
    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        /*
         * Check if the FlexE MAC client is in enable status
         */
        SHR_IF_ERR_EXIT(imb_feu_port_enable_get(unit, port, &enable));
        if (enable)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Cannot detach active port %d", port);
        }
        /*
         * Free the client resources, including SW DB, Hw resources for RMC, TMC, SB RX fifo.
         */
        SHR_IF_ERR_EXIT(imb_feu_port_remove_resource_free(unit, port, flags));
        /*
         * Update RMC calendar if needed
         */
        if (IMB_FLEXE_DIRECTION_RX_ONLY_GET(flags))
        {
            SHR_IF_ERR_EXIT(imb_feu_rmc_calendar_set(unit));
        }
        /*
         * Update TMC, SB rx/tx calendar
         */
        SHR_IF_ERR_EXIT(imb_feu_calendar_set(unit, 0, flags));
        /*
         * Free FlexE core resource if needed
         */
        if (IMB_FLEXE_DIRECTION_TX_ONLY_GET(flags))
        {
            SHR_IF_ERR_EXIT(flexe_core_mac_client_delete(unit, client_channel, 0));
        }
        /*
         * Disable tx data to PM
         */
        SHR_IF_ERR_EXIT(imb_feu_internal_tx_data_to_pm_enable_set(unit, client_channel, 0));
    }
    else
    {
        /*
         * SAR client
         */
        SHR_IF_ERR_EXIT(flexe_core_sar_client_delete(unit, client_channel, flags, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Deinit the FEU
 *
 * \param [in] unit - chip unit id
 * \param [in] imb_info - IMB info required for
 *        initalization.
 * \param [in] imb_specific_info - IMB specific info to be
 *        initialized.
 *
 * \return
 *   int - see _SHR_E_*
 *
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_FUNC_EXIT;
}

int
imb_feu_prd_port_profile_map_set(
    int unit,
    bcm_port_t port,
    uint32 prd_profile)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    /*
     * Save the mapping in sw state
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.set(unit, port, prd_profile));

    /*
     * Set the mapping in hw
     */
    SHR_IF_ERR_EXIT(imb_feu_rx_prd_port_profile_map_hw_set(unit, client_channel, prd_profile));
exit:
    SHR_FUNC_EXIT;
}

int
imb_feu_prd_port_profile_map_get(
    int unit,
    bcm_port_t port,
    uint32 *prd_profile_p)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_db.prd.port_to_profile_map.get(unit, port, prd_profile_p));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable PRD Hard stage per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
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
imb_feu_prd_hard_stage_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_feu_prd_hard_stage_enable_hw_set(unit, rmc.rmc_id, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get enable indication per port for the PRD hard
 *        stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable indication
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_prd_hard_stage_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    uint32 max_rmcs_per_port;
    int rmc_index;
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if (rmc.rmc_id == IMB_COSQ_RMC_INVALID)
        {
            /*
             * RMC is invalid
             */
            continue;
        }
        SHR_IF_ERR_EXIT(imb_feu_prd_hard_stage_enable_hw_get(unit, rmc.rmc_id, enable));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Verify if port type is supported for
 *         FlexE PRD.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static int
imb_feu_prd_enable_verify(
    int unit,
    bcm_port_t port)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * If port is NOT FlexE MAC L2/L3 return error message
     */
    if (!DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PRD should not be enabled on FlexE MAC L1 client.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable PRD for the port. this call should be
 *        last, after the PRD is already configured.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - enable modes for the PRD:
 * bcmCosqIngressPortDropDisable
 * bcmCosqIngressPortDropEnableHardStage
 * bcmCosqIngressPortDropEnableHardAndSoftStage
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * it is required to call header type set before enabling
 *     the PRD.
 * \see
 *   * None
 */
int
imb_feu_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    uint32 prd_profile;
    uint32 en_soft_stage_eth = 0, en_soft_stage_tm = 0;
    uint32 is_eth_port = 0, is_tm_port = 0;
    uint32 port_type = 0;
    uint32 outer_tag_size = 0;
    int tc_offset = 0;
    int dp_offset = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_enable_verify(unit, port));

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    if (enable_mode == bcmCosqIngressPortDropDisable)
    {
        /*
         * Disable Hard stage
         */
        SHR_IF_ERR_EXIT(imb_feu_prd_hard_stage_enable_set(unit, port, 0));

        /*
         * Disable Soft Stage
         */
        SHR_IF_ERR_EXIT(imb_feu_prd_soft_stage_enable_hw_set(unit, prd_profile, 0, 0));

    }
    else
    {
        /*
         * set TC/DP offsets for ITMH/FTMH
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_itmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_feu_prd_itmh_offsets_hw_set(unit, tc_offset, dp_offset));

        SHR_IF_ERR_EXIT(dnx_cosq_prd_ftmh_offsets_get(unit, port, &tc_offset, &dp_offset));
        SHR_IF_ERR_EXIT(imb_feu_prd_ftmh_offsets_hw_set(unit, tc_offset, dp_offset));
        /*
         * Get is port extender
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_outer_tag_size_get(unit, port, &outer_tag_size));
        SHR_IF_ERR_EXIT(imb_feu_prd_outer_tag_size_hw_set(unit, prd_profile, outer_tag_size));

        /*
         * Set prd port type
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));
        SHR_IF_ERR_EXIT(imb_feu_prd_port_type_hw_set(unit, prd_profile, port_type));

        /*
         * Enable soft stage
         */
        en_soft_stage_eth = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_eth_port)) ? 1 : 0;
        en_soft_stage_tm = ((enable_mode == bcmCosqIngressPortDropEnableHardAndSoftStage) && (is_tm_port)) ? 1 : 0;
        SHR_IF_ERR_EXIT(imb_feu_prd_soft_stage_enable_hw_set(unit, prd_profile, en_soft_stage_eth, en_soft_stage_tm));

        /*
         * Enable Hard stage
         */
        SHR_IF_ERR_EXIT(imb_feu_prd_hard_stage_enable_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD enable mode per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable_mode - PRD enable modes:
 *  bcmCosqIngressPortDropDisable
 *  bcmCosqIngressPortDropEnableHardStage
 *  bcmCosqIngressPortDropEnableHardAndSoftStage
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
imb_feu_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    uint32 is_eth_port = 0, is_tm_port = 0, en_hard_stage, en_soft_stage_eth = 0, en_soft_stage_tm = 0;
    uint32 prd_profile;
    uint32 port_type = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_enable_verify(unit, port));

    SHR_IF_ERR_EXIT(imb_feu_prd_hard_stage_enable_get(unit, port, &en_hard_stage));

    if (en_hard_stage)
    {
        /*
         * Get prd profile for the given port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

        /*
         * The expectation is that eth soft stage will only be set for eth ports, and tm soft stage will only be set
         * for tm ports. but we check for the port type just in case
         */
        SHR_IF_ERR_EXIT(dnx_cosq_prd_port_type_get(unit, port, &port_type, &is_eth_port, &is_tm_port));

        SHR_IF_ERR_EXIT(imb_feu_prd_soft_stage_enable_hw_get(unit, prd_profile, &en_soft_stage_eth, &en_soft_stage_tm));

        if ((en_soft_stage_eth && is_eth_port) || (en_soft_stage_tm && is_tm_port))
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardAndSoftStage;
        }
        else
        {
            *enable_mode = bcmCosqIngressPortDropEnableHardStage;
        }
    }
    else
    {
        *enable_mode = bcmCosqIngressPortDropDisable;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set different PRD properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to set:
 *  see enum imb_prd_properties_t.
 * \param [in] val - value of the property to set.
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
imb_feu_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    uint32 prd_profile;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_ip_dscp_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_mpls_exp_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_inner_tag_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_outer_tag_hw_set(unit, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_feu_prd_default_priority_hw_set(unit, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get PRD property value
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] property - PRD property to get.
 * see enum imb_prd_properties_t.
 * \param [out] val - returned property value
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
imb_feu_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    switch (property)
    {
        case imbImbPrdIgnoreIpDscp:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_ip_dscp_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreMplsExp:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_mpls_exp_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreInnerTag:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_inner_tag_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdIgnoreOuterTag:
            SHR_IF_ERR_EXIT(imb_feu_prd_ignore_outer_tag_hw_get(unit, prd_profile, val));
            break;
        case imbImbPrdDefaultPriority:
            SHR_IF_ERR_EXIT(imb_feu_prd_default_priority_hw_get(unit, prd_profile, val));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD property %d\n", property);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set PRD hard stage priority map.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - map type:
 *  see bcm_cosq_ingress_port_drop_map_t.
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [in] priority - priority value to set to the key in
 *        the map.
 * \param [in] is_tdm - TDM signal
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_tm_tc_dp_hw_set(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_ip_dscp_hw_set(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_eth_pcp_dei_hw_set(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_mpls_exp_hw_set(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get priority value mapped to a key in one of the PRD
 *        priority tables
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] map - type of map. see
 *        bcm_cosq_ingress_port_drop_map_t
 * \param [in] key - key index for the map (depends on the map
 *        type)
 * \param [out] priority - returned priority value
 * \param [out] is_tdm - TDM signal
 * \return
 *   int
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    uint32 dp, tc, dscp, pcp, dei, exp;
    SHR_FUNC_INIT_VARS(unit);

    *is_tdm = 0;
    switch (map)
    {
        case bcmCosqIngressPortDropTmTcDpPriorityTable:
            IMB_COSQ_PRD_TM_KEY_TC_GET(key, tc);
            IMB_COSQ_PRD_TM_KEY_DP_GET(key, dp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_tm_tc_dp_hw_get(unit, port, tc, dp, priority));
            break;
        case bcmCosqIngressPortDropIpDscpToPriorityTable:
            IMB_COSQ_PRD_IP_KEY_DSCP_GET(key, dscp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_ip_dscp_hw_get(unit, port, dscp, priority));
            break;
        case bcmCosqIngressPortDropEthPcpDeiToPriorityTable:
            IMB_COSQ_PRD_ETH_KEY_PCP_GET(key, pcp);
            IMB_COSQ_PRD_ETH_KEY_DEI_GET(key, dei);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_eth_pcp_dei_hw_get(unit, port, pcp, dei, priority));
            break;
        case bcmCosqIngressPortDropMplsExpToPriorityTable:
            IMB_COSQ_PRD_MPLS_KEY_EXP_GET(key, exp);
            SHR_IF_ERR_EXIT(imb_feu_prd_map_mpls_exp_hw_get(unit, port, exp, priority));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PRD Map %d\n", map);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_feu_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(imb_feu_prd_tpid_hw_set(unit, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get TPID value for the port profile
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] tpid_index - tpid index (0-3). per port there can
 *        be 4 identified TPIDs
 * \param [in] tpid_value - TPID value
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
imb_feu_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    uint32 prd_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get prd profile for the given port
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_prd_profile_get(unit, port, &prd_profile));

    SHR_IF_ERR_EXIT(imb_feu_prd_tpid_hw_get(unit, prd_profile, tpid_index, tpid_value));

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief - set PRD threshold value per priority per port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority for the threshold
 * \param [in] threshold - threshold value
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
imb_feu_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    int rmc_fifo_size;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get RMC id for port.
     */
    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));
    /*
     * Get RMC fifo size
     */
    SHR_IF_ERR_EXIT(imb_feu_prd_rmc_fifo_size_get(unit, port, &rmc_fifo_size));

    if (threshold > rmc_fifo_size)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Threshold %d for priority %d of port %d is higher than the fifo size %d\n",
                     threshold, priority, port, rmc_fifo_size);
    }
    SHR_IF_ERR_EXIT(imb_feu_prd_threshold_hw_set(unit, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get RMC fifo size
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] rmc_fifo_size - rmc_fifo_size
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
imb_feu_prd_rmc_fifo_size_get(
    int unit,
    bcm_port_t port,
    int *rmc_fifo_size)
{
    int client_channel;
    bcm_pbmp_t fifo_entries;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get client channel for port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));
    /*
     * Get fifo entries allocated per client channel and count them.
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get(unit, client_channel, DNX_ALGO_FLEXE_FIFO_RMC, &fifo_entries));
    _SHR_PBMP_COUNT(fifo_entries, *rmc_fifo_size);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get Threshold value for the given priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] priority - priority to get threshold
 * \param [in] threshold - returned threshold
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
imb_feu_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    dnx_algo_port_rmc_info_t rmc;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_prd_threshold_rmc_get(unit, port, priority, &rmc));

    SHR_IF_ERR_EXIT(imb_feu_prd_threshold_hw_get(unit, rmc.rmc_id, priority, threshold));
exit:
    SHR_FUNC_EXIT;

}

int
imb_feu_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    *threshold = dnx_data_nif.prd.rmc_threshold_max_get(unit);

    SHR_FUNC_EXIT;
}

/**
 * \brief - get PRD drop counter per sch_priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] sch_priority - scheduler priority. the PRD
 *        counter is per RMC, so the user of this function gives
 *        the scheduler priority in order to find which RMC to
 *        read the counter from
 * \param [out] count - counter value
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
imb_feu_prd_drop_count_get(
    int unit,
    bcm_port_t port,
    bcm_port_nif_scheduler_t sch_priority,
    uint64 *count)
{
    dnx_algo_port_rmc_info_t rmc;
    uint32 max_rmcs_per_port, rmc_found = 0;
    int rmc_index;
    SHR_FUNC_INIT_VARS(unit);

    max_rmcs_per_port = dnx_data_nif.flexe.priority_groups_nof_get(unit);

    for (rmc_index = 0; rmc_index < max_rmcs_per_port; ++rmc_index)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_logical_fifo_get(unit, port, 0, rmc_index, &rmc));
        if ((rmc.rmc_id != IMB_COSQ_RMC_INVALID) && (rmc.sch_priority == sch_priority))
        {
            SHR_IF_ERR_EXIT(imb_feu_prd_drop_count_hw_get(unit, rmc.rmc_id, count));
            rmc_found = 1;
            break;
        }
    }

    if (!rmc_found)
    {
        COMPILER_64_SET(*count, 0, 0);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set configurable ether type to be recognized by the
 *        PRD Parser
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code to recognize
 *        the new ether type (1-6)
 * \param [in] ether_type_val - ether type value
 *
 * \return
 *   int  -see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    uint32 ether_type_code_max;
    SHR_FUNC_INIT_VARS(unit);

    ether_type_code_max = dnx_data_nif.prd.custom_ether_type_code_max_get(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_custom_ether_type_hw_set(unit, ether_type_code - 1, ether_type_val));

    if (ether_type_code == ether_type_code_max)
    {
        /*
         * The last configurable ether type (with ether type code 7) is saved for TM port types. the way to save it is
         * to always configure ether type code 6 and ether type code 7 to the same value. so if the user want to
         * configure ether type code 6, we copy the same value to ether type code 7
         */
        SHR_IF_ERR_EXIT(imb_feu_prd_custom_ether_type_hw_set(unit, ether_type_code, ether_type_val));
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get ether type mapped to a specific configurable
 *        ether type  code
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - configurable ether type code to
 *        get (1-6)
 * \param [in] ether_type_val - eter type value mapped to the
 *        given ether type code.
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
imb_feu_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_custom_ether_type_hw_get(unit, ether_type_code - 1, ether_type_val));

exit:
    SHR_FUNC_EXIT;

}

int
imb_feu_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_base_hw_set(unit, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}
int
imb_feu_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_base_hw_get(unit, ether_type_code, offset_base));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set all fixed ether types sizes. (in bytes). this
 *        function is called only once per port, when the PRD is
 *        enabled (fixed ether types should always have the
 *        same size). the configurable ether types sizes are set
 *        by the user, see API
 *        imb_feu_prd_flex_key_construct_set
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [in] ether_type_size - ether type size
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
imb_feu_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_ether_type_size_hw_set(unit, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get ether types sizes. (in bytes).
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] ether_type_code - ether type code
 * \param [out] ether_type_size - ether type size
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
imb_feu_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_ether_type_size_hw_get(unit, ether_type_code, ether_type_size));
exit:
    SHR_FUNC_EXIT;
}

int
imb_feu_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_hw_set(unit, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;

}

int
imb_feu_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_hw_get(unit, ether_type_code, offset_index, offset_value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - set an entry to the TCAM table in the PRD soft stage
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM
 * \param [in] entry_info - info of the TCAM entry
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
imb_feu_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_hw_set(unit, key_index, entry_info));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get an entry from the PRD soft stage TCAM table
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] key_index - index in the TCAM table
 * \param [in] entry_info - returned entry info.
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
imb_feu_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_hw_get(unit, key_index, entry_info));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set control frame properties to be recognized by the
 *        PRD. if a control packet is recognized (each packet is
 *        compared against all control planes defined for the
 *        ETHU), it gets the highest priority
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        configure (0-3)
 * \param [in] control_frame_config - control frame properties
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
imb_feu_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_control_frame_hw_set(unit, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get Control frame properties recognized by the PRD
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] control_frame_index - index of control frame to
 *        get
 * \param [out] control_frame_config - returned control frame
 *        properties
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
imb_feu_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_control_frame_hw_get(unit, control_frame_index, control_frame_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - set MPLS special label properties. the special
 *        labels are compared globally against all the MPLS ports
 *        of the same ETHU.
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to set (0-3)
 * \param [in] label_config - special label configuration.
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
imb_feu_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_mpls_special_label_hw_set(unit, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get MPLS special label properties
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] label_index - label index to get (0-3)
 * \param [out] label_config - label properties
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_prd_mpls_special_label_hw_get(unit, label_index, label_config));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - get port link status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] flags - flags
 * \param [out] link - port link status
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_link_get(
    int unit,
    bcm_port_t port,
    int flags,
    int *link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_FIFO_STATUS,
                                    DBAL_FIELD_LINK_STATUS, (uint32 *) link));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port local fault
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] local_fault - local fault
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_local_fault_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault)
{
    int client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_STATUS,
                                        DBAL_FIELD_LOCAL_FAULT_LIVE_STATUS, (uint32 *) local_fault));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(flexe_core_sar_client_local_fault_get(unit, client_channel, local_fault));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_core_client_local_fault_get(unit, client_channel, local_fault));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear port local fault
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_local_fault_status_clear(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port remote fault
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] remote_fault - remote fault
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_remote_fault_status_get(
    int unit,
    bcm_port_t port,
    int *remote_fault)
{
    int client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &client_channel));

    if (DNX_ALGO_PORT_TYPE_IS_FLEXE_MAC(unit, port_info, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_feu_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_STATUS,
                                        DBAL_FIELD_REMOTE_FAULT_LIVE_STATUS, (uint32 *) remote_fault));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FLEXE_SAR(unit, port_info))
    {
        SHR_IF_ERR_EXIT(flexe_core_sar_client_remote_fault_get(unit, client_channel, remote_fault));
    }
    else
    {
        SHR_IF_ERR_EXIT(flexe_core_client_remote_fault_get(unit, client_channel, remote_fault));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear port remote fault
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 *
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_feu_port_remote_fault_status_clear(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}
