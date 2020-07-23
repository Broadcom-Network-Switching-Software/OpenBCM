
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    mirror.c
 * Purpose: Common Mirror API
 */

#include <soc/defs.h>

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/mirror.h>

/*
 * Function:
 *      bcm_mirror_int_probe_header_t_init
 * Purpose:
 *      Initialize a mirror INT probe header structure.
 * Parameters:
 *      updates - pointer to the bcm_mirror_int_probe_header_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_int_probe_header_t_init(bcm_mirror_int_probe_header_t *header)
{
    if (NULL != header) {
        sal_memset(header, 0, sizeof(bcm_mirror_int_probe_header_t));
    }
    return;
}

/*
 * Function:
 *      bcm_mirror_destination_t_init
 * Purpose:
 *      Initialize a mirror destination structure
 * Parameters:
 *      mirror_dest - pointer to the bcm_mirror_destination_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_destination_t_init(bcm_mirror_destination_t *mirror_dest)
{
    if (NULL != mirror_dest) {
        sal_memset (mirror_dest, 0, sizeof(bcm_mirror_destination_t));
        mirror_dest->stat_id2 = -1;
        (void)bcm_mirror_int_probe_header_t_init(&mirror_dest->int_probe_header);
    }
    return;
}

/*
 * Function:
 *      bcm_mirror_port_info_t_init
 * Purpose:
 *      Initialize a mirror port info structure
 * Parameters:
 *      mirror_info - pointer to the bcm_mirror_port_info_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_port_info_t_init(bcm_mirror_port_info_t *info)
{
    if (NULL != info) {
        sal_memset (info, 0, sizeof(bcm_mirror_port_info_t));
    }
    return;
}

/*
 * Function:
 *      bcm_mirror_options_t_init
 * Purpose:
 *      Initialize a mirror options  structure
 * Parameters:
 *      options - pointer to the bcm_mirror_options_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_options_t_init(bcm_mirror_options_t *options)
{
    if (NULL != options) {
        sal_memset (options, 0, sizeof(bcm_mirror_options_t));
        /* Enable mirroring by default*/
        options->forward_strength = 1;
        options->copy_strength = 1 ;
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_pkt_header_updates_t_init
 * Purpose:
 *      Initialize a mirror pkt header updates  structure
 * Parameters:
 *      updates - pointer to the bcm_mirror_pkt_header_updates_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_pkt_header_updates_t_init(bcm_mirror_pkt_header_updates_t  *updates)
{
    if (NULL != updates) {
        sal_memset (updates, 0, sizeof(bcm_mirror_pkt_header_updates_t));
    }
    return;
}

/*
 * Function:
 *      bcm_mirror_header_info_t_init
 * Purpose:
 *      Initialize a mirror header information structure
 * Parameters:
 *      updates - pointer to the bcm_mirror_header_info_t structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_header_info_t_init(bcm_mirror_header_info_t *mirror_header_info)
{
    if (NULL != mirror_header_info) {
        sal_memset (mirror_header_info, 0, sizeof(bcm_mirror_header_info_t));
    }
    return;
}

/*
 * Function:
 *      bcm_mirror_payload_zero_info_t_init
 * Purpose:
 *      Initialize a payload zero info structure
 * Parameters:
 *      info - Pointer to the bcm_mirror_payload_zero_info_t structure
 * Returns:
 *      NONE
 */
void
bcm_mirror_payload_zero_info_t_init(bcm_mirror_payload_zero_info_t *info)
{
    if (NULL != info) {
        sal_memset(info, 0, sizeof(bcm_mirror_payload_zero_info_t));
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_payload_zero_offsets_t_init
 * Purpose:
 *      Initialize a payload zero offset structure
 * Parameters:
 *      info - Pointer to the bcm_mirror_payload_zero_offsets_t structure
 * Returns:
 *      NONE
 */
void
bcm_mirror_payload_zero_offsets_t_init(bcm_mirror_payload_zero_offsets_t *offset)
{
    if (NULL != offset) {
        offset->l2_offset = -1;
        offset->l3_offset = -1;
        offset->udp_offset = -1;
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_stat_object_t_init
 * Purpose:
 *      Initialize mirror statistics object structure.
 * Parameters:
 *      object - Pointer to the mirror statistics object structure
 * Returns:
 *      NONE
 */
void
bcm_mirror_stat_object_t_init(bcm_mirror_stat_object_t *object)
{
    if (NULL != object) {
        sal_memset(object, 0, sizeof(bcm_mirror_stat_object_t));
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_source_t_init
 * Purpose:
 *      Initialize a mirror source structure
 * Parameters:
 *      source - Pointer to the mirror source structure
 * Returns:
 *      NONE
 */
void
bcm_mirror_source_t_init(bcm_mirror_source_t *source)
{
    if (NULL != source) {
        sal_memset(source, 0, sizeof(bcm_mirror_source_t));
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_sample_profile_t_init
 * Purpose:
 *      Initialize a sample profile structure.
 * Parameters:
 *      profile - Pointer to the sample profile structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_sample_profile_t_init(bcm_mirror_sample_profile_t *profile)
{
    if (NULL != profile) {
        sal_memset(profile, 0, sizeof(bcm_mirror_sample_profile_t));
    }

    return;
}


/*
 * Function:
 *      bcm_mirror_ingress_mod_event_profile_t_init
 * Purpose:
 *      Initialize an ingress mirror-on-drop profile structure.
 * Parameters:
 *      profile - Pointer to the ingress mirror-on-drop event profile structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_ingress_mod_event_profile_t_init(
    bcm_mirror_ingress_mod_event_profile_t *profile)
{
    if (NULL != profile) {
        sal_memset(profile, 0, sizeof(bcm_mirror_ingress_mod_event_profile_t));
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_global_truncate_t_init
 * Purpose:
 *      Initialize a global truncate structure.
 * Parameters:
 *      global_truncate - Pointer to global truncate structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_global_truncate_t_init(
    bcm_mirror_global_truncate_t *global_truncate)
{
    if (NULL != global_truncate) {
        sal_memset(global_truncate, 0, sizeof(bcm_mirror_global_truncate_t));
    }

    return;
}

/*
 * Function:
 *      bcm_mirror_truncate_length_profile_t_init
 * Purpose:
 *      Initialize a truncate length profile structure.
 * Parameters:
 *      profile - Pointer to truncate length profile structure.
 * Returns:
 *      NONE
 */
void
bcm_mirror_truncate_length_profile_t_init(
    bcm_mirror_truncate_length_profile_t *profile)
{
    if (NULL != profile) {
        sal_memset(profile, 0, sizeof(bcm_mirror_truncate_length_profile_t));
        /* The original design of the API is default to TruncateAdjustment */
        profile->mode = bcmMirrorTruncateAdjustment;
    }

    return;
}

