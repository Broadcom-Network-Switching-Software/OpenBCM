/**
 *  \file /src/bcm/dnx/rx/rx.c
 * 
 * RX procedures for DNX.
 * Traps
 * Adapter
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_RX
/*
 * Include files which are specifically for DNX. Final location.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/swstate/dnxc_sw_state_utils.h>
/*
 * }
 */
/*
 * Include files currently used for DNX. To be modified and moved to
 * final location.
 * {
 */

#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/switch.h>
#include <bcm_int/common/rx.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/rx/rx.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnxc/dnxc_wb_test_utils.h>
#include <netdb.h>
#include <shared/bitop.h>
#include <shared/bslenum.h>
#include <bcm/port.h>
#include <bcm/knet.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnxc/multithread_analyzer.h>
#include <soc/dnxc/dnxc_ha.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_rx_access.h>
#include <bcm/stack.h>
#include <src/bcm/dnx/trunk/trunk_utils.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <bcm_int/control.h>
#ifdef ADAPTER_SERVER_MODE
#include <soc/dnx/adapter/adapter_reg_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_adapter.h>
#include <fcntl.h>
#include <unistd.h>
#include <soc/cm.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <soc/sand/sand_signals.h>
#endif

/*
 * }
 */

/*
 * Macros and defines for rx thread management
 * {
 */
#ifdef ADAPTER_SERVER_MODE

/*
 * This is the default time between minimum token refreshes. It is also
 * the maximum time between RX thread wake-ups.
 */
#define BCM_RX_TOKEN_CHECK_US_DEFAULT   100000  /* 10 times/sec. */

#define BASE_SLEEP_VAL   500000

/*
 * Set sleep to base value
 */
#define INIT_SLEEP    rx_control.sleep_cur = BASE_SLEEP_VAL

/*
 * Lower sleep time if val is < current
 */
#define SLEEP_MIN_SET(val)                                           \
    (rx_control.sleep_cur = ((val) < rx_control.sleep_cur) ?         \
     (val) : rx_control.sleep_cur)
#endif

/*
 * Get the bits number
 */
#define DNX_RX_PARSE_HEADER_BITS_GET(_len)   ((_len) * SAL_UINT8_NOF_BITS)

/*
 * Get system header data information
 */
#define DNX_RX_PARSE_HEADER_DATA_GET(unit, type, field)  (DNX_DATA_MAX_HEADERS_##type##_##field)

/*
 * Get ftmh header data information
 */
#define DNX_RX_PARSE_SYSTEM_HEADERS_MODE_JERICHO2(unit)  \
    (dnx_data_headers.system_headers.system_headers_mode_get(unit) == DNX_RX_PARSE_HEADER_DATA_GET(unit, SYSTEM_HEADERS, SYSTEM_HEADERS_MODE_JERICHO2))

/*
 * Get field value from Jericho2 system header
 * This is used to read this field's offset and length in the system header.
 * Then get this field's value with offset and length.
 */
#define DNX_RX_PARSE_HEADER_FIELD_GET(unit, type, field, val) \
    offset = DNX_RX_PARSE_HEADER_DATA_GET(unit, type, field##_OFFSET); \
    nof_bits = DNX_RX_PARSE_HEADER_DATA_GET(unit, type, field##_BITS); \
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &val));

/*
 * Max length for certain system header
 */
#define DNX_RX_PARSE_HEADER_MAX_LEN                 0x20
/*
 * There is two padding bytes appendeded to packet in pipeline, which is included by packet_size of FTMH
 */
#define DNX_RX_PARSE_HEADER_LIF_EXT_TYPE            0x8

#define DNX_RX_PARSE_HEADER_FTMH_TM_PROFILE_BIT_1   0x2
#define DNX_RX_PARSE_HEADER_FTMH_TM_PROFILE_BIT_0   0x1

#define DNX_RX_PARSE_HEADER_INTERNAL_1              1
#define DNX_RX_PARSE_HEADER_INTERNAL_2              5
#define DNX_RX_PARSE_HEADER_INTERNAL_3              71
#define DNX_RX_PARSE_HEADER_INTERNAL_4              1
#define DNX_RX_PARSE_HEADER_INTERNAL_5              78
#define DNX_RX_PARSE_HEADER_INTERNAL_6              2
#define DNX_RX_PARSE_HEADER_INTERNAL_7              90
#define DNX_RX_PARSE_HEADER_INTERNAL_8              3
#define DNX_RX_PARSE_HEADER_INTERNAL_9              16
#define DNX_RX_PARSE_HEADER_INTERNAL_10             2
#define DNX_RX_PARSE_HEADER_INTERNAL_11             18
#define DNX_RX_PARSE_HEADER_INTERNAL_12             1
#define DNX_RX_PARSE_HEADER_INTERNAL_13             13
#define DNX_RX_PARSE_HEADER_INTERNAL_14             3
#define DNX_RX_PARSE_HEADER_INTERNAL_15             6
#define DNX_RX_PARSE_HEADER_INTERNAL_16             2
#define DNX_RX_PARSE_HEADER_INTERNAL_17             4
#define DNX_RX_PARSE_HEADER_INTERNAL_18             2
#define DNX_RX_PARSE_HEADER_INTERNAL_19             2
#define DNX_RX_PARSE_HEADER_INTERNAL_20             2
#define DNX_RX_PARSE_HEADER_INTERNAL_21             0
#define DNX_RX_PARSE_HEADER_INTERNAL_22             2
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_0        0
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_1        1
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_2        2
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_3        3
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_5        5
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_8        8
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_9        9
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_10       10
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_12       12
#define DNX_RX_PARSE_HEADER_INTERNAL_VALUE_19       19

#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_1       55
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_2       1
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_3       54
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_4       1
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_5       52
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_6       2
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_7       48
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_8       4
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_9       41
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_10      7
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_1 1
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_2 2
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_3 3
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_5 5
#define DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_8 8

#define DNX_RX_PARSE_HEADER_ETH1_SIZE               0x12

/*
 * Additional header and recycle header when packets are trapped to CPU via ETPP MTU trap
 */
#define DNX_RX_PARSE_ETPP_MTU_TRAP_ADDITIONAL_HEADER_SIZE               64
#define DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_SIZE                  40
#define DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_TIME_STAMP_SIZE       1
#define DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_PP_DSP_SIZE           1
#define DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_OUTLIF_TO_STAMP_SIZE  3

/*
 * }
 */
int bcm_adapter_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg);

/**
 * \brief - Return boolean as to whether unit is running
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Boolean: TRUE if unit is running
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_active(
    int unit)
{
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    return _bcm_common_rx_active(unit);
}

/**
 * \brief - Initialize the rx thread and resources
 *
 * \param [in] unit - Relevant unit
 * \param [in] cfg - RX configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Starts the packet receive thread if not already running.
 *   cfg may be null:  Use default config.
 *   alloc/free in cfg may be null:  Use default alloc/free functions
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    uint32 entry_handle_id;
    uint8 is_init;
    uint32 ftmh_lb_key_size;
    uint32 ftmh_stacking_ext_size;
    uint8 user_header_enable;
    uint32 pph_base_size;
    uint32 out_lif_x_1_ext_size;
    uint32 out_lif_x_2_ext_size;
    uint32 out_lif_x_3_ext_size;
    uint32 in_lif_ext_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_RCPU_ENABLE_CTRL, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RCPU_FOR_ATE, INST_SINGLE, FALSE);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Init RX parser related configration table
         * Per to use SW State instead of DBAL, because the register access via DBAL consumes time
         */
        SHR_IF_ERR_EXIT(dnx_rx_parser_info.is_init(unit, &is_init));
        if (is_init)
        {
            int oamp_ports_max = 2;
            bcm_gport_t oamp_gport[2] = { 0 };
            int oamp_port_count = 0;
            dnx_algo_gpm_gport_phy_info_t gport_info;

            /*
             * Get configration from DBAL
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_LB_KEY_EXT_SIZE, INST_SINGLE,
                                       &ftmh_lb_key_size);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FTMH_STACKING_EXT_SIZE, INST_SINGLE,
                                       &ftmh_stacking_ext_size);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, &pph_base_size);
            dbal_value_field8_request(unit, entry_handle_id, DBAL_FIELD_USER_HEADER_ENABLE, INST_SINGLE,
                                      &user_header_enable);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 0, &out_lif_x_1_ext_size);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 1, &out_lif_x_2_ext_size);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OUT_LIF_EXT_SIZE, 2, &out_lif_x_3_ext_size);
            dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IN_LIF_EXT_SIZE, INST_SINGLE,
                                       &in_lif_ext_size);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

            /*
             * Set swstate
             */
            dnx_rx_parser_info.ftmh_lb_key_size.set(unit, ftmh_lb_key_size);
            dnx_rx_parser_info.ftmh_stacking_ext_size.set(unit, ftmh_stacking_ext_size);
            dnx_rx_parser_info.pph_base_size.set(unit, pph_base_size);
            dnx_rx_parser_info.user_header_enable.set(unit, user_header_enable);
            dnx_rx_parser_info.out_lif_x_1_ext_size.set(unit, out_lif_x_1_ext_size);
            dnx_rx_parser_info.out_lif_x_2_ext_size.set(unit, out_lif_x_2_ext_size);
            dnx_rx_parser_info.out_lif_x_3_ext_size.set(unit, out_lif_x_3_ext_size);
            dnx_rx_parser_info.in_lif_ext_size.set(unit, in_lif_ext_size);

            /*
             * Record OAMP system ports
             */
            SHR_IF_ERR_EXIT(bcm_port_internal_get
                            (unit, BCM_PORT_INTERNAL_OAMP, oamp_ports_max, oamp_gport, &oamp_port_count));
            if (oamp_port_count >= 1)
            {
                /** Get information(including system port) from given gport or local port */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, oamp_gport[0],
                                                                DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                                &gport_info));
                dnx_rx_parser_info.oamp_system_port_0.set(unit, gport_info.sys_port);
            }
            if (oamp_port_count >= 2)
            {
                /** Get information(including system port) from given gport or local port */
                SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get(unit, oamp_gport[1],
                                                                DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT,
                                                                &gport_info));
                dnx_rx_parser_info.oamp_system_port_1.set(unit, gport_info.sys_port);
            }
        }
    }

#ifdef ADAPTER_SERVER_MODE
    SHR_IF_ERR_EXIT(bcm_adapter_rx_start(unit, cfg));
#else
    if (bcm_dnx_rx_active(unit))
    {
        SHR_EXIT();
    }
    /*
     * Use default config, see _cmicx_rx_dflt_cfg
     */
    SHR_IF_ERR_EXIT(_bcm_common_rx_start(unit, cfg));
#endif
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Allocate RX packet buffer
 *
 * \param [in] unit - Relevant unit
 * \param [in] pkt_size - Packet Size
 * \param [in] flags - Used to set up packet flags
 * \param [out] buf - Pointer to allocated buffer
 *
 * \return
 *   Pointer to new packet buffer or NULL if cannot alloc memory
 *
 * \remark
 *   If pkt_size <= 0, then the default packet size for the unit is used.
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_alloc(
    int unit,
    int pkt_size,
    uint32 flags,
    void **buf)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_alloc(unit, pkt_size, flags, buf));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Free RX packet buffer. Generally, packet buffer was allocated with bcm_rx_alloc.
 *
 * \param [in] unit - Relevant unit
 * \param [in] pkt_data - packet to free
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   packets with BCM_RX_HANDLED_OWNED should use this to free packets
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_free(
    int unit,
    void *pkt_data)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_free(unit, pkt_data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    /** Warmboot skipped, because of a usage between pkt_send and pkt_verify.*/
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 *  \brief Stop RX for the given unit; saves current configuration.
 *
 *  \param [in] unit - Relevant unit
 *  \param [in] cfg - Pointer to RX configuration
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_stop(
    int unit,
    bcm_rx_cfg_t * cfg)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_stop(unit, cfg));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Initialize the user level configuration.
 *
 * \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Can't use if currently running.  Should be called before
 *   doing a simple modification of the RX configuration in case
 *   the previous user has left it in a strange state.
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_cfg_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_cfg_init(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Register an upper layer driver
 *
 * \param [in] unit - Relevant unit
 * \param [in] name - Constant character string for debug purposes
 * \param [in] cb_f - Callback function when packet arrives
 * \param [in] pri - Priority of handler in list (0:lowest priority)
 * \param [in] cookie - Cookie passed to driver when packet arrives
 * \param [in] flags - Flags to indicate interrup or non-interrupt callback and which COS should this callback be run
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_register(
    int unit,
    const char *name,
    bcm_rx_cb_f cb_f,
    uint8 pri,
    void *cookie,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_register(unit, name, cb_f, pri, cookie, flags));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback from the Rx handlers.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [in] cb_f - Callback to unregister
 *   \param [in] pri - Priority of the callback
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_unregister(
    int unit,
    bcm_rx_cb_f cb_f,
    uint8 pri)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_unregister(unit, cb_f, pri));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Assign a RX channel to a cosq
 *
 * \param [in] unit - Relevant unit
 * \param [in] queue_id - CPU cos queue index (0 - (max cosq - 1)), Negative for all
 * \param [in] chan_id - channel index (0 - (BCM_RX_CHANNELS-1)), -1 for no channel
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_channel_set(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t chan_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_channel_set(unit, queue_id, chan_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Get the associated rx channel with a given cosq
 *
 * \param [in] unit - Relevant unit
 * \param [in] queue_id - CPU cos queue index (0 - (max cosq - 1))
 * \param [out] chan_id - channel index (0 - (BCM_RX_CHANNELS-1))
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_channel_get(
    int unit,
    bcm_cos_queue_t queue_id,
    bcm_rx_chan_t * chan_id)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_channel_get(unit, queue_id, chan_id));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief Register an application callback for the specified CPU queue
 *
 * \param [in] unit - Relevant unit
 * \param [in] name - Constant character string for debug purposes
 * \param [in] cosq - CPU cos queue
 * \param [in] callback - Callback function pointer
 * \param [in] priority - Priority of handler in list (0:lowest priority)
 * \param [in] cookie - Cookie passed to driver when packet arrives
 * \param [in] flags - Flags to indicate interrup or non-interrupt callback
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_register(
    int unit,
    const char *name,
    bcm_cos_queue_t cosq,
    bcm_rx_cb_f callback,
    uint8 priority,
    void *cookie,
    uint32 flags)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_register(unit, name, cosq, callback, priority, cookie, flags));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Unregister a callback.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [in] cosq - CPU cos queue
 *   \param [in] callback - Registered callback function
 *   \param [in] priority - Priority of registered callback function
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_unregister(
    int unit,
    bcm_cos_queue_t cosq,
    bcm_rx_cb_f callback,
    uint8 priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_unregister(unit, cosq, callback, priority));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    BCM_WARM_BOOT_API_TEST_MODE_SKIP_WB_SEQUENCE(unit);
    SHR_FUNC_EXIT;
}

#if defined(BROADCOM_DEBUG)
/**
 * \brief - Show RX information for the specified device.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_show(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_show(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - Get the current RX configuration if RX init is done.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [out] cfg - Current RX configuration
 *
 * \return
 *   BCM_E_INIT if RX is not running on current unit
 *   BCM_E_NONE if RX is running on current unit and no errors
 *   Negative in case of an error. See shr_error_e
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_cfg_get(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(cfg, _SHR_E_PARAM, "rx cfg");
    SHR_IF_ERR_EXIT(_bcm_common_rx_cfg_get(unit, cfg));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Returns a bitmap indicating which channels are active.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit      - Relevant unit
 *   \param [out] channels - Bitmap indicating which channels are active
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_channels_running(
    int unit,
    uint32 *channels)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(channels, _SHR_E_PARAM, "rx channels running");
    SHR_IF_ERR_EXIT(_bcm_common_rx_channels_running(unit, channels));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Clear all RX info.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   Shutdown threads, free up resources
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_clear(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_clear(unit));
exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the CPU RX rate in packets per second.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit      - Relevant unit
 *   \param [out] pps      - CPU RX rate in packets per second
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_cpu_rate_get(
    int unit,
    int *pps)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(pps, _SHR_E_PARAM, "CPU RX rate");
    SHR_IF_ERR_EXIT(_bcm_common_rx_cpu_rate_get(unit, pps));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Queue a packet to be freed by the RX thread.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit      - Relevant unit
 *   \param [in] pkt_data  - pointer to packet data to be enqueued for free
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   This may be called in interrupt context to queue a packet to
 *   be freed later.
 *   Assumes pkt_data is 32-bit aligned. Uses the first word of
 *   the freed data as a "next" pointer for the free list.
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_free_enqueue(
    int unit,
    void *pkt_data)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(pkt_data, _SHR_E_PARAM, "packet data");
    SHR_IF_ERR_EXIT(_bcm_common_rx_free_enqueue(unit, pkt_data));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get maximum cos queue number for the device.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit   - Relevant unit
 *   \param [out] cosq  - Maximum queue priority
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_max_get(
    int unit,
    bcm_cos_queue_t * cosq)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(cosq, _SHR_E_PARAM, "cosq");
    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_max_get(unit, cosq));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get number of packets awaiting processing in the specific device/queue.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit   - Relevant unit
 *   \param [in] cosq   - Maximum queue priority
 *   \param [out] packet_count - Number of packets awaiting processing
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_packet_count_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *packet_count)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(packet_count, _SHR_E_PARAM, "packet_count");
    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_packet_count_get(unit, cosq, packet_count));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the configured CPU RX rate in pps.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Relevant unit
 *   \param [out] pps - RX rate in packets per second rate
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_rate_get(
    int unit,
    int *pps)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(pps, _SHR_E_PARAM, "pps");
    SHR_IF_ERR_EXIT(_bcm_common_rx_rate_get(unit, pps));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Rx scheduler registration function.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit      - Relevant unit
 *   \param [out] sched_cb - Rx scheduler routine
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_sched_register(
    int unit,
    bcm_rx_sched_cb sched_cb)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(sched_cb, _SHR_E_PARAM, "sched_cb");
    SHR_IF_ERR_EXIT(_bcm_common_rx_sched_register(unit, sched_cb));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Rx scheduler de-registration function.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit      - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_sched_unregister(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_sched_unregister(unit));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Rx started units iteration routine.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit       - Relevant unit
 *   \param [out] unit_next - Relevant unit
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_unit_next_get(
    int unit,
    int *unit_next)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(unit_next, _SHR_E_PARAM, "unit_next");
    SHR_IF_ERR_EXIT(_bcm_common_rx_unit_next_get(unit, unit_next));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get number of packet that can be rx scheduled until system hits queue rx rate limit.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit       - Relevant unit
 *   \param [in] cosq       - Queue priority
 *   \param [out] packet_tokens - Maximum number of packets that can be  scheduled
 *
 * \return
 *   Negative in case of an error. See shr_error_e
 *   Zero in case of NO ERROR
 *
 * \remark
 *   None
 *
 * \see
 *   None
 */
int
bcm_dnx_rx_queue_rate_limit_status_get(
    int unit,
    bcm_cos_queue_t cosq,
    int *packet_tokens)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(packet_tokens, _SHR_E_PARAM, "packet_tokens");
    SHR_IF_ERR_EXIT(_bcm_common_rx_queue_rate_limit_status_get(unit, cosq, packet_tokens));

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert FTMH action type to char string.
 * \param [in] action_type - FTMH action type
 * \return
 *   Name of FTMH action type
 * \remark
 *   None
 * \see
 *   None
 */
static char *
dnx_rx_dump_ftmh_action_type_to_string(
    bcm_pkt_dnx_ftmh_action_type_t action_type)
{
    char *str = NULL;
    switch (action_type)
    {
        case bcmPktDnxFtmhActionTypeForward:
            str = "Forward";
            break;
        case bcmPktDnxFtmhActionTypeSnoop:
            str = "Snoop";
            break;
        case bcmPktDnxFtmhActionTypeInboundMirror:
            str = "InMirror";
            break;
        case bcmPktDnxFtmhActionTypeOutboundMirror:
            str = "OutMirror";
            break;
        case bcmPktDnxFtmhActionTypeMirror:
            str = "Mirror";
            break;
        case bcmPktDnxFtmhActionTypeStatisticalSampling:
            str = "StatSampling";
            break;
        default:
            str = "Unknown";
    }
    return str;
}

/**
 * \brief - Convert OTSH type to char string.
 * \param [in] otsh_type - OTSH type
 * \return
 *   Name of type in OTSH header
 * \remark
 *   None
 * \see
 *   None
 */
static char *
dnx_rx_dump_otsh_type_to_string(
    bcm_pkt_dnx_otsh_type_t otsh_type)
{
    char *str = NULL;
    switch (otsh_type)
    {
        case bcmPktDnxOtshTypeOam:
            str = "Oam";
            break;
        case bcmPktDnxOtshType1588v2:
            str = "1588v2";
            break;
        case bcmPktDnxOtshTypeReserved:
            str = "Reserved";
            break;
        case bcmPktDnxOtshTypeLatency:
            str = "Latency";
            break;
        default:
            str = "Unknown";
    }
    return str;
}

/**
 * \brief - Convert OTSH OAM-Sub-Type to char string.
 * \param [in] otsh_subtype - OTSH OAM-Sub-Type
 * \return
 *   Name of otsh_subtype in OTSH header
 * \remark
 *   None
 * \see
 *   None
 */
static char *
dnx_rx_dump_otsh_oam_subtype_to_string(
    bcm_pkt_dnx_otsh_oam_subtype_t otsh_subtype)
{
    char *str = NULL;
    switch (otsh_subtype)
    {
        case bcmPktDnxOtshOamSubtypeNone:
            str = "None";
            break;
        case bcmPktDnxOtshOamSubtypeLm:
            str = "Lm";
            break;
        case bcmPktDnxOtshOamSubtypeDm1588:
            str = "Dm1588";
            break;
        case bcmPktDnxOtshOamSubtypeDmNtp:
            str = "DmNtp";
            break;
        case bcmPktDnxOtshOamSubtypeOamDefault:
            str = "Default";
            break;
        case bcmPktDnxOtshOamSubtypeLoopback:
            str = "Loopback";
            break;
        case bcmPktDnxOtshOamSubtypeEcn:
            str = "Ecn";
            break;
        default:
            str = "Unknown";
    }
    return str;
}

/**
 * \brief - Dump system header of type ftmh lb extension.
 * \param [in] lb_extension - LB extension header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_lb_extension_dump(
    bcm_pkt_dnx_ftmh_lb_extension_t * lb_extension)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("----------------------------------------------------------------\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                           LB_Key_Ext\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                        LB_Key:    %8u (0x%x)\n\r"), lb_extension->lb_key,
                 lb_extension->lb_key));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type ftmh stack extension.
 * \param [in] stack_extension - Stack extension header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_stack_extension_dump(
    bcm_pkt_dnx_ftmh_stack_extension_t * stack_extension)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("----------------------------------------------------------------\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                           Stacking_Ext\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META(" Stacking-Route-History-Bitmap:    %8u (0x%x)\n\r"),
                 stack_extension->stack_route_history_bmp, stack_extension->stack_route_history_bmp));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type ftmh dest extension.
 * \param [in] dest_extension - Dest extension header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_dest_extension_dump(
    bcm_pkt_dnx_ftmh_dest_extension_t * dest_extension)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("----------------------------------------------------------------\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                           TM_Dst_Ext\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                TM_Destination:    %8u (0x%x)\n\r"), dest_extension->dst_sysport,
                 dest_extension->dst_sysport));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type ftmh flow_id extension.
 * \param [in] flow_id_extension - Flow ID extension header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_flow_id_extension_dump(
    bcm_pkt_dnx_ftmh_flow_id_extension_t * flow_id_extension)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("----------------------------------------------------------------\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                           FLOW_ID_Ext\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                       Flow_ID:    %8u (0x%x)\n\r"), flow_id_extension->flow_id,
                 flow_id_extension->flow_id));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                  Flow_Profile:    %8u (0x%x)\n\r"), flow_id_extension->flow_profile,
                 flow_id_extension->flow_profile));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type ftmh.
 * \param [in] ftmh - FTMH header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_ftmh_dump(
    bcm_pkt_dnx_ftmh_t * ftmh)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("================================================================\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                           FTMH BASE\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                   Packet-Size:    %8u (0x%04x)\n\r"), ftmh->packet_size,
                 ftmh->packet_size));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                 Traffic-Class:    %8u (0x%04x)\n\r"), ftmh->prio, ftmh->prio));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                  Src-Sys-Port:    %8u (0x%04x)\n\r"), ftmh->src_sysport,
                 ftmh->src_sysport));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                        PP-DSP:    %8u (0x%04x)\n\r"), ftmh->dst_port, ftmh->dst_port));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                            DP:    %8u (0x%04x)\n\r"), ftmh->ftmh_dp, ftmh->ftmh_dp));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                TM-action-type:%12s (0x%04x)\n\r"),
                 dnx_rx_dump_ftmh_action_type_to_string(ftmh->action_type), ftmh->action_type));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("            Out-mirror-disable:    %8u (0x%04x)\n\r"), ftmh->out_mirror_disable,
                 ftmh->out_mirror_disable));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("               TM-action-is-MC:    %8u (0x%04x)\n\r"), ftmh->is_mc_traffic,
                 ftmh->is_mc_traffic));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                MCID-or-OutLif:    %8u (0x%04x)\n\r"), ftmh->multicast_id,
                 ftmh->multicast_id));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("               DSP-ext-present:    %8u (0x%04x)\n\r"), ftmh->dest_ext.valid,
                 ftmh->dest_ext.valid));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                           CNI:    %8u (0x%04x)\n\r"), ftmh->cni, ftmh->cni));
    if (ftmh->lb_ext.valid)
    {
        dnx_rx_dump_ftmh_lb_extension_dump(&(ftmh->lb_ext));
    }
    if (ftmh->stack_ext.valid)
    {
        dnx_rx_dump_ftmh_stack_extension_dump(&(ftmh->stack_ext));
    }
    if (ftmh->dest_ext.valid)
    {
        dnx_rx_dump_ftmh_dest_extension_dump(&(ftmh->dest_ext));
    }
    if (ftmh->flow_id_ext.valid)
    {
        dnx_rx_dump_ftmh_flow_id_extension_dump(&(ftmh->flow_id_ext));
    }
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type internal.
 * \param [in] internal - Internal header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_internal_dump(
    bcm_pkt_dnx_internal_t * internal)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("================================================================\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                     Internal Header\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("            VSI/Out-RIF/In-RIF:    %8u (0x%04x)\n\r"), internal->forward_domain,
                 internal->forward_domain));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("       CPU-Trap-Code-Qualifier:    %8u (0x%04x)\n\r"), internal->trap_qualifier,
                 internal->trap_qualifier));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                 CPU-Trap-Code:    %8u (0x%04x)\n\r"), internal->trap_id,
                 internal->trap_id));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header of type OTSH.
 * \param [in] otsh - OAM Time-Stamp header structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_otsh_dump(
    bcm_pkt_dnx_otsh_t * otsh)
{
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("================================================================\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                      OAM-TS Header\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT,
                (BSL_META("                         Type:%12s (0x%04x)\n\r"),
                 dnx_rx_dump_otsh_type_to_string(otsh->otsh_type), otsh->otsh_type));
    switch (otsh->otsh_type)
    {
        case bcmPktDnxOtshTypeOam:
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                 OAM-Sub-Type:%12s (0x%04x)\n\r"),
                         dnx_rx_dump_otsh_oam_subtype_to_string(otsh->oam_sub_type), otsh->oam_sub_type));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                     MEP-Type:%12s (0x%04x)\n\r"),
                         dnx_rx_dump_otsh_oam_subtype_to_string(otsh->oam_sub_type), otsh->oam_sub_type));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                  OAM-TS-Data:  0x%08x\n\r"), COMPILER_64_HI(otsh->oam_ts_data)));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                                0x%08x\n\r"), COMPILER_64_LO(otsh->oam_ts_data)));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                       Offset:    %8u (0x%04x)\n\r"), otsh->offset, otsh->offset));
            break;
        case bcmPktDnxOtshType1588v2:
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                   TP-Command:    %8u (0x%04x)\n\r"), otsh->tp_cmd, otsh->tp_cmd));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("             TS-Encapsulation:    %8u (0x%04x)\n\r"), otsh->ts_encap,
                         otsh->ts_encap));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                  OAM-TS-Data:  0x%08x\n\r"), COMPILER_64_HI(otsh->oam_ts_data)));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                                0x%08x\n\r"), COMPILER_64_LO(otsh->oam_ts_data)));
            LOG_VERBOSE(BSL_LS_BCM_PKT,
                        (BSL_META("                       Offset:    %8u (0x%04x)\n\r"), otsh->offset, otsh->offset));
            break;
        default:
            break;
    }

    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("\n\r")));
}

/**
 * \brief - Dump system header stack.
 * \param [in] pkt - Packet structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_system_header_stack(
    bcm_pkt_t * pkt)
{
    int i;

    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("================================================================\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("                                   RX Parsing Result\n\r")));
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("           Total Headers Count:    %8u\n\r"), pkt->dnx_header_count));

    for (i = 0; i < pkt->dnx_header_count; i++)
    {
        switch (pkt->dnx_header_stack[i].type)
        {
            case bcmPktDnxTypeFtmh:
                dnx_rx_dump_ftmh_dump((bcm_pkt_dnx_ftmh_t *) & (pkt->dnx_header_stack[i].ftmh));
                break;
            case bcmPktDnxTypeInternals:
                dnx_rx_dump_internal_dump(&(pkt->dnx_header_stack[i].internal));
                break;
            case bcmPktDnxTypeOtsh:
                dnx_rx_dump_otsh_dump(&(pkt->dnx_header_stack[i].otsh));
                break;
            default:
                break;
        }
    }
    LOG_VERBOSE(BSL_LS_BCM_PKT, (BSL_META("================================================================\n\r")));
    return;
}

/**
 * \brief - Dump raw packet data.
 * \param [in] unit - Device number.
 * \param [in] pkt - Packet structure
 * \return
 *   None
 * \remark
 *   None
 * \see
 *   None
 */
static void
dnx_rx_dump_raw_packet_data(
    int unit,
    bcm_pkt_t * pkt)
{
    int i;
    char *packet_ptr;
    char *ch_ptr;
    uint32 buf_size = 0;

    if (bsl_check(bslLayerBcm, bslSourceRx, bslSeverityVerbose, unit))
    {
        LOG_VERBOSE(BSL_LS_BCM_RX,
                    (BSL_META_U(unit,
                                "RX: (Egress) port %d, cos %d, tot len %d, reason %04x\n\r"),
                     BCM_GPORT_LOCAL_GET(pkt->dst_gport), pkt->cos, pkt->tot_len, pkt->rx_reason));

        packet_ptr = sal_alloc(3 * pkt->tot_len, "Packet print");
        buf_size = 3 * pkt->tot_len;
        if (packet_ptr != NULL)
        {
            ch_ptr = packet_ptr;
            /*
             * Dump the packet data block
             */
            for (i = 0; i < pkt->tot_len; i++)
            {
                if ((i % 4) == 0)
                {
                    sal_snprintf(ch_ptr, buf_size, " ");
                    ch_ptr++;
                    buf_size--;
                }
                if ((i % 32) == 0)
                {
                    sal_snprintf(ch_ptr, buf_size, "\n");
                    ch_ptr++;
                    buf_size--;
                }

                sal_snprintf(ch_ptr, buf_size, "%02x", (pkt->_pkt_data.data)[i]);
                ch_ptr += 2;
                buf_size -= 2;
            }
            sal_snprintf(ch_ptr, buf_size, "\n");
            LOG_VERBOSE(BSL_LS_BCM_RX, (BSL_META_U(unit, "%s"), packet_ptr));
            sal_free(packet_ptr);
        }
    }
}

/**
 * \brief
 * Convert the bytes stream in network byte order to the type of array of uint32
 *
 * \param [in] unit  - Device number
 * \param [in] bytes_stream  - Pointer to the bytes stream in network byte order
 * \param [in] btyes_offset - The first byte to convert (the next one is 'btye+1')
 * \param [in] nof_bytes - Number of bytes to get from bytes stream and to array of uint32
 * \param [out] val - Pointer to array of uint32
 * \return
 *   None
 * \remark
 *   Example:
 *     bytes_stream: 0x00112233445566778899aabbccddeeff
 *     btyes_offset: 10
 *     nof_bytes:    5
 *
 *     outputs:
 *      var[0] = 0xbbccddee
 *      var[1] = 0xaa
 * \see
 *   None
 */
static shr_error_e
dnx_rx_byte_string_to_uint32(
    int unit,
    uint8 *bytes_stream,
    int btyes_offset,
    int nof_bytes,
    uint32 *val)
{
    int i;
    int j;
    int nof_words;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(bytes_stream, _SHR_E_PARAM, "bytes_stream");
    SHR_NULL_CHECK(val, _SHR_E_PARAM, "val");

    i = nof_bytes;
    j = 0;
    nof_words = BYTES2WORDS(nof_bytes);

    /** Move to start byte position */
    bytes_stream += btyes_offset;

    while (nof_words && (i >= 4))
    {
        *val++ =
            ((bytes_stream[i - 4] << 24) | (bytes_stream[i - 3] << 16) | (bytes_stream[i - 2] << 8) |
             bytes_stream[i - 1]);
        i -= 4;
        nof_words--;
    }
    *val = 0;
    while (i)
    {
        *val |= (bytes_stream[i - 1] << (j * 8));
        j++;
        i--;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Get UDH header length per type
 *
 * \param [in] unit  - Unit id
 * \param [in] udh_data_type - type of UDH data
 * \return
 *   Lenght of UDH data
 * \remark
 *   None
 * \see
 *   None
 */
static uint32
dnx_rx_udh_data_length_per_type(
    int unit,
    uint8 udh_data_type)
{
    uint32 udh_data_length = 0;
    switch (udh_data_type)
    {
        case 0:
            udh_data_length = DNX_DATA_MAX_FIELD_UDH_TYPE_0_LENGTH;
            break;
        case 1:
            udh_data_length = DNX_DATA_MAX_FIELD_UDH_TYPE_1_LENGTH;
            break;
        case 2:
            udh_data_length = DNX_DATA_MAX_FIELD_UDH_TYPE_2_LENGTH;
            break;
        case 3:
            udh_data_length = DNX_DATA_MAX_FIELD_UDH_TYPE_3_LENGTH;
            break;
        default:
            break;
    }

    return udh_data_length;
}

/**
* \brief
*  parser to FTMH headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \param [out] is_tsh_en  -  indicates whether tsh header is following
* \param [out] is_inter_hdr_en  -  indicates whether internal header is following
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_ftmh(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info,
    uint8 *is_tsh_en,
    uint8 *is_inter_hdr_en)
{
    uint32 lb_key_ext_size;
    uint32 stacking_ext_size;
    uint32 bier_bfr_ext_size;
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN];
    uint32 offset, nof_bits;
    uint32 fld_val;
    uint32 pkt_offset;
    bcm_pkt_dnx_ftmh_t *ftmh;
    bcm_pkt_dnx_t *curr_dnx_header;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read LB_KEY SIZE, Stacking size */
    dnx_rx_parser_info.ftmh_lb_key_size.get(unit, &lb_key_ext_size);
    dnx_rx_parser_info.ftmh_stacking_ext_size.get(unit, &stacking_ext_size);

    pkt_offset = packet_info->header_length;
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeFtmh;
    ftmh = &curr_dnx_header->ftmh;
    packet_info->dnx_header_count++;

    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH, BASE_HEADER_SIZE);
    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
    /** Move to start byte of next header, also the total length of system headers */
    pkt_offset += fld_val;

    /** FTMH.Packet-Size*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, PACKET_SIZE, fld_val);
    ftmh->packet_size = fld_val;

    /** FTMH.Traffic-Class*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TRAFFIC_CLASS, fld_val);
    ftmh->prio = fld_val;
    packet_info->tc = fld_val;

    /** FTMH.Source-System-Port-Aggregate */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, SRC_SYS_PORT_AGGREGATE, fld_val);
    ftmh->src_sysport = fld_val;
    packet_info->src_sys_port = fld_val;

    /** FTMH.PP-DSP */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, PP_DSP, fld_val);
    ftmh->dst_port = fld_val;
    packet_info->otm_port = fld_val;

    /** FTMH.Drop-Precedence */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, DROP_PRECEDENCE, fld_val);
    ftmh->ftmh_dp = fld_val;
    packet_info->dp = fld_val;

    /** FTMH.TM-Action-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TM_ACTION_TYPE, fld_val);
    switch (fld_val)
    {
        case 0:
            ftmh->action_type = bcmPktDnxFtmhActionTypeForward;
            packet_info->action_type = bcmPktDnxFtmhActionTypeForward;
            break;
        case 1:
            ftmh->action_type = bcmPktDnxFtmhActionTypeSnoop;
            packet_info->action_type = bcmPktDnxFtmhActionTypeSnoop;
            break;
        case 2:
            ftmh->action_type = bcmPktDnxFtmhActionTypeMirror;
            packet_info->action_type = bcmPktDnxFtmhActionTypeMirror;
            break;
        case 3:
            ftmh->action_type = bcmPktDnxFtmhActionTypeStatisticalSampling;
            packet_info->action_type = bcmPktDnxFtmhActionTypeStatisticalSampling;
            break;
        default:
            break;
    }

    /** FTMH.TM-Action-Is-MC */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TM_ACTION_IS_MC, fld_val);
    packet_info->is_mc_traffic = fld_val;
    ftmh->is_mc_traffic = fld_val;

    /** FTMH.Multicast-ID-or-MC-REP-IDX-or-OutLIF */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, OUTLIF, fld_val);
    packet_info->outlif_mcid = fld_val;
    if (ftmh->is_mc_traffic == TRUE)
    {
        ftmh->multicast_id = fld_val;
    }
    else
    {
        if (fld_val == 0)
        {
            /** an OutLIF value of '0' is treated at the egress as no encapsulation (i.e., packet is destined to a port level interface) */
            ftmh->out_vport = 0;
        }
        else
        {
            ftmh->out_vport = fld_val;
        }
    }

    /** FTMH.CNI */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, CNI, fld_val);
    ftmh->cni = fld_val;

    /** FTMH.ECN-Enable -- ignore */
    /** FTMH.TM profile -- ignore */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TM_PROFILE, fld_val);
    /** JR1 FTMH.Outbound-mirror-disable is from JR2 FTMH.TM profile[1] */
    ftmh->out_mirror_disable = (fld_val & DNX_RX_PARSE_HEADER_FTMH_TM_PROFILE_BIT_1);
    /** FTMH.Visibility -- ignore */

    /** FTMH.PPH-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, INTERNAL_HEADER_EN, fld_val);
    *is_inter_hdr_en = fld_val;
    /** TS existence is indicated by bit 6 of FTMH */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TSH_EN, fld_val);
    *is_tsh_en = fld_val;

    /** FTMH.TM-Destination-Extension-Present */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TM_DEST_EXT_PRESENT, fld_val);
    ftmh->dest_ext.valid = fld_val;

    /** FTMH.Application-Specific-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, ASE_PRESENT, fld_val);
    ftmh->ase_ext.valid = fld_val;

    /** FTMH.Flow-ID-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, FLOW_ID_EXT_PRESENT, fld_val);
    ftmh->flow_id_ext.valid = fld_val;

    /** FTMH.BIER-BFR-Extension-Size */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, BIER_BFR_EXT_PRESENT, fld_val);
    if (fld_val)
    {
        bier_bfr_ext_size = 2;
    }
    else
    {
        bier_bfr_ext_size = 0;
    }

    /** FTMH LB-Key Extension */
    if (lb_key_ext_size > 0)
    {
        ftmh->lb_ext.valid = TRUE;
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, lb_key_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += lb_key_ext_size;

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(lb_key_ext_size);
        /** Start bit of FTMH LB-Key Extension */
        offset = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        packet_info->lb_key = fld_val;
        ftmh->lb_ext.lb_key = fld_val;
    }

    /** FTMH Stacking Extension */
    if (stacking_ext_size > 0)
    {
        ftmh->stack_ext.valid = TRUE;
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, stacking_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += stacking_ext_size;
        /** Move backward to start position Stacking-Route-History-Bitmap in FTMH Stacking Extension Header */
        offset = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH, STACK_ROUTE_HISTORY_BMP_OFFSET);

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(stacking_ext_size);
        nof_bits -= offset;

        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));

        packet_info->stacking_ext = fld_val;
        ftmh->stack_ext.stack_route_history_bmp = fld_val;
    }

    /** FTMH BIER BFR Extension -- bypass */
    if (bier_bfr_ext_size > 0)
    {
        /** Move to start byte of next header */
        pkt_offset += bier_bfr_ext_size;
    }

    /** FTMH TM Destination Extension */
    if (ftmh->dest_ext.valid)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH, TM_DST_SIZE);
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
        /** Move to start byte of next header */
        pkt_offset += fld_val;

        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, TM_DST, fld_val);
        packet_info->dsp_ext = fld_val;
        ftmh->dest_ext.dst_sysport = fld_val;
    }

    /** FTMH Application Specific Extension Header -- bypass */
    if ((ftmh->ase_ext.valid))
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH, ASE_HEADER_SIZE);
        /** Move to start byte of next header */
        pkt_offset += fld_val;
    }

    /*
     * FTMH Flow-ID Extension
     */
    if (ftmh->flow_id_ext.valid == TRUE)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH, FLOW_ID_HEADER_SIZE);
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
        /** Move to start byte of next header */
        pkt_offset += fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, FLOW_ID, fld_val);
        ftmh->flow_id_ext.flow_id = fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH, FLOW_PROFILE, fld_val);
        ftmh->flow_id_ext.flow_profile = fld_val;
    }

    packet_info->header_length = pkt_offset;
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to Internal headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \param [in] src_sys_port  -  source system port in the case that packet is punted to CPU by OAMP

* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_internal(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info,
    bcm_gport_t src_sys_port)
{
    uint32 pph_lif_ext_size[DNX_RX_PARSE_HEADER_LIF_EXT_TYPE] = { 0 };
    uint32 inter_hdr_base_size;
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN];
    uint32 offset, nof_bits;
    uint32 fld_val;
    uint32 pkt_offset;
    bcm_pkt_dnx_internal_t *internal;
    bcm_pkt_dnx_t *curr_dnx_header;
    uint8 udh_en = FALSE;
    uint32 fhei_type = 0;
    uint32 start_type = 0;
    uint32 layer_index = 0;
    uint32 app_ext = 0;
    uint32 lif_ext = 0;
    uint32 learn_ext = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read internal header base size */
    dnx_rx_parser_info.pph_base_size.get(unit, &inter_hdr_base_size);
    dnx_rx_parser_info.user_header_enable.get(unit, &udh_en);

    /** If packet is punted to CPU by OAMP, no UDH at there first layer of headers */
    if (packet_info->is_punted && packet_info->src_sys_port == src_sys_port)
    {
        udh_en = FALSE;
    }

    /** Increase count in header stack */
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeInternals;
    internal = &curr_dnx_header->internal;
    packet_info->dnx_header_count++;

    pkt_offset = packet_info->header_length;
    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, inter_hdr_base_size, system_header));
    /** Move to start byte of next header */
    pkt_offset += inter_hdr_base_size;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_1;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_2;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    start_type = fld_val;

    switch (inter_hdr_base_size)
    {
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_9:
            offset = DNX_RX_PARSE_HEADER_INTERNAL_3;
            nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_4;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_10:
            offset = DNX_RX_PARSE_HEADER_INTERNAL_5;
            nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_6;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_12:
            offset = DNX_RX_PARSE_HEADER_INTERNAL_7;
            nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_8;
            SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
            break;
        default:
            break;
    }
    layer_index = fld_val;

    if ((start_type == DNX_RX_PARSE_HEADER_INTERNAL_VALUE_1) && (layer_index == DNX_RX_PARSE_HEADER_INTERNAL_VALUE_0))
    {
        /** INTERNAL.Forwarding-Layer-Additional-Info */
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, INTERNAL, FAI, fld_val);
        /*
         * MAP FAI to unknown_addr per forwarding_additional_info_config_map[]
         * FAI[0] is unknown address bit when FAI is 1,3,5,7,9,11,13,15
         */
        packet_info->unknown_addr = fld_val & 0x1;
    }

    /** INTERNAL.In-LIF */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, INTERNAL, INLIF, fld_val);
    packet_info->internal_inlif_inrif = fld_val;

    /** INTERNAL.Forward_Domain */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, INTERNAL, FORWARD_DOMAIN, fld_val);
    internal->forward_domain = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_9;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_10;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    app_ext = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_11;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_12;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    learn_ext = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_13;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_14;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    lif_ext = fld_val;

    /** INTRENAL Extension*/
    switch (app_ext)
    {
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_1:
            /** Bypass */
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_VALUE_3;
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_2:
            /** FHEI-Size == 5B, FHEI-Type == Trap/Sniff */
            SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                            (unit, pkt, pkt_offset, DNX_RX_PARSE_HEADER_INTERNAL_VALUE_5, system_header));
            /** Move to start byte of next header */
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_VALUE_5;
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_SZ1_TRAP_SNIFF, FHEI_TYPE, fhei_type);
            /** FHEI.Type 4'b0101: Trap/Snoop/Mirror/Statistical-Sampling */
            if (fhei_type == 0x5)
            {
                if ((packet_info->action_type != bcmPktDnxFtmhActionTypeSnoop)
                    && (packet_info->action_type != bcmPktDnxFtmhActionTypeMirror)
                    && (packet_info->action_type != bcmPktDnxFtmhActionTypeStatisticalSampling))
                {
                    packet_info->is_trapped = TRUE;
                }
                DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_SZ1_TRAP_SNIFF, QUALIFIER, fld_val);
                internal->trap_qualifier = fld_val;
                packet_info->cpu_trap_qualifier = fld_val;
                DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_SZ1_TRAP_SNIFF, CODE, fld_val);
                internal->trap_id = fld_val;
                packet_info->cpu_trap_code = fld_val;
            }
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_VALUE_3:
            /** Bypass */
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_VALUE_8;
            break;
        default:
            break;
    }

    if ((lif_ext > 0) && (lif_ext < DNX_RX_PARSE_HEADER_LIF_EXT_TYPE))
    {
        /** Read 1xOutLIF, 2xOutLIF, 3xOutLIF, InLIF + InLIF-Profile extension header size */
        dnx_rx_parser_info.out_lif_x_1_ext_size.get(unit, &pph_lif_ext_size[1]);
        dnx_rx_parser_info.out_lif_x_2_ext_size.get(unit, &pph_lif_ext_size[2]);
        dnx_rx_parser_info.out_lif_x_3_ext_size.get(unit, &pph_lif_ext_size[3]);
        dnx_rx_parser_info.in_lif_ext_size.get(unit, &pph_lif_ext_size[4]);
        pkt_offset += pph_lif_ext_size[lif_ext];
    }

    if (learn_ext == TRUE)
    {
        pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_VALUE_19;
    }

    /** UDH Header */
    if (udh_en)
    {
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                        (unit, pkt, pkt_offset, DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE, system_header));
        pkt_offset += DNX_DATA_MAX_HEADERS_UDH_BASE_SIZE;

        offset = DNX_RX_PARSE_HEADER_INTERNAL_15;
        nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_16;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        offset = DNX_RX_PARSE_HEADER_INTERNAL_17;
        nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_18;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        offset = DNX_RX_PARSE_HEADER_INTERNAL_19;
        nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_20;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);

        offset = DNX_RX_PARSE_HEADER_INTERNAL_21;
        nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_22;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        pkt_offset += dnx_rx_udh_data_length_per_type(unit, fld_val);
    }

    packet_info->header_length = pkt_offset;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to system headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \param [in] header_type_out  -  out header type of received port
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info,
    int header_type_out)
{
    uint8 is_inter_hdr_en = FALSE;
    uint8 is_tsh_en = FALSE;
    bcm_gport_t source_system_port = 0;
    uint32 oamp_system_port[2] = { 0 };

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read OAMP system ports */
    dnx_rx_parser_info.oamp_system_port_0.get(unit, &oamp_system_port[0]);
    dnx_rx_parser_info.oamp_system_port_1.get(unit, &oamp_system_port[1]);

    packet_info->dnx_header_count = 0;
    if (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU)
    {
        /** Start with ETH1, ETH1 + FTMH + ... */
        packet_info->header_length = DNX_RX_PARSE_HEADER_ETH1_SIZE;
    }
    else
    {
        /** Start with FTMH, given the packet is trapped to CPU */
        packet_info->header_length = 0;
    }

    SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh(unit, pkt, pkt_len, packet_info, &is_tsh_en, &is_inter_hdr_en));

    /** Time-Stamp Header -- bypass */
    if (is_tsh_en == TRUE)
    {
        /** Move to start byte of next header */
        packet_info->header_length += DNX_RX_PARSE_HEADER_DATA_GET(unit, TSH, BASE_HEADER_SIZE);
    }

    if ((oamp_system_port[0] && (packet_info->src_sys_port == oamp_system_port[0]))
        || (oamp_system_port[1] && (packet_info->src_sys_port == oamp_system_port[1])))
    {
        packet_info->is_punted = 1;
        source_system_port = packet_info->src_sys_port;
    }

    if (is_inter_hdr_en)
    {
        SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal(unit, pkt, pkt_len, packet_info, source_system_port));
    }

    if (packet_info->is_punted && packet_info->is_trapped)
    {
        is_inter_hdr_en = FALSE;
        is_tsh_en = FALSE;

        SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh(unit, pkt, pkt_len, packet_info, &is_tsh_en, &is_inter_hdr_en));
        /** Time-Stamp Header -- bypass */
        if (is_tsh_en == TRUE)
        {
            /** Move to start byte of next header */
            packet_info->header_length += DNX_RX_PARSE_HEADER_DATA_GET(unit, TSH, BASE_HEADER_SIZE);
        }
        if (is_inter_hdr_en)
        {
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal(unit, pkt, pkt_len, packet_info, source_system_port));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to legacy FTMH headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \param [out] is_tsh_en  -  ture if tsh is present
* \param [out] is_inter_hdr_en  -  ture is internal header is present
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_ftmh_legacy(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info,
    uint8 *is_tsh_en,
    uint8 *is_inter_hdr_en)
{
    uint32 lb_key_ext_size = 0;
    uint32 stacking_ext_size = 0;
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN] = { 0 };
    uint32 offset = 0, nof_bits = 0;
    uint32 fld_val = 0;
    uint32 pkt_offset = 0;
    bcm_pkt_dnx_ftmh_t *ftmh = NULL;
    bcm_pkt_dnx_t *curr_dnx_header = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read LB_KEY SIZE, Stacking size ,internal header base size */
    dnx_rx_parser_info.ftmh_lb_key_size.get(unit, &lb_key_ext_size);
    dnx_rx_parser_info.ftmh_stacking_ext_size.get(unit, &stacking_ext_size);

    pkt_offset = packet_info->header_length;
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeFtmh;
    ftmh = &curr_dnx_header->ftmh;
    packet_info->dnx_header_count++;

    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH_LEGACY, BASE_HEADER_SIZE);
    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
    /** Move to start byte of next header, also the total length of system headers */
    pkt_offset += fld_val;

    /** FTMH.Packet-Size*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, PACKET_SIZE, fld_val);
    ftmh->packet_size = fld_val;

    /** FTMH.Traffic-Class*/
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, TRAFFIC_CLASS, fld_val);
    ftmh->prio = fld_val;
    packet_info->tc = ftmh->prio;

    /** FTMH.Source-System-Port-Aggregate */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, SRC_SYS_PORT_AGGREGATE, fld_val);
    ftmh->src_sysport = fld_val;
    packet_info->src_sys_port = fld_val;

    /** FTMH.PP-DSP */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, PP_DSP, fld_val);
    ftmh->dst_port = fld_val;
    packet_info->otm_port = fld_val;

    /** FTMH.Drop-Precedence */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, DROP_PRECEDENCE, fld_val);
    ftmh->ftmh_dp = fld_val;
    packet_info->dp = fld_val;

    /** FTMH.TM-Action-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, TM_ACTION_TYPE, fld_val);
    switch (fld_val)
    {
        case 0:
            ftmh->action_type = bcmPktDnxFtmhActionTypeForward;
            packet_info->action_type = bcmPktDnxFtmhActionTypeForward;
            break;
        case 1:
            ftmh->action_type = bcmPktDnxFtmhActionTypeSnoop;
            packet_info->action_type = bcmPktDnxFtmhActionTypeSnoop;
            break;
        case 2:
            ftmh->action_type = bcmPktDnxFtmhActionTypeInboundMirror;
            packet_info->action_type = bcmPktDnxFtmhActionTypeInboundMirror;
            break;
        case 3:
            ftmh->action_type = bcmPktDnxFtmhActionTypeOutboundMirror;
            packet_info->action_type = bcmPktDnxFtmhActionTypeOutboundMirror;
            break;
        default:
            break;
    }

    /** FTMH.PPH-Type */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, INTERNAL_HEADER_EN, fld_val);
    *is_inter_hdr_en = fld_val;

    /** TS existence is indicated by bit 6 of FTMH */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, OTSH_EN, fld_val);
    *is_tsh_en = fld_val;

    /** FTMH.Outbound-mirror-disable */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, OUTBOUND_MIRR_DISABLE, fld_val);
    ftmh->out_mirror_disable = fld_val;

    /** FTMH.TM-Action-Is-MC */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, TM_ACTION_IS_MC, fld_val);
    ftmh->is_mc_traffic = fld_val;
    packet_info->is_mc_traffic = fld_val;

    /** FTMH.Multicast-ID-or-MC-REP-IDX-or-OutLIF */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, OUTLIF, fld_val);
    if (ftmh->is_mc_traffic == TRUE)
    {
        ftmh->multicast_id = fld_val;
        packet_info->outlif_mcid = fld_val;
    }
    else
    {
        if (fld_val == 0)
        {
            /** an OutLIF value of '0' is treated at the egress as no encapsulation (i.e., packet is destined to a port level interface) */
            ftmh->out_vport = 0;
        }
        else
        {
            ftmh->out_vport = fld_val;
        }
    }

    /** FTMH.DSP-Extension-Present */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, DSP_EXT_PRESENT, fld_val);
    ftmh->dest_ext.valid = fld_val;

    /** FTMH.CNI */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, CNI, fld_val);
    ftmh->cni = fld_val;

    /** FTMH.ECN-Enable -- ignore */
    /** FTMH.Exclude-source -- ignore */

    /** FTMH LB-Key Extension */
    if (lb_key_ext_size > 0)
    {
        ftmh->lb_ext.valid = TRUE;
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, lb_key_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += lb_key_ext_size;

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(lb_key_ext_size);
        /** Start bit of FTMH LB-Key Extension */
        offset = 0;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        packet_info->lb_key = fld_val;
        ftmh->lb_ext.lb_key = fld_val;
    }

    /** FTMH Stacking Extension */
    if (stacking_ext_size > 0)
    {
        ftmh->stack_ext.valid = TRUE;
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, stacking_ext_size, system_header));
        /** Move to start byte of next header */
        pkt_offset += stacking_ext_size;
        /** Move backward to start position Stacking-Route-History-Bitmap in FTMH Stacking Extension Header */
        offset = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH_LEGACY, STACK_ROUTE_HISTORY_BMP_OFFSET);

        nof_bits = DNX_RX_PARSE_HEADER_BITS_GET(stacking_ext_size);
        nof_bits -= offset;
        SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
        packet_info->stacking_ext = fld_val;
        ftmh->stack_ext.stack_route_history_bmp = fld_val;
    }

    /** FTMH TM Destination Extension */
    if (ftmh->dest_ext.valid)
    {
        fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, FTMH_LEGACY, DSP_EXT_HEADER_SIZE);
        SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
        /** Move to start byte of next header */
        pkt_offset += fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FTMH_LEGACY, DSP_EXT_HEADER, fld_val);
        packet_info->dsp_ext = fld_val;
        ftmh->dest_ext.dst_sysport = fld_val;
    }

    /** FTMH Application Specific Extension Header -- not exist in JR1, bypass */
    ftmh->ase_ext.valid = 0;
    /** FTMH Flow-ID Extension  -- not exist in JR1, bypass */
    ftmh->flow_id_ext.valid = 0;

    packet_info->header_length = pkt_offset;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to legacy OTSH headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_otsh_legacy(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info)
{
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN] = { 0 };
    uint32 offset = 0, nof_bits = 0;
    uint32 fld_val = 0;
    uint32 pkt_offset = 0;
    bcm_pkt_dnx_otsh_t *otsh = NULL;
    bcm_pkt_dnx_t *curr_dnx_header = NULL;
    uint32 oam_ts_data_0;
    uint32 oam_ts_data_1;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    pkt_offset = packet_info->header_length;
    /** Increase count in header stack */
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeOtsh;
    otsh = &curr_dnx_header->otsh;
    packet_info->dnx_header_count++;

    fld_val = DNX_RX_PARSE_HEADER_DATA_GET(unit, OTSH, BASE_HEADER_SIZE);
    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, fld_val, system_header));
    /** Move to start byte of next header */
    pkt_offset += fld_val;

    DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, TYPE, otsh->otsh_type);
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, OFFSET, otsh->offset);

    switch (otsh->otsh_type)
    {
        case bcmPktDnxOtshTypeOam:
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, OAM_SUB_TYPE, otsh->oam_sub_type);
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, MEP_TYPE, otsh->oam_up_mep);
            break;
        case bcmPktDnxOtshType1588v2:
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, TP_COMMAND, otsh->tp_cmd);
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, TS_ENCAPSULATION, fld_val);
            otsh->ts_encap = (uint8) fld_val;
            break;
        default:
            break;
    }

    DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, OAM_TS_DATA_0, oam_ts_data_0);
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, OTSH_LEGACY, OAM_TS_DATA_1, oam_ts_data_1);
    COMPILER_64_SET(curr_dnx_header->otsh.oam_ts_data, oam_ts_data_0, oam_ts_data_1);

    packet_info->header_length = pkt_offset;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to legacy Internal headers
* \param [in] unit  -  Unit id
* \param [in] pkt  -  pointer to packet data buffer
* \param [in] pkt_len  -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_internal_legacy(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info)
{
    uint32 inter_hdr_base_size = 0;
    uint32 system_header[DNX_RX_PARSE_HEADER_MAX_LEN] = { 0 };
    uint32 offset = 0, nof_bits = 0;
    uint32 fld_val = 0;
    uint32 pkt_offset = 0;
    bcm_pkt_dnx_internal_t *internal = NULL;
    bcm_pkt_dnx_t *curr_dnx_header = NULL;
    uint8 is_ingress_trap = FALSE;
    uint32 app_ext = 0;
    uint32 l_ext = 0;
    uint32 e_ext = 0;
    uint32 forwarding_header_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    dnx_rx_parser_info.pph_base_size.get(unit, &inter_hdr_base_size);

    pkt_offset = packet_info->header_length;
    /** Increase count in header stack */
    curr_dnx_header = &(packet_info->dnx_header_stack[packet_info->dnx_header_count]);
    curr_dnx_header->type = bcmPktDnxTypeInternals;
    internal = &curr_dnx_header->internal;
    packet_info->dnx_header_count++;

    SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32(unit, pkt, pkt_offset, inter_hdr_base_size, system_header));
    /** Move to start byte of next header */
    pkt_offset += inter_hdr_base_size;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_1;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_2;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    e_ext = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_3;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_4;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    l_ext = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_5;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_6;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    app_ext = fld_val;

    offset = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_7;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_8;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    packet_info->jr_mode_fwd_code = fld_val;
    if (packet_info->jr_mode_fwd_code == DBAL_ENUM_FVAL_JR_FWD_CODE_CPU_TRAP1)
    {
        is_ingress_trap = TRUE;
        packet_info->is_trapped = TRUE;
    }

    offset = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_9;
    nof_bits = DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_10;
    SHR_IF_ERR_EXIT(utilex_bitstream_get_field(system_header, offset, nof_bits, &fld_val));
    forwarding_header_offset = fld_val;

    /** INTERNAL.Unknown-Address */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, INTERNAL_LEGACY, UNKNOWN_ADDRESS, fld_val);
    packet_info->unknown_addr = (uint8) fld_val;

    /** INTERNAL.In-LIF */
    DNX_RX_PARSE_HEADER_FIELD_GET(unit, INTERNAL_LEGACY, INLIF_INRIF, fld_val);
    packet_info->internal_inlif_inrif = fld_val;

    switch (app_ext)
    {
        case DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_1:
            SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                            (unit, pkt, pkt_offset, DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_3, system_header));
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_3;
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_2:
            SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                            (unit, pkt, pkt_offset, DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_5, system_header));
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_5;
            break;
        case DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_3:
            SHR_IF_ERR_EXIT(dnx_rx_byte_string_to_uint32
                            (unit, pkt, pkt_offset, DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_8, system_header));
            pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_8;
            break;
        default:
            break;
    }

    /** Get trap code and qualifier in case of ingress trap*/
    if (is_ingress_trap == TRUE && app_ext)
    {
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_TRAP_SNIFF_LEGACY, QUALIFIER, fld_val);
        internal->trap_qualifier = fld_val;
        packet_info->cpu_trap_qualifier = fld_val;
        DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_TRAP_SNIFF_LEGACY, CODE, fld_val);
        internal->trap_id = fld_val;
        packet_info->cpu_trap_code = fld_val;
    }
    else if (app_ext)
    {
        internal->trap_id = -1;
        packet_info->cpu_trap_code = -1;

        /** Get CPU trap code qualifier for snooped or mirrored packets */
        if (packet_info->action_type == bcmPktDnxFtmhActionTypeSnoop ||
            packet_info->action_type == bcmPktDnxFtmhActionTypeInboundMirror)
        {
            DNX_RX_PARSE_HEADER_FIELD_GET(unit, FHEI_TRAP_SNIFF_LEGACY, QUALIFIER, fld_val);
            internal->trap_qualifier = fld_val;
            packet_info->cpu_trap_qualifier = fld_val;
        }
    }

    if (e_ext == TRUE)
    {
        pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_3;
    }

    if (l_ext == TRUE)
    {
        pkt_offset += DNX_RX_PARSE_HEADER_INTERNAL_LEGACY_VALUE_5;
    }

    /*
     * If packet is punted by OAMP, skip UDH at both outer layer and inner layer
     * Else, UDH size in JR mode is compensated per INTERNAL.forward-header-offset
     */
    if (!packet_info->is_punted)
    {
        pkt_offset += forwarding_header_offset;
    }

    packet_info->header_length = pkt_offset;

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  parser to legacy system headers
* \param [in] unit    -  Unit id
* \param [in] pkt     -  pointer to packet data buffer
* \param [in] pkt_len     -  total length of packet from DMA
* \param [out] packet_info  -  information from header parser
* \param [in] header_type_out  -  out header type of received port
* \return
*   \retval Non-zero in case of an error
*   \retval Zero in case of NO ERROR
* \remark
*   The procedure works as dnx_rx_system_header_parse but in jericho1 mode.
*   In consideration of RX performance, we file a new procedure for legacy system headers.
* \see
*   None
*/
static shr_error_e
dnx_rx_system_header_parse_legacy(
    int unit,
    uint8 *pkt,
    uint32 pkt_len,
    dnx_rx_parse_headers_t * packet_info,
    int header_type_out)
{
    uint8 is_inter_hdr_en = FALSE;
    uint8 is_tsh_en = FALSE;
    uint32 oamp_system_port[2] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");
    SHR_NULL_CHECK(packet_info, _SHR_E_PARAM, "packet_info");

    /** Read OAMP system ports */
    dnx_rx_parser_info.oamp_system_port_0.get(unit, &oamp_system_port[0]);
    dnx_rx_parser_info.oamp_system_port_1.get(unit, &oamp_system_port[1]);

    packet_info->dnx_header_count = 0;
    if (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU)
    {
        /** Start with ETH1, ETH1 + FTMH + ... */
        packet_info->header_length = DNX_RX_PARSE_HEADER_ETH1_SIZE;
    }
    else
    {
        /** Start with FTMH, given the packet is trapped to CPU */
        packet_info->header_length = 0;
    }

    SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh_legacy
                    (unit, pkt, pkt_len, packet_info, &is_tsh_en, &is_inter_hdr_en));

    if ((oamp_system_port[0] && (packet_info->src_sys_port == oamp_system_port[0]))
        || (oamp_system_port[1] && (packet_info->src_sys_port == oamp_system_port[1])))
    {
        packet_info->is_punted = 1;
    }

    /** Time-Stamp Header */
    if (is_tsh_en == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_otsh_legacy(unit, pkt, pkt_len, packet_info));
    }

    /** Internal Header */
    if (is_inter_hdr_en == TRUE)
    {
        SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal_legacy(unit, pkt, pkt_len, packet_info));
    }

    if (packet_info->is_punted && packet_info->is_trapped)
    {
        is_inter_hdr_en = FALSE;
        is_tsh_en = FALSE;

        SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh_legacy
                        (unit, pkt, pkt_len, packet_info, &is_tsh_en, &is_inter_hdr_en));
        if (is_tsh_en == TRUE)
        {
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_otsh_legacy(unit, pkt, pkt_len, packet_info));
        }
        if (is_inter_hdr_en)
        {
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal_legacy(unit, pkt, pkt_len, packet_info));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
dnx_rx_packet_parse(
    int unit,
    bcm_pkt_t * pkt,
    uint8 device_access_allowed)
{
    static dnx_rx_parse_headers_t packet_info;
    int header_type_out = BCM_SWITCH_PORT_HEADER_TYPE_NONE;
    uint16 vlan_control_tag = 0;
    uint16 tpid = 0;
    uint8 *eth_pkt_header = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");

    dnx_rx_dump_raw_packet_data(unit, pkt);

    sal_memset(&packet_info, 0, sizeof(dnx_rx_parse_headers_t));

    if (BCM_GPORT_IS_LOCAL(pkt->dst_gport))
    {
        header_type_out =
            dnx_data_port.static_add.header_type_get(unit, BCM_GPORT_LOCAL_GET(pkt->dst_gport))->header_type_out;
    }

    if ((header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_CPU)
        || (header_type_out == BCM_SWITCH_PORT_HEADER_TYPE_ENCAP_EXTERNAL_CPU))
    {
        int index = 0;
        bcm_rx_trap_t trap_type;

        if (DNX_RX_PARSE_SYSTEM_HEADERS_MODE_JERICHO2(unit))
        {
            /** JR2 system header parser */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse
                            (unit, pkt->_pkt_data.data, pkt->tot_len, &packet_info, header_type_out));
        }
        else
        {
            /** JR1 system header parser */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_legacy
                            (unit, pkt->_pkt_data.data, pkt->tot_len, &packet_info, header_type_out));
        }

        /** Fill dnx header stack in bcm_pkt_t */
        for (index = 0; index < packet_info.dnx_header_count; index++)
        {
            sal_memcpy(&(pkt->dnx_header_stack[index]), &(packet_info.dnx_header_stack[index]), sizeof(bcm_pkt_dnx_t));
        }
        pkt->dnx_header_count = packet_info.dnx_header_count;

        if (rx_ctl[unit]->cpu_port_priorities != 64)
        {
            pkt->cos = packet_info.tc;
        }
        pkt->prio_int = packet_info.dp;

        /** Including Length of ETH1 for ENCAP_EXTERNAL_CPU */
        pkt->pkt_len = pkt->tot_len - packet_info.header_length;

        if (packet_info.is_trapped == TRUE)
        {
            if (packet_info.is_punted)
            {
                /** Punt or protection packet */
                pkt->rx_reason = packet_info.cpu_trap_code;
                pkt->rx_trap_data = packet_info.cpu_trap_qualifier;
            }
            else
            {
                SHR_IF_ERR_EXIT(bcm_rx_trap_type_from_id_get(unit, 0, packet_info.cpu_trap_code, &trap_type));
                pkt->rx_reason = trap_type;
                if (trap_type == bcmRxTrapUserDefine)
                {
                    pkt->rx_trap_data = packet_info.cpu_trap_code;
                    pkt->src_gport = packet_info.internal_inlif_inrif;
                }
                else
                {
                    pkt->rx_trap_data = packet_info.cpu_trap_qualifier;
                }
            }
        }
        else
        {
            pkt->rx_reason = 0;
            /** If packet is snooped, get cpu trap qualifier */
            if (packet_info.action_type == bcmPktDnxFtmhActionTypeSnoop)
            {
                pkt->rx_trap_data = packet_info.cpu_trap_qualifier;
            }
        }

        if (device_access_allowed)
        {
            bcm_gport_t gport_sysport = 0;
            bcm_gport_t gport_modport = 0;
            int is_spa = FALSE;

            /** Check if FTMH.Source-System-Port-Aggregate is LAG */
            SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, packet_info.src_sys_port, &is_spa));
            if (is_spa)
            {
                uint32 flags = 0;
                int pool = 0;
                int member = 0;

                SHR_IF_ERR_EXIT(dnx_trunk_utils_spa_decode
                                (unit, &pool, &pkt->src_trunk, &member, packet_info.src_sys_port));
                /** Map SPA to its corresponding system gport */
                SHR_IF_ERR_EXIT(bcm_trunk_spa_to_system_phys_port_map_get
                                (unit, flags, packet_info.src_sys_port, &gport_sysport));
            }
            else
            {
                pkt->src_trunk = -1;
                BCM_GPORT_SYSTEM_PORT_ID_SET(gport_sysport, packet_info.src_sys_port);
            }
            /** Get modport gport from sysport gport */
            SHR_IF_ERR_EXIT(bcm_stk_sysport_gport_get(unit, gport_sysport, &gport_modport));
            pkt->src_port = BCM_GPORT_MODPORT_PORT_GET(gport_modport);
            pkt->src_mod = BCM_GPORT_MODPORT_MODID_GET(gport_modport);

            if (packet_info.is_mc_traffic)
            {
                pkt->multicast_group = packet_info.outlif_mcid;
            }
            pkt->dest_port = packet_info.otm_port;
            pkt->stk_load_balancing_number = packet_info.lb_key;
            pkt->stk_dst_gport = packet_info.dsp_ext;
            pkt->stk_route_tm_domains = packet_info.stacking_ext;
            pkt->dst_vport = packet_info.outlif_mcid;
            pkt->src_vport = packet_info.internal_inlif_inrif;

            if (packet_info.unknown_addr)
            {
                pkt->flags2 |= BCM_PKT_F2_UNKNOWN_DEST;
            }

            if (packet_info.action_type == bcmPktDnxFtmhActionTypeSnoop)
            {
                pkt->flags2 |= BCM_PKT_F2_SNOOPED;
            }

            switch (packet_info.jr_mode_fwd_code)
            {
                case 5:
                    pkt->stk_forward = BCM_PKT_STK_FORWARD_MPLS;
                    break;
                case 6:
                    pkt->stk_forward = BCM_PKT_STK_FORWARD_TRILL;
                    break;
                case 9:
                    pkt->stk_forward = BCM_PKT_STK_FORWARD_FCOE;
                    break;
                case 0xb:
                    pkt->stk_forward = BCM_PKT_STK_FORWARD_SNOOP;
                    break;
                case 0xe:
                    pkt->stk_forward = BCM_PKT_STK_FORWARD_TRAFFIC_MANAGEMENT;
                    break;
                default:
                    break;
            }
        }
        else
        {
            pkt->src_port = 0;
            pkt->src_mod = 0;
        }
    }

    /** Update priority and vlan id based on outer vlan tag of pkt */
    eth_pkt_header = pkt->_pkt_data.data + packet_info.header_length;

    /*
     * 0x88a8 S-Tag in IEEE 802.1ad
     * 0x9100 S-Tag in old non-standard 802.1QinQ protocol
     * 0x8100 C-Tag in IEEE 802.1Q
     */
    pkt->rx_untagged = (BCM_PKT_OUTER_UNTAGGED | BCM_PKT_INNER_UNTAGGED);
    pkt->prio_int = 0;
    pkt->vlan = 0;
    tpid = ((uint16) ((eth_pkt_header[12] << 8) | eth_pkt_header[13]));
    vlan_control_tag = ((uint16) ((eth_pkt_header[14] << 8) | eth_pkt_header[15]));
    if ((tpid == 0x88a8) || (tpid == 0x9100))
    {
        uint16 inner_tpid = 0;

        /** Outer tagged */
        pkt->rx_untagged &= ~BCM_PKT_OUTER_UNTAGGED;
        inner_tpid = ((uint16) ((eth_pkt_header[16] << 8) | eth_pkt_header[17]));
        if (inner_tpid == 0x8100)
        {
            /** Double tagged */
            pkt->rx_untagged &= ~BCM_PKT_INNER_UNTAGGED;
        }
        pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
        pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);
    }
    if (tpid == 0x8100)
    {
        /** Inner tagged */
        pkt->rx_untagged &= ~BCM_PKT_INNER_UNTAGGED;
        pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
        pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);
    }

    dnx_rx_dump_system_header_stack(pkt);

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  parser to headers when packets are trapped to CPU port by ETPP MTU trap
 * \param [in] unit    -  Unit id
 * \param [inout] pkt  -  pointer to packet data buffer
 *                     -  pkt->tot_len: total length of received packet
 *                     -  pkt->pkt_len: the length of received packet without parsed headers
 *                     -  pkt->_pkt_data.data: the start pointer of received packet buffer
 *                     -  pkt->dnx_header_count: the number of headers in header stack
 *                     -  pkt->dnx_header_stack: header stack for parsed headers, 9 at most
 * \param [in] is_recycle_append_ftmh    -  indication regarding appending the original FTMH to the recycle packet
 * \return
 *   \retval Non-zero in case of an error
 *   \retval Zero in case of NO ERROR
 * \remark
 *   is_recycle_append_ftmh=TRUE, additional header (64 bytes) + recycle header (40 bytes) + Raw packet
 *   is_recycle_append_ftmh=FAKSE, recycle header (40 bytes) + Raw packet
 *   pkt->dnx_header_stack is updated to store addtianal header(if exist) and recycle header
 */
int
dnx_rx_packet_parser_etpp_mtu_trap(
    int unit,
    bcm_pkt_t * pkt,
    uint8 is_recycle_append_ftmh)
{
    static dnx_rx_parse_headers_t packet_info;
    uint8 *header_buffer = NULL;
    uint32 parsed_header_length = 0;
    int index = 0;
    uint8 is_inter_hdr_en = FALSE;
    uint8 is_tsh_en = FALSE;
    uint8 *eth_pkt_header = NULL;
    uint16 vlan_control_tag = 0;
    uint16 tpid = 0;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    SHR_NULL_CHECK(pkt, _SHR_E_PARAM, "pkt");

    /** Get start of system headers */
    parsed_header_length = pkt->tot_len - pkt->pkt_len;
    header_buffer = pkt->_pkt_data.data + parsed_header_length;

    /** Init DNX header information structure */
    sal_memset(&packet_info, 0, sizeof(dnx_rx_parse_headers_t));
    packet_info.dnx_header_count = 0;
    packet_info.header_length = 0;

    if (DNX_RX_PARSE_SYSTEM_HEADERS_MODE_JERICHO2(unit))
    {
        /*
         * Setting this field to TRUE results in adding additional header (64 bytes) that starts with FTMH_BASE header
         * This header is added before the recycle header (40 bytes)
         */
        if (is_recycle_append_ftmh == TRUE)
        {
            /** ftmh_base (10B)*/
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh
                            (unit, header_buffer, pkt->tot_len, &packet_info, &is_tsh_en, &is_inter_hdr_en));
            /** tsh_ext (4B), the existence is inidcated by ftmh_base.pph[1] */
            if (is_tsh_en == TRUE)
            {
                /** Move to start byte of next header */
                packet_info.header_length += DNX_RX_PARSE_HEADER_DATA_GET(unit, TSH, BASE_HEADER_SIZE);
            }
            /** pph_base (12B), the existence is inidcated by ftmh_base.pph[0] */
            if (is_inter_hdr_en)
            {
                SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal
                                (unit, header_buffer, pkt->tot_len, &packet_info, -1));
            }
            /** additional header is always 64 bytes */
            packet_info.header_length = DNX_RX_PARSE_ETPP_MTU_TRAP_ADDITIONAL_HEADER_SIZE;

            /** stamp_value (1B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_TIME_STAMP_SIZE;
            /** pp_dsp (1B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_PP_DSP_SIZE;
            /** outlif_to_stamp (3B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_OUTLIF_TO_STAMP_SIZE;
            /** ftmh_base (10B), lb_key_ext (3B), app_specific_ext (6B) */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh
                            (unit, header_buffer, pkt->tot_len, &packet_info, &is_tsh_en, &is_inter_hdr_en));
            /** tsh_ext (4B) */
            packet_info.header_length += DNX_RX_PARSE_HEADER_DATA_GET(unit, TSH, BASE_HEADER_SIZE);
            /** pph_base (12B) */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal(unit, header_buffer, pkt->tot_len, &packet_info, -1));

            packet_info.header_length =
                DNX_RX_PARSE_ETPP_MTU_TRAP_ADDITIONAL_HEADER_SIZE + DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_SIZE;

        }
        else
        {
            /** stamp_value (1B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_TIME_STAMP_SIZE;
            /** pp_dsp (1B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_PP_DSP_SIZE;
            /** outlif_to_stamp (3B) */
            packet_info.header_length += DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_OUTLIF_TO_STAMP_SIZE;

            /** ftmh_base (10B), lb_key_ext (3B), app_specific_ext (6B) */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_ftmh
                            (unit, header_buffer, pkt->tot_len, &packet_info, &is_tsh_en, &is_inter_hdr_en));
            /** tsh_ext (4B) */
            packet_info.header_length += DNX_RX_PARSE_HEADER_DATA_GET(unit, TSH, BASE_HEADER_SIZE);
            /** pph_base (12B) */
            SHR_IF_ERR_EXIT(dnx_rx_system_header_parse_internal(unit, header_buffer, pkt->tot_len, &packet_info, -1));
            packet_info.header_length = DNX_RX_PARSE_ETPP_MTU_TRAP_RECYCLE_HEADER_SIZE;
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ETPP MTU trap parser is not supported on JR1 mode.\n");
    }

    /** Fill dnx header stack in bcm_pkt_t */
    if (pkt->dnx_header_count + packet_info.dnx_header_count > BCM_PKT_NOF_DNX_HEADERS)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "DNX header stack is overflow.\n");
    }
    for (index = 0; index < packet_info.dnx_header_count; index++)
    {
        sal_memcpy(&(pkt->dnx_header_stack[index + pkt->dnx_header_count]), &(packet_info.dnx_header_stack[index]),
                   sizeof(bcm_pkt_dnx_t));
    }
    pkt->dnx_header_count += packet_info.dnx_header_count;

    pkt->pkt_len -= packet_info.header_length;

    /** Update priority and vlan id based on outer vlan tag of pkt */
    eth_pkt_header = pkt->_pkt_data.data + (pkt->tot_len - pkt->pkt_len);

    /*
     * 0x88a8 S-Tag in IEEE 802.1ad
     * 0x9100 S-Tag in old non-standard 802.1QinQ protocol
     * 0x8100 C-Tag in IEEE 802.1Q
     */
    pkt->rx_untagged = (BCM_PKT_OUTER_UNTAGGED | BCM_PKT_INNER_UNTAGGED);
    pkt->prio_int = 0;
    pkt->vlan = 0;
    tpid = ((uint16) ((eth_pkt_header[12] << 8) | eth_pkt_header[13]));
    vlan_control_tag = ((uint16) ((eth_pkt_header[14] << 8) | eth_pkt_header[15]));
    if ((tpid == 0x88a8) || (tpid == 0x9100))
    {
        uint16 inner_tpid = 0;

        /** Outer tagged */
        pkt->rx_untagged &= ~BCM_PKT_OUTER_UNTAGGED;
        inner_tpid = ((uint16) ((eth_pkt_header[16] << 8) | eth_pkt_header[17]));
        if (inner_tpid == 0x8100)
        {
            /** Double tagged */
            pkt->rx_untagged &= ~BCM_PKT_INNER_UNTAGGED;
        }
        pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
        pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);
    }
    if (tpid == 0x8100)
    {
        /** Inner tagged */
        pkt->rx_untagged &= ~BCM_PKT_INNER_UNTAGGED;
        pkt->prio_int = ((vlan_control_tag >> 13) & 0x007);
        pkt->vlan = ((vlan_control_tag >> 0) & 0xfff);
    }

    dnx_rx_dump_system_header_stack(pkt);

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * Adapter functions for RX
 */
#ifdef ADAPTER_SERVER_MODE

/**
 * \brief
 *  get the destination gport, based on the interface_type, channel and the first lane of the adapter outgoing port.
 *
 * \param [in] unit     - unit id
 * \param [in] ifc_type - outgoing packet interface type (CPU/OLP/RCY/NIF_IF)
 * \param [in] port_channel - port_channel
 * \param [in] first_lane - first lane of the port. Relevant only if the interface type in NIF_IF
 * \param [out] dst_gport - destination gport (decoded as local gport).
 *
 * \return
 *   shr_error_e - Error Type
 * \remark
 *
 * \see
 *   * None
 */
static shr_error_e
adapter_rx_dest_gport_get(
    int unit,
    int ifc_type,
    uint32 port_channel,
    uint32 first_lane,
    bcm_gport_t * dst_gport)
{
    bcm_port_if_t bcm_egr_if;
    dnx_algo_port_info_indicator_t indicator;
    dnx_algo_port_info_s port_info;
    bcm_port_t logical_port = DNX_ALGO_PORT_INVALID;
    int cosq = 0;
    SHR_FUNC_INIT_VARS(unit);

    *dst_gport = 0;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_hw_egr_if_convert(unit, ifc_type, &bcm_egr_if));
    sal_memset(&indicator, 0x0, sizeof(dnx_algo_port_info_indicator_t));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_to_port_info_get(unit, bcm_egr_if, &indicator, &port_info));
    if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_info, TRUE, TRUE, TRUE, TRUE))
    {
        
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phy_to_logical_get(unit, first_lane,
                                                             DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, TRUE,
                                                                                            TRUE), 0, &logical_port));
    }
    else
    {
        /** adapter channel, is actually channel+cosq. */
        /** since we don't know nof_priorities of the port, we will try all 8 priorities */
        while ((logical_port == DNX_ALGO_PORT_INVALID) && (cosq < 8) && (port_channel >= cosq))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_and_channel_to_logical_get
                            (unit, 0, port_info, (port_channel - cosq), &logical_port));
            cosq++;
        }
    }
    if (logical_port != DNX_ALGO_PORT_INVALID)
    {
        BCM_GPORT_LOCAL_SET(*dst_gport, logical_port);
    }

exit:
    SHR_FUNC_EXIT;
}

void
adapter_convert_rx_data_from_chars(
    int unit,
    int data_length_in_bits,
    unsigned char *data_chars,
    uint8 *data_binary)
{
    int convert_position = 0;
    int result_index = 0;
    char zero_char = '0';

    while (convert_position + 8 <= data_length_in_bits)
    {
        data_binary[result_index] = (((uint8) (data_chars[convert_position]) - (uint8) (zero_char)) << 7 |
                                     ((uint8) (data_chars[convert_position + 1]) - (uint8) (zero_char)) << 6 |
                                     ((uint8) (data_chars[convert_position + 2]) - (uint8) (zero_char)) << 5 |
                                     ((uint8) (data_chars[convert_position + 3]) - (uint8) (zero_char)) << 4 |
                                     ((uint8) (data_chars[convert_position + 4]) - (uint8) (zero_char)) << 3 |
                                     ((uint8) (data_chars[convert_position + 5]) - (uint8) (zero_char)) << 2 |
                                     ((uint8) (data_chars[convert_position + 6]) - (uint8) (zero_char)) << 1 |
                                     ((uint8) (data_chars[convert_position + 7]) - (uint8) (zero_char)));
        convert_position += 8;
        result_index++;
    }
}

/*
 * Look for a generated header signal. Return a pointer to the signal's data or NULL if it doesn't exist.
 */
void
adapter_find_generated_header(
    int unit,
    uint32 nof_signals,
    uint32 pkt_length,
    unsigned char *pkt_data,
    unsigned char **generated_header_signal,
    uint32 *generated_header_length)
{
    int signal_index;
    unsigned char *pkt_data_current_signal;
    uint32 signal_id;
    uint32 signal_length;

    pkt_data_current_signal = pkt_data;
    *generated_header_signal = NULL;

    /** If only one signal came on the packet use it as the data */
    if (nof_signals == 1)
    {
        *generated_header_length = ntohl(*(uint32 *) &(pkt_data_current_signal[4]));
        *generated_header_signal = pkt_data + 2 * sizeof(uint32);
    }
    else
    {
        /** Run over the signals and look for the generated header signal */
        for (signal_index = 0; signal_index < nof_signals; signal_index++)
        {
            signal_id = ntohl(*(uint32 *) pkt_data_current_signal);
            signal_length = ntohl(*(int *) &(pkt_data_current_signal[4]));

            /** Length validation check */
            if (signal_length + (uint32) (pkt_data_current_signal - pkt_data) > pkt_length)
            {
                /** Illegal packet format */
                break;
            }

            if (signal_id == dnx_data_adapter.rx.packet_header_signal_id_get(unit))
            {
                /** Update the generated header signal */
                *generated_header_length = signal_length;
                *generated_header_signal = &(pkt_data_current_signal[8]);

                break;
            }
            /** Point to the next signal. Add the signal's length + length field + signal id field  */
            pkt_data_current_signal += signal_length + 2 * sizeof(uint32);
        }
    }
}

/*
 * Build the pkt struct
 * buf format - [signal id (4 bytes) | signal data length (4 bytes) NW order | signal data (each bit is a char)]*
 */
void
adapter_build_rx_packet(
    int unit,
    adapter_ms_id_e ms_id,
    uint32 nof_signals,
    uint32 port_first_lane,
    uint32 port_channel,
    int ifc_type,
    int len,
    unsigned char *buf,
    bcm_pkt_t * pkt)
{
    unsigned char *generated_header_signal;
    uint32 generated_header_length;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    adapter_find_generated_header(unit, nof_signals, len, buf, &generated_header_signal, &generated_header_length);

    if (generated_header_signal != NULL)
    {
        /*
         * Currently there are no signals, only the packet's data. Take the data after the signal ID and data size
         */
        adapter_convert_rx_data_from_chars(unit, generated_header_length, generated_header_signal, pkt->pkt_data->data);
        pkt->pkt_data->len = (generated_header_length >> 3);    /* Convert from bit to bytes. The first 8 bytes are the
                                                                 * signal ID and * data length */
        pkt->_pkt_data.len = pkt->pkt_data->len;
        pkt->pkt_len = pkt->pkt_data->len;
        pkt->tot_len = pkt->pkt_data->len;

        pkt->blk_count = 1;     /* Number of blocks in data array. */
        pkt->unit = unit;       /* Unit number. */
        /*
         * pkt->cos;
         *//*
         * The local COS queue to use.
         */
        /*
         * pkt->color;
         *//*
         * Packet color.
         */
        /*
         * pkt->src_port
         */
        /*
         * dest_port
         */
        /*
         * dest_gport - Destination port used in header/tag.
         */
        SHR_IF_ERR_EXIT(adapter_rx_dest_gport_get(unit, ifc_type, port_channel, port_first_lane, &pkt->dst_gport));

        pkt->opcode = BCM_HG_OPCODE_CPU;        /* BCM_HG_OPCODE_xxx. */

        /*
         * bcm_gport_t src_gport;
         *//*
         * Source virtual port
         */
        /*
         * bcm_multicast_t multicast_group;
         *//*
         * Destination multicast group.
         */
        SHR_EXIT();
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "No generated header was found in the packet!");
    }

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_VOID_FUNC_EXIT;
}

static void
adapter_rx_pkt_thread(
    void *param)
{
    int unit = 0;
    unsigned char buf[MAX_PACKET_SIZE_ADAPTER] = { 0 };
    /** data_buf[MAX_PACKETS_IN_PARALLEL][MAX_PACKET_SIZE_ADAPTER] */
    uint8 *data_buf;
    int len = 0;
    int rv = _SHR_E_NONE;
    uint32 port_first_lane = 0;
    uint32 port_channel = 0;
    int ifc_type = -1;
    adapter_ms_id_e ms_id = 0;
    uint32 nof_signals = 0;
    bcm_pkt_t *pkt;
    uint32 next_packet_index;
    uint32 ii;
    bcm_pkt_blk_t pkt_data[MAX_PACKETS_IN_PARALLEL];
    int u_number, sub_unit_id = 0;

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, TRUE));
    DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_ADD_ALL_UNITS(DNX_ERR_RECOVERY_INTERNAL_THREAD_RX);

    sal_memset(pkt_data, 0, sizeof(pkt_data));
    /** Allocate a pool of MAX_PACKETS_IN_PARALLEL packets */
    data_buf = sal_alloc(MAX_PACKETS_IN_PARALLEL * MAX_PACKET_SIZE_ADAPTER, "Buffers for rx packets pool");
    assert(data_buf);
    sal_memset(data_buf, 0, (MAX_PACKETS_IN_PARALLEL * MAX_PACKET_SIZE_ADAPTER));

    pkt = sal_alloc(sizeof(bcm_pkt_t) * MAX_PACKETS_IN_PARALLEL, "Rx packets pool");
    assert(pkt);
    sal_memset(pkt, 0, (sizeof(bcm_pkt_t) * MAX_PACKETS_IN_PARALLEL));

    for (ii = 0; ii < MAX_PACKETS_IN_PARALLEL; ii++)
    {
        pkt[ii].pkt_data = &(pkt_data[ii]);
        pkt[ii].pkt_data->data = data_buf + ii * MAX_PACKET_SIZE_ADAPTER;
        pkt[ii]._pkt_data.data = pkt[ii].pkt_data->data;
    }

    next_packet_index = 0;
    /** assuming system with multiple units and multiple sub-units is not allowed. */
    /** find the number of units/sub_units to handle */
    u_number = (dnx_data_device.general.nof_sub_units_get(unit) > 1) ?
        dnx_data_device.general.nof_sub_units_get(unit) : soc_cm_get_num_devices();
    INIT_SLEEP;
    while (rx_control.thread_running)
    {
        if (dnx_init_is_init_done_get(unit) == FALSE)
        {
            unit = (unit + 1) % soc_cm_get_num_devices();
            continue;
        }

        /*
         * Lock system rx start/stop mechanism.
         */
        _BCM_RX_SYSTEM_LOCK;
        /*
         * check if packet is ready and read it
         */
        rv = adapter_read_buffer(unit, sub_unit_id, &ms_id, &nof_signals, &ifc_type, &port_channel, &port_first_lane,
                                 &len, &(buf[0]));
        /*
         * Packet was read from buffer
         */
        if (rv == _SHR_E_NONE)
        {
            /*
             * Build the packets struct
             */
            adapter_build_rx_packet(unit, ms_id, nof_signals, port_first_lane,
                                    port_channel, ifc_type, len, &(buf[0]), &(pkt[next_packet_index]));

            /*
             * Process the packet
             */
            rx_adapter_process_packet(unit, &(pkt[next_packet_index]));

            next_packet_index = (next_packet_index + 1) % MAX_PACKETS_IN_PARALLEL;
        }
        else if (rv == _SHR_E_DISABLED)
        {
        /** c model server disconnected. Close Rx thread (this one) in the SDK */
            rx_control.thread_running = FALSE;
        }
        /*
         * Unlock system rx start/stop mechanism.
         */
        _BCM_RX_SYSTEM_UNLOCK;
        _BCM_RX_CHECK_THREAD_DONE;
        SLEEP_MIN_SET(BASE_SLEEP_VAL / u_number);
        sal_sem_take(rx_control.pkt_notify, rx_control.sleep_cur);
        rx_control.pkt_notify_given = FALSE;
        INIT_SLEEP;
        /** toggle between unit/sub_units - assuming system with multiple units and multiple sub-units is not allowed */
        sub_unit_id = (sub_unit_id + 1) % (dnx_data_device.general.nof_sub_units_get(unit));
        unit = (unit + 1) % soc_cm_get_num_devices();
    }
    /*
     * Done using self-pipe, close fds
     */
    close(pipe_fds[0]);
    close(pipe_fds[1]);

    rx_control.thread_exit_complete = TRUE;

    sal_free(data_buf);
    sal_free(pkt);

    DNXC_MTA(dnxc_multithread_analyzer_declare_api_in_play(unit, __FUNCTION__, MTA_FLAG_THREAD_MAIN_FUNC, FALSE));
    DNX_ERR_RECOVERY_UTILS_EXCLUDED_THREADS_REMOVE_ALL_UNITS(DNX_ERR_RECOVERY_INTERNAL_THREAD_RX);
    sal_thread_exit(0);
}

/*
 * Init the Rx thread that deals with packets to the CPU
 */
int
bcm_adapter_rx_start(
    int unit,
    bcm_rx_cfg_t * cfg)
{
    int priority = RX_THREAD_PRI_DFLT;
    int flags;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

    /** if several units active, do the following just once  - one thread for all rx units */
    if (rx_control.thread_running == FALSE)
    {
        /*
         * Timer/Event semaphore thread sleeping on.
         */
        if (NULL == rx_control.pkt_notify)
        {
            rx_control.pkt_notify = sal_sem_create("RX pkt ntfy", sal_sem_BINARY, 0);
            if (NULL == rx_control.pkt_notify)
            {
                SHR_ERR_EXIT(_SHR_E_MEMORY, "NULL == rx_control.pkt_notify");
            }
            rx_control.pkt_notify_given = FALSE;
        }

        /*
         * RX start/stop on one of the units protection mutex.
         */
        if (NULL == rx_control.system_lock)
        {
            rx_control.system_lock = sal_mutex_create("RX system lock");
            if (NULL == rx_control.system_lock)
            {
                sal_sem_destroy(rx_control.pkt_notify);
                SHR_ERR_EXIT(_SHR_E_MEMORY, "NULL == rx_control.system_lock");
            }
        }

        /*
         * Setup pipe for select exit notification. We use a "self-pipe" trick: - The write end is maintained by the main
         * thread - The read end is selected in the RX thread When we need to close the RX thread, we simply write to the
         * pipe, and we exit the blocking select call
         */
        if (pipe(pipe_fds) == -1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "pipe(pipe_fds) == -1");
        }

        /*
         * Make read and write ends of pipe nonblocking: Get read end flags
         */
        flags = fcntl(pipe_fds[0], F_GETFL);
        if (flags == -1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[0], F_GETFL)=-1");
        }

        /*
         * Make read end nonblocking
         */
        flags |= O_NONBLOCK;
        if (fcntl(pipe_fds[0], F_SETFL, flags) == -1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[0], F_SETFL, flags) == -1");
        }

        /*
         * Get write end flags
         */
        flags = fcntl(pipe_fds[1], F_GETFL);
        if (flags == -1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[1], F_GETFL)= -1");
        }

        /*
         * Make write end nonblocking
         */
        flags |= O_NONBLOCK;
        if (fcntl(pipe_fds[1], F_SETFL, flags) == -1)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "fcntl(pipe_fds[1], F_SETFL, flags) == -1");
        }

        /*
         * Start rx thread.
         */
        rx_control.thread_running = TRUE;
        rx_control.adapter_rx_tid =
            sal_thread_create("bcmAdapterRX", SAL_THREAD_STKSZ, priority, adapter_rx_pkt_thread, NULL);

    }

    /*
     * Indicate RX is running for all active units
     */
    rx_ctl[unit]->flags |= BCM_RX_F_STARTED;

exit:
    DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);
    SHR_FUNC_EXIT;
}

/*
 * End of adapter functions
 */
#endif /* ADAPTER_SERVER_MODE */

/**
 * See rx.h
 */
shr_error_e
dnx_rx_init(
    int unit)
{
    uint8 is_init;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(_bcm_common_rx_init(unit));

    if (!sw_state_is_warm_boot(unit))
    {
        SHR_IF_ERR_EXIT(dnx_rx_trap_init(unit));
        SHR_IF_ERR_EXIT(dnx_rx_parser_info.is_init(unit, &is_init));
        if (!is_init)
        {
            SHR_IF_ERR_EXIT(dnx_rx_parser_info.init(unit));
            SHR_IF_ERR_EXIT(dnx_rx_parser_info.dma_channel_to_cpu_port.alloc(unit, BCM_CMICX_RX_CHANNELS));
            SHR_IF_ERR_EXIT(dnx_rx_parser_info.cpu_port_to_dma_channel.alloc(unit, SOC_MAX_NUM_PORTS));
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * See rx.h
*/
shr_error_e
dnx_rx_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * clear rx info
     */
    SHR_IF_ERR_EXIT(_bcm_rx_shutdown(unit));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
