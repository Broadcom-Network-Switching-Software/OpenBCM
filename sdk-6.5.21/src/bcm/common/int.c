/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * INT - In-band Telemetry Turnaround Embedded Application APP interface
 * Purpose: API to configure In-band Telemetry Turnaround embedded app interface.
 */
#if defined(INCLUDE_INT)
#include <shared/bslenum.h>
#include <shared/bsl.h>

#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/int.h>

/* Initialize INT Turnaround configuration information structure. */
void bcm_int_turnaround_config_t_init(bcm_int_turnaround_config_t *config_info)
{
    sal_memset(config_info, 0, sizeof(bcm_int_turnaround_config_t));
}

/*
 * Function:
 *      bcm_int_ifa_vector_match_t_init
 * Purpose:
 *      Initialize the bcm_int_ifa_vector_match_t struct.
 * Parameters:
 *      vector_match - IFA vector match structure to be nitialized
 */
void bcm_int_ifa_vector_match_t_init(
    bcm_int_ifa_vector_match_t *vector_match)
{
    if (vector_match != NULL) {
        sal_memset(vector_match, 0, sizeof(bcm_int_ifa_vector_match_t));
    }
}

/*
 * Function:
 *      bcm_int_ioam_vector_match_t_init
 * Purpose:
 *      Initialize the bcm_int_ioam_vector_match_t struct.
 * Parameters:
 *      vector_match - IOAM vector match structure to be nitialized
 */
void bcm_int_ioam_vector_match_t_init(
    bcm_int_ioam_vector_match_t *vector_match)
{
    if (vector_match != NULL) {
        sal_memset(vector_match, 0, sizeof(bcm_int_ioam_vector_match_t));
    }
}

/*
 * Function:
 *      bcm_int_dp_vector_match_t_init
 * Purpose:
 *      Initialize the bcm_int_dp_vector_match_t struct.
 * Parameters:
 *      vector_match - INT-DP vector match structure to be nitialized
 */
void bcm_int_dp_vector_match_t_init(
    bcm_int_dp_vector_match_t *vector_match)
{
    if (vector_match != NULL) {
        sal_memset(vector_match, 0, sizeof(bcm_int_dp_vector_match_t));
    }
}

/*
 * Function:
 *      bcm_int_ifa_action_profile_t_init
 * Purpose:
 *      Initialize the bcm_int_ifa_action_profile_t struct.
 * Parameters:
 *      profile - Action profile
 */
void bcm_int_ifa_action_profile_t_init(
    bcm_int_ifa_action_profile_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(bcm_int_ifa_action_profile_t));
    }
}

/*
 * Function:
 *      bcm_int_ioam_action_profile_t_init
 * Purpose:
 *      Initialize the bcm_int_ioam_action_profile_t struct.
 * Parameters:
 *      profile - Action profile
 */
void bcm_int_ioam_action_profile_t_init(
    bcm_int_ioam_action_profile_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(bcm_int_ioam_action_profile_t));
    }
}

/*
 * Function:
 *      bcm_int_dp_action_profile_t_init
 * Purpose:
 *      Initialize the bcm_int_dp_action_profile_t struct.
 * Parameters:
 *      profile - Action profile
 */
void bcm_int_dp_action_profile_t_init(
    bcm_int_dp_action_profile_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(bcm_int_dp_action_profile_t));
    }
}

/*
 * Function:
 *      bcm_int_opaque_data_profile_t_init
 * Purpose:
 *      Initialize the bcm_int_opaque_data_profile_t struct.
 * Parameters:
 *      profile - Action profile
 */
void bcm_int_opaque_data_profile_t_init(
    bcm_int_opaque_data_profile_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(bcm_int_opaque_data_profile_t));
    }
}

/*
 * Function:
 *      bcm_int_cosq_stat_config_t_init
 * Purpose:
 *      Initialize the bcm_int_cosq_stat_config_t struct.
 * Parameters:
 *      profile - Action profile
 */
void bcm_int_cosq_stat_config_t_init(
    bcm_int_cosq_stat_config_t *profile)
{
    if (profile != NULL) {
        sal_memset(profile, 0, sizeof(bcm_int_cosq_stat_config_t));
    }
}

/*
 * Function:
 *      bcm_int_metadata_field_select_t_init
 * Purpose:
 *      Initialize the bcm_int_metadata_field_select_t struct.
 * Parameters:
 *      field_select - Metadata field select
 */
void bcm_int_metadata_field_select_t_init(
    bcm_int_metadata_field_select_t *field_select)
{
    if (field_select != NULL) {
        sal_memset(field_select, 0, sizeof(bcm_int_metadata_field_select_t));
    }
}

/*
 * Function:
 *      bcm_int_metadata_field_construct_t_init
 * Purpose:
 *      Initialize the bcm_int_metadata_field_construct_t struct.
 * Parameters:
 *      field_construct - Metadata field construct
 */
void bcm_int_metadata_field_construct_t_init(
    bcm_int_metadata_field_construct_t *field_construct)
{
    if (field_construct != NULL) {
        sal_memset(field_construct, 0,
                   sizeof(bcm_int_metadata_field_construct_t));
    }
}

/*
 * Function:
 *      bcm_int_metadata_field_entry_t_init
 * Purpose:
 *      Initialize the bcm_int_metadata_field_entry_t struct.
 * Parameters:
 *      field_entry  - Metadata field entry
 */
void bcm_int_metadata_field_entry_t_init(
    bcm_int_metadata_field_entry_t *field_entry)
{
    if (field_entry != NULL) {
        sal_memset(field_entry, 0, sizeof(bcm_int_metadata_field_entry_t));
    }
}

/*
 * Function:
 *      bcm_int_metadata_field_select_info_t
 * Purpose:
 *      Initialize the bcm_int_metadata_field_select_info_t struct.
 * Parameters:
 *      field_select_info  - Metadata field select information
 */
void bcm_int_metadata_field_select_info_t_init(
    bcm_int_metadata_field_select_info_t *field_select_info)
{
    if (field_select_info != NULL) {
        sal_memset(field_select_info, 0,
                   sizeof(bcm_int_metadata_field_select_info_t));
    }
}
#else /* INCLUDE_INT */
typedef int _int_turnaround_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_INT */
