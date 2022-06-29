/** \file port_preemption.c
 * 
 *
 * Frame preemption functionality for DNX.
 *
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_TSN

/*
 * Include files.
 * {
 */

#include <bcm/port.h>
#include <bcm_int/dnx/auto_generated/dnx_port_dispatch.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/nif/dnx_port_nif_ofr.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/cosq/egress/esb.h>
#include <bcm_int/dnx/cosq/egress/egq_dbal.h>
#include <bcm_int/dnx/cosq/egress/egr_tm_dispatch.h>
#include <bcm_int/dnx/tsn/port_preemption.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnx/dbal/dbal.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/portmod/portmod.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_tsn.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esb.h>

#include <shared/utilex/utilex_integer_arithmetic.h>

/*
 * }
 */

/*
 * Defines:
 * {
 */

/*
 * }
 */

/**
 * \brief -
 *  Retrieve preemption TX enable status on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e dnx_port_preemption_tx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable);

/**
 * \brief -
 * Verify Input parameters of bcmPortPreemptControlPreemptionSupport control
 */
static shr_error_e
dnx_port_preemption_support_enable_set_verify(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    bcm_gport_t tc_gport;
    bcm_pbmp_t logical_ports;
    int current_port;
    int is_port_enabled;
    int num_priorities, cosq, priority;
    SHR_FUNC_INIT_VARS(unit);

    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "Only values 0 and 1 are supported!\r\n");

    /** preemption support can be enabled only if traffic is disabled */
    SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port, &is_port_enabled));
    if (is_port_enabled && enable)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Preemption support cannot be enabled if port is enabled.\n");
    }

    /** Verify all Qpairs are High on all ports on interface */
    if (enable)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &logical_ports));
        BCM_PBMP_ITER(logical_ports, current_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, current_port, &num_priorities));
            BCM_COSQ_GPORT_PORT_TC_SET(tc_gport, current_port);
            for (cosq = 0; cosq < num_priorities; cosq++)
            {
                SHR_IF_ERR_EXIT(dnx_egr_tm_cosq_control_port_tc_get
                                (unit, tc_gport, cosq, bcmCosqControlPrioritySelect, &priority));
                if (priority != BCM_COSQ_SP0)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "All Qpairs should be set to high priority \n");
                }

            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Enable / disable preemption support on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_support_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_support_enable_set_verify(unit, port, enable));

    if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        /** Enable NIF preemption support */
        SHR_IF_ERR_EXIT(dnx_port_ofr_preemption_enable_set(unit, port, enable));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmPortPreemptControlPreemptionSupport not supported for port type\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Retrieve preemption support enable status for port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_support_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_nif.ofr.feature_get(unit, dnx_data_nif_ofr_is_supported))
    {
        /** Enable NIF preemption support */
        SHR_IF_ERR_EXIT(dnx_port_ofr_preemption_enable_get(unit, port, enable));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmPortPreemptControlPreemptionSupport not supported for port type\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Enable / disable preemption TX on Egress pipeline
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_egress_tx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{

    int egr_if, egr_nif_if;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));

    if (dnx_data_esb.general.feature_get(unit, dnx_data_esb_general_esb_support))
    {
        /*
         * Change FQP to ESB mapping
         */
        SHR_IF_ERR_EXIT(dnx_esb_to_fqp_mapping_set(unit, port, enable));

        /**
         * Enable port preemption in ESB
         * to access ESB we need to use the Egress NIF IF
         */
        egr_nif_if = egr_if - dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit);
        SHR_IF_ERR_EXIT(dnx_esb_dbal_frame_preemption_set(unit, egr_nif_if, core, enable));
    }

    /**
     * Change FQP to use 2 TXI
     */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_use_single_txi_set(unit, egr_if, core, !enable));

    /**
     * Enable port preemption in FQP
     */
    SHR_IF_ERR_EXIT(dnx_egq_dbal_egq_preemption_set(unit, egr_if, core, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcmPortPreemptControlEnableTx control
 */
static shr_error_e
dnx_port_preemption_tx_enable_set_verify(
    int unit,
    bcm_port_t port,
    uint32 enable)
{

    uint32 is_preemption_support_enabled = 0, is_preemption_tx_enabled = 0;
    int is_port_enabled;

    SHR_FUNC_INIT_VARS(unit);

    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "Only values 0 and 1 are supported!\r\n");

    if (enable)
    {
        /** Get preemption support from NIF */
        SHR_IF_ERR_EXIT(dnx_port_preemption_support_enable_get(unit, port, &is_preemption_support_enabled));

        /** Verify that support is enabled when we enable the TX side*/
        if (!is_preemption_support_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Need to enable port preemption on RX side \n");
        }
    }
    else
    {
        /** Check if we disable under traffic */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port, &is_port_enabled));
        SHR_IF_ERR_EXIT(dnx_port_preemption_tx_enable_get(unit, port, &is_preemption_tx_enabled));

        if (is_port_enabled && is_preemption_tx_enabled)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Need to disable port to be able to disable port preemption on TX side \n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Enable / disable preemption TX on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_tx_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_tx_enable_set_verify(unit, port, enable));

    if (enable)
    {
        /** Enable NIF preemption transmission */
        SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlEnableTx, 0));
        SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlEnableTx, 1));

        /** Configure Egress mapping and enable Egress frame preemption */
        SHR_IF_ERR_EXIT(dnx_port_preemption_egress_tx_enable_set(unit, port, 1));
    }
    else
    {
        /** Configure Egress mapping and enable Egress frame preemption */
        SHR_IF_ERR_EXIT(dnx_port_preemption_egress_tx_enable_set(unit, port, 0));

        SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlEnableTx, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Retrieve preemption TX enable status on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_tx_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    int egr_if;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    /**
     * Get port preemption in ESB
     * to access ESB we need to use the Egress NIF IF
     */
    egr_if -= dnx_data_egr_queuing.params.egr_if_nif_alloc_start_get(unit);
    SHR_IF_ERR_EXIT(dnx_esb_dbal_frame_preemption_get(unit, egr_if, core, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcmPortPreemptControlVerifyEnable control
 */
static shr_error_e
dnx_port_preemption_verify_enable_set_verify(
    int unit,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_BOOL_VERIFY(enable, _SHR_E_PARAM, "Only values 0 and 1 are supported!\r\n");

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Enable / disable preemption verify on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_enable_set(
    int unit,
    bcm_port_t port,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_verify_enable_set_verify(unit, enable));

    SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlVerifyEnable, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Retrieve preemption verify enable status on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] enable - enable indication
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_enable_get(
    int unit,
    bcm_port_t port,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(imb_preemption_control_get(unit, port, bcmPortPreemptControlVerifyEnable, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcmPortPreemptControlVerifyTime control
 */
static shr_error_e
dnx_port_preemption_verify_time_set_verify(
    int unit,
    uint32 time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(time, 1, dnx_data_tsn.preemption.verify_max_time_get(unit), _SHR_E_PARAM,
                     "time %d is out of range\n", time);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set preemption verify wait time on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] time - verification wait time in milliseconds
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_time_set(
    int unit,
    bcm_port_t port,
    uint32 time)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_verify_time_set_verify(unit, time));

    /*
     * Attempt time in CDMAC is (VERIFY_TIME+1) 
     */
    SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlVerifyTime, (time - 1)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get preemption verify wait time on port
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] time - verification wait time in milliseconds
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_time_get(
    int unit,
    bcm_port_t port,
    uint32 *time)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Attempt time in CDMAC is (VERIFY_TIME+1) 
     */
    SHR_IF_ERR_EXIT(imb_preemption_control_get(unit, port, bcmPortPreemptControlVerifyTime, time));
    (*time)++;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcmPortPreemptControlVerifyAttempts control
 */
static shr_error_e
dnx_port_preemption_verify_attempts_set_verify(
    int unit,
    uint32 attempts)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(attempts, 1, dnx_data_tsn.preemption.verify_max_attempts_get(unit), _SHR_E_PARAM,
                     "attempts %d is out of range\n", attempts);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set number of preemption verification attempts.
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] attempts - number of preemption verification attempts
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_attempts_set(
    int unit,
    bcm_port_t port,
    uint32 attempts)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_verify_attempts_set_verify(unit, attempts));

    /*
     * Attempts in CDMAC is (VERIFY_ATTEMPT_LIMIT+1) 
     */
    SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, bcmPortPreemptControlVerifyAttempts, (attempts - 1)));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get number of preemption verification attempts.
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [out] attempts - number of preemption verification attempts
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_verify_attempts_get(
    int unit,
    bcm_port_t port,
    uint32 *attempts)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Attempts in CDMAC is (VERIFY_ATTEMPT_LIMIT+1) 
     */
    SHR_IF_ERR_EXIT(imb_preemption_control_get(unit, port, bcmPortPreemptControlVerifyAttempts, attempts));
    (*attempts)++;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of :
 *     bcmPortPreemptControlNonFinalFragSizeRx
 *     bcmPortPreemptControlFinalFragSizeRx
 *     bcmPortPreemptControlNonFinalFragSizeTx
 *     bcmPortPreemptControlFinalFragSizeTx
 */
static shr_error_e
dnx_port_preemption_frag_size_set_verify(
    int unit,
    bcm_port_preempt_control_t type,
    uint32 size)
{
    uint32 size_verify = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortPreemptControlNonFinalFragSizeRx:
            size_verify = dnx_data_tsn.preemption.non_final_frag_size_rx_get(unit);
            break;
        case bcmPortPreemptControlFinalFragSizeRx:
            size_verify = dnx_data_tsn.preemption.final_frag_size_rx_get(unit);
            break;
        case bcmPortPreemptControlNonFinalFragSizeTx:
            size_verify = dnx_data_tsn.preemption.non_final_frag_size_tx_get(unit);
            break;
        case bcmPortPreemptControlFinalFragSizeTx:
            size_verify = dnx_data_tsn.preemption.final_frag_size_tx_get(unit);
            break;
        default:
            break;
    }

    if (size != size_verify)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Fragment size %d is not valid for type %d.\n", size, type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Set minimal fragment size
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] type - fragment type :
 *      bcmPortPreemptControlNonFinalFragSizeRx
 *         bcmPortPreemptControlFinalFragSizeRx
 *         bcmPortPreemptControlNonFinalFragSizeTx
 *         bcmPortPreemptControlFinalFragSizeTx
 * \param [in] size - fragment size
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_frag_size_set(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 size)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_frag_size_set_verify(unit, type, size));

    switch (type)
    {
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
            size = UTILEX_DIV_ROUND_UP(size, dnx_data_tsn.preemption.frag_size_bytes_in_hw_unit_get(unit));
            SHR_IF_ERR_EXIT(imb_preemption_control_set(unit, port, type, size));
            break;

        case bcmPortPreemptControlNonFinalFragSizeTx:
        case bcmPortPreemptControlFinalFragSizeTx:
            /*
             * do nothing
             */
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  Get minimal fragment size
 *
 * \param [in] unit - the relevant unit
 * \param [in] port - logical port number
 * \param [in] type - fragment type :
 *      bcmPortPreemptControlNonFinalFragSizeRx
 *         bcmPortPreemptControlFinalFragSizeRx
 *         bcmPortPreemptControlNonFinalFragSizeTx
 *         bcmPortPreemptControlFinalFragSizeTx
 * \param [out] size - fragment size
 *
 * \return
 *   bcm_error_t
 *
 * \remark
 *   * None
 */
static shr_error_e
dnx_port_preemption_frag_size_get(
    int unit,
    bcm_port_t port,
    bcm_port_preempt_control_t type,
    uint32 *size)
{
    uint32 frag_size_bytes_in_hw_unit = 0;
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
            frag_size_bytes_in_hw_unit = dnx_data_tsn.preemption.frag_size_bytes_in_hw_unit_get(unit);

            SHR_IF_ERR_EXIT(imb_preemption_control_get(unit, port, type, size));
            (*size) *= frag_size_bytes_in_hw_unit;
            break;

        case bcmPortPreemptControlNonFinalFragSizeTx:
            *size = dnx_data_tsn.preemption.non_final_frag_size_tx_get(unit);
            break;

        case bcmPortPreemptControlFinalFragSizeTx:
            *size = dnx_data_tsn.preemption.final_frag_size_tx_get(unit);
            break;

        default:
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify port is frame preemptable
 *
 */
static shr_error_e
dnx_port_preemption_port_preemptable_verify(
    int unit,
    bcm_gport_t gport)
{
    dnx_algo_port_info_s port_info;
    int frame_preemptable = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport, &port_info));

    if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, 0))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_frame_preemptable_get(unit, gport, &frame_preemptable));
    }

    if (!frame_preemptable)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Port %d is not frame preemptable\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcm_dnx_port_preemption_control_get() / bcm_dnx_port_preemption_control_set()
 *
 */
static shr_error_e
dnx_port_preemption_control_verify(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 *arg,
    uint32 is_set)
{

    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport));

    /** Verify port is frame preemptable */
    SHR_IF_ERR_EXIT(dnx_port_preemption_port_preemptable_verify(unit, gport));

    /** Verify type */
    switch (type)
    {
        case bcmPortPreemptControlPreemptionSupport:
        case bcmPortPreemptControlEnableTx:
        case bcmPortPreemptControlVerifyEnable:
        case bcmPortPreemptControlVerifyTime:
        case bcmPortPreemptControlVerifyAttempts:
        case bcmPortPreemptControlNonFinalFragSizeTx:
        case bcmPortPreemptControlFinalFragSizeTx:
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected type %d.\n", type);
            break;
    }

    if (is_set == FALSE)
    {
        /** Verify arg */
        SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_preemption_control_set(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_control_verify(unit, gport, type, NULL, TRUE));

    switch (type)
    {
        case bcmPortPreemptControlPreemptionSupport:
            SHR_IF_ERR_EXIT(dnx_port_preemption_support_enable_set(unit, gport, arg));
            break;
        case bcmPortPreemptControlEnableTx:
            SHR_IF_ERR_EXIT(dnx_port_preemption_tx_enable_set(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyEnable:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_enable_set(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyTime:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_time_set(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyAttempts:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_attempts_set(unit, gport, arg));
            break;
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
        case bcmPortPreemptControlNonFinalFragSizeTx:
        case bcmPortPreemptControlFinalFragSizeTx:
            SHR_IF_ERR_EXIT(dnx_port_preemption_frag_size_set(unit, gport, type, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected type %d.\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_preemption_control_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_control_t type,
    uint32 *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_control_verify(unit, gport, type, arg, FALSE));

    switch (type)
    {
        case bcmPortPreemptControlPreemptionSupport:
            SHR_IF_ERR_EXIT(dnx_port_preemption_support_enable_get(unit, gport, arg));
            break;
        case bcmPortPreemptControlEnableTx:
            SHR_IF_ERR_EXIT(dnx_port_preemption_tx_enable_get(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyEnable:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_enable_get(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyTime:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_time_get(unit, gport, arg));
            break;
        case bcmPortPreemptControlVerifyAttempts:
            SHR_IF_ERR_EXIT(dnx_port_preemption_verify_attempts_get(unit, gport, arg));
            break;
        case bcmPortPreemptControlNonFinalFragSizeRx:
        case bcmPortPreemptControlFinalFragSizeRx:
        case bcmPortPreemptControlNonFinalFragSizeTx:
        case bcmPortPreemptControlFinalFragSizeTx:
            SHR_IF_ERR_EXIT(dnx_port_preemption_frag_size_get(unit, gport, type, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected type %d.\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcm_dnx_port_preemption_status_get()
 *
 */
static shr_error_e
dnx_port_preemption_status_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_status_t type,
    uint32 *status)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify logical port */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, gport));

    /** Verify port is frame preemptable */
    SHR_IF_ERR_EXIT(dnx_port_preemption_port_preemptable_verify(unit, gport));

    /** Verify type */
    switch (type)
    {
        case bcmPortPreemptStatusTx:
        case bcmPortPreemptStatusVerify:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected type %d.\n", type);
            break;
    }

    /** Verify arg */
    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_port_preemption_status_get(
    int unit,
    bcm_gport_t gport,
    bcm_port_preempt_status_t type,
    uint32 *status)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_port_preemption_status_get_verify(unit, gport, type, status));

    switch (type)
    {
        case bcmPortPreemptStatusTx:
            SHR_IF_ERR_EXIT(imb_preemption_tx_status_get(unit, gport, status));
            break;
        case bcmPortPreemptStatusVerify:
            SHR_IF_ERR_EXIT(imb_preemption_verify_status_get(unit, gport, status));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "unexpected type %d.\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}
