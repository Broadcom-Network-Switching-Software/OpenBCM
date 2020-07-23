/** \file oam_tune.c
 * Tune oam module file
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_INITSEQDNX

/*
 * Include files. {
 */
#include "oam_tune.h"
#include <soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <soc/sand/sand_aux_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** 
 * Default channel type value for Y1731 over PWE
 */
#define DNX_OAM_PWE_CHANNEL_TYPE_VALUE 0x27
/** 
 * Default channel type value for Y1731 over MPLS TP
 */
#define DNX_OAM_Y1731_MPLS_TP_CHANNEL_TYPE_VALUE 0x8902
/** 
 * Default channel type value for BFD over PWE
 */
#define DNX_PWE_BFD_CHANNEL_TYPE_VALUE 0x7
/**
 * Default channel type value for BFD over MPLS-TP
 */
#define DNX_MPLS_TP_BFD_CHANNEL_TYPE_VALUE 0x22
/**
 * Default channel type value for
 */
#define DNX_MPLS_TP_RFC6374_LM_VALUE 0x0A
/**
 * Default channel type value for
 */
#define DNX_MPLS_TP_RFC6374_ILM_VALUE 0x0B
/**
 * Default channel type value for
 */
#define DNX_MPLS_TP_RFC6374_DM_VALUE 0x0C

/**
 * \brief
 *   Sets the defult defult channel types for MPLS-TP channel
 *   types. This function sets the RX side.
 * \param [in] unit  -  
 *   Number of hardware unit used..
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected 
 * \remark
 *   * None
 * \see
 */
static shr_error_e
dnx_tune_oam_mpls_tp_channel_type_rx_init(
    int unit)
{
    int num_of_values = 1;
    int channel_type;
    int list_of_values[1];

    SHR_FUNC_INIT_VARS(unit);
    /*
     * set the default standard value of channel type for Y1731 over PWE
     */
    list_of_values[0] = DNX_OAM_PWE_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelPweonoam;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));
    /*
     *  set the default channel type value for BFD over PWE
     */
    list_of_values[0] = DNX_PWE_BFD_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelPweBfd;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));

    /*
     * set the default channel type value for Y1731 over MPLS TP
     */
    list_of_values[0] = DNX_OAM_Y1731_MPLS_TP_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelY1731;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));

    /*
     * set the default channel type value for BFD over MPLS TP
     */
    list_of_values[0] = DNX_MPLS_TP_BFD_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelMplsTpBfd;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));

    /*
     * set the default channel type value for LM over rfc.6374
     */
    list_of_values[0] = DNX_MPLS_TP_RFC6374_LM_VALUE;
    channel_type = bcmOamMplsTpChannelMplsDirectLm;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));

    /*
     * set the default channel type value for LM over rfc.6374
     */
    list_of_values[0] = DNX_MPLS_TP_RFC6374_DM_VALUE;
    channel_type = bcmOamMplsTpChannelMplsDm;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_rx_set(unit, channel_type, num_of_values, list_of_values));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the defult defult channel types for MPLS-TP channel
 *   types. This function sets the TX side.
 * \param [in] unit  -  
 *   Number of the hardware unit used..
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected 
 * \remark
 *   * None
 * \see
 */
static shr_error_e
dnx_tune_oam_mpls_tp_channel_type_tx_init(
    int unit)
{
    int channel_type;
    int value;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * set the default standard value of channel type for Y1731 over PWE
     */
    value = DNX_OAM_PWE_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelPweonoam;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value));

    /*
     *  set the default channel type value for BFD over PWE
     */
    value = DNX_PWE_BFD_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelPweBfd;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value));

    /*
     * set the default channel type value for Y1731 over MPLS TP
     */
    value = DNX_OAM_Y1731_MPLS_TP_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelY1731;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value));

    /*
     * set the default channel type value for BFD over MPLS TP
     */
    value = DNX_MPLS_TP_BFD_CHANNEL_TYPE_VALUE;
    channel_type = bcmOamMplsTpChannelMplsTpBfd;
    SHR_IF_ERR_EXIT(bcm_oam_mpls_tp_channel_type_tx_set(unit, channel_type, value));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Sets the defult OAM counter range for LM
 * \param [in] unit  -  
 *   Number of hardware unit used..
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected 
 * \remark
 *   * None
 * \see
 */
static shr_error_e
dnx_tune_oam_lm_counters_range_init(
    int unit)
{
    uint64 counter_value;
    uint32 max_counter = 0, counter_mask = 0xffffffff;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Configure range to allow not pcp counter_base: Assuming pcp is not used we set all range for not-pcp.
     * OAM_COUNTER_MIN and OAM_COUNTER_MAX are 20 bit registers So the range should be 0 - 0xfffff 
     */
    COMPILER_64_ZERO(counter_value);
    SHR_IF_ERR_EXIT(bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMin, counter_value));
    SHR_BITCOPY_RANGE(&(max_counter), 0, &counter_mask, 0, dnx_data_oam.general.oam_nof_bits_counter_base_get(unit));
    COMPILER_64_SET(counter_value, 0, max_counter);
    SHR_IF_ERR_EXIT(bcm_oam_control_set(unit, bcmOamControlLmPcpCounterRangeMax, counter_value));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See prototype definition for function description
 */
shr_error_e
dnx_tune_oam_init(
    int unit)
{

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Set the defult standard value of channel types for mpls-tp RX
     */
    SHR_IF_ERR_EXIT(dnx_tune_oam_mpls_tp_channel_type_rx_init(unit));
    /*
     * Set the defult standard value of channel types for mpls-tp TX
     */
    SHR_IF_ERR_EXIT(dnx_tune_oam_mpls_tp_channel_type_tx_init(unit));
    /*
     * Set the defult OAM counter range for LM 
     */
    SHR_IF_ERR_EXIT(dnx_tune_oam_lm_counters_range_init(unit));

exit:
    SHR_FUNC_EXIT;
}
