/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * ! \file bcm_int/dnx/fabric/fabric.h
 * Reserved.$
 */

#ifndef _DNX_FABRIC_H_INCLUDED_
/*
 * {
 */
#define _DNX_FABRIC_H_INCLUDED_

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>

#define DNX_FABRIC_FMAC_EMULATION_NOT_SUPPORTED(unit) \
    (dnx_data_device.emulation.feature_get(unit, dnx_data_device_emulation_fmac_supported) == FALSE)

/*
 * Typedefs:
 * {
 */
/**
 * \brief
 *   Which part to configure wfq weights opon
 */
typedef enum
{
    /**
     * Configure both ingress and egress
     */
    DNX_FABRIC_WFQ_TYPE_ALL = 0,
    /**
     * Configure ingress
     */
    DNX_FABRIC_WFQ_TYPE_CONTEXT_INGRESS = 1,
    /**
     * Configure egress
     */
    DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS = 2
} dnx_fabric_wfq_type_e;

/**
 * \brief
 *   Indicates how the fabric interface is connected.
 */
typedef enum
{
    /**
     * Indicates single FAP in the system, without other FAPs of FEs
     */
    DNX_FABRIC_CONNECT_MODE_SINGLE_FAP = 0,
    /**
     * Indicates Fabric interface is connected to other FAPs directly.
     * No FEs in the system.
     */
    DNX_FABRIC_CONNECT_MODE_MESH = 1,
    /**
     * Indicates Fabric interface is connected to FE device.
     */
    DNX_FABRIC_CONNECT_MODE_FE = 2,
    DNX_FABRIC_NOF_CONNECT_MODES
} dnx_fabric_connect_mode_e;
/*
 * }
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief
 *   Configure ALDWP value.
 *   ALDWP = Active Link Down Watchdog Period.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_aldwp_config(
    int unit);

/**
 * \brief
 *   Create load balancing periodic event.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_load_balance_thread_init(
    int unit);

/**
 * \brief
 *   Destroy load balancing periodic event.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_load_balance_thread_deinit(
    int unit);

/**
 * \brief
 *   Initialize dnx fabric module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_init(
    int unit);

/**
 * \brief
 *   Deinitialize dnx fabric module.
 *
 * \param [in] unit -
 *   The unit number.
 * \return
 *   \retval See \ref shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_fabric_deinit(
    int unit);

/**
 * \brief
 *   Set link isolation, i.e.
 *   Isolate/Activate a fabric link by stop/enable sending
 *   reachability messages.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   Fabric link to isolate/activate.
 * \param [in] enable -
 *   Isolate/Activate the link.
 *   1 - isolate the link.
 *   0 - activate the link.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_link_isolation_set(
    int unit,
    int link,
    int enable);

/**
 * \brief
 *   Disable fabric control cells during init sequence (Stop receiving and transmitting control cells).
 *   This function is used as a wrapper for init sequence since init sequence invokes functions only with
 *   'unit' argument.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   shr_error_e
 * \remark
 *  This function should be invoked during init sequence as one of the first steps (before HW is accessed).
 *  Meaning that this function should be invoked right after access is initiliazed.
 *  The reason to disable traffic before any access is because some HW configurations must not be done during traffic,
 *  so it's expected from the application to re-enable fabric control cells after it's done with all the required configurations.
 * \see
 *   * None
 */
shr_error_e dnx_fabric_control_cells_disable(
    int unit);

/**
 * \brief
 *   Disable/Enable Tx traffic on a fabric link.
 * \param [in] unit -
 *   The unit number.
 * \param [in] link -
 *   Fabric link to disable/enable tx traffic upon.
 * \param [in] disable -
 *   Disable/enable tx traffic on the link.
 *   1 - disable tx traffix.
 *   0 - enable tx traffix.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * There is no 'get' function since this information is also written
 *     into sw_state, so when we want a 'get' we read it from sw_state
 *     directly.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_link_tx_traffic_disable_set(
    int unit,
    int link,
    int disable);

/**
 * \brief
 *   Set weight for WFQs in fabric pipes.
 * \param [in] unit -
 *   The unit number.
 * \param [in] pipe -
 *   The pipe number (0/1/2) to configure.
 * \param [in] weight -
 *   The weight to configure.
 * \param [in] fabric_wfq_type -
 *   Which WFQ to configure:
 *     Ingress (FDT)
 *     Egress (FDR)
 *     All (Ingress+Egress)
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_wfq_set(
    int unit,
    int pipe,
    int weight,
    dnx_fabric_wfq_type_e fabric_wfq_type);

/**
 * \brief
 *   Get weight for WFQs in fabric pipes.
 * \param [in] unit -
 *   The unit number.
 * \param [in] pipe -
 *   The pipe number (0/1/2) to get WFQ configurations.
 * \param [out] weight -
 *   The configured weight
 * \param [in] fabric_wfq_type -
 *   Which WFQ to get configurations:
 *     Ingress (FDT)
 *     Egress (FDR)
 *     All (assuming Ingress+Egress was configured the same)
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_wfq_get(
    int unit,
    int pipe,
    int *weight,
    dnx_fabric_wfq_type_e fabric_wfq_type);

/**
 * \brief
 *   Set dynamic-WFQ weight.
 *   There are 2 sets of WFQs:
 *     1) For normal scenario.
 *     2) For congested scenario.
 *   Each input ('leg') of the WFQs can be set with weight.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fabric_wfq_fifo -
 *   Which input ('leg') of the WFQ to configure the weight.
 * \param [in] is_congested -
 *   1 - Configure WFQ for congested scenario.
 *   0 - Configure WFQ for normal scenario.
 * \param [in] weight -
 *   The weight to configure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Moving between the 'sets' is according to the local unicast congestion
 *     compared to a configured threshold (which is set using the CGM API).
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_wfq_dynamic_set(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int weight);

/**
 * \brief
 *   Get dynamic-WFQ weight.
 *   There are 2 sets of WFQs:
 *     1) For normal scenario.
 *     2) For congested scenario.
 *   Each input ('leg') of the WFQs can be set with weight.
 * \param [in] unit -
 *   The unit number.
 * \param [in] fabric_wfq_fifo -
 *   Which input ('leg') of the WFQ to configure the weight.
 * \param [in] is_congested -
 *   1 - Configure WFQ for congested scenario.
 *   0 - Configure WFQ for normal scenario.
 * \param [in] weight -
 *   The weight to configure.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Moving between the 'sets' is according to the local unicast congestion
 *     compared to a configured threshold (which is set using the CGM API).
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_wfq_dynamic_get(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int *weight);

/**
 * \brief
 *   Set fabric blocks to power down
 * \param [in] unit -
 *   The unit number.
 * \param [in] power_down -
 *   power down indication
 * \return
 *   See \ref shr_error_e
 * \remark
 * \see
 *   * None.
 */
shr_error_e dnx_fabric_blocks_power_down_set(
    int unit,
    int power_down);

#endif /*_DNX_FABRIC_H_INCLUDED_*/
