/** ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_policer_resolution_config.c
 * Purpose:     example for resolve tables configuration
 *
 * *
 * The cint include:
 *  - primary resolution configuration in parallel mode between 3 meters (or two meters for egress)
 *  - ingress primary resolution configuration in serial mode between meter0 and the result of {meter1 and meter2 in parallel mode. (m0->[m1||m2])
 *  - primary resolution configuration: how to update the buckets in parallel mode. (3 options: CONSTANT, TRANSPARENT, DEFERRED)
 *  - modify primary resolution table according to the meter[i]-profile-data values (ingress and egress).
 *  - ingress final resolution table configuration - set black color if the global is red and red color if the resolved_color is red.
 *  - modify ingress final resolution table configuration - set black color if the global is red and red color if the resolved_color is red.
 */

int POLICER_UPDATE_PARALLEL_BUCKET_CONSTANT = 0; /** Update the target bucket regardless whether it is positive or not. */

int POLICER_UPDATE_PARALLEL_BUCKET_TRANSPARENT = 1; /** Update the target bucket only if it is positive. If the bucket is negative, don't update any bucket. */

int POLICER_UPDATE_PARALLEL_BUCKET_DEFERRED = 2; /** if the target bucket is positive, then update the target bucket. If the target bucket is negative
                                                                                               and the other bucket is positive then update the other bucket.
                                                                                               If both buckets are negative then don't update any bucket. */

int POLICER_NOF_DP_CMDS_OPTIONS = 16;
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
int
policer_resolution_color_convert(
    int unit,
    bcm_policer_color_t internal_color,
    bcm_color_t * external_color)
{
    switch (internal_color)
    {
        case bcmPolicerColorGreen:
            *external_color = bcmColorGreen;
            break;
        case bcmPolicerColorYellow:
            *external_color = bcmColorYellow;
            break;
        case bcmPolicerColorRed:
            *external_color = bcmColorRed;
            break;
        case bcmPolicerColorInvalid:
            *external_color = bcmColorRed;
            break;
        default:
            printf("Invalid policer color \n");
            return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/**
* \brief
*      determine how to update the bucket in parallel mode, base on the update_bucket mode and the meter color and other_credits indication.
* \param [in] unit -unit id
* \param [in] resolved_color -color decision for all serial mode
* \param [in] policer_color -currect meter decision as made by the HW
* \param [in] other_credits -is the meter has credits from other bucket
* \param [in] update_bucket_mode -how to update the bucket: CONSTANT, TRANSPARENT, DEFERRED.
* \param [out] update_bucket -how to update the meter buckets (3 color mode)

* \return
*   shr_error_e
* \remark
*   NONE
* \see
*   NONE
*/
int
policer_resolution_update_bucket_parallel_calc(
    int unit,
    bcm_color_t resolved_color,
    bcm_policer_color_t policer_color,
    int other_credits,
    int update_bucket_mode,
    bcm_policer_color_t * update_bucket)
{
    int is_green_credits = FALSE, is_yellow_credits = FALSE;

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

    switch (update_bucket_mode)
    {
        case POLICER_UPDATE_PARALLEL_BUCKET_CONSTANT:
            /** If red or black then don't reduce anything. */
            if (resolved_color == bcmColorRed || resolved_color == bcmColorBlack)
            {
                *update_bucket = bcmPolicerColorInvalid;
            }
            /*
             * Otherwise according to result color
             */
            else
            {
                *update_bucket = (resolved_color == bcmColorGreen) ? bcmPolicerColorGreen : bcmPolicerColorYellow;
            }
            break;
        case POLICER_UPDATE_PARALLEL_BUCKET_TRANSPARENT:
            /** If red or black then don't reduce anything. */
            if (resolved_color == bcmColorRed || resolved_color == bcmColorBlack)
            {
                *update_bucket = bcmPolicerColorInvalid;
            }
            /** If we have the same color credits then reduce them */
            else if (resolved_color == bcmColorGreen && is_green_credits)
            {
                *update_bucket = bcmPolicerColorGreen;
            }
            else if (resolved_color == bcmColorYellow && is_yellow_credits)
            {
                *update_bucket = bcmPolicerColorYellow;
            }
            /** Otherwise none */
            else
            {
                *update_bucket = bcmPolicerColorInvalid;
            }
            break;
        case POLICER_UPDATE_PARALLEL_BUCKET_DEFERRED:
            /** If red or black then don't reduce anything. */
            if (resolved_color == bcmColorRed || resolved_color == bcmColorBlack)
            {
                *update_bucket = bcmPolicerColorInvalid;
            }
            /** If green, try to reduce from green and than from yellow */
            else if (resolved_color == bcmColorGreen)
            {
                if (is_green_credits)
                {
                    *update_bucket = bcmPolicerColorGreen;
                }
                else if (is_yellow_credits)
                {
                    *update_bucket = bcmPolicerColorYellow;
                }
                else
                {
                    *update_bucket = bcmPolicerColorInvalid;
                }
            }
            /** (resolved_color == bcmColorYellow) - try to reduce from yellow and than from green */
            else
            {
                if (is_yellow_credits)
                {
                    *update_bucket = bcmPolicerColorYellow;
                }
                else if (is_green_credits)
                {
                    *update_bucket = bcmPolicerColorGreen;
                }
                else
                {
                    *update_bucket = bcmPolicerColorInvalid;
                }
            }

            break;
        default:
            printf("Invalid update_bucket_mode \n");
            return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

void
policer_resolution_dp_calc(
    int unit,
    int resolve_table_mode_is_worst,
    bcm_policer_color_t internal_policer_color,
    bcm_color_t current_policer_color,
    bcm_color_t * resolved_dp)
{
    if (internal_policer_color == bcmPolicerColorInvalid)
    {
        *resolved_dp = *resolved_dp;
    }
    else if (resolve_table_mode_is_worst == TRUE)
    {
        *resolved_dp = (*resolved_dp > current_policer_color) ? *resolved_dp : current_policer_color;
    }
    else
    {
        *resolved_dp = (*resolved_dp < current_policer_color) ? *resolved_dp : current_policer_color;
    }

}

/**
* \brief
*     configure the meter resolution table (the primary table) in parallel mode of all 3 meters for ingress and 2 meters for egress.
* \param [in] unit -unit id
* \param [in] is_ingress -ingress or egress
* \param [in] resolve_table_mode_is_worst -is resolved table mode is best or worst. It determines whether in parallel mode the
*                     final meter result is the best of the two meter results or (more commonly) to the worst of the two meter results.
*
* \param [in] update_bucket_mode -how to update the bucket: CONSTANT, TRANSPARENT, DEFERRED.
*     Explanation:
*     The bucket modes indicate which bucket to update according to the meter color, the final color, and whether the
*     bucket is negative. 
*     The bucket modes imply the following:
*      -CONSTANT: Update the bucket (according to resolve color) regardless whether it is positive or not.
*      -TRANSPARENT: Update the bucket (according to resolve color) only if it is positive. If the bucket is negative, don't update the bucket.
*      -DEFERRED: If the bucket (according to resolve color) is positive, update the bucket. if it is negative
*             and the other bucket is positive then update the other bucket. If both buckets are negative then don't update any bucket.
*
* \param [in] mark_black -mark_black indicator.
*      example of using the meter-profile bit as "mark_black". (same as was in QUX device).
*      It gives the user the option for different mapping between meters that were marked in their profile and their color decision is Red.
*      if the entry contain at least one meter which its meter-profile-bit=1, and its color is Red, the resolve color will be mark as black instead of red.
*      Note: it is expected that the meter processor (ingress or egress) using the profile-data and not the dp-cmd data
*       (this is the default configuration and can be modified by control API)
*
* \return
*   error indication.
* \remark
*   NONE
* \see
*   NONE
*/
int
policer_primary_resolution_parallel_three_meters_example(
    int unit,
    int is_ingress,
    int resolve_table_mode_is_worst,
    int update_bucket_mode,
    int mark_black)
{
    bcm_policer_primary_color_resolution_key_t key;
    bcm_policer_primary_color_resolution_config_t config;
    int meter_0_color, meter_1_color, meter_2_color;
    int meter_0_other_credits, meter_1_other_credits, meter_2_other_credits;
    int meter_profile_data, i, rv;
    int flags = 0;
    const uint32 *nof_ingress_meter_db = dnxc_data_get(unit, "meter", "meter_db", "nof_ingress_db", NULL);
    const uint32 *nof_egress_meter_db = dnxc_data_get(unit, "meter", "meter_db", "nof_egress_db", NULL);
    bcm_color_t resolved_dp[POLICER_NOF_DP_CMDS_OPTIONS], current_policer_color, resolved_dp_base;
    bcm_policer_color_t meter_2_color_init, meter_2_color_last;
    uint32 nof_meter_db;
    int max_meter_2_other_credits;

    bcm_policer_primary_color_resolution_config_t_init(&config);
    bcm_policer_primary_color_resolution_key_t_init(&key);
    /** configure table for all cores and for ingress */
    key.core_id = BCM_CORE_ALL;
    key.is_ingress = is_ingress;

    /** the functionality below support bith ingress and egress tables. */
    /** For egress there is one less meters databases, so meter_2 is not relevant */
    /** here, we set the relevant diffrences between ingress and egress */
    if (key.is_ingress == TRUE)
    {
        meter_2_color_init = bcmPolicerColorGreen;
        meter_2_color_last = bcmPolicerColorRed;
        nof_meter_db = *nof_ingress_meter_db;
        max_meter_2_other_credits = TRUE;
    }
    else
    {
        meter_2_color_init = bcmPolicerColorInvalid;
        meter_2_color_last = bcmPolicerColorInvalid;
        nof_meter_db = *nof_egress_meter_db;
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

                             /** set the initial resolve_dp. if it is worst mode - set it to the best option, if best mode, set it to the worst option */
                             /** if all invalid, it will be red */
                            if (key.color_data[0].policer_color == bcmPolicerColorInvalid &&
                                key.color_data[1].policer_color == bcmPolicerColorInvalid &&
                                key.color_data[2].policer_color == bcmPolicerColorInvalid)
                            {
                                resolved_dp_base = bcmColorRed;
                            }
                            else if (resolve_table_mode_is_worst == TRUE)
                            {
                                resolved_dp_base = bcmColorGreen;
                            }
                            else
                            {
                                 /** if mark_black option, the worst color is black */
                                resolved_dp_base = mark_black == TRUE ? bcmColorBlack : bcmColorRed;
                            }
                            for (i = 0; i < nof_meter_db; i++)
                            {
                                 /** don't count if the meter color is invalid */
                                config.policer_count[i] =
                                    (key.color_data[i].policer_color == bcmPolicerColorInvalid) ? FALSE : TRUE;

                                 /** the counted policer_color is key->policer_color converted to 4color mode */
                                rv = policer_resolution_color_convert
                                    (unit, key.color_data[i].policer_color, &config.policer_color[i]);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in policer_resolution_color_convert \n");
                                    return rv;
                                }
                            }

                             /** Note: if working in mark_black mode, the resolve_dp calculation is affected by the meter_profile_data, */
                             /** so the logic has to be inside the loop of meter_profile_data. */
                             /** If it is not mark black, we do the logic of calculating the resolve dp outside the loop to optimise the timeIn order to optimize the time */
                            if (mark_black == TRUE)
                            {
                                 /** calculate the resolve_dp and how to update thebucket per profile_data (or dp_cmd) */
                                for (meter_profile_data = 0; meter_profile_data < POLICER_NOF_DP_CMDS_OPTIONS;
                                     meter_profile_data++)
                                {
                                    resolved_dp[i] = resolved_dp_base;
                                    /** run over all meters and for each one configure its attributes */
                                    for (i = 0; i < nof_meter_db; i++)
                                    {
                                        /** if mark black mode and the meter-profile-data (mark_black_ind) is true and the color is Red, change it to black*/
                                        if ((meter_profile_data & (1 << i))
                                            && (key.color_data[i].policer_color == bcmPolicerColorRed))
                                        {
                                            current_policer_color = bcmColorBlack;
                                        }
                                        else
                                        {
                                            current_policer_color = config.policer_color[i];
                                        }

                                        /** calc the next resoved_dp stage according to the table mode */
                                        /** if invalid, the next stage is the same as the prev stage */
                                        policer_resolution_dp_calc(unit, resolve_table_mode_is_worst,
                                                                   key.color_data[i].policer_color,
                                                                   current_policer_color, &resolved_dp[i]);
                                    }
                                }
                            }
                            else
                            {
                                resolved_dp[0] = resolved_dp_base;
                                for (i = 0; i < nof_meter_db; i++)
                                {
                                    /** calc the next resoved_dp stage according to the table mode */
                                    /** if invalid, the next stage is the same as the prev stage */
                                    policer_resolution_dp_calc(unit, resolve_table_mode_is_worst,
                                                               key.color_data[i].policer_color, config.policer_color[i],
                                                               &resolved_dp[0]);
                                }
                            }

                            /** once we decided on the resolve color, we can calculate how to update each meter. */
                            /** resolved_dp[16] is all the same unless if the mark_black=TRUE and the decision was red. */
                            /** In such case, some of the elements will be red and other black. */
                            /** update bucket is not sensitive to Red/Black. therfore, we can use any of the resolved_dp indexes. */
                            /** We use index zero */
                            for (i = 0; i < nof_meter_db; i++)
                            {
                                rv = policer_resolution_update_bucket_parallel_calc(unit, resolved_dp[0],
                                                                                    key.color_data[i].policer_color,
                                                                                    key.color_data
                                                                                    [i].policer_other_bucket_has_credits,
                                                                                    update_bucket_mode,
                                                                                    &config.policer_update_bucket[i]);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in policer_resolution_update_bucket_parallel_calc \n");
                                    return rv;
                                }
                            }
                            /** call the APi for each one of the profile data (or dp_cmd) */
                            for (meter_profile_data = 0; meter_profile_data < POLICER_NOF_DP_CMDS_OPTIONS;
                                 meter_profile_data++)
                            {
                                /** update the resolved color */
                                config.resolved_color =
                                    (mark_black == TRUE) ? resolved_dp[meter_profile_data] : resolved_dp[0];
                                key.action = meter_profile_data;
                                rv = bcm_policer_primary_color_resolution_set(unit, flags, &key, &config);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in bcm_policer_primary_color_resolution_set \n");
                                    return rv;
                                }
                                /** compare the configuration- not mandatory, just for validation*/
                                rv = policer_primary_color_resolution_compare(unit,&key,&config);
                                if (rv != BCM_E_NONE)
                                {
                                    return rv;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return BCM_E_NONE;

}

/**
* \brief
*     configure the meter resolution table (the primary table) in the following way: M0>>(M1||M2).
*     ignore the dp-cmd inputs.
*     get the configuration of each entry and verify that set_config=get_config
* \param [in] unit -unit id
* \param [in] is_ingress -ingress or egress
* \param [in] resolve_table_mode_is_worst -is resolved table mode is best or worst. It determines whether in parallel mode the
*                     final meter result is the best of the two meter results or (more commonly) to the worst of the two meter results.
*
* \param [in] update_bucket_mode -how to update the bucket: CONSTANT, TRANSPARENT, DEFERRED.
*     Explanation:
*     The bucket modes indicate which bucket to update according to the meter color, the final color, and whether the
*     bucket is negative. For example, consider a packet that resolves in one meter to Green, and in one meter to
*     Yellow, final color resolution is Green.
*     The bucket with the same color as the final color bucket is referred to be the target bucket (in other words, the
*     Green bucket when the final color is Green). The other bucket is referred to as the other bucket (in other words,
*     the Yellow bucket when the final color is Green).
*     The bucket modes imply the following:
*         -CONSTANT: Update the target bucket regardless whether it is positive or not.
*         -TRANSPARENT: Update the target bucket only if it is positive. If the bucket is negative, don't update any bucket.
*         -DEFERRED: If the target bucket is positive, then update the target bucket. If the target bucket is negative
*             and the other bucket is positive then update the other bucket. If both buckets are negative then don't update any bucket.
*
* \return
*   error indication.
* \remark
*   THis example will generate resolve color: green, yellow or red. but not black!!
* \see
*   NONE
*/
int
policer_ingress_primary_resolution_serial_meter0_to_parallel_other_meters_example(
    int unit,
    int resolve_table_mode_is_worst,
    int update_bucket_mode)
{
    bcm_policer_primary_color_resolution_key_t key;
    bcm_policer_primary_color_resolution_config_t config, config_get;
    int meter_0_color, meter_1_color, meter_2_color, parallel_color;
    int meter_0_other_credits, meter_1_other_credits, meter_2_other_credits;
    int dp_cmd, i, rv;
    int flags = 0;
    bcm_color_t resolved_dp, converted_parallel_color;

    bcm_policer_primary_color_resolution_config_t_init(&config);
    bcm_policer_primary_color_resolution_config_t_init(&config_get);
    bcm_policer_primary_color_resolution_key_t_init(&key);
    /** configure table for all cores and for ingress */
    key.core_id = BCM_CORE_ALL;
    key.is_ingress = TRUE;

    for (meter_0_color = bcmPolicerColorGreen; meter_0_color <= bcmPolicerColorRed; meter_0_color++)
    {
        for (meter_0_other_credits = FALSE; meter_0_other_credits <= TRUE; meter_0_other_credits++)
        {
            for (meter_1_color = bcmPolicerColorGreen; meter_1_color <= bcmPolicerColorRed; meter_1_color++)
            {
                for (meter_1_other_credits = FALSE; meter_1_other_credits <= TRUE; meter_1_other_credits++)
                {
                    for (meter_2_color = bcmPolicerColorGreen; meter_2_color <= bcmPolicerColorRed; meter_2_color++)
                    {
                        for (meter_2_other_credits = FALSE; meter_2_other_credits <= TRUE; meter_2_other_credits++)
                        {
                             /** set the key */
                            key.color_data[0].policer_color = meter_0_color;
                            key.color_data[1].policer_color = meter_1_color;
                            key.color_data[2].policer_color = meter_2_color;
                            key.color_data[0].policer_other_bucket_has_credits = meter_0_other_credits;
                            key.color_data[1].policer_other_bucket_has_credits = meter_1_other_credits;
                            key.color_data[2].policer_other_bucket_has_credits = meter_2_other_credits;

                            for (i = 0; i < 3; i++)
                            {
                                /** don't count if the meter color is invalid */
                                config.policer_count[i] =
                                    (key.color_data[i].policer_color == bcmPolicerColorInvalid) ? FALSE : TRUE;

                                /** the counted policer_color is key->policer_color converted to 4color mode */
                                rv = policer_resolution_color_convert
                                    (unit, key.color_data[i].policer_color, &config.policer_color[i]);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in policer_resolution_color_convert \n");
                                    return rv;
                                }
                            }

                            /** find the resolution for parallel meter1 and meter2 */
                            if (key.color_data[1].policer_color == bcmPolicerColorInvalid)
                            {
                                parallel_color = key.color_data[2].policer_color;
                            }
                            else if (key.color_data[2].policer_color == bcmPolicerColorInvalid)
                            {
                                parallel_color = key.color_data[1].policer_color;
                            }
                            else if (resolve_table_mode_is_worst == TRUE)
                            {
                                parallel_color = (key.color_data[1].policer_color > key.color_data[2].policer_color) ?
                                    key.color_data[1].policer_color : key.color_data[2].policer_color;
                            }
                            else
                            {
                                parallel_color = (key.color_data[1].policer_color < key.color_data[2].policer_color) ?
                                    key.color_data[1].policer_color : key.color_data[2].policer_color;
                            }
                            /** the counted policer_color is key->policer_color converted to 4color mode */
                            rv = policer_resolution_color_convert(unit, parallel_color, converted_parallel_color);
                            if (rv != BCM_E_NONE)
                            {
                                printf("Error in policer_resolution_color_convert \n");
                                return rv;
                            }

                            /** find the resolution of meter0>>(meter1||meter2) */
                            /** also decide how to update meter0 bucket. (meter1 and 2 calc will be made later */
                            if ((meter_0_color == bcmPolicerColorInvalid))
                            {
                                resolved_dp = converted_parallel_color;
                                config.policer_update_bucket[0] = bcmPolicerColorInvalid;
                            }
                            else if (parallel_color == bcmPolicerColorInvalid)
                            {
                                resolved_dp = config.policer_color[0];
                                config.policer_update_bucket[0] =
                                    (meter_0_color == bcmPolicerColorRed) ? bcmPolicerColorInvalid : meter_0_color;
                            }
                            else if (meter_0_color == bcmPolicerColorRed || parallel_color == bcmPolicerColorRed)
                            {
                                resolved_dp = bcmColorRed;
                                config.policer_update_bucket[0] = bcmPolicerColorInvalid;
                                /** in serial mode, don't count meters after red decision was made */
                                if (meter_0_color == bcmPolicerColorRed)
                                {
                                    config.policer_count[1] = config.policer_count[2] = FALSE;
                                }
                            }
                            else if (meter_0_color == parallel_color)
                            {
                                resolved_dp = converted_parallel_color;
                                config.policer_update_bucket[0] =
                                    (meter_0_color == bcmPolicerColorRed) ? bcmPolicerColorInvalid : meter_0_color;
                            }
                            else if ((meter_0_color == bcmPolicerColorGreen)
                                     && (parallel_color == bcmPolicerColorYellow))
                            {
                                resolved_dp = bcmColorYellow;
                                config.policer_update_bucket[0] = (key.color_data[0].policer_other_bucket_has_credits) ?
                                    bcmPolicerColorYellow : bcmPolicerColorGreen;
                            }
                            else                 /**(meter_0_color == bcmPolicerColorYellow) && (parallel_color == bcmPolicerColorGreen))*/
                            {
                                /** resolve_dp is yellow only if one of the parallel has yellow credits. otherwse, red */
                                if (key.color_data[1].policer_other_bucket_has_credits
                                    || key.color_data[2].policer_other_bucket_has_credits)
                                {
                                    resolved_dp = bcmColorYellow;
                                    config.policer_update_bucket[0] = bcmPolicerColorYellow;
                                }
                                else
                                {
                                    resolved_dp = bcmColorRed;
                                    config.policer_update_bucket[0] = bcmPolicerColorInvalid;
                                }
                            }

                            config.resolved_color = resolved_dp;

                            /** once we decided on the resolve color, we can calculate how to update each of the parallel meters */
                            for (i = 1; i < 3; i++)
                            {
                                rv = policer_resolution_update_bucket_parallel_calc(unit, resolved_dp,
                                                                                    key.color_data[i].policer_color,
                                                                                    key.color_data
                                                                                    [i].policer_other_bucket_has_credits,
                                                                                    update_bucket_mode,
                                                                                    &config.policer_update_bucket[i]);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in policer_resolution_update_bucket_parallel_calc \n");
                                    return rv;
                                }
                            }

                            /** call the APi for each one of the profile data (or dp_cmd) */
                            for (dp_cmd = 0; dp_cmd < POLICER_NOF_DP_CMDS_OPTIONS; dp_cmd++)
                            {
                                key.action = dp_cmd;
                                rv = bcm_policer_primary_color_resolution_set(unit, flags, &key, &config);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in bcm_policer_primary_color_resolution_set \n");
                                    return rv;
                                }
                                /** compare the configuration- not mandatory, just for validation*/
                                rv = policer_primary_color_resolution_compare(unit,&key,&config);
                                if (rv != BCM_E_NONE)
                                {
                                    return rv;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return BCM_E_NONE;

}
int policer_primary_color_resolution_compare(
    int unit,
    bcm_policer_primary_color_resolution_key_t *key,
    bcm_policer_primary_color_resolution_config_t * config)
{
    int rv,flags = 0;
    bcm_policer_primary_color_resolution_config_t config_get;
    bcm_policer_primary_color_resolution_config_t_init(&config_get);
    rv = bcm_policer_primary_color_resolution_get(unit, flags, key, &config_get);
    if (rv != BCM_E_NONE)
    {
        printf("Error in bcm_policer_primary_color_resolution_get \n");
        printf("Key:\n");
        print *key;
        return rv;
    }
    if(sal_memcmp(config,&config_get,sizeof(config_get))){
        printf("Confiugred and received primary resolution table mis-match.\n");
        printf("Key:\n");
        print *key;
        printf("Configured:\n");
        print *config;
        printf("Received:\n");
        print config_get;
        return BCM_E_FAIL;
    }
    return BCM_E_NONE;
}
/**
* \brief
*     example of modifying a egress primary resolution table in the following way:
*       if the meter-profile-nit is set and the meter_decision is red and the resolve_dp is red, modify it to black
* \param [in] unit -unit id
*
* \return
*   error indication.
* \remark
*      This is example for backword compatability as was in  QUX device. ("mark_black" feature)
*      It gives the user the option for different mapping between meters that were marked in their profile and their color decision is Red.
*      if the entry contain at least one meter which its meter-profile-bit=1, and its color is Red, the resolve color will be mark as black instead of red.
*      Note: it is expected that the meter processor (ingress or egress) using the profile-data and not the dp-cmd data
*       (this is the default configuration and can be modified by control API)

* \see
*   NONE
*/
int
policer_egress_primary_resolution_mark_black_modify_example(
    int unit)
{
    bcm_policer_primary_color_resolution_key_t key;
    bcm_policer_primary_color_resolution_config_t config, config_get;
    int meter_0_color, meter_1_color, meter_2_color, parallel_color;
    int meter_0_other_credits, meter_1_other_credits, meter_2_other_credits;
    int profile_data, i, rv;
    int flags = 0;
    bcm_color_t resolved_dp, converted_parallel_color;

    bcm_policer_primary_color_resolution_config_t_init(&config);
    bcm_policer_primary_color_resolution_config_t_init(&config_get);
    bcm_policer_primary_color_resolution_key_t_init(&key);
    /** configure table for all cores and for ingress */
    key.core_id = BCM_CORE_ALL;
    key.is_ingress = FALSE;

    for (meter_0_color = bcmPolicerColorGreen; meter_0_color <= bcmPolicerColorRed; meter_0_color++)
    {
        for (meter_0_other_credits = FALSE; meter_0_other_credits <= TRUE; meter_0_other_credits++)
        {
            for (meter_1_color = bcmPolicerColorGreen; meter_1_color <= bcmPolicerColorRed; meter_1_color++)
            {
                for (meter_1_other_credits = FALSE; meter_1_other_credits <= TRUE; meter_1_other_credits++)
                {
                    /** call the APi for each one of the profile data (or dp_cmd) */
                    for (profile_data = 0; profile_data < POLICER_NOF_DP_CMDS_OPTIONS; profile_data++)
                    {
                         /** set the key */
                        key.color_data[0].policer_color = meter_0_color;
                        key.color_data[1].policer_color = meter_1_color;
                        key.color_data[0].policer_other_bucket_has_credits = meter_0_other_credits;
                        key.color_data[1].policer_other_bucket_has_credits = meter_1_other_credits;
                        key.action = profile_data;
                        rv = bcm_policer_primary_color_resolution_get(unit, flags, &key, &config);
                        if (rv != BCM_E_NONE)
                        {
                            printf("Error in bcm_policer_primary_color_resolution_get \n");
                            return rv;
                        }
                        /** if the profile bit is set and the meter color is red and the resolve_dp is red, change it to black */
                        if (config.resolved_color == bcmColorRed)
                        {
                            if (((meter_0_color == bcmPolicerColorRed) && (profile_data & 1)) ||
                                ((meter_1_color == bcmPolicerColorRed) && (profile_data & 2)))
                            {
                                config.resolved_color = bcmColorBlack;
                                rv = bcm_policer_primary_color_resolution_set(unit, flags, &key, &config);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in bcm_policer_primary_color_resolution_set \n");
                                    return rv;
                                }
                                /** get back the configuration - not mandatory, just for validation */
                                rv = bcm_policer_primary_color_resolution_get(unit, flags, &key, &config_get);
                                if (rv != BCM_E_NONE)
                                {
                                    printf("Error in bcm_policer_primary_color_resolution_get \n");
                                    return rv;
                                }
                                /** compare */
                                if ((config.resolved_color != config_get.resolved_color) ||
                                    (config.policer_count[0] != config_get.policer_count[0]) ||
                                    (config.policer_count[1] != config_get.policer_count[1]) ||
                                    (config.policer_count[2] != config_get.policer_count[2]) ||
                                    (config.policer_color[0] != config_get.policer_color[0]) ||
                                    (config.policer_color[1] != config_get.policer_color[1]) ||
                                    (config.policer_color[2] != config_get.policer_color[2]) ||
                                    (config.policer_update_bucket[0] != config_get.policer_update_bucket[0]) ||
                                    (config.policer_update_bucket[1] != config_get.policer_update_bucket[1]) ||
                                    (config.policer_update_bucket[2] != config_get.policer_update_bucket[2]))
                                {
                                    printf("Confiugred and received primary resolution table mis-match.\n");
                                    printf("Key:\n");
                                    print key;
                                    printf("Configured:\n");
                                    print config;
                                    printf("Received:\n");
                                    print config_get;
                                    return BCM_E_FAIL;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    return BCM_E_NONE;

}

/**
* \brief
*   example of configuring the ingress final resolution table in the following way:
*       set the output color according to the resolve color except of the following case:
*           if the global meter show drop:
*                   set the ingress output color to Black if global_drop_as_black_for_ingress=TRUE, otherwise Red .
*                   set the egress output color to Black if global_drop_as_black_for_ingress=TRUE, otherwise Red .
* \param [in] unit -unit id
* \param [in] global_drop_as_black_for_ingress -see above description
* \param [in] global_drop_as_black_for_egress -see above description
* \param [in] unit -unit id
* \return
*   error indication.
* \remark
* \see
*   NONE
*/
int
policer_ingress_final_resolution_example(
    int unit,
    int global_drop_as_black_for_ingress,
    int global_drop_as_black_for_egress)
{
    bcm_policer_color_resolution_t config, config_get;
    int policer_action, global_drop, rv;
    int incoming_color, resolved_dp;

    bcm_policer_color_resolution_t_init(&config);
    bcm_policer_color_resolution_t_init(&config_get);
    config.flags = config_get.flags = 0;

    for (policer_action = 0; policer_action < POLICER_NOF_DP_CMDS_OPTIONS; policer_action++)
    {
        for (incoming_color = bcmColorGreen; incoming_color <= bcmColorBlack; incoming_color++)
        {
            for (resolved_dp = bcmColorGreen; resolved_dp <= bcmColorBlack; resolved_dp++)
            {
                for (global_drop = FALSE; global_drop <= TRUE; global_drop++)
                {
                    /** update the key */
                    config_get.policer_action = config.policer_action = policer_action;
                    config_get.incoming_color = config.incoming_color = incoming_color;
                    config_get.policer_color = config.policer_color = resolved_dp;
                    config_get.ethernet_policer_color = config.ethernet_policer_color =
                        (global_drop == TRUE) ? bcmColorRed : bcmColorGreen;

                    /** set the value */
                    if (global_drop == TRUE)
                    {
                        config.ingress_color = (global_drop_as_black_for_ingress == TRUE) ? bcmColorBlack : bcmColorRed;
                        config.egress_color = (global_drop_as_black_for_egress == TRUE) ? bcmColorBlack : bcmColorRed;
                    }
                    else
                    {
                        config.ingress_color = config.egress_color = resolved_dp;
                    }

                    /** call API */
                    rv = bcm_policer_color_resolution_set(unit, &config);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error in bcm_dnx_policer_color_resolution_set \n");
                        return rv;
                    }
                    /** get back config - not mandatory,  just for validation */
                    rv = bcm_policer_color_resolution_get(unit, &config_get);
                    if (rv != BCM_E_NONE)
                    {
                        printf("Error in bcm_dnx_policer_color_resolution_get \n");
                        return rv;
                    }
                    /** compare */
                    if (config.ingress_color != config_get.ingress_color
                        || config.egress_color != config_get.egress_color)
                    {
                        printf("Confiugred and received final resolution table mis-match.\n");
                        printf("Configured:\n");
                        print config;
                        printf("Received:\n");
                        print config_get;
                        return BCM_E_FAIL;
                    }
                }
            }
        }
    }

    return BCM_E_NONE;
}

/**
* \brief
*   example of configuring egress final resolution different from the default configuration:
*       set the output color according to the dp_cmd and incoming_color:
*       1. if action (dp_cmd) = > 8, set the output by the incoming_color, otherwise, set it according to the resolved_dp
* \param [in] unit -unit id
* \return
*   error indication.
* \remark
* \see
*   NONE
*/
int
policer_egress_final_resolution_example(
    int unit)
{
    bcm_policer_color_resolution_t config, config_get;
    int policer_action, global_drop, rv;
    int incoming_color, resolved_dp;

    bcm_policer_color_resolution_t_init(&config);
    bcm_policer_color_resolution_t_init(&config_get);
    config.flags = config_get.flags = BCM_COLOR_RESOLUTION_EGRESS;

    for (policer_action = 0; policer_action < POLICER_NOF_DP_CMDS_OPTIONS; policer_action++)
    {
        for (incoming_color = bcmColorGreen; incoming_color <= bcmColorBlack; incoming_color++)
        {
            for (resolved_dp = bcmColorGreen; resolved_dp <= bcmColorBlack; resolved_dp++)
            {
                /** update the key */
                config_get.policer_action = config.policer_action = policer_action;
                config_get.incoming_color = config.incoming_color = incoming_color;
                config_get.policer_color = config.policer_color = resolved_dp;
                /** ingress is not relevant, set it to preserved */
                config.ingress_color = bcmColorPreserve;

                /** set the value */
                if (policer_action >= 8)
                {
                    config.egress_color = incoming_color;
                }
                else
                {
                    config.egress_color = resolved_dp;
                }

                /** call API */
                rv = bcm_policer_color_resolution_set(unit, &config);
                if (rv != BCM_E_NONE)
                {
                    printf("Error in bcm_dnx_policer_color_resolution_set \n");
                    return rv;
                }
                /** get back config - not mandatory,  just for validation */
                rv = bcm_policer_color_resolution_get(unit, &config_get);
                if (rv != BCM_E_NONE)
                {
                    printf("Error in bcm_dnx_policer_color_resolution_get \n");
                    return rv;
                }
                /** compare */
                if ((config.ingress_color != config_get.ingress_color) ||
                    (config.egress_color != config_get.egress_color))
                {
                    printf("Confiugred and received final resolution table mis-match.\n");
                    printf("Configured:\n");
                    print config;
                    printf("Received:\n");
                    print config_get;
                    return BCM_E_FAIL;
                }
            }
        }
    }

    return BCM_E_NONE;
}
