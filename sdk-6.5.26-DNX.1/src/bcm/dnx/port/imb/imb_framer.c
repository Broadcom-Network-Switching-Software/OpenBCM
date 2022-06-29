/** \file imb_framer.c
 *
 *  FlexE client procedures for DNX.
 *
 */
/*
 *
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 */
#include <soc/sand/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/imb/imb_dispatch.h>
#include <bcm_int/dnx/port/imb/imb_feu.h>
#include <bcm_int/dnx/port/imb/imb_common.h>
#include <bcm_int/dnx/port/imb/imb_feu.h>
#include <bcm_int/dnx/port/imb/imb_framer.h>
#include <bcm_int/dnx/port/port_flexe.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_calendar.h>
#include <bcm_int/dnx/algo/flexe_general/algo_flexe_general.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_imb.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm/types.h>
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/dnx_algo_port_access.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_arb.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr_prd.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_oft.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>

#include <sal/core/boot.h>

#include <soc/util.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/recovery/generic_state_journal.h>

#include <src/bcm/dnx/stat/mib/mib_stat_dbal.h>

#include "imb_framer_internal.h"
#include "imb_ethu_internal.h"
#include "../nif/dnx_port_nif_arb_internal.h"
#include "../nif/dnx_port_nif_oft_internal.h"

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

#define IMB_FRAMER_MAC_DRAIN_CELL_TIMEOUT_USEC        (250000)
#define IMB_FRAMER_MAC_RSV_MASK_ETH_TYPE_CHECK_BIT    (5)
#define IMB_FRAMER_MAC_SPEED_NOF_FIELDS               (2)
#define IMB_FRAMER_FEU_IS_VALID(unit)                 (dnx_data_nif.flexe.feu_nof_get(unit) > 0)
/*
 * Framer Tiny MAC reset mode
 */
typedef enum
{
    IMB_FRAMER_MAC_SOFT_RESET_MODE_IN_OUT = 0,
    IMB_FRAMER_MAC_SOFT_RESET_MODE_IN = 1,
    IMB_FRAMER_MAC_SOFT_RESET_MODE_OUT = 2
} imb_framer_mac_soft_reset_mode_e;
/*
 * Framer Tiny MAC Tx Fault source
 */
typedef enum
{
    /*
     * The fault is according to the Rx state machine
     */
    IMB_FRAMER_MAC_TX_FAULT_SOURCE_RX_FAULT_SM = 0,
    /*
     * The fault is according to the external input
     */
    IMB_FRAMER_MAC_TX_FAULT_SOURCE_EXT_INPUT,
    /*
     * The fault is according to the SW configuration
     */
    IMB_FRAMER_MAC_TX_FAULT_SOURCE_SW_CFG
} imb_framer_mac_tx_fault_source_e;

/*
 * Framer Tiny MAC Ctrl info
 */
typedef struct imb_framer_mac_ctrl_s
{
    uint32 rx_enable;
    uint32 tx_enable;
    uint32 soft_reset;
} imb_framer_mac_ctrl_t;
/*
 * FEU Tiny MAC drain cell info
 */
typedef struct imb_framer_mac_drain_cells_s
{
    uint32 rx_pfc_en;
    uint32 llfc_en;
    uint32 rx_pause;
    uint32 tx_pause;
} imb_framer_mac_drain_cells_t;

static int imb_framer_mac_get(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 *val);

static int imb_framer_mac_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 val);

static int imb_framer_mac_multi_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e * field,
    uint32 *val,
    int nof_fields);

static int imb_framer_mac_polling(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 expected_val);

static int imb_framer_nof_master_ports_get(
    int unit,
    int *nof_ports);

/**
 * \brief - Convert speed to ep_credit:
 *
 * \param [in] unit - chip unit id
 * \param [in] speed - speed
 * \param [out] speed_group - speed group for different speed
 * \param [out] ep_credit - credit to EP
 * \param [out] tx_threshold - Tx threshold
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
imb_framer_utils_mac_tx_speed_config_get(
    int unit,
    int speed,
    uint32 *speed_group,
    uint32 *ep_credit,
    uint32 *tx_threshold)
{
    int idx;
    SHR_FUNC_INIT_VARS(unit);

    for (idx = dnx_data_nif.mac_client.speed_tx_property_table_info_get(unit)->key_size[0] - 1; idx >= 0; idx--)
    {
        if (speed >= dnx_data_nif.mac_client.speed_tx_property_table_get(unit, idx)->speed)
        {
            *speed_group = dnx_data_nif.mac_client.speed_tx_property_table_get(unit, idx)->speed_group;
            *ep_credit = dnx_data_nif.mac_client.speed_tx_property_table_get(unit, idx)->num_credit;
            *tx_threshold = dnx_data_nif.mac_client.speed_tx_property_table_get(unit, idx)->tx_threshold;
            break;
        }
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - Initialize IMB framer:
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
imb_framer_init(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        if (IMB_FRAMER_FEU_IS_VALID(unit))
        {
            SHR_IF_ERR_EXIT(imb_feu_init(unit, imb_info));
        }
        if (dnx_data_nif.wbc.feature_get(unit, dnx_data_nif_wbc_is_used))
        {
            if (dnx_data_nif.framer.feature_get(unit, dnx_data_nif_framer_is_clock_independent))
            {
                /*
                 * Set Tiny MAC out of reset
                 */
                SHR_IF_ERR_EXIT(imb_framer_internal_tiny_mac_reset(unit, IMB_COMMON_IN_RESET));
                SHR_IF_ERR_EXIT(imb_framer_internal_tiny_mac_reset(unit, IMB_COMMON_OUT_OF_RESET));
            }
            if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
            {
                for (client_channel = 0; client_channel < dnx_data_nif.sar_client.nof_clients_get(unit);
                     ++client_channel)
                {
                    SHR_IF_ERR_EXIT(imb_framer_internal_sar_to_egq_mapping_set(unit, client_channel, 0));
                    SHR_IF_ERR_EXIT(imb_framer_internal_egq_to_sar_mapping_set(unit, client_channel, 0));
                }
            }
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
imb_framer_port_max_packet_size_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RX_MAX_SIZE, DBAL_FIELD_RX_MAX_SIZE, value));
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
imb_framer_port_crc_mode_set(
    int unit,
    bcm_port_t port,
    int mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_CRC_MODE, mode));
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
imb_framer_port_max_packet_size_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get
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
imb_framer_port_pad_size_set(
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
    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_EN, enable));
    if (enable)
    {
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_THRESHOLD, value));
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
imb_framer_port_pad_size_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    uint32 enable;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_PAD_EN, &enable));
    if (!enable)
    {
        *value = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL,
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
imb_framer_port_cntmaxsize_set(
    int unit,
    bcm_port_t port,
    int value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_MAX_SIZE, value));
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
imb_framer_port_cntmaxsize_get(
    int unit,
    bcm_port_t port,
    int *value)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(imb_framer_mac_get
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
imb_framer_port_mib_counter_control_set(
    int unit,
    bcm_port_t port,
    int enable,
    int clear)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_ENABLE, enable));
    /*
     * Enable and Clear the MAC MIB counters
     * No need to explicitly set the clear flag to 0 since the
     * function internally unsets the clear flag
     */
    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_CLEAR, clear));
    if (clear)
    {
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_MIB_CTRL, DBAL_FIELD_COUNT_CLEAR, 0));
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
imb_framer_port_mib_counter_get(
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

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_counter_get(unit, field_id, counter_val));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Rollback for imb_feu_internal_mac_access_control_set
 */
static shr_error_e
imb_framer_internal_mac_access_control_set_er_rollback(
    int unit,
    dnx_er_db_t * dnx_er_db,
    uint8 *metadata,
    uint8 *payload,
    char *stamp)
{
    int *port = NULL;

    SHR_FUNC_INIT_VARS(unit);

    port = (int *) metadata;

    SHR_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, *port));
exit:
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
imb_framer_port_mib_counter_set(
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

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_counter_set(unit, field_id, counter_val));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_framer_internal_mac_access_control_set_er_rollback,
                                                        FALSE));

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
imb_framer_port_mac_ctrl_soft_reset_set(
    int unit,
    bcm_port_t port,
    uint32 in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_SOFT_RESET, in_reset));
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
imb_framer_port_mac_ctrl_soft_reset_get(
    int unit,
    bcm_port_t port,
    uint32 *in_reset)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_SOFT_RESET, in_reset));
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
imb_framer_port_mac_rx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_RX_EN, enable));
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
imb_framer_port_mac_rx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_RX_EN, enable));
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
imb_framer_port_mac_tx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_TX_EN, enable));
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
imb_framer_port_mac_tx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_CTRL, DBAL_FIELD_TX_EN, enable));
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
imb_framer_port_mac_tx_fault_src_set(
    int unit,
    bcm_port_t port,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_FAULT_SOURCE_FOR_TX, val));
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
imb_framer_port_mac_rsv_mask_set(
    int unit,
    bcm_port_t port,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RSV_MASK, DBAL_FIELD_MASK, val));
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
imb_framer_port_mac_rsv_mask_get(
    int unit,
    bcm_port_t port,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RSV_MASK, DBAL_FIELD_MASK, val));
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
imb_framer_port_mac_tx_pause_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_TX_PAUSE_EN, enable));
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
imb_framer_port_mac_tx_pause_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_TX_PAUSE_EN, enable));
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
imb_framer_port_mac_rx_pause_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_RX_PAUSE_EN, enable));
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
imb_framer_port_mac_rx_pause_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PAUSE_CTRL, DBAL_FIELD_RX_PAUSE_EN, enable));
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
imb_framer_port_mac_rx_pfc_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_PFC_CTRL, DBAL_FIELD_RX_PFC_EN, enable));
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
imb_framer_port_mac_rx_pfc_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_PFC_CTRL, DBAL_FIELD_RX_PFC_EN, enable));
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
imb_framer_port_mac_tx_discard_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_DISCARD_EN, enable));
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
imb_framer_port_mac_txfifo_cell_cnt_get(
    int unit,
    bcm_port_t port,
    uint32 *cell_cnt)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_TXFIFO_STATUS, DBAL_FIELD_CELL_CNT, cell_cnt));

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
imb_framer_port_mac_egress_queue_drain_get(
    int unit,
    bcm_port_t port,
    imb_framer_mac_ctrl_t * mac_ctrl)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_get(unit, port, &mac_ctrl->rx_enable));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_enable_get(unit, port, &mac_ctrl->tx_enable));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_get(unit, port, &mac_ctrl->soft_reset));
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
imb_framer_port_mac_drain_cell_get(
    int unit,
    bcm_port_t port,
    imb_framer_mac_drain_cells_t * drain_cells)
{
    SHR_FUNC_INIT_VARS(unit);
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_pfc_pause_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pfc_enable_get(unit, port, &drain_cells->rx_pfc_en));
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pause_enable_get(unit, port, &drain_cells->rx_pause));
        SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_pause_enable_get(unit, port, &drain_cells->tx_pause));
    }
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
imb_framer_port_mac_drain_cell_start(
    int unit,
    bcm_port_t port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Don't disable TX since it stops egress and hangs if CPU sends
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_enable_set(unit, port, 1));
    /*
     * Disable RX
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_set(unit, port, 0));
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_pfc_pause_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pause_enable_set(unit, port, 0));
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pfc_enable_set(unit, port, 0));
    }
    SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, 1));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_discard_set(unit, port, 1));
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
imb_framer_port_mac_drain_cell_stop(
    int unit,
    bcm_port_t port,
    const imb_framer_mac_drain_cells_t * drain_cells)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clear Discard fields
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_discard_set(unit, port, 0));
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_pfc_pause_supported))
    {
        /*
         * set pause fields
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pause_enable_set(unit, port, drain_cells->rx_pause));
        SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_pause_enable_set(unit, port, drain_cells->tx_pause));
        /*
         * set pfc rx_en fields
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_pfc_enable_set(unit, port, drain_cells->rx_pfc_en));
    }
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
imb_framer_port_mac_egress_queue_drain_rx_enable(
    int unit,
    bcm_port_t port,
    uint32 rx_en)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Enable RX, de-assert SOFT_RESET
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_set(unit, port, rx_en ? 1 : 0));
    /*
     * Bring mac out of reset
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, 0));

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
imb_framer_port_mac_ctrl_recover(
    int unit,
    bcm_port_t port,
    imb_framer_mac_ctrl_t * mac_ctrl)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_set(unit, port, mac_ctrl->rx_enable));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_enable_set(unit, port, mac_ctrl->tx_enable));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, mac_ctrl->soft_reset));

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
imb_framer_port_mac_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Don't disable TX since it stops egress and hangs if CPU sends
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_enable_set(unit, port, 1));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_set(unit, port, enable ? 1 : 0));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, enable ? 0 : 1));

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
imb_framer_port_mac_soft_reset_set(
    int unit,
    bcm_port_t port,
    imb_framer_mac_soft_reset_mode_e reset_mode,
    imb_framer_mac_ctrl_t * saved_mac_ctrl)
{
    uint32 cell_count;
    imb_framer_mac_drain_cells_t drain_cells;
    imb_framer_mac_ctrl_t mac_ctrl;
    soc_timeout_t to;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&drain_cells, 0, sizeof(drain_cells));
    /*
     * portmodCallBackActionTypePre
     */
    if (reset_mode != IMB_FRAMER_MAC_SOFT_RESET_MODE_OUT)
    {
        SHR_IF_ERR_EXIT(imb_framer_port_mac_egress_queue_drain_get(unit, port, &mac_ctrl));
        /*
         * Store the original MAC control info.
         */
        if (saved_mac_ctrl != NULL)
        {
            sal_memcpy(saved_mac_ctrl, &mac_ctrl, sizeof(imb_framer_mac_ctrl_t));
        }
        /*
         * Drain cells
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_drain_cell_get(unit, port, &drain_cells));
        /*
         * Start TX FIFO draining
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_drain_cell_start(unit, port));
        /*
         * De-assert SOFT_RESET to let the drain start
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, 0));

        if (!SAL_BOOT_PLISIM)
        {
            /*
             * Wait until TX fifo cell count is 0
             */
            soc_timeout_init(&to, IMB_FRAMER_MAC_DRAIN_CELL_TIMEOUT_USEC, 0);
            for (;;)
            {
                SHR_IF_ERR_EXIT(imb_framer_port_mac_txfifo_cell_cnt_get(unit, port, &cell_count));
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
            SHR_IF_ERR_EXIT(imb_framer_port_mac_txfifo_cell_cnt_get(unit, port, &cell_count));
        }

        /*
         * Stop TX FIFO draining
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_drain_cell_stop(unit, port, &drain_cells));

        /*
         * Put port into SOFT_RESET
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_soft_reset_set(unit, port, 1));
    }

    if (reset_mode != IMB_FRAMER_MAC_SOFT_RESET_MODE_IN)
    {
        /*
         * portmodCallBackActionTypeDuring
         */
        if (IMB_FRAMER_FEU_IS_VALID(unit))
        {
            SHR_IF_ERR_EXIT(imb_feu_port_credit_tx_reset(unit, port));
        }
        /*
         * For out of reset operation, mac control should be taken from input
         */
        if (reset_mode == IMB_FRAMER_MAC_SOFT_RESET_MODE_OUT)
        {
            SHR_NULL_CHECK(saved_mac_ctrl, _SHR_E_PARAM, "saved_mac_ctrl");
            sal_memcpy(&mac_ctrl, saved_mac_ctrl, sizeof(imb_framer_mac_ctrl_t));
        }
        /*
         * Enable TX, restore RX, de-assert SOFT_RESET
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_egress_queue_drain_rx_enable(unit, port, mac_ctrl.rx_enable));
        /*
         * Restore clmac_CTRL to original value, make sure the mac is not in soft reset status
         */
        mac_ctrl.soft_reset = 0;
        SHR_IF_ERR_EXIT(imb_framer_port_mac_ctrl_recover(unit, port, &mac_ctrl));
        /*
         * portmodCallBackActionTypePost
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
imb_framer_port_tx_average_ipg_set(
    int unit,
    bcm_port_t port,
    int ipg_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set
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
imb_framer_port_tx_average_ipg_get(
    int unit,
    bcm_port_t port,
    int *ipg_value)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get
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
imb_framer_port_mac_tx_thr_hw_set(
    int unit,
    bcm_port_t port,
    int tx_thr)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_TX_CTRL, DBAL_FIELD_TX_THRESHOLD, tx_thr));
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
imb_framer_mac_init(
    int unit,
    bcm_port_t port)
{
    int client_channel, average_ipg_in_bits, tx_thr, pad_size;
    uint32 rsv_mask[1];
    dnx_algo_port_info_s port_info;
    imb_framer_mac_tx_fault_source_e tx_fault_src;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * CRC append mode
     */
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_tx_crcmode_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_crc_mode_set(unit, port, 0));
    }
    /*
     * Configure tx_threshold in Tiny MAC
     */
    tx_thr = dnx_data_nif.mac_client.mac_tx_threshold_get(unit);
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_thr_hw_set(unit, port, tx_thr));
    /*
     * Enable padding
     */
    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_tx_pading_supported))
    {
        pad_size = dnx_data_nif.mac_client.mac_pad_size_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_port_pad_size_set(unit, port, pad_size));
    }
    /*
     * Config average IPG for L1 ports
     */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit, port_info))
    {
        average_ipg_in_bits = dnx_data_nif.mac_client.average_ipg_for_l1_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_port_tx_average_ipg_set(unit, port, average_ipg_in_bits));
    }
    if (DNX_ALGO_FLEXE_MAC_CHANNEL_IS_PTP_OR_OAM(unit, client_channel))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_max_packet_size_set(unit, port, 512));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_port_max_packet_size_set(unit, port, 10240));
        SHR_IF_ERR_EXIT(imb_framer_port_mib_counter_control_set(unit, port, 1, 1));
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rsv_mask_get(unit, port, rsv_mask));
        SHR_BITCLR(rsv_mask, IMB_FRAMER_MAC_RSV_MASK_ETH_TYPE_CHECK_BIT);
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rsv_mask_set(unit, port, rsv_mask[0]));
        /*
         * L1 switching includes the LF/RF indication, so
         * the Tx fault source should be from L1 ETH port.
         * The fault should be terminated for L2/L3 switching
         */
        tx_fault_src =
            DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit,
                                                port_info) ? IMB_FRAMER_MAC_TX_FAULT_SOURCE_EXT_INPUT :
            IMB_FRAMER_MAC_TX_FAULT_SOURCE_RX_FAULT_SM;
        SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_fault_src_set(unit, port, tx_fault_src));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable or disable port in shared resources,
 *    arbiter, ofr and oft
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
imb_framer_port_shr_resource_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    /*
     * Enable port in Arb. Rx
     */
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, FALSE, FALSE, FALSE))
    {
        /*
         * Enable port in OFT and resetting credits
         */
        if (enable)
        {
            SHR_IF_ERR_EXIT(dnx_port_oft_credits_counter_reset(unit, port, IMB_COMMON_IN_RESET));
        }
        SHR_IF_ERR_EXIT(dnx_port_oft_port_enable_set(unit, port, enable));
        /*
         * Enable port in Arb. Rx
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_port_enable(unit, port, ARB_RX_DIRECTION, enable));
        /*
         * Enable port in OFR
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_enable(unit, port, enable));
        /*
         * Enable port in Arb. TX
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_port_enable(unit, port, ARB_TX_DIRECTION, enable));
        /*
         * Enable credits in Arb. Tx
         */
        SHR_IF_ERR_EXIT(dnx_port_arb_tmac_credits_set(unit, port, enable));
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
imb_framer_port_enable_set(
    int unit,
    bcm_port_t port,
    uint32 flags,
    int enable)
{
    dnx_algo_port_info_s port_info;
    int client_channel, has_tx_speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE) &&
        !(DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE)
          && dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port)))
    {
        SHR_EXIT();
    }

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));

        if (enable)
        {
            if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_enc_dec_configurable))
            {
                SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_enc_reset_set
                                (unit, client_channel, IMB_COMMON_OUT_OF_RESET));
                SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_dec_reset_set
                                (unit, client_channel, IMB_COMMON_OUT_OF_RESET));
            }
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_tx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
            if (IMB_FRAMER_FEU_IS_VALID(unit))
            {
                /*
                 * Disable the FEU Tx flush
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_er_tx_egress_flush_set(unit, port, 0));
            }
            /*
             * Configure TXI logic and IRDY threshold
             */
            if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
                && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, TRUE));
            }
            if (IMB_FRAMER_FEU_IS_VALID(unit))
            {
                SHR_IF_ERR_EXIT(imb_feu_port_enable_set(unit, port, flags, enable));
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_framer_port_shr_resource_enable_set(unit, port, enable));
            }
            /*
             * Wait for NIF to finish transmitting initial credits
             */
            sal_usleep(10);
            /*
             * EGQ credit set
             */
            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info) && has_tx_speed)
            {
                SHR_IF_ERR_EXIT(dnx_egr_tm_port_credit_init(unit, port));
            }
            SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_enable_set(unit, port, enable));
            SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_set(unit, port, enable));
            SHR_IF_ERR_EXIT(imb_framer_port_mac_soft_reset_set
                            (unit, port, IMB_FRAMER_MAC_SOFT_RESET_MODE_IN_OUT, NULL));
            /*
             * Enable Qpairs, if there is no Tx speed, keep Qpair disabled.
             */
            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info) && has_tx_speed)
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, FALSE));
            }
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
        }
        else
        {
            /*
             * disable Qpairs AND flush the buffer
             */
            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, enable, TRUE));
            }
            if (IMB_FRAMER_FEU_IS_VALID(unit))
            {
                /*
                 * flush the egress
                 */
                SHR_IF_ERR_EXIT(imb_feu_port_er_tx_egress_flush_set(unit, port, 1));
            }
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));

            if (IMB_FRAMER_FEU_IS_VALID(unit))
            {
                SHR_IF_ERR_EXIT(imb_feu_port_enable_set(unit, port, flags, enable));
            }
            else
            {
                SHR_IF_ERR_EXIT(imb_framer_port_shr_resource_enable_set(unit, port, enable));
            }
            SHR_IF_ERR_EXIT(imb_framer_port_mac_enable_set(unit, port, 0));
            /*
             * Configure TXI logic and IRDY threshold
             */
            if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support)
                && DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info))
            {
                DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(dnx_esb_port_txi_config_set(unit, port, FALSE));
            }
            SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_tx_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
            if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_enc_dec_configurable))
            {
                SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_enc_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
                SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_dec_reset_set(unit, client_channel, IMB_COMMON_IN_RESET));
            }
        }
    }
    else
    {
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_sar_enable_set
                                                   (unit, client_channel, BCM_PORT_FLEXE_RX, enable));
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
imb_framer_port_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    int client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    *enable = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE) &&
        !(DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE)
          && dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port)))
    {
        SHR_EXIT();
    }
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_mac_rx_enable_get(unit, port, (uint32 *) enable));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
        SHR_IF_ERR_EXIT(flexe_core_sar_enable_get(unit, client_channel, BCM_PORT_FLEXE_RX, enable));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Port speed configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - speed
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
imb_framer_port_mac_speed_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    uint32 dummy, val[IMB_FRAMER_MAC_SPEED_NOF_FIELDS];
    dbal_fields_e field[IMB_FRAMER_MAC_SPEED_NOF_FIELDS] = { DBAL_FIELD_SPEED_GROUP, DBAL_FIELD_NUMB_CREDIT };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_utils_mac_tx_speed_config_get(unit, speed, &val[0], &val[1], &dummy));
    SHR_IF_ERR_EXIT(imb_framer_mac_multi_set
                    (unit, port, DBAL_TABLE_TINY_MAC_CTRL, field, val, IMB_FRAMER_MAC_SPEED_NOF_FIELDS));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure Tx threshold as per
 *    the port speed
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - speed
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
imb_framer_port_mac_tx_threshold_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    uint32 dummy1, dummy2, tx_thr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_utils_mac_tx_speed_config_get(unit, speed, &dummy1, &dummy2, &tx_thr));
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_thr_hw_set(unit, port, tx_thr));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Port speed configuration
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed_config - speed config
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
imb_framer_port_speed_config_set(
    int unit,
    bcm_port_t port,
    const portmod_speed_config_t * speed_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_is_speed_supported))
    {
        /*
         * Program the "speed group" register (FLEXMAC2_CTRL.NUMB_CREDIT)
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_speed_set(unit, port, speed_config->speed));
        /*
         * Program tx threshold
         */
        SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_threshold_set(unit, port, speed_config->speed));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Port speed configuration for G.hao
 *    Only relevant for Tx direction
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - speed config
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
imb_framer_port_ghao_speed_config_set(
    int unit,
    bcm_port_t port,
    int speed)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Program the "speed group" register (FLEXMAC2_CTRL.NUMB_CREDIT)
     */
    SHR_IF_ERR_EXIT(imb_framer_port_mac_speed_set(unit, port, speed));
    /*
     * Polling the "Done" status
     */
    SHR_IF_ERR_EXIT(imb_framer_mac_polling(unit, -1, DBAL_TABLE_TINY_MAC_TXFIFO_STATUS, DBAL_FIELD_IS_SWITCH_DONE, 1));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure port mapping between EGQ and SAR in WC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable the mapping
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
imb_framer_sar_egq_mapping_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int sar_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &sar_channel));
    /*
     * Configure HW
     */
    SHR_IF_ERR_EXIT(imb_framer_internal_sar_to_egq_mapping_set(unit, sar_channel, enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_egq_to_sar_mapping_set(unit, sar_channel, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/Disable the ENC/DEC global data path
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
static int
imb_framer_enc_dec_path_enable_set(
    int unit,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_internal_enc_datapath_reset_set(unit, !enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_dec_datapath_reset_set(unit, !enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_dec_ready_set(unit, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/Disable ENC-DEC control blocks,
 *    including LF/RF/LPI/LI
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
imb_framer_enc_dec_control_blocks_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(imb_framer_internal_enc_control_blocks_enable_set(unit, client_channel, enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_dec_control_blocks_enable_set(unit, client_channel, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable/Disable ENC-DEC cl82
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
imb_framer_enc_dec_cl82_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    int client_channel;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
    SHR_IF_ERR_EXIT(imb_framer_internal_enc_cl82_enable_set(unit, client_channel, enable));
    SHR_IF_ERR_EXIT(imb_framer_internal_dec_cl82_enable_set(unit, client_channel, enable));
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
imb_framer_port_attach(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    int nof_master_ports;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        if (IMB_FRAMER_FEU_IS_VALID(unit))
        {
            /*
             * Attach port in FEU if FEU is valid
             */
            SHR_IF_ERR_EXIT(imb_feu_port_attach(unit, port, flags));
        }
        else
        {
            if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_STAT(unit, port_info)
                || DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_PROTECTION(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_port_arb_disable_datapath_set(unit, port, TRUE));
            }
            else
            {
                /*
                 * Configure Arbiter, OFR and OFT
                 */
                SHR_IF_ERR_EXIT(dnx_port_ofr_port_add(unit, port));
                SHR_IF_ERR_EXIT(dnx_port_oft_port_add(unit, port));
                SHR_IF_ERR_EXIT(dnx_port_arb_port_add(unit, port));
            }
        }
        /*
         * Configure Tiny Mac
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_init(unit, port));
        /*
         * Configure ENC_DEC
         */
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_enc_dec_configurable))
        {
            SHR_IF_ERR_EXIT(imb_framer_enc_dec_control_blocks_enable_set(unit, port, 1));
            SHR_IF_ERR_EXIT(imb_framer_enc_dec_cl82_enable_set(unit, port, 1));
            SHR_IF_ERR_EXIT(imb_framer_nof_master_ports_get(unit, &nof_master_ports));
            /*
             * Open the ENC DEC data path
             */
            if (nof_master_ports == 1)
            {
                SHR_IF_ERR_EXIT(imb_framer_enc_dec_path_enable_set(unit, 1));
            }
        }
        /*
         * Disable the port
         */
        SHR_IF_ERR_EXIT(imb_framer_port_enable_set(unit, port, 0, 0));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, TRUE)
             && dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_add(unit, port));
        /*
         * Configure SAR <-> EGQ port mapping
         */
        SHR_IF_ERR_EXIT(imb_framer_sar_egq_mapping_set(unit, port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - set the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] speed - port interface rate
 * \param [in] start_tx_thr - Start tx threshold
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
imb_framer_port_tx_start_thr_set(
    int unit,
    bcm_port_t port,
    int speed,
    int start_tx_thr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_tx_start_thr_set(unit, port, speed, start_tx_thr));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get the TX start threshold for the port
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] start_tx_thr - Start tx threshold
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
imb_framer_port_tx_start_thr_get(
    int unit,
    bcm_port_t port,
    int *start_tx_thr)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_tx_start_thr_get(unit, port, start_tx_thr));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for SB RX
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - Indicate calendar actions
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
imb_framer_sb_rx_calendar_set(
    int unit,
    uint32 flags)
{
    int speed, client_channel, has_rx_speed;
    int max_cal_len, max_nof_clients;
    uint32 *slots_per_client = NULL;
    uint32 *calendar = NULL;
    int cal_to_config = -1;
    bcm_pbmp_t logical_ports;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure shadow calendar
     */
    if (IMB_FLEXE_RESOURCE_PREPARE_GET(flags))
    {
        /*
         * Allcoate space
         */
        SHR_ALLOC_SET_ZERO(slots_per_client, dnx_data_nif.mac_client.nof_clients_get(unit) * sizeof(uint32),
                           "slots_per_client", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(calendar, dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit) * sizeof(uint32), "calendar",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        /*
         * Get all the FlexE MAC clients
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                         DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
        _SHR_PBMP_ITER(logical_ports, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &has_rx_speed));
            if (!has_rx_speed)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, DNX_ALGO_PORT_SPEED_F_RX, &speed));
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &client_channel));

            slots_per_client[client_channel] =
                UTILEX_MAX((speed / dnx_data_nif.mac_client.sb_cal_granularity_get(unit)), 1);
            /*
             * Always enable OAM client
             */
            if (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0)
            {
                slots_per_client[dnx_data_nif.mac_client.oam_client_channel_get(unit)] = 1;
            }
        }

        /** Build uniform calendar */
        max_nof_clients = dnx_data_nif.mac_client.nof_clients_get(unit);
        max_cal_len = dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit);
        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                        (unit, slots_per_client, max_nof_clients, max_cal_len, calendar));

        SHR_IF_ERR_EXIT(imb_framer_internal_shadow_calendar_config
                        (unit, IMB_FRAMER_SB_RX_CALENDAR, max_cal_len, calendar, &cal_to_config));
    }
    /*
     * Switch calendar
     */
    if (IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_switch(unit, IMB_FRAMER_SB_RX_CALENDAR, cal_to_config));
    }
exit:
    SHR_FREE(slots_per_client);
    SHR_FREE(calendar);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure uniform calendar for SB TX
 *
 * \param [in] unit - chip unit id
 * \param [in] flags - Indicate calendar actions
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
imb_framer_sb_tx_calendar_set(
    int unit,
    uint32 flags)
{
    int speed, client_channel, has_tx_speed;
    int max_cal_len, nof_flexe_clients = 0, nof_special_clients;
    uint32 *slots_per_client = NULL;
    uint32 *calendar = NULL;
    int cal_to_config = -1;
    int oam_channel_is_active = 0, ptp_channel_is_active = 0;
    bcm_pbmp_t logical_ports, active_client_channel;
    bcm_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure shadow calendar
     */
    if (IMB_FLEXE_RESOURCE_PREPARE_GET(flags))
    {
        /*
         * Allcoate space
         */
        SHR_ALLOC_SET_ZERO(slots_per_client, dnx_data_nif.mac_client.nof_clients_get(unit) * sizeof(uint32),
                           "slots_per_client", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(calendar, dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit) * sizeof(uint32), "calendar",
                           "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        /*
         * Get all the FlexE MAC clients
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC,
                         DNX_ALGO_PORT_LOGICALS_F_EXCLUDE_REMOVED_PORT, &logical_ports));
        _SHR_PBMP_CLEAR(active_client_channel);
        _SHR_PBMP_ITER(logical_ports, logical_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, DNX_ALGO_PORT_SPEED_F_TX, &has_tx_speed));
            if (!has_tx_speed)
            {
                continue;
            }
            SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &client_channel));
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
                slots_per_client[client_channel] = speed / dnx_data_nif.mac_client.sb_cal_granularity_get(unit);
            }
            _SHR_PBMP_PORT_ADD(active_client_channel, client_channel);
        }
        /*
         * Need to disable the credit counter for inactive client to avoid credit overflow
         */
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported))
        {
            for (client_channel = 0; client_channel < dnx_data_nif.mac_client.nof_clients_get(unit); ++client_channel)
            {
                if (!_SHR_PBMP_MEMBER(active_client_channel, client_channel))
                {
                    SHR_IF_ERR_EXIT(imb_framer_internal_tx_credit_counter_enable_set(unit, client_channel, 0));
                }
            }
        }
        /** Build uniform calendar */
        nof_flexe_clients = dnx_data_nif.mac_client.nof_normal_clients_get(unit);
        nof_special_clients = dnx_data_nif.mac_client.nof_special_clients_get(unit);
        max_cal_len =
            (nof_special_clients >
             0) ? dnx_data_nif.mac_client.nof_normal_clients_get(unit) : dnx_data_nif.
            mac_client.nof_sb_cal_slots_get(unit);

        SHR_IF_ERR_EXIT(dnx_algo_flexe_general_cal_build
                        (unit, slots_per_client, nof_flexe_clients, max_cal_len, calendar));
        /*
         * For SB tx, put the slots for channel 80 and 81 at the last if there are OAM and PTP clients.
         */
        if (nof_special_clients > 0)
        {
            calendar[dnx_data_nif.mac_client.oam_client_channel_get(unit)] =
                oam_channel_is_active ? dnx_data_nif.
                mac_client.oam_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
            calendar[dnx_data_nif.mac_client.ptp_client_channel_get(unit)] =
                ptp_channel_is_active ? dnx_data_nif.
                mac_client.ptp_client_channel_get(unit) : DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
        }
        /*
         * Set real Max calendar length
         */
        max_cal_len =
            (nof_special_clients >
             0) ? dnx_data_nif.mac_client.nof_clients_get(unit) : dnx_data_nif.mac_client.nof_sb_cal_slots_get(unit);
        SHR_IF_ERR_EXIT(imb_framer_internal_shadow_calendar_config
                        (unit, IMB_FRAMER_SB_TX_CALENDAR, max_cal_len, calendar, &cal_to_config));
    }
    /*
     * Switch calendar
     */
    if (IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_calendar_switch(unit, IMB_FRAMER_SB_TX_CALENDAR, cal_to_config));
        /*
         * Enable the SB Tx credit counter after the calendar is configured
         */
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_credit_cnt_en_supported))
        {
            _SHR_PBMP_ITER(active_client_channel, client_channel)
            {
                SHR_IF_ERR_EXIT(imb_framer_internal_tx_credit_counter_enable_set(unit, client_channel, 1));
            }
        }
    }
exit:
    SHR_FREE(slots_per_client);
    SHR_FREE(calendar);
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
imb_framer_calendar_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * If no specific calendar action is configured, set both actions for calendar.
     */
    if (!IMB_FLEXE_RESOURCE_PREPARE_GET(flags) && !IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
    {
        IMB_FLEXE_RESOURCE_PREPARE_SET(flags);
        IMB_FLEXE_RESOURCE_SWITCH_SET(flags);
    }

    if (IMB_FLEXE_DIRECTION_RX_GET(flags))
    {
        /*
         * SB RX calendar
         */
        SHR_IF_ERR_EXIT(imb_framer_sb_rx_calendar_set(unit, flags));
    }

    if (IMB_FLEXE_DIRECTION_TX_GET(flags))
    {
        /*
         * SB Tx calendar
         */
        SHR_IF_ERR_EXIT(imb_framer_sb_tx_calendar_set(unit, flags));
    }
    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_calendar_set(unit, flags));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure init credit for SB Tx fifo
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] speed - BusB client speed
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_framer_sb_tx_fifo_init_credit_set(
    int unit,
    int client_channel,
    int speed)
{
    int init_credit;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the init credit value
     */
    SHR_IF_ERR_EXIT(dnx_algo_framer_mac_tx_init_credit_value_get(unit, speed, &init_credit));
    SHR_IF_ERR_EXIT(imb_framer_internal_sb_tx_fifo_init_credit_set(unit, client_channel, init_credit));
exit:
    SHR_FUNC_EXIT;
}
static int
imb_framer_sb_fifo_link_list_force_config_set(
    int unit,
    int client_channel,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int speed,
    int orig_nof_entries,
    int new_nof_entries)
{
    int oam_channel, oam_speed;
    bcm_pbmp_t fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    if (orig_nof_entries != new_nof_entries)
    {
        if (orig_nof_entries != 0)
        {
            /*
             * if original speed is not zero
             */
            SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_force_config_set
                            (unit, client_channel, -1, 0, fifo_type, 0));
        }
        if (speed != 0)
        {
            /*
             * Config init credit
             */
            if (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_TX)
            {
                SHR_IF_ERR_EXIT(imb_framer_sb_tx_fifo_init_credit_set(unit, client_channel, speed));
            }
            /*
             * Allocate new SB fifo link list and enable it
             */
            SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_force_config_set
                            (unit, client_channel, -1, speed, fifo_type, 1));

        }
    }
    /*
     * Always enable OAM channel by default, which is used to active SB RX calendar
     */
    if (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0)
    {
        if (!DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel))
        {
            oam_channel = dnx_data_nif.mac_client.oam_client_channel_get(unit);
            SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get(unit, oam_channel, fifo_type, &fifo_entries));
            /*
             * Allocate the link list only when fifo_entries is NULL
             */
            if (_SHR_PBMP_IS_NULL(fifo_entries))
            {
                oam_speed = dnx_data_nif.framer.oam_channel_max_bandwidth_get(unit);
                SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_force_config_set
                                (unit, oam_channel, -1, oam_speed, fifo_type, 1));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}
static int
imb_framer_sb_fifo_link_list_ghao_config_set(
    int unit,
    int client_channel,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    int speed,
    int delta_val,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (delta_val != 0)
    {
        /*
         * Config init credit
         */
        if ((fifo_type == DNX_ALGO_FLEXE_FIFO_SB_TX) && IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
        {
            SHR_IF_ERR_EXIT(imb_framer_sb_tx_fifo_init_credit_set(unit, client_channel, speed));
        }
        /*
         * Allocate new SB fifo link list and enable it
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_ghao_config_set
                        (unit, client_channel, fifo_type, delta_val, flags));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure the FIFO resources for
 *    SB RX/TX FIFO
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fifo_type - SB fifo type
 * \param [in] flags - Indicate if the adjustment is for G.hao
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_framer_port_sb_fifo_resource_set(
    int unit,
    bcm_port_t port,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type,
    uint32 flags)
{
    int client_channel;
    int new_nof_entries, orig_nof_entries;
    int speed, has_speed;
    uint32 speed_flags = 0;
    bcm_pbmp_t fifo_entries;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    speed_flags = (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) ? DNX_ALGO_PORT_SPEED_F_RX : DNX_ALGO_PORT_SPEED_F_TX;
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, speed_flags, &has_speed));
    if (has_speed)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, speed_flags, &speed));
    }
    else
    {
        speed = 0;
    }
    /*
     * Get required Entries
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_entries_required_get(unit, fifo_type, speed, &new_nof_entries));
    /*
     * Get original nof entries
     */
    SHR_IF_ERR_EXIT(dnx_algo_flexe_fifo_link_list_get(unit, client_channel, fifo_type, &fifo_entries));
    _SHR_PBMP_COUNT(fifo_entries, orig_nof_entries);
    /*
     * Check if it is for G.hao operation
     */
    if (IMB_FLEXE_RESOURCE_PREPARE_GET(flags) || IMB_FLEXE_RESOURCE_SWITCH_GET(flags))
    {
        SHR_IF_ERR_EXIT(imb_framer_sb_fifo_link_list_ghao_config_set
                        (unit, client_channel, fifo_type, speed, (new_nof_entries - orig_nof_entries), flags));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_sb_fifo_link_list_force_config_set
                        (unit, client_channel, fifo_type, speed, orig_nof_entries, new_nof_entries));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure the FIFO resources for
 *    SB RX FIFO
 *    SB TX FIFO
 *    FEU TMC
 *    FEU RMC
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
imb_framer_port_fifo_resource_set(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FLEXE_DIRECTION_RX_GET(flags))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_sb_fifo_resource_set(unit, port, DNX_ALGO_FLEXE_FIFO_SB_RX, flags));
    }
    if (IMB_FLEXE_DIRECTION_TX_GET(flags)
        && dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_fifo_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_sb_fifo_resource_set(unit, port, DNX_ALGO_FLEXE_FIFO_SB_TX, flags));
    }
    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_fifo_resource_set(unit, port, flags));
    }

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
imb_framer_bandwidth_get(
    int unit,
    bcm_port_t port,
    int is_port_include,
    int *bandwidth)
{
    bcm_pbmp_t feu_ports;
    bcm_port_t port_i;
    int speed, has_speed;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_master_ports_get(unit, &feu_ports));

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
 * \brief - Configure Framer port scheduler
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
imb_framer_port_scheduler_config_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_scheduler_config_set(unit, port, enable));
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
imb_framer_port_priority_config_set(
    int unit,
    bcm_port_t port,
    const bcm_port_prio_config_t * priority_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_priority_config_set(unit, port, priority_config));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_set(unit, port, priority_config));
    }
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
imb_framer_port_priority_config_get(
    int unit,
    bcm_port_t port,
    bcm_port_prio_config_t * priority_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_port_priority_config_get(unit, port, priority_config));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_port_ofr_port_priority_config_get(unit, port, priority_config));
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
imb_framer_mac_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 val)
{
    dbal_fields_e field_tmp[1];
    uint32 val_tmp[1];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));

    field_tmp[0] = field;
    val_tmp[0] = val;
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_set(unit, table, field_tmp, val_tmp, 1));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_framer_internal_mac_access_control_set_er_rollback,
                                                        FALSE));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}
/**
 * \brief - Polling the framer MAC and check
 *    if the value is expected within the polling
 *    loop
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] table - Tiny MAC table
 * \param [in] field - Tiny MAC field
 * \param [in] expected_val - the expected value
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
imb_framer_mac_polling(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 expected_val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_polling(unit, table, field, expected_val));
exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Common Tiny MAC configuration API for
 *    multi fields together
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] table - Tiny MAC table
 * \param [in] field - Tiny MAC fields
 * \param [in] val - configuration values
 * \param [in] nof_fields - nof fields to be configured
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
imb_framer_mac_multi_set(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e * field,
    uint32 *val,
    int nof_fields)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_set(unit, table, field, val, nof_fields));

    SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_entry(unit,
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        (uint8 *) &port,
                                                        sizeof(int),
                                                        &imb_framer_internal_mac_access_control_set_er_rollback,
                                                        FALSE));

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
imb_framer_mac_get(
    int unit,
    bcm_port_t port,
    dbal_tables_e table,
    dbal_fields_e field,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, port));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_get(unit, table, field, val));

exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}
/**
 * \brief - get nof master logical ports on FEU
 */
static int
imb_framer_nof_master_ports_get(
    int unit,
    int *nof_ports)
{
    bcm_pbmp_t framer_ports;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * clear output pbmp
     */
    *nof_ports = 0;
    SHR_IF_ERR_EXIT(imb_framer_master_ports_get(unit, &framer_ports));
    _SHR_PBMP_COUNT(framer_ports, *nof_ports);
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Free the FIFO resources for
 *    SB RX/TX FIFO
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] fifo_type - SB fifo type
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 */
static int
imb_framer_port_sb_fifo_resource_free(
    int unit,
    bcm_port_t port,
    dnx_algo_flexe_link_list_fifo_type_e fifo_type)
{
    int client_channel, oam_channel;
    int speed, nof_ports;
    uint32 speed_flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    speed_flags = (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX) ? DNX_ALGO_PORT_SPEED_F_RX : DNX_ALGO_PORT_SPEED_F_TX;
    SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, port, speed_flags, &speed));
    /*
     * Free the SB Rx resources.
     */
    SHR_IF_ERR_EXIT(imb_framer_nof_master_ports_get(unit, &nof_ports));
    if ((nof_ports > 1) && DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel)
        && (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX))
    {
        /*
         * When removing OAM client, we need to check if there is other active client.
         * If yes, we need to keep OAM client active in SB RX.
         */
        SHR_IF_ERR_EXIT(imb_framer_internal_sb_client_rx_reset_set(unit, client_channel, IMB_COMMON_OUT_OF_RESET));
    }
    else
    {
        SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_force_config_set
                        (unit, client_channel, -1, speed, fifo_type, 0));
    }
    if (dnx_data_nif.mac_client.nof_special_clients_get(unit) > 0)
    {
        if ((nof_ports == 1) && !DNX_ALGO_FLEXE_MAC_CHANNEL_IS_OAM(unit, client_channel)
            && (fifo_type == DNX_ALGO_FLEXE_FIFO_SB_RX))
        {
            /*
             * If this is the last MAC client in FEU and it is not OAM client, need to
             * remove the OAM client resource in SB RX.
             */
            speed = dnx_data_nif.framer.oam_channel_max_bandwidth_get(unit);
            oam_channel = dnx_data_nif.mac_client.oam_client_channel_get(unit);
            SHR_IF_ERR_EXIT(imb_framer_internal_port_fifo_link_list_force_config_set
                            (unit, oam_channel, -1, speed, fifo_type, 0));
        }
    }

exit:
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
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_framer_port_remove_resource_free(
    int unit,
    bcm_port_t port,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FLEXE_DIRECTION_RX_GET(flags))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_sb_fifo_resource_free(unit, port, DNX_ALGO_FLEXE_FIFO_SB_RX));
    }
    if (IMB_FLEXE_DIRECTION_TX_GET(flags)
        && dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_sb_tx_fifo_supported))
    {
        SHR_IF_ERR_EXIT(imb_framer_port_sb_fifo_resource_free(unit, port, DNX_ALGO_FLEXE_FIFO_SB_TX));
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
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_framer_port_detach(
    int unit,
    bcm_port_t port)
{
    int client_channel, enable, has_speed, nof_master_ports;
    int mgmt_channel = -1;
    dnx_algo_port_info_s port_info;
    uint32 flags = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));
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
        IMB_FLEXE_DIRECTION_RX_SET(flags);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, DNX_ALGO_PORT_SPEED_F_TX, &has_speed));
    if (has_speed)
    {
        IMB_FLEXE_DIRECTION_TX_SET(flags);
    }
    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        /*
         * Check if the FlexE MAC client is in enable status
         */
        SHR_IF_ERR_EXIT(imb_framer_port_enable_get(unit, port, &enable));
        if (enable)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Cannot detach active port %d", port);
        }
        if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MGMT(unit, port_info))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_framer_mgmt_channel_get(unit, port, &mgmt_channel));
        }
        /*
         * Free FlexE core resource if needed
         */
        if (IMB_FLEXE_DIRECTION_TX_GET(flags))
        {
            DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_mac_client_delete
                                                       (unit, client_channel, mgmt_channel, flags, 0));
        }
        /*
         * Free the client resources, including SW DB, Hw resources for RMC, TMC, SB RX fifo.
         */
        SHR_IF_ERR_EXIT(imb_framer_port_remove_resource_free(unit, port, flags));
        /*
         * Update TMC, SB rx/tx calendar
         */
        SHR_IF_ERR_EXIT(imb_framer_calendar_set(unit, 0, flags));

        if (IMB_FRAMER_FEU_IS_VALID(unit))
        {
            SHR_IF_ERR_EXIT(imb_feu_port_detach(unit, port));
        }
        else
        {
            if (!DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_STAT(unit, port_info)
                && !DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_PROTECTION(unit, port_info))
            {
                SHR_IF_ERR_EXIT(dnx_port_arb_port_remove(unit, port));
                SHR_IF_ERR_EXIT(dnx_port_ofr_port_remove(unit, port));
                SHR_IF_ERR_EXIT(dnx_port_oft_port_remove(unit, port));
            }
        }
        /*
         * Configure ENC_DEC
         */
        if (dnx_data_nif.mac_client.feature_get(unit, dnx_data_nif_mac_client_enc_dec_configurable))
        {
            SHR_IF_ERR_EXIT(imb_framer_nof_master_ports_get(unit, &nof_master_ports));
            /*
             * Close the ENC DEC data path
             */
            if (nof_master_ports == 1)
            {
                SHR_IF_ERR_EXIT(imb_framer_enc_dec_path_enable_set(unit, 0));
            }
            SHR_IF_ERR_EXIT(imb_framer_enc_dec_control_blocks_enable_set(unit, port, 0));
            SHR_IF_ERR_EXIT(imb_framer_enc_dec_cl82_enable_set(unit, port, 0));
        }
    }
    else
    {
        /*
         * SAR client
         */
        DNX_ERR_RECOVERY_EXPR_SUPPRESS_IF_ERR_EXIT(flexe_core_sar_client_delete(unit, client_channel, flags, 0));
        /*
         * Clear SAR EGQ mapping
         */
        if (dnx_data_nif.sar_client.feature_get(unit, dnx_data_nif_sar_client_is_tdm_port))
        {
            SHR_IF_ERR_EXIT(dnx_port_ofr_port_remove(unit, port));
            SHR_IF_ERR_EXIT(imb_framer_sar_egq_mapping_set(unit, port, 0));
        }
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
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_framer_deinit(
    int unit,
    const imb_create_info_t * imb_info,
    imb_specific_create_info_t * imb_specific_info)
{
    SHR_FUNC_INIT_VARS(unit);
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
imb_framer_port_link_get(
    int unit,
    bcm_port_t port,
    int flags,
    int *link)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_FIFO_STATUS,
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
imb_framer_port_local_fault_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault)
{
    int client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_STATUS,
                                           DBAL_FIELD_LOCAL_FAULT_LIVE_STATUS, (uint32 *) local_fault));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
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
imb_framer_port_local_fault_status_clear(
    int unit,
    bcm_port_t port)
{
    int local_fault;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * There is no real clear bit. Read to clear
     */
    SHR_IF_ERR_EXIT(imb_framer_port_local_fault_status_get(unit, port, &local_fault));
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
imb_framer_port_remote_fault_status_get(
    int unit,
    bcm_port_t port,
    int *remote_fault)
{
    int client_channel;
    dnx_algo_port_info_s port_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, port, &client_channel));

    if (DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC(unit, port_info, TRUE, TRUE, TRUE))
    {
        SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_STATUS,
                                           DBAL_FIELD_REMOTE_FAULT_LIVE_STATUS, (uint32 *) remote_fault));
    }
    else if (DNX_ALGO_PORT_TYPE_IS_FRAMER_SAR(unit, port_info, FALSE))
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
imb_framer_port_remote_fault_status_clear(
    int unit,
    bcm_port_t port)
{
    int remote_fault;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * There is no real clear bit. Read to clear
     */
    SHR_IF_ERR_EXIT(imb_framer_port_remote_fault_status_get(unit, port, &remote_fault));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable remote fault for MAC client
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable
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
imb_framer_port_remote_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_REMOTE_FAULT_DISABLE, enable ? 0 : 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Enable local fault for MAC client
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] enable - enable or disable
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
imb_framer_port_local_fault_enable_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_set
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_LOCAL_FAULT_DISABLE, enable ? 0 : 1));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get port remote fault enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable or disable
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
imb_framer_port_remote_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    uint32 is_disabled;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_REMOTE_FAULT_DISABLE, &is_disabled));
    *enable = is_disabled ? 0 : 1;
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get port local fault enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] enable - enable or disable
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
imb_framer_port_local_fault_enable_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    uint32 is_disabled;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_get
                    (unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, DBAL_FIELD_LOCAL_FAULT_DISABLE, &is_disabled));
    *enable = is_disabled ? 0 : 1;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - force Local/remote fault in Tiny MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - local fault or remote fault
 * \param [in] enable - enable or disable
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
imb_framer_port_link_fault_force_set(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int enable)
{
    int orig_enable;
    imb_framer_mac_tx_fault_source_e tx_fault_src;
    dnx_algo_port_info_s port_info;
    dbal_fields_e dbal_field, dbal_field_to_clear;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_port_link_fault_force_get(unit, port, type, &orig_enable));
    if (orig_enable == enable)
    {
        SHR_EXIT();
    }
    if (enable)
    {
        /*
         * Change Tx fault source to SW
         */
        tx_fault_src = IMB_FRAMER_MAC_TX_FAULT_SOURCE_SW_CFG;
    }
    else
    {
        /*
         * Restore default Tx fault source
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
        tx_fault_src =
            DNX_ALGO_PORT_TYPE_IS_FRAMER_MAC_L1(unit,
                                                port_info) ? IMB_FRAMER_MAC_TX_FAULT_SOURCE_EXT_INPUT :
            IMB_FRAMER_MAC_TX_FAULT_SOURCE_RX_FAULT_SM;
    }
    SHR_IF_ERR_EXIT(imb_framer_port_mac_tx_fault_src_set(unit, port, tx_fault_src));
    /*
     * If forcing local fault, forcing remote fault is cleared, vice verse.
     */
    if (enable)
    {
        dbal_field_to_clear =
            (type ==
             bcmPortControlLinkFaultLocalForce) ? DBAL_FIELD_FORCE_REMOTE_FAULT_OS : DBAL_FIELD_FORCE_LOCAL_FAULT_OS;
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, dbal_field_to_clear, 0));
    }
    /*
     * Force Link fault
     */
    dbal_field =
        (type ==
         bcmPortControlLinkFaultLocalForce) ? DBAL_FIELD_FORCE_LOCAL_FAULT_OS : DBAL_FIELD_FORCE_REMOTE_FAULT_OS;
    SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, dbal_field, enable));
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get port local/remote fault force status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] type - local fault or remote fault
 * \param [out] enable - enable or disable
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
imb_framer_port_link_fault_force_get(
    int unit,
    bcm_port_t port,
    bcm_port_control_t type,
    int *enable)
{
    dbal_fields_e dbal_field;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get Link fault force status
     */
    dbal_field =
        (type ==
         bcmPortControlLinkFaultLocalForce) ? DBAL_FIELD_FORCE_LOCAL_FAULT_OS : DBAL_FIELD_FORCE_REMOTE_FAULT_OS;
    SHR_IF_ERR_EXIT(imb_framer_mac_get(unit, port, DBAL_TABLE_TINY_MAC_RX_LSS_CTRL, dbal_field, (uint32 *) enable));
exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
int
imb_framer_port_faults_status_get(
    int unit,
    bcm_port_t port,
    int *local_fault,
    int *remote_fault)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(imb_framer_port_local_fault_status_get(unit, port, local_fault));
    SHR_IF_ERR_EXIT(imb_framer_port_remote_fault_status_get(unit, port, remote_fault));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Program loopback tdm table for tinymac
 *
 * \param [in] unit - chip unit id
 * \param [in] client_channel - client channel
 * \param [in] valid - valid or not
 * \param [in] tdm_slot_start - slot start in the range of 1700
 * \param [in] nof_slot - number of slot from tdm_slot_start
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
imb_framer_mac_loopback_tdm_table_set(
    int unit,
    int client_channel,
    int valid,
    int tdm_slot_start,
    int nof_slot)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_flexe_tiny_mac_access_lock_take(unit));

    SHR_IF_ERR_EXIT(imb_framer_internal_mac_access_control_set(unit, -1));
    SHR_IF_ERR_EXIT(imb_framer_internal_mac_loopback_tdm_table_hw_set
                    (unit, client_channel, valid, tdm_slot_start, nof_slot));
exit:
    SHR_IF_ERR_CONT(dnx_algo_flexe_tiny_mac_access_lock_give(unit));
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port local fault enable status
 *
 * \param [in] unit - chip unit id
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */

int
imb_framer_mac_loopback_tdm_table_clear(
    int unit)
{

    int total_slots = dnx_data_nif.mac_client.max_nof_slots_get(unit);
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_framer_mac_loopback_tdm_table_set(unit, 0, 0, 0, total_slots));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - mac client calendar set
 *
 * \param [in] unit - chip unit id
 * \return
 *   int - see _SHR_E*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
int
imb_framer_mac_loopback_calendar_set(
    int unit)
{
    uint32 *nof_slots_per_port = NULL, *calendar = NULL;
    int nof_calendar_clients, calendar_length, granularity;
    bcm_pbmp_t all_tmac_ports;
    bcm_port_t logical_port;
    int speed, has_speed;
    int loopback, total_slot = 0;
    int client_channel, valid;
    int calendar_entry_index;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get calendar parameters for algo
     */
    nof_calendar_clients = dnx_data_nif.mac_client.nof_clients_get(unit);
    calendar_length = dnx_data_nif.mac_client.max_nof_slots_get(unit);
    /*
     * The calendar granularity is 1G
     */
    granularity = dnx_data_nif.flexe.client_speed_granularity_get(unit, bcmPortFlexePhySlot1G)->val;

    /*
     * Allocating arraies memory
     */
    SHR_ALLOC_SET_ZERO(calendar, calendar_length * sizeof(uint32),
                       "Calendar to set slots per port", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_NULL_CHECK(calendar, _SHR_E_MEMORY, "calendar");

    SHR_ALLOC_SET_ZERO(nof_slots_per_port, nof_calendar_clients * sizeof(uint32),
                       "Saves how many slots each port needs", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_NULL_CHECK(nof_slots_per_port, _SHR_E_MEMORY, "nof_slots_per_port");

    /*
     * Build calendar per port.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FRAMER_MAC, 0, &all_tmac_ports));
    /*
     * Step1: Build slot per port.
     */
    _SHR_PBMP_ITER(all_tmac_ports, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_framer_mac_loopback_enable_get(unit, logical_port, &loopback));
        if (loopback != BCM_PORT_LOOPBACK_MAC)
        {
            continue;
        }
        speed = 0;
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }

        if (!speed)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(dnx_algo_port_flexe_channel_get(unit, logical_port, &client_channel));
        nof_slots_per_port[client_channel] = UTILEX_DIV_ROUND_UP(speed, granularity);
        total_slot += nof_slots_per_port[client_channel];
    }
    /*
     * Step2: Build calendar
     */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build
                    (unit, TRUE, nof_slots_per_port, nof_calendar_clients, calendar_length, calendar_length, calendar));
    /*
     * Step3: Config calendar
     */
    if (total_slot > 0)
    {
        /*
         * Disable global loopback
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, -1, DBAL_TABLE_TINY_MAC_COMMON_CTRL, DBAL_FIELD_LOCAL_LPBK, 0));
        /*
         * Program LPBK_WRAP_PTR
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_set
                        (unit, -1, DBAL_TABLE_TINY_MAC_COMMON_CTRL, DBAL_FIELD_LOCAL_LPBK_WRAP_PTR, total_slot - 1));
        /*
         * Program flexmac2_lpbk_Tdm_table
         */
        for (calendar_entry_index = 0; calendar_entry_index < calendar_length; ++calendar_entry_index)
        {
            client_channel =
                (calendar[calendar_entry_index] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID) ? 0 : calendar[calendar_entry_index];
            valid = (calendar[calendar_entry_index] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID) ? 0 : 1;
            SHR_IF_ERR_EXIT(imb_framer_mac_loopback_tdm_table_set(unit, client_channel,
                                                                  valid, calendar_entry_index, 1));
        }
        /*
         * Enable global loopback
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, -1, DBAL_TABLE_TINY_MAC_COMMON_CTRL, DBAL_FIELD_LOCAL_LPBK, 1));
    }
    else
    {
        /*
         * Disable global loopback
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_set(unit, -1, DBAL_TABLE_TINY_MAC_COMMON_CTRL, DBAL_FIELD_LOCAL_LPBK, 0));
        /*
         * Clear all clients credit and valid in flexmac2_lpbk_Tdm_table
         */
        SHR_IF_ERR_EXIT(imb_framer_mac_loopback_tdm_table_clear(unit));
    }

exit:
    SHR_FREE(calendar);
    SHR_FREE(nof_slots_per_port);
    SHR_FUNC_EXIT;
}

/**
 * \brief - set port loopback
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback mode
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
imb_framer_port_loopback_set(
    int unit,
    bcm_port_t port,
    int loopback)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_framer_mac_loopback_enable_set(unit, port, loopback));

    SHR_IF_ERR_EXIT(imb_framer_mac_loopback_calendar_set(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get port local fault enable status
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] loopback - loopback mode
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
imb_framer_port_loopback_get(
    int unit,
    bcm_port_t port,
    int *loopback)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_framer_mac_loopback_enable_get(unit, port, loopback));

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Set PRD profile for Framer MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [in] profile - Profile ID
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
imb_framer_prd_port_profile_map_set(
    int unit,
    bcm_port_t port,
    uint32 profile)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_port_profile_map_set(unit, port, profile));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_port_profile_map_set(unit, port, profile));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get PRD profile for Framer MAC
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] profile - Profile ID
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
imb_framer_prd_port_profile_map_get(
    int unit,
    bcm_port_t port,
    uint32 *profile)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_port_profile_map_get(unit, port, profile));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_port_profile_map_get(unit, port, profile));
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
imb_framer_prd_enable_set(
    int unit,
    bcm_port_t port,
    int enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_enable_set(unit, port, enable_mode));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_enable_set(unit, port, enable_mode));
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
imb_framer_prd_enable_get(
    int unit,
    bcm_port_t port,
    int *enable_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_enable_get(unit, port, enable_mode));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_enable_get(unit, port, enable_mode));
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
imb_framer_prd_properties_set(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_properties_set(unit, port, property, val));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_properties_set(unit, port, property, val));
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
imb_framer_prd_properties_get(
    int unit,
    bcm_port_t port,
    imb_prd_properties_t property,
    uint32 *val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_properties_get(unit, port, property, val));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_properties_get(unit, port, property, val));
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
imb_framer_prd_map_set(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 priority,
    uint32 is_tdm)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_map_set(unit, port, map, key, priority, is_tdm));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_map_set(unit, port, map, key, priority, is_tdm));
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
imb_framer_prd_map_get(
    int unit,
    bcm_port_t port,
    bcm_cosq_ingress_port_drop_map_t map,
    uint32 key,
    uint32 *priority,
    uint32 *is_tdm)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_map_get(unit, port, map, key, priority, is_tdm));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_map_get(unit, port, map, key, priority, is_tdm));
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
imb_framer_prd_tpid_set(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tpid_set(unit, port, tpid_index, tpid_value));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tpid_set(unit, port, tpid_index, tpid_value));
    }
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
imb_framer_prd_tpid_get(
    int unit,
    bcm_port_t port,
    uint32 tpid_index,
    uint32 *tpid_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tpid_get(unit, port, tpid_index, tpid_value));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tpid_get(unit, port, tpid_index, tpid_value));
    }
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
imb_framer_prd_threshold_set(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_threshold_set(unit, port, priority, threshold));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_set(unit, port, priority, threshold));
    }
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
imb_framer_prd_threshold_get(
    int unit,
    bcm_port_t port,
    uint32 priority,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_threshold_get(unit, port, priority, threshold));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_get(unit, port, priority, threshold));
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_framer_prd_threshold_max_get(
    int unit,
    bcm_port_t port,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_threshold_max_get(unit, port, threshold));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_threshold_max_get(unit, port, threshold));
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
imb_framer_prd_custom_ether_type_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_custom_ether_type_set(unit, port, ether_type_code, ether_type_val));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_custom_ether_type_set(unit, port, ether_type_code, ether_type_val));
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
imb_framer_prd_custom_ether_type_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_val)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_custom_ether_type_get(unit, port, ether_type_code, ether_type_val));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_custom_ether_type_get(unit, port, ether_type_code, ether_type_val));
    }
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
imb_framer_prd_ether_type_size_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_ether_type_size_set(unit, port, ether_type_code, ether_type_size));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_ether_type_size_set(unit, port, ether_type_code, ether_type_size));
    }
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
imb_framer_prd_ether_type_size_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *ether_type_size)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_ether_type_size_get(unit, port, ether_type_code, ether_type_size));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_ether_type_size_get(unit, port, ether_type_code, ether_type_size));
    }
exit:
    SHR_FUNC_EXIT;
}

int
imb_framer_prd_tcam_entry_key_offset_base_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_base_set(unit, port, ether_type_code, offset_base));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_base_set(unit, port, ether_type_code, offset_base));
    }
exit:
    SHR_FUNC_EXIT;

}

int
imb_framer_prd_tcam_entry_key_offset_base_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 *offset_base)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_base_get(unit, port, ether_type_code, offset_base));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_base_get(unit, port, ether_type_code, offset_base));
    }
exit:
    SHR_FUNC_EXIT;

}

int
imb_framer_prd_tcam_entry_key_offset_set(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_set(unit, port, ether_type_code, offset_index, offset_value));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_set(unit, port, ether_type_code, offset_index, offset_value));
    }
exit:
    SHR_FUNC_EXIT;

}

int
imb_framer_prd_tcam_entry_key_offset_get(
    int unit,
    bcm_port_t port,
    uint32 ether_type_code,
    uint32 offset_index,
    uint32 *offset_value)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_tcam_entry_key_offset_get(unit, port, ether_type_code, offset_index, offset_value));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_tcam_entry_key_offset_get(unit, port, ether_type_code, offset_index, offset_value));
    }
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
imb_framer_prd_flex_key_entry_set(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    const dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_flex_key_entry_set(unit, port, key_index, entry_info));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_flex_key_entry_set(unit, port, key_index, entry_info));
    }
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
imb_framer_prd_flex_key_entry_get(
    int unit,
    bcm_port_t port,
    uint32 key_index,
    dnx_cosq_prd_tcam_entry_info_t * entry_info)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_flex_key_entry_get(unit, port, key_index, entry_info));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_flex_key_entry_get(unit, port, key_index, entry_info));
    }
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
imb_framer_prd_control_frame_set(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    const bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_control_frame_set(unit, port, control_frame_index, control_frame_config));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_control_frame_set(unit, port, control_frame_index, control_frame_config));
    }
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
imb_framer_prd_control_frame_get(
    int unit,
    bcm_port_t port,
    uint32 control_frame_index,
    bcm_cosq_ingress_drop_control_frame_config_t * control_frame_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_control_frame_get(unit, port, control_frame_index, control_frame_config));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_control_frame_get(unit, port, control_frame_index, control_frame_config));
    }
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
imb_framer_prd_mpls_special_label_set(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    const bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_mpls_special_label_set(unit, port, label_index, label_config));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_mpls_special_label_set(unit, port, label_index, label_config));
    }
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
imb_framer_prd_mpls_special_label_get(
    int unit,
    bcm_port_t port,
    uint32 label_index,
    bcm_cosq_ingress_port_drop_mpls_special_label_config_t * label_config)
{
    SHR_FUNC_INIT_VARS(unit);

    if (IMB_FRAMER_FEU_IS_VALID(unit))
    {
        SHR_IF_ERR_EXIT(imb_feu_prd_mpls_special_label_get(unit, port, label_index, label_config));
    }
    else
    {
        /*
         * Configure OFR
         */
        SHR_IF_ERR_EXIT(dnx_ofr_prd_mpls_special_label_get(unit, port, label_index, label_config));
    }
exit:
    SHR_FUNC_EXIT;

}
