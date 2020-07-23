/*! \file tx.c
 * 
 *
 * TX procedures for DNX.
 *
 * Here add DESCRIPTION.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TX
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/common/tx.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */
#include <shared/bslenum.h>
#include <bcm/types.h>
#include <bcm/tx.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <soc/dnx/dnx_visibility.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_rx_access.h>
#include <soc/dnx/dbal/dbal.h>
#ifdef ADAPTER_SERVER_MODE
#include <soc/sand/sand_signals.h>
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/cm.h>
#endif
/*
 * }
 */

/*
 * Macros and definitions
 * {
 */

/*
 * Channel is always derived from table PktSrcToChannelMap.
 */
#define CPU_CHANNEL_SOURCE_0        0x0
/*
 * Channel is derived from module header (if exists) or from table PktSrcToChannelMap.
 */
#define CPU_CHANNEL_SOURCE_1        0x1
/*
 * Channel is derived from module header (if exists) or from register CpuChannel.
 */
#define CPU_CHANNEL_SOURCE_2        0x2
/*
 * Channel is always derived register CpuChannel.
 */
#define CPU_CHANNEL_SOURCE_3        0x3

/*
 * }
 */

int
bcm_dnx_tx(
    int unit,
    bcm_pkt_t * pkt,
    void *cookie)
{
#ifdef ADAPTER_SERVER_MODE
    uint32 adapter_loopback_enable;
    adapter_ms_id_e ms_id = ADAPTER_MS_ID_FIRST_MS;
#endif

    SHR_FUNC_INIT_VARS(unit);

    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

#ifdef ADAPTER_SERVER_MODE
    /** clearing the signals cache is made for all units, */
    /** in order to support packet transmitted between units. (multi device scenario) */
    /** in adapter all units ordered from zero to nof devices */
    for (int i = 0; i < soc_cm_get_num_devices(); i++)
    {
        sand_adapter_clear_signals(i);
    }

    /** Gets the value of the adapter_loopback_enable from the dnx_data */
    adapter_loopback_enable = dnx_data_adapter.tx.loopback_enable_get(unit);

    if (adapter_loopback_enable == 1)
    {
        ms_id = ADAPTER_MS_ID_LOOPBACK;
    }

    /*
     * Use a different flow for sending a packet in adapter.
     * The first two entries in the packet data contain a hard coded value for verification and the tx port
     * We therefore pass the data starting at index 2 and use index 1 to pass the tx port
     */

    SHR_IF_ERR_EXIT(adapter_send_buffer(unit, (uint32) ms_id, (uint32) (pkt->src_gport), (uint32) (pkt->_dpp_hdr[0]),
                                        pkt->pkt_data->len, pkt->pkt_data->data, 1));

#else

    /** Regular TX handling */
    /*
     * Clear Debug Signals Memory - return status irrelevant
     */
    if (!(pkt->flags & BCM_TX_NO_VISIBILITY_RESUME))
    {
        dnx_visibility_resume(unit, BCM_CORE_ALL,
                              BCM_INSTRU_CONTROL_FLAG_DIRECTION_INGRESS | BCM_INSTRU_CONTROL_FLAG_DIRECTION_EGRESS);
    }

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pointer to pkt is not valid");
    SHR_IF_ERR_EXIT(bcm_common_tx(unit, pkt, cookie));

#endif

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Transmit an array of packets
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 *   \param [in] pkt  -  array of pointers to packets to transmit
 *   \param [in] count  -  Number of packets in the array
 *   \param [in] all_done_cb  -  Chain done callback function
 *   \param [in] cookie  -  Callback cookie
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   Packets are sent synchronously
 * \see
 *   * None
 */
int
bcm_dnx_tx_array(
    int unit,
    bcm_pkt_t ** pkt,
    int count,
    bcm_pkt_cb_f all_done_cb,
    void *cookie)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tx_array(unit, pkt, count, all_done_cb, cookie));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Transmit a linked list of packets
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 *   \param [in] pkt  -  Pointer to linked list of packets
 *   \param [in] all_done_cb  -  Chain done callback function
 *   \param [in] cookie  -  Callback cookie
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   Packets are sent synchronously
 * \see
 *   * None
 */
int
bcm_dnx_tx_list(
    int unit,
    bcm_pkt_t * pkt,
    bcm_pkt_cb_f all_done_cb,
    void *cookie)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tx_list(unit, pkt, all_done_cb, cookie));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

#if defined(BROADCOM_DEBUG)
/**
 * \brief
 *   Display info about tx state
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_tx_show(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tx_show(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Display info about a DV that is setup for TX
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 *   \param [in] dv    -  The DV to show info about
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   * Mainly, dumps the tx_dv_info_t structure; then calls soc_dma_dump_dv
 * \see
 *   * None
 */
int
bcm_dnx_tx_dv_dump(
    int unit,
    void *dv_p)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(dv_p, _SHR_E_PARAM, "dv_p");
    SHR_IF_ERR_EXIT(bcm_common_tx_dv_dump(unit, dv_p));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief
 *   Initialize BCM TX API
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   Please see bcm_common_tx_init
 * \see
 *   * None
 */
shr_error_e
dnx_tx_init(
    int unit)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tx_init(unit));

    /*
     * JR2 and above - CPU channel derived from module header (if exists), otherwise 0
     */
    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_CPU_CHANNEL_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHANNEL_SELECT, INST_SINGLE,
                                     CPU_CHANNEL_SOURCE_2);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   De-initialize BCM TX API
 * \par DIRECT INPUT:
 *   \param [in] unit  -  Unit ID
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e - Error Type
 * \par INDIRECT OUTPUT:
 *   * None
 * \remark
 *   free resource allocated in bcm_tx_init
 * \see
 *   * None
 */
shr_error_e
dnx_tx_deinit(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(bcm_common_tx_deinit(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
