/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        ft_udf.h
 * Purpose:     Function declarations for flowtracker udf support.
 */

#ifndef _BCM_INT_FT_UDF_H_
#define _BCM_INT_FT_UDF_H_

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)
#include <shared/bsl.h>
#include <soc/drv.h>
#include <bcm/error.h>
#include <soc/debug.h>
#include <soc/mem.h>
#include <soc/format.h>
#include <bcm/flowtracker.h>

/* Structure to maintain UDF to tracking param mapping information */
typedef struct bcmi_ft_udf_tracking_param_info_s {

    /* Tracking Param Type */
    bcm_flowtracker_tracking_param_type_t param;

    /* Custom Identifier */
    uint16 custom_id;

    /* UDF Object Identifier */
    bcm_udf_id_t udf_id;

    /* Relative offset in the UDF object */
    uint16 offset;

    /* Requested Width in the UDF object */
    uint16 width;

    /* Next udf param info */
    struct bcmi_ft_udf_tracking_param_info_s *next;

} bcmi_ft_udf_tracking_param_info_t;

/* Structure to maintain database of UDF to tracking param maps */
typedef struct bcmi_ft_udf_tracking_param_info_db_s {

    /* Number of valid mapping */
    uint16 count;

    /* UDF Tracking param info head */
    bcmi_ft_udf_tracking_param_info_t *head;

} bcmi_ft_udf_tracking_param_info_db_t;

extern int
bcmi_ft_udf_tracking_param_set(int unit,
        uint32 flags,
        bcm_flowtracker_tracking_param_type_t param,
        uint16 custom_id,
        bcm_flowtracker_udf_info_t *udf_info);

extern int
bcmi_ft_udf_tracking_param_get(int unit,
        uint32 flags,
        bcm_flowtracker_tracking_param_type_t param,
        uint16 custom_id,
        bcm_flowtracker_udf_info_t *udf_info);

extern int
bcmi_ft_udf_tracking_param_info_init(int unit);

extern int
bcmi_ft_udf_tracking_param_info_cleanup(int unit);

extern int
bcmi_ft_udf_tracking_param_info_alloc_size(int unit,
        int *size);

extern int
bcmi_ft_udf_tracking_param_info_sync(int unit,
        uint8 **p_ft_scache_ptr);

extern int
bcmi_ft_udf_tracking_param_info_recover(int unit,
        uint8 **p_ft_scache_ptr);

extern int
bcmi_ft_udf_tracking_param_info_show(int unit);

#endif /* BCM_FLOWTRACKER_SUPPORT */
#endif /* _BCM_INT_FT_UDF_H_ */
