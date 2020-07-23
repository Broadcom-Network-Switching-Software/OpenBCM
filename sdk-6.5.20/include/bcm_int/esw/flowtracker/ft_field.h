/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_field.h
 * Purpose:     Flowtracker module specific routines in flowtracker
 *              stage in FP.
 */

#ifndef _BCM_INT_FT_FIELD_H
#define _BCM_INT_FT_FIELD_H

#include <soc/defs.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <bcm/flowtracker.h>

/*
 * Typedef:
 *   ALU Data flags.
 * Purpose:
 */
typedef enum _bcm_field_ft_alu_data_e {
    _bcmFieldFtAluDataTcpFlagsFIN,

    _bcmFieldFtAluDataTcpFlagsSYN,

    _bcmFieldFtAluDataTcpFlagsRST,

    _bcmFieldFtAluDataTcpFlagsPSH,

    _bcmFieldFtAluDataTcpFlagsACK,

    _bcmFieldFtAluDataTcpFlagsURG,

    _bcmFieldFtAluDataTcpFlagsECE,

    _bcmFieldFtAluDataTcpFlagsCWR,

    _bcmFieldFtAluDataCount
} _bcm_field_ft_alu_data_t;

#define _BCM_FIELD_FT_ALU_DATA_STRINGS \
{ \
    "TcpFlagsFIN", \
    "TcpFlagsSYN", \
    "TcpFlagsRST", \
    "TcpFlagsPSH", \
    "TcpFlagsACK", \
    "TcpFlagsURG", \
    "TcpFlagsECE", \
    "TcpFLagsCWR", \
    "AluDataCount", \
}

/* Extract session key and session data */
extern int
_bcm_field_ft_group_tracking_info_process(
        int unit,
        bcm_flowtracker_group_t id,
        uint8 save_output);

/* Clear FT group for extraction info */
extern int
_bcm_field_ft_group_extraction_clear(
        int unit,
        bcm_flowtracker_group_t id);

/* Setup Group Control to FTFP Entries */
extern int
_bcm_field_ft_group_control_set(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_flowtracker_group_control_type_t control_type,
        int param);

/* Mark FT Group in FTFP Entries as Stale */
extern int
_bcm_field_ft_group_invalidate(int unit);

/* Get min and max values supported for tracking params */
extern int
_bcm_field_ft_tracking_param_range_get(
        int unit,
        bcm_flowtracker_tracking_param_type_t param,
        uint16 custom_id,
        int *num_bits,
        uint32 *min_value,
        uint32 *max_value);

extern int
_bcm_field_ft_tracking_param_type_get(
        int unit,
        bcm_flowtracker_tracking_param_type_t param,
        uint16 custom_id, uint32 flags, uint16 *ft_type_bmp);

/* Get alu data related config. */
extern int
_bcm_field_ft_alu_data_config_get(
        int unit,
        bcm_flowtracker_group_t id,
        _bcm_field_ft_alu_data_t alu_data,
        int *offset,
        int *width);

/* Install Session Profiles */
extern int
_bcm_field_ft_session_profiles_install(
        int unit,
        bcm_flowtracker_group_t id);

/* Un-install Session Profiles */
extern int
_bcm_field_ft_session_profiles_uninstall(
        int unit,
        bcm_flowtracker_group_t id);

/* Recover Session Profiles */
extern int
_bcm_field_ft_session_profiles_recover(
        int unit,
        bcm_flowtracker_group_t id);

/* Check if UDF tracking param supported */
extern int
_bcm_field_ft_udf_tracking_param_supported(
        int unit,
        bcm_flowtracker_tracking_param_type_t param,
        uint16 *width);

/* Show tracking param and stages supported */
extern int
_bcm_field_ft_tracking_param_show(int unit);

#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_FIELD_H */
