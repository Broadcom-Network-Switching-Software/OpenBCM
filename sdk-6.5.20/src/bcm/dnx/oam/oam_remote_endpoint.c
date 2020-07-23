
/** \file oam_remote_endpoint.c
 * 
 *
 * OAM remote endpoint create/get/destroy procedures for DNX.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <bcm/oam.h>
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <bcm_int/dnx/algo/oamp/algo_oamp.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oam_access.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_oamp_access.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */

/*
 * MACROs
 * {
 */
/*
 * }
 */

/*
 * Structure with punt profile HW information
 * to be passed from SW related functions to functions
 * related to HW write/clear
 */
typedef struct dnx_oam_rmep_punt_profile_temp_data_s
{
    /** The new profile id */
    int new_punt_profile;

    /** The new punt profile data */
    dnx_oam_oamp_punt_event_profile_t new_punt_profile_data;

    /** Whether HW needs to be written with this new profile */
    uint8 write_new_punt_profile;

    /** Old punt profile, in case of update to new profile */
    int old_punt_profile;

    /*
     * Whether delete old profile or not.
     * This is true only in case of update
     */
    uint8 delete_old_punt_profile;
} dnx_oam_rmep_punt_profile_temp_data_t;

/**
 * \brief - Utility function for Remote MEP Punt profile HW configuration
 *          This function does HW writes/clear based on information in
 *          the structure.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] hw_write_data - The HW write/clear information.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_punt_profile_hw_configure(
    int unit,
    const dnx_oam_rmep_punt_profile_temp_data_t * hw_write_data)
{
    dnx_oam_oamp_punt_event_profile_t zero_punt_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    /** New profile configuration */
    if (hw_write_data->write_new_punt_profile)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_profile_tbl_set(unit, hw_write_data->new_punt_profile,
                                                          &(hw_write_data->new_punt_profile_data)));
    }

    /** Old profile configuration */
    if (hw_write_data->delete_old_punt_profile)
    {
        sal_memset(&zero_punt_profile_data, 0, sizeof(zero_punt_profile_data));
        /*
         * Delete the old profile
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_profile_tbl_set(unit, hw_write_data->old_punt_profile,
                                                          &zero_punt_profile_data));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function multiplies a period given in milli seconds & micro seconds
 *
 * \param [in] _whole - Value in milli seconds
 * \param [in] _thousandth - Value in micro seconds
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
void
dnx_oam_multiply_by_3_5(
    uint32 *_whole,
    uint32 *_thousandth)
{
    uint32 _res_whole;
    uint32 _res_thousandth;
    _res_whole = ((*_whole) * 35) / 10;
    _res_thousandth = ((*_thousandth) * 35) / 10;
    if ((((*_whole) * 35) % 10) > 0)
    {
        _res_thousandth += ((*_whole) * 35) % 10;
    }
    if (_res_thousandth > 1000)
    {
        _res_whole += _res_thousandth / 1000;
        _res_thousandth = _res_thousandth % 1000;
    }
    (*_whole) = _res_whole;
    (*_thousandth) = _res_thousandth;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function calculates and fills
 *          RMEPs timeout period.
 *          if ccm_period is given,
 *              timeout period = ccm_period.
 *          else
 *              timeout period = local endpoint transmit period * 3.5
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] endpoint_info - info structure of the requested
 *        RMEP.
 *        Timeout period will be filled in endpoint_info->ccm_period
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_fill_ccm_period(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    dnx_oam_oamp_ccm_endpoint_t *entry_values = NULL;
    uint32 ccm_period_ms = 0, ccm_period_micro_s = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC(entry_values, sizeof(dnx_oam_oamp_ccm_endpoint_t),
              "Endpoint data read to find MDB entries before deleting", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    if (DNX_OAM_IS_ENDPOINT_RFC6374(endpoint_info))
    {
        entry_values->mep_type = DBAL_ENUM_FVAL_OAMP_MEP_TYPE_RFC_6374;
    }
    else
    {
        /** Set default value, since we have check in next step */
        entry_values->mep_type = 0;
    }

    /**
    * If RMEP period is set to "0", take the ccm_period from local endpoint.
    */
    if (endpoint_info->ccm_period == 0)
    {
        SHR_IF_ERR_EXIT(dnx_oamp_mep_db_ccm_endpoint_get(unit, endpoint_info->local_id, 0, entry_values));
        SHR_IF_ERR_EXIT(dnx_oam_oamp_ccm_interval_to_ccm_period
                        (unit, entry_values->ccm_interval, (int *) &(ccm_period_ms)));
        dnx_oam_multiply_by_3_5(&ccm_period_ms, &ccm_period_micro_s);

        endpoint_info->ccm_period = ccm_period_ms;
    }

exit:
    SHR_FREE(entry_values);
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP creation
 *          This function verifies the following:
 *          a) Local MEP should exist
 *          b) Local MEP should be accelerated.
 *          c) loc_clear_threshold parameter should be positive
 *          d) RMEP WITH_ID: Another RMEP with same ID should not exist.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        RMEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_create_verify(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    int is_update = 0;
    int local_endpoint_id = endpoint_info->local_id;
    dnx_oam_endpoint_info_t sw_endpoint_info;
    SHR_FUNC_INIT_VARS(unit);

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)
    {
        is_update = 1;
    }

    /*
     * Check if LOCAL MEP exists. If it does not, SW STATE info get will return ERROR
     */
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_endpoint_info_get(unit, local_endpoint_id, &sw_endpoint_info));

    /*
     * If LOCAL MEP exists, check whether it is accelerated or not
     */
    if (!(sw_endpoint_info.sw_state_flags & DBAL_DEFINE_OAM_ENDPOINT_SW_STATE_FLAGS_ACCELERATED))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: RMEPs are not supported for Non-accelerated local endpoints\n");
    }

    /*
     * Check if loc_clear_threshold is positive
     */
    if (endpoint_info->loc_clear_threshold < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: loc_clear_threshold must be positive. Current value:%d\n",
                     endpoint_info->loc_clear_threshold);
    }

    /*
     * Update should always be called with BCM_OAM_ENDPOINT_WITH_ID flag
     */
    if (is_update)
    {
        if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Error: REPLACE flag should be used with WITH_ID flag\n");
        }
    }

    /*
     * REMOTE_EVENT_DISABLE and RDI_AUTO_UPDATE flags can't be set together
     */
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) &&
        (endpoint_info->flags & BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE and "
                     "BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE flags can't be set together\n");
    }

    /*
     * REMOTE_EVENT_DISABLE and REMOTE_UPDATE_STATE_DISABLE flags can't be set together
     */
     /* coverity[copy_paste_error:FALSE]  */
    if ((endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE) &&
         /* coverity[copy_paste_error:FALSE]  */
        (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "Error: BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE and "
                     "BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE flags can't be set together\n");
    }

    /*
     * Set up endpoint_info->ccm_period.
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_fill_ccm_period(unit, endpoint_info));

    /** Validity check for non-zero lifetime units */
    if (dnx_data_oam.oamp.rmep_db_non_zero_lifetime_units_limitation_get(unit))
    {
        uint32 lifetime, lifetime_units;
        /** Calculate lifetime and lifetime unis according period */
        dnx_oam_oamp_period_to_lifetime(unit, (endpoint_info->ccm_period * 1000), &lifetime, &lifetime_units);
        if (lifetime_units != DBAL_ENUM_FVAL_LIFETIME_UNITS_SCAN_PERIOD_1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Non-zero lifetime units are not supported. Lower CCM period expected.\n");
        }
    }

    /*
     * Check if sampling_ratio is valid
     */
    if (endpoint_info->sampling_ratio < 0 || endpoint_info->sampling_ratio > MAX_SAMPLING_RATIO)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Error: sampling_ratio %d not valid, value must be between 0 and %d",
                     endpoint_info->sampling_ratio, MAX_SAMPLING_RATIO);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - OAM RMEP punt profile free function
 *          This function frees a punt profile
 *          of a RMEP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] punt_profile - Punt profile for the requested
 *        RMEP
 * \param [out] hw_write_data - Profile deletion info for HW.
 *                              Assumed to be zeroed out by caller.
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_punt_profile_free(
    int unit,
    int punt_profile,
    dnx_oam_rmep_punt_profile_temp_data_t * hw_write_data)
{
    uint8 is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.free_single(unit, punt_profile, &is_last));

    hw_write_data->delete_old_punt_profile = is_last;
    hw_write_data->old_punt_profile = punt_profile;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - OAM RMEP punt profile data updation based on endpoint info
 *          This function gets an endpoint_info structure
 *          of a RMEP and converts it into
 *          PUNT profile data.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [out] endpoint_info - info structure of the requested
 *        RMEP
 * \param [out] punt_profile_data - Punt profile data of the requested
 *        RMEP
 *
 * \return
 *   * None
 *
 * \see
 *   * None
 */
static void
dnx_oam_remote_endpoint_info_to_punt_profile_data(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    dnx_oam_oamp_punt_event_profile_t * punt_profile_data)
{
    punt_profile_data->punt_enable = (endpoint_info->sampling_ratio > 0) ? 1 : 0;
    punt_profile_data->punt_rate = (endpoint_info->sampling_ratio > 0) ? endpoint_info->sampling_ratio - 1 : 0;
    punt_profile_data->rx_state_update_en =
        (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE) ?
        DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_DONT_UPDATE_STATE_RDI_GEN_PACKET : (endpoint_info->flags2 &
                                                                                      BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE)
        ? DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_DONT_GEN_PACKET : (endpoint_info->flags2 &
                                                                                     BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE)
        ? DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_GEN_PACKET :
        DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_IF_FIFO_NOT_FULL_GEN_PKT;
    punt_profile_data->profile_scan_state_update_en =
        (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE) ?
        DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_DONT_UPDATE_RMEP_DB_STATE_GEN_PACKET : (endpoint_info->flags &
                                                                                            BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE)
        ? DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_DONT_GEN_PACKET : (endpoint_info->flags &
                                                                                           BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE)
        ? DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_GEN_PACKET :
        DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_IF_FIFO_NOT_FULL_GEN_PKT;
    punt_profile_data->mep_rdi_update_loc_en = ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC) != 0);
    punt_profile_data->mep_rdi_update_loc_clear_en =
        ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR) != 0);
    punt_profile_data->mep_rdi_update_rx_en = ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI) != 0);
}

/**
 * \brief - OAM RMEP punt profile based endpoint updation
 *          This function gets a punt profile data
 *          of a RMEP endpoint info and updates
 *          endpoint_info with that information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] punt_profile_data - Punt profile data of the requested
 *        RMEP
 * \param [out] endpoint_info - info structure of the requested
 *        RMEP
 *
 * \return
 *   * None
 *
 * \see
 *   * None
 */
static void
dnx_oam_remote_endpoint_punt_profile_data_to_endpoint_info(
    int unit,
    const dnx_oam_oamp_punt_event_profile_t * punt_profile_data,
    bcm_oam_endpoint_info_t * endpoint_info)
{

    endpoint_info->sampling_ratio = punt_profile_data->punt_enable ? punt_profile_data->punt_rate + 1 : 0;
    switch (punt_profile_data->rx_state_update_en)
    {
        case DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_DONT_UPDATE_STATE_RDI_GEN_PACKET:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_DONT_GEN_PACKET:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_EVENT_DISABLE;
            break;
        case DBAL_ENUM_FVAL_PROFILE_RX_STATE_UPDATE_EN_UPD_STATE_RDI_GEN_PACKET:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_RX_REMOTE_DEFECT_AUTO_UPDATE;
            break;
        default:
            break;
    }
    switch (punt_profile_data->profile_scan_state_update_en)
    {
        case DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_DONT_UPDATE_RMEP_DB_STATE_GEN_PACKET:
            endpoint_info->flags2 |= BCM_OAM_ENDPOINT_FLAGS2_REMOTE_UPDATE_STATE_DISABLE;
            break;
        case DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_DONT_GEN_PACKET:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE_EVENT_DISABLE;
            break;
        case DBAL_ENUM_FVAL_PROFILE_SCAN_STATE_UPDATE_EN_UPD_RMEP_DB_STATE_GEN_PACKET:
            endpoint_info->flags |= BCM_OAM_ENDPOINT_RDI_AUTO_UPDATE;
            break;
        default:
            break;
    }

    endpoint_info->flags2 |= punt_profile_data->mep_rdi_update_loc_en ? BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_LOC : 0;
    endpoint_info->flags2 |=
        punt_profile_data->mep_rdi_update_loc_clear_en ? BCM_OAM_ENDPOINT_FLAGS2_RDI_CLEAR_ON_LOC_CLEAR : 0;
    endpoint_info->flags2 |= punt_profile_data->mep_rdi_update_rx_en ? BCM_OAM_ENDPOINT_FLAGS2_RDI_ON_RX_RDI : 0;

}

/**
 * \brief - OAM RMEP punt profile get function
 *          This function gets a punt profile
 *          of a RMEP endpoint info and updates
 *          endpoint_info with that information.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] punt_profile - Punt profile for the requested
 *        RMEP
 * \param [out] endpoint_info - info structure of the requested
 *        RMEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_punt_profile_get(
    int unit,
    int punt_profile,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    dnx_oam_oamp_punt_event_profile_t punt_profile_data;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&punt_profile_data, 0, sizeof(punt_profile_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_profile_tbl_get(unit, punt_profile, &punt_profile_data));

    /*
     * Convert the punt profile data to endpoint_info punt info
     */
    dnx_oam_remote_endpoint_punt_profile_data_to_endpoint_info(unit, &punt_profile_data, endpoint_info);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - OAM RMEP punt profile allocate function
 *          This function allocates a punt profile
 *          based on RMEP endpoint info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        RMEP
 * \param [in] is_update - update or new punt profile
 * \param [in] old_profile_id - if update - the id of the old punt profile
 * \param [out] hw_write_data - Punt profile Hw info for the requested
 *        RMEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_punt_profile_alloc(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    int is_update,
    int old_profile_id,
    dnx_oam_rmep_punt_profile_temp_data_t * hw_write_data)
{
    dnx_oam_oamp_punt_event_profile_t punt_profile_data;
    int profile_id = -1;
    uint8 write_hw = 0, is_last = 0;
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(&punt_profile_data, 0, sizeof(punt_profile_data));

    /*
     * Convert the punt profile information in endpoint_info to punt profile data
     */
    dnx_oam_remote_endpoint_info_to_punt_profile_data(unit, endpoint_info, &punt_profile_data);

    if (is_update)
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.exchange
                        (unit, 0, &punt_profile_data, old_profile_id, NULL, &profile_id, &write_hw, &is_last));

        hw_write_data->old_punt_profile = old_profile_id;
        hw_write_data->delete_old_punt_profile = is_last;
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_oam_db.oamp_punt_event_prof_template.allocate_single
                        (unit, 0, &punt_profile_data, NULL, &profile_id, &write_hw));
    }

    hw_write_data->new_punt_profile = profile_id;
    hw_write_data->write_new_punt_profile = write_hw;
    sal_memcpy(&(hw_write_data->new_punt_profile_data), &punt_profile_data, sizeof(dnx_oam_oamp_punt_event_profile_t));
exit:
    SHR_FUNC_EXIT;
}

/*
 * Integer division x/y with integer round to nearest.
 * NOTE!!!!!! if y=0  .... ERROR
 */
#define SOC_SAND_DIV_ROUND(x,y) ( ((x) + ((y)/2) ) / (y) )

/**
 * \brief - Utility function for Remote MEP GET
 *          This function gets the HW rmep db structure
 *          and fills the endpoint info structure based on that
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_rmep_db_entry-  structure of the requested
 *        RMEP HW entry
 * \param [out] endpoint_info - info structure of the requested
 *        RMEP
 *
 * \return
 *   none
 *
 * \see
 *   * None
 */
static void
dnx_oam_remote_endpoint_hw_entry_struct_get(
    int unit,
    const dnx_oam_oamp_rmep_db_entry_t * oamp_rmep_db_entry,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    endpoint_info->local_id = oamp_rmep_db_entry->oam_id;
    /*
     * Divided by 1000 since the RMEP DB entry period is in
     * micro seconds(rounding it as well).
     */
    endpoint_info->ccm_period = SOC_SAND_DIV_ROUND(oamp_rmep_db_entry->period, 1000);
    endpoint_info->loc_clear_threshold = oamp_rmep_db_entry->loc_clear_threshold;
    endpoint_info->interface_state = oamp_rmep_db_entry->rmep_state.eth_state.interface_status;
    endpoint_info->port_state = oamp_rmep_db_entry->rmep_state.eth_state.port_status;
    endpoint_info->faults |= oamp_rmep_db_entry->rdi_received ? BCM_OAM_ENDPOINT_FAULT_REMOTE : 0;
    endpoint_info->faults |= oamp_rmep_db_entry->loc ? BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT : 0;
}
/**
 * \brief - Utility function for Remote MEP creation
 *          This function sets the HW rmep db structure
 *          based on endpoint info
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        RMEP
 * \param [in] punt_profile - punt profile of the requested
 *        RMEP
  * \param [in] punt_good_profile - punt good profile of the requested
 *        RMEP
 * \param [out] oamp_rmep_db_entry-  structure of the requested
 *        RMEP HW entry
 *
 * \return
 *   none
 *
 * \see
 *   * None
 */
static void
dnx_oam_remote_endpoint_hw_entry_struct_set(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    int punt_profile,
    int punt_good_profile,
    dnx_oam_oamp_rmep_db_entry_t * oamp_rmep_db_entry)
{
    oamp_rmep_db_entry->oam_id = endpoint_info->local_id;
    /*
     * Multiplied by 1000 since the RMEP DB entry period is expected in
     * micro seconds.
     */
    oamp_rmep_db_entry->period = endpoint_info->ccm_period * 1000;
    oamp_rmep_db_entry->loc_clear_threshold = endpoint_info->loc_clear_threshold;
    oamp_rmep_db_entry->punt_profile = punt_profile;
    oamp_rmep_db_entry->punt_good_profile = punt_good_profile;
    oamp_rmep_db_entry->punt_next_good_packet =
        ((endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_PUNT_NEXT_GOOD_PACKET) ? 1 : 0);
    oamp_rmep_db_entry->rmep_state.eth_state.interface_status = endpoint_info->interface_state;
    oamp_rmep_db_entry->rmep_state.eth_state.port_status = endpoint_info->port_state;
    oamp_rmep_db_entry->last_ccm_lifetime_valid_on_create =
        ((endpoint_info->faults & BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT) == BCM_OAM_ENDPOINT_FAULT_CCM_TIMEOUT);
}

/**
 * \brief - Utility function for Remote MEP destroy.
 *          This function removes an RMEP from the list of RMEPs
 *          for a given local MEP.
 *
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] local_endpoint_id - id of the local MEP
 * \param [in] rmep_idx - id of the remote MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_remove_from_local_endpoint_list(
    int unit,
    int local_endpoint_id,
    int rmep_idx)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Delete RMEP from endpoint linked list. */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_delete_rmep(unit, local_endpoint_id, rmep_idx));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Utility function for Remote MEP create.
 *          This function adds an RMEP to the list of RMEPs
 *          for a given local MEP.
 *
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_remote_endpoint_add_to_local_endpoint_list(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Add RMEP to endpoint linked list. This is executed at each remote_endpoint_create. */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_insert_rmep(unit, endpoint_info->local_id, endpoint_info->id));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Main function for Remote MEP create.
 *          This function calls to sub-functions that
 *          verifies create request and adds the new MEP
 *          to sw and hw
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_remote_endpoint_create(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    uint32 alloc_flags = 0;
    int rmep_idx = -1;
    int is_update = 0;
    int punt_profile = -1, old_punt_profile = -1;
    int punt_good_profile = -1, old_punt_good_profile = -1;
    dnx_oam_oamp_rmep_db_entry_t oamp_rmep_entry;
    dnx_oam_oamp_rmep_db_entry_t old_oamp_rmep_entry;
    dnx_oam_remote_endpoint_info_t rmep_sw_info;
    dnx_oam_rmep_punt_profile_temp_data_t punt_profile_hw_write_data;
    dnx_oam_oamp_punt_good_profile_temp_data_t punt_good_profile_hw_write_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNX(dnx_oam_remote_endpoint_create_verify(unit, endpoint_info));

    if (endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)
    {
        is_update = 1;
    }

    if (!is_update)
    {
        /*
         *  Allocate ID for the new RMEP if its not an update/replace
         */
        if (_SHR_IS_FLAG_SET(endpoint_info->flags, BCM_OAM_ENDPOINT_WITH_ID))
        {
            alloc_flags = DNX_ALGO_RES_ALLOCATE_WITH_ID;
        }
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_alloc(unit, alloc_flags, &endpoint_info->id));
        rmep_idx = endpoint_info->id;
    }
    else
    {
        DNX_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_idx, (endpoint_info->id));
    }
    if (!is_update)
    {
        /*
         * Add remote endpoint to rmep linked list of the local endpoint,
         * if its not an update/replace
         */
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_add_to_local_endpoint_list(unit, endpoint_info));

        /*
         * Store the RMEP INFO in SW State
         */
        sal_memset(&rmep_sw_info, 0, sizeof(rmep_sw_info));
        rmep_sw_info.rmep_id = endpoint_info->name;
        rmep_sw_info.oam_id = endpoint_info->local_id;
        SHR_IF_ERR_EXIT(dnx_oam_sw_state_remote_endpoint_info_set(unit, rmep_idx, &rmep_sw_info));
    }

    if (is_update)
    {
        /*
         * Get old OAMP RMEP_DB entry
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, rmep_idx, 1 /* is_oam */ , &old_oamp_rmep_entry));
        old_punt_profile = old_oamp_rmep_entry.punt_profile;
        old_punt_good_profile = old_oamp_rmep_entry.punt_good_profile;
    }

    /*
     * Allocate or exchange punt profile
     */
    sal_memset(&punt_profile_hw_write_data, 0, sizeof(punt_profile_hw_write_data));
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_profile_alloc
                    (unit, endpoint_info, is_update, old_punt_profile, &punt_profile_hw_write_data));

    punt_profile = punt_profile_hw_write_data.new_punt_profile;

    /*
     * Allocate or exchange punt good profile
     */
    sal_memset(&punt_good_profile_hw_write_data, 0, sizeof(punt_good_profile_hw_write_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_alloc
                    (unit, is_update, endpoint_info->punt_good_packet_period, old_punt_good_profile,
                     &punt_good_profile_hw_write_data));

    punt_good_profile = punt_good_profile_hw_write_data.new_punt_good_profile;

    /*
     * Set the remote endpoint HW structure.
     */
    sal_memset(&oamp_rmep_entry, 0, sizeof(oamp_rmep_entry));
    dnx_oam_remote_endpoint_hw_entry_struct_set(unit, endpoint_info, punt_profile, punt_good_profile, &oamp_rmep_entry);

    /*
     * Punt profile HW configuration
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_profile_hw_configure(unit, &punt_profile_hw_write_data));

    /*
     * Punt good profile HW configuration
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure(unit, &punt_good_profile_hw_write_data));

    if (is_update)
    {
        /** Check if an update is required */
        if (dnx_oam_oamp_rmep_compare(TRUE, &old_oamp_rmep_entry, &oamp_rmep_entry))
        {
            /*
             * Add the remote endpoint to HW.
             */
            SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_add(unit, rmep_idx, 1, is_update, &oamp_rmep_entry));
        }
    }
    else
    {
        /*
         * Add the remote endpoint to HW.
         */
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_add(unit, rmep_idx, 1, is_update, &oamp_rmep_entry));
    }

    if (!is_update)
    {
        SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_index_db_add(unit, endpoint_info->local_id, endpoint_info->name, rmep_idx));
    }

    /*
     * Convert the remote mep id internal to external endpoint.
     */
    DNX_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_idx, (endpoint_info->id));

exit:
    if (SHR_FUNC_ERR())
    {
        /*
         * Free the allocated index if there was an error
         */
        if (rmep_idx != -1)
        {
            dnx_oam_remote_endpoint_id_free(unit, rmep_idx);
        }
    }
    SHR_FUNC_EXIT;
}

/**
* See oam_internal.h
*/
shr_error_e
dnx_oam_remote_endpoint_get(
    int unit,
    int rmep_idx_internal,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    dnx_oam_remote_endpoint_info_t rmep_sw_info;
    dnx_oam_oamp_rmep_db_entry_t rmep_db_entry;
    SHR_FUNC_INIT_VARS(unit);

    endpoint_info->flags |= BCM_OAM_ENDPOINT_REMOTE;

    /*
     * Get remote MEP ID and Local endpoint ID using the SW state
     */
    sal_memset(&rmep_sw_info, 0, sizeof(rmep_sw_info));
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_remote_endpoint_info_get(unit, rmep_idx_internal, &rmep_sw_info));
    endpoint_info->name = rmep_sw_info.rmep_id;
    endpoint_info->local_id = rmep_sw_info.oam_id;

    sal_memset(&rmep_db_entry, 0, sizeof(rmep_db_entry));
    /*
     * Remote acc endpoint get
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, rmep_idx_internal, 1 /* is_oam */ , &rmep_db_entry));

    dnx_oam_remote_endpoint_hw_entry_struct_get(unit, &rmep_db_entry, endpoint_info);

    /*
     * Punt profile info get
     */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_profile_get(unit, rmep_db_entry.punt_profile, endpoint_info));

    /*
     * Punt good profile info get
     */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_get
                    (unit, rmep_db_entry.punt_good_profile, &endpoint_info->punt_good_packet_period));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy a REMOTE OAM Endpoint (internal function).
*
* \param [in] unit  - Relevant unit.
* \param [in] rmep_idx - Id of the remote endpoint to destroy.
* \param [in] endpoint_info - Info structure of the endpoint.
* \param [in] del_rmep_from_ll - Whether to delete RMEP from LMEP linked list
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_oam_remote_endpoint_destroy_internal(
    int unit,
    int rmep_idx,
    bcm_oam_endpoint_info_t * endpoint_info,
    uint8 del_rmep_from_ll)
{
    int punt_profile = -1;
    uint8 punt_good_profile = 0;
    dnx_oam_oamp_rmep_db_entry_t oamp_rmep_entry;
    dnx_oam_rmep_punt_profile_temp_data_t punt_profile_hw_write_data;
    dnx_oam_oamp_punt_good_profile_temp_data_t punt_good_profile_hw_write_data;
    SHR_FUNC_INIT_VARS(unit);
    sal_memset(&punt_good_profile_hw_write_data, 0, sizeof(punt_good_profile_hw_write_data));
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_get(unit, rmep_idx, 1 /* is_oam */ , &oamp_rmep_entry));
    punt_profile = oamp_rmep_entry.punt_profile;
    punt_good_profile = oamp_rmep_entry.punt_good_profile;

    /** Free up the punt profile in SW */
    sal_memset(&punt_profile_hw_write_data, 0, sizeof(punt_profile_hw_write_data));
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_profile_free(unit, punt_profile, &punt_profile_hw_write_data));

    /** Free up the punt good profile */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_punt_good_profile_free(unit, punt_good_profile, &punt_good_profile_hw_write_data));

    if (del_rmep_from_ll)
    {
        /** Remove the rmep from rmep linked list of local endpoint */
        SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_remove_from_local_endpoint_list
                        (unit, endpoint_info->local_id, rmep_idx));
    }
    /** Free up the software info */
    SHR_IF_ERR_EXIT(dnx_oam_sw_state_remote_endpoint_info_delete(unit, rmep_idx));

    /** Free up the punt profile in HW */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_profile_hw_configure(unit, &punt_profile_hw_write_data));

    /** Free up the punt good profile in HW */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_punt_good_profile_hw_configure(unit, &punt_good_profile_hw_write_data));

    /** Remove from the OAMP_RMEP_INDEX_DB */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_index_db_remove(unit, endpoint_info->local_id, endpoint_info->name));

    /** Remove from the OAMP_RMEP_DB */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_rmep_remove(unit, rmep_idx, 1));

    /** Free up the index */
    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_id_free(unit, rmep_idx));
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  Destroy a REMOTE OAM Endpoint.
*
* \param [in] unit  - Relevant unit.
* \param [in] rmep_idx - Id of the remote endpoint to destroy.
* \param [in] endpoint_info - Info structure of the endpoint.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e
dnx_oam_remote_endpoint_destroy(
    int unit,
    int rmep_idx,
    bcm_oam_endpoint_info_t * endpoint_info)
{
    return dnx_oam_remote_endpoint_destroy_internal(unit, rmep_idx, endpoint_info, 1    /* delete the rmep from lmep's
                                                                                         * linked list */ );
}

/**
* \brief
*  Utility callback function to destroy one REMOTE OAM Endpoint
*  associated with a particular LMEP
*
* \param [in] unit  - Relevant unit.
* \param [in] local_endpoint  - local endpoint
* \param [in] remote_endpoint - remote endpoint
* \remark
*   * None
* \see
*   * None
*/

shr_error_e
dnx_oam_lmep_rmep_destroy_traverse_callback(
    int unit,
    bcm_oam_endpoint_t local_endpoint,
    bcm_oam_endpoint_t remote_endpoint)
{
    bcm_oam_endpoint_info_t remote_ep_info;
    SHR_FUNC_INIT_VARS(unit);

    bcm_oam_endpoint_info_t_init(&remote_ep_info);

    SHR_IF_ERR_EXIT(dnx_oam_remote_endpoint_get(unit, remote_endpoint, &remote_ep_info));
    return dnx_oam_remote_endpoint_destroy_internal(unit, remote_endpoint, &remote_ep_info, 0   /* do not delete the
                                                                                                 * rmep from lmep's
                                                                                                 * linked list */ );
exit:
    SHR_FUNC_EXIT;
}

/*
 * See oam_internal.h file for description
 */
shr_error_e
dnx_oam_lmep_remote_endpoints_destroy(
    int unit,
    bcm_oam_endpoint_t local_id)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_traverse_list
                    (unit, local_id, dnx_oam_lmep_rmep_destroy_traverse_callback));
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_lmep_to_rmep_empty_rmep_list(unit, local_id));
exit:
    SHR_FUNC_EXIT;
}
