/*! \file bcmsec_ha_internal.h
 *
 * Data structures stored in HA area for BCMSEC component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_HA_INTERNAL_H
#define BCMSEC_HA_INTERNAL_H

/*! Reserved BCMSEC HA block Ids. */
typedef enum bcmsec_ha_blk_id_s {
    /*! HA Block id for bcmsec device info. */
    BCMSEC_HA_DEV_INFO              = 1,

    /*! HA Block id for physical port info. */
    BCMSEC_HA_PHYSICAL_PORT_INFO    = 2,

    /*! HA Block id for logical port info. */
    BCMSEC_HA_LOGICAL_PORT_INFO     = 3,

    /*! Always last. Not a usable value. */
    BCMSEC_HA_BLK_ID_COUNT,
} bcmsec_ha_blk_id_t;

#endif /* BCMSEC_HA_INTERNAL_H */

