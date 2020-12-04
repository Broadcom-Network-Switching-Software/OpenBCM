/*! \file telemetry_int.h
 *
 * Telemetry header file.
 * This file contains the management support of Telemetry module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifndef BCMINT_LTSW_TELEMETRY_INT_H
#define BCMINT_LTSW_TELEMETRY_INT_H

#include <sal/sal_mutex.h>
#include <shr/shr_bitop.h>
#include <bcm/telemetry.h>
#include <bcm_int/ltsw/generated/telemetry_ha.h>

/* Telemetry module invalid value definition */
#define _BCM_TELEMETRY_INVALID_VALUE -1

/* Maximum number of telemetry objects */
#define _BCM_TELEMETRY_MAX_OBJECTS 256

/* Maximum number of telemetry instances */
#define _BCM_TELEMETRY_MAX_INSTANCES 1

typedef enum _telemetry_ha_sub_comp_id_e {
    TELEMETRY_SUB_COMP_ID_HA_INFO       = 0
}telemetry_ha_sub_comp_id_t;

/* Telemetry database. */
typedef struct bcmi_telemetry_info_s {
    /* Telemetry module initialized flag. */
    int initialized;

    /* Telemetry module lock. */
    sal_mutex_t lock;

    /* Telemetry HA information. */
    bcmi_telemetry_ha_info_t *ha_info;

} bcmi_telemetry_info_t;


/* Telemetry book keeping Info Structure */
typedef struct bcm_telemetry_bk_info_s {

    /* Bitmap of configured object ids. */
    SHR_BITDCL *object_id_bmp;

    /* Bitmap of configured instance indexes. */
    SHR_BITDCL *instance_idx_bmp;

    /* Instance ID to internal index mapping. */
    int *instance_id_to_idx_map;

    /* Telemetry config object count */
    int config_count;

    /* List of telemetry config objects */
    bcm_telemetry_config_t *telemetry_config;

}bcm_telemetry_bk_info_t;

#endif /* BCMINT_LTSW_TELEMETRY_INT_H */
