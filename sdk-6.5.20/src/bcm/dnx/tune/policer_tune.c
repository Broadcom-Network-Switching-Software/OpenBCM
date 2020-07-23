/** \file policer_tune.c
 * Tuning of policer (ingress and egress)
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
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/policer.h>
#include <bcm_int/dnx/policer/policer_mgmt.h>
#include <bcm_int/dnx_dispatch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_meter.h>

/*
 * }
 * Include files.
 */

/**
 * \brief
 *      convert the color from 3 color mode (internal usage) to 4 color mode (external usage)
 * \param [in] unit -unit id
 * \param [in] internal_color -3 color mode
 * \param [in] external_color -4 color mode 
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_tune_color_convert(
    int unit,
    bcm_policer_color_t internal_color,
    bcm_color_t * external_color)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (internal_color)
    {
        case bcmPolicerColorGreen:
            *external_color = bcmColorGreen;
            break;
        case bcmPolicerColorYellow:
            *external_color = bcmColorYellow;
            break;
        case bcmPolicerColorRed:
        case bcmPolicerColorInvalid:
            *external_color = bcmColorRed;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid policer color: %d \n", internal_color);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *      function simulate a single meter decision in serial mode. It use the prev meter decision and its own decision as inputs and decide on the resolved color
 * \param [in] unit -unit id
 * \param [in] in_color -prev meter decision
 * \param [in] policer_color -currect meter decision as made by the HW
 * \param [in] other_credits -is the meter has credits from other bucket
 * \param [out] resolved_color -color decision which simulate serial mode of two meters
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_tune_single_resolved_calc(
    int unit,
    bcm_color_t in_color,
    bcm_policer_color_t policer_color,
    int other_credits,
    bcm_color_t * resolved_color)
{
    SHR_FUNC_INIT_VARS(unit);
    /* *INDENT-OFF* */
    /*
            in_color   policer_color   other_credits   resolved_color 
            NA              R               NA              R
            R               NA              NA              R 
            NA              INVALID         NA              in_color 
            G               G               NA              G 
            NA              Y               NA              Y 
            Y               G               0               R 
            Y               G               1               Y 
     */
    /* *INDENT-ON* */    

    if (policer_color == bcmPolicerColorRed || in_color == bcmColorRed)
    {
        *resolved_color = bcmColorRed;
    }
    else if (policer_color == bcmPolicerColorInvalid)
    {
        *resolved_color = in_color;
    }
    else if (in_color == bcmColorGreen && policer_color == bcmPolicerColorGreen)
    {
        *resolved_color = bcmColorGreen;
    }
    else if (policer_color == bcmPolicerColorYellow)
    {
        *resolved_color = bcmColorYellow;
    }
    else if (in_color == bcmColorYellow && policer_color == bcmPolicerColorGreen && other_credits == 0)
    {
        *resolved_color = bcmColorRed;
    }
    else
    {
        *resolved_color = bcmColorYellow;
    }

/**exit:*/
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      function calculate how to update the meter bucket base on the resolved color decision and the current meter data
 * \param [in] unit -unit id
 * \param [in] resolved_color -color decision for all serial mode
 * \param [in] policer_color -currect meter decision as made by the HW 
 * \param [in] other_credits -is the meter has credits from other bucket
 * \param [out] update_bucket -how to update the meter buckets (3 color mode)
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
static shr_error_e
dnx_policer_tune_update_bucket_set(
    int unit,
    bcm_color_t resolved_color,
    bcm_policer_color_t policer_color,
    int other_credits,
    bcm_policer_color_t * update_bucket)
{
    int is_green_credits = FALSE, is_yellow_credits = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    /* *INDENT-OFF* */
    /*
        resolved_color green_credits yellow_credits update_bucket   
                R          NA                  NA         NONE
                G          T                   NA         G 
                Y          NA                  T          Y 
                Y          T                   F          G 
     */    
    /* *INDENT-ON* */

    /** calc if there is available green credits and yellow credits for the current meter */
    if (policer_color == bcmPolicerColorGreen)
    {
        is_green_credits = TRUE;
        is_yellow_credits = (other_credits == TRUE) ? TRUE : FALSE;
    }
    else if (policer_color == bcmPolicerColorYellow)
    {
        is_yellow_credits = TRUE;
        is_green_credits = (other_credits == TRUE) ? TRUE : FALSE;
    }

    /** calc how to update the meter */
    if (policer_color == bcmPolicerColorInvalid)
    {
        *update_bucket = bcmPolicerColorInvalid;
    }
    else if (resolved_color == bcmColorRed)
    {
        *update_bucket = bcmPolicerColorInvalid;
    }
    else if (resolved_color == bcmColorGreen && is_green_credits == TRUE)
    {
        *update_bucket = bcmPolicerColorGreen;
    }
    else if (resolved_color == bcmColorYellow && is_yellow_credits == TRUE)
    {
        *update_bucket = bcmPolicerColorYellow;
    }
    else if (resolved_color == bcmColorYellow && is_yellow_credits == FALSE && is_green_credits == TRUE)
    {
        *update_bucket = bcmPolicerColorGreen;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "wrong configuration. (resolved_color=%d, other_credits=%d, policer_color=%d) \n",
                     resolved_color, other_credits, policer_color);
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      configure the ingress and egress policer resolution table (primary table) in serial mode, for all cores
 * \param [in] unit -unit id
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e
dnx_policer_tune_primary_resolved_serial_init(
    int unit)
{
    bcm_policer_primary_color_resolution_key_t key;
    bcm_policer_primary_color_resolution_config_t config;
    bcm_policer_color_t meter_0_color, meter_1_color, meter_2_color;
    int meter_0_other_credits, meter_1_other_credits, meter_2_other_credits;
    int dp_cmd, i;
    int flags = 0;

    bcm_color_t resolved_dp;
    bcm_policer_color_t meter_2_color_init, meter_2_color_last;
    int nof_meter_db, max_meter_2_other_credits;

    SHR_FUNC_INIT_VARS(unit);

    /** configure table for all cores and for ingress */
    key.core_id = BCM_CORE_ALL;
    key.is_ingress = TRUE;

    /** run the same functionality for ingress and egress. */
    for (key.is_ingress = FALSE; key.is_ingress <= TRUE; key.is_ingress++)
    {
        /** the functionality below support bith ingress and egress tables. */
        /** For egress there is one less meters databases, so meter_2 is not relevant */
        /** here, we set the relevant diffrences between ingress and egress */
        if (key.is_ingress == TRUE)
        {
            meter_2_color_init = bcmPolicerColorGreen;
            meter_2_color_last = bcmPolicerColorRed;
            nof_meter_db = dnx_data_meter.meter_db.nof_ingress_db_get(unit);
            max_meter_2_other_credits = TRUE;
        }
        else
        {
            meter_2_color_init = bcmPolicerColorInvalid;
            meter_2_color_last = bcmPolicerColorInvalid;
            nof_meter_db = dnx_data_meter.meter_db.nof_egress_db_get(unit);
            max_meter_2_other_credits = FALSE;
        }

        for (meter_0_color = bcmPolicerColorGreen; meter_0_color <= bcmPolicerColorRed; meter_0_color++)
        {
            for (meter_0_other_credits = FALSE; meter_0_other_credits <= TRUE; meter_0_other_credits++)
            {
                for (meter_1_color = bcmPolicerColorGreen; meter_1_color <= bcmPolicerColorRed; meter_1_color++)
                {
                    for (meter_1_other_credits = FALSE; meter_1_other_credits <= TRUE; meter_1_other_credits++)
                    {
                        for (meter_2_color = meter_2_color_init; meter_2_color <= meter_2_color_last; meter_2_color++)
                        {
                            for (meter_2_other_credits = FALSE; meter_2_other_credits <= max_meter_2_other_credits;
                                 meter_2_other_credits++)
                            {
                                /** set the key */
                                key.color_data[0].policer_color = meter_0_color;
                                key.color_data[1].policer_color = meter_1_color;
                                key.color_data[2].policer_color = meter_2_color;
                                key.color_data[0].policer_other_bucket_has_credits = meter_0_other_credits;
                                key.color_data[1].policer_other_bucket_has_credits = meter_1_other_credits;
                                key.color_data[2].policer_other_bucket_has_credits = meter_2_other_credits;
                                /** start with green color. */
                                resolved_dp = bcmColorGreen;
                                /** run over all meters and for each one find the resolve dp based on the prev meter decision (which simulate serial mode) */
                                for (i = 0; i < nof_meter_db; i++)
                                {
                                    /** don't count if the prev decision is red or if the meter is invalid  */
                                    config.policer_count[i] =
                                        (resolved_dp == bcmColorRed
                                         || key.color_data[i].policer_color == bcmPolicerColorInvalid) ? FALSE : TRUE;

                                    /** the counted policer_color is key->policer_color vonverted to 4color mode */
                                    SHR_IF_ERR_EXIT(dnx_policer_tune_color_convert
                                                    (unit, key.color_data[i].policer_color, &config.policer_color[i]));
                                    /** get the next meter resolved_dp decision, based on the prev decision in order to simulate serial mode */
                                    SHR_IF_ERR_EXIT(dnx_policer_tune_single_resolved_calc
                                                    (unit, resolved_dp, key.color_data[i].policer_color,
                                                     key.color_data[i].policer_other_bucket_has_credits, &resolved_dp));
                                }
                                /** update the resolved color */
                                config.resolved_color = resolved_dp;
                                /** once we decided on the resolve color, we can calculate how to update each meter */
                                for (i = 0; i < nof_meter_db; i++)
                                {
                                    SHR_IF_ERR_EXIT(dnx_policer_tune_update_bucket_set
                                                    (unit, resolved_dp, key.color_data[i].policer_color,
                                                     key.color_data[i].policer_other_bucket_has_credits,
                                                     &config.policer_update_bucket[i]));

                                }

                                /** call the API for each dp_cmd */
                                for (dp_cmd = 0; dp_cmd <= DNX_POLICER_MGMT_DP_CMD_MAX; dp_cmd++)
                                {
                                    key.action = dp_cmd;
                                    SHR_IF_ERR_EXIT(bcm_dnx_policer_primary_color_resolution_set
                                                    (unit, flags, &key, &config));
                                }
                            }
                        }
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *      configure the ingress and egress policer final resolution table , for all cores.
 * \param [in] unit -unit id
 * \return
 *   shr_error_e
 * \remark
 *   NONE
 * \see
 *   NONE
 */
shr_error_e
dnx_policer_tune_final_resolved_init(
    int unit)
{
    bcm_policer_color_resolution_t config;
    int is_ingress, policer_action, global_drop, max_global_drop;
    bcm_color_t incoming_color, resolved_dp;
    SHR_FUNC_INIT_VARS(unit);

    /** configure the final resolution table for ingress and egress */
    for (is_ingress = FALSE; is_ingress <= TRUE; is_ingress++)
    {
        /** set the differneces between ingress and egress tables */
        if (is_ingress == TRUE)
        {
            config.flags = 0;
            max_global_drop = TRUE;
        }
        else
        {
            config.flags = BCM_COLOR_RESOLUTION_EGRESS;
            /** there is no global meter in egress, so it is not relevant */
            max_global_drop = FALSE;
        }

        for (policer_action = 0; policer_action <= DNX_POLICER_MGMT_DP_CMD_MAX; policer_action++)
        {
            for (incoming_color = bcmColorGreen; incoming_color <= bcmColorBlack; incoming_color++)
            {
                for (resolved_dp = bcmColorGreen; resolved_dp <= bcmColorBlack; resolved_dp++)
                {
                    for (global_drop = FALSE; global_drop <= max_global_drop; global_drop++)
                    {
                        /** update the key */
                        config.policer_action = policer_action;
                        config.incoming_color = incoming_color;
                        config.policer_color = resolved_dp;
                        config.ethernet_policer_color = (global_drop == TRUE) ? bcmColorRed : bcmColorGreen;

                        /** set the value */
                        
                        /** Note: if global_drop=TRUE, the HW set the resolved_dp as RED. therefore, driver use the resolved_dp as the output color */
                        config.egress_color = /**  (tm_mode == TRUE) ? incoming_color : */ resolved_dp;
                        /** for egress, ingress color is not relevant ans set as preserved */
                        config.ingress_color = (is_ingress == TRUE) ? config.egress_color : bcmColorPreserve;

                        /** call API */
                        SHR_IF_ERR_EXIT(bcm_dnx_policer_color_resolution_set(unit, &config));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_policer_tune_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_policer_tune_primary_resolved_serial_init(unit));
    SHR_IF_ERR_EXIT(dnx_policer_tune_final_resolved_init(unit));

exit:
    SHR_FUNC_EXIT;

}
