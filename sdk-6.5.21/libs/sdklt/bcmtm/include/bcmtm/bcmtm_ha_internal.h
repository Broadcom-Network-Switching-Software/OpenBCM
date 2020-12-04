/*! \file bcmtm_ha_internal.h
 *
 * Data structures stored in HA area for BCMTM component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMTM_HA_INTERNAL_H
#define BCMTM_HA_INTERNAL_H

/*! Reserved BCMTM HA block Ids. */
typedef enum bcmtm_ha_blk_id_s {
    /*! HA Block id for bcmtm driver info. */
    BCMTM_HA_DEV_DRV_INFO = 1,

    /*! HA Block id for bcmtm threshold info. */
    BCMTM_HA_DEV_THD_INFO = 2,

    /*! HA Block id for bcmtm pfc deadlock info. */
    BCMTM_HA_DEV_PFC_DEADLOCK_INFO = 3,

    /*! HA Block id for bcmtm MC Dev info. */
    BCMTM_HA_DEV_MC_DEV_INFO = 4,

    /*! HA Block id for bcmtm MC Repl List info. */
    BCMTM_HA_DEV_MC_REPL_LIST_INFO = 5,

    /*! HA Block id for bcmtm OBM port usage configuration. */
    BCMTM_HA_DEV_OBM_USAGE_PBMP_INFO = 6,

    /*! HA Block id for bcmtm EBST Monitor info. */
    BCMTM_HA_DEV_EBST_DEV_INFO = 7,

    /*! HA Block id for bcmtm OBM threshold info. */
    BCMTM_HA_DEV_OBM_THD_INFO = 8,

    /*! HA Block id for bcmtm LPORT Map info. */
    BCMTM_HA_LPORT_DEV_INFO = 9,

    /*! HA Block id for bcmtm PPORT Map info. */
    BCMTM_HA_PPORT_DEV_INFO = 10,

    /*! HA Block id for bcmtm MPORT Map info. */
    BCMTM_HA_MPORT_DEV_INFO = 11,

    /*! Always last. Not a usable value. */
    BCMTM_HA_BLK_ID_COUNT,
} bcmtm_ha_blk_id_t;

#endif /* BCMTM_HA_INTERNAL_H */

