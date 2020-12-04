/*! \file xflow_macsec_int.h
 *
 * Xflow_macsec internal management header file.
 * This file contains the management for XFLOW_MACSEC module.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMINT_LTSW_XFLOW_MACSEC_INT_H
#define BCMINT_LTSW_XFLOW_MACSEC_INT_H

#include <sal/sal_mutex.h>
#include <ltsw/xflow_macsec_common.h>

extern ltsw_xflow_macsec_cfg_t ltsw_xflow_macsec_info[BCM_MAX_NUM_UNITS];

/* Xflow_macsec info. */
#define XFLOW_MACSEC_INFO(unit) \
    (&ltsw_xflow_macsec_info[unit])

/* Xflow_macsec SC encrypt bitmap info. */
#define XFLOW_MACSEC_SC_ENCRYPT_BMP_INFO(unit) \
    (XFLOW_MACSEC_INFO(unit)->sc_encrypt_bmp)

/* Xflow_macsec policy bitmap info. */
#define XFLOW_MACSEC_POLICY_BMP_INFO(unit) \
    (XFLOW_MACSEC_INFO(unit)->policy_bmp)

/* Check if xflow_macsec module is initialized */
#define XFLOW_MACSEC_INIT_CHECK(u)                                \
    do {                                                \
        ltsw_xflow_macsec_cfg_t *xflow_macsec_cfg = XFLOW_MACSEC_INFO(u);               \
        if (xflow_macsec_cfg->inited == false) {                     \
            SHR_IF_ERR_VERBOSE_EXIT(SHR_E_INIT);        \
        }                                               \
    } while(0)

/* xflow_macsec Lock */
#define XFLOW_MACSEC_LOCK(u) \
    sal_mutex_take(XFLOW_MACSEC_INFO(u)->mutex, SAL_MUTEX_FOREVER)

#define XFLOW_MACSEC_UNLOCK(u) \
    sal_mutex_give(XFLOW_MACSEC_INFO(u)->mutex);

/*!
 *  * \brief HA subcomponent id for XFLOW_MACSEC module.
 */

/*! XFLOW_MACSEC resource sc encrypt bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_ENCRYPT_BITMAP       0

/*! XFLOW_MACSEC resource sc decrypt bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_DECRYPT_BITMAP       1

/*! XFLOW_MACSEC resource sc decrypt LT index bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_DECRYPT_LT_BITMAP    2

/*! XFLOW_MACSEC resource policy bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_POLICY_BITMAP           3

/*! XFLOW_MACSEC SC TCAM map structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SC_TCAM_MAP             4

/*! XFLOW_MACSEC resource decrypt flow bitmap structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_FLOW_BITMAP             5

/*! XFLOW_MACSEC SP TCAM map structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_FLOW_TCAM_MAP           6

/*! XFLOW_MACSEC SVTAG_CPU_FLEX_MAP TCAM map structure. */
#define BCMINT_XFLOW_MACSEC_SUB_COMP_ID_SVTAG_CPU_FLEX_MAP      7

#endif /* BCMINT_LTSW_XFLOW_MACSEC_INT_H */
